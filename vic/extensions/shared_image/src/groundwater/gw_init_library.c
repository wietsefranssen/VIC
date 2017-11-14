#include <ext_driver_shared_image.h>

void
initialize_gw_local_structures(void)
{
    
}

void
initialize_gw_var(gw_var_struct *gw_var)
{
    
}

void
initialize_gw_con(gw_con_struct *gw_con)
{
    
}

void
initialize_gw_parameters(ext_parameters_struct *parameters)
{
    
}

void
initialize_gw_info(ext_info_struct *info)
{
    
}

void
initialize_gw_filenames(ext_filenames_struct *filenames)
{
    initialize_nameid(&filenames->groundwater);
}

void
initialize_gw_options(ext_option_struct *options)
{
    options->GROUNDWATER = false;
}