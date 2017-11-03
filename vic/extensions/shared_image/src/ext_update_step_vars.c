#include <ext_driver_shared_image.h>

void
routing_update_step_vars(rout_var_struct *rout_var){
    
    extern ext_option_struct ext_options;  
    
    rout_var->discharge[0] = 0.0;
    rout_var->nat_discharge[0] = 0.0;
    cshift(rout_var->discharge, 1, ext_options.uh_steps, 1, 1);
    cshift(rout_var->nat_discharge, 1, ext_options.uh_steps, 1, 1);
}

void
dams_update_step_vars(dam_var_struct *dam_var, dam_con_struct dam_con){
    extern global_param_struct global_param;
    extern ext_option_struct ext_options;
    extern ext_parameters_struct ext_param;
    extern size_t current;
    extern dmy_struct *dmy;
    
    size_t years;
    
    size_t i;
    
    dam_var->inflow=0.0;
    dam_var->discharge=0.0;
    dam_var->nat_inflow=0.0;    
    dam_var->history_offset++;
    
    // Simulation year has passed
    if(current > 0 && 
            dmy[current].month == global_param.startmonth &&
            dmy[current].day == global_param.startday &&
            dmy[current].dayseconds == global_param.startsec){
        
        dam_var->years_running ++;
    }
    
    // Operational year has passed
    if(current > 0 &&
            dmy[current].month == dam_var->op_year.month && 
            dmy[current].day == dam_var->op_year.day &&
            dmy[current].dayseconds == dam_var->op_year.dayseconds){
                
        dam_var->inflow_history[0] = dam_var->inflow_total / dam_var->history_offset;
        dam_var->nat_inflow_history[0] = dam_var->nat_inflow_total / dam_var->history_offset;
        dam_var->inflow_total = 0.0;
        dam_var->nat_inflow_total = 0.0;
        dam_var->history_offset = 0;
        dam_var->discharge_cor = 0.0;
        
        years = dam_var->years_running;
        if(years > (size_t) ext_param.DAM_HISTORY){
            years = ext_param.DAM_HISTORY;
        }
        
        // Calculate multi-year averages
        calculate_multi_year_average(dam_var->inflow_history, 
                years, 
                ext_options.history_steps_per_history_year,
                0,
                0,
                &dam_var->calc_my_inflow);
        calculate_multi_year_average(dam_var->nat_inflow_history, 
                years, 
                ext_options.history_steps_per_history_year,
                0,
                0,
                &dam_var->calc_my_nat_inflow);
        for(i=0;i<ext_options.history_steps_per_history_year;i++){
            calculate_multi_year_average(dam_var->inflow_history,
                    years,
                    1,
                    i,
                    ext_options.history_steps_per_history_year - i - 1,
                    &dam_var->calc_inflow[i]);
            calculate_multi_year_average(dam_var->nat_inflow_history,
                    years,
                    1,
                    i,
                    ext_options.history_steps_per_history_year - i - 1,
                    &dam_var->calc_nat_inflow[i]);
        }
        
        cshift(dam_var->inflow_history, 1, ext_options.history_steps, 1, -1);
        cshift(dam_var->nat_inflow_history, 1, ext_options.history_steps, 1, -1);
        cshift(dam_var->calc_inflow, 1, ext_options.history_steps_per_history_year, 1, -1);
        cshift(dam_var->calc_nat_inflow, 1, ext_options.history_steps_per_history_year, 1, -1);
        cshift(dam_var->calc_discharge, 1, ext_options.history_steps_per_history_year, 1, -1);
        cshift(dam_var->calc_efr, 1, ext_options.history_steps_per_history_year, 1, -1);
        
        // Calculate operational year
        calculate_operational_year(dam_var->calc_my_inflow, 
                dam_var->calc_inflow, 
                ext_options.history_steps_per_history_year,
                ext_param.DAM_HISTORY_LENGTH,
                &dam_var->op_year);
        
        // Calculate optimal discharge
        calculate_optimal_discharge(dam_con.max_volume * DAM_MAX_PVOLUME, 
                dam_var->volume, 
                dam_var->calc_my_inflow, 
                dam_var->calc_inflow, 
                dam_var->calc_discharge, 
                &dam_var->amplitude, 
                &dam_var->offset);
        
        // Calculate efr        
        calculate_efr(dam_var->calc_nat_inflow,
                ext_options.history_steps_per_history_year,
                dam_var->calc_my_nat_inflow,
                dam_var->calc_efr);
        
        // Ensure efr is being met (when possible)
        calculate_corrected_discharge(dam_var->calc_discharge,
                dam_var->calc_efr, 
                ext_options.history_steps_per_history_year, 
                dam_var->calc_my_inflow);
        
//        debug("op_year %d/%d/%d:%d [%d]",
//                dam_var->op_year.year,
//                dam_var->op_year.month,
//                dam_var->op_year.day,
//                dam_var->op_year.dayseconds,
//                dam_var->op_year.day_in_year);
//        debug("my_inflow %.2f\tamplitude %.2f\toffset %.2f",
//                dam_var->calc_my_inflow,
//                dam_var->amplitude,
//                dam_var->offset);
//        debug("INFLOW\tEFR\tDISCHARGE");
//        for(i=0; i<ext_options.history_steps_per_history_year; i++){
//            debug("%.2f\t%.2f\t%.2f",
//                    dam_var->calc_inflow[i],
//                    dam_var->calc_efr[i],
//                    dam_var->calc_discharge[i]);
//        }
//        debug("test");
    }        
        
    // Operational step has passed
    else if(dam_var->history_offset >= ext_options.model_steps_per_history_step){
                
        dam_var->inflow_history[0] = dam_var->inflow_total / dam_var->history_offset;
        dam_var->nat_inflow_history[0] = dam_var->nat_inflow_total / dam_var->history_offset;
        dam_var->inflow_total = 0.0;
        dam_var->nat_inflow_total = 0.0;
        dam_var->history_offset = 0;
        
        cshift(dam_var->inflow_history, 1, ext_options.history_steps, 1, -1);
        cshift(dam_var->nat_inflow_history, 1, ext_options.history_steps, 1, -1);
        cshift(dam_var->calc_inflow, 1, ext_options.history_steps_per_history_year, 1, -1);
        cshift(dam_var->calc_nat_inflow, 1, ext_options.history_steps_per_history_year, 1, -1);
        cshift(dam_var->calc_discharge, 1, ext_options.history_steps_per_history_year, 1, -1);
        cshift(dam_var->calc_efr, 1, ext_options.history_steps_per_history_year, 1, -1);
    }
}

