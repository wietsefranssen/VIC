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
    if(dam_var->years_running > 0){
        if(dam_var->years_running < (size_t)ext_param.DAM_HISTORY){
            for(i=0;i < dam_var->years_running * ext_options.history_steps_per_history_year; i++){
                ainflow += dam_var->inflow_history[i];
            }
            ainflow = ainflow / (dam_var->years_running * ext_options.history_steps_per_history_year);
        }else{
            for(i=0;i < ext_options.history_steps; i++){
                ainflow += dam_var->inflow_history[i];
            }
            ainflow = ainflow / ext_options.history_steps;
        }
    }
    
    dam_var->annual_inflow = ainflow;    
}

void 
calculate_annual_nat_inflow(dam_var_struct *dam_var){
    extern ext_option_struct ext_options;
    extern ext_parameters_struct ext_param;
    
    double ainflow;
    
    size_t i;
    
    ainflow = 0.0;
    if(dam_var->years_running > 0){
        if(dam_var->years_running < (size_t)ext_param.DAM_HISTORY){
            for(i=0;i < dam_var->years_running * ext_options.history_steps_per_history_year; i++){
                ainflow += dam_var->nat_inflow_history[i];
            }
            ainflow = ainflow / (dam_var->years_running * ext_options.history_steps_per_history_year);
        }else{
            for(i=0;i < ext_options.history_steps; i++){
                ainflow += dam_var->nat_inflow_history[i];
            }
            ainflow = ainflow / ext_options.history_steps;
        }
    }
    
    dam_var->annual_nat_inflow = ainflow;    
}

void
calculate_step_inflow(dam_var_struct *dam_var){
    extern ext_option_struct ext_options;
    extern ext_parameters_struct ext_param;
    
    double sinflow;
    
    size_t i;
    
    sinflow = 0.0;
    if(dam_var->years_running > 0){
        if(dam_var->years_running < (size_t)ext_param.DAM_HISTORY){
            for(i=0;i < dam_var->years_running; i++){
                sinflow += dam_var->inflow_history[i * ext_options.history_steps_per_history_year];
            }
            sinflow = sinflow / dam_var->years_running;
        }else{
            for(i=0;i < (size_t) ext_param.DAM_HISTORY; i++){
                sinflow += dam_var->inflow_history[i * ext_options.history_steps_per_history_year];
            }
            sinflow = sinflow / ext_param.DAM_HISTORY;
        }
    }
    
    dam_var->step_inflow = sinflow;    
}

void
calculate_step_nat_inflow(dam_var_struct *dam_var){
    extern ext_option_struct ext_options;
    extern ext_parameters_struct ext_param;
    
    double sinflow;
    
    size_t i;
    
    sinflow = 0.0;
    if(dam_var->years_running > 0){
        if(dam_var->years_running < (size_t)ext_param.DAM_HISTORY){
            for(i=0;i < dam_var->years_running; i++){
                sinflow += dam_var->nat_inflow_history[i * ext_options.history_steps_per_history_year];
            }
            sinflow = sinflow / dam_var->years_running;
        }else{
            for(i=0;i < (size_t) ext_param.DAM_HISTORY; i++){
                sinflow += dam_var->nat_inflow_history[i * ext_options.history_steps_per_history_year];
            }
            sinflow = sinflow / ext_param.DAM_HISTORY;
        }
    }
    
    dam_var->step_nat_inflow = sinflow;    
}

double
calculate_efr(double flow, double annual_flow){
    return calculate_efr_fraction(flow,annual_flow) * flow;
}

double
calculate_efr_fraction(double flow, double annual_flow){
    if(flow < DAM_EFR_MINF * annual_flow){
        return DAM_EFR_MINR;
    }else if(flow > DAM_EFR_MAXF * annual_flow){        
        return DAM_EFR_MAXR;
    }else{
        return linear_interp((flow / annual_flow),DAM_EFR_MINF,DAM_EFR_MAXF,DAM_EFR_MINR,DAM_EFR_MAXR);
    }
}

