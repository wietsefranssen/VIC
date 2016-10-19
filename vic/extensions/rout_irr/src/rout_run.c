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
    extern veg_con_struct **veg_con;
    extern soil_con_struct *soil_con;
    extern veg_con_map_struct *veg_con_map;
    
    //routing variables
    double runoff = 0.0;                    //m^3s
    double inflow = 0.0;                    //m^3s
    
    double moisture_content[rout.nr_crop_classes];               //mm (per crop)
    double irrigation_demand[rout.nr_crop_classes];              //mm (per crop)
    double irrigation_demand_cell=0.0;      //mm
    
    
    double added_river_water=0.0;           //mm
    double added_runoff_water=0.0;          //mm
    double added_reservoir_water=0.0;       //mm
    double available_river_water;           //mm
    double available_runoff_water;          //mm
    
    double target_release =0.0;             //m3
    double target_irrigation=0.0;           //m3
    double total_current_demand = 0.0;      //m3
    double total_added_reservoir_water=0.0; //m3
    double overflow = 0.0;                  //m3
    
    double new_moisture_content =0.0;       //mm
    
    size_t vidx = 0;
    
    rout_cell* cur_cell;
    
    //values used for iteration
    size_t iRes;
    size_t iCell;
    size_t iRank;
    size_t iVeg;
    size_t iCrop;
    size_t t;
    
    /* 
     * For all reservoirs, calculate and reset values      
     */
    if(rout.reservoirs){
        reset_reservoirs(current_dmy);
    }
    
    /*
     * Go through the sorted cells and handle routing and irrigation     
     */
    for(iRank=0;iRank<global_domain.ncells_active;iRank++){
        cur_cell = rout.sorted_cells[iRank];
                
        //Shift outflow array
        shift_outflow_array(cur_cell);
        
        //Determine runoff and inflow from upstream cells
        runoff = (out_data[cur_cell->id][OUT_RUNOFF][0]+out_data[cur_cell->id][OUT_BASEFLOW][0]) 
                * local_domain.locations[cur_cell->id].area / MM_PER_M / global_param.dt;
        
        inflow=0.0;
        for(iCell=0;iCell<cur_cell->nr_upstream;iCell++){
            inflow += cur_cell->upstream[iCell]->outflow[0];
        }
        

        change_crop_fraction(cur_cell,current_dmy);
        
        out_data[cur_cell->id][OUT_CROP_FRAC][0]=
                 veg_con[cur_cell->id][vidx].Cv;
        out_data[cur_cell->id][OUT_BARE_FRAC][0]=
                 veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv;

        //If the cell is able to irrigate, do local irrigation  (if irrigation is true)
        if(rout.firrigation && cur_cell->irrigate){
            
            //Get available water from inflow and runoff in mm
            available_river_water = inflow * global_param.dt / 
                local_domain.locations[cur_cell->id].area * MM_PER_M;

            available_runoff_water = runoff * global_param.dt /
                local_domain.locations[cur_cell->id].area * MM_PER_M;
            
            irrigation_demand_cell = 0.0;
            iCrop=0;
            
            for(iVeg=0;iVeg < rout.nr_crop_classes;iVeg++){
                
                vidx=veg_con_map[cur_cell->id].vidx[rout.crop_class[iVeg]];
                if(vidx==(size_t)NODATA_VEG){
                    continue;
                }
                  
                //Get moisture content of our cell
                moisture_content[iCrop] = get_moisture_content(*cur_cell,vidx);
                
                //Get irrigation demand
                //(Wcr / 0.7) is the field capacity of a cell!
                irrigation_demand[iCrop] = 0.0;
                if(moisture_content[iCrop] <= soil_con[cur_cell->id].Wcr[0]){

                    irrigation_demand[iCrop] = 
                            ((soil_con[cur_cell->id].Wcr[0] / 0.7) - moisture_content[iCrop]) * 
                            veg_con[cur_cell->id][vidx].Cv;
                    irrigation_demand_cell += irrigation_demand[iCrop];
                }
                
                iCrop++;
            }
            
            out_data[cur_cell->id][OUT_IRR_DEMAND][0] = irrigation_demand_cell;
            
            iCrop=0;
            
            for(iVeg=0;iVeg < rout.nr_crop_classes;iVeg++){
                
                vidx=veg_con_map[cur_cell->id].vidx[rout.crop_class[iVeg]];
                if(vidx==(size_t)NODATA_VEG){
                    continue;
                }
                
                added_runoff_water = 0.0;
                added_river_water = 0.0;
                
                //Do runoff irrigation
                if(irrigation_demand_cell>0){
                    if(available_runoff_water >= irrigation_demand_cell){
                        added_runoff_water= irrigation_demand[iCrop];
                    }else{
                        added_runoff_water= available_runoff_water * 
                                (irrigation_demand[iCrop] / irrigation_demand_cell);
                    }
                    irrigation_demand[iCrop] -= added_runoff_water;
                    available_runoff_water -= added_runoff_water;

                    //Do river irrigation
                    if(available_river_water >= irrigation_demand_cell){
                        added_river_water = irrigation_demand[iCrop];
                    }else{
                        added_river_water = available_river_water *
                                (irrigation_demand[iCrop] / irrigation_demand_cell);
                    }
                    irrigation_demand[iCrop] -= added_river_water;
                    available_river_water -= added_river_water;
                }
                
                //Communicate leftover demand with reservoirs
                if(irrigation_demand[iCrop]>0){
                    distribute_demand_among_reservoirs(cur_cell,irrigation_demand[iCrop],iCrop);
                }
                
                //Add water to moisture content in soil
                new_moisture_content = moisture_content[iCrop] + ((added_river_water + added_runoff_water) / 
                           veg_con[cur_cell->id][vidx].Cv);;
                add_moisture_content(cur_cell,vidx,new_moisture_content);
                
                out_data[cur_cell->id][OUT_LOCAL_IRR][0] += 
                        added_river_water + added_runoff_water;
                
                iCrop++;
                
            }
            
            //Recalculate runoff and inflow based on left-over available water
            runoff = available_runoff_water / global_param.dt * 
                    local_domain.locations[cur_cell->id].area / MM_PER_M;
            inflow = available_river_water / global_param.dt * 
                    local_domain.locations[cur_cell->id].area / MM_PER_M;

            //Write away output data
            out_data[cur_cell->id][OUT_IRR][0] += 
                    out_data[cur_cell->id][OUT_LOCAL_IRR][0];

        }
        
        //Do routing
        if(cur_cell->reservoir==NULL || cur_cell->reservoir->run==false){
            
            //If the current cell does not have an active reservoir, do normal routing
            cur_cell->outflow[0] += runoff;
            
            for(t=0;t<rout.max_days_uh * global_param.model_steps_per_day;t++){
                cur_cell->outflow[t] += cur_cell->uh[t] * inflow;
            }
        }else{
            
            //If the current cell does have an active reservoir, no routing but rather add to reservoir storage
            cur_cell->reservoir->current_storage += 
                    (inflow + runoff) * global_param.dt;
            
            cur_cell->reservoir->current_inflow += 
                    (inflow + runoff) * global_param.dt;
        }
        
        //Write output data
        out_data[cur_cell->id][OUT_DISCHARGE][0] += cur_cell->outflow[0];
        
        //Calculate naturalized flow (if naturalized flow is true)
        if(rout.naturalized_flow){
            
            //Determine runoff and inflow from upstream cells
            runoff = (out_data[cur_cell->id][OUT_RUNOFF][0]+out_data[cur_cell->id][OUT_BASEFLOW][0]) 
                    * local_domain.locations[cur_cell->id].area / MM_PER_M / global_param.dt;

            inflow=0;
            for(iCell=0;iCell<cur_cell->nr_upstream;iCell++){
                inflow += cur_cell->upstream[iCell]->outflow_natural[0];
            }
            
            //Do routing
            cur_cell->outflow_natural[0] += runoff;
            
            for(t=0;t<rout.max_days_uh * global_param.model_steps_per_day;t++){
                cur_cell->outflow_natural[t] += cur_cell->uh[t] * inflow;
            }
            
            if(cur_cell->reservoir!=NULL){
                cur_cell->reservoir->current_inflow_natural +=
                        (inflow + runoff) * global_param.dt;
            }
            
            //Write output data
            out_data[cur_cell->id][OUT_NATURAL_DISCHARGE][0] += cur_cell->outflow_natural[0];
        }
    }
    
    
    /*
     * Go through the reservoirs and do irrigation     
     */
    if(rout.reservoirs){
        for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
                        
            //Continue if reservoir is not run
            if(!rout.reservoirs[iRes].run){
                continue;
            }
            
            //Reset values
            total_added_reservoir_water=0.0;
            total_current_demand = 0.0;
            
            target_release = rout.reservoirs[iRes].target_release;
            if(target_release>rout.reservoirs[iRes].current_storage){
                target_release = rout.reservoirs[iRes].current_storage;
            }

            //If the reservoir has an irrigation function
            if(rout.reservoirs[iRes].function==RES_IRR_FUNCTION){
                
                //Calculate target irrigation
                target_irrigation = target_release - rout.reservoirs[iRes].monthly_inflow_natural * PERC_ENV_INFLOW;

                //Get total demand from all cells
                for(iCell=0;iCell<rout.reservoirs[iRes].nr_serviced_cells;iCell++){
                    for(iCrop=0;iCrop<cur_cell->nr_crop_class;iCrop++){
                        total_current_demand += rout.reservoirs[iRes].cell_demand[iCell][iCrop];
                    }
                }
                
                //If there is a demand for irrigation water, go through all cells this reservoir serves
                for(iCell=0;iCell<rout.reservoirs[iRes].nr_serviced_cells;iCell++){
                    cur_cell = rout.reservoirs[iRes].serviced_cells[iCell];
                    
                    iCrop=0;
                    
                    for(iVeg=0;iVeg<rout.nr_crop_classes;iVeg++){
                
                        vidx=veg_con_map[cur_cell->id].vidx[rout.crop_class[iVeg]];
                        if(vidx==(size_t)NODATA_VEG){
                            continue;
                        }
                        
                        added_reservoir_water=0.0;
                        
                        //Get moisture content
                        moisture_content[iCrop] = get_moisture_content(*cur_cell,vidx);
                        
                        if(rout.reservoirs[iRes].prev_soil_moisture[iCell][iCrop] - moisture_content[iCrop] > 0 &&
                                moisture_content[iCrop] < (soil_con[cur_cell->id].Wcr[0])){
                            rout.reservoirs[iRes].current_demand += (rout.reservoirs[iRes].prev_soil_moisture[iCell][iCrop] - moisture_content[iCrop]) * 
                                (veg_con[cur_cell->id][vidx].Cv * 
                                local_domain.locations[cur_cell->id].area / MM_PER_M);
                        }
                        
                        //Calculate irrigation water
                        if(total_current_demand>0){
                            if(total_current_demand < target_irrigation){
                                added_reservoir_water =  rout.reservoirs[iRes].cell_demand[iCell][iCrop];
                            }else{
                                added_reservoir_water =  target_irrigation * 
                                        (rout.reservoirs[iRes].cell_demand[iCell][iCrop] / total_current_demand);
                            }
                        }
                        
                        if(added_reservoir_water<0){
                            log_warn("Added reservoir water of reservoir %zu is smaller than 0, ignoring",rout.reservoirs[iRes].id);
                            added_reservoir_water=0.0;
                        }
                        
                        //Add water to moisture content in soil
                        new_moisture_content = moisture_content[iCrop] + (added_reservoir_water /
                                   local_domain.locations[cur_cell->id].area * MM_PER_M /
                                   veg_con[cur_cell->id][vidx].Cv);
                        add_moisture_content(cur_cell,vidx,new_moisture_content);                      

                        //Save total added water
                        total_added_reservoir_water += added_reservoir_water;
                        
                        //Write output data
                        out_data[cur_cell->id][OUT_RES_IRR][0] += added_reservoir_water / local_domain.locations[cur_cell->id].area * MM_PER_M;
                        rout.reservoirs[iRes].prev_soil_moisture[iCell][iCrop]=moisture_content[iCrop];
                        
                        iCrop++;
                    }
                    
                    out_data[cur_cell->id][OUT_IRR][0] += out_data[cur_cell->id][OUT_RES_IRR][0];
                }
            }
            
            //Do overflow if reservoir is full
            overflow = 0;
            if(rout.reservoirs[iRes].current_storage - target_release > rout.reservoirs[iRes].storage_capacity){
                overflow = (rout.reservoirs[iRes].current_storage - target_release) - rout.reservoirs[iRes].storage_capacity;
            }
            
            //Reduce reservoir storage and do outflow
            rout.reservoirs[iRes].current_storage -= target_release + overflow;
            rout.reservoirs[iRes].cell->outflow[1] += ((target_release-total_added_reservoir_water) + overflow) / global_param.dt;
            
            //Write output data
            out_data[rout.reservoirs[iRes].cell->id][OUT_RES_STORE][0]=
                    rout.reservoirs[iRes].current_storage/rout.reservoirs[iRes].storage_capacity;
        }
    }
}

