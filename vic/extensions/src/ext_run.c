#include <ext_driver_shared_image.h>

void
ext_run()
{
    extern ext_option_struct ext_options;
    
    timer_struct timer;
    
    // Update variables locally
    ext_update_step_vars();
       
    timer_start(&timer);
    
    // Run the extensions
    if(ext_options.ROUTING){
        rout_run();
    }
    
    timer_stop(&timer);
    
    // Save the output
    ext_put_data(timer);
}