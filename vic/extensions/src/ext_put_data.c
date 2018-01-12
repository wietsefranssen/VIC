#include <ext_driver_shared_image.h>

void
ext_put_data(timer_struct timer)
{
    extern domain_struct local_domain;
    extern ext_option_struct ext_options;
    extern double ***out_data;
    
    size_t i;
    
    for(i=0;i<local_domain.ncells_active;i++){
        out_data[i][OUT_TIME_EXTRUN_WALL][0] = timer.delta_wall;
        out_data[i][OUT_TIME_EXTRUN_CPU][0] = timer.delta_cpu;
    }    
    
    if(ext_options.GROUNDWATER){
        gw_put_data();
    }
    if(ext_options.ROUTING){
        rout_put_data();
    }
    if(ext_options.WATER_USE){
        wu_put_data();
    }
    if(ext_options.DAMS){
        dam_put_data();
    }
}
