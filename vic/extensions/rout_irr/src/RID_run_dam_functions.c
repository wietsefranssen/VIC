/******************************************************************************
 * @section DESCRIPTION
 *  
 * Functions used in rout_run
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Update the inflow and demand history of a dam. Recalculate multi-year monthly
 * average inflow, natural inflow and demand.
 ******************************************************************************/
void update_dam_history(dam_unit* cur_dam, dmy_struct* cur_dmy){
    extern global_param_struct global_param;
    
    int month_nr;
    int year_nr;
    int prev_month;
              
    month_nr = cur_dmy->month - global_param.startmonth;            
    year_nr = cur_dmy->year - global_param.startyear;
    prev_month = (year_nr * MONTHS_PER_YEAR + month_nr-1) % (DAM_HIST_YEARS * MONTHS_PER_YEAR);
    if(prev_month<0){
        prev_month += DAM_HIST_YEARS * MONTHS_PER_YEAR;
    }
    
    cur_dam->history_inflow[prev_month] = cur_dam->total_inflow / nr_days_in_month(cur_dmy->month-1,cur_dmy->year);
    cur_dam->history_demand[prev_month] = cur_dam->total_demand / nr_days_in_month(cur_dmy->month-1,cur_dmy->year);
    cur_dam->history_inflow_natural[prev_month] = cur_dam->total_inflow_natural / nr_days_in_month(cur_dmy->month-1,cur_dmy->year);

    cur_dam->total_inflow=0.0;
    cur_dam->total_inflow_natural=0.0;
    cur_dam->total_demand=0.0;
    
    cur_dam->release = cur_dam->monthly_release[month_nr] / global_param.model_steps_per_day;
    cur_dam->environmental_release = cur_dam->monthly_environmental_release[month_nr] / global_param.model_steps_per_day;
}


/******************************************************************************
 * @section brief
 *  
 * Update the inflow and demand history of a dam. Recalculate multi-year annual
 * average inflow, natural inflow and demand. Recalculate flow variability
 ******************************************************************************/
void get_multi_year_average(dam_unit* cur_dam, dmy_struct* cur_dmy,
                        double monthly_inflow[], double monthly_inflow_natural[], 
                        double monthly_demand[], double *annual_inflow, 
                        double *annual_inflow_natural, double *annual_demand){
    extern global_param_struct global_param;
    
    size_t i;
    size_t j;
    size_t years_passed;
    
    for(j=0;j<MONTHS_PER_YEAR;j++){
        monthly_inflow[j] = 0;
        monthly_inflow_natural[j] = 0;
        monthly_demand[j] = 0;
    }
    *annual_inflow=0;
    *annual_inflow_natural=0;
    *annual_demand=0;
    
    years_passed = cur_dmy->year - global_param.startyear;
    if(years_passed>DAM_HIST_YEARS){
        years_passed=DAM_HIST_YEARS;
    }
    
    for(i=0;i<years_passed;i++){
        for(j=0;j<MONTHS_PER_YEAR;j++){
            monthly_inflow[j] += cur_dam->history_inflow[i * MONTHS_PER_YEAR + j]/years_passed;
            monthly_inflow_natural[j] += cur_dam->history_inflow_natural[i * MONTHS_PER_YEAR + j]/years_passed;
            monthly_demand[j] += cur_dam->history_demand[i * MONTHS_PER_YEAR + j]/years_passed;
            *annual_inflow += cur_dam->history_inflow[i * MONTHS_PER_YEAR + j]/(years_passed * MONTHS_PER_YEAR);
            *annual_inflow_natural += cur_dam->history_inflow_natural[i * MONTHS_PER_YEAR + j]/(years_passed * MONTHS_PER_YEAR);
            *annual_demand += cur_dam->history_demand[i * MONTHS_PER_YEAR + j]/(years_passed * MONTHS_PER_YEAR);
        }        
    }
}

