#include <vic.h>

void
irr_run(size_t cur_cell)
{
    extern dmy_struct         *dmy;
    extern size_t              current;
    extern option_struct       options;
    extern all_vars_struct    *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct    **irr_con;
    extern irr_var_struct   ***irr_var;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;
    extern option_struct       options;

    double                     moist[options.Nlayer];
    double                     total_moist;
    double                     total_wcr;
    double                     season_day;
    size_t                     cur_veg;

    size_t                     i;
    size_t                     j;
    size_t                     k;
    size_t                     l;

    for (i = 0; i < irr_con_map[cur_cell].ni_active; i++) {
        cur_veg = irr_con[cur_cell][i].veg_index;

        // Reset values
        for (j = 0; j < options.SNOW_BAND; j++) {
            irr_var[cur_cell][i][j].need = 0.0;
            irr_var[cur_cell][i][j].shortage = 0.0;
        }

        // Check irrigation season
        for (j = 0; j < irr_con[cur_cell][i].nseasons; j++) {
            season_day = between_jday(irr_con[cur_cell][i].season_start[j],
                                      irr_con[cur_cell][i].season_end[j],
                                      dmy[current].day_in_year);

            if (season_day > 0) {
                break;
            }
        }
        if (season_day <= 0.0) {
            for (j = 0; j < options.SNOW_BAND; j++) {
                irr_var[cur_cell][i][j].requirement = 0.0;
                all_vars[cur_cell].cell[cur_veg][j].layer[0].Ksat =
                    soil_con[cur_cell].Ksat[0];

                irr_var[cur_cell][i][j].prev_store =
                    irr_var[cur_cell][i][j].pond_storage;
                irr_var[cur_cell][i][j].prev_req =
                    irr_var[cur_cell][i][j].requirement;
            }

            continue;
        }

        // Run irrigated vegetation
        for (j = 0; j < options.SNOW_BAND; j++) {
            // Get moisture content and critical moisture content of every layer
            for (k = 0; k < options.Nlayer; k++) {
                moist[k] = 0.0;
                for (l = 0; l < options.Nfrost; l++) {
                    moist[k] +=
                        ((all_vars[cur_cell].cell[cur_veg][j].layer[k].moist -
                          all_vars[cur_cell].cell[cur_veg][j].layer[k].
                          ice[l]) *
                         soil_con[cur_cell].frost_fract[l]);
                }
            }

            total_moist = 0.0;
            total_wcr = 0.0;
            for (k = 0; k < options.Nlayer; k++) {
                if (veg_con[cur_cell][cur_veg].root[k] > 0.) {
                    total_moist += moist[k];
                    total_wcr += soil_con[cur_cell].Wcr[k];
                }
            }

            // Get irrigation need and demand
            if (irr_con[cur_cell][i].ponding) {
                all_vars[cur_cell].cell[cur_veg][j].layer[0].Ksat =
                    soil_con[cur_cell].Ksat[0] * POND_KSAT_FRAC;

                if (irr_var[cur_cell][i][j].prev_store <
                    irr_var[cur_cell][i][j].pond_storage) {
                    irr_var[cur_cell][i][j].requirement -=
                        irr_var[cur_cell][i][j].pond_storage -
                        irr_var[cur_cell][i][j].prev_store;

                    if (irr_var[cur_cell][i][j].requirement < 0.0) {
                        irr_var[cur_cell][i][j].requirement = 0.0;
                    }
                }

                if (irr_var[cur_cell][i][j].pond_storage <
                    irr_con[cur_cell][i].pond_capacity * POND_IRR_CRIT_FRAC) {
                    // moisture content is below critical

                    irr_var[cur_cell][i][j].requirement =
                        irr_con[cur_cell][i].pond_capacity -
                        irr_var[cur_cell][i][j].pond_storage;
                }

                irr_var[cur_cell][i][j].prev_store =
                    irr_var[cur_cell][i][j].pond_storage;
            }
            else {
                if (irr_var[cur_cell][i][j].prev_store <
                    total_moist) {
                    irr_var[cur_cell][i][j].requirement -=
                        total_moist -
                        irr_var[cur_cell][i][j].prev_store;

                    if (irr_var[cur_cell][i][j].requirement < 0.0) {
                        irr_var[cur_cell][i][j].requirement = 0.0;
                    }
                }

                if (options.SHARE_LAYER_MOIST) {
                    if (total_moist < total_wcr / IRR_CRIT_FRAC) {
                        // moisture content is below critical

                        irr_var[cur_cell][i][j].requirement =
                            (total_wcr / FIELD_CAP_FRAC) - total_moist;
                    }
                }
                else {
                    bool calc_req = false;

                    for (k = 0; k < options.Nlayer; k++) {
                        // TODO check if requirement should be set to zero
                        if (veg_con[cur_cell][cur_veg].root[k] > 0.) {
                            if (moist[k] < soil_con[cur_cell].Wcr[k] /
                                IRR_CRIT_FRAC) {
                                calc_req = true;
                                break;
                            }
                        }
                    }

                    if (calc_req) {
                        // moisture content is below critical

                        irr_var[cur_cell][i][j].requirement =
                            (total_wcr / FIELD_CAP_FRAC) - total_moist;
                    }
                }

                irr_var[cur_cell][i][j].prev_store =
                    total_moist;
            }

            irr_var[cur_cell][i][j].need =
                irr_var[cur_cell][i][j].requirement -
                irr_var[cur_cell][i][j].prev_req;
            if (irr_var[cur_cell][i][j].need < 0.0) {
                irr_var[cur_cell][i][j].need = 0.0;
            }
            irr_var[cur_cell][i][j].prev_req =
                irr_var[cur_cell][i][j].requirement;

            // Calculate shortage
            irr_var[cur_cell][i][j].shortage = 0.0;
            if (options.SHARE_LAYER_MOIST) {
                if (total_moist < total_wcr &&
                    season_day > irr_con[cur_cell][i].season_offset) {
                    irr_var[cur_cell][i][j].shortage +=
                        total_wcr - total_moist;
                }
            }
            else {
                for (k = 0; k < options.Nlayer; k++) {
                    if (veg_con[cur_cell][cur_veg].root[k] > 0.) {
                        if (moist[k] < soil_con[cur_cell].Wcr[k] &&
                            season_day > irr_con[cur_cell][i].season_offset) {
                            irr_var[cur_cell][i][j].shortage +=
                                soil_con[cur_cell].Wcr[k] - moist[k];
                        }
                    }
                }
            }

            irr_var[cur_cell][i][j].deficit =
                irr_var[cur_cell][i][j].shortage -
                irr_var[cur_cell][i][j].prev_short;
            if (irr_var[cur_cell][i][j].deficit < 0.0) {
                irr_var[cur_cell][i][j].deficit = 0.0;
            }
            irr_var[cur_cell][i][j].prev_short =
                irr_var[cur_cell][i][j].shortage;
        }
    }
}

