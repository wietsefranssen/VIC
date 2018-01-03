#include <ext_driver_shared_image.h>

void
ext_update_step_vars(void)
{            
    extern ext_option_struct ext_options;
    
    if(ext_options.GROUNDWATER){
        gw_update_step_vars();
    }
}