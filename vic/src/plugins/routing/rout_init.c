#include <vic.h>

void
rout_set_uh(void)
{
    extern filenames_struct filenames;
    extern option_struct    options;
    extern domain_struct    global_domain;
    extern domain_struct    local_domain;
    extern rout_con_struct *rout_con;

    double                 *dvar = NULL;

    size_t                  i;
    size_t                  j;

    size_t                  d3count[3];
    size_t                  d3start[3];

    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    for (j = 0; j < options.RIRF_NSTEPS; j++) {
        d3start[0] = j;

        get_scatter_nc_field_double(&(filenames.routing),
                                    "river_irf", d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            rout_con[i].river_irf[j] = dvar[i];
        }
    }

    for (j = 0; j < options.GIRF_NSTEPS; j++) {
        d3start[0] = j;

        get_scatter_nc_field_double(&(filenames.routing),
                                    "grid_irf", d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            rout_con[i].grid_irf[j] = dvar[i];
        }
    }

    free(dvar);
}

void
rout_set_direction(void)
{
    extern domain_struct    global_domain;
    extern domain_struct    local_domain;
    extern filenames_struct filenames;
    extern rout_con_struct *rout_con;

    int                    *ivar;

    size_t                  i;

    size_t                  d2count[2];
    size_t                  d2start[2];

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    get_scatter_nc_field_int(&(filenames.routing),
                             "flow_direction", d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].direction = ivar[i];
    }

    free(ivar);
}

size_t
get_downstream_global(size_t id,
                      int    direction)
{
    extern domain_struct global_domain;
    extern size_t       *filter_active_cells;

    size_t               current_io_idx;
    size_t               downstream_io_idx;

    current_io_idx = filter_active_cells[id];

    switch (direction) {
    case 3:
        if (current_io_idx == global_domain.ncells_total - 1) {
            downstream_io_idx = current_io_idx;
        }
        else {
            downstream_io_idx = current_io_idx + 1;
        }
        break;
    case 4:
        if (current_io_idx < global_domain.n_nx - 1) {
            downstream_io_idx = current_io_idx;
        }
        else {
            downstream_io_idx = current_io_idx - global_domain.n_nx + 1;
        }
        break;
    case 5:
        if (current_io_idx < global_domain.n_nx) {
            downstream_io_idx = current_io_idx;
        }
        else {
            downstream_io_idx = current_io_idx - global_domain.n_nx;
        }
        break;
    case 6:
        if (current_io_idx < global_domain.n_nx + 1) {
            downstream_io_idx = current_io_idx;
        }
        else {
            downstream_io_idx = current_io_idx - global_domain.n_nx - 1;
        }
        break;
    case 7:
        if (current_io_idx <= 0) {
            downstream_io_idx = current_io_idx;
        }
        else {
            downstream_io_idx = current_io_idx - 1;
        }
        break;
    case 8:
        if (current_io_idx > global_domain.ncells_total - global_domain.n_nx) {
            downstream_io_idx = current_io_idx;
        }
        else {
            downstream_io_idx = current_io_idx + global_domain.n_nx - 1;
        }
        break;
    case 1:
        if (current_io_idx > global_domain.ncells_total - global_domain.n_nx -
            1) {
            downstream_io_idx = current_io_idx;
        }
        else {
            downstream_io_idx = current_io_idx + global_domain.n_nx;
        }
        break;
    case 2:
        if (current_io_idx > global_domain.ncells_total - global_domain.n_nx -
            2) {
            downstream_io_idx = current_io_idx;
        }
        else {
            downstream_io_idx = current_io_idx + global_domain.n_nx + 1;
        }
        break;
    case 9:
        downstream_io_idx = current_io_idx;
        break;
    default:
        downstream_io_idx = current_io_idx;
        break;
    }

    if (downstream_io_idx >= global_domain.ncells_total) {
        downstream_io_idx = current_io_idx;
    }

    if (global_domain.locations[downstream_io_idx].global_idx == MISSING_USI) {
        downstream_io_idx = current_io_idx;
    }

    return global_domain.locations[downstream_io_idx].global_idx;
}

