#include "ext_driver_shared_image.h"

/******************************************************************************
 * @brief    Initialize local cell order before it is called by the
 *           model.
 *****************************************************************************/
void
initialize_local_cell_order(size_t *cell_order){    
    (*cell_order) = MISSING_USI;
}

/******************************************************************************
 * @brief    Initialize wu_con before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_wu_con(wu_con_struct **wu_con){
    extern ext_parameters_struct ext_param;
    size_t i;
    
    for(i=0;i<WU_NSECTORS;i++){
        (*wu_con)[i].consumption_factor = 0.0;
        (*wu_con)[i].demand = 0.0;
        (*wu_con)[i].delay = ext_param.RETURN_DELAY[i];
        (*wu_con)[i].return_location = ext_param.RETURN_LOCATION[i];
    }
}

/******************************************************************************
 * @brief    Initialize wu_hist before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_wu_hist(wu_hist_struct **wu_hist){
    extern global_param_struct global_param;
    
    size_t i;
    size_t j;
    
    for(i=0;i<WU_NSECTORS;i++){
        for(j=0;j<global_param.model_steps_per_day;j++){
            (*wu_hist)[i].demand[j] = 0.0;
            (*wu_hist)[i].consumption_factor[j] = 0.0;
        }
    }
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
    extern ext_option_struct ext_options;
    
    size_t i;
    size_t j;
        
    if(ext_options.ROUTING){
        for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
            ext_all_vars->rout_var.discharge[j] = 0.0;                
        }
    }

    if(ext_options.WATER_USE){
        for(i=0;i<WU_NSECTORS;i++){
            ext_all_vars->wu_var[i].shortage = 0.0;
            ext_all_vars->wu_var[i].consumed = 0.0;
            ext_all_vars->wu_var[i].withdrawn = 0.0;
            ext_all_vars->wu_var[i].returned = 0.0;
            for(j=0;j< ext_param.RETURN_DELAY[i];j++){
                ext_all_vars->wu_var[i].return_flow[j]=0.0;
            }
        }
    }
}  

/******************************************************************************
 * @brief    Initialize local structures
 *****************************************************************************/
void
initialize_ext_local_structures(void)
{  
    extern ext_option_struct ext_options;
    extern domain_struct local_domain;
    extern rout_con_struct *rout_con;
    extern wu_con_struct **wu_con;
    extern wu_hist_struct **wu_hist;
    extern ext_all_vars_struct *ext_all_vars;
    extern size_t *cell_order_local;
    
    size_t i;
        
    for(i=0;i<local_domain.ncells_active;i++){ 
        initialize_ext_all_vars(&ext_all_vars[i]); 
        
        if(ext_options.ROUTING){
            initialize_rout_con(&rout_con[i]);
            initialize_local_cell_order(&cell_order_local[i]);
        }
        if(ext_options.WATER_USE){
            initialize_wu_con(&wu_con[i]);
            initialize_wu_hist(&wu_hist[i]);
        }
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
    options->WATER_USE = false;
    
    options->UH_PARAMETERS = CONSTANT_UH_PARAMETERS;
}

/******************************************************************************
 * @brief    Initialize all nameids before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_nameid(nameid_struct *nameid){
    strcpy(nameid->nc_filename, "MISSING");
    nameid->nc_id = NODATA_NC_ID;
}

/******************************************************************************
 * @brief    Initialize all filenames before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_filenames(ext_filenames_struct *filenames)
{
    initialize_nameid(&filenames->routing);
    initialize_nameid(&filenames->water_use);
    
    initialize_ext_info(&filenames->info);
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
    strcpy(info->distance_var, "MISSING");
    
    strcpy(info->irr_demand_var, "MISSING");
    strcpy(info->dom_demand_var, "MISSING");
    strcpy(info->ind_demand_var, "MISSING");
    strcpy(info->irr_cons_var, "MISSING");
    strcpy(info->dom_cons_var, "MISSING");
    strcpy(info->ind_cons_var, "MISSING");
}

/******************************************************************************
 * @brief    Initialize all parameters before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_parameters(ext_parameters_struct *parameters)
{      
    size_t i;
    
    // FOLLOWING PARAMETERS NEED TO BE ADJUSTED BASED ON EXPERIMENTATION
    parameters->MPI_N_PROCESS_COST=4;  
    parameters->MPI_E_PROCESS_COST=5; 
    
    parameters->UH_FLOW_DIFFUSION=800;
    parameters->UH_FLOW_VELOCITY=1.75;
    parameters->UH_MAX_LENGTH=2;
    parameters->UH_PARTITIONS=20;      
        
    for(i=0;i<WU_NSECTORS;i++){
        parameters->RETURN_DELAY[i]=8;
        parameters->RETURN_LOCATION[i]=WU_RETURN_SURFACEWATER;
    }
    parameters->RETURN_LOCATION[WU_IRRIGATION]=WU_RETURN_GROUNDWATER;
    
    parameters->forceoffset = 0;
    parameters->wu_hist_offset = 0;
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
    extern int mpi_rank;
    
    if(mpi_rank == VIC_MPI_ROOT){    
        initialize_ext_options(&ext_options);
        initialize_ext_filenames(&ext_filenames);
        initialize_ext_parameters(&ext_param);
    }
}

/******************************************************************************
 * @brief    Initialize global cell order before it is called by the
 *           model.
 *****************************************************************************/
void
initialize_global_cell_order(size_t *cell_order){
    extern domain_struct global_domain;
    
    size_t i;
    
    for(i=0;i<global_domain.ncells_active;i++){
        cell_order[i] = MISSING_USI;
    }
}