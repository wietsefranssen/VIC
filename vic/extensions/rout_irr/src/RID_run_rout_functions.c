/******************************************************************************
 * @section DESCRIPTION
 *  
 * Functions used in rout_run
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Gather both runoff and base-flow from VIC and add the inflow from
 * upstream cells. Both natural and normal flows
 ******************************************************************************/

void gather_runoff_inflow(RID_cell *cur_cell, double *runoff, double *inflow, bool naturalized){
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern double ***out_data;
    
    size_t i;
    
    *runoff = (out_data[cur_cell->id][OUT_RUNOFF][0]+out_data[cur_cell->id][OUT_BASEFLOW][0]) 
            * local_domain.locations[cur_cell->id].area / MM_PER_M / global_param.dt;

    *inflow=0.0;
    if(naturalized){
        for(i=0;i<cur_cell->rout->nr_upstream;i++){
            *inflow += cur_cell->rout->upstream[i]->outflow_natural[0];
        }
    }else{
        for(i=0;i<cur_cell->rout->nr_upstream;i++){
            *inflow += cur_cell->rout->upstream[i]->outflow[0];
        }
    }
    
    out_data[cur_cell->id][OUT_RUNOFF_ROUT][0] = *runoff;
    out_data[cur_cell->id][OUT_INFLOW_ROUT][0] = *inflow;
}

/******************************************************************************
 * @section brief
 *  
 * Shift the outflow array of a cell to the next time-step
 ******************************************************************************/

void shift_outflow_array(RID_cell* current_cell){
    extern global_param_struct global_param;
    
    size_t t;                
    
    for(t=0;t<(MAX_UH_DAYS * global_param.model_steps_per_day)-1;t++){
        *(current_cell->rout->outflow + t) = *(current_cell->rout->outflow + (t+1));
        *(current_cell->rout->outflow_natural + t) = *(current_cell->rout->outflow_natural + (t+1));
    }
    
    *(current_cell->rout->outflow + t) = 0.0;
    *(current_cell->rout->outflow_natural + t) = 0.0;
}

/******************************************************************************
 * @section brief
 *  
 * Rout the runoff and inflow based on the unit hydro-graph of the cell.
 * Both natural and normal flows
 ******************************************************************************/

void do_routing(RID_cell* cur_cell, double runoff, double inflow, bool naturalized){
    extern global_param_struct global_param;
    
    size_t t;
    
    if(!naturalized){
        for(t=0;t<MAX_UH_DAYS * global_param.model_steps_per_day;t++){
            cur_cell->rout->outflow[t] += cur_cell->rout->uh[t] * (inflow+runoff);
        }       
    }else{
        for(t=0;t<MAX_UH_DAYS * global_param.model_steps_per_day;t++){
            cur_cell->rout->outflow_natural[t] += cur_cell->rout->uh[t] * (inflow+runoff);
        }
    }
}