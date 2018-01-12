#include <ext_driver_shared_image.h>

void
ext_run()
{
    extern domain_struct local_domain;
    extern ext_option_struct ext_options;
    extern size_t *routing_order;
    
    timer_struct timer;
    size_t cur_cell;
    
    size_t i;
    
    // Update variables locally
    ext_update_step_vars();
       
    timer_start(&timer);
    
    for(i = 0; i < local_domain.ncells_active; i++){
        cur_cell = routing_order[i];
        
        // Run the extensions
        if(ext_options.ROUTING){
            rout_run(cur_cell);
        } 
        if(ext_options.IRRIGATION){
            irr_run1(cur_cell);
        } 
        if(ext_options.EFR){
            efr_run(cur_cell);
        }
        if(ext_options.DAMS){
            dam_run(cur_cell);
        }
        if(ext_options.WATER_USE){
            wu_run(cur_cell);
        }
        if(ext_options.IRRIGATION){
            irr_run2(cur_cell);
        }
            
    }
    
    timer_stop(&timer);
    
    // Save the output
    ext_put_data(timer);
}