void calculate_dam_release(dam_unit *cur_dam, dmy_struct* cur_dmy,
                        double monthly_inflow[], double monthly_inflow_natural[], 
                        double annual_inflow, double annual_inflow_natural){
    extern global_param_struct global_param;
    
    size_t j;
    size_t month;
    
    double annual_factor;
    double env_release[MONTHS_PER_YEAR];
    double dam_discharge[MONTHS_PER_YEAR];
    double cumulative_cap;
    double total_cap_needed;   
    double release_added; 
    double inter_annual_change;
          
    if(!global_param.fnaturalized_flow){
        for(j=0;j<MONTHS_PER_YEAR;j++){
            monthly_inflow_natural[j]=monthly_inflow[j];
        }
        annual_inflow_natural = annual_inflow;
    }    
    
    for(annual_factor = DAM_ANN_FRACT_MIN; annual_factor<DAM_ANN_FRACT_MAX; annual_factor+=DAM_ANN_FRACT_ITE){
        total_cap_needed=0;
        cumulative_cap=0;
        
        for(j=0;j<MONTHS_PER_YEAR;j++){
            dam_discharge[j]= annual_inflow * (1-annual_factor) +
                    annual_inflow * annual_factor * (monthly_inflow[j]/annual_inflow);
            
            month = global_param.startmonth+j;
            if(month>MONTHS_PER_YEAR){
                month-=MONTHS_PER_YEAR;
            }
            
            cumulative_cap += (monthly_inflow[j]-dam_discharge[j]) * nr_days_in_month(month,cur_dmy->year);
            if(cumulative_cap<0){
                cumulative_cap=0;
            }
            
            if(cumulative_cap>total_cap_needed){
                total_cap_needed=cumulative_cap;
            }
        }
        
        if(total_cap_needed<cur_dam->capacity){
            break;
        }        
    }
       
    if(cur_dam->current_storage>cur_dam->capacity){
        inter_annual_change = ((cur_dam->capacity * DAM_PREF_STORE) - cur_dam->capacity) / DAYS_PER_YEAR;
    }else{
        inter_annual_change = ((cur_dam->capacity * DAM_PREF_STORE) - cur_dam->current_storage) / DAYS_PER_YEAR;
    }
    
    for(j=0;j<MONTHS_PER_YEAR;j++){
        dam_discharge[j] -= inter_annual_change * (dam_discharge[j]/annual_inflow);
        
        if(dam_discharge[j]<0){
            dam_discharge[j]=0;
        }
    }
    
    if(global_param.fenv_flow){
        for(j=0;j<MONTHS_PER_YEAR;j++){
            if(monthly_inflow_natural[j] > DAM_HIGH_FLOW_PERC * annual_inflow_natural){
                env_release[j] = monthly_inflow_natural[j] * DAM_ENV_FLOW_LOW;
            }else if(monthly_inflow_natural[j] > DAM_LOW_FLOW_PERC * annual_inflow_natural){
                env_release[j] = monthly_inflow_natural[j] * DAM_ENV_FLOW_INT;
            }else{
                env_release[j] = monthly_inflow_natural[j] * DAM_ENV_FLOW_HIGH;
            }
        }
    }
    
    release_added=0;
    for(j=0;j<MONTHS_PER_YEAR;j++){
        if(dam_discharge[j]<env_release[j]){
            month = global_param.startmonth+j;
            if(month>MONTHS_PER_YEAR){
                month-=MONTHS_PER_YEAR;
            }
            
            release_added += (env_release[j]-dam_discharge[j]) * nr_days_in_month(month,cur_dmy->year);
            dam_discharge[j]=env_release[j];
        }
    }
    
    release_added /= DAYS_PER_YEAR;    
    for(j=0;j<MONTHS_PER_YEAR;j++){
        dam_discharge[j] -= release_added;
        
        if(dam_discharge[j]<0){
            dam_discharge[j]=0;
        }
        
        if(dam_discharge[j]<env_release[j]){
            env_release[j]=dam_discharge[j];
        }
    }
    
    for(j=0;j<MONTHS_PER_YEAR;j++){
        cur_dam->monthly_release[j]=dam_discharge[j];
        cur_dam->monthly_environmental_release[j]=env_release[j];
    }
    
    log_info("\n"
            "name\t%s\n"
            "capacity\t%.1f\n"
            "annual_inflow\t%.1f\n"
            "annual_factor\t%.2f\n"
            "total_capacity_needed\t%.1f\n"
            "inter_annual_change\t%.1f\n"
            "release_added\t%.1f\n"
            "inflow;\t\tdischarge;\tenv_release;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n"
            "%.1f;\t\t%.1f;\t\t%.1f;\n",
            cur_dam->name,
            cur_dam->capacity/10000,
            annual_inflow/10000,
            annual_factor,
            total_cap_needed/10000,
            inter_annual_change/10000,
            release_added/10000,
            monthly_inflow[0]/10000,dam_discharge[0]/10000,env_release[0]/10000,
            monthly_inflow[1]/10000,dam_discharge[1]/10000,env_release[1]/10000,
            monthly_inflow[2]/10000,dam_discharge[2]/10000,env_release[2]/10000,
            monthly_inflow[3]/10000,dam_discharge[3]/10000,env_release[3]/10000,
            monthly_inflow[4]/10000,dam_discharge[4]/10000,env_release[4]/10000,
            monthly_inflow[5]/10000,dam_discharge[5]/10000,env_release[5]/10000,
            monthly_inflow[6]/10000,dam_discharge[6]/10000,env_release[6]/10000,
            monthly_inflow[7]/10000,dam_discharge[7]/10000,env_release[7]/10000,
            monthly_inflow[8]/10000,dam_discharge[8]/10000,env_release[8]/10000,
            monthly_inflow[9]/10000,dam_discharge[9]/10000,env_release[9]/10000,
            monthly_inflow[10]/10000,dam_discharge[10]/10000,env_release[10]/10000,
            monthly_inflow[11]/10000,dam_discharge[11]/10000,env_release[11]/10000);
}

