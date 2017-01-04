#include <vic_def.h>
#include <rout.h>
#include <assert.h>
#include <vic_driver_image.h>
#include <math.h>


void rout_run(dmy_struct* current_dmy){
    //Main module routine
    extern module_struct rout;
    extern domain_struct global_domain;
    
    module_cell *cur_cell;
    dam_unit *cur_dam;
    
    size_t iRank;
    size_t iDam;
    
    double runoff = 0.0; //m^3s
    double inflow = 0.0; //m^3s
    
    for(iRank=0;iRank<global_domain.ncells_active;iRank++){
        //Go through cells from upstream to downstream
        cur_cell = rout.sorted_cells[iRank];
        
        //Gather inflow and runoff
        gather_runoff_inflow(cur_cell, &runoff, &inflow, false);
    
        if(rout.param.firrigation && cur_cell->irr!=NULL){
            //Do irrigation
            do_irrigation(cur_cell, &runoff, &inflow, current_dmy);
        }
        
        //Do routing
        do_routing(cur_cell, runoff, inflow);
    }
    
    if(rout.param.fdams){
        for(iDam=0;iDam<rout.nr_dams;iDam++){
            //Go through all dams
            cur_dam = &rout.dams[iDam];
            
            //Do dam operation
            do_dam_operation(cur_dam,current_dmy);
        }
    }  
}

void gather_runoff_inflow(module_cell *cur_cell, double *runoff, double *inflow, bool naturalized){
    //Get runoff from VIC and outflow from upstream cells
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern double ***out_data;
    
    size_t iCell;
    
    *runoff = (out_data[cur_cell->id][OUT_RUNOFF][0]+out_data[cur_cell->id][OUT_BASEFLOW][0]) 
            * local_domain.locations[cur_cell->id].area / MM_PER_M / global_param.dt;

    *inflow=0.0;
    if(naturalized){
        //Gather natural outflow
        for(iCell=0;iCell<cur_cell->rout->nr_upstream;iCell++){
            *inflow += cur_cell->rout->upstream[iCell]->outflow_natural[0];
        }
    }else{
        //Gather outflow
        for(iCell=0;iCell<cur_cell->rout->nr_upstream;iCell++){
            *inflow += cur_cell->rout->upstream[iCell]->outflow[0];
        }
    }
    
    //Save data
    out_data[cur_cell->id][OUT_RUNOFF_M3][0]=*runoff;
}

