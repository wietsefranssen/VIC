/******************************************************************************
 * @section DESCRIPTION
 *  
 * Functions used in rout_run
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Update the inflow history of a dam.
 ******************************************************************************/
void update_dam_history_day(dam_unit* cur_dam){    
    size_t i;
    size_t j;
        
    if(cur_dam->function == DAM_IRR_FUNCTION){
        for(i=0;i<cur_dam->nr_serviced_cells;i++){
            
            for(j=0;j<cur_dam->serviced_cells[i]->nr_crops;j++){
                cur_dam->total_demand += cur_dam->serviced_cells[i]->deficit[j];               
            }
        }
    }
}

/******************************************************************************
 * @section brief
 *  
 * Update the inflow and demand history of a dam. Recalculate multi-year monthly
 * average inflow, natural inflow and demand.
 ******************************************************************************/
void update_dam_history_month(dam_unit* cur_dam, dmy_struct* cur_dmy){
    extern RID_struct RID;
    extern global_param_struct global_param;
    
    size_t i;
    size_t years_passed;
    int month_nr;
    int year_nr;
    int prev_month;
              
    month_nr = cur_dmy->month - global_param.startmonth;            
    year_nr = cur_dmy->year - global_param.startyear;
    prev_month = (year_nr * MONTHS_PER_YEAR + month_nr-1) % (DAM_HIST_YEARS * MONTHS_PER_YEAR);
    if(prev_month<0){
        prev_month += DAM_HIST_YEARS * MONTHS_PER_YEAR;
    }
    
    cur_dam->inflow[prev_month] = cur_dam->total_inflow / global_param.model_steps_per_day / nr_days_in_month(cur_dmy->month-1,cur_dmy->year);
    cur_dam->demand[prev_month] = cur_dam->total_demand / global_param.model_steps_per_day / nr_days_in_month(cur_dmy->month-1,cur_dmy->year);
    cur_dam->inflow_natural[prev_month] = cur_dam->total_inflow_natural / global_param.model_steps_per_day / nr_days_in_month(cur_dmy->month-1,cur_dmy->year);

    cur_dam->total_inflow=0.0;
    cur_dam->total_inflow_natural=0.0;
    cur_dam->total_demand=0.0;
    
    years_passed = cur_dmy->year - global_param.startyear;
    if(years_passed>DAM_HIST_YEARS){
        years_passed=DAM_HIST_YEARS;
    }
    
    cur_dam->monthly_demand = 0;
    cur_dam->monthly_inflow = 0;
    cur_dam->monthly_inflow_natural = 0;
    for(i=0;i<years_passed;i++){
        cur_dam->monthly_demand += cur_dam->demand[i * MONTHS_PER_YEAR + month_nr]/years_passed;
        cur_dam->monthly_inflow += cur_dam->inflow[i * MONTHS_PER_YEAR + month_nr]/years_passed;
        cur_dam->monthly_inflow_natural += cur_dam->inflow_natural[i * MONTHS_PER_YEAR + month_nr]/years_passed;  
    }

    if(!RID.param.fnaturalized_flow){
        cur_dam->monthly_inflow_natural=cur_dam->monthly_inflow;
        cur_dam->annual_inflow_natural=cur_dam->annual_inflow;
    }    
}

/******************************************************************************
 * @section brief
 *  
 * Calculate target release based on operation schemes for hydropower,
 * irrigation and flood control dams
 ******************************************************************************/
void calculate_target_release(dam_unit* cur_dam){
    double release_coefficient;
    double target_release;
    double environmental_release;
    double main_release;
    double external_release;
    
    if(cur_dam->annual_inflow==0){
        cur_dam->release=0;
        return;
    }
    
    release_coefficient = (double)cur_dam->storage_start_operation / (DAM_PREF_STORE * (double)cur_dam->capacity);
    
    if(cur_dam->monthly_inflow_natural > ENV_HIGH_FLOW_PERC * cur_dam->annual_inflow_natural){
        environmental_release = cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_LOW;
    }else if(cur_dam->monthly_inflow_natural > ENV_LOW_FLOW_PERC * cur_dam->annual_inflow_natural){
        environmental_release = cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_INT;
    }else{
        environmental_release = cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_HIGH;
    }
    
    main_release = (cur_dam->annual_inflow - cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_HIGH) * 
            (1-cur_dam->ext_influence_factor);
    
    if(cur_dam->annual_inflow>0){
        external_release = (cur_dam->annual_inflow - cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_HIGH) * 
                cur_dam->ext_influence_factor * (cur_dam->monthly_inflow/cur_dam->annual_inflow);
    }else{
        external_release = (cur_dam->annual_inflow - cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_HIGH) * 
                cur_dam->ext_influence_factor;
    }
    
    if(main_release<0){
        main_release=0;
    }
    if(external_release<0){
        external_release=0;
    }
    
    switch(cur_dam->function){
        case DAM_IRR_FUNCTION:
            if(cur_dam->annual_demand>0){
                target_release = environmental_release + (main_release + external_release) * 
                        (cur_dam->monthly_demand/cur_dam->annual_demand);
            }else{
                target_release = environmental_release + main_release + external_release;
            }      
            break;
            
        case DAM_CON_FUNCTION:
            target_release = environmental_release + main_release + external_release;                     
            break;
            
        case DAM_HYD_FUNCTION:
            target_release = environmental_release + main_release + external_release;            
            break;        
        
        default:
            log_err("Dam found with unknown function");
            break;
    }
          
    cur_dam->release = target_release * release_coefficient;
    cur_dam->environmental_release = environmental_release * release_coefficient;
    
    if(cur_dam->release<0){
        log_err("target release is negative?");
    }
    
    if(cur_dam->environmental_release<0){
        log_err("environmental target release is negative?");
    }
    
    log_info("\n\ntarget_release %.1f;\n"
            "inflow %.1f; annual_inflow %.1f;\n"
            "inflow_natural %.1f; annual_inflow_natural %.1f;\n"
            "demand %.1f; annual_demand %.1f;\n"
            "function %zu\n"
            "extreme_stor %zu; influence %.2f\n"
            "release_coefficient %.1f",
            cur_dam->release/10000,
            cur_dam->monthly_inflow/10000, cur_dam->annual_inflow/10000, 
            cur_dam->monthly_inflow_natural/10000, cur_dam->annual_inflow_natural/10000, 
            cur_dam->monthly_demand/10000, cur_dam->annual_demand/10000, 
            cur_dam->function,
            cur_dam->extreme_stor, cur_dam->ext_influence_factor,
            release_coefficient); 
}


