/******************************************************************************
 * @section DESCRIPTION
 *  
 * The main function running the routing, irrigation and dam module
 ******************************************************************************/

#include <vic_def.h>
#include <rout.h>
#include <assert.h>
#include <vic_driver_image.h>
#include <math.h>

/******************************************************************************
 * @section brief
 *  
 * The main function running the routing, irrigation and dam module
 ******************************************************************************/
void RID_run(dmy_struct* cur_dmy){
    extern domain_struct global_domain;
    extern RID_struct RID;
    extern double ***out_data;
    
    RID_cell *cur_cell;
    size_t i;
        
    for(i=0;i<global_domain.ncells_active;i++){
        cur_cell = RID.sorted_cells[i];
        
        do_routing_module(cur_cell);
        
        if(cur_cell->irr!=NULL){
            do_irrigation_module(cur_cell,cur_dmy);
        }
        
        if(cur_cell->dam!=NULL){
            do_dam_flow(cur_cell->dam);
        }
        
        out_data[cur_cell->id][OUT_DISCHARGE][0]=cur_cell->rout->outflow[0]; 
        out_data[cur_cell->id][OUT_NATURAL_DISCHARGE][0]=cur_cell->rout->outflow_natural[0];        
    }
    
    for(i=0;i<RID.nr_dams;i++){
        do_dam_history_module(&RID.dams[i], cur_dmy);
        do_dam_module(&RID.dams[i], cur_dmy);
    }
}

/******************************************************************************
 * @section brief
 *  
 * The main function running the routing module. Gathers inflow from this and
 * upstream cells, and uses unit hydro-graphs to rout the water.
 ******************************************************************************/
void do_routing_module(RID_cell *cur_cell){
    extern RID_struct RID;
    
    double runoff = 0.0;                                //m^3s
    double inflow = 0.0;                                //m^3s
        
    gather_runoff_inflow(cur_cell, &runoff, &inflow, false);
    shift_outflow_array(cur_cell);
    do_routing(cur_cell, runoff, inflow, false);

    if(RID.param.fnaturalized_flow){
        do_routing(cur_cell,runoff, inflow, true);
    }
}

/******************************************************************************
 * @section brief
 *  
 * The main function running the irrigation module. Check whether irrigation
 * is needed, gather water from local sources. Distribute leftover demands
 * to servicing dams.
 ******************************************************************************/
void do_irrigation_module(RID_cell *cur_cell, dmy_struct *cur_dmy){
    extern global_param_struct global_param;
    extern double ***out_data;
    
    double moisture_content[cur_cell->irr->nr_crops];         //mm (per crop)
    double demand_crop[cur_cell->irr->nr_crops];              //m3 (per crop)
    double demand_cell;                                       //m3
    double irrigation_crop[cur_cell->irr->nr_crops];          //m3 (per crop)
    double irrigation_cell;                                   //m3
    double available_water;                                   //m3
    size_t i;
    
    demand_cell=0;
    irrigation_cell=0;    
    for(i=0;i<cur_cell->irr->nr_crops;i++){
        moisture_content[i]=0;
        demand_crop[i]=0;
        irrigation_crop[i]=0;
    }
    
    for(i=0;i<cur_cell->irr->nr_crops;i++){
        if(!in_irrigation_season(cur_cell->irr->crop_index[i],cur_dmy->day_in_year)){
            continue;
        }
        
        get_moisture_content(cur_cell->id,cur_cell->irr->veg_index[i],&moisture_content[i]);
        get_irrigation_demand(cur_cell->id,cur_cell->irr->veg_index[i],moisture_content[i],&demand_crop[i]);
        demand_cell += demand_crop[i];
    }
    
    out_data[cur_cell->id][OUT_DEMAND_START][0] = demand_cell / M3_PER_HM3;
    available_water = cur_cell->rout->outflow[0] * global_param.dt;
    
    if(available_water>1 && demand_cell>1){
        for(i=0;i<cur_cell->irr->nr_crops;i++){
            if(!in_irrigation_season(cur_cell->irr->crop_index[i],cur_dmy->day_in_year)){
                continue;
            }

            get_irrigation(&irrigation_crop[i],demand_cell,demand_crop[i],available_water);
        }
        
        for(i=0;i<cur_cell->irr->nr_crops;i++){
            if(!in_irrigation_season(cur_cell->irr->crop_index[i],cur_dmy->day_in_year)){
                continue;
            }
            
            update_demand_and_irrigation(&irrigation_cell, &irrigation_crop[i], &demand_cell, &demand_crop[i], &available_water);
        }
        
        for(i=0;i<cur_cell->irr->nr_crops;i++){
            if(!in_irrigation_season(cur_cell->irr->crop_index[i],cur_dmy->day_in_year)){
                continue;
            }
            
            do_irrigation(cur_cell->id,cur_cell->irr->veg_index[i], &moisture_content[i],irrigation_crop[i]);
        }
    }
    
    cur_cell->rout->outflow[0]= available_water / global_param.dt;
    
    if(cur_cell->irr->serviced_cell!=NULL){
        for(i=0;i<cur_cell->irr->nr_crops;i++){
            update_servicing_dam_values(cur_cell->irr->serviced_cell,i,moisture_content[i],demand_crop[i]);
        }        
    }
    
    out_data[cur_cell->id][OUT_LOCAL_IRR][0]=irrigation_cell / M3_PER_HM3;
    out_data[cur_cell->id][OUT_IRR][0]=out_data[cur_cell->id][OUT_LOCAL_IRR][0];
    out_data[cur_cell->id][OUT_DEMAND_END][0]=out_data[cur_cell->id][OUT_DEMAND_DAM][0];
    
}

