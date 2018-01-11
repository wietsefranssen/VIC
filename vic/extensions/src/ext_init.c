#include <ext_driver_shared_image.h>

void
ext_init(void)
{
    extern ext_option_struct ext_options;
    
    ext_set_state_meta_data_info();
    
    if(ext_options.GROUNDWATER){
        gw_init();
    }
    if(ext_options.ROUTING){
        rout_init();
    }
    if(ext_options.IRRIGATION){
        irr_init();
    }
    if(ext_options.DAMS){
        dam_init();
    }
}