void 
calculate_multi_year_average(double *history, 
        size_t repetitions, 
        size_t length, 
        size_t offset, 
        size_t skip, 
        double *average){
    
    size_t i;
    size_t j;
    
    (*average) = 0.0;
    for(i=0;i<repetitions;i++){
        for(j=0;j<length;j++){
            (*average) += history[(i * (offset+length+skip)) + offset + j] / (repetitions * length);
        }
    }    
}

void
calculate_operational_year(double my_inflow, 
        double *ms_inflow,
        size_t nsteps,
        size_t days_per_step, 
        dmy_struct *op_dmy){
    
    size_t i;
    
    double op_inflow;
    double max_op_inflow;
    
    size_t add_step;
    size_t new_day_in_year;
    
    op_inflow=0;
    max_op_inflow=0;
    add_step=0;
    
    // Calculate the step with the most consecutive high inflows
    // above the mean yearly average
    for(i = 0; i < 2 * nsteps; i++){

        if(ms_inflow[0] > my_inflow){
            op_inflow += ms_inflow[0];

            if(op_inflow>max_op_inflow){
                max_op_inflow=op_inflow;
                add_step=i;
            }
        }else{
            op_inflow=0;
        }
        
        cshift(ms_inflow, 1, nsteps, 1, -1);
    }
    
    // Change operational year
    add_step = (add_step + 1) % nsteps;
    
    new_day_in_year = op_dmy->day_in_year + add_step * days_per_step;
    if(new_day_in_year > DAYS_PER_YEAR){
        new_day_in_year = op_dmy->day_in_year + (add_step - nsteps) * days_per_step;
    }
    
    dmy_from_day_in_year(new_day_in_year, op_dmy->year, op_dmy);
}

void
calculate_efr(double *flow, 
        size_t nsteps, 
        double annual_flow, 
        double *efr){
    
    size_t i;
    
    for(i=0; i<nsteps; i++){
        efr[i] = calculate_efr_fraction(flow[i],annual_flow) * flow[i];
    }
}

