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
    extern global_param_struct global_param;
    extern double ***out_data;       
    extern all_vars_struct *all_vars;
    extern veg_con_struct **veg_con;
    extern option_struct options;
        
    size_t iRes;
    for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
        //calculate_reservoir_values(&rout.reservoirs[iRes],current_dmy);
        //reset_reservoir_run(&rout.reservoirs[iRes],current_dmy);
        //reset_reservoir_demand(&rout.reservoirs[iRes]);
    }
    
    
    size_t iRank;
    for(iRank=0;iRank<global_domain.ncells_active;iRank++){
        rout_cell *current_cell = rout.sorted_cells[iRank];
                
        double runoff = 0.0; //m^3
        double inflow = 0.0; //m^3
        
        shift_outflow_array(current_cell);
               
        runoff = (out_data[current_cell->id][OUT_RUNOFF][0]+out_data[current_cell->id][OUT_BASEFLOW][0]) 
                * current_cell->location->area / MM_PER_M / global_param.dt;
        
        inflow=0.0;        
        size_t iCell;
        for(iCell=0;iCell<current_cell->nr_upstream;iCell++){
            inflow += current_cell->upstream[iCell]->outflow[0];
        }
        
        //####################
        //start local irrigation
        //####################
        
        if(current_cell->irrigate){
            size_t iVeg = current_cell->irr_veg_nr;
            double available_river_water = inflow * global_param.dt / current_cell->location->area * MM_PER_M / veg_con[current_cell->id][iVeg].Cv; //mm
            double available_runoff_water = runoff * global_param.dt / current_cell->location->area * MM_PER_M / veg_con[current_cell->id][iVeg].Cv; //mm
            
            double moisture_content=0.0; //mm
            double irrigation_demand=0.0; //mm
            double added_river_water=0.0; //mm
            double added_runoff_water=0.0; //mm

            moisture_content = get_moisture_content(current_cell);
            irrigation_demand = get_irrigation_demand(current_cell,moisture_content);
            
            added_runoff_water = do_runoff_irrigation(&irrigation_demand,available_runoff_water);
            added_river_water = do_river_irrigation(&irrigation_demand,available_river_water);
            distribute_demand_among_reservoirs(current_cell,irrigation_demand);
            
            out_data[current_cell->id][OUT_IRR_DEMAND][0] = irrigation_demand; //mm
            
            if(added_river_water + added_runoff_water > 0.0){
                size_t iBand;
                for(iBand=0;iBand<options.SNOW_BAND;iBand++){
                   all_vars[current_cell->id].cell[iVeg][iBand].layer[0].moist = moisture_content + added_river_water + added_runoff_water; //mm
                }

                inflow -= (added_river_water * current_cell->location->area * veg_con[current_cell->id][iVeg].Cv / global_param.dt / MM_PER_M);
                runoff -= (added_runoff_water * current_cell->location->area * veg_con[current_cell->id][iVeg].Cv / global_param.dt / MM_PER_M);
                
                out_data[current_cell->id][OUT_LOCAL_IRR][0] = added_river_water + added_runoff_water;
            }
        }
        
        //####################
        //end local irrigation
        //####################
        
        //####################
        //start routing
        //####################
                
        if(current_cell->reservoir==NULL || current_cell->reservoir->run==false){
            current_cell->outflow[0] += runoff;

            if(inflow > 0){
                    size_t t;
                for(t=0;t<UH_MAX_DAYS * global_param.model_steps_per_day;t++){
                    current_cell->outflow[t]+=current_cell->uh[t] * inflow;
                }
            }
        }else{
            current_cell->reservoir->current_storage += (inflow + runoff) * global_param.dt;
            current_cell->reservoir->current_mean_monthly_inflow += (inflow + runoff) * global_param.dt / global_param.model_steps_per_day / nr_days_per_month(current_dmy->month,current_dmy->year);
        }
        
        //####################
        //end routing
        //####################
        
        out_data[current_cell->id][OUT_DISCHARGE][0] += current_cell->outflow[0];
        
    }
        
    //####################
    //start reservoir operation
    //####################
   
    /*
    for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
        reservoir_unit* current_reservoir = &rout.reservoirs[iRes];
        
        if(!current_reservoir->run){
            continue;
        }
        
        double target_release =0.0; //m3
        double total_added_reservoir_water=0.0;//m3
        double overflow = 0.0; //m3
        
        target_release = do_reservoir_operation(current_reservoir);
        
        if(current_reservoir->function==RES_IRR_FUNCTION){
            
            double total_demand = 0.0; //m3
            
            total_demand = get_reservoir_demand(current_reservoir);
            current_reservoir->current_mean_monthly_demand += total_demand / nr_days_per_month(current_dmy->month,current_dmy->year) / global_param.model_steps_per_day;
            
            if(total_demand>0){
            
                rout_cell* service_cell;
                size_t iCell;
                for(iCell=0;iCell<current_reservoir->nr_serviced_cells;iCell++){
                    service_cell = current_reservoir->serviced_cells[iCell];
                    size_t iVeg = service_cell->irr_veg_nr;
                    
                    double added_reservoir_water=0.0; //mm
                    double moisture_content=0.0; //mm
                    
                    moisture_content = get_moisture_content(service_cell);
                    added_reservoir_water = do_reservoir_irrigation(target_release,total_demand,current_reservoir->demand[iCell]);

                    size_t iBand;
                    for(iBand=0;iBand<options.SNOW_BAND;iBand++){
                       all_vars[service_cell->id].cell[iVeg][iBand].layer[0].moist = moisture_content + added_reservoir_water;
                    }                        

                    total_added_reservoir_water += added_reservoir_water * veg_con[service_cell->id][iVeg].Cv * service_cell->location->area / MM_PER_M;

                    out_data[service_cell->id][OUT_RES_IRR][0] = added_reservoir_water;
                }
            }
        }
        
        overflow = do_overflow(current_reservoir, target_release);
        
        current_reservoir->current_storage -= target_release + overflow;
        
        if(current_reservoir->current_storage < 0){
            log_info("something wong");
            current_reservoir->current_storage=0.0;
        }
        
        current_reservoir->cell->outflow[1] += ((target_release-total_added_reservoir_water) + overflow) / global_param.dt;
                
        out_data[current_reservoir->cell->id][OUT_RES_STORE][0]=(current_reservoir->current_storage / current_reservoir->storage_capacity);
    }
    */
     
    //####################
    //end reservoir operation
    //####################
    
    size_t iCell;
    for(iCell=0;iCell<global_domain.ncells_active;iCell++){
        out_data[rout.cells[iCell].id][OUT_IRR][0]=out_data[rout.cells[iCell].id][OUT_LOCAL_IRR][0]+out_data[rout.cells[iCell].id][OUT_RES_IRR][0];
    }    
}