size_t
get_downstream_local(size_t id,
                     int    direction,
                     size_t n_nx)
{
    extern domain_struct local_domain;

    size_t               current_io_idx;
    size_t               downstream_io_idx;

    size_t               i;

    current_io_idx = local_domain.locations[id].io_idx;

    switch (direction) {
    case 3:
        downstream_io_idx = current_io_idx + 1;
        break;
    case 4:
        downstream_io_idx = current_io_idx - n_nx + 1;
        break;
    case 5:
        downstream_io_idx = current_io_idx - n_nx;
        break;
    case 6:
        downstream_io_idx = current_io_idx - n_nx - 1;
        break;
    case 7:
        downstream_io_idx = current_io_idx - 1;
        break;
    case 8:
        downstream_io_idx = current_io_idx + n_nx - 1;
        break;
    case 1:
        downstream_io_idx = current_io_idx + n_nx;
        break;
    case 2:
        downstream_io_idx = current_io_idx + n_nx + 1;
        break;
    case 9:
        downstream_io_idx = current_io_idx;
        break;
    default:
        downstream_io_idx = current_io_idx;
        break;
    }

    for (i = 0; i < local_domain.ncells_active; i++) {
        if (local_domain.locations[i].io_idx == downstream_io_idx) {
            return i;
        }
    }

    return id;
}

void
rout_set_downstream(void)
{
    extern domain_struct    local_domain;
    extern domain_struct    global_domain;
    extern rout_con_struct *rout_con;
    extern int              mpi_rank;
    extern MPI_Comm         MPI_COMM_VIC;

    size_t                  n_nx;

    int                     status;
    size_t                  i;

    if (mpi_rank == VIC_MPI_ROOT) {
        n_nx = global_domain.n_nx;
    }

    status = MPI_Bcast(&n_nx, 1, MPI_AINT, VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].downstream = get_downstream_local(i, rout_con[i].direction,
                                                      n_nx);
    }
}

void
rout_gl_set_downstream(void)
{
    extern domain_struct    local_domain;
    extern domain_struct    global_domain;
    extern rout_con_struct *rout_con;
    extern int              mpi_rank;

    int                    *dir_global;
    int                    *dir_local;
    int                    *down_global;
    int                    *down_local;
    size_t                  i;

    // Alloc
    if (mpi_rank == VIC_MPI_ROOT) {
        dir_global = malloc(global_domain.ncells_active * sizeof(*dir_global));
        check_alloc_status(dir_global, "Memory allocation error");
        down_global = malloc(global_domain.ncells_active * sizeof(*down_global));
        check_alloc_status(down_global, "Memory allocation error");
    }
    dir_local = malloc(local_domain.ncells_active * sizeof(*dir_local));
    check_alloc_status(dir_local, "Memory allocation error");
    down_local = malloc(local_domain.ncells_active * sizeof(*down_local));
    check_alloc_status(down_local, "Memory allocation error");

    // Get direction
    for (i = 0; i < local_domain.ncells_active; i++) {
        dir_local[i] = rout_con[i].direction;
    }

    // Gather direction
    gather_int(dir_global, dir_local);

    // Get downstream
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            down_global[i] = get_downstream_global(i, dir_global[i]);
        }
    }

    // Scatter downstream
    scatter_int(down_global, down_local);

    // Set downstream
    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].downstream = down_local[i];
    }

    // Free
    if (mpi_rank == VIC_MPI_ROOT) {
        free(dir_global);
        free(down_global);
    }
    free(dir_local);
    free(down_local);
}

void
rout_set_upstream(void)
{
    extern domain_struct    local_domain;
    extern rout_con_struct *rout_con;

    size_t                  upstream[MAX_UPSTREAM];

    size_t                  i;
    size_t                  j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < local_domain.ncells_active; j++) {
            if (rout_con[j].downstream == i && i != j) {
                upstream[rout_con[i].Nupstream] = j;
                rout_con[i].Nupstream++;
            }
        }

        rout_con[i].upstream =
            malloc(rout_con[i].Nupstream * sizeof(*rout_con[i].upstream));
        check_alloc_status(rout_con[i].upstream, "Memory allocation error.");

        for (j = 0; j < rout_con[i].Nupstream; j++) {
            rout_con[i].upstream[j] = upstream[j];
        }
    }
}

