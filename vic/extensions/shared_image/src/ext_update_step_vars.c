#include <ext_driver_shared_image.h>

void
routing_update_step_vars(rout_var_struct *rout_var){
    
    extern ext_option_struct ext_options;  
    
    rout_var->discharge[0] = 0.0;
    cshift(rout_var->discharge, ext_options.uh_steps, 1, 0, 1);
}

void
dams_update_step_vars(dam_var_struct *dam_var, dam_con_struct dam_con){
    extern ext_option_struct ext_options;
    
    if(dam_var->inflow_history_offset >= ext_options.history_lsteps){
        
        // Shift inflow array
        dam_var->inflow_history[ext_options.history_steps - 1] = 0.0;
        cshift(dam_var->inflow_history, ext_options.history_steps, 1, 0, -1);
        
        // Put inflow
        dam_var->inflow_history[0] = dam_var->inflow_total / dam_var->inflow_history_offset;
        dam_var->inflow_total = 0.0;
        dam_var->inflow_history_offset = 0;
        
        // Calculate mean inflows
        calculate_annual_inflow(dam_var);
        calculate_step_inflow(dam_var);
        
        // Calculate factors
        calculate_outflow_variability(dam_var, dam_con);
        dam_var->outflow_offset = 0.0;
        
    }
    
    dam_var->inflow_history_offset++;
}