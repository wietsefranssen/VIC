/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vic_def.h>
#include <rout.h>
#include <assert.h>
#include <vic_driver_image.h>


void rout_run(size_t time_step){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern double ***out_data;
    double inflow;
    double runoff;
    
    size_t current_rank;
    
    //goes through cells from upstream to downstream;
    for(current_rank=0;current_rank<global_domain.ncells_active;current_rank++){
        rout_cell *current_cell = rout.sorted_cells[current_rank];
        
        //determine runoff and inflow
        runoff = out_data[current_cell->id][OUT_RUNOFF][0]+out_data[current_cell->id][OUT_BASEFLOW][0];

        inflow=0.0;
        int i;
        for(i=0;i<current_cell->nr_upstream;i++){
            inflow += current_cell->upstream[i]->outflow[time_step];
        }

        //do irrigation with inflow!
        //subtract from inflow

        //convolute runoff and inflow to future
        size_t t;
        for(t=0;t<UH_MAX_DAYS * global_param.model_steps_per_day;t++){
            current_cell->outflow[t]+=current_cell->uh[t] * runoff;
            current_cell->outflow[t]+=current_cell->uh[t] * inflow;
        }
        
        //write data and shift array
        out_data[current_cell->id][OUT_DISCHARGE][0] = current_cell->outflow[0];
        
        for(t=0;t<(UH_MAX_DAYS * global_param.model_steps_per_day)-1;t++){
            *(current_cell->outflow + t) = *(current_cell->outflow + (t+1));
        }
        *(current_cell->outflow + t) = 0.0;
    }
}