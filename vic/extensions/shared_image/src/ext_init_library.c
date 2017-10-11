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
 * @brief    Initialize dmy before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_dmy(dmy_struct *dmy)
{           
    dmy->day = 0;
    dmy->day_in_year = 0;
    dmy->dayseconds = 0;
    dmy->month = 0;
    dmy->year = 0;
}

/******************************************************************************
 * @brief    Initialize rout_con before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_rout_con(rout_con_struct *rout_con)
{   
    extern ext_option_struct ext_options;
    
    size_t i;
        
    rout_con->Nupstream=0;
    rout_con->downstream=MISSING_USI;

    for(i=0;i<ext_options.uh_steps;i++){
        rout_con->uh[i]=0.0;
    }
}

/******************************************************************************
 * @brief    Initialize dam_con before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_dam_con(dam_con_struct *dam_con)
{           
    dam_con->lat=0.0;
    dam_con->lon=0.0;
    dam_con->year=0;
    dam_con->max_area=0.0;
    dam_con->max_height=0.0;
    dam_con->max_volume=0.0;
}

/******************************************************************************
 * @brief    Initialize rout_var before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_rout_var(rout_var_struct *rout_var)
{           
    extern ext_option_struct ext_options;
    
    size_t i;
    
    for(i=0;i<ext_options.uh_steps;i++){
        rout_var->discharge[i] = 0.0;   
        rout_var->nat_discharge[i] = 0.0;            
    }
}

/******************************************************************************
 * @brief    Initialize dam_var before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_dam_var(dam_var_struct *dam_var)
{           
    extern ext_option_struct ext_options;
    
    size_t i;
    
    dam_var->years_running=0;
    dam_var->run=false;
    
    dam_var->area=0.0;
    dam_var->height=0.0;
    dam_var->volume=0.0;
    
    
    dam_var->inflow_total=0.0;
    dam_var->nat_inflow_total=0.0;
    dam_var->inflow_offset=0.0;
    dam_var->discharge=0.0;
    for(i=0;i<ext_options.history_steps;i++){
        dam_var->inflow_history[i] = 0.0;
        dam_var->calc_discharge[i] = 0.0;
        dam_var->nat_inflow_history[i] = 0.0;
    }
    
    initialize_dmy(&dam_var->op_year);
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
    extern dam_con_struct **dam_con;
    extern dam_con_map_struct *dam_con_map;
    extern ext_all_vars_struct *ext_all_vars;
    extern size_t *cell_order_local;
    
    size_t i;
    size_t j;
        
    for(i=0;i<local_domain.ncells_active;i++){
        
        if(ext_options.ROUTING){
            initialize_rout_con(&rout_con[i]);
            initialize_rout_var(&ext_all_vars[i].rout_var);
            initialize_local_cell_order(&cell_order_local[i]);
        }
        if(ext_options.DAMS){
            for(j=0;j<(size_t) dam_con_map[i].Ndams;j++){
                initialize_dam_con(&dam_con[i][j]);
                initialize_dam_var(&ext_all_vars[i].dam_var[j]);
            }
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
    options->DAMS = false;
    
    options->UH_PARAMETERS = CONSTANT_UH_PARAMETERS;
    
    options->uh_steps = 0;
    
    options->history_steps = 0;
    options->model_steps_per_history_step = 0;
    options->history_steps_per_history_year = 0;
    options->ndams = 0;
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
    initialize_nameid(&filenames->dams);
    
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
    
    strcpy(info->ndam_var, "MISSING");
    strcpy(info->dam_year_var, "MISSING");
    strcpy(info->dam_lat_var, "MISSING");
    strcpy(info->dam_lon_var, "MISSING");
    strcpy(info->dam_volume_var, "MISSING");
    strcpy(info->dam_area_var, "MISSING");
    strcpy(info->dam_height_var, "MISSING");
}

/******************************************************************************
 * @brief    Initialize all parameters before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_parameters(ext_parameters_struct *parameters)
{          
    // FOLLOWING PARAMETERS NEED TO BE ADJUSTED BASED ON EXPERIMENTATION
    parameters->MPI_N_PROCESS_COST = 4;  
    parameters->MPI_E_PROCESS_COST = 5; 
    
    parameters->UH_FLOW_DIFFUSION = 800;
    parameters->UH_FLOW_VELOCITY = 1.75;
    parameters->UH_LENGTH = 2;
    parameters->UH_PARTITIONS = 20;
    
    parameters->DAM_HISTORY = 3;
    parameters->DAM_HISTORY_LENGTH = 10;
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