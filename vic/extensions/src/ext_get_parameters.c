#include <ext_driver_shared_image.h>

bool
ext_get_global_param(char *cmdstr)
{    
    if(ext_mpi_get_global_parameters(cmdstr)){
        return true;
    }    
    if(gw_get_global_parameters(cmdstr)){
        return true;
    }
    if(rout_get_global_parameters(cmdstr)){
        return true;
    }
    if(wu_get_global_parameters(cmdstr)){
        return true;
    }
    return false;
}

bool
ext_get_parameters(char *cmdstr)
{    
    return false;
}

void
ext_validate_global_parameters(void)
{
    extern ext_option_struct ext_options;
    
    if(ext_options.GROUNDWATER){
        gw_validate_global_parameters();
    }
    if(ext_options.ROUTING){
        rout_validate_global_parameters();
    }  
    if(ext_options.WATER_USE){
        wu_validate_global_parameters();
    }  
}

void
ext_validate_parameters(void)
{
    
}