void reset_reservoirs(dmy_struct* current_dmy){
    extern rout_struct rout;
    
    size_t iRes;
    size_t iCell;
    size_t iCrop;
    
    for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
            
        //Reservoir is only run after it is build
        rout.reservoirs[iRes].run=false;
        if(current_dmy->year>=rout.reservoirs[iRes].activation_year){
            rout.reservoirs[iRes].run=true;
        }

        //Set reservoir cell demand to 0
        for(iCell=0;iCell<rout.reservoirs[iRes].nr_serviced_cells;iCell++){
            for(iCrop=0;iCrop<rout.reservoirs[iRes].serviced_cells[iCell]->nr_crop_class;iCrop++){
                rout.reservoirs[iRes].cell_demand[iCell][iCrop]=0.0;
            }

        }

        //Do history calculations
        calculate_reservoir_values(&rout.reservoirs[iRes],current_dmy);

    }
}

void change_crop_fraction(rout_cell* cur_cell, dmy_struct* current_dmy){
    extern rout_struct rout;
    extern veg_con_struct **veg_con;
    extern veg_con_map_struct *veg_con_map;
    
    double factor = 0.0;
    double difference = 0.0;
    double moisture_content_crop = 0.0;
    double moisture_content_bare = 0.0;
    double new_moisture_content = 0.0;
    
    size_t iVeg;
    size_t vidx;
    
    if(current_dmy->dayseconds>1){
        return;
    }
    
    for(iVeg=0;iVeg < rout.nr_crop_classes;iVeg++){

        vidx=veg_con_map[cur_cell->id].vidx[rout.crop_class[iVeg]];
        if(vidx==(size_t)NODATA_VEG){
            continue;
        }
        //Calculate the new crop fraction
        if(current_dmy->day_in_year >= rout.crop_end){

        }else if(current_dmy->day_in_year >= rout.crop_late){
            factor = 1-(((double)current_dmy->day_in_year - (double)rout.crop_late) /
                    ((double)rout.crop_end - (double)rout.crop_late));
        }else if(current_dmy->day_in_year >= rout.crop_developed){
            factor=1;
        }else if(current_dmy->day_in_year >= rout.crop_start){
            factor = (((double)current_dmy->day_in_year - (double)rout.crop_start) /
                    ((double)rout.crop_developed - (double)rout.crop_start));
        }

        if(factor<0.001){
            factor=0.001;
        }

        difference = veg_con[cur_cell->id][vidx].Cv - 
                veg_con_map[cur_cell->id].Cv[rout.crop_class[iVeg]] * 
                factor;

        //Get moisture content of our crop and bare soil
        moisture_content_crop = get_moisture_content(*cur_cell,vidx);
        moisture_content_bare = get_moisture_content(*cur_cell,veg_con[cur_cell->id][0].vegetat_type_num);

        //Change soil moisture based on fraction change
        if(difference<0){
            new_moisture_content = 
                    moisture_content_crop * (veg_con[cur_cell->id][vidx].Cv / (veg_con[cur_cell->id][vidx].Cv-difference)) +
                    moisture_content_bare * ((-difference) / (veg_con[cur_cell->id][vidx].Cv-difference));
            add_moisture_content(cur_cell,vidx,new_moisture_content);
        }else if(difference>0){
            new_moisture_content = 
                    moisture_content_crop * (difference / (veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv+difference)) +
                    moisture_content_bare * (veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv / (veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv+difference));
            add_moisture_content(cur_cell,veg_con[cur_cell->id][0].vegetat_type_num,new_moisture_content);
        }

        //Change crop fractions
        veg_con[cur_cell->id][vidx].Cv -= difference;                
        veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv += difference;
    }
}

