#include <ext_driver_shared_image.h>

void
ext_restore(){
    extern int                 mpi_rank;
    extern domain_struct       global_domain;
    extern domain_struct       local_domain;
    extern ext_option_struct    ext_options;
    extern ext_all_vars_struct  *ext_all_vars;
    extern filenames_struct    filenames;
    extern metadata_struct     state_metadata[N_STATE_VARS];

    size_t                     i;
    size_t                     j;
    int                        status;
    double                    *dvar = NULL;
    size_t                     d3count[3];
    size_t                     d3start[3];
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;
    
    if (mpi_rank == VIC_MPI_ROOT) {
        // open initial state file
        status = nc_open(filenames.init_state.nc_filename, NC_NOWRITE,
                         &(filenames.init_state.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.init_state.nc_filename);
    }

    // validate state file dimensions and coordinate variables
    check_init_state_file();
    // read state variables
    
    // allocate memory for variables to be stored
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error");
    
    if(ext_options.ROUTING){
        // discharge
        for (j = 0; j < ext_options.uh_steps; j++) {
            d3start[0] = j;
            get_scatter_nc_field_double(&(filenames.init_state),
                                        state_metadata[STATE_DISCHARGE].varname,
                                        d3start, d3count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                ext_all_vars[i].rout_var.discharge[j] = dvar[i];
            }
        }
        
        // natural discharge
        for (j = 0; j < ext_options.uh_steps; j++) {
            d3start[0] = j;
            get_scatter_nc_field_double(&(filenames.init_state),
                                        state_metadata[STATE_NAT_DISCHARGE].varname,
                                        d3start, d3count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                ext_all_vars[i].rout_var.nat_discharge[j] = dvar[i];
            }
        }
    }
    
    free(dvar);

    // close initial state file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(filenames.init_state.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.init_state.nc_filename);
    }
}

