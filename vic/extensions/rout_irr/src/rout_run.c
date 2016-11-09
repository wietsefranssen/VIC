#include <vic_def.h>
#include <rout.h>
#include <assert.h>
#include <vic_driver_image.h>
#include <math.h>


void rout_run(dmy_struct* current_dmy){
    
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern double ***out_data;       
    extern veg_con_struct **veg_con;
    extern soil_con_struct *soil_con;
    extern veg_con_map_struct *veg_con_map;
    
    //routing variables
    double runoff = 0.0;                    //m^3s
    double inflow = 0.0;                    //m^3s
    
    double moisture_content[rout.nr_crop_classes];               //mm (per crop)
    double irrigation_demand[rout.nr_crop_classes];              //mm (per crop)
    double irrigation_demand_cell=0.0;      //mm
    
    
    double added_river_water=0.0;           //mm
    double added_runoff_water=0.0;          //mm
    double added_reservoir_water=0.0;       //mm
    double available_river_water;           //mm
    double available_runoff_water;          //mm
    
    double target_release =0.0;             //m3
    double target_irrigation=0.0;           //m3
    double total_current_demand = 0.0;      //m3
    double total_added_reservoir_water=0.0; //m3
    double overflow = 0.0;                  //m3
    
    double new_moisture_content =0.0;       //mm
    
    size_t vidx = 0;
    
    rout_cell* cur_cell;
    
    //values used for iteration
    size_t iRes;
    size_t iCell;
    size_t iRank;
    size_t iVeg;
    size_t iCrop;
    
    //double lat[]={15.1167,17.3983,18.01,19.4917};
    //double lon[]={105.8,104.8033,102.62,101.5367};
    
    /* 
     * For all reservoirs, calculate and reset values      
     */
    if(rout.reservoirs){
        reset_reservoirs(current_dmy);
    }
    
    /*
     * Go through the sorted cells and handle routing and irrigation     
     */
    for(iRank=0;iRank<global_domain.ncells_active;iRank++){
        cur_cell = rout.sorted_cells[iRank];
        
        /*
        for(iCell=0;iCell<4;iCell++){
            if(lat[iCell]>local_domain.locations[cur_cell->id].latitude-(VIC_RESOLUTION/2) &&
                    lat[iCell]<local_domain.locations[cur_cell->id].latitude+(VIC_RESOLUTION/2) &&
                    lon[iCell]>local_domain.locations[cur_cell->id].longitude-(VIC_RESOLUTION/2) &&
                    lon[iCell]<local_domain.locations[cur_cell->id].longitude+(VIC_RESOLUTION/2)){
                out_data[cur_cell->id][OUT_RES_STORE][0]=1;
            }
        }
        */
        
        //Shift outflow array
        shift_outflow_array(cur_cell);
        
        //Determine runoff and inflow from upstream cells
        runoff = (out_data[cur_cell->id][OUT_RUNOFF][0]+out_data[cur_cell->id][OUT_BASEFLOW][0]) 
                * local_domain.locations[cur_cell->id].area / MM_PER_M / global_param.dt;
        
        inflow=0.0;
        for(iCell=0;iCell<cur_cell->nr_upstream;iCell++){
            inflow += cur_cell->upstream[iCell]->outflow[0];
        }
        
        //Change the crop fraction based on growing season
        change_crop_fraction(cur_cell,current_dmy);
        
        
            
        for(iVeg=0;iVeg < rout.nr_crop_classes;iVeg++){

            vidx=veg_con_map[cur_cell->id].vidx[rout.crop_class[iVeg]];
            if(vidx==(size_t)NODATA_VEG){
                continue;
            }
            
            //Write away data
            out_data[cur_cell->id][OUT_CROP_FRAC][0]= veg_con[cur_cell->id][vidx].Cv;
            out_data[cur_cell->id][OUT_BARE_FRAC][0]= veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv;
        }

        //If the cell is able to irrigate, do local irrigation  (if irrigation is true)
        if(rout.firrigation && cur_cell->irrigate){
            
            //Get available water from inflow and runoff in mm
            available_river_water = inflow * global_param.dt / 
                local_domain.locations[cur_cell->id].area * MM_PER_M;

            available_runoff_water = runoff * global_param.dt /
                local_domain.locations[cur_cell->id].area * MM_PER_M;
            
            irrigation_demand_cell = 0.0;
            iCrop=0;
            
            for(iVeg=0;iVeg < rout.nr_crop_classes;iVeg++){
                
                vidx=veg_con_map[cur_cell->id].vidx[rout.crop_class[iVeg]];
                if(vidx==(size_t)NODATA_VEG){
                    continue;
                }
                
                //Get moisture content of our cell
                moisture_content[iCrop] = get_moisture_content(*cur_cell,vidx);
                
                //Get irrigation demand
                //(Wcr / 0.7) is the field capacity of a cell!
                irrigation_demand[iCrop] = 0.0;
                if(moisture_content[iCrop] <= soil_con[cur_cell->id].Wcr[0]){

                    irrigation_demand[iCrop] = 
                            ((soil_con[cur_cell->id].Wcr[0] / 0.7) - moisture_content[iCrop]) * 
                            veg_con[cur_cell->id][vidx].Cv;
                    irrigation_demand_cell += irrigation_demand[iCrop];
                }
                
                iCrop++;
            }
            
            out_data[cur_cell->id][OUT_DEMAND_START][0] = irrigation_demand_cell;
            
            iCrop=0;
            
            for(iVeg=0;iVeg < rout.nr_crop_classes;iVeg++){
                
                vidx=veg_con_map[cur_cell->id].vidx[rout.crop_class[iVeg]];
                if(vidx==(size_t)NODATA_VEG){
                    continue;
                }
                
                added_runoff_water = 0.0;
                added_river_water = 0.0;
                
                if(rout.fpot_irrigation){
                    added_river_water = irrigation_demand_cell;
                    irrigation_demand[iCrop] -= added_river_water;
                    irrigation_demand_cell -= added_river_water;
                }else{
                    //Do runoff irrigation
                    if(irrigation_demand_cell>0){
                        if(available_runoff_water >= irrigation_demand_cell){
                            added_runoff_water= irrigation_demand[iCrop];
                        }else{
                            added_runoff_water= available_runoff_water * 
                                    (irrigation_demand[iCrop] / irrigation_demand_cell);
                        }
                        irrigation_demand[iCrop] -= added_runoff_water;
                        available_runoff_water -= added_runoff_water;
                        irrigation_demand_cell -= added_runoff_water;

                        //Do river irrigation
                        if(available_river_water >= irrigation_demand_cell){
                            added_river_water = irrigation_demand[iCrop];
                        }else{
                            added_river_water = available_river_water *
                                    (irrigation_demand[iCrop] / irrigation_demand_cell);
                        }
                        irrigation_demand[iCrop] -= added_river_water;
                        available_river_water -= added_river_water;
                        irrigation_demand_cell -= added_river_water;
                    }
                }
                
                out_data[cur_cell->id][OUT_DEMAND_RES][0] = irrigation_demand_cell;
                out_data[cur_cell->id][OUT_DEMAND_END][0] = irrigation_demand_cell;
                
                //Communicate leftover demand with reservoirs
                if(irrigation_demand[iCrop]>0){
                    distribute_demand_among_reservoirs(cur_cell,irrigation_demand[iCrop],iCrop);
                }
                
                //Add water to moisture content in soil
                new_moisture_content = moisture_content[iCrop] + ((added_river_water + added_runoff_water) / 
                           veg_con[cur_cell->id][vidx].Cv);
                add_moisture_content(cur_cell,vidx,new_moisture_content);
                
                out_data[cur_cell->id][OUT_LOCAL_IRR][0] = added_river_water + added_runoff_water;
                out_data[cur_cell->id][OUT_LOCAL_IRR_M3][0] = (added_river_water + added_runoff_water) * 
                        local_domain.locations[cur_cell->id].area / MM_PER_M;
                
                iCrop++;
                
            }
            
            //Recalculate runoff and inflow based on left-over available water
            runoff = available_runoff_water / global_param.dt * 
                    local_domain.locations[cur_cell->id].area / MM_PER_M;
            inflow = available_river_water / global_param.dt * 
                    local_domain.locations[cur_cell->id].area / MM_PER_M;

        }
        
        //Do routing
        do_routing(cur_cell,inflow,runoff);

        //Write output data
        out_data[cur_cell->id][OUT_IRR][0] += out_data[cur_cell->id][OUT_LOCAL_IRR][0];
        out_data[cur_cell->id][OUT_IRR_M3][0] += out_data[cur_cell->id][OUT_LOCAL_IRR_M3][0];
        out_data[cur_cell->id][OUT_DISCHARGE][0] += cur_cell->outflow[0];
        out_data[cur_cell->id][OUT_NATURAL_DISCHARGE][0] += cur_cell->outflow_natural[0];
        
    }
    
    /*
     * Go through the reservoirs and do irrigation     
     */
    if(rout.reservoirs){
        for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
                        
            //Continue if reservoir is not run
            if(!rout.reservoirs[iRes].run){
                continue;
            }
            
            //Reset values
            total_added_reservoir_water=0.0;
            total_current_demand = 0.0;
            
            target_release = rout.reservoirs[iRes].target_release;
            if(target_release>rout.reservoirs[iRes].current_storage){
                target_release = rout.reservoirs[iRes].current_storage;
            }

            //If the reservoir has an irrigation function
            if(rout.reservoirs[iRes].function==RES_IRR_FUNCTION){
                
                //Calculate target irrigation
                target_irrigation = target_release - rout.reservoirs[iRes].monthly_inflow_natural * PERC_ENV_INFLOW;

                //Get total demand from all cells
                for(iCell=0;iCell<rout.reservoirs[iRes].nr_serviced_cells;iCell++){
                    for(iCrop=0;iCrop<cur_cell->nr_crops;iCrop++){
                        total_current_demand += rout.reservoirs[iRes].cell_demand[iCell][iCrop];
                    }
                }
                
                //If there is a demand for irrigation water, go through all cells this reservoir serves
                for(iCell=0;iCell<rout.reservoirs[iRes].nr_serviced_cells;iCell++){
                    cur_cell = rout.reservoirs[iRes].serviced_cells[iCell];
                    
                    iCrop=0;
                    
                    for(iVeg=0;iVeg<rout.nr_crop_classes;iVeg++){
                
                        vidx=veg_con_map[cur_cell->id].vidx[rout.crop_class[iVeg]];
                        if(vidx==(size_t)NODATA_VEG){
                            continue;
                        }
                        
                        added_reservoir_water=0.0;
                        
                        //Get moisture content
                        moisture_content[iCrop] = get_moisture_content(*cur_cell,vidx);
                        
                        //Save the current reservoir demand - the deficit of the previous step
                        set_reservoir_demand(&rout.reservoirs[iRes],cur_cell,iCell,iCrop);
                        
                        //Calculate irrigation water
                        if(total_current_demand>0){
                            if(total_current_demand < target_irrigation){
                                added_reservoir_water =  rout.reservoirs[iRes].cell_demand[iCell][iCrop];
                            }else{
                                added_reservoir_water =  target_irrigation * 
                                        (rout.reservoirs[iRes].cell_demand[iCell][iCrop] / total_current_demand);
                            }
                        }
                        
                        if(added_reservoir_water<0){
                            log_warn("Added reservoir water of reservoir %zu is smaller than 0, ignoring",rout.reservoirs[iRes].id);
                            added_reservoir_water=0.0;
                        }
                        
                        //Add water to moisture content in soil
                        new_moisture_content = moisture_content[iCrop] + (added_reservoir_water /
                                   local_domain.locations[cur_cell->id].area * MM_PER_M /
                                   veg_con[cur_cell->id][vidx].Cv);
                        add_moisture_content(cur_cell,vidx,new_moisture_content);                      

                        //Save total added water
                        total_added_reservoir_water += added_reservoir_water;
                        
                        //Calculate the current reservoir deficit (used to calculate demand next step)
                        set_reservoir_deficit(&rout.reservoirs[iRes],cur_cell,moisture_content[iCrop],iCell,iCrop,vidx);
                        out_data[cur_cell->id][OUT_DEMAND_END][0] = rout.reservoirs[iRes].prev_deficit[iCell][iCrop];
                        
                        //Write output data
                        out_data[cur_cell->id][OUT_RES_IRR][0] += added_reservoir_water / local_domain.locations[cur_cell->id].area * MM_PER_M;
                        out_data[cur_cell->id][OUT_RES_IRR_M3][0] += added_reservoir_water;
                        
                        iCrop++;
                    }
                    
                    out_data[cur_cell->id][OUT_IRR][0] += out_data[cur_cell->id][OUT_RES_IRR][0];
                    out_data[cur_cell->id][OUT_IRR_M3][0] += out_data[cur_cell->id][OUT_RES_IRR_M3][0];
                }
            }
            
            //Do overflow if reservoir is full
            overflow = 0;
            if(rout.reservoirs[iRes].current_storage - target_release > rout.reservoirs[iRes].storage_capacity){
                overflow = (rout.reservoirs[iRes].current_storage - target_release) - rout.reservoirs[iRes].storage_capacity;
            }
            
            //Reduce reservoir storage and do outflow
            rout.reservoirs[iRes].current_storage -= target_release + overflow;
            rout.reservoirs[iRes].cell->outflow[1] += ((target_release-total_added_reservoir_water) + overflow) / global_param.dt;
            
            //Write output data
            //out_data[rout.reservoirs[iRes].cell->id][OUT_RES_STORE][0]=
            //        rout.reservoirs[iRes].current_storage/rout.reservoirs[iRes].storage_capacity;
        }
    }
}