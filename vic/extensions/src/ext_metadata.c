#include <ext_driver_shared_image.h>

void
ext_set_output_met_data_info(void)
{
    extern ext_option_struct ext_options;
    
    if(ext_options.GROUNDWATER){
        gw_set_output_meta_data_info();
    }
    if(ext_options.ROUTING){
        rout_set_output_meta_data_info();
    }
    if(ext_options.WATER_USE){
        wu_set_output_meta_data_info();
    }
    if(ext_options.DAMS){
        dam_set_output_meta_data_info();
    }
}

void
ext_set_state_meta_data_info(void)
{
    extern ext_option_struct ext_options;
    
    if(ext_options.GROUNDWATER){
        gw_set_state_meta_data_info();
    }
    if(ext_options.ROUTING){
        rout_set_state_meta_data_info();
    }
    if(ext_options.WATER_USE){
        wu_set_state_meta_data_info();
    }
    if(ext_options.DAMS){
        dam_set_state_meta_data_info();
    }
}