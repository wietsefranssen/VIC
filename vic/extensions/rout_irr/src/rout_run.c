/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vic_def.h>
#include <rout.h>
#include <assert.h>
#include <vic_driver_image.h>
#include <math.h>


void rout_run(){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern double ***out_data;        
    extern soil_con_struct *soil_con;
    extern all_vars_struct *all_vars;
    extern veg_con_struct **veg_con;
    extern option_struct options;
    
    double runoff; //m^3
    double inflow; //m^3
    
    size_t current_rank;
    
    //goes through cells from upstream to downstream;
    for(current_rank=0;current_rank<global_domain.ncells_active;current_rank++){
        rout_cell *current_cell = rout.sorted_cells[current_rank];
        
        //shift outflow array
        size_t t;                
        for(t=0;t<(UH_MAX_DAYS * global_param.model_steps_per_day)-1;t++){
            *(current_cell->outflow + t) = *(current_cell->outflow + (t+1));
        }
        *(current_cell->outflow + t) = 0.0;
        
        //determine runoff and inflow
        runoff = (out_data[current_cell->local_id][OUT_RUNOFF][0]+out_data[current_cell->local_id][OUT_BASEFLOW][0]) 
                * rout.cells[current_cell->local_id].location->area / (MM_PER_M * global_param.dt);

        inflow=0.0;        
        int c;
        for(c=0;c<current_cell->nr_upstream;c++){
            inflow += current_cell->upstream[c]->outflow[0];
        }
        
        //################local irrigation####################################
        //FIXME: make this dynamic, probably should be in options       
        size_t irrig_veg_class_size = 3;
        size_t i;
        int irrig_veg_class[3] = {1,2,3};
        
        double available_water = inflow * global_param.dt; //m^3
        
        double *irrigation_demand;
        if((irrigation_demand = malloc(irrig_veg_class_size * sizeof(*irrigation_demand)))==NULL){
            log_err("Memory allocation for irrigation_demand failed!");
        }
        
        double irrigation_demand_sum = 0.0;
        
        double **moisture_content;
        if((moisture_content = malloc(irrig_veg_class_size * sizeof(*moisture_content)))!=NULL){
            for(i=0;i<irrig_veg_class_size;i++){
                if((moisture_content[i] = malloc(options.SNOW_BAND * sizeof(*moisture_content[i])))==NULL){
                    log_err("Memory allocation for moisture_content failed!");
                }
            }
        }else{
            log_err("Memory allocation for *moisture_content failed!");
        }
        
        double *moisture_sum;
        if((moisture_sum = malloc(irrig_veg_class_size * sizeof(*moisture_sum)))==NULL){
            log_err("Memory allocation for moisture_sum failed!");
        }
        
        double field_capacity;
        double irrigation_point;
        
        out_data[current_cell->local_id][OUT_IRR][0] = 0.0;
        out_data[current_cell->local_id][OUT_IRR_DEMAND][0]=0.0;
        
        size_t iVeg;
        for(i=0;i<irrig_veg_class_size;i++){
            iVeg = irrig_veg_class[i];
            
            if(iVeg<veg_con[current_cell->local_id][0].vegetat_type_num){
                double current_area = veg_con[current_cell->local_id][iVeg].Cv * current_cell->location->area;
                
                irrigation_point = soil_con[current_cell->local_id].Wcr[0] * current_area / MM_PER_M; //m^3
                field_capacity = soil_con[current_cell->local_id].max_moist[0] * current_area / MM_PER_M; //m^3
                
                moisture_sum[i] = 0.0;
                
                size_t j;
                for(j=0;j<options.SNOW_BAND;j++){
                    moisture_content[i][j] = all_vars[current_cell->local_id].cell[iVeg][j].layer[0].moist * current_area / MM_PER_M; //m^3
                    moisture_sum[i] += moisture_content[i][j];
                }
                
                if(moisture_sum[i] < irrigation_point){
                    irrigation_demand[i] = field_capacity - moisture_sum[i]; //m^3
                    irrigation_demand_sum += irrigation_demand[i];
                }else{
                    irrigation_demand[i] = 0.0;
                }
                
                out_data[current_cell->local_id][OUT_IRR_DEMAND][0] +=irrigation_demand[i];
            }
        }
        
        for(i=0;i<irrig_veg_class_size;i++){
            iVeg = irrig_veg_class[i];
            
            if(iVeg<veg_con[current_cell->local_id][0].vegetat_type_num){
                double current_area = veg_con[current_cell->local_id][iVeg].Cv * current_cell->location->area;                
                
                double added_water = 0.0;
                //double orig_irrigation_demand = irrigation_demand[i];

                if(irrigation_demand[i]>0){
                    if(available_water * (irrigation_demand[i] / irrigation_demand_sum) > irrigation_demand[i]){
                        //get irrigation water from local inflow

                        added_water += irrigation_demand[i];
                        irrigation_demand[i] = 0.0;

                    }else{
                        //reduce local inflow_m2

                        added_water += available_water * (irrigation_demand[i] / irrigation_demand_sum);
                        irrigation_demand[i] -= added_water;

                    }
                }
                
                out_data[current_cell->local_id][OUT_IRR][0] += added_water;
                
                double new_moisture_sum = 0.0;
                size_t j;
                for(j=0;j<options.SNOW_BAND;j++){
                   all_vars[current_cell->local_id].cell[iVeg][j].layer[0].moist += ((moisture_content[i][j]/moisture_sum[i]) * added_water) / current_area * MM_PER_M; //mm
                   new_moisture_sum +=  all_vars[current_cell->local_id].cell[iVeg][j].layer[0].moist;
                }
                
                inflow-=added_water;
            }
        }
        
        //################################################################

        //convolute runoff and inflow to future
        current_cell->outflow[0] += runoff;
        if(inflow > 0){
            for(t=0;t<UH_MAX_DAYS * global_param.model_steps_per_day;t++){
                current_cell->outflow[t]+=current_cell->uh[t] * inflow;
            }
        }
    }
    
    size_t i;
    for(i=0;i<global_domain.ncells_active;i++){
        //write data
        out_data[rout.cells[i].local_id][OUT_DISCHARGE][0] += rout.cells[i].outflow[0];
    }
}