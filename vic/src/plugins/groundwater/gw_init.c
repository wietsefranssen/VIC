#include <vic.h>

void
gw_set_aquifer(void)
{
    extern domain_struct    local_domain;
    extern domain_struct    global_domain;
    extern filenames_struct filenames;
    extern gw_con_struct   *gw_con;

    double                 *dvar = NULL;

    size_t                  d2count[2];
    size_t                  d2start[2];

    size_t                  i;

    // allocate memory for variables to be read
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    get_scatter_nc_field_double(&(filenames.groundwater),
                                "Ka_expt", d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        gw_con[i].Ka_expt = dvar[i];
    }

    get_scatter_nc_field_double(&(filenames.groundwater),
                                "Sy", d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        gw_con[i].Sy = dvar[i];
    }

    get_scatter_nc_field_double(&(filenames.groundwater),
                                "Qb_max", d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        gw_con[i].Qb_max = dvar[i];
    }

    get_scatter_nc_field_double(&(filenames.groundwater),
                                "Qb_expt", d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        gw_con[i].Qb_expt = dvar[i];
    }

    get_scatter_nc_field_double(&(filenames.groundwater),
                                "Za_max", d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        gw_con[i].Za_max = dvar[i];
    }

    free(dvar);
}

void
gw_init(void)
{
    extern filenames_struct filenames;
    extern int              mpi_rank;

    int                     status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(filenames.groundwater.nc_filename, NC_NOWRITE,
                         &(filenames.groundwater.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.groundwater.nc_filename);
    }

    gw_set_aquifer();

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(filenames.groundwater.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.groundwater.nc_filename);
    }
}
