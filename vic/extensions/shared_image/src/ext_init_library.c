#include "ext_driver_shared_image.h"

/******************************************************************************
 * @brief    Initialize basins before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_basins(basin_struct *basins)
{   
    basins->Nbasin = 0;    
}

/******************************************************************************
 * @brief    Initialize rout_con before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_rout_con(rout_con_struct *rout_con)
{   
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    size_t i;
    
    rout_con->Nupstream_global=0;
    rout_con->downstream_global=MISSING_USI;
    rout_con->downstream_local=MISSING_USI;

    for(i=0;i<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;i++){
        rout_con->uh[i]=0.0;
    }
}

/******************************************************************************
 * @brief    Initialize ext_all_vars before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_all_vars(ext_all_vars_struct *ext_all_vars)
{    
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    size_t i;
        
    for(i=0;i<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;i++){
        ext_all_vars->rout_var.discharge[i] = 0.0;                
    }
    
}

/******************************************************************************
 * @brief    Initialize local structures
 *****************************************************************************/
void
initialize_ext_local_structures(void)
{  
    extern domain_struct local_domain;
    extern basin_struct basins;
    extern rout_con_struct *rout_con;
    extern ext_all_vars_struct *ext_all_vars;
    extern size_t *cell_order_local;
    extern size_t *mpi_map_mapping_array_reverse;
    
    size_t i;
    
    for(i=0;i<local_domain.ncells_active;i++){
        initialize_basins(&basins);
        initialize_rout_con(&rout_con[i]);
        initialize_ext_all_vars(&ext_all_vars[i]);
        
        cell_order_local[i] = MISSING_USI;
        mpi_map_mapping_array_reverse[i] = MISSING_USI;
    }
}

/******************************************************************************
 * @brief    Initialize all options before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_options(ext_option_struct *options)
{    
    options->ROUTING = false;
    
    options->UH_PARAMETERS = CONSTANT_UH_PARAMETERS;
}

/******************************************************************************
 * @brief    Initialize all filenames before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_filenames(ext_filenames_struct *filenames)
{
    strcpy(filenames->routing, "MISSING");
}

/******************************************************************************
 * @brief    Initialize all info before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_info(ext_info_struct *info){    
    strcpy(info->direction_var, "MISSING");
    strcpy(info->velocity_var, "MISSING");
    strcpy(info->diffusion_var, "MISSING");
}

/******************************************************************************
 * @brief    Initialize all parameters before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_parameters(ext_parameters_struct *parameters)
{   
    parameters->UH_FLOW_DIFFUSION=800;
    parameters->UH_FLOW_VELOCITY=1.75;
    parameters->UH_MAX_LENGTH=2;
    parameters->UH_PARTITIONS=20;  
    
    parameters->MPI_N_PROCESS_COST=4;  
    parameters->MPI_E_PROCESS_COST=5;      
}

void
initialize_global_cell_order(size_t *cell_order){
    extern domain_struct global_domain;
    
    size_t i;
    
    for(i=0;i<global_domain.ncells_active;i++){
        cell_order[i] = MISSING_USI;
    }
}

/******************************************************************************
 * @brief    Initialize global structures
 *****************************************************************************/
void
initialize_ext_global_structures(void)
{  
    extern ext_filenames_struct ext_filenames;
    extern ext_option_struct ext_options;
    extern ext_parameters_struct ext_param;
    extern ext_info_struct ext_info;
    extern int mpi_rank;
    
    if(mpi_rank == VIC_MPI_ROOT){    
        initialize_ext_options(&ext_options);
        initialize_ext_filenames(&ext_filenames);
        initialize_ext_parameters(&ext_param);
        initialize_ext_info(&ext_info);
    }
}
