#include <vic.h>

void
wu_run(size_t cur_cell)
{
    extern option_struct    options;
    extern wu_con_struct  **wu_con;
    extern wu_var_struct  **wu_var;
    extern rout_var_struct *rout_var;

    double                  total_available;
    double                  total_demand;
    double                  withdrawn_fraction;

    size_t                  i;

    // Reset values
    for (i = 0; i < WU_NSECTORS; i++) {
        wu_var[cur_cell][i].demand = wu_con[cur_cell][i].demand;
        wu_var[cur_cell][i].withdrawn = 0.0;
        wu_var[cur_cell][i].consumed = 0.0;
        wu_var[cur_cell][i].returned = 0.0;
    }

    // Get availability
    total_available = rout_var[cur_cell].discharge[0];

    // Satisfy demand
    total_demand = 0;
    for (i = 0; i < WU_NSECTORS; i++) {
        total_demand += wu_var[cur_cell][i].demand;
    }

    withdrawn_fraction = 0.0;
    if (total_available > 0 && total_demand > 0) {
        withdrawn_fraction = total_available / total_demand;
        if (withdrawn_fraction > 1) {
            withdrawn_fraction = 1.0;
        }

        for (i = 0; i < WU_NSECTORS; i++) {
            wu_var[cur_cell][i].withdrawn =
                wu_var[cur_cell][i].demand *
                withdrawn_fraction;
            rout_var[cur_cell].discharge[0] -=
                wu_var[cur_cell][i].withdrawn;
            wu_var[cur_cell][i].consumed +=
                wu_var[cur_cell][i].withdrawn *
                wu_con[cur_cell][i].consumption_fraction;
            wu_var[cur_cell][i].returned +=
                wu_var[cur_cell][i].withdrawn *
                (1 - wu_con[cur_cell][i].consumption_fraction);
        }

        if (rout_var[cur_cell].discharge[0] < 0) {
            rout_var[cur_cell].discharge[0] = 0.0;
        }
    }

    // Return flow
    for (i = 0; i < WU_NSECTORS; i++) {
        if (options.WU_RETURN_LOCATION[i] == WU_RETURN_SURFACEWATER) {
            rout_var[cur_cell].discharge[0] +=
                wu_var[cur_cell][i].returned;
        }
        else if (options.WU_RETURN_LOCATION[i] == WU_RETURN_GROUNDWATER) {
            log_warn("Water use returned to groundwater not yet implemented...");
            rout_var[cur_cell].discharge[0] +=
                wu_var[cur_cell][i].returned;
        }
        else {
            log_err("Unknown water use return location");
        }
    }
}
