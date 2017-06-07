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
    extern int mpi_rank;
    
    RID_cell *cur_cell;
    size_t i;

    if (mpi_rank == VIC_MPI_ROOT) {        
        for(i=0;i<global_domain.ncells_active;i++){
            cur_cell = RID.sorted_cells[i];

            do_routing_module(cur_cell);

            if(cur_cell->irr!=NULL){
                do_irrigation_module(cur_cell->irr,cur_dmy);

                if(cur_cell->irr->servicing_dam!=NULL){
                    do_dam_demand(cur_cell->irr);
                }
            }

            if(cur_cell->dam!=NULL){
                do_dam_flow(cur_cell->dam);
            }

            out_data[cur_cell->id][OUT_DISCHARGE][0]=cur_cell->rout->outflow[0]; 
            out_data[cur_cell->id][OUT_NATURAL_DISCHARGE][0]=cur_cell->rout->outflow_natural[0]; 
            out_data[cur_cell->id][OUT_IRR][0]=+out_data[cur_cell->id][OUT_LOCAL_IRR][0]; 
        }

        for(i=0;i<RID.nr_dams;i++){
            do_dam_history_module(&RID.dams[i], cur_dmy);
            do_dam_module(&RID.dams[i], cur_dmy);
        }
    }
}

/******************************************************************************
 * @section brief
 *  
 * The main function running the routing module. Gathers inflow from this and
 * upstream cells, and uses unit hydro-graphs to rout the water.
 ******************************************************************************/