/******************************************************************************
 * @section brief
 *  
  * Recalculate beginning operational year as the point where the average
 * monthly inflow is less than annual inflow
 ******************************************************************************/
void calculate_operational_year(dam_unit* cur_dam, dmy_struct *cur_dmy,
                        double monthly_inflow[], double annual_inflow){   
    extern global_param_struct global_param;
    
    size_t j;
    int count;
    double count_inflow;
    int longest;
    double longest_inflow;
    int month;
    
        
    count=0;
    count_inflow=0;
    longest=0;
    longest_inflow=0;
    month=0;
    for(j=0;j<(2 * MONTHS_PER_YEAR);j++){
        size_t i = j % MONTHS_PER_YEAR;

        if(monthly_inflow[i]>annual_inflow){
            count++;
            count_inflow += monthly_inflow[i];

            if(count>longest){
                longest=count;
                longest_inflow=count_inflow;
                month=i;

            }else if (count==longest && count_inflow>longest_inflow){
                longest=count;
                longest_inflow=count_inflow;
                month=i;
            }
        }else{
            count=0;
            count_inflow=0;
        }
    }
    
    month=global_param.startmonth+month;
    month = month % MONTHS_PER_YEAR;

    cur_dam->start_operation.day=1;
    cur_dam->start_operation.month=month+1;
    cur_dam->start_operation.dayseconds=0;
    cur_dam->start_operation.year=cur_dmy->year;
    
    cur_dam->storage_start_operation = cur_dam->current_storage;
    if(cur_dam->storage_start_operation > cur_dam->capacity){
        cur_dam->storage_start_operation = cur_dam->capacity;
    }
    
    log_info("\nOperational month calculated at:\t%d",cur_dam->start_operation.month);
}

/******************************************************************************
 * @section brief
 *  
 * Limit target release based on current dam storage
 ******************************************************************************/
void get_actual_release(dam_unit* cur_dam, double *actual_release){
    
    if(cur_dam->release<cur_dam->current_storage){
        *actual_release=cur_dam->release;
    }else{
        *actual_release=cur_dam->current_storage;
    }
}

