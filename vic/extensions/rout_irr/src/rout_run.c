/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vic_def.h>
#include <rout.h>
#include <assert.h>
#include <vic_driver_image.h>
#include <math.h>


void rout_run(dmy_struct* current_dmy){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern double ***out_data;       
    extern all_vars_struct *all_vars;
    extern veg_con_struct **veg_con;
    extern soil_con_struct *soil_con;
    extern option_struct options;
    extern rout_options_struct rout_options;
    
    if(!rout_options.routing){
        return;
    }
    
    size_t iRes;
    if(rout_options.reservoirs){
        for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
            calculate_reservoir_values(&rout.reservoirs[iRes],current_dmy);
            reset_reservoir_run(&rout.reservoirs[iRes],current_dmy);
            reset_reservoir_demand(&rout.reservoirs[iRes]);
        }
    }
    
    //####################
    //start routing
    //####################
    
    size_t iRank;
    for(iRank=0;iRank<global_domain.ncells_active;iRank++){
        rout_cell *current_cell = rout.sorted_cells[iRank];
                
        double runoff = 0.0; //m^3/s
        double inflow = 0.0; //m^3/s
        
        shift_outflow_array(current_cell);
               
        runoff = (out_data[current_cell->id][OUT_RUNOFF][0]+out_data[current_cell->id][OUT_BASEFLOW][0]) 
                * local_domain.locations[current_cell->id].area / MM_PER_M / global_param.dt;
        
        size_t iCell;
        for(iCell=0;iCell<current_cell->nr_upstream;iCell++){
            inflow += current_cell->upstream[iCell]->outflow[0];
        }
        
        if(rout_options.irrigation){
            
            //####################
            //start local irrigation
            //####################
            if(current_cell->irrigate){
                size_t iVeg = current_cell->irr_veg_id;
                double available_river_water = inflow * global_param.dt / local_domain.locations[current_cell->id].area * MM_PER_M / veg_con[current_cell->id][iVeg].Cv;
                double available_runoff_water = runoff * global_param.dt /local_domain.locations[current_cell->id].area * MM_PER_M / veg_con[current_cell->id][iVeg].Cv;

                double moisture_content=0.0; //mm
                double irrigation_demand=0.0; //mm
                double added_river_water=0.0; //mm
                double added_runoff_water=0.0; //mm

                //get moisture content and irrigation demand
                moisture_content = get_moisture_content(current_cell);
                if(moisture_content <= soil_con[current_cell->id].Wcr[0]){
                    irrigation_demand = ((soil_con[current_cell->id].Wcr[0] / 0.7) - moisture_content);
                }

                //do runoff irrigation
                if(available_runoff_water > irrigation_demand){
                    added_runoff_water = irrigation_demand;
                }else{
                    added_runoff_water = available_runoff_water;
                }
                irrigation_demand -= added_runoff_water;
                available_runoff_water -= added_runoff_water;

                //do river irrigation
                if(available_river_water > irrigation_demand){
                    added_river_water = irrigation_demand;
                }else{
                    added_river_water = available_river_water;
                }
                irrigation_demand -= added_river_water;
                available_river_water -= added_river_water;

                //communicate leftover demand with reservoirs
                distribute_demand_among_reservoirs(current_cell,irrigation_demand);

                out_data[current_cell->id][OUT_IRR_DEMAND][0] = irrigation_demand * veg_con[current_cell->id][iVeg].Cv;

                //add water to soil moisture and reduce runoff and inflow
                size_t iBand;
                for(iBand=0;iBand<options.SNOW_BAND;iBand++){
                   all_vars[current_cell->id].cell[iVeg][iBand].layer[0].moist = moisture_content + added_river_water + added_runoff_water;
                }
                runoff = available_runoff_water / global_param.dt * local_domain.locations[current_cell->id].area / MM_PER_M * veg_con[current_cell->id][iVeg].Cv;
                inflow = available_river_water / global_param.dt * local_domain.locations[current_cell->id].area / MM_PER_M * veg_con[current_cell->id][iVeg].Cv;

                out_data[current_cell->id][OUT_LOCAL_IRR][0] = added_river_water + added_runoff_water;
                out_data[current_cell->id][OUT_IRR][0] += out_data[current_cell->id][OUT_LOCAL_IRR][0];
            }
            
            //####################
            //end local irrigation
            //####################
        }
        
        if(current_cell->reservoir==NULL || current_cell->reservoir->run==false){
            
            //normal routing
            current_cell->outflow[0] += runoff;
            size_t t;
            for(t=0;t<rout_options.max_days_uh * global_param.model_steps_per_day;t++){
                current_cell->outflow[t]+=current_cell->uh[t] * inflow;
            }
        }else{
            
            //no routing but rather add to reservoir storage
            current_cell->reservoir->current_storage += (inflow + runoff) * global_param.dt;
            current_cell->reservoir->current_inflow += (inflow + runoff) * global_param.dt / nr_days_per_month(current_dmy->month,current_dmy->year) / global_param.model_steps_per_day;
        }
        
        out_data[current_cell->id][OUT_DISCHARGE][0] += current_cell->outflow[0];
        
        //####################
        //end routing
        //####################
        
    }
    
    if(rout_options.reservoirs){
        
        //####################
        //start reservoir operation
        //####################
        for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
            reservoir_unit* current_reservoir = &rout.reservoirs[iRes];

            if(!current_reservoir->run){
                continue;
            }

            double target_release =0.0; //m3
            double environmental_flow = 0.0; //m3
            double total_current_demand = 0.0; //m3
            double total_added_reservoir_water=0.0;//m3
            double overflow = 0.0; //m3

            target_release = do_reservoir_operation(current_reservoir,current_dmy);
            double old_target_release = target_release;    
            if(target_release > current_reservoir->current_storage){
                target_release = current_reservoir->current_storage;
            }
            environmental_flow = PERC_ENV_INFLOW * target_release;

            if(current_reservoir->function==RES_IRR_FUNCTION){

                total_current_demand = get_reservoir_demand(current_reservoir);

                if(total_current_demand>0){

                    rout_cell* service_cell;
                    size_t iCell;
                    for(iCell=0;iCell<current_reservoir->nr_serviced_cells;iCell++){
                        service_cell = current_reservoir->serviced_cells[iCell];
                        size_t iVeg = service_cell->irr_veg_id;

                        double added_reservoir_water=0.0; //mm
                        double moisture_content=0.0; //mm

                        moisture_content = get_moisture_content(service_cell);
                        added_reservoir_water = do_reservoir_irrigation(target_release - environmental_flow,total_current_demand,current_reservoir->cell_demand[iCell]);
                        if(added_reservoir_water>0.0){

                            size_t iBand;
                            for(iBand=0;iBand<options.SNOW_BAND;iBand++){
                               all_vars[service_cell->id].cell[iVeg][iBand].layer[0].moist = moisture_content + added_reservoir_water;
                            }                        

                            total_added_reservoir_water += added_reservoir_water * veg_con[service_cell->id][iVeg].Cv * local_domain.locations[service_cell->id].area / MM_PER_M;
                            out_data[service_cell->id][OUT_RES_IRR][0] = added_reservoir_water;
                            out_data[service_cell->id][OUT_IRR][0] += out_data[service_cell->id][OUT_RES_IRR][0];
                        }
                    }
                }
            }

            current_reservoir->added_water += total_added_reservoir_water;
            current_reservoir->current_demand = (current_reservoir->added_water + total_current_demand) / nr_days_per_month(current_dmy->month,current_dmy->year) / global_param.model_steps_per_day;

            overflow = do_overflow(current_reservoir, target_release);

            current_reservoir->current_storage -= target_release + overflow;
            current_reservoir->cell->outflow[1] += ((target_release-total_added_reservoir_water) + overflow) / global_param.dt;

            if(rout_options.debug_mode){
                if(current_dmy->year > global_param.startyear){
                    log_info(" ");
                    log_info("-----------------------routing debug -----------------------------------");
                    log_info("res %zu %s (cap %.2f o_cap %.2f c_cap %.2f), has irr %.2f, rel %.2f (%.2f over %.2f env); tar_rel %.2f [m3]",current_reservoir->id,
                            current_reservoir->name,current_reservoir->storage_capacity/1000,(current_reservoir->current_storage + target_release)/1000,current_reservoir->current_storage/1000,
                            total_added_reservoir_water/1000, current_reservoir->cell->outflow[1] * global_param.dt / 1000,overflow/1000,
                            environmental_flow/1000, old_target_release/1000);
                    if(current_reservoir->function==RES_IRR_FUNCTION){
                        log_info("demand was %.2f over %zu cells (%.2f per cell) [m3]", total_current_demand/1000,current_reservoir->nr_serviced_cells,total_current_demand/1000/current_reservoir->nr_serviced_cells);
                    }
                    log_info("------------------------------------------------------------------------");
                    log_info(" ");
                }
            }

            out_data[current_reservoir->cell->id][OUT_RES_STORE][0]=(current_reservoir->current_storage / current_reservoir->storage_capacity);
        }
        
        //####################
        //end reservoir operation
        //#################### 
    }     
}