void
irr_set_demand(size_t cur_cell)
{
    extern domain_struct       local_domain;
    extern global_param_struct global_param;
    extern option_struct       options;
    extern wu_con_struct     **wu_con;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;

    double                     total_demand;
    size_t                     cur_veg;

    size_t                     i;
    size_t                     j;

    if (options.WATER_USE) {
        total_demand = 0;
        for (i = 0; i < irr_con_map[cur_cell].ni_active; i++) {
            cur_veg = irr_con[cur_cell][i].veg_index;

            for (j = 0; j < options.SNOW_BAND; j++) {
                total_demand += irr_var[cur_cell][i][j].requirement *
                                soil_con[cur_cell].AreaFract[j] *
                                veg_con[cur_cell][cur_veg].Cv;
            }
        }

        wu_con[cur_cell][WU_IRRIGATION].demand = total_demand /
                                                 MM_PER_M *
                                                 local_domain.locations[
            cur_cell].area /
                                                 global_param.dt;
        wu_con[cur_cell][WU_IRRIGATION].consumption_fraction = 1.0;
    }
}

void
irr_get_withdrawn(size_t cur_cell)
{
    extern domain_struct       local_domain;
    extern global_param_struct global_param;
    extern all_vars_struct    *all_vars;
    extern option_struct       options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct    **irr_con;
    extern irr_var_struct   ***irr_var;
    extern wu_var_struct     **wu_var;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;
    extern option_struct       options;

    double                     irrigation_available;
    double                     irrigation_need;
    size_t                     cur_veg;

    size_t                     i;
    size_t                     j;

    for (i = 0; i < irr_con_map[cur_cell].ni_active; i++) {
        cur_veg = irr_con[cur_cell][i].veg_index;

        for (j = 0; j < options.SNOW_BAND; j++) {
            if (irr_var[cur_cell][i][j].leftover > 0) {
                // Leftover water for irrigation

                if (irr_con[cur_cell][i].ponding) {
                    irrigation_need = irr_con[cur_cell][i].pond_capacity -
                                      irr_var[cur_cell][i][j].pond_storage;

                    if (irr_var[cur_cell][i][j].leftover >
                        irrigation_need) {
                        // Leftover too much
                        irr_var[cur_cell][i][j].pond_storage =
                            irr_con[cur_cell][i].pond_capacity;
                        irr_var[cur_cell][i][j].leftover -=
                            irrigation_need;
                    }
                    else {
                        // Leftover too little
                        irr_var[cur_cell][i][j].pond_storage +=
                            irr_var[cur_cell][i][j].leftover;
                        irr_var[cur_cell][i][j].leftover = 0;
                    }
                }
                else {
                    irrigation_need = soil_con[cur_cell].max_moist[0] -
                                      all_vars[cur_cell].cell[cur_veg][j].layer
                                      [0].moist;

                    if (irr_var[cur_cell][i][j].leftover >
                        irrigation_need) {
                        // Leftover too much
                        all_vars[cur_cell].cell[cur_veg][j].layer[0].moist =
                            soil_con[cur_cell].max_moist[0];
                        irr_var[cur_cell][i][j].leftover -=
                            irrigation_need;
                    }
                    else {
                        // Leftover too little
                        all_vars[cur_cell].cell[cur_veg][j].layer[0].moist +=
                            irr_var[cur_cell][i][j].leftover;
                        irr_var[cur_cell][i][j].leftover = 0.0;
                    }
                }
            }

            if (irr_var[cur_cell][i][j].pond_storage > 0) {
                // Pond water for irrigation

                irrigation_need = soil_con[cur_cell].max_moist[0] -
                                  all_vars[cur_cell].cell[cur_veg][j].layer[0].
                                  moist;

                if (irr_var[cur_cell][i][j].pond_storage >
                    irrigation_need) {
                    // Pond storage too much
                    all_vars[cur_cell].cell[cur_veg][j].layer[0].moist =
                        soil_con[cur_cell].max_moist[0];
                    irr_var[cur_cell][i][j].pond_storage -=
                        irrigation_need;
                }
                else {
                    // Pond storage too little
                    all_vars[cur_cell].cell[cur_veg][j].layer[0].moist +=
                        irr_var[cur_cell][i][j].pond_storage;
                    irr_var[cur_cell][i][j].pond_storage = 0.0;
                }
            }

            if (options.WATER_USE) {
                irrigation_available =
                    wu_var[cur_cell][WU_IRRIGATION].consumed *
                    global_param.dt / local_domain.locations[cur_cell].area /
                    veg_con[cur_cell][cur_veg].Cv * MM_PER_M;

                if (irrigation_available > 0) {
                    // Received water for irrigation

                    if (irr_con[cur_cell][i].ponding) {
                        irrigation_need = irr_con[cur_cell][i].pond_capacity -
                                          irr_var[cur_cell][i][j].pond_storage;

                        if (irrigation_available > irrigation_need) {
                            // Received too much
                            irr_var[cur_cell][i][j].pond_storage =
                                irr_con[cur_cell][i].pond_capacity;
                            irr_var[cur_cell][i][j].leftover =
                                irrigation_available -
                                irrigation_need;
                        }
                        else {
                            // Received too little
                            irr_var[cur_cell][i][j].pond_storage +=
                                irrigation_available;
                        }
                    }
                    else {
                        irrigation_need = soil_con[cur_cell].max_moist[0] -
                                          all_vars[cur_cell].cell[cur_veg][j].
                                          layer[0].moist;

                        if (irrigation_available > irrigation_need) {
                            // Received too much
                            all_vars[cur_cell].cell[cur_veg][j].layer[0].moist
                                =
                                    soil_con[cur_cell].max_moist[0];
                            irr_var[cur_cell][i][j].leftover =
                                irrigation_available -
                                irrigation_need;
                        }
                        else {
                            // Received too little
                            all_vars[cur_cell].cell[cur_veg][j].layer[0].moist
                                +=
                                    irrigation_available;
                            irrigation_available = 0.0;
                        }
                    }
                }
            }
        }
    }
}
