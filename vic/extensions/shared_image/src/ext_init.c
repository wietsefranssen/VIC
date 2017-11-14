#include <ext_driver_shared_image.h>

void
ext_init(void){
    extern ext_option_struct ext_options;
    
    ext_set_state_meta_data_info();
    
    if(ext_options.GROUNDWATER){
        gw_init();
    }
}
