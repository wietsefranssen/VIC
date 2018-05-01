#include <vic.h>
void
rout_gl_run()
{
    extern domain_struct       local_domain;
    extern domain_struct       global_domain;
    extern global_param_struct global_param;
    extern option_struct       options;
    extern rout_var_struct    *rout_var;
    extern rout_con_struct    *rout_con;
    extern double           ***out_data;
    extern size_t             *routing_order;
    extern int                 mpi_rank;

    size_t                    *nup_global;
    size_t                   **up_global;
    double                   **girf_global;
    double                   **rirf_global;
    double                    *run_global;
    double                   **dis_global;
    double                   **ndis_global;
    double                    *store_global;

    size_t                    *nup_local;
    size_t                   **up_local;
    double                   **girf_local;
    double                   **rirf_local;
    double                    *run_local;
    double                   **dis_local;
    double                   **ndis_local;
    double                    *store_local;

    size_t                     cur_cell;
    double                     inflow;
    double                     nat_inflow;
    double                     runoff;

    size_t                     i;
    size_t                     j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_var[i].discharge[0] = 0.0;
        rout_var[i].nat_discharge[0] = 0.0;
        cshift(rout_var[i].discharge, options.RIRF_NSTEPS, 1, 0, 1);
        cshift(rout_var[i].nat_discharge, options.RIRF_NSTEPS, 1, 0, 1);
    }

    // Alloc
    nup_global = malloc(global_domain.ncells_active * sizeof(*nup_global));
    check_alloc_status(nup_global, "Memory allocation error");
    up_global = malloc(global_domain.ncells_active * sizeof(*up_global));
    check_alloc_status(up_global, "Memory allocation error");
    girf_global = malloc(global_domain.ncells_active * sizeof(*girf_global));
    check_alloc_status(girf_global, "Memory allocation error");
    rirf_global = malloc(global_domain.ncells_active * sizeof(*rirf_global));
    check_alloc_status(rirf_global, "Memory allocation error");
    run_global = malloc(global_domain.ncells_active * sizeof(*run_global));
    check_alloc_status(run_global, "Memory allocation error");
    dis_global = malloc(global_domain.ncells_active * sizeof(*dis_global));
    check_alloc_status(dis_global, "Memory allocation error");
    ndis_global = malloc(global_domain.ncells_active * sizeof(*ndis_global));
    check_alloc_status(ndis_global, "Memory allocation error");
    store_global = malloc(global_domain.ncells_active * sizeof(*store_global));
    check_alloc_status(store_global, "Memory allocation error");

    for (i = 0; i < global_domain.ncells_active; i++) {
        up_global[i] = malloc(MAX_UPSTREAM * sizeof(*up_global[i]));
        check_alloc_status(up_global[i], "Memory allocation error");
        girf_global[i] = malloc(options.GIRF_NSTEPS * sizeof(*girf_global[i]));
        check_alloc_status(girf_global[i], "Memory allocation error");
        rirf_global[i] = malloc(options.RIRF_NSTEPS * sizeof(*rirf_global[i]));
        check_alloc_status(rirf_global[i], "Memory allocation error");
        dis_global[i] = malloc(options.RIRF_NSTEPS * sizeof(*dis_global[i]));
        check_alloc_status(dis_global[i], "Memory allocation error");
        ndis_global[i] = malloc(options.RIRF_NSTEPS * sizeof(*ndis_global[i]));
        check_alloc_status(ndis_global[i], "Memory allocation error");
    }

    nup_local = malloc(local_domain.ncells_active * sizeof(*nup_local));
    check_alloc_status(nup_local, "Memory allocation error");
    up_local = malloc(local_domain.ncells_active * sizeof(*up_local));
    check_alloc_status(up_local, "Memory allocation error");
    girf_local = malloc(local_domain.ncells_active * sizeof(*girf_local));
    check_alloc_status(girf_local, "Memory allocation error");
    rirf_local = malloc(local_domain.ncells_active * sizeof(*rirf_local));
    check_alloc_status(rirf_local, "Memory allocation error");
    run_local = malloc(local_domain.ncells_active * sizeof(*run_local));
    check_alloc_status(run_local, "Memory allocation error");
    dis_local = malloc(local_domain.ncells_active * sizeof(*dis_local));
    check_alloc_status(dis_local, "Memory allocation error");
    ndis_local = malloc(local_domain.ncells_active * sizeof(*ndis_local));
    check_alloc_status(ndis_local, "Memory allocation error");
    store_local = malloc(local_domain.ncells_active * sizeof(*store_local));
    check_alloc_status(store_local, "Memory allocation error");

    for (i = 0; i < local_domain.ncells_active; i++) {
        up_local[i] = malloc(MAX_UPSTREAM * sizeof(*up_local[i]));
        check_alloc_status(up_local[i], "Memory allocation error");
        girf_local[i] = malloc(options.GIRF_NSTEPS * sizeof(*girf_local[i]));
        check_alloc_status(girf_local[i], "Memory allocation error");
        rirf_local[i] = malloc(options.RIRF_NSTEPS * sizeof(*rirf_local[i]));
        check_alloc_status(rirf_local[i], "Memory allocation error");
        dis_local[i] = malloc(options.RIRF_NSTEPS * sizeof(*dis_local[i]));
        check_alloc_status(dis_local[i], "Memory allocation error");
        ndis_local[i] = malloc(options.RIRF_NSTEPS * sizeof(*ndis_local[i]));
        check_alloc_status(ndis_local[i], "Memory allocation error");
    }

    // Get
    for (i = 0; i < local_domain.ncells_active; i++) {
        nup_local[i] = rout_con[i].Nupstream;
        for (j = 0; j < rout_con[i].Nupstream; j++) {
            up_local[i][j] = rout_con[i].upstream[j];
        }
        for (j = 0; j < options.GIRF_NSTEPS; j++) {
            girf_local[i][j] = rout_con[i].grid_irf[j];
        }
        for (j = 0; j < options.RIRF_NSTEPS; j++) {
            rirf_local[i][j] = rout_con[i].river_irf[j];
        }
        run_local[i] =
            (out_data[i][OUT_RUNOFF][0] + out_data[i][OUT_BASEFLOW][0]) /
            MM_PER_M * local_domain.locations[i].area /
            global_param.dt;
        for (j = 0; j < options.RIRF_NSTEPS; j++) {
            dis_local[i][j] = rout_var[i].discharge[j];
        }
        for (j = 0; j < options.RIRF_NSTEPS; j++) {
            ndis_local[i][j] = rout_var[i].nat_discharge[j];
        }
        store_local[i] = rout_var[i].storage;
    }

    // Gather
    gather_sizet(nup_global, nup_local);
    gather_sizet_2d(up_global, up_local, MAX_UPSTREAM);
    gather_double_2d(girf_global, girf_local, options.GIRF_NSTEPS);
    gather_double_2d(rirf_global, rirf_local, options.RIRF_NSTEPS);
    gather_double(run_global, run_local);
    gather_double_2d(dis_global, dis_local, options.RIRF_NSTEPS);
    gather_double_2d(ndis_global, ndis_local, options.RIRF_NSTEPS);
    gather_double(store_global, store_local);

    // Calculate
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            cur_cell = routing_order[i];

            inflow = 0;
            for (j = 0; j < nup_global[cur_cell]; j++) {
                inflow += dis_global[up_global[cur_cell][j]][0];
            }

            nat_inflow = 0;
            for (j = 0; j < nup_global[cur_cell]; j++) {
                nat_inflow += ndis_global[up_global[cur_cell][j]][0];
            }

            runoff = 0;
            runoff += run_global[cur_cell];

            rout(inflow, rirf_global[cur_cell], dis_global[cur_cell],
                 options.RIRF_NSTEPS);
            rout(runoff, girf_global[cur_cell], dis_global[cur_cell],
                 options.GIRF_NSTEPS);
            rout(nat_inflow, rirf_global[cur_cell], ndis_global[cur_cell],
                 options.RIRF_NSTEPS);
            rout(runoff, girf_global[cur_cell], ndis_global[cur_cell],
                 options.GIRF_NSTEPS);

            if (dis_global[cur_cell][0] < 0) {
                dis_global[cur_cell][0] = 0.0;
            }
            if (ndis_global[cur_cell][0] < 0) {
                ndis_global[cur_cell][0] = 0.0;
            }

            store_global[cur_cell] += (inflow + runoff) *
                                      global_param.dt /
                                      global_domain.locations[cur_cell].area *
                                      MM_PER_M;
            store_global[cur_cell] -= (dis_global[cur_cell][0]) *
                                      global_param.dt /
                                      global_domain.locations[cur_cell].area *
                                      MM_PER_M;
        }
    }

    // Scatter discharge
    scatter_double_2d(dis_global, dis_local, options.RIRF_NSTEPS);
    scatter_double_2d(ndis_global, ndis_local, options.RIRF_NSTEPS);
    scatter_double(store_global, store_local);

    // Set discharge
    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < options.RIRF_NSTEPS; j++) {
            rout_var[i].discharge[j] = dis_local[i][j];
        }
        for (j = 0; j < options.RIRF_NSTEPS; j++) {
            rout_var[i].nat_discharge[j] = ndis_local[i][j];
        }
        rout_var[i].storage = store_local[i];
    }

    // Free
    for (i = 0; i < global_domain.ncells_active; i++) {
        free(up_global[i]);
        free(girf_global[i]);
        free(rirf_global[i]);
        free(dis_global[i]);
        free(ndis_global[i]);
    }
    free(nup_global);
    free(up_global);
    free(girf_global);
    free(rirf_global);
    free(run_global);
    free(dis_global);
    free(ndis_global);
    free(store_global);

    for (i = 0; i < local_domain.ncells_active; i++) {
        free(up_local[i]);
        free(girf_local[i]);
        free(rirf_local[i]);
        free(dis_local[i]);
        free(ndis_local[i]);
    }
    free(nup_local);
    free(up_local);
    free(girf_local);
    free(rirf_local);
    free(run_local);
    free(dis_local);
    free(ndis_local);
    free(store_local);
}

