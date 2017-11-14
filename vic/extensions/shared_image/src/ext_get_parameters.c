#include <ext_driver_shared_image.h>

bool
ext_get_parameters(char *optstr)
{
    return false;
}

void
validate_ext_parameters(){
    extern ext_option_struct ext_options;
    
    if(ext_options.GROUNDWATER){
        validate_gw_parameters();
    }
}