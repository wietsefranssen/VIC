#include "ext_driver_shared_image.h"

/******************************************************************************
 * @brief    Initialize all options before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_options(ext_option_struct *options)
{    
    options->ROUTING = false;
    
    options->MPI_DECOMPOSITION = RANDOM_DECOMPOSITION;
}

/******************************************************************************
 * @brief    Initialize all options before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_filenames(ext_filenames_struct *filenames)
{
    strcpy(filenames->routing, "MISSING");
}

/******************************************************************************
 * @brief    Initialize all options before they are called by the
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

/******************************************************************************
 * @brief    Initialize global structures
 *****************************************************************************/
void
initialize_ext_global_structures(void)
{  
    extern ext_filenames_struct ext_filenames;
    extern ext_option_struct ext_options;
    extern ext_parameters_struct ext_param;
    
    initialize_ext_options(&ext_options);
    initialize_ext_filenames(&ext_filenames);
    initialize_ext_parameters(&ext_param);
}