double
calculate_efr_fraction(double flow, 
        double annual_flow){
    if(flow < DAM_EFR_MINF * annual_flow){
        return DAM_EFR_MINR;
    }else if(flow > DAM_EFR_MAXF * annual_flow){        
        return DAM_EFR_MAXR;
    }else{
        return linear_interp((flow / annual_flow),
                DAM_EFR_MINF,DAM_EFR_MAXF,
                DAM_EFR_MINR,DAM_EFR_MAXR);
    }
}

void
calculate_optimal_discharge(double max_volume, 
        double cur_volume, 
        double my_inflow, 
        double *ms_inflow, 
        double *discharge, 
        double *amplitude, 
        double *offset){
    extern ext_option_struct ext_options;
    extern global_param_struct global_param;
    
    double volume_needed;
    
    size_t i;
    
    // Calculate amplitude
    for((*amplitude) = 0; *amplitude <= 1; (*amplitude) += DAM_ASTEP){
        volume_needed = calculate_volume_needed(*amplitude, 
                0, 
                my_inflow, 
                ms_inflow,
                ext_options.history_steps_per_history_year, 
                ext_options.model_steps_per_history_step * global_param.dt);
        if(max_volume >= volume_needed){
            break;
        }
    }
    
    // Calculate offset
    (*offset) = (max_volume - cur_volume) /
            (ext_options.history_steps_per_history_year * 
            ext_options.model_steps_per_history_step * 
            global_param.dt);
    
    // Calculate discharge;
    for(i=0;i<ext_options.history_steps_per_history_year;i++){
        discharge[i] = get_amplitude_discharge(my_inflow, ms_inflow[i], *amplitude, *offset);
    }
}

double
calculate_volume_needed(double amplitude, double offset, double my_inflow, double *ms_inflow, 
        size_t history_length, size_t time_per_history_step){
    
    double discharge;
    double capacity_needed;
    
    size_t i;
    
    capacity_needed = 0.0;
    if(my_inflow > 0){
        for(i = 0; i < history_length; i++){
            discharge = get_amplitude_discharge(my_inflow, ms_inflow[i], amplitude, offset);
            
            if(discharge < ms_inflow[i]){
                capacity_needed += (ms_inflow[i] - discharge) * time_per_history_step;
            }
        }
    }
    
    return capacity_needed;
}

double 
get_amplitude_discharge(double my_inflow, double ms_inflow, double amplitude, double offset){
    return my_inflow +
            ((ms_inflow - my_inflow) * amplitude) + 
            offset;
}

void 
calculate_corrected_discharge(double *discharge, 
        double *efr, 
        size_t nsteps, 
        double my_inflow){   
    
    double cor_needed;
    double cor_available;
    double cor_discharge[nsteps];
    
    size_t i;
    
    // See how much discharge we need to correct for and how much we have
    // to correct the discharge
    cor_needed = 0.0;
    cor_available = 0.0;
    for(i = 0; i < nsteps; i++){
        if(discharge[i] < efr[i]){
            // Dam needs to discharge more
            cor_needed += abs(discharge[i] - efr[i]);
        }else if(discharge[i] > my_inflow){
            // Dam is storing water
        }else{
            // Can reduce discharge this month
            cor_available += discharge[i] - efr[i];
        }
    }
    
    if(cor_available > 0.0 && cor_needed > 0.0){
        for(i=0;i<nsteps;i++){
            if(discharge[i] < efr[i]){
                // Dam needs to discharge more
                cor_discharge[i] = efr[i];
            }else if(discharge[i] > my_inflow){
                // Dam is storing water
                cor_discharge[i] = discharge[i];
            }else{
                // Can reduce discharge this month
                if(cor_needed > cor_available){
                    // Cannot correct for all efr, do as much as possible
                    cor_discharge[i] = efr[i];
                }else{
                    // Reduce discharge in months where correction is possible
                    cor_discharge[i] = discharge[i] - 
                            ((discharge[i] - efr[i]) *
                            (cor_needed / cor_available));
                }
            }        
        }
    
        for(i=0; i<nsteps; i++){
            discharge[i] = cor_discharge[i];
        }
    }
}