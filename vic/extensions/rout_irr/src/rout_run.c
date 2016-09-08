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
        runoff = (out_data[current_cell->id][OUT_RUNOFF][0]+out_data[current_cell->id][OUT_BASEFLOW][0]) 
                * current_cell->location->area / MM_PER_M / global_param.dt;

        inflow=0.0;        
        size_t c;
        for(c=0;c<current_cell->nr_upstream;c++){
            inflow += current_cell->upstream[c]->outflow[0];
        }
        
        
        //################ irrigation section ####################################
                
        size_t i;
        size_t j;
        size_t iVeg;
        
        out_data[current_cell->id][OUT_IRR][0]=0;
        
        for(i=0;i<=veg_con[current_cell->id][0].vegetat_type_num;i++){
            iVeg = veg_con[current_cell->id][i].veg_class;
            
            if(iVeg==VEG_IRR_CLASS){
                
                //allocate and instantiate variables
                double moisture_ice_mm = 0.0;
                double moisture_content;
                
                double irrigation_demand;
                double available_river_water = inflow * global_param.dt / current_cell->location->area * MM_PER_M; //mm
                double available_runoff_water = runoff * global_param.dt / current_cell->location->area * MM_PER_M; //mm
        
                double added_river_water = 0.0;
                double added_runoff_water = 0.0;
                               
                //calculate the soil moisture content
                for(j=0;j<options.SNOW_BAND;j++){
                    moisture_content =  all_vars[current_cell->id].cell[i][j].layer[0].moist * soil_con[current_cell->id].AreaFract[j]; //mm
                    
                    size_t frost_area;
                    for (frost_area = 0; frost_area < options.Nfrost; frost_area++) {
                        moisture_ice_mm += all_vars[current_cell->id].cell[i][j].layer[0].ice[frost_area] * soil_con[current_cell->id].AreaFract[j] * soil_con[current_cell->id].frost_fract[frost_area];
                    }
                    moisture_content -= moisture_ice_mm;
                }
                
                //calculate the irrigation demand
                if(moisture_content < soil_con[current_cell->id].Wcr[0]){
                    irrigation_demand = (soil_con[current_cell->id].Wcr[0] / 0.7) - moisture_content; //mm
                }else{
                    irrigation_demand = 0.0;
                }
                
                out_data[current_cell->id][OUT_IRR_DEMAND][0] = irrigation_demand;
                
                //get the water demand from runoff
                if(irrigation_demand>0.0){
                    if(available_runoff_water > irrigation_demand * veg_con[current_cell->id][i].Cv){
                        added_runoff_water = irrigation_demand;
                        irrigation_demand = 0.0;
                    }else{
                        added_runoff_water = available_runoff_water / veg_con[current_cell->id][i].Cv;
                        irrigation_demand -= added_runoff_water;
                    }
                }
                
                //get the water demand from local river-flow
                if(irrigation_demand>0.0){
                    if(available_river_water > irrigation_demand * veg_con[current_cell->id][i].Cv){
                        added_river_water = irrigation_demand * veg_con[current_cell->id][i].Cv;
                        irrigation_demand = 0.0;

                    }else{
                        added_river_water = available_river_water / veg_con[current_cell->id][i].Cv;
                        irrigation_demand -= added_river_water;

                    }
                }
                
                //FIXME: get the water demand from reservoirs
                
                //write away data and reduce inflow
                for(j=0;j<options.SNOW_BAND;j++){
                   all_vars[current_cell->id].cell[i][j].layer[0].moist = moisture_content + added_river_water + added_runoff_water; //mm
                }
                
                inflow -= (added_river_water * current_cell->location->area * veg_con[current_cell->id][i].Cv / global_param.dt / MM_PER_M);
                runoff -= (added_runoff_water * current_cell->location->area * veg_con[current_cell->id][i].Cv / global_param.dt / MM_PER_M);
                
                out_data[current_cell->id][OUT_IRR][0] = added_river_water + added_runoff_water;
                
                break;
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
        out_data[rout.cells[i].id][OUT_DISCHARGE][0] += rout.cells[i].outflow[0];
    }
}