void
rout_gl_set_upstream(void)
{
    extern domain_struct    local_domain;
    extern domain_struct    global_domain;
    extern rout_con_struct *rout_con;
    extern int              mpi_rank;

    size_t                  i;
    size_t                  j;

    size_t                 *down_global;
    size_t                 *down_local;
    size_t                **up_global;
    size_t                **up_local;
    size_t                 *nup_global;
    size_t                 *nup_local;

    // Alloc
    if (mpi_rank == VIC_MPI_ROOT) {
        down_global = malloc(global_domain.ncells_active * sizeof(*down_global));
        check_alloc_status(down_global, "Memory allocation error");
        nup_global = malloc(global_domain.ncells_active * sizeof(*nup_global));
        check_alloc_status(nup_global, "Memory allocation error");
        up_global = malloc(global_domain.ncells_active * sizeof(*up_global));
        check_alloc_status(up_global, "Memory allocation error");
        for (i = 0; i < global_domain.ncells_active; i++) {
            up_global[i] = malloc(MAX_UPSTREAM * sizeof(*up_global[i]));
            check_alloc_status(up_global[i], "Memory allocation error");
        }
    }
    down_local = malloc(local_domain.ncells_active * sizeof(*down_local));
    check_alloc_status(down_local, "Memory allocation error");
    nup_local = malloc(local_domain.ncells_active * sizeof(*nup_local));
    check_alloc_status(nup_local, "Memory allocation error");
    up_local = malloc(local_domain.ncells_active * sizeof(*up_local));
    check_alloc_status(up_local, "Memory allocation error");
    for (i = 0; i < local_domain.ncells_active; i++) {
        up_local[i] = malloc(MAX_UPSTREAM * sizeof(*up_local[i]));
        check_alloc_status(up_local[i], "Memory allocation error");
    }

    // Get downstream
    for (i = 0; i < local_domain.ncells_active; i++) {
        down_local[i] = rout_con[i].downstream;
    }

    // Gather downstream
    gather_sizet(down_global, down_local);

    // Get upstream
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            nup_global[i] = 0;

            for (j = 0; j < global_domain.ncells_active; j++) {
                if (down_global[j] == i && i != j) {
                    up_global[i][nup_global[i]] = j;
                    nup_global[i]++;
                }
            }
        }
    }

    // Scatter upstream
    scatter_sizet(nup_global, nup_local);
    scatter_sizet_2d(up_global, up_local, MAX_UPSTREAM);

    // Set upstream
    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].Nupstream = nup_local[i];

        rout_con[i].upstream =
            malloc(rout_con[i].Nupstream * sizeof(*rout_con[i].upstream));
        check_alloc_status(rout_con[i].upstream, "Memory allocation error.");

        for (j = 0; j < rout_con[i].Nupstream; j++) {
            rout_con[i].upstream[j] = up_local[i][j];
        }
    }

    // Free
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            free(up_global[i]);
        }
        free(down_global);
        free(nup_global);
        free(up_global);
    }
    for (i = 0; i < local_domain.ncells_active; i++) {
        free(up_local[i]);
    }
    free(down_local);
    free(nup_local);
    free(up_local);
}

void
rout_set_order()
{
    extern domain_struct    local_domain;
    extern rout_con_struct *rout_con;
    extern size_t          *routing_order;

    bool                    done_tmp[local_domain.ncells_active];
    bool                    done_fin[local_domain.ncells_active];
    size_t                  rank;
    bool                    has_upstream;

    size_t                  i;
    size_t                  j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        done_tmp[i] = false;
        done_fin[i] = false;
    }

    // Set cell_order_local for node
    rank = 0;
    while (rank < local_domain.ncells_active) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (done_fin[i]) {
                continue;
            }

            // count number of upstream cells that are not processed yet
            has_upstream = false;
            for (j = 0; j < rout_con[i].Nupstream; j++) {
                if (!done_fin[rout_con[i].upstream[j]]) {
                    has_upstream = true;
                    break;
                }
            }

            if (has_upstream) {
                continue;
            }

            // if no upstream, add as next order
            routing_order[rank] = i;
            done_tmp[i] = true;
            rank++;

            if (rank > local_domain.ncells_active) {
                log_err("Error in ordering and ranking cells");
            }
        }
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (done_tmp[i] == true) {
                done_fin[i] = true;
            }
        }
    }
}