void do_irrigation(module_cell *cur_cell, double *inflow, double *runoff, dmy_struct *current_dmy){
    extern module_struct rout;
    extern veg_con_struct **veg_con;
    extern global_param_struct global_param;
    extern double ***out_data;
    extern domain_struct local_domain;
    extern all_vars_struct *all_vars;
    extern option_struct options;
    
    double moisture_content[cur_cell->irr->nr_crops];               //mm (per crop)
    double irrigation_demand[cur_cell->irr->nr_crops];              //m3 (per crop)
    double irrigation_demand_cell;          //m3
    
    double added_runoff_water;              //m3
    double added_inflow_water;              //m3
    double total_added_runoff_water;        //m3
    double total_added_inflow_water;        //m3
    
    double available_inflow_water;           //m3
    double available_runoff_water;          //m3
    
    double new_moisture_content;            //mm
    
    double old_runoff;                      //m3/s
    double old_inflow;                      //m3/s
    
    size_t iCrop;
    size_t iBand;
    
    available_inflow_water = *inflow * global_param.dt;
    available_runoff_water = *runoff * global_param.dt;
    
    total_added_runoff_water = 0.0;
    total_added_inflow_water = 0.0;
    irrigation_demand_cell=0.0;
    
    old_runoff = *runoff;
    old_inflow = *inflow;
    
    //Change crop fraction and moisture content based on growing season
    change_crop_fraction(cur_cell,current_dmy);
    
    //Save data (currently only for the first crop specified) FIXME: make available for multiple crops
    out_data[cur_cell->id][OUT_CROP_FRAC][0]=veg_con[cur_cell->id][cur_cell->irr->veg_index[0]].Cv;
    out_data[cur_cell->id][OUT_BARE_FRAC][0]=veg_con[cur_cell->id][veg_con[cur_cell->id][0].vegetat_type_num].Cv;
    for(iBand=0;iBand<options.SNOW_BAND;iBand++){
        out_data[cur_cell->id][OUT_CROP_MOIST][0]+=all_vars[cur_cell->id].cell[cur_cell->irr->veg_index[0]][iBand].layer[0].moist;
        out_data[cur_cell->id][OUT_BARE_MOIST][0]+=all_vars[cur_cell->id].cell[veg_con[cur_cell->id][0].vegetat_type_num][iBand].layer[0].moist;
    }

    for(iCrop=0;iCrop<cur_cell->irr->nr_crops;iCrop++){
        //Get moisture content and irrigation demand of our cell
        moisture_content[iCrop] = get_moisture_content(cur_cell->id,cur_cell->irr->veg_index[iCrop]);
        irrigation_demand[iCrop] = get_irrigation_demand(cur_cell->id,moisture_content[iCrop],cur_cell->irr->veg_index[iCrop]);
        irrigation_demand_cell += irrigation_demand[iCrop];
    }
    
    //Save data (currently only for the first crop specified) FIXME: make available for multiple crops
    out_data[cur_cell->id][OUT_MOISTURE_CONTENT][0] = moisture_content[0];
    out_data[cur_cell->id][OUT_DEMAND_START][0] = irrigation_demand_cell / M3_PER_HM3;

    if(irrigation_demand_cell > MIN_DEMAND){
        if(rout.param.fpot_irrigation){
            //Add all water needed but do not reduce outflow
            added_inflow_water = irrigation_demand[iCrop];
            irrigation_demand[iCrop] = 0;
        }else{
            //For each crop do runoff irrigation
            for(iCrop=0;iCrop<cur_cell->irr->nr_crops;iCrop++){
                added_runoff_water=do_source_irrigation(available_runoff_water,irrigation_demand[iCrop],irrigation_demand_cell);
                irrigation_demand[iCrop] -= added_runoff_water;
                total_added_runoff_water += added_runoff_water;
                
            }
            irrigation_demand_cell -= total_added_runoff_water;

            //For each crop do inflow irrigation
            for(iCrop=0;iCrop<cur_cell->irr->nr_crops;iCrop++){
                added_inflow_water=do_source_irrigation(available_inflow_water,irrigation_demand[iCrop],irrigation_demand_cell);
                irrigation_demand[iCrop] -= added_inflow_water;
                total_added_inflow_water += added_inflow_water;
            }
            irrigation_demand_cell -= added_inflow_water;

            for(iCrop=0;iCrop<cur_cell->irr->nr_crops;iCrop++){
                //Distribute leftover demand among reservoirs
                distribute_demand_among_dams(cur_cell,irrigation_demand[iCrop],iCrop);

                //Increase soil moisture
                new_moisture_content = moisture_content[iCrop] + ((added_inflow_water + added_runoff_water) / 
                           (local_domain.locations[cur_cell->id].area *
                            veg_con[cur_cell->id][cur_cell->irr->veg_index[iCrop]].Cv) * MM_PER_M);
                add_moisture_content(cur_cell,cur_cell->irr->veg_index[iCrop],new_moisture_content);
            }
            
            available_runoff_water -= total_added_runoff_water;
            available_inflow_water -= total_added_inflow_water;
        }
    }
    
    //Save data
    out_data[cur_cell->id][OUT_LOCAL_IRR][0] = (total_added_runoff_water + total_added_inflow_water) / M3_PER_HM3;
    out_data[cur_cell->id][OUT_IRR][0]+=out_data[cur_cell->id][OUT_LOCAL_IRR][0];
    out_data[cur_cell->id][OUT_DEMAND_RES][0] = irrigation_demand_cell / M3_PER_HM3;
    out_data[cur_cell->id][OUT_DEMAND_END][0] = irrigation_demand_cell / M3_PER_HM3;
    
    //Change runoff and inflow and check their values (because of rounding errors)
    *runoff = available_runoff_water / global_param.dt;
    *inflow = available_inflow_water / global_param.dt;
    check_irrigation(runoff,old_runoff,total_added_runoff_water);
    check_irrigation(inflow,old_inflow,total_added_inflow_water);
}

void do_routing(module_cell* cur_cell, double runoff, double inflow){
    extern module_struct rout;
    extern global_param_struct global_param;
    extern double ***out_data;
    
    shift_outflow_array(cur_cell);
    
    if(cur_cell->dam==NULL || cur_cell->dam->run==false){
        //Do normal routing with unit hydrograph
        do_uh_routing(cur_cell,inflow,runoff);
    }else{
        //Do not do routing but save water in dam storage
        do_dam_routing(cur_cell,inflow,runoff);
    }

    if(rout.param.fnaturalized_flow){
        //If routing is to be done twice, redo routing without dams
        gather_runoff_inflow(cur_cell,&runoff,&inflow,true);
        do_uh_routing(cur_cell,inflow,runoff);

        if(cur_cell->dam!=NULL){
            cur_cell->dam->current_inflow_natural +=
                    (inflow + runoff) * global_param.dt;
        }
    }else{
        //Else set the inflow as natural inflow
        if(cur_cell->dam!=NULL){
            cur_cell->dam->current_inflow_natural =
                    cur_cell->dam->current_inflow;
        }
    }
    
    //Save data
    out_data[cur_cell->id][OUT_DISCHARGE][0]=cur_cell->rout->outflow[0];
    out_data[cur_cell->id][OUT_NATURAL_DISCHARGE][0]=cur_cell->rout->outflow[0];
}