/******************************************************************************
 * @section brief
 *  
 * Get the demand for the dam
 ******************************************************************************/
void get_demand_cells(double *demand_cells, double *demand_cell, double demand_crop){
    
    *demand_cell += demand_crop;
    *demand_cells += demand_crop;
}

/******************************************************************************
 * @section brief
 *  
 * Irrigate cells with an irrigation demand equally with available irrigation 
 * water.
 ******************************************************************************/
void get_dam_irrigation(double demand_cells, double demand_crop, double *irrigation_crop, double available_water){
    
    if(demand_cells<available_water){
        *irrigation_crop = demand_crop;
    }else{
        *irrigation_crop = demand_crop * (available_water/demand_cells);
    }
    
    if(*irrigation_crop<0){
        log_err("Negative crop irrigation?");
    }
}

/******************************************************************************
 * @section brief
 *  
 * Update values used in irrigation, mostly for output.
 ******************************************************************************/
void update_dam_demand_and_irrigation(double *demand_cells, double *demand_cell, double *demand_crop, double *irrigation_cells, double *irrigation_cell, double irrigation_crop, double *available_water){

    *demand_crop -= irrigation_crop;
    *demand_cell -= irrigation_crop;
    *demand_cells -= irrigation_crop;
    *irrigation_cell += irrigation_crop;
    *irrigation_cells += irrigation_crop;            
    *available_water -= irrigation_crop;
}

/******************************************************************************
 * @section brief
 *  
 * Increase soil moisture for irrigated cells
 ******************************************************************************/
void do_dam_irrigation(size_t cell_id, size_t veg_index, double *moisture_content, double irrigation_crop){
    extern all_vars_struct *all_vars;
    extern domain_struct local_domain;
    extern veg_con_struct **veg_con;
    extern option_struct options;
        
    size_t i;
    
    if(irrigation_crop<0){
        log_err("Adding a negative amount of water?");
    }   
    if(*moisture_content<0){
        log_err("Negative moisture content?");
    }
      
    *moisture_content += irrigation_crop 
                / (local_domain.locations[cell_id].area * 
                veg_con[cell_id][veg_index].Cv) * MM_PER_M;

    for(i=0;i<options.SNOW_BAND;i++){ 
        all_vars[cell_id].cell[veg_index][i].layer[0].moist = *moisture_content;
    }
}

/******************************************************************************
 * @section brief
 *  
 * Set the water deficit for all serviced cells
 ******************************************************************************/
void set_deficit(double *deficit, double *demand){
    
    *deficit = *demand;
}

/******************************************************************************
 * @section brief
 *  
 * Calculate evaporation based on 0.7 * potential evaporation
 ******************************************************************************/
void get_dam_evaporation(dam_unit* cur_dam, double *evaporation){
    extern all_vars_struct *all_vars;
    extern option_struct options;
    
    size_t i;
    
    for(i=0;i<options.SNOW_BAND;i++){
        *evaporation+=all_vars[cur_dam->cell->id].cell[0][i].pot_evap / MM_PER_M * DAM_EVAP_FRAC;
    }
    *evaporation *= cur_dam->area;
}

/******************************************************************************
 * @section brief
 *  
 * Calculate overflow
 ******************************************************************************/
void get_dam_overflow(dam_unit *cur_dam, double *overflow, double actual_release, double irrigation_cells, double evaporation){
    
    double subtraction = actual_release + irrigation_cells + evaporation;
    
    if(cur_dam->current_storage-subtraction > cur_dam->capacity){
        *overflow = (cur_dam->current_storage-subtraction) - cur_dam->capacity;
    }
}

/******************************************************************************
 * @section brief
 *  
 * Release water and overflow to outflow and remove release, overflow, evaporation
 * and irrigation from the dam storage.
 ******************************************************************************/
void do_dam_release(dam_unit* cur_dam, double actual_release, double irrigation_release, double overflow, double evaporation){
    
    cur_dam->current_storage-= actual_release + irrigation_release + overflow + evaporation;
    
    cur_dam->previous_release = actual_release+overflow;
}

