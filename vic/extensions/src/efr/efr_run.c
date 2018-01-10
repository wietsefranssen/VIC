#include <ext_driver_shared_image.h>

#include "efr.h"

void
efr_run(size_t cur_cell)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern ext_all_vars_struct *ext_all_vars;
    extern wu_con_struct **wu_con;
    
    double demand;
    size_t years_running;
    
    if(dmy[current].month != dmy[current-1].month){
        ext_all_vars[cur_cell].efr.months_running++;
        if(ext_all_vars[cur_cell].efr.months_running > 
                EFR_HIST_YEARS * MONTHS_PER_YEAR){
            ext_all_vars[cur_cell].efr.months_running =
                    EFR_HIST_YEARS * MONTHS_PER_YEAR;
        }
        
        years_running = (size_t)(ext_all_vars[cur_cell].efr.months_running / 
                MONTHS_PER_YEAR);
        if(years_running > EFR_HIST_YEARS){
            years_running = EFR_HIST_YEARS;
        }
        
        // Shift array
        ext_all_vars[cur_cell].efr.history_flow[EFR_HIST_YEARS * MONTHS_PER_YEAR-1] = 0.0;
        cshift(ext_all_vars[cur_cell].efr.history_flow, 1, EFR_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);
        
        // Store monthly average
        ext_all_vars[cur_cell].efr.history_flow[0] =
                ext_all_vars[cur_cell].efr.total_flow / 
                ext_all_vars[cur_cell].efr.total_steps;
        ext_all_vars[cur_cell].efr.total_flow = 0.0;
        ext_all_vars[cur_cell].efr.total_steps = 0;        
        
        // Calculate multi-yearly averages
        ext_all_vars[cur_cell].efr.ay_flow = 
                array_average(ext_all_vars[cur_cell].efr.history_flow,
                years_running, MONTHS_PER_YEAR, 0, 0);
        ext_all_vars[cur_cell].efr.am_flow = 
                array_average(ext_all_vars[cur_cell].efr.history_flow,
                years_running, 1, 0, MONTHS_PER_YEAR - 1);
    }
    
    ext_all_vars[cur_cell].efr.total_flow += 
            ext_all_vars[cur_cell].routing.nat_discharge[0];
    ext_all_vars[cur_cell].efr.total_steps++;
    
    demand = 0.0;
    if(ext_all_vars[cur_cell].efr.ay_flow > 0){
        if(ext_all_vars[cur_cell].routing.nat_discharge[0] <
                ext_all_vars[cur_cell].efr.ay_flow * EFR_LOW_FLOW_FRAC){
            demand = ext_all_vars[cur_cell].routing.nat_discharge[0] * EFR_LOW_DEMAND_FRAC;
        }else if(ext_all_vars[cur_cell].routing.nat_discharge[0] >
                ext_all_vars[cur_cell].efr.ay_flow * EFR_HIGH_FLOW_FRAC){
            demand = ext_all_vars[cur_cell].routing.nat_discharge[0] * EFR_HIGH_DEMAND_FRAC;
        }else{
            demand = linear_interp(ext_all_vars[cur_cell].routing.nat_discharge[0],
                    ext_all_vars[cur_cell].efr.ay_flow * EFR_LOW_FLOW_FRAC,
                    ext_all_vars[cur_cell].efr.ay_flow * EFR_HIGH_FLOW_FRAC,
                    EFR_LOW_DEMAND_FRAC, EFR_HIGH_DEMAND_FRAC) *
                    ext_all_vars[cur_cell].routing.nat_discharge[0];
        }    
    }
    
    wu_con[cur_cell][WU_ENVIRONMENTAL].demand = demand;
    wu_con[cur_cell][WU_ENVIRONMENTAL].consumption_fraction = 0.0; 
}