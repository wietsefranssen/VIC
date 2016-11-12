#include <rout.h>

double get_moisture_content(size_t id, size_t veg_index){
    //Get the moisture content for each snow band,
    //and remove ice from the moisture content
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern soil_con_struct *soil_con;
    
    double moisture_ice = 0.0; //mm
    double moisture_content = 0.0; //mm
    
    size_t iBand;
    size_t iFrost;
    
    for(iBand=0;iBand<options.SNOW_BAND;iBand++){
        moisture_content +=  all_vars[id].cell[veg_index][iBand].layer[0].moist 
                * soil_con[id].AreaFract[iBand];
        
        //Gather ice content
        for (iFrost = 0; iFrost < options.Nfrost; iFrost++) {
            moisture_ice += all_vars[id].cell[veg_index][iBand].layer[0].ice[iFrost] 
                    * soil_con[id].AreaFract[iBand] * soil_con[id].frost_fract[iFrost];
        }
    }
        
    moisture_content -= moisture_ice;
    
    return moisture_content;
}

double get_irrigation_demand(size_t id, double moisture_content, size_t veg_index){
    //Calculate irrigation demand based on soil moisture and field capacity
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern domain_struct local_domain;
    
    double irrigation_demand=0.0;  //m3
    
    if(moisture_content <= soil_con[id].Wcr[0]){ 
        //(Wcr / 0.7) is the field capacity of a cell                   
        irrigation_demand = ((soil_con[id].Wcr[0] / 0.7) - moisture_content)
                / MM_PER_M * (local_domain.locations[id].area * 
                veg_con[id][veg_index].Cv);
    }
    
    return irrigation_demand;
}

double do_source_irrigation(double available, double irrigation_demand, double irrigation_demand_total){
    //Do irrigation from a source, takes into account that the source might
    //not be able to irrigate everything, 
    //then irrigates a percentage of the total demand
    double added=0.0;
    
    if(available >= irrigation_demand_total){
        added = irrigation_demand;
    }else{
        added = available *
                (irrigation_demand / irrigation_demand_total);
    }
    
    if(added<0){
        log_err("Adding a negative amount of water?");
    }
    
    return added;
}

void distribute_demand_among_dams(module_cell* cur_cell, double irrigation_demand, size_t iCrop){
    //For each dam of this cell, distribute leftover demand based on
    //dam capacity per cell
    double total_capacity_per_cell=0.0;

    size_t iRes;
    size_t iCell;
    
    if(cur_cell->irr->nr_servicing_dams<=0){
        return;
    }
    
    if(irrigation_demand<0){
        log_warn("Irrigation demand of cell %zu is smaller than 0, ignoring",cur_cell->id);
        return;
    }
    
    //Get total dam capacity for this cell
    for(iRes=0;iRes<cur_cell->irr->nr_servicing_dams;iRes++){
        if(cur_cell->irr->servicing_dams[iRes]->run==false){
            continue;
        }
        total_capacity_per_cell+=cur_cell->irr->servicing_dams[iRes]->capacity / cur_cell->irr->servicing_dams[iRes]->nr_serviced_cells;
    }

    if(total_capacity_per_cell>0.0){
        for(iRes=0;iRes<cur_cell->irr->nr_servicing_dams;iRes++){
            if(cur_cell->irr->servicing_dams[iRes]->run==false){
                continue;
            }
            
            for(iCell=0;iCell<cur_cell->irr->servicing_dams[iRes]->nr_serviced_cells;iCell++){
                if(cur_cell->irr->servicing_dams[iRes]->serviced_cells[iCell]->id == cur_cell->id){
                    //Set cell demand for the dam
                    cur_cell->irr->servicing_dams[iRes]->cell_demand[iCell][iCrop] = irrigation_demand *
                            ((cur_cell->irr->servicing_dams[iRes]->capacity / cur_cell->irr->servicing_dams[iRes]->nr_serviced_cells) 
                            / total_capacity_per_cell);
                    break;
                }
            }
        }
    }
}