void
rout_gl_set_order()
{
    extern domain_struct    local_domain;
    extern domain_struct    global_domain;
    extern rout_con_struct *rout_con;
    extern size_t          *routing_order;
    extern int              mpi_rank;

    size_t                **up_global;
    size_t                **up_local;
    size_t                 *nup_global;
    size_t                 *nup_local;

    bool                    done_tmp[global_domain.ncells_active];
    bool                    done_fin[global_domain.ncells_active];

    size_t                  rank;
    bool                    has_upstream;

    size_t                  i;
    size_t                  j;

    // Alloc
    if (mpi_rank == VIC_MPI_ROOT) {
        nup_global = malloc(global_domain.ncells_active * sizeof(*nup_global));
        check_alloc_status(nup_global, "Memory allocation error");
        up_global = malloc(global_domain.ncells_active * sizeof(*up_global));
        check_alloc_status(up_global, "Memory allocation error");
        for (i = 0; i < global_domain.ncells_active; i++) {
            up_global[i] = malloc(MAX_UPSTREAM * sizeof(*up_global[i]));
            check_alloc_status(up_global[i], "Memory allocation error");
        }
    }
    nup_local = malloc(local_domain.ncells_active * sizeof(*nup_local));
    check_alloc_status(nup_local, "Memory allocation error");
    up_local = malloc(local_domain.ncells_active * sizeof(*up_local));
    check_alloc_status(up_local, "Memory allocation error");
    for (i = 0; i < local_domain.ncells_active; i++) {
        up_local[i] = malloc(MAX_UPSTREAM * sizeof(*up_local[i]));
        check_alloc_status(up_local[i], "Memory allocation error");
    }

    // Set nupstream and upstream
    for (i = 0; i < local_domain.ncells_active; i++) {
        nup_local[i] = rout_con[i].Nupstream;

        for (j = 0; j < rout_con[i].Nupstream; j++) {
            up_local[i][j] = rout_con[i].upstream[j];
        }
    }

    // Gather nupstream and upstream
    gather_sizet(nup_global, nup_local);
    gather_sizet_2d(up_global, up_local, MAX_UPSTREAM);

    // Get order
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            done_tmp[i] = false;
            done_fin[i] = false;
        }

        rank = 0;
        while (rank < global_domain.ncells_active) {
            for (i = 0; i < global_domain.ncells_active; i++) {
                if (done_fin[i]) {
                    continue;
                }

                // count number of upstream cells that are not processed yet
                has_upstream = false;
                for (j = 0; j < nup_global[i]; j++) {
                    if (!done_fin[up_global[i][j]]) {
                        has_upstream = true;
                        break;
                    }
                }

                if (has_upstream) {
                    continue;
                }

                // if no upstream, add as next order
                routing_order[rank] = i;
                done_tmp[i] = true;
                rank++;

                if (rank > global_domain.ncells_active) {
                    log_err("Error in ordering and ranking cells");
                }
            }
            for (i = 0; i < global_domain.ncells_active; i++) {
                if (done_tmp[i] == true) {
                    done_fin[i] = true;
                }
            }
        }
    }

    // Free
    if (mpi_rank == VIC_MPI_ROOT) {
        for (i = 0; i < global_domain.ncells_active; i++) {
            free(up_global[i]);
        }
        free(up_global);
        free(nup_global);
    }
    for (i = 0; i < local_domain.ncells_active; i++) {
        free(up_local[i]);
    }
    free(up_local);
    free(nup_local);
}

void
rout_init(void)
{
    extern option_struct    options;
    extern filenames_struct filenames;
    extern int              mpi_rank;

    int                     status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(filenames.routing.nc_filename, NC_NOWRITE,
                         &(filenames.routing.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.routing.nc_filename);
    }

    rout_set_uh();
    rout_set_direction();

    if (options.ROUTING_TYPE == ROUTING_BASIN) {
        rout_set_downstream();
        rout_set_upstream();
        rout_set_order();
    }
    else if (options.ROUTING_TYPE == ROUTING_RANDOM) {
        rout_gl_set_downstream();
        rout_gl_set_upstream();
        rout_gl_set_order();
    }

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(filenames.routing.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.routing.nc_filename);
    }
}
