#include <ext_driver_shared_image.h>

int
dam_get_op_year_month(double ay_flow, double *am_flow, int current_month)
{
        double con_inflow;
        double max_con_inflow;
        size_t month_nr;
        size_t month_add;
        
        size_t j;
        
        con_inflow = 0.0;
        max_con_inflow = 0.0;
        month_add = 0;
        
        for(j = 0; j < 2 * MONTHS_PER_YEAR; j++){
            month_nr = j % MONTHS_PER_YEAR;
            
            if(am_flow[month_nr] > ay_flow){
                con_inflow += am_flow[month_nr];

                if(con_inflow > max_con_inflow){
                    max_con_inflow = con_inflow;
                    month_add = month_nr;
                }
            }else{
                con_inflow = 0.0;
            }
        }            
        month_add++;  
}

double
dam_calc_discharge(double ay_flow, 
        double am_flow, 
        double amplitude, 
        double offset){
    
    double discharge = 0.0;
    
    discharge = ay_flow + ((am_flow - ay_flow) * amplitude) + offset;
    if(discharge < 0){
        discharge = 0.0;
    }
    
    return discharge;
}

void
dam_calc_year_discharge(double ay_flow, 
        double *am_flow, 
        double amplitude, 
        double offset,
        double *op_dicharge)
{
    size_t i;
    
    for(i = 0; i < MONTHS_PER_YEAR; i ++){
        op_dicharge[i] = dam_calc_discharge(ay_flow, am_flow[i], amplitude, offset);
    }
}

void
dam_get_operation(double ay_flow, 
        double *am_flow, 
        double cur_volume, 
        double pref_volume, 
        double max_volume, 
        double *op_discharge, 
        double *op_volume)
{
    extern global_param_struct global_param;
    
    double amplitude;
    double offset;
    double volume_needed;
    double cur_volume_tmp;
    
    size_t i;
    
    offset = (cur_volume - pref_volume) /
            (DAYS_PER_YEAR * 
            global_param.model_steps_per_day *
            global_param.dt);
    
    for(amplitude = 0; amplitude < 1; amplitude += DAM_AMP_STEP){
        dam_calc_year_discharge(ay_flow, am_flow, amplitude, offset, op_discharge);
        
        volume_needed = 0.0;
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            if(op_discharge[i] > am_flow[i]){
                volume_needed += (op_discharge[i] - am_flow[i]) * 
                        global_param.dt * 
                        global_param.model_steps_per_day * 
                        DAYS_PER_MONTH_AVG;
            }
        }
         
        if(volume_needed < pref_volume){
            break;
        }
    }    
    
    if(volume_needed > pref_volume){
        for(i = 0; i < MONTHS_PER_YEAR; i ++){
            op_discharge[i] = am_flow[i];
        }
    }
    
    cur_volume_tmp = cur_volume;
    if(cur_volume_tmp > max_volume){
        cur_volume_tmp = max_volume;
    }
    
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        if(i == 0){
            op_volume[i] = pref_volume + 
                    ((am_flow[i] - op_discharge[i]) * 
                    global_param.dt * 
                    global_param.model_steps_per_day * 
                    DAYS_PER_MONTH_AVG);
        } else if(i == MONTHS_PER_YEAR - 1) {
            op_volume[i] = pref_volume;
        } else {
            op_volume[i] = op_volume[i-1] + 
                    ((am_flow[i] - op_discharge[i]) * 
                    global_param.dt * 
                    global_param.model_steps_per_day * 
                    DAYS_PER_MONTH_AVG);
        }
        if(op_volume[i] < 0){
            op_volume[i] = 0.0;
        }else if(op_volume[i] > max_volume){
            op_volume[i] = max_volume;
        }
    }
}