void check_irrigation(double *end_value, double start_value, double added_water){
    //Check if runoff and inflow are correct (also account for accuracy errors)
    if(*end_value<0){
        log_err("Runoff or inflow is negative");
    }
    
    if(*end_value>start_value){
        if(added_water == 0){
            *end_value=start_value;
        }else{
            log_err("Runoff or inflow has increased by irrigation?");
        }
    }
}

void do_uh_routing(module_cell* cur_cell, double inflow, double runoff){
    //If the current cell does not have an active reservoir, do normal routing
    extern module_struct rout;
    extern global_param_struct global_param;
    
    size_t t;
    
    cur_cell->rout->outflow[0] += runoff;

    for(t=0;t<rout.param.max_days_uh * global_param.model_steps_per_day;t++){
        cur_cell->rout->outflow[t] += cur_cell->rout->uh[t] * inflow;
    }
}

void do_dam_routing(module_cell* cur_cell, double inflow, double runoff){
    //If the current cell does have an active reservoir, no routing but rather add to reservoir storage
    extern global_param_struct global_param;
    
    cur_cell->dam->current_storage += 
            (inflow + runoff) * global_param.dt;

    cur_cell->dam->current_inflow += 
            (inflow + runoff) * global_param.dt;

}

double get_total_dam_demand(dam_unit *dam){
    //Get all the demand for the current dam
    size_t iCell;
    size_t iCrop;
    
    double demand=0.0;
    
    for(iCell=0;iCell<dam->nr_serviced_cells;iCell++){
        for(iCrop=0;iCrop<dam->serviced_cells[iCell]->irr->nr_crops;iCrop++){
            demand += dam->cell_demand[iCell][iCrop];
        }
    }
    
    return demand;
}

void reset_dam_demand(dam_unit *dam){
    //Set reservoir cell demand to 0
    size_t iCell;
    size_t iCrop;
    
    for(iCell=0;iCell<dam->nr_serviced_cells;iCell++){
        for(iCrop=0;iCrop<dam->serviced_cells[iCell]->irr->nr_crops;iCrop++){
            dam->cell_demand[iCell][iCrop]=0.0;
        }
    }
}

