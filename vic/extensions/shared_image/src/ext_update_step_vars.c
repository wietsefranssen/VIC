#include <ext_driver_shared_image.h>

void
routing_update_step_vars(rout_var_struct *rout_var){
    
    extern ext_option_struct ext_options;  
    
    rout_var->discharge[0] = 0.0;
    cshift(rout_var->discharge, ext_options.uh_steps, 1, 0, 1);
}

void
efr_update_step_vars(efr_var_struct *efr_var){
    
    extern ext_option_struct ext_options;
    
    efr_var->discharge[0] = 0.0;
    cshift(efr_var->discharge, ext_options.uh_steps, 1, 0, 1);    
}

void
dams_update_step_vars(dam_var_struct *dam_var, dam_con_struct dam_con){
    extern ext_option_struct ext_options;
    extern dmy_struct *dmy;
    extern size_t current;
    
    if(dam_var->inflow_offset >= ext_options.model_steps_per_history_step){
        // Model steps per history step has passed
        
        cshift(dam_var->inflow_history, ext_options.history_steps, 1, 0, -1);
        cshift(dam_var->nat_inflow_history, ext_options.history_steps, 1, 0, -1);
                
        dam_var->inflow_history[0] = dam_var->inflow_total / dam_var->inflow_offset;
        dam_var->nat_inflow_history[0] = dam_var->nat_inflow_total / dam_var->inflow_offset;
        dam_var->inflow_total = 0.0;
        dam_var->nat_inflow_total = 0.0;
        dam_var->inflow_offset = 0;
        
        calculate_step_inflow(dam_var);
        calculate_step_nat_inflow(dam_var);
    }
    
    if(current > 0 &&
            dmy[current].month == dam_var->op_year.month && 
            dmy[current].day == dam_var->op_year.day &&
            dmy[current].dayseconds == dam_var->op_year.dayseconds){
        // Operational year has passed
        dam_var->years_running ++;  
        
        cshift(dam_var->inflow_history, ext_options.history_steps, 1, 0, -1);
        cshift(dam_var->nat_inflow_history, ext_options.history_steps, 1, 0, -1);
                
        dam_var->inflow_history[0] = dam_var->inflow_total / dam_var->inflow_offset;
        dam_var->nat_inflow_history[0] = dam_var->nat_inflow_total / dam_var->inflow_offset;
        dam_var->inflow_total = 0.0;
        dam_var->nat_inflow_total = 0.0;
        dam_var->inflow_offset = 0;
        
        // Calculate mean inflows
        calculate_annual_inflow(dam_var);
        calculate_annual_nat_inflow(dam_var);
        calculate_step_inflow(dam_var);
        calculate_step_nat_inflow(dam_var);
        
        // Calculate factors
        calculate_discharge_amplitude(dam_var, dam_con);
        calculate_discharge_offset(dam_var, dam_con);
                
        debug("DAM amp %.2f off %.2f",dam_var->discharge_amplitude,dam_var->discharge_offset);        
    }
    
    dam_var->inflow_offset++;
}