#include <ext_driver_shared_image.h>

void ext_finalize()
{
    extern ext_option_struct ext_options;
    extern ext_all_vars_struct *ext_all_vars;
    
    if(ext_options.GROUNDWATER){
        gw_finalize();
    }
        
    free(ext_all_vars);
}