void change_crop_fraction(module_cell* cur_cell, dmy_struct* current_dmy){
    //Change crop fraction and soil moisture content based on growing season
    //FIXME: Crop fractions can now only do exactly 1 year and not over the newyear    
    extern module_struct rout;
    extern veg_con_struct **veg_con;
    extern veg_con_map_struct *veg_con_map;
    
    double factor = 0.0;
    double difference = 0.0;
    double moisture_content_crop = 0.0;
    double moisture_content_bare = 0.0;
    double new_moisture_content = 0.0;
    
    size_t iCrop;
    
    if(current_dmy->dayseconds>1){
        return;
    }
    
    for(iCrop=0;iCrop<cur_cell->irr->nr_crops;iCrop++){
        //Calculate the new crop fraction
        if(current_dmy->day_in_year >= rout.param.crop_harvest[cur_cell->irr->crop_index[iCrop]]){

        }else if(current_dmy->day_in_year >= rout.param.crop_matured[cur_cell->irr->crop_index[iCrop]]){
            factor = 1-(((double)current_dmy->day_in_year - 
                    (double)rout.param.crop_matured[cur_cell->irr->crop_index[iCrop]]) /
                    ((double)rout.param.crop_harvest[cur_cell->irr->crop_index[iCrop]] - 
                    (double)rout.param.crop_matured[cur_cell->irr->crop_index[iCrop]]));
        }else if(current_dmy->day_in_year >= rout.param.crop_developed[cur_cell->irr->crop_index[iCrop]]){
            factor=1;
        }else if(current_dmy->day_in_year >= rout.param.crop_sow[cur_cell->irr->crop_index[iCrop]]){
            factor = (((double)current_dmy->day_in_year - 
                    (double)rout.param.crop_sow[cur_cell->irr->crop_index[iCrop]]) /
                    ((double)rout.param.crop_developed[cur_cell->irr->crop_index[iCrop]] - 
                    (double)rout.param.crop_sow[cur_cell->irr->crop_index[iCrop]]));
        }

        if(factor<MIN_CROP_FACTOR){
            factor=MIN_CROP_FACTOR;
        }

        difference = veg_con[cur_cell->id][cur_cell->irr->veg_index[iCrop]].Cv - 
                cur_cell->irr->max_cv[iCrop] * 
                factor;

        //Get moisture content of our crop and bare soil
        moisture_content_crop = get_moisture_content(cur_cell->id,cur_cell->irr->veg_index[iCrop]);
        moisture_content_bare = get_moisture_content(cur_cell->id,veg_con[cur_cell->id][0].vegetat_type_num);

        //Change soil moisture based on fraction change
        if(difference<0){
            new_moisture_content = 
                    moisture_content_crop * (veg_con[cur_cell->id][cur_cell->irr->veg_index[iCrop]].Cv 
                    / (veg_con[cur_cell->id][cur_cell->irr->veg_index[iCrop]].Cv-difference)) +
                    moisture_content_bare * ((-difference) 
                    / (veg_con[cur_cell->id][cur_cell->irr->veg_index[iCrop]].Cv-difference));
            add_moisture_content(cur_cell,cur_cell->irr->veg_index[iCrop],new_moisture_content);
        }else if(difference>0){
            new_moisture_content = 
                    moisture_content_crop * (difference 
                    / (veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv+difference)) +
                    moisture_content_bare * (veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv 
                    / (veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv+difference));
            add_moisture_content(cur_cell,veg_con[cur_cell->id][0].vegetat_type_num,new_moisture_content);
        }

        //Change crop fractions
        veg_con[cur_cell->id][cur_cell->irr->veg_index[iCrop]].Cv -= difference;                
        veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv += difference;
        veg_con_map[cur_cell->id].Cv[cur_cell->irr->veg_class[iCrop]] -= difference;                
        veg_con_map[cur_cell->id].Cv[BARE_SOIL_VEG_CLASS] += difference;
        
        iCrop++;
    }
}

void add_moisture_content(module_cell* cur_cell, size_t veg_index, double new_content){
    //Add moisture content over all snow bands
    extern option_struct options;
    extern all_vars_struct *all_vars;
    
    size_t iBand;
    
    for(iBand=0;iBand<options.SNOW_BAND;iBand++){
        all_vars[cur_cell->id].cell[veg_index][iBand].layer[0].moist = new_content;
    }
}

