#include <ext_driver_shared_image.h>

void
routing_update_step_vars(ext_all_vars_struct *ext_all_vars){
    
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;    
    
    ext_all_vars->rout_var.discharge[0] = 0.0;
    cshift(ext_all_vars->rout_var.discharge, global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH, 1, 0, 1);
}

void
water_use_update_step_vars(ext_all_vars_struct *ext_all_vars, wu_con_struct *wu_con, wu_hist_struct *wu_hist){    
    extern ext_parameters_struct ext_param;    
    
    size_t i;
    
    for(i=0;i<WU_NSECTORS;i++){
        wu_con[i].demand = wu_hist[i].demand[ext_param.wu_hist_offset];
        wu_con[i].consumption_factor = wu_hist[i].consumption_factor[ext_param.wu_hist_offset];

        ext_all_vars->wu_var[i].return_flow[0] = 0.0;
        cshift(ext_all_vars->wu_var[i].return_flow, wu_con[i].delay, 1, 0, 1);
    }
    
    ext_param.wu_hist_offset++;
}