/******************************************************************************
 * @section brief
 *  
 * Outflow of a cell is stored in the reservoir of the dam and previously 
 * calculated release is set as the outflow
 ******************************************************************************/
void do_dam_flow(dam_unit *cur_dam){
    extern RID_struct RID;
    extern global_param_struct global_param;
    
    cur_dam->total_inflow += cur_dam->cell->rout->outflow[0] * global_param.dt;
    if(RID.param.fnaturalized_flow){
        cur_dam->total_inflow_natural+=cur_dam->cell->rout->outflow_natural[0] * global_param.dt;
    }
    
    if(cur_dam->run){
        cur_dam->current_storage += cur_dam->cell->rout->outflow[0] * global_param.dt;        
        cur_dam->cell->rout->outflow[0] = cur_dam->previous_release / global_param.dt;
        cur_dam->previous_release=0;
    }
}

/******************************************************************************
 * @section brief
 *  
 * Update dam history and calculate target release every operational month.
 * Recalculate operational year every operational year.
 ******************************************************************************/

void do_dam_history_module(dam_unit *cur_dam, dmy_struct *cur_dmy){
    extern global_param_struct global_param;
    
    /*******************************
     Operational day has passed
    *******************************/
    update_dam_history_day(cur_dam,cur_dmy);
    
    if(cur_dmy->dayseconds==0 && cur_dmy->day==cur_dam->start_operation.day){  
        /*******************************
         Operational month has passed
        *******************************/
        update_dam_history_month(cur_dam,cur_dmy);
        calculate_target_release(cur_dam);

        if(cur_dmy->month == cur_dam->start_operation.month && cur_dmy->year > global_param.startyear){
            /*******************************
             Operational year has passed
            *******************************/
            update_dam_history_year(cur_dam,cur_dmy);
            calculate_operational_year(cur_dam, cur_dmy);
        }
    }
}

/******************************************************************************
 * @section brief
 *  
 * Redistribution and release of water based on target release and irrigation
 * demand.
 ******************************************************************************/