void add_moisture_content(rout_cell* cur_cell, size_t vidx, double new_content){
    extern option_struct options;
    extern all_vars_struct *all_vars;
    
    size_t iBand;
    
    for(iBand=0;iBand<options.SNOW_BAND;iBand++){
        all_vars[cur_cell->id].cell[vidx][iBand].layer[0].moist = new_content;
    }
}

void calculate_reservoir_values(reservoir_unit* current_reservoir, dmy_struct* current_dmy){
    /* The function recalculates all values for a reservoir 
     * 
     * It does so every month or every year. Every month values
     * of inflow, demand and natural inflow are stored in their
     * history. Every month average monthly and annual inflow,
     * demand and natural inflow are calculated. From these values
     * the target release of the month are calculated. Every year
     * the new operational year is calculated.
     */
    
    extern global_param_struct global_param;
    extern rout_struct rout;
            
    double mean_monthly_inflow[MONTHS_PER_YEAR];
    double mean_monthly_demand[MONTHS_PER_YEAR];
    double mean_monthly_inflow_natural[MONTHS_PER_YEAR];
    double mean_annual_inflow = 0.0;
    double mean_annual_demand = 0.0;
    double mean_annual_inflow_natural = 0.0;
    
    size_t t;
    size_t count=0;
    size_t longest=0;
    int month=-1;
    dmy_struct operational_year;          

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
                if(current_year>0){
                    current_year--;
                }
            }
            
            current_year = current_year % RES_CALC_YEARS_MEAN;
            
            //Add inflow, demand and natural inflow to history
            current_reservoir->inflow[current_year][current_month] = current_reservoir->current_inflow / global_param.model_steps_per_day / nr_days_per_month(current_dmy->month,current_dmy->year);
            current_reservoir->demand[current_year][current_month] = current_reservoir->current_demand / global_param.model_steps_per_day / nr_days_per_month(current_dmy->month,current_dmy->year);
            if(rout.naturalized_flow){
                current_reservoir->inflow_natural[current_year][current_month] = current_reservoir->current_inflow_natural / global_param.model_steps_per_day / nr_days_per_month(current_dmy->month,current_dmy->year);
            }
            
            current_reservoir->current_inflow=0.0;
            current_reservoir->current_inflow_natural=0.0;
            current_reservoir->current_demand=0.0;
            
            for(t=0;t<MONTHS_PER_YEAR;t++){
                mean_monthly_inflow[t]=0.0;
                mean_monthly_demand[t]=0.0;
                mean_monthly_inflow_natural[t]=0.0;
            }
            
            //Get average values from history
            get_all_mean_reservoir_values(current_reservoir,current_dmy,mean_monthly_inflow,mean_monthly_demand,mean_monthly_inflow_natural,
                    &mean_annual_inflow,&mean_annual_demand,&mean_annual_inflow_natural);
            
            //Add values for each reservoir
            current_reservoir->monthly_demand = mean_monthly_demand[current_dmy->month-1];
            current_reservoir->monthly_inflow = mean_monthly_inflow[current_dmy->month-1];
            current_reservoir->monthly_inflow_natural = mean_monthly_inflow_natural[current_dmy->month-1];
            current_reservoir->annual_demand=mean_annual_demand;
            current_reservoir->annual_inflow=mean_annual_inflow;
            current_reservoir->annual_inflow_natural=mean_annual_inflow_natural;
            
            //Calculate target release
            current_reservoir->target_release=0.0;
            if(current_dmy->year > global_param.startyear){
                current_reservoir->target_release = do_reservoir_operation(*current_reservoir);
            }
            
            if(rout.fdebug_mode){
                fprintf(LOG_DEST,"An operational month has passed for reservoir %zu %s\n",current_reservoir->id,current_reservoir->name);
                fprintf(LOG_DEST,"Monthly inflow %.1f, demand %.1f and natural inflow %.1f\nannual inflow %.1f, demand %.1f and natural inflow %.1f [hm3]\n",
                        current_reservoir->monthly_inflow/1000,current_reservoir->monthly_demand/1000,current_reservoir->monthly_inflow_natural/1000,
                        current_reservoir->annual_inflow/1000,current_reservoir->annual_demand/1000,current_reservoir->annual_inflow_natural/1000);
                fprintf(LOG_DEST," \n");
            }
            
            if(current_dmy->month == current_reservoir->start_operation.month && current_dmy->year > global_param.startyear){

                //an operational year has passed
                
                //Calculate operational year  
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

                operational_year.day=1;
                operational_year.month=month+2;
                operational_year.dayseconds=0;
                operational_year.year=current_dmy->year;

                if(operational_year.month>MONTHS_PER_YEAR){
                    operational_year.month-=MONTHS_PER_YEAR;
                }
                
                //Set operational year
                current_reservoir->start_operation = operational_year;         
                current_reservoir->storage_start_operation = current_reservoir->current_storage;
                if(current_reservoir->storage_start_operation > current_reservoir->storage_capacity){
                    current_reservoir->storage_start_operation = current_reservoir->storage_capacity;
                }

                if(rout.fdebug_mode){
                    fprintf(LOG_DEST,"An operational year has passed for reservoir %zu %s:\n",current_reservoir->id,current_reservoir->name);

                    fprintf(LOG_DEST,"Mean monthly inflow [hm3] %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f;\n"
                            ,mean_monthly_inflow[0]/1000,mean_monthly_inflow[1]/1000,mean_monthly_inflow[2]/1000
                            ,mean_monthly_inflow[3]/1000,mean_monthly_inflow[4]/1000,mean_monthly_inflow[5]/1000
                            ,mean_monthly_inflow[6]/1000,mean_monthly_inflow[7]/1000,mean_monthly_inflow[8]/1000
                            ,mean_monthly_inflow[9]/1000,mean_monthly_inflow[10]/1000,mean_monthly_inflow[11]/1000);
                    fprintf(LOG_DEST,"Mean monthly demand [hm3] %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f;\n"        
                            ,mean_monthly_demand[0]/1000,mean_monthly_demand[1]/1000,mean_monthly_demand[2]/1000
                            ,mean_monthly_demand[3]/1000,mean_monthly_demand[4]/1000,mean_monthly_demand[5]/1000
                            ,mean_monthly_demand[6]/1000,mean_monthly_demand[7]/1000,mean_monthly_demand[8]/1000
                            ,mean_monthly_demand[9]/1000,mean_monthly_demand[10]/1000,mean_monthly_demand[11]/1000);
                    fprintf(LOG_DEST,"Mean monthly natural inflow [hm3] %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f; %.1f;\n"        
                            ,mean_monthly_inflow_natural[0]/1000,mean_monthly_inflow_natural[1]/1000,mean_monthly_inflow_natural[2]/1000
                            ,mean_monthly_inflow_natural[3]/1000,mean_monthly_inflow_natural[4]/1000,mean_monthly_inflow_natural[5]/1000
                            ,mean_monthly_inflow_natural[6]/1000,mean_monthly_inflow_natural[7]/1000,mean_monthly_inflow_natural[8]/1000
                            ,mean_monthly_inflow_natural[9]/1000,mean_monthly_inflow_natural[10]/1000,mean_monthly_inflow_natural[11]/1000);
                    fprintf(LOG_DEST,"Mean annual inflow %.1f; mean annual demand %.1f; mean annual natural inflow %.1f [hm3]\n",
                            mean_annual_inflow/1000,mean_annual_demand/1000,mean_annual_inflow_natural/1000);
                    fprintf(LOG_DEST,"Start operational year set at month %d\n",operational_year.month);
                    fprintf(LOG_DEST," \n");
                }
            }
        }
    }
}

