#include <ext_driver_shared_image.h>

void
routing_update_step_vars(ext_all_vars_struct *ext_all_vars){
    
    extern ext_option_struct ext_options;  
    
    ext_all_vars->rout_var.discharge[0] = 0.0;
    cshift(ext_all_vars->rout_var.discharge, ext_options.uh_steps, 1, 0, 1);
}
