#include <vic.h>

void
dam_set_info(void)
{
    extern domain_struct       local_domain;
    extern domain_struct       global_domain;
    extern filenames_struct    filenames;
    extern option_struct       options;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct    **dam_con;

    int                       *ivar;
    double                    *dvar;

    size_t                     i;
    size_t                     j;

    size_t                     d3count[3];
    size_t                     d3start[3];

    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    for (j = 0; j < (size_t)options.MAXDAMS; j++) {
        d3start[0] = j;

        get_scatter_nc_field_int(&(filenames.dams),
                                 "year", d3start, d3count, ivar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (j < dam_con_map[i].nd_active) {
                dam_con[i][j].year = ivar[i];
            }
        }

        get_scatter_nc_field_double(&(filenames.dams),
                                    "max_height", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (j < dam_con_map[i].nd_active) {
                dam_con[i][j].max_height = dvar[i];
            }
        }

        get_scatter_nc_field_double(&(filenames.dams),
                                    "max_area", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (j < dam_con_map[i].nd_active) {
                dam_con[i][j].max_area = dvar[i] * pow(M_PER_KM, 2);
            }
        }

        get_scatter_nc_field_double(&(filenames.dams),
                                    "max_volume", d3start, d3count, dvar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (j < dam_con_map[i].nd_active) {
                dam_con[i][j].max_volume = dvar[i] * pow(M_PER_KM, 2);
            }
        }

        get_scatter_nc_field_int(&(filenames.dams),
                                 "function", d3start, d3count, ivar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if (j < dam_con_map[i].nd_active) {
                dam_con[i][j].function = ivar[i];
            }
        }
    }

    free(ivar);
    free(dvar);
}

void
dam_init(void)
{
    extern filenames_struct filenames;
    extern int              mpi_rank;

    int                     status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(filenames.dams.nc_filename, NC_NOWRITE,
                         &(filenames.dams.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.dams.nc_filename);
    }

    dam_set_info();

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(filenames.dams.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.dams.nc_filename);
    }
}