void
calculate_discharge_amplitude(dam_var_struct *dam_var, dam_con_struct dam_con){
    extern ext_option_struct ext_options;
    
    double step_inflow[ext_options.history_steps_per_history_year];
    double step_nat_inflow[ext_options.history_steps_per_history_year];
    double efrf[ext_options.history_steps_per_history_year];
    double calc_inflow[ext_options.history_steps_per_history_year];
    
    size_t i;
        
    for(i=0;i<ext_options.history_steps_per_history_year;i++){        
        step_inflow[i] = -1.0;        
        step_nat_inflow[i] = -1.0;        
        efrf[i] = -1.0;        
        calc_inflow[i] = -1.0;
    }
    
    for(i=0;i<ext_options.history_steps_per_history_year;i++){
        cshift(dam_var->inflow_history, ext_options.history_steps, 1, 0, 1);
        cshift(dam_var->nat_inflow_history, ext_options.history_steps, 1, 0, 1);
    }
    
    for(i=0;i<ext_options.history_steps_per_history_year;i++){
        step_inflow[i] = dam_var->step_inflow;
        step_nat_inflow[i] = dam_var->step_nat_inflow;
        
        cshift(dam_var->inflow_history, ext_options.history_steps, 1, 0, -1);
        cshift(dam_var->nat_inflow_history, ext_options.history_steps, 1, 0, -1);
        
        calculate_step_inflow(dam_var);
        calculate_step_nat_inflow(dam_var);
    }
    
    debug("Calculated flows\n"
            "ANN_INFLOW\n"
            "%.2f",
            dam_var->annual_nat_inflow);
    for(i=0;i<ext_options.history_steps_per_history_year;i++){
        efrf[i] = calculate_efr_fraction(step_nat_inflow[i],dam_var->annual_nat_inflow);
        calc_inflow[i] = step_inflow[i] - (efrf[i] * step_nat_inflow[i]);
        debug("%.2f\t%.2f",
                step_nat_inflow[i],
                efrf[i]);
    }    
    
    for(dam_var->discharge_amplitude = 0; dam_var->discharge_amplitude <= 1; dam_var->discharge_amplitude += DAM_ASTEP){
        if(dam_con.max_volume * DAM_PVOLUME >= calculate_volume_needed((*dam_var),calc_inflow,efrf)){
            break;
        }
    }
}

void
calculate_discharge_offset(dam_var_struct *dam_var, dam_con_struct dam_con){
    extern ext_option_struct ext_options;
    
    double difference = dam_con.max_volume * DAM_PVOLUME - dam_var->volume;
    dam_var->discharge_offset = difference / 
            (ext_options.model_steps_per_history_step * ext_options.history_steps_per_history_year);
}

double
calculate_volume_needed(dam_var_struct dam_var, double *inflow, double *efrf){
    extern ext_option_struct ext_options;
    
    double nefrf;
    double discharge;
    double capacity_needed;
    
    size_t i;
    
    capacity_needed = 0.0;
    if(dam_var.annual_inflow > 0){
        for(i=0; i<ext_options.history_steps_per_history_year;i++){
            nefrf = 1 - efrf;
            discharge = (dam_var.annual_inflow * nefrf) * 
                    (1 - dam_var.discharge_amplitude) +
                    (dam_var.annual_inflow - efr[i]) * 
                    dam_var.discharge_amplitude * 
                    (inflow[i] / (dam_var.annual_inflow - efr[i])) + 
                    efr[i];
            
            if(discharge - (inflow[i] + efr[i]) < 0){
                capacity_needed += discharge * ext_options.model_steps_per_history_step;
            }
        }
    }
    
    return capacity_needed;
}

void
dam_run(dam_con_struct dam_con, dam_var_struct *dam_var, rout_var_struct *rout_var, efr_var_struct *efr_var, dmy_struct dmy){
    extern global_param_struct global_param;
                
    // River discharge is saved for history
    dam_var->inflow_total += rout_var->discharge[0] * global_param.dt;
    dam_var->nat_inflow_total += efr_var->discharge[0] * global_param.dt;
    
    // Check if dam is run
    if(!dam_var->run){
        if(dam_con.year <= dmy.year){
            dam_var->run = true;
        }else{
            return;
        }
    }
    
    // River discharge adds to dam reservoir volume
//    dam_var->volume += rout_var->discharge[0] * global_param.dt;
//    rout_var->discharge[0] = 0;
//    
//    // Recalculate dam water area and height
//    calculate_dam_surface_area(dam_con,dam_var);
//    calculate_dam_height(dam_var);
//    
//    // Calculate outflow
//    dam_var->discharge = 0.0;
//    if(dam_var->annual_inflow > 0){
//        dam_var->discharge = dam_var->annual_inflow * (1 - dam_var->discharge_amplitude) +
//                dam_var->annual_inflow * dam_var->discharge_amplitude * 
//                (dam_var->step_inflow / dam_var->annual_inflow) + 
//                dam_var->discharge_offset;
//    }
//    
//    // Remove outflow from dam reservoir volume and add to discharge
//    dam_var->volume -= dam_var->discharge;
//    if(dam_var->volume > dam_con.max_volume){
//        dam_var->discharge += dam_var->volume - dam_con.max_volume;
//        dam_var->volume = dam_con.max_volume;
//    }
//    rout_var->discharge[0] = dam_var->discharge / global_param.dt;
//    
//    // Recalculate dam water area and height
//    calculate_dam_surface_area(dam_con,dam_var);
//    calculate_dam_height(dam_var);
}