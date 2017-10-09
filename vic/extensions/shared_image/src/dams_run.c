#include <ext_driver_shared_image.h>

void
calculate_dam_surface_area(dam_con_struct dam_con, dam_var_struct *dam_var){
    // Calculate surface area based on Kaveh et al 2013
    double N = ( 2 * dam_con.max_volume ) / ( dam_con.max_height * dam_con.max_area );
    dam_var->area = dam_con.max_area * pow(( dam_var->volume / dam_con.max_volume ), ((2-N)/2));
}

void
calculate_dam_height(dam_var_struct *dam_var){
    // Calculate dam height based on Liebe et al 2005
    dam_var->height = sqrt(dam_var->area) * (1 / 19.45);
}

void 
calculate_annual_inflow(dam_var_struct *dam_var){
    extern ext_option_struct ext_options;
    extern ext_parameters_struct ext_param;
    
    double ainflow;
    
    size_t i;
    
    ainflow = 0.0;
    if(dam_var->years_running < (size_t)ext_param.DAM_HISTORY){
        for(i=0;i < dam_var->years_running * (DAYS_PER_LYEAR / ext_param.DAM_HISTORY_LENGTH); i++){
            ainflow += dam_var->inflow_history[i];
        }
        ainflow = ainflow / ext_options.history_steps;
    }else{
        for(i=0;i < ext_options.history_steps; i++){
            ainflow += dam_var->inflow_history[i];
        }
        ainflow = ainflow / ext_options.history_steps;
    }
    
    dam_var->annual_inflow = ainflow;    
}

void
calculate_step_inflow(dam_var_struct *dam_var){
    extern ext_option_struct ext_options;
    extern ext_parameters_struct ext_param;
    
    double sinflow;
    
    size_t i;
    
    sinflow = 0.0;
    if(dam_var->years_running < (size_t)ext_param.DAM_HISTORY){
        for(i=0;i < dam_var->years_running; i++){
            sinflow += dam_var->inflow_history[i * (int) ceil(DAYS_PER_LYEAR / ext_param.DAM_HISTORY_LENGTH)];
        }
        sinflow = sinflow / dam_var->years_running;
    }else{
        for(i=0;i < (size_t) ext_param.DAM_HISTORY; i++){
            sinflow += dam_var->inflow_history[i * (int) ceil(DAYS_PER_LYEAR / ext_param.DAM_HISTORY_LENGTH)];
        }
        sinflow = sinflow / ext_param.DAM_HISTORY;
    }
    
    dam_var->step_inflow = sinflow;    
}

void
calculate_outflow_variability(dam_var_struct *dam_var, dam_con_struct dam_con){
    extern ext_parameters_struct ext_param;
    extern ext_option_struct ext_options;
    
    double step_inflow[ext_options.history_steps];
    
    size_t i;
    float f;
    
    for(i=0;i<ext_options.history_steps;i++){
        step_inflow[i] = dam_var->step_inflow;
        
        cshift(dam_var->inflow_history, ext_options.history_steps, 1, 0, 1);
        calculate_step_inflow(dam_var);
    }
    
    for(f = 0; f <= 1; f += DAM_VSTEP){
        dam_var->outflow_offset = f;
        if(dam_con.max_volume <= DAM_PVOLUME * calculate_volume_needed((*dam_var),step_inflow)){
            break;
        }
    }
}

double
calculate_volume_needed(dam_var_struct dam_var, double *step_inflow){
    extern ext_option_struct ext_options;
    
    double discharge;
    double capacity_needed;
    
    size_t i;
    
    capacity_needed = 0.0;
    if(dam_var.annual_inflow > 0){
        for(i=0; i<ext_options.history_steps;i++){
            discharge = dam_var.annual_inflow * (1 - dam_var.outflow_variability) +
                    dam_var.annual_inflow * dam_var.outflow_variability * 
                    (step_inflow[i] / dam_var.annual_inflow) + 
                    dam_var.outflow_offset;
            
            if(discharge - dam_var.annual_inflow > 0){
                capacity_needed += discharge;
            }
        }
    }
    
    return capacity_needed;
}

void
dam_run(dam_con_struct dam_con, dam_var_struct *dam_var, rout_var_struct *rout_var, dmy_struct dmy){
    extern global_param_struct global_param;
    
    
    // Check if dam is run
    if(!dam_var->run){
        if(dam_con.year >= dmy.year){
            dam_var->run = true;
        }else{
            return;
        }
    }
    
    // River discharge adds to dam reservoir volume
    dam_var->inflow_total += rout_var->discharge[0] * global_param.dt;
    dam_var->volume += rout_var->discharge[0] * global_param.dt;
    rout_var->discharge[0] = 0;
    
    // Recalculate dam water area and height
    calculate_dam_surface_area(dam_con,dam_var);
    calculate_dam_height(dam_var);
    
    // Calculate outflow
    dam_var->discharge = 0.0;
    if(dam_var->annual_inflow > 0){
        dam_var->discharge = dam_var->annual_inflow * (1 - dam_var->outflow_variability) +
                dam_var->annual_inflow * dam_var->outflow_variability * 
                (dam_var->step_inflow / dam_var->annual_inflow) + 
                dam_var->outflow_offset;
    }
    
    // Remove outflow from dam reservoir volume and add to discharge
    dam_var->volume -= dam_var->discharge;
    if(dam_var->volume > dam_con.max_volume){
        dam_var->discharge += dam_var->volume - dam_con.max_volume;
        dam_var->volume = dam_con.max_volume;
    }
    rout_var->discharge[0] = dam_var->discharge / global_param.dt;
    
    // Recalculate dam water area and height
    calculate_dam_surface_area(dam_con,dam_var);
    calculate_dam_height(dam_var);
}