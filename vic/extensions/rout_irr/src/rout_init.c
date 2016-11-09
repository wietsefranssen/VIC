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
    extern rout_struct rout;
    
    //Set cell values based on location and vegetation
    set_cells();
    
    //Set cell crop struct based on irrigated vegetation
    set_cell_irrigate();
    
    //Set cell upstream and downstream cells based on flow direction
    set_upstream_downstream(rout.param_filename,"flow_direction");
    
    //Sort cells upstream to downstream
    sort_cells();
    
    //Set cell unit hydrograph based of flow distance
    set_uh(rout.param_filename,"flow_distance");
    
    //Set reservoir information
    set_reservoirs();
    
    //Check if we need to do naturalized routing
    set_naturalized_routing();
    
    //Set reservoir irrigation services
    set_reservoir_service();

    if(rout.fdebug_mode){
        //Make routing debug files
        
        log_info("Making routing debug files...");
        make_location_file(rout.debug_path,"location");
        log_info("Finished cell location file");
        make_global_location_file(rout.debug_path,"global_location");
        log_info("Finished cell global location file");
        make_nr_upstream_file(rout.debug_path,"nr_upstream");
        log_info("Finished number upstream file");
        make_uh_file(rout.debug_path,"uh");
        log_info("Finished unit hydrograph file");
        make_ranked_cells_file(rout.debug_path,"ranked_cells");
        log_info("Finished cell rank file");
        
        if(rout.reservoirs){
            make_reservoir_file(rout.debug_path,"reservoir");
            log_info("Finished reservoir file");
            make_reservoir_service_file(rout.debug_path,"reservoir_service");
            log_info("Finished reservoir service file");
            make_nr_reservoir_service_file(rout.debug_path,"nr_reservoir_service");
            log_info("Finished number of reservoir service file");
            make_nr_crops_file(rout.debug_path,"nr_crops");
            log_info("Finished number of crops file");
        }
    }
    
}