void shift_outflow_array(rout_cell* current_cell){
    extern global_param_struct global_param;
    extern rout_struct rout;
    
    size_t t;                
    
    //Shift the outflow array
    for(t=0;t<(rout.max_days_uh * global_param.model_steps_per_day)-1;t++){
        *(current_cell->outflow + t) = *(current_cell->outflow + (t+1));
        *(current_cell->outflow_natural + t) = *(current_cell->outflow_natural + (t+1));
    }
    
    *(current_cell->outflow + t) = 0.0;
    *(current_cell->outflow_natural + t) = 0.0;
}

double get_moisture_content(rout_cell current_cell, size_t iVeg){
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern soil_con_struct *soil_con;
    
    double moisture_ice = 0.0; //mm
    double moisture_content = 0.0; //mm
    
    size_t iBand;
    size_t iFrost;
    
    //Get the moisture content for each snow band,
    //and remove ice from the moisture content.
    for(iBand=0;iBand<options.SNOW_BAND;iBand++){
        moisture_content +=  all_vars[current_cell.id].cell[iVeg][iBand].layer[0].moist 
                * soil_con[current_cell.id].AreaFract[iBand];

        for (iFrost = 0; iFrost < options.Nfrost; iFrost++) {
            moisture_ice += all_vars[current_cell.id].cell[iVeg][iBand].layer[0].ice[iFrost] 
                    * soil_con[current_cell.id].AreaFract[iBand] * soil_con[current_cell.id].frost_fract[iFrost];
        }
    }
        
    moisture_content -= moisture_ice;
    
    return moisture_content;
}

