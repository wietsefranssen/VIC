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
 * Initialize and dynamically allocate memory for the routing, 
 * irrigation and dam module.
 ******************************************************************************/

void RID_init(void){
    extern RID_struct RID;
    extern int mpi_rank;
    
    if (mpi_rank == VIC_MPI_ROOT) {
        init_module();

        init_routing();

        if(RID.param.firrigation){
            init_irr();
        }
        if(RID.param.fdams){
            init_dams();
        }

        if(RID.nr_dams>0 && RID.nr_irr_cells>0){
            init_dam_irr();
        }
    }
}

/******************************************************************************
 * @section brief
 *  
 * Initialize the module.
 ******************************************************************************/

void init_module(void){
    
    set_cell_location();   
    
}

/******************************************************************************
 * @section brief
 *  
 * Initialize and dynamically allocate memory for the routing module.
 * Sets the unit hydro-graphs, downstream and upstream cells
 * and ranks all cells from upstream to downstream.
 ******************************************************************************/

void init_routing(void){
    extern RID_struct RID;
    
    set_cell_uh("flow_distance");
    set_cell_downstream("flow_direction");
    set_cell_upstream();
    set_cell_rank();
    
    if(RID.param.fdebug_mode){
    //Debug functions
        make_location_file(RID.param.debug_path, "locations");
        make_global_location_file(RID.param.debug_path, "global_locations");
        make_nr_upstream_file(RID.param.debug_path, "nr_upstream");
        make_ranked_cells_file(RID.param.debug_path, "ranks");
        make_uh_file(RID.param.debug_path, "unit_hydrograph");
    }
    
}

/******************************************************************************
 * @section brief
 *  
 * Initialize and dynamically allocate memory for the irrigation module.
 * Sets the irrigation and cells and locates the crops.
 ******************************************************************************/

void init_irr(void){
    extern RID_struct RID;
    
    set_irr();
    set_irr_crops();
    
    if(RID.param.fdebug_mode){
        //Debug functions
        make_nr_crops_file(RID.param.debug_path, "nr_crops");
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
    
    if(RID.param.fdebug_mode){
        //Debug functions
        make_dam_file(RID.param.debug_path,"dams");
    }
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
    
    if(RID.param.fdebug_mode){
        //Debug functions
        make_dam_service_file(RID.param.debug_path,"dam_service");
    }
}