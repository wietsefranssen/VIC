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
    extern double ***out_data;
    double inflow;
    double runoff;
    
    size_t current_rank;
    for(current_rank=0;current_rank<(rout.x_size*rout.y_size);current_rank++){
        rout_cell *current_cell = rout.ranked_cells[current_rank];
        if(current_cell!=NULL){
            //goes through cells from upstream to downstream;
                                
            runoff = out_data[current_cell->vic_id][OUT_RUNOFF][0]+out_data[current_cell->vic_id][OUT_BASEFLOW][0];
            
            inflow=0.0;
            int i;
            for(i=0;i<current_cell->nr_upstream;i++){
                inflow += current_cell->upstream[i]->outflow[time_step];
            }
            
            //do irrigation with inflow!
            //subtract from inflow
            
            //convolute runoff and inflow to future
            size_t t;
            size_t tmax = time_step + rout.uh_length;
            if(tmax>rout.total_time){
                tmax=rout.total_time;
            }
            for(t=time_step;t<=tmax;t++){
                current_cell->outflow[t]+=current_cell->uh[t-time_step] * runoff;
                current_cell->outflow[t]+=current_cell->uh[t-time_step] * inflow;
            }
        }
    }
}