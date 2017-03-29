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
        
    init_global_domain_vars();
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

/******************************************************************************
 * @section brief
 *  
 * Allocate and initialize several variables from local domains for use on the
 * main node.
 ******************************************************************************/
void init_global_domain_vars(void){
    extern RID_struct RID;
    extern domain_struct global_domain;
    extern soil_con_struct *soil_con;
    extern veg_con_struct *veg_con;
    extern int mpi_rank;
    extern option_struct options;
    
    size_t i;
    size_t j;
    size_t k;    
    
    if (mpi_rank == VIC_MPI_ROOT) {
        RID.global_vars.Wcr = malloc(global_domain.ncells_active * sizeof(*RID.global_vars.Wcr));
        check_alloc_status(RID.global_vars.Wcr,"Memory allocation error");
        RID.global_vars.cv = malloc(global_domain.ncells_active * sizeof(*RID.global_vars.cv));
        check_alloc_status(RID.global_vars.cv,"Memory allocation error");
        RID.global_vars.frost_frac = malloc(global_domain.ncells_active * sizeof(*RID.global_vars.frost_frac));
        check_alloc_status(RID.global_vars.frost_frac,"Memory allocation error");
        RID.global_vars.snow_frac = malloc(global_domain.ncells_active * sizeof(*RID.global_vars.snow_frac));
        check_alloc_status(RID.global_vars.snow_frac,"Memory allocation error");
        RID.global_vars.ice = malloc(global_domain.ncells_active * sizeof(*RID.global_vars.ice));
        check_alloc_status(RID.global_vars.ice,"Memory allocation error");
        RID.global_vars.moisture = malloc(global_domain.ncells_active * sizeof(*RID.global_vars.moisture));
        check_alloc_status(RID.global_vars.moisture,"Memory allocation error");

        for(i=0;i<global_domain.ncells_active;i++){
            
            RID.global_vars.Wcr[i]=0;
            
            if(RID.cells[i].irr!=NULL){
                RID.global_vars.cv[i] = malloc(RID.cells[i].irr->nr_crops * sizeof(*RID.global_vars.cv[i]));
                check_alloc_status(RID.global_vars.cv[i],"Memory allocation error");
                RID.global_vars.ice[i] = malloc(RID.cells[i].irr->nr_crops * sizeof(*RID.global_vars.ice[i]));
                check_alloc_status(RID.global_vars.ice[i],"Memory allocation error");
                RID.global_vars.moisture[i] = malloc(RID.cells[i].irr->nr_crops * sizeof(*RID.global_vars.moisture[i]));
                check_alloc_status(RID.global_vars.moisture[i],"Memory allocation error");
                
                for(j=0;j<RID.cells[i].irr->nr_crops;j++){
                    
                     RID.global_vars.cv[i][j]=0;
                    
                    RID.global_vars.ice[i][j] = malloc(options.SNOW_BAND * sizeof(*RID.global_vars.ice[i][j]));
                    check_alloc_status(RID.global_vars.ice[i][j],"Memory allocation error");
                    RID.global_vars.moisture[i][j] = malloc(options.SNOW_BAND * sizeof(*RID.global_vars.moisture[i][j]));
                    check_alloc_status(RID.global_vars.moisture[i][j],"Memory allocation error");
                    
                    for(k=0;k<options.SNOW_BAND;k++){
                        RID.global_vars.ice[i][j][k]=0;
                        RID.global_vars.moisture[i][j][k]=0;
                    }
                }
            }
            
            RID.global_vars.snow_frac[i] = malloc(options.SNOW_BAND * sizeof(*RID.global_vars.snow_frac[i]));
            check_alloc_status(RID.global_vars.snow_frac[i],"Memory allocation error");
            RID.global_vars.frost_frac[i] = malloc(options.Nfrost * sizeof(*RID.global_vars.frost_frac[i]));
            check_alloc_status(RID.global_vars.frost_frac[i],"Memory allocation error");
            
            for(j=0;j<options.SNOW_BAND;j++){
                RID.global_vars.snow_frac[i][j]=0;
            }
            
            for(j=0;j<options.Nfrost;j++){
                RID.global_vars.frost_frac[i][j]=0;
            }
        }        
    }
    
    //gather variables for snow_frac, frost_frac,cv and Wcr
    gather_var_double(RID.global_vars.Wcr,&soil_con->Wcr[0]);
    
    for(j=0;j<RID.cells[i].irr->nr_crops;j++){
        gather_var_double(RID.global_vars.Wcr,veg_con);
    }
    
}