void do_routing_module(RID_cell *cur_cell){
    extern global_param_struct global_param;
    
    double runoff = 0.0;                                //m^3s
    double inflow = 0.0;                                //m^3s
        
    gather_runoff_inflow(cur_cell, &runoff, &inflow, false);
    shift_outflow_array(cur_cell);
    do_routing(cur_cell, runoff, inflow, false);

    if(global_param.fnaturalized_flow){
        gather_runoff_inflow(cur_cell, &runoff, &inflow, true);
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
void do_irrigation_module(irr_cell *cur_irr, dmy_struct *cur_dmy){
    extern global_param_struct global_param;
    extern double ***out_data;
    
    bool run [cur_irr->nr_crops];
    double infiltration;                                      //mm
    double demand_cell;                                       //m3
    double irrigation_crop[cur_irr->nr_crops];                //m3 (per crop)
    double irrigation_cell;                                   //m3
    double available_water;                                   //m3
    size_t i;
    
    demand_cell=0;
    irrigation_cell=0;  
    for(i=0;i<cur_irr->nr_crops;i++){
        irrigation_crop[i]=0;  
    }
        
    for(i=0;i<cur_irr->nr_crops;i++){
        run[i]=true;
        get_moisture_content(cur_irr->cell->id,cur_irr->veg_index[i],&cur_irr->moisture[i]);
        get_storage_infiltration(cur_irr->cell->id,&cur_irr->storage[i],&infiltration,cur_irr->moisture[i]);
        increase_moisture_content(cur_irr->cell->id,cur_irr->veg_index[i],&cur_irr->moisture[i],infiltration); 
    }    
    available_water = cur_irr->cell->rout->outflow[0] * global_param.dt * AVAILABLE_IRR_FRAC;
    
    out_data[cur_irr->cell->id][OUT_CROP_STORE][0] = cur_irr->storage[0]; 
      
    for(i=0;i<cur_irr->nr_crops;i++){
        if(!in_irrigation_season(cur_irr->crop_index[i],cur_dmy->day_in_year)){  
            run[i]=false;    
            cur_irr->demand[i]=0.0;
            cur_irr->deficit[i]=0.0;
        }   
    }
        
    for(i=0;i<cur_irr->nr_crops;i++){
        if(!run[i]){
            continue;
        }        
        
        get_demand(cur_irr,cur_irr->veg_index[i],&cur_irr->demand[i],cur_irr->storage[i]);
        demand_cell += cur_irr->demand[i];         
    }
    
    out_data[cur_irr->cell->id][OUT_DEMAND][0] = demand_cell / M3_PER_HM3;
    
    cur_irr->cell->rout->outflow[0] -= available_water / global_param.dt;    
    if(available_water>1 && demand_cell>1){
        for(i=0;i<cur_irr->nr_crops;i++){
            if(!run[i]){
                continue;
            }
            
            get_irrigation(&irrigation_crop[i],demand_cell,cur_irr->demand[i],available_water);       
            increase_storage_content(cur_irr->cell->id,cur_irr->veg_index[i], &cur_irr->storage[i], irrigation_crop[i]);
            irrigation_cell += irrigation_crop[i];
        }
        
        for(i=0;i<cur_irr->nr_crops;i++){  
            if(!run[i]){
                continue;
            }          
            
            cur_irr->demand[i] -= irrigation_crop[i];
            available_water -= irrigation_crop[i];
        }
    }    
    cur_irr->cell->rout->outflow[0] += available_water / global_param.dt;
    
    out_data[cur_irr->cell->id][OUT_LOCAL_IRR][0]=irrigation_cell / M3_PER_HM3;    
    
    for(i=0;i<cur_irr->nr_crops;i++){    
        if(!run[i]){
            continue;
        }              
        set_deficit(&cur_irr->deficit[i],&cur_irr->demand[i]);
    }    
}

/******************************************************************************
 * @section brief
 *  
 * Irrigation demand of cells is stored in the dams history
 ******************************************************************************/
void do_dam_demand(irr_cell *cur_irr){    
    size_t i;
    
    for(i=0;i<cur_irr->nr_crops;i++){
        cur_irr->servicing_dam->total_demand += cur_irr->deficit[i];               
    }
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
    if(global_param.fnaturalized_flow){
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
                
    double monthly_inflow[MONTHS_PER_YEAR];
    double monthly_inflow_natural[MONTHS_PER_YEAR];
    double monthly_demand[MONTHS_PER_YEAR];
    double annual_inflow;
    double annual_inflow_natural;
    double annual_demand;
    
    if(cur_dmy->dayseconds==0){        
        if(cur_dmy->day==cur_dam->start_operation.day){  
            /*******************************
             Operational month has passed
            *******************************/
            
            update_dam_history(cur_dam,cur_dmy);

            if(cur_dmy->month == cur_dam->start_operation.month && cur_dmy->year > global_param.startyear){
                /*******************************
                 Operational year has passed
                *******************************/
                
                get_multi_year_average(cur_dam, cur_dmy,
                        monthly_inflow,monthly_inflow_natural,monthly_demand,
                        &annual_inflow,&annual_inflow_natural,&annual_demand);
                calculate_dam_release(cur_dam,cur_dmy,
                        monthly_inflow,monthly_inflow_natural,
                        annual_inflow,annual_inflow_natural);
                calculate_operational_year(cur_dam, cur_dmy,
                        monthly_inflow, annual_inflow);
            }
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
                for(j=0;j<cur_dam->serviced_cells[i]->nr_crops;j++){
                    irrigation_crop[i][j]=0;
                }
            }
        
            available_water = actual_release - cur_dam->environmental_release;
            if(available_water<0){
                available_water=0.0;
            }
            
            for(i=0;i<cur_dam->nr_serviced_cells;i++){
                for(j=0;j<cur_dam->serviced_cells[i]->nr_crops;j++){   
                    cur_dam->serviced_cells[i]->deficit[j]=0.0;
                    get_demand_cells(&demand_cells,&demand_cell[i],cur_dam->serviced_cells[i]->demand[j]);
                }
            }
            
            if(demand_cells>0 && available_water>0){
                for(i=0;i<cur_dam->nr_serviced_cells;i++){
                    for(j=0;j<cur_dam->serviced_cells[i]->nr_crops;j++){                    
                        get_dam_irrigation(demand_cells,cur_dam->serviced_cells[i]->demand[j], &irrigation_crop[i][j],available_water);
                    }
                }
                
                for(i=0;i<cur_dam->nr_serviced_cells;i++){
                    for(j=0;j<cur_dam->serviced_cells[i]->nr_crops;j++){                    
                        update_dam_demand_and_irrigation(&demand_cells,&demand_cell[i],&cur_dam->serviced_cells[i]->demand[j],
                                &irrigation_cells,&irrigation_cell[i],irrigation_crop[i][j],&available_water);
                    }
                }
                
                for(i=0;i<cur_dam->nr_serviced_cells;i++){
                    for(j=0;j<cur_dam->serviced_cells[i]->nr_crops;j++){                    
                        do_dam_irrigation(cur_dam->serviced_cells[i]->cell->id,cur_dam->serviced_cells[i]->veg_index[j],
                                &cur_dam->serviced_cells[i]->moisture[j],irrigation_crop[i][j]);
                    }
                }                
            }
            
            for(i=0;i<cur_dam->nr_serviced_cells;i++){
                out_data[cur_dam->serviced_cells[i]->cell->id][OUT_DAM_IRR][0]=irrigation_cell[i]/M3_PER_HM3;
                out_data[cur_dam->serviced_cells[i]->cell->id][OUT_IRR][0]=+out_data[cur_dam->serviced_cells[i]->cell->id][OUT_DAM_IRR][0]; 
                
                for(j=0;j<cur_dam->serviced_cells[i]->nr_crops;j++){                    
                    set_deficit(&cur_dam->serviced_cells[i]->deficit[j],&cur_dam->serviced_cells[i]->demand[j]);
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