void calculate_dam_values(dam_unit* cur_dam, dmy_struct* current_dmy){
    //Do dam history calculations with inflow, demand and natural inflow
    //Recalculate the operational year at the end of an operational year
    //Set target release every month
    extern global_param_struct global_param;
    extern module_struct rout;
            
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
    
    if(current_dmy->dayseconds==0){
        if(current_dmy->day==cur_dam->start_operation.day){
            //an operational month has passed
            
            int prev_month = current_dmy->month-2;            
            int year_nr = (current_dmy->year - global_param.startyear);
            
            if(prev_month<0){
                prev_month += MONTHS_PER_YEAR;
                if(year_nr>0){
                    year_nr--;
                }
            }
            
            year_nr = year_nr % DAM_CALC_YEARS_MEAN;
            
            //Add inflow, demand and natural inflow to history
            cur_dam->inflow[year_nr][prev_month] = cur_dam->current_inflow / global_param.model_steps_per_day / nr_days_per_month(current_dmy->month,current_dmy->year);
            cur_dam->demand[year_nr][prev_month] = cur_dam->current_demand / global_param.model_steps_per_day / nr_days_per_month(current_dmy->month,current_dmy->year);
            cur_dam->inflow_natural[year_nr][prev_month] = cur_dam->current_inflow_natural / global_param.model_steps_per_day / nr_days_per_month(current_dmy->month,current_dmy->year);
            
            cur_dam->current_inflow=0.0;
            cur_dam->current_inflow_natural=0.0;
            cur_dam->current_demand=0.0;
            
            for(t=0;t<MONTHS_PER_YEAR;t++){
                mean_monthly_inflow[t]=0.0;
                mean_monthly_demand[t]=0.0;
                mean_monthly_inflow_natural[t]=0.0;
            }
            
            //Get average values from history
            get_all_mean_dam_values(cur_dam,current_dmy,mean_monthly_inflow,mean_monthly_demand,mean_monthly_inflow_natural,
                    &mean_annual_inflow,&mean_annual_demand,&mean_annual_inflow_natural);
            
            //Add values for each reservoir
            cur_dam->monthly_demand = mean_monthly_demand[current_dmy->month-1];
            cur_dam->monthly_inflow = mean_monthly_inflow[current_dmy->month-1];
            cur_dam->monthly_inflow_natural = mean_monthly_inflow_natural[current_dmy->month-1];
            cur_dam->annual_demand=mean_annual_demand;
            cur_dam->annual_inflow=mean_annual_inflow;
            cur_dam->annual_inflow_natural=mean_annual_inflow_natural;
            
            //Calculate target release after 1 year
            cur_dam->target_release=0.0;
            if(current_dmy->year > global_param.startyear){
                cur_dam->target_release = do_reservoir_operation(*cur_dam);
            }
            
            if(current_dmy->month == cur_dam->start_operation.month && current_dmy->year > global_param.startyear){
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
                cur_dam->start_operation = operational_year;         
                cur_dam->storage_start_operation = cur_dam->current_storage;
                if(cur_dam->storage_start_operation > cur_dam->capacity){
                    cur_dam->storage_start_operation = cur_dam->capacity;
                }
            }
        }
    }
}

void shift_outflow_array(module_cell* current_cell){
    //Shift the outflow array
    extern global_param_struct global_param;
    extern module_struct rout;
    
    size_t t;                
    
    for(t=0;t<(rout.param.max_days_uh * global_param.model_steps_per_day)-1;t++){
        *(current_cell->rout->outflow + t) = *(current_cell->rout->outflow + (t+1));
        *(current_cell->rout->outflow_natural + t) = *(current_cell->rout->outflow_natural + (t+1));
    }
    
    *(current_cell->rout->outflow + t) = 0.0;
    *(current_cell->rout->outflow_natural + t) = 0.0;
}

