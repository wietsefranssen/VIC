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
void update_dam_history_day(dam_unit* cur_dam, dmy_struct *cur_dmy){    
    size_t i;
    size_t j;
    
    double demand_increase=0;
        
    if(cur_dam->function == DAM_IRR_FUNCTION){
        for(i=0;i<cur_dam->nr_serviced_cells;i++){
            irr_cell *irr_cell = cur_dam->serviced_cells[i].cell->irr;
            
            for(j=0;j<irr_cell->nr_crops;j++){
                if(!in_irrigation_season(irr_cell->crop_index[j],cur_dmy->day_in_year)){
                    continue;
                }
                
                if(cur_dam->serviced_cells[i].demand_crop[j] > 0){
                    demand_increase = cur_dam->serviced_cells[i].demand_crop[j] -
                         cur_dam->serviced_cells[i].deficit[j];
                    if(demand_increase>0){
                        cur_dam->total_demand += demand_increase;
                    }
                }                
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
    
    if(cur_dam->annual_inflow==0){
        cur_dam->release=0;
        return;
    }
    
    release_coefficient = (double)cur_dam->storage_start_operation / (RES_PREF_STORAGE * (double)cur_dam->capacity);
    
    switch(cur_dam->function){
        case DAM_IRR_FUNCTION:
            if(cur_dam->annual_demand>0 && cur_dam->annual_inflow>(cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_PERC)){
                target_release = 
                        (cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_PERC) + 
                        (cur_dam->annual_inflow - (cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_PERC)) *
                        cur_dam->ext_influence_factor *
                        (cur_dam->monthly_demand / cur_dam->annual_demand) +
                        (cur_dam->annual_inflow - (cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_PERC)) *
                        (1-cur_dam->ext_influence_factor) *
                        (cur_dam->monthly_inflow/cur_dam->annual_inflow) *
                        (cur_dam->monthly_demand / cur_dam->annual_demand);
            }else{
                target_release = cur_dam->annual_inflow * cur_dam->ext_influence_factor + 
                        cur_dam->annual_inflow * (1-cur_dam->ext_influence_factor) * 
                        (cur_dam->monthly_inflow/cur_dam->annual_inflow);  
            }      
            break;
            
        case DAM_CON_FUNCTION:
            target_release = cur_dam->annual_inflow * cur_dam->ext_influence_factor + 
                    cur_dam->annual_inflow * (1-cur_dam->ext_influence_factor) * 
                    (cur_dam->monthly_inflow/cur_dam->annual_inflow);                       
            break;
            
        case DAM_HYD_FUNCTION:
            target_release = cur_dam->annual_inflow * cur_dam->ext_influence_factor + 
                    cur_dam->annual_inflow * (1-cur_dam->ext_influence_factor) * 
                    (cur_dam->monthly_inflow/cur_dam->annual_inflow);             
            break;        
        
        default:
            log_err("Dam found with unknown function");
            break;
    }
          
    cur_dam->release = target_release * release_coefficient;
    
    if(cur_dam->release<0){
        log_err("target release is negative?");
    }
    
    log_warn("\n\ntarget_release %.1f;\n"
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
        cur_dam->ext_influence_factor -= DAM_INFL_CHANGE;
    }else{
        cur_dam->ext_influence_factor += DAM_INFL_CHANGE;
    }
    
    if(cur_dam->ext_influence_factor<0.1){
        cur_dam->ext_influence_factor=0.1;
    }else if(cur_dam->ext_influence_factor>0.9){
        cur_dam->ext_influence_factor=0.9;
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
    int longest;
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
    longest=0;
    month=0;
    for(j=0;j<(2 * MONTHS_PER_YEAR);j++){
        size_t i = j % MONTHS_PER_YEAR;

        if(average_monthly_inflow[i]>cur_dam->annual_inflow){
            count++;

            if(count>longest){
                longest=count;
                month=i;

            }
        }else{
            count=0;
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
void calculate_actual_release(dam_unit* cur_dam, double *actual_release){
    double target_release = cur_dam->release;
    
    if(target_release<cur_dam->current_storage){
        *actual_release=target_release;
    }else{
        *actual_release=cur_dam->current_storage;
    }
}

/******************************************************************************
 * @section brief
 *  
 * Irrigate cells with an irrigation demand equally with available irrigation 
 * water.
 ******************************************************************************/
void do_dam_irrigation(dam_unit* cur_dam, double *actual_release, double *irrigation_release){
    extern all_vars_struct *all_vars;
    extern veg_con_struct **veg_con;
    extern domain_struct local_domain;
    extern option_struct options;
    extern double ***out_data;
        
    double **irrigation_crop=0;         //m3 (per crop)
    double *irrigation_cell=0;          //m3 (per cell)
    double irrigation_cells=0;          //m3
    
    double available_water=0;           //m3
    double demand_cells=0;              //m3
    
    size_t i;
    size_t j;
    size_t k;
    
    available_water = *actual_release - cur_dam->monthly_inflow_natural * DAM_ENV_FLOW_PERC;     
    
    if(available_water <= 0){
        *irrigation_release=0;
        return;
    }    
    
    for(i=0;i<cur_dam->nr_serviced_cells;i++){
        irr_cell *irr_cell = cur_dam->serviced_cells[i].cell->irr;
        
        for(j=0;j<irr_cell->nr_crops;j++){  
            if(cur_dam->serviced_cells[i].demand_crop[j]<=0){
                continue;
            }
            
            demand_cells += cur_dam->serviced_cells[i].demand_crop[j];
        }
    }
        
    if(demand_cells<=0){
        *irrigation_release=0;
        return;
    }

    irrigation_cell = malloc(cur_dam->nr_serviced_cells * sizeof(*irrigation_cell));
    check_alloc_status(irrigation_cell,"Memory allocation error");
    irrigation_crop = malloc(cur_dam->nr_serviced_cells * sizeof(*irrigation_crop));
    check_alloc_status(irrigation_crop,"Memory allocation error");
        
    for(i=0;i<cur_dam->nr_serviced_cells;i++){
        irr_cell *irr_cell = cur_dam->serviced_cells[i].cell->irr;
                
        irrigation_crop[i] = malloc(irr_cell->nr_crops * sizeof(*irrigation_crop[i]));
        check_alloc_status(irrigation_crop[i],"Memory allocation error");
        
        irrigation_cell[i]=0;
        for(j=0;j<irr_cell->nr_crops;j++){
            irrigation_crop[i][j]=0;            
        }
        
        for(j=0;j<irr_cell->nr_crops;j++){
            if(cur_dam->serviced_cells[i].demand_crop[j]<=0){
                continue;
            }
            
            if(demand_cells<available_water){
                irrigation_crop[i][j] = cur_dam->serviced_cells[i].demand_crop[j];
            }else{
                irrigation_crop[i][j] = cur_dam->serviced_cells[i].demand_crop[j] * (available_water/demand_cells);
            }
        }
        
        for(j=0;j<irr_cell->nr_crops;j++){
            if(cur_dam->serviced_cells[i].demand_crop[j]<=0){
                continue;
            }
            
            available_water -= irrigation_crop[i][j];
            cur_dam->serviced_cells[i].demand_crop[j]-=irrigation_crop[i][j];
            irrigation_cell[i] += irrigation_crop[i][j];
            irrigation_cells += irrigation_crop[i][j];
        }
        
        out_data[irr_cell->cell->id][OUT_DAM_IRR][0] = irrigation_cell[i] / M3_PER_HM3;
        
        if(irrigation_cell[i]<0){
            log_err("Adding negative amount of irrigation water");
        }
        
        for(j=0;j<irr_cell->nr_crops;j++){
            if(cur_dam->serviced_cells[i].demand_crop[j]<=0){
                continue;
            }
            
            cur_dam->serviced_cells[i].moisture_content[j] += irrigation_crop[i][j]/ 
                    (local_domain.locations[irr_cell->cell->id].area * 
                    veg_con[irr_cell->cell->id][irr_cell->veg_index[j]].Cv) * MM_PER_M;
            
            for(k=0;k<options.SNOW_BAND;k++){ 
                all_vars[irr_cell->cell->id].cell[irr_cell->veg_index[j]][k].layer[0].moist =
                        cur_dam->serviced_cells[i].moisture_content[j];
            }
        }
        
        free(irrigation_crop[i]);
    }
    
    *irrigation_release = irrigation_cells;
    *actual_release -= *irrigation_release;

    free(irrigation_crop);
    free(irrigation_cell);
}

/******************************************************************************
 * @section brief
 *  
 * Calculate the water deficit for all serviced cells
 ******************************************************************************/

void calculate_defict(dam_unit* cur_dam){
    extern double ***out_data;
    
    size_t i;
    size_t j;
    
    for(i=0;i<cur_dam->nr_serviced_cells;i++){
        irr_cell *irr_cell = cur_dam->serviced_cells[i].cell->irr;
        out_data[irr_cell->cell->id][OUT_DEMAND_END][0] = 0;
        
        for(j=0;j<irr_cell->nr_crops;j++){
            cur_dam->serviced_cells[i].deficit[j] = cur_dam->serviced_cells[i].demand_crop[j];             
            cur_dam->serviced_cells[i].demand_crop[j]=0;
            
            out_data[irr_cell->cell->id][OUT_DEMAND_END][0] += cur_dam->serviced_cells[i].deficit[j];
        }
    }
}

/******************************************************************************
 * @section brief
 *  
 * Release water and overflow to outflow and remove release, overflow and
 * irrigation from the dam storage.
 ******************************************************************************/
void do_dam_release(dam_unit* cur_dam, double actual_release, double irrigation_release){
    extern double ***out_data;
    
    double overflow=0;
    
    cur_dam->current_storage-= actual_release + irrigation_release;
    if(cur_dam->current_storage > cur_dam->capacity){
        overflow = cur_dam->current_storage - cur_dam->capacity;
    }
    cur_dam->current_storage-= overflow;
    
    cur_dam->previous_release = actual_release+overflow;
    
    if((cur_dam->current_storage/cur_dam->capacity)>0.9 || (cur_dam->current_storage/cur_dam->capacity)<0.1){
        cur_dam->extreme_stor++;
    }       
    out_data[cur_dam->cell->id][OUT_DAM_STORE][0]+= cur_dam->current_storage/cur_dam->capacity;
}

