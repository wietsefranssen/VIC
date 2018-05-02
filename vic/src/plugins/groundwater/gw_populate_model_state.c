#include <vic.h>

void
gw_calculate_derived_states(void)
{
    extern domain_struct       local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern soil_con_struct    *soil_con;
    extern option_struct       options;
    extern gw_var_struct    ***gw_var;
    extern all_vars_struct    *all_vars;
    extern gw_con_struct      *gw_con;

    size_t                     i;
    size_t                     j;
    size_t                     k;
    size_t                     l;
    size_t                     m;
    size_t                     n;

    double                     z_tmp;
    double                     ice;
    double                     eff_porosity;
    bool                       in_column;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            for (k = 0; k < elev_con_map[i].ne_active; k++) {
                in_column = false;

                z_tmp = 0.0;
                for (l = 0; l < options.Nlayer; l++) {
                    z_tmp += soil_con[i].depth[l];

                    if (gw_var[i][j][k].zwt < z_tmp &&
                        !in_column) {
                        // groundwater table is in layer
                        in_column = true;

                        // add water for current layer
                        ice = 0.0;
                        for (m = 0; m < options.Nfrost; m++) {
                            ice += all_vars[i].cell[j][k].layer[l].ice[m];
                        }
                        eff_porosity = (soil_con[i].max_moist[l] - ice) /
                                       (soil_con[i].depth[l] * MM_PER_M);

                        gw_var[i][j][k].Wt =
                            (z_tmp - gw_var[i][j][k].zwt) *
                            eff_porosity * MM_PER_M;

                        // add water for lower layers
                        for (n = l + 1; n < options.Nlayer; n++) {
                            z_tmp += soil_con[i].depth[n];

                            ice = 0.0;
                            for (m = 0; m < options.Nfrost; m++) {
                                ice += all_vars[i].cell[j][k].layer[n].ice[m];
                            }
                            eff_porosity = (soil_con[i].max_moist[n] - ice) /
                                           (soil_con[i].depth[n] * MM_PER_M);

                            gw_var[i][j][k].Wt +=
                                soil_con[i].depth[n] *
                                eff_porosity * MM_PER_M;
                        }

                        break;
                    }
                }
                if (!in_column) {
                    gw_var[i][j][k].Wa =
                        (gw_con[i].Za_max - gw_var[i][j][k].zwt) *
                        gw_con[i].Sy * MM_PER_M;
                    gw_var[i][j][k].Wt +=
                        gw_var[i][j][k].Wa;
                }
                else {
                    gw_var[i][j][k].Wa =
                        (gw_con[i].Za_max - z_tmp) *
                        gw_con[i].Sy * MM_PER_M;
                    gw_var[i][j][k].Wt +=
                        gw_var[i][j][k].Wa;
                }
            }
        }
    }
}

void
gw_generate_default_state(void)
{
    extern domain_struct       local_domain;
    extern domain_struct       global_domain;
    extern veg_con_map_struct *veg_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern option_struct       options;
    extern gw_var_struct    ***gw_var;
    extern gw_con_struct      *gw_con;
    extern option_struct       options;
    extern filenames_struct    filenames;
    extern int                 mpi_rank;

    size_t                     i;
    size_t                     j;
    size_t                     k;

    double                    *dvar = NULL;
    size_t                     d2count[2];
    size_t                     d2start[2];
    int                        status;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    if (options.GW_INIT_FROM_FILE) {
        // open parameter file
        if (mpi_rank == VIC_MPI_ROOT) {
            status = nc_open(filenames.groundwater.nc_filename, NC_NOWRITE,
                             &(filenames.groundwater.nc_id));
            check_nc_status(status, "Error opening %s",
                            filenames.groundwater.nc_filename);
        }

        get_scatter_nc_field_double(&(filenames.groundwater),
                                    "zwt_init", d2start, d2count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            for (j = 0; j < veg_con_map[i].nv_active; j++) {
                for (k = 0; k < elev_con_map[i].ne_active; k++) {
                    gw_var[i][j][k].zwt = (double) dvar[i];
                }
            }
        }

        // close parameter file
        if (mpi_rank == VIC_MPI_ROOT) {
            status = nc_close(filenames.groundwater.nc_id);
            check_nc_status(status, "Error closing %s",
                            filenames.groundwater.nc_filename);
        }
    }
    else {
        for (i = 0; i < local_domain.ncells_active; i++) {
            for (j = 0; j < veg_con_map[i].nv_active; j++) {
                for (k = 0; k < elev_con_map[i].ne_active; k++) {
                    gw_var[i][j][k].zwt = gw_con[i].Za_max;
                }
            }
        }
    }

    gw_calculate_derived_states();

    // Free
    free(dvar);
}

