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
    
    double runoff = 0.0; //m^3s
    double inflow = 0.0; //m^3s
        
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
    extern double ***out_data;
    
    double moisture_content[cur_cell->irr->nr_crops];         //mm (per crop)
    double demand_crop[cur_cell->irr->nr_crops];              //m3 (per crop)
    double demand_cell;                                       //m3
    size_t i;
    
    demand_cell=0;
    for(i=0;i<cur_cell->irr->nr_crops;i++){
        moisture_content[i]=0;
        demand_crop[i]=0;
    }
    
    for(i=0;i<cur_cell->irr->nr_crops;i++){
        if(!in_irrigation_season(cur_cell->irr->crop_index[i],cur_dmy->day_in_year)){
            continue;
        }
        
        get_moisture_content(cur_cell->id,cur_cell->irr->veg_index[i],&moisture_content[i]);
        get_irrigation_demand(cur_cell->id,cur_cell->irr->veg_index[i],moisture_content[i],&demand_crop[i]);
        demand_cell += demand_crop[i];
    }
    
    if(demand_cell<=0){
        update_servicing_dam_values(cur_cell,demand_crop,moisture_content);
        return;
    }
    
    out_data[cur_cell->id][OUT_DEMAND_START][0]=demand_cell / M3_PER_HM3;        
    do_irrigation(cur_cell,demand_crop,&demand_cell,moisture_content);    
    out_data[cur_cell->id][OUT_DEMAND_END][0]=demand_cell / M3_PER_HM3;
    
    update_servicing_dam_values(cur_cell,demand_crop,moisture_content);
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
    extern double ***out_data;
    
    cur_dam->total_inflow += cur_dam->cell->rout->outflow[0] * global_param.dt;
    if(RID.param.fnaturalized_flow){
        cur_dam->total_inflow_natural+=cur_dam->cell->rout->outflow_natural[0] * global_param.dt;
    }
    
    if(cur_dam->run){
        cur_dam->current_storage += cur_dam->cell->rout->outflow[0] * global_param.dt;        
        cur_dam->cell->rout->outflow[0] = cur_dam->previous_release / global_param.dt;
        out_data[cur_dam->cell->id][OUT_DISCHARGE][0]=cur_dam->previous_release / global_param.dt; 
        cur_dam->previous_release=0;
    }
}

/******************************************************************************
 * @section brief
 *  
 * Update dam history and calculate target release every operational month.
 * Recalculate operational year every operational year. Redistribution of water
 * based on target release.
 ******************************************************************************/

void do_dam_module(dam_unit *cur_dam, dmy_struct *cur_dmy){
    extern global_param_struct global_param;
    
    double actual_release=0;
    double irrigation_release=0;
    
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
    
    if(!cur_dam->run){
        if(cur_dmy->year>=cur_dam->activation_year){
            cur_dam->run=true;
        }else{
            return;
        }
    }
    
    calculate_actual_release(cur_dam,&actual_release);
    
    if(cur_dam->function==DAM_IRR_FUNCTION){
        if(actual_release>0){
            do_dam_irrigation(cur_dam,&actual_release, &irrigation_release);
        }else{
            irrigation_release=0;
        }
        
        calculate_defict(cur_dam);
    }
    
    do_dam_release(cur_dam,actual_release,irrigation_release);
}