void calculate_reservoir_values(reservoir_unit* current_reservoir, dmy_struct* current_dmy){
    extern global_param_struct global_param;
    extern rout_options_struct rout_options;
    
    if(current_reservoir->run == false){
        return;
    }
    
    if(current_dmy->dayseconds==0){
        if(current_dmy->day==current_reservoir->start_operation.day){

            //an operational month has passed
            int current_month = current_dmy->month-2;            
            int current_year = (current_dmy->year - global_param.startyear);
            
            if(current_month<0){
                current_month += MONTHS_PER_YEAR;
                current_year--;
            }
            
            current_year = current_year % RES_CALC_YEARS_MEAN;
            
            current_reservoir->inflow[current_year][current_month] = current_reservoir->current_inflow;
            current_reservoir->demand[current_year][current_month] = current_reservoir->current_demand;

            current_reservoir->current_inflow=0.0;
            current_reservoir->current_demand=0.0;
            current_reservoir->added_water=0.0;
        }

        if(current_dmy->day==current_reservoir->start_operation.day && current_dmy->month == current_reservoir->start_operation.month){
            
            //an operational year has passed
            double mean_monthly_inflow[MONTHS_PER_YEAR];
            double mean_monthly_demand[MONTHS_PER_YEAR];
            double mean_annual_inflow = 0.0;
            double mean_annual_demand = 0.0;

            get_all_mean_reservoir_values(current_reservoir,current_dmy,mean_monthly_inflow,mean_monthly_demand,&mean_annual_inflow,&mean_annual_demand);
            
            size_t t;
            size_t count=0;
            size_t longest=0;
            size_t month=0;            
            for(t=0;t<(2 * MONTHS_PER_YEAR);t++){
                size_t i = t % MONTHS_PER_YEAR;
                
                if(mean_monthly_inflow[i]>mean_annual_inflow){
                    count++;
                    
                    if(count>longest){
                        longest=count;
                        month=i;
                        
                    }
                }else{
                    count=0;
                }
            }
            
            dmy_struct operational_year;
            operational_year.day=1;
            operational_year.month=month+2;
            operational_year.dayseconds=0;
            operational_year.year=current_dmy->year;
            
            if(operational_year.month>MONTHS_PER_YEAR){
                operational_year.month-=MONTHS_PER_YEAR;
            }
            
            current_reservoir->start_operation = operational_year;         
            current_reservoir->storage_start_operation = current_reservoir->current_storage;
            
            if(rout_options.debug_mode){
                log_info(" ");
                log_info("-----------------------routing debug -----------------------------------");
                log_info("Reservoir %zu %s:",current_reservoir->id,current_reservoir->name);
                log_info("Start operational year set at month %d",operational_year.month);
                
                double mean_annual_inflow = 0.0;
                double mean_annual_demand = 0.0;
                
                get_all_mean_reservoir_values(current_reservoir,current_dmy,mean_monthly_inflow,mean_monthly_demand,&mean_annual_inflow,&mean_annual_demand);
                
                log_info("Mean monthly inflow [hm3] %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f;"
                        ,mean_monthly_inflow[0]/1000,mean_monthly_inflow[1]/1000,mean_monthly_inflow[2]/1000
                        ,mean_monthly_inflow[3]/1000,mean_monthly_inflow[4]/1000,mean_monthly_inflow[5]/1000
                        ,mean_monthly_inflow[6]/1000,mean_monthly_inflow[7]/1000,mean_monthly_inflow[8]/1000
                        ,mean_monthly_inflow[9]/1000,mean_monthly_inflow[10]/1000,mean_monthly_inflow[11]/1000);
                log_info("Mean monthly demand [hm3] %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f; %.2f;"
                        ,mean_monthly_demand[0]/1000,mean_monthly_demand[1]/1000,mean_monthly_demand[2]/1000
                        ,mean_monthly_demand[3]/1000,mean_monthly_demand[4]/1000,mean_monthly_demand[5]/1000
                        ,mean_monthly_demand[6]/1000,mean_monthly_demand[7]/1000,mean_monthly_demand[8]/1000
                        ,mean_monthly_demand[9]/1000,mean_monthly_demand[10]/1000,mean_monthly_demand[11]/1000);
                log_info("Mean annual inflow [hm3] %.2f; mean annual demand [hm3] %.2f",mean_annual_inflow/1000,mean_annual_demand/1000);
                log_info("------------------------------------------------------------------------");
                log_info(" ");
            }
        }
    }
}