void
gw_restore(void)
{
// TODO
// extern domain_struct global_domain;
// extern domain_struct local_domain;
// extern ext_all_vars_struct *ext_all_vars;
// extern option_struct options;
// extern veg_con_map_struct *veg_con_map;
// extern ext_option_struct ext_options;
// extern ext_filenames_struct ext_filenames;
// extern filenames_struct filenames;
// extern metadata_struct state_metadata[N_STATE_VARS];
// extern int mpi_rank;
//
// double                    *dvar = NULL;
//
// int status;
// size_t                     d4count[4];
// size_t                     d4start[4];
// size_t                     d2count[2];
// size_t                     d2start[2];
//
// int                        v;
// size_t                     i;
// size_t                     j;
// size_t                     k;
// size_t                     m;
//
// d4start[0] = 0;
// d4start[1] = 0;
// d4start[2] = 0;
// d4start[3] = 0;
// d4count[0] = 1;
// d4count[1] = 1;
// d4count[2] = global_domain.n_ny;
// d4count[3] = global_domain.n_nx;
// d2start[0] = 0;
// d2start[1] = 0;
// d2count[0] = global_domain.n_ny;
// d2count[1] = global_domain.n_nx;
//
// // Allocate
// dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
// check_alloc_status(dvar, "Memory allocation error");
//
// if(ext_options.GW_INIT_FROM_FILE){
//
// // open parameter file
// if(mpi_rank == VIC_MPI_ROOT){
// status = nc_open(ext_filenames.groundwater.nc_filename, NC_NOWRITE,
// &(ext_filenames.groundwater.nc_id));
// check_nc_status(status, "Error opening %s",
// ext_filenames.groundwater.nc_filename);
// }
//
// get_scatter_nc_field_double(&(ext_filenames.groundwater),
// ext_filenames.info.zwt_init, d2start, d2count, dvar);
//
// for (i = 0; i < local_domain.ncells_active; i++) {
// for (j = 0; j < veg_con_map[i].nv_active; j++) {
// for (k = 0; k < options.SNOW_BAND; k++) {
// ext_all_vars[i].groundwater[j][k].zwt = (double) dvar[i];
// }
// }
// }
//
// get_scatter_nc_field_double(&(ext_filenames.groundwater),
// ext_filenames.info.Ws_init, d2start, d2count, dvar);
//
// for (i = 0; i < local_domain.ncells_active; i++) {
// for (j = 0; j < veg_con_map[i].nv_active; j++) {
// for (k = 0; k < options.SNOW_BAND; k++) {
// ext_all_vars[i].groundwater[j][k].Ws = (double) dvar[i];
// }
// }
// }
//
// // close parameter file
// if(mpi_rank == VIC_MPI_ROOT){
// status = nc_close(ext_filenames.groundwater.nc_id);
// check_nc_status(status, "Error closing %s",
// ext_filenames.groundwater.nc_filename);
// }
//
// }else{
//
// // Read variables from state file
// for (m = 0; m < options.NVEGTYPES; m++) {
// d4start[0] = m;
// for (k = 0; k < options.SNOW_BAND; k++) {
// d4start[1] = k;
// get_scatter_nc_field_double(&(filenames.init_state),
// state_metadata[STATE_GW_ZWT].varname,
// d4start, d4count, dvar);
// for (i = 0; i < local_domain.ncells_active; i++) {
// v = veg_con_map[i].vidx[m];
// if (v >= 0) {
// ext_all_vars[i].groundwater[v][k].zwt = dvar[i];
// }
// }
// }
// }
//
// for (m = 0; m < options.NVEGTYPES; m++) {
// d4start[0] = m;
// for (k = 0; k < options.SNOW_BAND; k++) {
// d4start[1] = k;
// get_scatter_nc_field_double(&(filenames.init_state),
// state_metadata[STATE_GW_WS].varname,
// d4start, d4count, dvar);
// for (i = 0; i < local_domain.ncells_active; i++) {
// v = veg_con_map[i].vidx[m];
// if (v >= 0) {
// ext_all_vars[i].groundwater[v][k].Ws = dvar[i];
// }
// }
// }
// }
//
// }
//
// gw_calculate_derived_states();
//
// // Free
// free(dvar);
}
