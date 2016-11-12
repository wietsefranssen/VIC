/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>
#include <netcdf.h>
#include <rout.h>
#include <math.h>
#include <string.h>
#define M_PI 3.14159265358979323846

void rout_init(void){
    extern module_struct rout;
    
    //Initialize the routing part of the module
    init_routing();
    
    if(rout.param.firrigation){
        //Initialize the irrigation part of the module
        init_irr();
    }
    if(rout.param.fdams){
        //Initialize the dam part of the module
        init_dams();
    }
    
    if(rout.nr_dams>0 && rout.nr_irr_cells>0){
        //Initialize dam irrigation
        init_dam_irr();
    }
}

void init_routing(void){
    extern module_struct rout;
    
    //Set the cell locations
    set_cell_location();
    //Set the unit hydrograph based on input file distance
    set_cell_uh("flow_distance");
    //Set the downstream cells based on the input file direction
    set_cell_downstream("flow_direction");
    //Set the upstream cells based on the downstream cells
    set_cell_upstream();
    //Rank the cells from upstream to downstream
    set_cell_rank();
    
    //Debug functions
    make_location_file(rout.param.debug_path, "locations");
    make_global_location_file(rout.param.debug_path, "global_locations");
    make_nr_upstream_file(rout.param.debug_path, "nr_upstream");
    make_ranked_cells_file(rout.param.debug_path, "ranks");
    make_uh_file(rout.param.debug_path, "unit_hydrograph");
    
}

void init_irr(void){
    extern module_struct rout;
    
    //Set the irrigation part for cells with irrigation
    set_irr();
    //Set crop information for irrigated cells
    set_irr_crops();
    
    //Debug functions
    make_nr_crops_file(rout.param.debug_path, "nr_crops");
}

void init_dams(void){
    extern module_struct rout;
    
    //Set dam information based on dam file
    set_dam_location();
    //Compute natural routing if dams are in series
    set_dam_natural_routing();
    
    //Debug functions
    make_dam_file(rout.param.debug_path,"dams");
}

void init_dam_irr(void){
    extern module_struct rout;
    
    //Set dam irrigation services
    set_dam_irr_service();
    
    //Debug functions
    make_dam_service_file(rout.param.debug_path,"dam_service");
    make_nr_dam_service_file(rout.param.debug_path,"nr_dam_service");
}