void distribute_demand_among_reservoirs(rout_cell* cur_cell, double irrigation_demand, size_t iCrop){
    /* The function distributes leftover demand among the servicing reservoirs
     * 
     * It does so by calculating the reservoir storage a reservoir has
     * per cell. The demand is then distributed relatively among 
     * servicing reservoirs based on the reservoir storage per cell.    
     */
    
    extern domain_struct local_domain;
    
    double total_capacity_per_cell=0.0;

    size_t r;
    size_t c;
    
    if(irrigation_demand<0){
        log_warn("Irrigation demand of cell %zu is smaller than 0, ignoring",cur_cell->id);
        return;
    }
    
    for(r=0;r<cur_cell->nr_servicing_reservoirs;r++){
        if(cur_cell->servicing_reservoirs[r]->run==false){
            continue;
        }

        total_capacity_per_cell+=cur_cell->servicing_reservoirs[r]->storage_capacity / cur_cell->servicing_reservoirs[r]->nr_serviced_cells;
    }

    if(total_capacity_per_cell>0.0){

        for(r=0;r<cur_cell->nr_servicing_reservoirs;r++){
            if(cur_cell->servicing_reservoirs[r]->run==false){
                continue;
            }
            for(c=0;c<cur_cell->servicing_reservoirs[r]->nr_serviced_cells;c++){

                if(cur_cell->servicing_reservoirs[r]->serviced_cells[c]->id == cur_cell->id){
                    cur_cell->servicing_reservoirs[r]->cell_demand[c][iCrop] = irrigation_demand *
                            local_domain.locations[cur_cell->id].area / MM_PER_M *
                            ((cur_cell->servicing_reservoirs[r]->storage_capacity / cur_cell->servicing_reservoirs[r]->nr_serviced_cells) 
                            / total_capacity_per_cell);
                    break;
                }
            }
        }
    }
}