void reset_reservoir_run(reservoir_unit* current_reservoir, dmy_struct* current_dmy){
    current_reservoir->run=false;

    if(current_dmy->year>=current_reservoir->activation_year){
        current_reservoir->run=true;
    }
}

void reset_reservoir_demand(reservoir_unit* current_reservoir){
    
    size_t i;
    for(i=0;i<current_reservoir->nr_serviced_cells;i++){
        current_reservoir->cell_demand[i]=0.0;

    }
}

void shift_outflow_array(rout_cell* current_cell){
    extern global_param_struct global_param;
    extern rout_options_struct rout_options;
    
    size_t t;                
    for(t=0;t<(rout_options.max_days_uh * global_param.model_steps_per_day)-1;t++){
        *(current_cell->outflow + t) = *(current_cell->outflow + (t+1));
    }
    
    *(current_cell->outflow + t) = 0.0;
}

double get_moisture_content(rout_cell* current_cell){
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern soil_con_struct *soil_con;
    
    double moisture_ice = 0.0; //mm
    double moisture_content; //mm
    
    size_t iVeg = current_cell->irr_veg_id;
    
    size_t s;
    for(s=0;s<options.SNOW_BAND;s++){
        moisture_content =  all_vars[current_cell->id].cell[iVeg][s].layer[0].moist * soil_con[current_cell->id].AreaFract[s];

        size_t f;
        for (f = 0; f < options.Nfrost; f++) {
            moisture_ice += all_vars[current_cell->id].cell[iVeg][s].layer[0].ice[f] * soil_con[current_cell->id].AreaFract[s] * soil_con[current_cell->id].frost_fract[f];
        }
        moisture_content -= moisture_ice;
    }
    
    return moisture_content;
}