void calculate_reservoir_values(reservoir_unit* current_reservoir, dmy_struct* current_dmy){
    extern global_param_struct global_param;
    
    if(current_reservoir->run == false){
        return;
    }
    
    if(current_dmy->dayseconds==0){
        if(current_dmy->day==current_reservoir->start_operation.day){

            //an operational month has passed
            current_reservoir->mean_monthly_inflow[0][0] = current_reservoir->current_mean_monthly_inflow;
            current_reservoir->mean_monthly_demand[0][0] = current_reservoir->current_mean_monthly_demand;
            
            size_t t;                
            for(t=MONTHS_PER_YEAR;t>=1;t--){
                
                size_t y;
                for(y=0;y<RES_CALC_YEARS_MEAN;y++){
                
                    double bi = *(current_reservoir->mean_monthly_inflow[y] + (MONTHS_PER_YEAR-1));
                    double bd = *(current_reservoir->mean_monthly_demand[y] + (MONTHS_PER_YEAR-1));
                    
                    *(current_reservoir->mean_monthly_inflow[y] + t) = *(current_reservoir->mean_monthly_inflow[y] + (t-1));
                    *(current_reservoir->mean_monthly_demand[y] + t) = *(current_reservoir->mean_monthly_demand[y] + (t-1));
                    
                    *(current_reservoir->mean_monthly_inflow[y]) = bi;
                    *(current_reservoir->mean_monthly_demand[y]) = bd;
                }
            }
            
            current_reservoir->current_mean_annual_inflow += current_reservoir->current_mean_monthly_inflow / MONTHS_PER_YEAR;
            current_reservoir->current_mean_annual_demand += current_reservoir->current_mean_monthly_demand / MONTHS_PER_YEAR;

            current_reservoir->current_mean_monthly_inflow=0.0;
            current_reservoir->current_mean_monthly_demand=0.0;
        }

        if(current_dmy->day==current_reservoir->start_operation.day && current_dmy->month == current_reservoir->start_operation.month){
            
            //an operational year has passed
            current_reservoir->mean_annual_inflow[0] = current_reservoir->current_mean_annual_inflow;
            current_reservoir->mean_annual_demand[0] = current_reservoir->current_mean_annual_demand;
            current_reservoir->storage_start_operation = current_reservoir->current_storage;
            
            double bai = *(current_reservoir->mean_annual_inflow + (RES_CALC_YEARS_MEAN-1));
            double bad = *(current_reservoir->mean_annual_demand + (RES_CALC_YEARS_MEAN-1));
            double* bmi = *(current_reservoir->mean_monthly_inflow + (RES_CALC_YEARS_MEAN-1));
            double* bmd = *(current_reservoir->mean_monthly_demand + (RES_CALC_YEARS_MEAN-1));
            
            size_t t;                
            for(t=RES_CALC_YEARS_MEAN-1;t>=1;t--){
                *(current_reservoir->mean_annual_inflow + t) = *(current_reservoir->mean_annual_inflow + (t-1));
                *(current_reservoir->mean_annual_demand + t) = *(current_reservoir->mean_annual_demand + (t-1));
                
                *(current_reservoir->mean_monthly_inflow + t) = *(current_reservoir->mean_monthly_inflow + (t-1));
                *(current_reservoir->mean_monthly_demand + t) = *(current_reservoir->mean_monthly_demand + (t-1));
            }
            
            *(current_reservoir->mean_annual_inflow) = bai;
            *(current_reservoir->mean_annual_demand) = bad;
            *(current_reservoir->mean_monthly_inflow) = bmi;
            *(current_reservoir->mean_monthly_demand) = bmd;

            current_reservoir->current_mean_annual_inflow=0.0;
            current_reservoir->current_mean_annual_demand=0.0;
        }
    }
}

