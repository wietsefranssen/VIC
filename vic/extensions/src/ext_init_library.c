#include "ext_driver_shared_image.h"

/******************************************************************************
 * @brief    Initialize local structures
 *****************************************************************************/
void
initialize_ext_local_structures(void)
{  
    extern ext_option_struct ext_options;
    
    if(ext_options.GROUNDWATER){
        initialize_gw_local_structures();
    }
    if(ext_options.ROUTING){
        initialize_rout_local_structures();
    }
    if(ext_options.WATER_USE){
        initialize_wu_local_structures();
    }
    if(ext_options.IRRIGATION){
        initialize_irr_local_structures();
    }
}

/******************************************************************************
 * @brief    Initialize a dmy
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
 * @brief    Initialize a nameid
 *****************************************************************************/
void
initialize_nameid(nameid_struct *nameid){
    strcpy(nameid->nc_filename, MISSING_S);
    nameid->nc_id = MISSING;
}

/******************************************************************************
 * @brief    Initialize all options before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_options(ext_option_struct *options)
{    
    size_t i;
    
    options->GROUNDWATER = false;
    options->ROUTING = false;
    options->WATER_USE = false;
    options->IRRIGATION = false;
    
    options->GW_INIT_FROM_FILE = false;
    options->UH_NSTEPS = 0;
    options->WU_NINPUT_FROM_FILE = 0;
    options->WU_INPUT_FREQUENCY = WU_INPUT_MONTHLY;
    for(i = 0; i < WU_NSECTORS; i++){
        options->WU_COMPENSATION_TIME[i] = 0;
        options->WU_INPUT_LOCATION[i] = WU_INPUT_NONE;
        options->WU_RETURN_LOCATION[i] = WU_RETURN_SURFACEWATER;
    }
    
    options->NIRRTYPES = 0;
    options->NIRRSEASONS = 0;
    
    options->wu_force_offset = 0;
}

/******************************************************************************
 * @brief    Initialize all filenames before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_filenames(ext_filenames_struct *filenames)
{    
    initialize_nameid(&filenames->groundwater);
    initialize_nameid(&filenames->routing);
    initialize_nameid(&filenames->water_use);
    initialize_nameid(&filenames->irrigation);
    
    strcpy(filenames->water_use_forcing_pfx, MISSING_S);
    
    initialize_ext_info(&filenames->info);
}

/******************************************************************************
 * @brief    Initialize all info before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_info(ext_info_struct *info){
    strcpy(info->Ka_expt, "Ka_expt");
    strcpy(info->zwt_init, "zwt_init");
    strcpy(info->Ws_init, "Ws_init");
    strcpy(info->Sy, "Sy");
    
    strcpy(info->flow_direction, "flow_direction");
    strcpy(info->uh_nsteps, "uh_nsteps");
    strcpy(info->uh, "unit_hydrograph");
    
    strcpy(info->demand, "demand");
    strcpy(info->consumption_fraction, "consumption_fraction");
    
    strcpy(info->nirr_dim, "irr_classes");
    strcpy(info->nseason_dim, "grow_seasons");
    strcpy(info->nirr, "nirr");
    strcpy(info->nseason, "nseason");
    strcpy(info->irrigated_class, "irr_veg_class");
    strcpy(info->ponded_class, "pond_veg_class");
    strcpy(info->season_start, "growing_season_start");
    strcpy(info->season_end, "growing_season_end");
}

/******************************************************************************
 * @brief    Initialize all parameters before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_parameters(ext_parameters_struct *parameters)
{          
    
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
 * @brief    Initialize mpi structures
 *****************************************************************************/
void
initialize_ext_mpi()
{    
    extern MPI_Datatype mpi_ext_option_struct_type;
    extern MPI_Datatype mpi_ext_param_struct_type;
    extern int mpi_decomposition;
    
    create_MPI_ext_option_struct_type(&mpi_ext_option_struct_type);
    create_MPI_ext_parameters_struct_type(&mpi_ext_param_struct_type);
    
    mpi_decomposition = MPI_DECOMPOSITION_RANDOM;
}