void distribute_demand_among_reservoirs(rout_cell* current_cell, double irrigation_demand){
    
    if(irrigation_demand > 0 && current_cell->nr_servicing_reservoirs > 0){
        
        double total_capacity_per_cell=0.0;
        
        size_t r;
        for(r=0;r<current_cell->nr_servicing_reservoirs;r++){
            if(current_cell->servicing_reservoirs[r]->run==false){
                continue;
            }
            
            total_capacity_per_cell+=current_cell->servicing_reservoirs[r]->storage_capacity / current_cell->servicing_reservoirs[r]->nr_serviced_cells;
        }
        
        if(total_capacity_per_cell>0.0){
            
            for(r=0;r<current_cell->nr_servicing_reservoirs;r++){
                if(current_cell->servicing_reservoirs[r]->run==false){
                    continue;
                }

                size_t c;
                for(c=0;c<current_cell->servicing_reservoirs[r]->nr_serviced_cells;c++){

                    if(current_cell->servicing_reservoirs[r]->serviced_cells[c]->id == current_cell->id){
                        current_cell->servicing_reservoirs[r]->cell_demand[c]=irrigation_demand * ((current_cell->servicing_reservoirs[r]->storage_capacity / current_cell->servicing_reservoirs[r]->nr_serviced_cells) / total_capacity_per_cell);
                        break;
                    }
                }
            }
        }
    }
}

double get_reservoir_demand(reservoir_unit * current_reservoir){
    extern veg_con_struct **veg_con;
    extern domain_struct local_domain;
    
    double total_demand=0.0; //m3
    
    rout_cell* service_cell;
    size_t iCell;
    for(iCell=0;iCell<current_reservoir->nr_serviced_cells;iCell++){
        service_cell = current_reservoir->serviced_cells[iCell];
        size_t iVeg = service_cell->irr_veg_id;
        
        total_demand += current_reservoir->cell_demand[iCell] * veg_con[service_cell->id][iVeg].Cv * local_domain.locations[service_cell->id].area / MM_PER_M;
    }
    
    return total_demand;
}

