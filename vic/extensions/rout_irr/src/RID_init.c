/******************************************************************************
 * @section DESCRIPTION
 *  
 * Initialize and dynamically allocate memory for the routing, 
 * irrigation and dam module.
 ******************************************************************************/

#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>
#include <netcdf.h>
#include <rout.h>
#include <math.h>
#include <string.h>

/******************************************************************************
 * @section brief
 *  
 * Dynamically allocate and initialize variables for the routing, 
 * irrigation and dam modules on the master node
 ******************************************************************************/

void RID_init(void){
    extern RID_struct RID;
    extern int mpi_rank;
    extern global_param_struct global_param;

    if (mpi_rank == VIC_MPI_ROOT) {
        
        printf("Into INITgeneral\n");
        
        // General
        init_RID();
        set_cell_locations();
        
        printf("Exit INITgeneral, into INITrouting\n");
        
        // Routing
        init_routing();
        printf("Done init_routing()\n");
        set_routing_locations();
        printf("Done set_routing_locations()\n");
        set_routing_uh("flow_distance");
        printf("Done set_routing_uh()\n");
        set_routing_downstream("flow_direction");
        printf("Done set_routing_downstream()\n");
        set_routing_upstream();
        printf("Done set_routing_upstream()\n");
        set_routing_rank();
        printf("Done set_routing_rank()\n");
        
        printf("Exit INITrouting, into INITirrigation\n");
    }    
    printf("finished, node: %d\n",mpi_rank);
    // Irrigation
    if (global_param.firrigation) {
        printf("en?!?!!?\n");
        alloc_init_set_irrigation();
    printf("finished2, node: %d\n",mpi_rank);
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        
        if(global_param.fdams){
            init_dams();
        }

        if(RID.nr_dams>0 && RID.nr_irr_cells>0){
            init_dam_irr();
        }
        
        //Debug
        if(global_param.fdebug_mode){
            make_location_file(global_param.debug_path, "locations");
            make_global_location_file(global_param.debug_path, "global_locations");
            make_nr_upstream_file(global_param.debug_path, "nr_upstream");
            make_ranked_cells_file(global_param.debug_path, "ranks");
            make_uh_file(global_param.debug_path, "unit_hydrograph");
            
            if(global_param.firrigation){
                make_nr_crops_file(global_param.debug_path, "nr_crops");
            }
            if(global_param.fdams){
                make_dam_file(global_param.debug_path,"dams");
            }

            if(RID.nr_dams>0 && RID.nr_irr_cells>0){
                make_dam_service_file(global_param.debug_path,"dam_service");
            }
        }
    }else{
        printf("Entering sleep...\n");
//        sleep(500000);
    }
}

/******************************************************************************
 * @section brief
 *  
 * Initialize and dynamically allocate memory for the dam module.
 * Set dam information from file and check whether natural routing is done.
 ******************************************************************************/

void init_dams(void){
    extern RID_struct RID;
    
    set_dam_information();
    set_dam_natural_routing();
}

/******************************************************************************
 * @section brief
 *  
 * Initialize and dynamically allocate memory for the irrigation and dam module.
 * Link irrigated cells to dam so water redistribution is possible.
 ******************************************************************************/

void init_dam_irr(void){
    extern RID_struct RID;
    
    set_dam_irr_service();
}