double do_reservoir_operation(reservoir_unit current_reservoir){   
    /* The function calculates target release
     *
     * It does so by using the reservoir operation scheme 
     * of Biemans et al. (2006). If the reservoir does not
     * have an irrigation function the target release is equal
     * to the annual inflow. 
     * 
     * Else, if demand is high compared to the inflow 
     * target release is 10% of the natural inflow + 
     * the rest of the inflow scaled by the relative demand 
     * for this month. if demand is low compared to the
     * inflow the annual inflow is released with the difference
     * between the monthly demand and the annual demand.
     * 
     * The target release is scaled by release coefficient
     * which takes into account the inter-annual flow variations.
     */
    
    extern global_param_struct global_param;
    
    double target_release = 0.0; //m3
    double c;
    double release_coefficient;
           
    //Reservoir operation scheme
    
    //Coefficient based on inter-annual flow variations
    release_coefficient = (double)current_reservoir.storage_start_operation / (RES_PREF_STORAGE * (double)current_reservoir.storage_capacity); 
    //Coefficient which includes the relative storage capacity to the annual inflow
    c = current_reservoir.storage_capacity / current_reservoir.annual_inflow;
    
    if(current_reservoir.function==RES_IRR_FUNCTION){
        //If the reservoir has an irrigation function
        if(current_reservoir.annual_demand >= 0.5 * current_reservoir.annual_inflow){
            if(current_reservoir.annual_demand>0){
                //If our demand is larger than half of the annual inflow (high demand)
                //Release environmental inflow + rest of the inflow based on the relative demand
                target_release = (current_reservoir.monthly_inflow_natural * PERC_ENV_INFLOW) + 
                        (current_reservoir.annual_inflow - (current_reservoir.monthly_inflow_natural * PERC_ENV_INFLOW)) * 
                        (current_reservoir.monthly_demand / current_reservoir.annual_demand);
            }else{
                //This is here if annual demand is 0 which would lead to a mathematical error
                //Releases only the environmental flow
                 target_release = (current_reservoir.monthly_inflow_natural * PERC_ENV_INFLOW);
            }
        }else{
            //If demand is smaller than half of the annual inflow (low demand)
            //Release full monthly demand while keeping in mind annual inflow and demand
            target_release = current_reservoir.annual_inflow + current_reservoir.monthly_demand - current_reservoir.annual_demand;
        }
    }else{
        //If the reservoir does not have an irrigation function        
        //Release annual inflow
        target_release = current_reservoir.annual_inflow;
    }
    
    if(c >= 0.5){
        //If we have a low relative inflow        
        //release all keeping in mind annual flow variations
        target_release = target_release * release_coefficient;
    }
    else{
        //If we have a high relative inflow
        //release part of the target release and part of the monthly inflow
        target_release = pow((c/0.5),2) * release_coefficient * target_release + (1-pow((c/0.5),2)) * current_reservoir.monthly_inflow;
    }
    
    //Do not release more than is available
    if(target_release > current_reservoir.current_storage){
        target_release = current_reservoir.current_storage;
    }
    
    //Do not release negative target release
    if(target_release < 0){
        target_release=0;
    }
    
    return target_release;
}