void do_dam_module(dam_unit *cur_dam, dmy_struct *cur_dmy){
    extern double ***out_data;
    
    double actual_release;
    double **irrigation_crop;
    double *irrigation_cell;
    double irrigation_cells;
    double *demand_cell;
    double demand_cells;
    double available_water;
    double overflow;
    double evaporation;
    
    size_t i;
    size_t j;
    
    if(!cur_dam->run){
        if(cur_dmy->year>=cur_dam->activation_year){
            cur_dam->run=true;
        }
    }
    
    if(cur_dam->run){
    
        actual_release=0.0;
        overflow=0.0;
        evaporation=0.0;
        irrigation_cells=0.0;
        demand_cells=0.0;
                    
        get_actual_release(cur_dam,&actual_release);

        if(cur_dam->function==DAM_IRR_FUNCTION){
            irrigation_cell = malloc(cur_dam->nr_serviced_cells * sizeof(*irrigation_cell));
            check_alloc_status(irrigation_cell,"Memory allocation error");
            demand_cell = malloc(cur_dam->nr_serviced_cells * sizeof(*demand_cell));
            check_alloc_status(demand_cell,"Memory allocation error");
            irrigation_crop = malloc(cur_dam->nr_serviced_cells * sizeof(*irrigation_crop));
            check_alloc_status(irrigation_crop,"Memory allocation error");
            
            for(i=0;i<cur_dam->nr_serviced_cells;i++){                
                irrigation_crop[i] = malloc(cur_dam->nr_serviced_cells * sizeof(*irrigation_crop[i]));
                check_alloc_status(irrigation_crop[i],"Memory allocation error");
            }
            
            for(i=0;i<cur_dam->nr_serviced_cells;i++){
                irrigation_cell[i]=0;
                demand_cell[i]=0;
                for(j=0;j<cur_dam->serviced_cells[i].cell->nr_crops;j++){
                    irrigation_crop[i][j]=0;
                }
            }
        
            available_water = actual_release - cur_dam->environmental_release;
            if(available_water<0){
                available_water=0.0;
            }
            
            for(i=0;i<cur_dam->nr_serviced_cells;i++){
                get_demand_cells(&cur_dam->serviced_cells[i],&demand_cells,&demand_cell[i]);
                
                out_data[cur_dam->serviced_cells[i].cell->cell->id][OUT_DEMAND_DAM][0]=demand_cell[i]/M3_PER_HM3;
            }
            
            if(demand_cells>0 && available_water>0){
                for(i=0;i<cur_dam->nr_serviced_cells;i++){
                    for(j=0;j<cur_dam->serviced_cells[i].cell->nr_crops;j++){                    
                        get_dam_irrigation(demand_cells,cur_dam->serviced_cells[i].demand_crop[j], &irrigation_crop[i][j],available_water);
                    }
                }
                
                for(i=0;i<cur_dam->nr_serviced_cells;i++){
                    for(j=0;j<cur_dam->serviced_cells[i].cell->nr_crops;j++){                    
                        update_dam_demand_and_irrigation(&demand_cells,&demand_cell[i],&cur_dam->serviced_cells[i].demand_crop[j],
                                &irrigation_cells,&irrigation_cell[i],irrigation_crop[i][j],&available_water);
                    }
                }
                
                for(i=0;i<cur_dam->nr_serviced_cells;i++){
                    for(j=0;j<cur_dam->serviced_cells[i].cell->nr_crops;j++){                    
                        do_dam_irrigation(cur_dam->serviced_cells[i].cell->cell->id,cur_dam->serviced_cells[i].cell->veg_index[j],
                                &cur_dam->serviced_cells[i].moisture_content[j],irrigation_crop[i][j]);
                    }
                }                
            }
            
            for(i=0;i<cur_dam->nr_serviced_cells;i++){
                
                out_data[cur_dam->serviced_cells[i].cell->cell->id][OUT_DEMAND_END][0]=demand_cell[i]/M3_PER_HM3;
                out_data[cur_dam->serviced_cells[i].cell->cell->id][OUT_DAM_IRR][0]=irrigation_cell[i]/M3_PER_HM3;
                out_data[cur_dam->serviced_cells[i].cell->cell->id][OUT_IRR][0]+=out_data[cur_dam->serviced_cells[i].cell->cell->id][OUT_DAM_IRR][0];
                
                for(j=0;j<cur_dam->serviced_cells[i].cell->nr_crops;j++){                    
                    set_deficit(&cur_dam->serviced_cells[i].deficit[j],&cur_dam->serviced_cells[i].demand_crop[j]);
                }
            }
            
            for(i=0;i<cur_dam->nr_serviced_cells;i++){
                free(irrigation_crop[i]);
            }
            free(irrigation_cell);
            free(irrigation_crop);
            free(demand_cell);
        }
        
        actual_release -= irrigation_cells;
        
        get_dam_evaporation(cur_dam, &evaporation);
        get_dam_overflow(cur_dam, &overflow, actual_release, irrigation_cells, evaporation);
        do_dam_release(cur_dam,actual_release,irrigation_cells,overflow,evaporation);
        
        out_data[cur_dam->cell->id][OUT_DAM_STORE][0]=cur_dam->current_storage/cur_dam->capacity;
        out_data[cur_dam->cell->id][OUT_DAM_EVAP][0]=evaporation;
    }
}