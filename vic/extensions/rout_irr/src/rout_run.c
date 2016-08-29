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
    
    double runoff;
    
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
        runoff = out_data[current_cell->id][OUT_RUNOFF][0]+out_data[current_cell->id][OUT_BASEFLOW][0];

        double inflow=0.0; //m^3
        double total_area=0.0; //m^2
        
        int c;
        size_t iVeg;
        for(c=0;c<current_cell->nr_upstream;c++){
            
            for(iVeg=0;iVeg<veg_con[current_cell->id][0].vegetat_type_num;iVeg++){
                total_area += veg_con[current_cell->upstream[c]->id][iVeg].Cv;
            }
            
            inflow += current_cell->upstream[c]->outflow[0] * total_area / 1000;
        }
        
        log_info("cell %zu area %.2f inflow %.2f",current_cell->id,total_area,inflow);
        
        //################irrigation####################################
        
        double moisture_content=0.0;
        
        double irrigation_point = soil_con[current_cell->id].Wcr[0];
        double field_capacity = soil_con[current_cell->id].max_moist[0];
        int irrig_veg_class[3] = {1,2,3};
        
        size_t i;
        //for each vegetation that can use irrigation
        for(i=0;i<(sizeof(irrig_veg_class)/sizeof(irrig_veg_class[0]))+1;i++){
            iVeg = irrig_veg_class[i];
            
            //if irrigated vegetation exists in the cell
            if(iVeg<veg_con[current_cell->id][0].vegetat_type_num){
                moisture_content = all_vars[current_cell->id].cell[iVeg][0].layer[0].moist;

                if(moisture_content < irrigation_point){
                    //do irrigation
                    
                    double irrigation_demand = (field_capacity - moisture_content) * veg_con[current_cell->id][iVeg].Cv / 1000; //m^3
                    
                    double old_inflow = inflow;

                    if(inflow > irrigation_demand){
                        //get irrigation water from local inflow
                        all_vars[current_cell->id].cell[iVeg][0].layer[0].moist += irrigation_demand / veg_con[current_cell->id][iVeg].Cv * 1000;
                        inflow -= irrigation_demand;
                        irrigation_demand = 0.0;
                    }else{
                        //reduce local inflow
                        all_vars[current_cell->id].cell[iVeg][0].layer[0].moist += inflow / veg_con[current_cell->id][iVeg].Cv * 1000;
                        irrigation_demand -= inflow;
                        inflow = 0.0;
                    }
                    
                    //log_info("old content = %.2f; new content = %.2f; inflow was %.2f",moisture_content,all_vars[current_cell->id].cell[iVeg][0].layer[0].moist,old_inflow);
                }
            }
        }
        //################################################################

        //convolute runoff and inflow to future
        for(t=0;t<UH_MAX_DAYS * global_param.model_steps_per_day;t++){
            current_cell->outflow[t]+=current_cell->uh[t] * runoff;
            current_cell->outflow[t]+=current_cell->uh[t] * inflow;
        }
        
    }
    
    size_t i;
    for(i=0;i<global_domain.ncells_active;i++){
        //write data
        out_data[rout.cells[i].id][OUT_DISCHARGE][0] += rout.cells[i].outflow[0];
    }
    
    //make_out_discharge_file("./debug_output/out_data/out_discharge.txt");
}

void make_out_discharge_file(char file_path[]){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern double ***out_data;
    
    FILE *file;
    
    if((file = fopen(file_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%.2f;", out_data[rout.gridded_cells[x][y-1]->id][OUT_DISCHARGE][0]);
                }else{
                    fprintf(file,"    ;");                    
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}