int is_leap_year(int year){
    //calculate leap year (returns 1 if leap year)
    if((year % 4 == 0) || ((year % 100 == 0) && (year % 400 == 0))){
        return 1;
    }
    return 0;
}

int nr_days_per_month(int month, int year){
    //calculate days per month
    if(month == 2){
        return 28 + is_leap_year(year);
    }
    return 31 - (month-1) % 7 % 2;
}

void get_all_mean_reservoir_values(reservoir_unit* current_reservoir, dmy_struct* current_dmy, 
    /* The function calculates the average monthly and annual values
     * of inflow, demand and natural inflow.
     * 
     * It does so by adding the inflow, demand and natural inflow
     * divided by the number of values we have. This way the spinup time
     * is not equal to the history time.
     */
        
    double mean_monthly_inflow[MONTHS_PER_YEAR], double mean_monthly_demand[MONTHS_PER_YEAR], double mean_monthly_inflow_natural[MONTHS_PER_YEAR],
    double* mean_annual_inflow, double* mean_annual_demand, double* mean_annual_inflow_natural){
    
    extern global_param_struct global_param;
    extern rout_struct rout;
    
    size_t years_done = current_dmy->year - global_param.startyear;
    int months_done = current_dmy->month - global_param.startmonth;
    
    int t;
    size_t j;
    
    //Calculate how many months have already been simulated
    if(months_done<0){
        months_done += MONTHS_PER_YEAR;
    }
    if(years_done>RES_CALC_YEARS_MEAN-1){
        years_done=RES_CALC_YEARS_MEAN-1;
        months_done=MONTHS_PER_YEAR;
    }
    
    //Add together values for all the years we have already done
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
    
    //Add together values for the current year
    for(t=0;t<months_done;t++){
        mean_monthly_inflow[t]+= current_reservoir->inflow[j][t] / (years_done+1);
        mean_monthly_demand[t]+= current_reservoir->demand[j][t] / (years_done+1);

        if(years_done==0){
            *mean_annual_demand += current_reservoir->demand[years_done][t] /(months_done);
            *mean_annual_inflow += current_reservoir->inflow[years_done][t] /(months_done);
        }
    }
    
    //Set natural inflow based on the options
    for(t=0;t<MONTHS_PER_YEAR;t++){
        mean_monthly_inflow_natural[t]=mean_monthly_inflow[t];
    }
    *mean_annual_inflow_natural = *mean_annual_inflow;
    
    if(rout.naturalized_flow){
        for(j=0;j<years_done;j++){
            for(t=0;t<MONTHS_PER_YEAR;t++){
                if(t<months_done){
                    mean_monthly_inflow_natural[t]+=current_reservoir->inflow_natural[j][t] / (years_done+1);
                }else{
                    mean_monthly_inflow_natural[t]+=current_reservoir->inflow_natural[j][t] / (years_done);
                }

                *mean_annual_inflow_natural += current_reservoir->inflow_natural[j][t] /((years_done) * MONTHS_PER_YEAR);
            }
        }

        for(t=0;t<months_done;t++){
            mean_monthly_inflow_natural[t]+= current_reservoir->inflow_natural[j][t] / (years_done+1);

            if(years_done==0){
                *mean_annual_inflow_natural += current_reservoir->inflow_natural[years_done][t] /(months_done);
            }
        }
    }
}