double do_reservoir_operation(dam_unit current_reservoir){   
    //Calculate reservoir release
    //Based on the operation scheme of Biemans et al. (2011)
    //Only difference is that hydropower dams release their mean monthly inflow
    extern module_struct rout;
    
    double target_release = 0.0; //m3
    double c;
    double release_coefficient;
    
    //Coefficient based on inter-annual flow variations
    release_coefficient = (double)current_reservoir.storage_start_operation / (RES_PREF_STORAGE * (double)current_reservoir.capacity); 
    //Coefficient which includes the relative storage capacity to the annual inflow
    c = current_reservoir.capacity / current_reservoir.annual_inflow;
    
    if(current_reservoir.function==DAM_IRR_FUNCTION){
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
    }else if (current_reservoir.function==DAM_CON_FUNCTION){
        //If the reservoir does not has a control function        
        //Release annual inflow
        target_release = current_reservoir.annual_inflow;
    }else{
        //If the reservoir has a hydropower or other function       
        //Release annual inflow
        target_release = current_reservoir.monthly_inflow;
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

void get_all_mean_dam_values(dam_unit* current_reservoir, dmy_struct* current_dmy,
    double mean_monthly_inflow[MONTHS_PER_YEAR], double mean_monthly_demand[MONTHS_PER_YEAR], double mean_monthly_inflow_natural[MONTHS_PER_YEAR],
    double* mean_annual_inflow, double* mean_annual_demand, double* mean_annual_inflow_natural){
    
    //Calculate the mean inflow, natural inflow and demand values from the dam history
    extern global_param_struct global_param;
    
    size_t years_done = current_dmy->year - global_param.startyear;
    int months_done = current_dmy->month - global_param.startmonth;
    
    int t;
    size_t j;
    
    //Calculate how many months have already been simulated
    if(months_done<0){
        months_done += MONTHS_PER_YEAR;
    }
    if(years_done>DAM_CALC_YEARS_MEAN-1){
        years_done=DAM_CALC_YEARS_MEAN-1;
        months_done=MONTHS_PER_YEAR;
    }
    
    //Add together values for all the years we have already done
    for(j=0;j<years_done;j++){
        for(t=0;t<MONTHS_PER_YEAR;t++){
            if(t<months_done){
                mean_monthly_inflow[t]+=current_reservoir->inflow[j][t] / (years_done+1);
                mean_monthly_demand[t]+=current_reservoir->demand[j][t] / (years_done+1);
                mean_monthly_inflow_natural[t]+=current_reservoir->inflow_natural[j][t] / (years_done+1);
            }else{
                mean_monthly_inflow[t]+=current_reservoir->inflow[j][t] / (years_done);
                mean_monthly_demand[t]+=current_reservoir->demand[j][t] / (years_done); 
                mean_monthly_inflow_natural[t]+=current_reservoir->inflow_natural[j][t] / (years_done); 
            }

            *mean_annual_demand += current_reservoir->demand[j][t] /((years_done) * MONTHS_PER_YEAR);
            *mean_annual_inflow += current_reservoir->inflow[j][t] /((years_done) * MONTHS_PER_YEAR);
            *mean_annual_inflow_natural += current_reservoir->inflow_natural[j][t] /((years_done) * MONTHS_PER_YEAR);
        }
    }
    
    //Add together values for the current year
    for(t=0;t<months_done;t++){
        mean_monthly_inflow[t]+= current_reservoir->inflow[j][t] / (years_done+1);
        mean_monthly_demand[t]+= current_reservoir->demand[j][t] / (years_done+1);
        mean_monthly_inflow_natural[t]+= current_reservoir->inflow_natural[j][t] / (years_done+1);

        if(years_done==0){
            *mean_annual_demand += current_reservoir->demand[years_done][t] /(months_done);
            *mean_annual_inflow += current_reservoir->inflow[years_done][t] /(months_done);
            *mean_annual_inflow_natural += current_reservoir->inflow_natural[years_done][t] /(months_done);
        }
    }
}

void set_dam_deficit(dam_unit* cur_dam, module_cell* cur_cell, double moisture_content,size_t iCell, size_t iCrop, size_t vidx){
    //Calculate how much water the dam failed to irrigate
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    
    if(cur_dam->cell_demand[iCell][iCrop] > 0){
        if(moisture_content<(soil_con[cur_cell->id].Wcr[0] / 0.7)){
            cur_dam->prev_deficit[iCell][iCrop]=
                    ((soil_con[cur_cell->id].Wcr[0] / 0.7) -
                    moisture_content) *
                    veg_con[cur_cell->id][vidx].Cv;
        }
    }else{
        cur_dam->prev_deficit[iCell][iCrop]=0;
    }
}

void set_dam_demand(dam_unit* cur_dam, module_cell* cur_cell,size_t iCell, size_t iCrop){
    //Calculate how much extra demand the dam received since the last time step
    extern domain_struct local_domain;
    
    double demand_increase=0.0;
    
    if(cur_dam->cell_demand[iCell][iCrop] > 0){
        demand_increase = (cur_dam->cell_demand[iCell][iCrop] /
                local_domain.locations[cur_cell->id].area * MM_PER_M) -
             cur_dam->prev_deficit[iCell][iCrop];
        if(demand_increase>0){
            cur_dam->current_demand += demand_increase * 
            local_domain.locations[cur_cell->id].area / MM_PER_M;
        }
    }
}