/******************************************************************************
 * @section brief
 *  
 * Update the inflow and demand history of a dam. Recalculate multi-year annual
 * average inflow, natural inflow and demand. Recalculate flow variability
 ******************************************************************************/
void update_dam_history_year(dam_unit* cur_dam, dmy_struct* cur_dmy){
    extern global_param_struct global_param;
    extern RID_struct RID;
    
    size_t i;
    size_t j;
    size_t years_passed;
        
    years_passed = cur_dmy->year - global_param.startyear;
    if(years_passed>DAM_HIST_YEARS){
        years_passed=DAM_HIST_YEARS;
    }
    
    cur_dam->annual_inflow=0;
    cur_dam->annual_inflow_natural=0;
    cur_dam->annual_demand=0;
    for(i=0;i<years_passed;i++){
        for(j=0;j<MONTHS_PER_YEAR;j++){
            cur_dam->annual_inflow += cur_dam->inflow[i * MONTHS_PER_YEAR + j]/ (years_passed * MONTHS_PER_YEAR);
            cur_dam->annual_inflow_natural += cur_dam->inflow_natural[i * MONTHS_PER_YEAR + j]/ (years_passed * MONTHS_PER_YEAR);
            cur_dam->annual_demand += cur_dam->demand[i * MONTHS_PER_YEAR + j]/ (years_passed * MONTHS_PER_YEAR);
        }        
    }
    
    if(!RID.param.fnaturalized_flow){
        cur_dam->annual_inflow_natural=cur_dam->annual_inflow;
    }
    
    if(cur_dam->extreme_stor>0){
        cur_dam->ext_influence_factor += DAM_EXT_INF_CHANGE;
    }else{
        cur_dam->ext_influence_factor -= DAM_EXT_INF_CHANGE;
    }
    
    if(cur_dam->ext_influence_factor<DAM_MIN_EXT_INF){
        cur_dam->ext_influence_factor=DAM_MIN_EXT_INF;
    }else if(cur_dam->ext_influence_factor>DAM_MAX_EXT_INF){
        cur_dam->ext_influence_factor=DAM_MAX_EXT_INF;
    }
    cur_dam->extreme_stor=0;
}

/******************************************************************************
 * @section brief
 *  
  * Recalculate beginning operational year as the point where the average
 * monthly inflow is less than annual inflow
 ******************************************************************************/
void calculate_operational_year(dam_unit* cur_dam, dmy_struct *cur_dmy){    
    extern global_param_struct global_param;
    
    size_t i;
    size_t j;
    size_t years_passed;
    int count;
    double count_inflow;
    int longest;
    double longest_inflow;
    int month;
    
    double average_monthly_inflow[MONTHS_PER_YEAR];
    
    for(i=0;i<MONTHS_PER_YEAR;i++){
        average_monthly_inflow[i]=0;
    }
        
    years_passed = cur_dmy->year - global_param.startyear;
    if(years_passed>DAM_HIST_YEARS){
        years_passed=DAM_HIST_YEARS;
    }
    
    for(i=0;i<years_passed;i++){
        for(j=0;j<MONTHS_PER_YEAR;j++){
            average_monthly_inflow[j] += cur_dam->inflow[i * MONTHS_PER_YEAR + j]/years_passed;
        }        
    }
    
    count=0;
    count_inflow=0;
    longest=0;
    longest_inflow=0;
    month=0;
    for(j=0;j<(2 * MONTHS_PER_YEAR);j++){
        size_t i = j % MONTHS_PER_YEAR;

        if(average_monthly_inflow[i]>cur_dam->annual_inflow){
            count++;
            count_inflow += average_monthly_inflow[i];

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
    //*demand=0;
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
        *evaporation+=all_vars[cur_dam->cell->id].cell[0][i].pot_evap / MM_PER_M * RES_POT_FRAC;
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
    
    if((cur_dam->current_storage/cur_dam->capacity)>DAM_MAX_PREF_STORE || (cur_dam->current_storage/cur_dam->capacity)<DAM_MIN_PREF_STORE){
        cur_dam->extreme_stor++;
    }       
}

