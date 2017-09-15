#include <ext_driver_shared_image.h>

void
ext_update_step_vars(ext_all_vars_struct *ext_all_vars){
    
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    
    ext_all_vars->rout_var.discharge[0] = 0.0;
    cshift(ext_all_vars->rout_var.discharge, global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH, 1, 0, 1);
}