double do_reservoir_operation(reservoir_unit* current_reservoir, dmy_struct* current_dmy){
    extern global_param_struct global_param;
    
    double target_release = 0.0; //m3
    
    if((current_dmy->year - global_param.startyear) < 1){
        return target_release;
    }
    
    double release_coefficient = (double)current_reservoir->storage_start_operation / (RES_PREF_STORAGE * (double)current_reservoir->storage_capacity);
    
    double mean_monthly_inflow_all[MONTHS_PER_YEAR];
    double mean_monthly_demand_all[MONTHS_PER_YEAR];
    double mean_annual_inflow = 0.0;
    double mean_annual_demand = 0.0;
    double c=0.0; //-
    
    get_all_mean_reservoir_values(current_reservoir,current_dmy,mean_monthly_inflow_all,mean_monthly_demand_all,&mean_annual_inflow,&mean_annual_demand);
    
    double mean_monthly_inflow = mean_monthly_inflow_all[current_dmy->month-1];
    double mean_monthly_demand = mean_monthly_demand_all[current_dmy->month-1];
        
    if(current_reservoir->function==RES_IRR_FUNCTION){
        
        if(mean_monthly_inflow > current_reservoir->storage_capacity && current_reservoir->current_storage == current_reservoir->storage_capacity){
            return current_reservoir->storage_capacity;
        }
        
        if(mean_annual_demand >= 0.5 * mean_annual_inflow){
                
            if(mean_annual_demand>0){
                target_release = (mean_monthly_inflow / 10) + 
                        (9/10) * mean_annual_inflow * 
                        (mean_monthly_demand / mean_annual_demand);
            }else{
                 target_release = (mean_monthly_inflow / 10);
            }
        }else{
            target_release = mean_annual_inflow + mean_monthly_demand - mean_annual_demand;
        }
    }else{
        target_release = mean_annual_inflow;
    }

    c = current_reservoir->storage_capacity / mean_annual_inflow;

    if(c >= 0.5){
        target_release = target_release * release_coefficient;
    }else{
        target_release = pow((c/0.5),2)*release_coefficient*target_release + (1-pow((c/0.5),2))* mean_monthly_inflow;
    }
    
    return target_release;
}

double do_reservoir_irrigation(double target_release, double total_demand, double demand){
    
    double added_reservoir_water = 0.0; //mm
    
    if(demand > 0){
        if(total_demand < target_release){
            added_reservoir_water =  demand;
        }else{
            added_reservoir_water =  demand * (target_release / total_demand);
        }
    }
    
    return added_reservoir_water;
}

double do_overflow(reservoir_unit* current_reservoir, double total_added_reservoir_water){
    
    double overflow = 0.0; //m3
    
    if((current_reservoir->current_storage - total_added_reservoir_water) > current_reservoir->storage_capacity){
        overflow = (current_reservoir->current_storage - total_added_reservoir_water) - current_reservoir->storage_capacity;
    }
    
    return overflow;
}

int is_leap_year(int year){
    if((year % 4 == 0) || ((year % 100 == 0) && (year % 400 == 0))){
        return 1;
    }
    return 0;
}

int nr_days_per_month(int month, int year){
    if(month == 2){
        return 28 + is_leap_year(year);
    }
    return 31 - (month-1) % 7 % 2;
}

void get_all_mean_reservoir_values(reservoir_unit* current_reservoir, dmy_struct* current_dmy, double mean_monthly_inflow[MONTHS_PER_YEAR], double mean_monthly_demand[MONTHS_PER_YEAR], double* mean_annual_inflow, double* mean_annual_demand){
    extern global_param_struct global_param;
    
    size_t years_done = current_dmy->year - global_param.startyear;
    int months_done = current_dmy->month - global_param.startmonth;
    
    if(months_done<0){
        months_done += MONTHS_PER_YEAR;
    }
    if(years_done>RES_CALC_YEARS_MEAN-1){
        years_done=RES_CALC_YEARS_MEAN-1;
        months_done=MONTHS_PER_YEAR;
    }

    int t;
    for(t=0;t<MONTHS_PER_YEAR;t++){
        mean_monthly_inflow[t]=0.0;
        mean_monthly_demand[t]=0.0;
    }
    
    size_t j;
    for(j=0;j<years_done;j++){
        for(t=0;t<MONTHS_PER_YEAR;t++){
            if(t<months_done){
                mean_monthly_inflow[t]+=current_reservoir->inflow[j][t] / (years_done+1);
                mean_monthly_demand[t]+=current_reservoir->demand[j][t] / (years_done+1);                            
            }else{
                mean_monthly_inflow[t]+=current_reservoir->inflow[j][t] / (years_done);
                mean_monthly_demand[t]+=current_reservoir->demand[j][t] / (years_done);   
            }

            *mean_annual_demand += current_reservoir->demand[j][t] /((years_done) * MONTHS_PER_YEAR);
            *mean_annual_inflow += current_reservoir->inflow[j][t] /((years_done) * MONTHS_PER_YEAR);
        }
    }

    for(t=0;t<months_done;t++){
        mean_monthly_inflow[t]+= current_reservoir->inflow[j][t] / (years_done+1);
        mean_monthly_demand[t]+= current_reservoir->demand[j][t] / (years_done+1);

        if(years_done==0){
            *mean_annual_demand += current_reservoir->demand[years_done][t] /(months_done);
            *mean_annual_inflow += current_reservoir->inflow[years_done][t] /(months_done);
        }
    }
}