void reset_reservoir_run(reservoir_unit* current_reservoir, dmy_struct* current_dmy){
    extern global_param_struct global_param;
    
    current_reservoir->run=false;

    if(current_dmy->year>=current_reservoir->activation_year){
        current_reservoir->run=true;
    }
}

void reset_reservoir_demand(reservoir_unit* current_reservoir){
    
    size_t i;
    for(i=0;i<current_reservoir->nr_serviced_cells;i++){
        current_reservoir->demand[i]=0.0;

    }
}

void shift_outflow_array(rout_cell* current_cell){
    extern global_param_struct global_param;
    
    size_t t;                
    for(t=0;t<(UH_MAX_DAYS * global_param.model_steps_per_day)-1;t++){
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
    
    size_t iVeg = current_cell->irr_veg_nr;
    
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

double get_irrigation_demand(rout_cell* current_cell, double moisture_content){
    extern soil_con_struct *soil_con;
    
    double irrigation_demand; //mm

    if(moisture_content < soil_con[current_cell->id].Wcr[0]){
        irrigation_demand = ((soil_con[current_cell->id].Wcr[0] / 0.7) - moisture_content); //mm
    }else{
        irrigation_demand = 0.0;
    }
    
    return irrigation_demand;
}

double do_runoff_irrigation(double* irrigation_demand, double available_runoff_water){
    
    double added_runoff_water=0.0;
    
    if(*irrigation_demand>0.0 && available_runoff_water>0.0){
        if(available_runoff_water > *irrigation_demand){
            added_runoff_water = *irrigation_demand;
            *irrigation_demand = 0.0;
        }else{
            added_runoff_water = available_runoff_water;
            *irrigation_demand -= added_runoff_water;
        }
    }
    
    return added_runoff_water;
}

double do_river_irrigation(double* irrigation_demand, double available_river_water){
        
    double added_river_water=0.0;
    
    if(*irrigation_demand>0.0 && available_river_water>0){
        if(available_river_water > *irrigation_demand){
            added_river_water = *irrigation_demand;
            *irrigation_demand = 0.0;
        }else{
            added_river_water = available_river_water;
            *irrigation_demand -= available_river_water;
        }
    }
    
    return added_river_water;
}

void distribute_demand_among_reservoirs(rout_cell* current_cell, double irrigation_demand){
    
    if(irrigation_demand > 0 && current_cell->nr_servicing_reservoirs > 0){

        double total_current_capacity=0.0;
        
        size_t r;
        for(r=0;r<current_cell->nr_servicing_reservoirs;r++){
            if(current_cell->servicing_reservoirs[r]->run==false){
                continue;
            }
            
            total_current_capacity+=current_cell->servicing_reservoirs[r]->current_storage;
        }
        
        for(r=0;r<current_cell->nr_servicing_reservoirs;r++){
            if(current_cell->servicing_reservoirs[r]->run==false){
                continue;
            }
            
            size_t c;
            for(c=0;c<current_cell->servicing_reservoirs[r]->nr_serviced_cells;c++){

                if(current_cell->servicing_reservoirs[r]->serviced_cells[c]->id == current_cell->id){
                    current_cell->servicing_reservoirs[r]->demand[c]=irrigation_demand * (current_cell->servicing_reservoirs[r]->current_storage / total_current_capacity);
                }
            }
        }
    }
}

double get_reservoir_demand(reservoir_unit * current_reservoir){
    extern veg_con_struct **veg_con;
    
    double total_demand=0.0; //m3
    
    rout_cell* service_cell;
    size_t iCell;
    for(iCell=0;iCell<current_reservoir->nr_serviced_cells;iCell++){
        service_cell = current_reservoir->serviced_cells[iCell];
        size_t iVeg = service_cell->irr_veg_nr;
        
        total_demand += current_reservoir->demand[iCell] * veg_con[service_cell->id][iVeg].Cv * service_cell->location->area / MM_PER_M;
    }
    
    return total_demand;
}

double do_reservoir_operation(reservoir_unit* current_reservoir){    
    double release_coefficient = (double)current_reservoir->storage_start_operation / (RES_PREF_STORAGE * (double)current_reservoir->storage_capacity);
    double total_mean_annual_demand = 0.0; //m3
    double total_mean_annual_inflow = 0.0; //m3
    double total_mean_monthly_demand = 0.0; //m3
    double total_mean_monthly_inflow = 0.0; //m3
    double target_release = 0.0; //m3
    double c=0.0; //-
    
    size_t t;
    for(t=0;t<RES_CALC_YEARS_MEAN;t++){
        total_mean_annual_demand += current_reservoir->mean_annual_demand[t] / RES_CALC_YEARS_MEAN;
        total_mean_annual_inflow += current_reservoir->mean_annual_inflow[t] / RES_CALC_YEARS_MEAN;
        total_mean_monthly_demand += current_reservoir->mean_monthly_demand[t][0] / RES_CALC_YEARS_MEAN;
        total_mean_monthly_inflow += current_reservoir->mean_monthly_inflow[t][0] / RES_CALC_YEARS_MEAN;
    }
    
    if(current_reservoir->function==RES_IRR_FUNCTION){
        if(total_mean_annual_demand >= 0.5 * total_mean_annual_inflow){
                
            if(total_mean_annual_demand>0){
                target_release = (total_mean_monthly_inflow / 10) + 
                        (9/10) * total_mean_annual_inflow * 
                        (total_mean_monthly_demand / total_mean_annual_demand);
            }else{
                 target_release = (total_mean_monthly_inflow / 10);
            }
        }else{
            target_release = total_mean_annual_inflow + total_mean_monthly_demand - total_mean_annual_demand;
        }
    }else{
        target_release = total_mean_annual_inflow;
    }

    c = current_reservoir->storage_capacity / total_mean_annual_inflow;

    if(c >= 0.5){
        target_release *= release_coefficient;
    }else{
        target_release = pow((c/0.5),2)*release_coefficient*target_release+(1-pow((c/0.5),2))*total_mean_monthly_inflow;
    }
    
    log_info("res %zu target release %.2f",current_reservoir->id,target_release);
    
    if(target_release > current_reservoir->current_storage){
        //log_info("sometin wong 2; c= %.2f",c);
        target_release = current_reservoir->current_storage;
    }
    
    return target_release;
}

double do_reservoir_irrigation(double target_release, double total_demand, double demand){
    
    double added_reservoir_water = 0.0; //mm
    
    if(demand >0){
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