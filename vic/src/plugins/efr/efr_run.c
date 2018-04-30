#include <vic.h>

#include "efr.h"

double
calc_efr(double ay_flow,
         double discharge)
{
    /* Variable Monthly Flow (VMF) method (Pastor et al., 2014) */
    if (ay_flow > 0) {
        if (discharge < ay_flow * EFR_LOW_FLOW_FRAC) {
            return discharge * EFR_LOW_DEMAND_FRAC;
        }
        else if (discharge > ay_flow * EFR_HIGH_FLOW_FRAC) {
            return discharge * EFR_HIGH_DEMAND_FRAC;
        }
        else {
            return discharge * linear_interp(discharge,
                                             ay_flow * EFR_LOW_FLOW_FRAC,
                                             ay_flow * EFR_HIGH_FLOW_FRAC,
                                             EFR_LOW_DEMAND_FRAC,
                                             EFR_HIGH_DEMAND_FRAC);
        }
    }
    else {
        return 0.0;
    }
}

void
efr_run(size_t cur_cell)
{
    extern dmy_struct      *dmy;
    extern size_t           current;
    extern efr_var_struct  *efr_var;
    extern rout_var_struct *rout_var;

    size_t                  years_running;

    if (current > 0 && dmy[current].month != dmy[current - 1].month) {
        efr_var[cur_cell].months_running++;
        if (efr_var[cur_cell].months_running >
            EFR_HIST_YEARS * MONTHS_PER_YEAR) {
            efr_var[cur_cell].months_running =
                EFR_HIST_YEARS * MONTHS_PER_YEAR;
        }

        years_running = (size_t)(efr_var[cur_cell].months_running /
                                 MONTHS_PER_YEAR);
        if (years_running > EFR_HIST_YEARS) {
            years_running = EFR_HIST_YEARS;
        }

        // Shift array
        efr_var[cur_cell].history_flow[EFR_HIST_YEARS * MONTHS_PER_YEAR -
                                       1] = 0.0;
        cshift(efr_var[cur_cell].history_flow, 1, EFR_HIST_YEARS *
               MONTHS_PER_YEAR, 1,
               -1);

        // Store monthly average
        efr_var[cur_cell].history_flow[0] =
            efr_var[cur_cell].total_flow /
            efr_var[cur_cell].total_steps;
        efr_var[cur_cell].total_flow = 0.0;
        efr_var[cur_cell].total_steps = 0;

        // Calculate multi-yearly averages
        efr_var[cur_cell].ay_flow =
            array_average(efr_var[cur_cell].history_flow,
                          years_running, MONTHS_PER_YEAR, 0, 0);
        efr_var[cur_cell].am_flow =
            array_average(efr_var[cur_cell].history_flow,
                          years_running, 1, 0, MONTHS_PER_YEAR - 1);
    }

    efr_var[cur_cell].total_flow += rout_var[cur_cell].nat_discharge[0];
    efr_var[cur_cell].total_steps++;

    efr_var[cur_cell].requirement = calc_efr(
        efr_var[cur_cell].ay_flow,
        rout_var[cur_cell].nat_discharge[0
        ]);
}

void
efr_set_demand(size_t cur_cell)
{
    extern option_struct   options;
    extern wu_con_struct **wu_con;

    if (options.WATER_USE) {
        wu_con[cur_cell][WU_ENVIRONMENTAL].demand =
            efr_var[cur_cell].requirement;
        wu_con[cur_cell][WU_ENVIRONMENTAL].consumption_fraction = 0.0;
    }
}
