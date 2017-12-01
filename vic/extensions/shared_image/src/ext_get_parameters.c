#include <ext_driver_shared_image.h>

bool
ext_get_global_param(char *cmdstr)
{
    if(gw_get_global_parameters(cmdstr)){
        return true;
    }else{
        return false;
    }    
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
}

void
ext_validate_parameters(void)
{
    
}