void
dam_run(size_t cur_cell)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern global_param_struct global_param;
    extern ext_all_vars_struct *ext_all_vars;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    size_t years_running;
    double ay_flow;
    double am_flow[MONTHS_PER_YEAR];
    double calc_volume;
    double day_step;
    double discharge_correction;
    
    size_t i;
    size_t j;
    
    for(i = 0; i < dam_con_map[cur_cell].nd_active; i++){
        
        if(current > 0 && dmy[current].month != dmy[current-1].month){
            ext_all_vars[cur_cell].dams[i].months_running++;
            if(ext_all_vars[cur_cell].dams[i].months_running > 
                    DAM_HIST_YEARS * MONTHS_PER_YEAR){
                ext_all_vars[cur_cell].dams[i].months_running =
                        DAM_HIST_YEARS * MONTHS_PER_YEAR;
            }
        }
        
        years_running = (size_t)(ext_all_vars[cur_cell].dams[i].months_running / 
                MONTHS_PER_YEAR);
        if(years_running > DAM_HIST_YEARS){
            years_running = DAM_HIST_YEARS;
        }

        if(current > 0 && dmy[current].month != dmy[current-1].month){
            // Store monthly average
            ext_all_vars[cur_cell].dams[i].history_flow[0] =
                    ext_all_vars[cur_cell].dams[i].total_flow / 
                    ext_all_vars[cur_cell].dams[i].total_steps;
            ext_all_vars[cur_cell].dams[i].total_flow = 0.0;
            ext_all_vars[cur_cell].dams[i].total_steps = 0;       
            
            // Calculate averages            
            if(dmy[current].month == ext_all_vars[cur_cell].dams[i].op_year.month){
                ay_flow = array_average(ext_all_vars[cur_cell].dams[i].history_flow,
                    years_running, MONTHS_PER_YEAR, 0, 0);
                for(j = 0; j < MONTHS_PER_YEAR; j++){
                    am_flow[j] = array_average(ext_all_vars[cur_cell].dams[i].history_flow,
                    years_running, 1, j, MONTHS_PER_YEAR - j - 1);
                }
                
                double_flip(am_flow,MONTHS_PER_YEAR);
                
                // Calculate operational year
                ext_all_vars[cur_cell].dams[i].op_year.month = 
                        dam_get_op_year_month(ay_flow, am_flow,
                        ext_all_vars[cur_cell].dams[i].op_year.month);

                // Calculate operation discharge and volume
                dam_get_operation(ay_flow, am_flow, 
                        ext_all_vars[cur_cell].dams[i].volume,
                        dam_con[cur_cell][i].max_volume * DAM_PREF_VOL_FRAC,
                        dam_con[cur_cell][i].max_volume,
                        ext_all_vars[cur_cell].dams[i].op_discharge,
                        ext_all_vars[cur_cell].dams[i].op_volume);
            }

            // Shift array
            cshift(ext_all_vars[cur_cell].dams[i].history_flow, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);
            cshift(ext_all_vars[cur_cell].dams[i].op_discharge, 1, MONTHS_PER_YEAR, 1, 1);
            cshift(ext_all_vars[cur_cell].dams[i].op_volume, 1, MONTHS_PER_YEAR, 1, 1);    
        }

        ext_all_vars[cur_cell].dams[i].total_flow += 
                ext_all_vars[cur_cell].routing.nat_discharge[0];
        ext_all_vars[cur_cell].dams[i].total_steps++;

        // Run dams
        if(dmy[current].year >= dam_con[cur_cell][i].year && years_running > 0){
            
            ext_all_vars[cur_cell].dams[i].discharge = 0.0;
            
            ext_all_vars[cur_cell].dams[i].volume += 
                    ext_all_vars[cur_cell].routing.discharge[0] * 
                    global_param.dt;            
            ext_all_vars[cur_cell].routing.discharge[0] = 0.0;
            
            // Calculate expected volume
            day_step = ext_all_vars[cur_cell].dams[i].total_steps / 
                    global_param.model_steps_per_day;
            if(day_step > (size_t)DAYS_PER_MONTH_AVG){
                day_step = DAYS_PER_MONTH_AVG;
            }
            calc_volume = linear_interp(day_step,
                    0, (size_t)DAYS_PER_MONTH_AVG,
                    ext_all_vars[cur_cell].dams[i].op_volume[MONTHS_PER_YEAR - 1],
                    ext_all_vars[cur_cell].dams[i].op_volume[0]);
            
            // Calculate discharge correction
            discharge_correction = 
                    (ext_all_vars[cur_cell].dams[i].volume - 
                    calc_volume) / (global_param.dt * 
                    global_param.model_steps_per_day * DAYS_PER_WEEK);
            if(abs(discharge_correction) > 
                    ext_all_vars[cur_cell].dams[i].op_discharge[0] * 
                    DAM_DIS_MOD_FRAC){
                if(discharge_correction > 0){
                    discharge_correction = 
                            ext_all_vars[cur_cell].dams[i].op_discharge[0] * 
                            DAM_DIS_MOD_FRAC;
                } else {
                    discharge_correction = 
                            -ext_all_vars[cur_cell].dams[i].op_discharge[0] * 
                            DAM_DIS_MOD_FRAC;
                }
            }
            
            // Release
            ext_all_vars[cur_cell].dams[i].discharge = 
                    ext_all_vars[cur_cell].dams[i].op_discharge[0] +
                    discharge_correction;
            if(ext_all_vars[cur_cell].dams[i].discharge < 0){
                ext_all_vars[cur_cell].dams[i].discharge = 0.0;
            }
            ext_all_vars[cur_cell].dams[i].volume -= 
                    ext_all_vars[cur_cell].dams[i].discharge * 
                    global_param.dt;
            if(ext_all_vars[cur_cell].dams[i].volume < 0){
                ext_all_vars[cur_cell].dams[i].discharge -= 
                        ext_all_vars[cur_cell].dams[i].volume / 
                        global_param.dt;
                ext_all_vars[cur_cell].dams[i].volume = 0.0;
            }
            
            // Overflow
            if(ext_all_vars[cur_cell].dams[i].volume >
                    dam_con[cur_cell][i].max_volume){
                ext_all_vars[cur_cell].dams[i].discharge +=
                        (ext_all_vars[cur_cell].dams[i].volume -
                        dam_con[cur_cell][i].max_volume) / 
                        global_param.dt;            
                ext_all_vars[cur_cell].dams[i].volume = 
                        dam_con[cur_cell][i].max_volume;
            }
                        
            ext_all_vars[cur_cell].routing.discharge[0] += 
                    ext_all_vars[cur_cell].dams[i].discharge;
            
            // Recalculate dam info
            ext_all_vars[cur_cell].dams[i].area = 
                    dam_area(ext_all_vars[cur_cell].dams[i].volume,
                    dam_con[cur_cell][i].max_volume,
                    dam_con[cur_cell][i].max_area,
                    dam_con[cur_cell][i].max_height);
            ext_all_vars[cur_cell].dams[i].height = 
                    dam_height(ext_all_vars[cur_cell].dams[i].area,
                    dam_con[cur_cell][i].max_height);
        }
    }
}

double
dam_area(double volume, double max_volume, double max_area, double max_height){
    // Calculate surface area based on Kaveh et al 2013
    double N = ( 2 * max_volume ) / ( max_height * max_area );
    
    return max_area * pow(( volume / max_volume ), ((2-N)/2));
}

double
dam_height(double area, double max_height){
    // Calculate dam height based on Liebe et al 2005
    double height = sqrt(area) * (1 / 19.45);
    
    if(height > max_height){
        height = max_height;
    }
    
    return height;
}