void
rout_run(size_t cur_cell)
{
    extern domain_struct       local_domain;
    extern global_param_struct global_param;
    extern option_struct       options;
    extern rout_var_struct    *rout_var;
    extern rout_con_struct    *rout_con;
    extern double           ***out_data;

    double                     inflow;
    double                     nat_inflow;
    double                     runoff;

    size_t                     i;

    rout_var[cur_cell].discharge[0] = 0.0;
    rout_var[cur_cell].nat_discharge[0] = 0.0;
    cshift(rout_var[cur_cell].discharge, options.RIRF_NSTEPS, 1, 0, 1);
    cshift(rout_var[cur_cell].nat_discharge, options.RIRF_NSTEPS, 1, 0, 1);

    inflow = 0;
    for (i = 0; i < rout_con[cur_cell].Nupstream; i++) {
        inflow += rout_var[rout_con[cur_cell].upstream[i]].discharge[0];
    }

    nat_inflow = 0;
    for (i = 0; i < rout_con[cur_cell].Nupstream; i++) {
        nat_inflow += rout_var[rout_con[cur_cell].upstream[i]].nat_discharge[0];
    }

    runoff = 0;
    runoff +=
        (out_data[cur_cell][OUT_RUNOFF][0] +
         out_data[cur_cell][OUT_BASEFLOW][0]) /
        MM_PER_M * local_domain.locations[cur_cell].area /
        global_param.dt;

    rout(inflow, rout_con[cur_cell].river_irf, rout_var[cur_cell].discharge,
         options.RIRF_NSTEPS);
    rout(runoff, rout_con[cur_cell].grid_irf, rout_var[cur_cell].discharge,
         options.GIRF_NSTEPS);
    rout(nat_inflow, rout_con[cur_cell].river_irf,
         rout_var[cur_cell].nat_discharge,
         options.RIRF_NSTEPS);
    rout(runoff, rout_con[cur_cell].grid_irf, rout_var[cur_cell].nat_discharge,
         options.GIRF_NSTEPS);

    if (rout_var[cur_cell].discharge[0] < 0) {
        rout_var[cur_cell].discharge[0] = 0.0;
    }
    if (rout_var[cur_cell].nat_discharge[0] < 0) {
        rout_var[cur_cell].nat_discharge[0] = 0.0;
    }

    rout_var[cur_cell].storage += (inflow + runoff) *
                                  global_param.dt /
                                  local_domain.locations[cur_cell].area *
                                  MM_PER_M;
    rout_var[cur_cell].storage -= (rout_var[cur_cell].discharge[0]) *
                                  global_param.dt /
                                  local_domain.locations[cur_cell].area *
                                  MM_PER_M;
}
