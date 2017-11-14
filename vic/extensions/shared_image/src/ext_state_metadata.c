#include <ext_driver_shared_image.h>

void
ext_set_state_meta_data_info(){
    extern ext_option_struct ext_options;
    
    if(ext_options.GROUNDWATER){
        gw_set_state_meta_data_info();
    }
}