void do_dam_operation(dam_unit *cur_dam, dmy_struct *cur_dmy){
    extern global_param_struct global_param;
    extern veg_con_struct **veg_con;
    extern domain_struct local_domain;
    extern double ***out_data;
    
    module_cell *cur_cell;
    
    double total_current_demand;
    double target_release;
    double target_irrigation;
    
    double total_added_dam_water;
    double added_dam_water;
    
    double moisture_content;
    double new_moisture_content;
    
    double overflow;
    
    size_t iCell;
    size_t iCrop;
    
    total_added_dam_water=0.0;
    
    calculate_dam_values(cur_dam,cur_dmy);
                        
    //Return if reservoir is not yet run
    if(!cur_dam->run){
        if(cur_dmy->year>=cur_dam->activation_year){
            cur_dam->run=true;
        }else{
            return;
        }
    }
    
    //Get target release
    target_release = cur_dam->target_release;
    if(target_release>cur_dam->current_storage){
        target_release = cur_dam->current_storage;
    }

    if(cur_dam->function==DAM_IRR_FUNCTION){
        
        //Target irrigation is target release minus the environmental flow
        target_irrigation = target_release - cur_dam->monthly_inflow_natural * PERC_ENV_INFLOW;
        
        //Get the demand of all cells and all crops this dam serves
        total_current_demand = get_total_dam_demand(cur_dam);

        if(total_current_demand>MIN_DEMAND){
            for(iCell=0;iCell<cur_dam->nr_serviced_cells;iCell++){
                cur_cell = cur_dam->serviced_cells[iCell];
                
                //For each crop do dam irrigation
                for(iCrop=0;iCrop<cur_cell->irr->nr_crops;iCrop++){
                    added_dam_water = do_source_irrigation(target_irrigation,cur_dam->cell_demand[iCell][iCrop],total_current_demand);
                    cur_dam->cell_demand[iCell][iCrop]-= added_dam_water;
                    total_added_dam_water += added_dam_water;

                    //Save the current reservoir demand minus the deficit of the previous step
                    moisture_content = get_moisture_content(cur_cell->id,cur_cell->irr->veg_index[iCrop]);
                    set_dam_demand(cur_dam,cur_cell,iCell,iCrop);

                    //Add water to moisture content in soil
                    new_moisture_content = moisture_content + (added_dam_water /
                               (local_domain.locations[cur_cell->id].area 
                            * veg_con[cur_cell->id][cur_cell->irr->veg_index[iCrop]].Cv) * MM_PER_M);
                    add_moisture_content(cur_cell,cur_cell->irr->veg_index[iCrop],new_moisture_content);                      

                    //Calculate the current reservoir deficit (used to calculate demand next step)
                    set_dam_deficit(cur_dam,cur_cell,moisture_content,iCell,iCrop,cur_cell->irr->veg_index[iCrop]);


                    iCrop++;
                }
                
                //Save data (currently only for the first crop specified) FIXME: make available for multiple crops
                out_data[cur_cell->id][OUT_DEMAND_END][0] = cur_dam->cell_demand[iCell][0] / M3_PER_HM3;
                out_data[cur_cell->id][OUT_RES_IRR][0] += added_dam_water / M3_PER_HM3;
                out_data[cur_cell->id][OUT_IRR][0] += out_data[cur_cell->id][OUT_RES_IRR][0];
            }
        }
        
        //Set all demand to 0
        reset_dam_demand(cur_dam);
    }

    //Do overflow if reservoir is full
    overflow = 0;
    if(cur_dam->current_storage - target_release > cur_dam->capacity){
        overflow = (cur_dam->current_storage - target_release) - cur_dam->capacity;
    }

    //Reduce reservoir storage and do outflow
    cur_dam->current_storage -= target_release + overflow;
    cur_dam->cell->rout->outflow[0] += ((target_release-total_added_dam_water) + overflow) / global_param.dt;

    //Save data
    out_data[cur_dam->cell->id][OUT_RES_STORE][0]= cur_dam->current_storage/cur_dam->capacity;
}