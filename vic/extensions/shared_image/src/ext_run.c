#include <ext_driver_shared_image.h>

void
ext_run()
{
    extern domain_struct local_domain;
    extern ext_option_struct ext_options;
    extern ext_all_vars_struct *ext_all_vars;
    extern wu_hist_struct **wu_hist;
    extern wu_con_struct **wu_con;
    extern double ***out_data;
    
    timer_struct               timer;
    size_t i;
        
    for(i=0;i<local_domain.ncells_active;i++){
        if(ext_options.ROUTING){
            routing_update_step_vars(&ext_all_vars[i]);
        }
        if(ext_options.WATER_USE){
            water_use_update_step_vars(&ext_all_vars[i], wu_con[i], wu_hist[i]);
        }
    }
    
    timer_start(&timer);
    if(ext_options.ROUTING){
        routing_run();      
    }
    if(ext_options.WATER_USE){
        local_water_use_run();
    }
    timer_stop(&timer);
    
    for(i=0;i<local_domain.ncells_active;i++){
        ext_put_data(&ext_all_vars[i], out_data[i], &timer);            
    }
}