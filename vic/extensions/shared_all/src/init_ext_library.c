
#include "ext_driver_shared_image.h"

/******************************************************************************
 * @brief    Initialize all options before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_options()
{
    extern ext_option_struct ext_options;
    
    ext_options.ROUTING=false;
    ext_options.DAMS=false;
}

/******************************************************************************
 * @brief    Initialize all options before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_filenames()
{
    extern ext_filenames_struct ext_filenames;

    strcpy(ext_filenames.routing, "MISSING");
}

/******************************************************************************
 * @brief    Initialize all options before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_parameters()
{
    extern ext_parameters_struct ext_param;
    
    ext_param.UH_FLOW_DIFFUSION=800;
    ext_param.UH_FLOW_VELOCITY=1.75;
    ext_param.UH_MAX_LENGTH=2;
    ext_param.UH_PARTITIONS=20;    
}

/******************************************************************************
 * @brief    Initialize global structures
 *****************************************************************************/
void
initialize_ext_global_structures(void)
{  
        initialize_ext_options();
        initialize_ext_filenames();
        initialize_ext_parameters();
}
