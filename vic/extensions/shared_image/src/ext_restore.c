#include <ext_driver_shared_image.h>

void
ext_restore(){
    extern int                 mpi_rank;
    extern domain_struct       global_domain;
    extern domain_struct       local_domain;
    extern ext_option_struct    ext_options;
    extern ext_all_vars_struct  *ext_all_vars;
    extern ext_parameters_struct ext_param;
    extern filenames_struct    filenames;
    extern metadata_struct     state_metadata[N_STATE_VARS];
    extern dam_con_struct       **dam_con;
    extern dam_con_map_struct  *dam_con_map;

    size_t                     i;
    size_t                     j;
    size_t                     p;
    int                        status;
    double                    *dvar = NULL;
    int                       *ivar = NULL;
    size_t                     d3count[3];
    size_t                     d3start[3];
    size_t                     d4count[4];
    size_t                     d4start[4];
    
    size_t years;
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;
    
    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx;
    
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
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error");
    
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
    
    if(ext_options.DAMS){        
        for (j = 0; j < ext_options.ndams; j++) {
            d3start[0] = j;
            get_scatter_nc_field_double(&(filenames.init_state),
                                        state_metadata[STATE_DAM_VOLUME].varname,
                                        d3start, d3count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if(dam_con_map[i].Ndams>j){
                    ext_all_vars[i].dam_var[j].volume = dvar[i];
                }
            }
        }    
        for (j = 0; j < ext_options.ndams; j++) {
            d3start[0] = j;
            get_scatter_nc_field_int(&(filenames.init_state),
                                        state_metadata[STATE_DAM_YEARS_RUNNING].varname,
                                        d3start, d3count, ivar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if(dam_con_map[i].Ndams>j){
                    ext_all_vars[i].dam_var[j].years_running = ivar[i];
                }
            }
        } 
        for (j = 0; j < ext_options.ndams; j++) {
            d3start[0] = j;
            get_scatter_nc_field_int(&(filenames.init_state),
                                        state_metadata[STATE_DAM_OY_SEC].varname,
                                        d3start, d3count, ivar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if(dam_con_map[i].Ndams>j){
                    ext_all_vars[i].dam_var[j].op_year.dayseconds = ivar[i];
                }
            }
        } 
        for (j = 0; j < ext_options.ndams; j++) {
            d3start[0] = j;
            get_scatter_nc_field_int(&(filenames.init_state),
                                        state_metadata[STATE_DAM_OY_DAY].varname,
                                        d3start, d3count, ivar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if(dam_con_map[i].Ndams>j){
                    ext_all_vars[i].dam_var[j].op_year.day = ivar[i];
                }
            }
        } 
        for (j = 0; j < ext_options.ndams; j++) {
            d3start[0] = j;
            get_scatter_nc_field_int(&(filenames.init_state),
                                        state_metadata[STATE_DAM_OY_MONTH].varname,
                                        d3start, d3count, ivar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if(dam_con_map[i].Ndams>j){
                    ext_all_vars[i].dam_var[j].op_year.month = ivar[i];
                }
            }
        } 
        for (j = 0; j < ext_options.ndams; j++) {
            d3start[0] = j;
            get_scatter_nc_field_int(&(filenames.init_state),
                                        state_metadata[STATE_DAM_OY_YEAR].varname,
                                        d3start, d3count, ivar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if(dam_con_map[i].Ndams>j){
                    ext_all_vars[i].dam_var[j].op_year.year = ivar[i];
                }
            }
        } 
        for (j = 0; j < ext_options.ndams; j++) {
            d3start[0] = j;
            get_scatter_nc_field_double(&(filenames.init_state),
                                        state_metadata[STATE_DAM_INFLOW_TOTAL].varname,
                                        d3start, d3count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if(dam_con_map[i].Ndams>j){
                        ext_all_vars[i].dam_var[j].inflow_total = dvar[i];
                    }
            }
        } 
        for (j = 0; j < ext_options.ndams; j++) {
            d3start[0] = j;
            get_scatter_nc_field_double(&(filenames.init_state),
                                        state_metadata[STATE_DAM_NAT_INFLOW_TOTAL].varname,
                                        d3start, d3count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if(dam_con_map[i].Ndams>j){
                    ext_all_vars[i].dam_var[j].nat_inflow_total = dvar[i];
                }
            }
        } 
        for (j = 0; j < ext_options.ndams; j++) {
            d3start[0] = j;
            get_scatter_nc_field_int(&(filenames.init_state),
                                        state_metadata[STATE_DAM_HIS_OFFSET].varname,
                                        d3start, d3count, ivar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if(dam_con_map[i].Ndams>j){
                    ext_all_vars[i].dam_var[j].history_offset = ivar[i];
                }
            }
        }   
        
        for (j = 0; j < ext_options.ndams; j++) {
            d4start[0] = j;
            for (p = 0; p < ext_options.history_steps; p++) {
                d4start[1] = p;
                get_scatter_nc_field_double(&(filenames.init_state),
                                        state_metadata[STATE_DAM_INFLOW_HIS].varname,
                                        d4start, d4count, dvar);
                for (i = 0; i < local_domain.ncells_active; i++) {
                    if(dam_con_map[i].Ndams>j){
                        ext_all_vars[i].dam_var[j].inflow_history[p] = dvar[i];
                    }
                }
            }
        }
        for (j = 0; j < ext_options.ndams; j++) {
            d4start[0] = j;
            for (p = 0; p < ext_options.history_steps; p++) {
                d4start[1] = p;
                get_scatter_nc_field_double(&(filenames.init_state),
                                        state_metadata[STATE_DAM_NAT_INFLOW_HIS].varname,
                                        d4start, d4count, dvar);
                for (i = 0; i < local_domain.ncells_active; i++) {
                    if(dam_con_map[i].Ndams>j){
                        ext_all_vars[i].dam_var[j].nat_inflow_history[p] = dvar[i];
                    }
                }
            }
        }
        for (j = 0; j < ext_options.ndams; j++) {
            d4start[0] = j;
            for (p = 0; p < ext_options.history_steps_per_history_year; p++) {
                d4start[1] = p;
                get_scatter_nc_field_double(&(filenames.init_state),
                                        state_metadata[STATE_DAM_CALC_DISCHARGE].varname,
                                        d4start, d4count, dvar);
                for (i = 0; i < local_domain.ncells_active; i++) {
                    if(dam_con_map[i].Ndams>j){
                        ext_all_vars[i].dam_var[j].calc_discharge[p] = dvar[i];
                    }
                }
            }
        }
        
        
        for (i = 0; i < local_domain.ncells_active; i++) {
            for(j=0;j<dam_con_map[i].Ndams;j++){
                calculate_dam_surface_area(dam_con[i][j], &ext_all_vars[i].dam_var[j]);
                calculate_dam_height(&ext_all_vars[i].dam_var[j]);
                
                ext_all_vars[i].dam_var[j].op_year.day_in_year = 
                        (int) no_leap_day_in_year_from_dmy(ext_all_vars[i].dam_var[j].op_year);
                
                years = ext_all_vars[i].dam_var[j].years_running;
                if(years > (size_t) ext_param.DAM_HISTORY){
                    years = ext_param.DAM_HISTORY;
                }

                // Calculate multi-year averages
                calculate_multi_year_average(ext_all_vars[i].dam_var[j].inflow_history, 
                        years, 
                        ext_options.history_steps_per_history_year,
                        0,
                        0,
                        &ext_all_vars[i].dam_var[j].calc_my_inflow);
                calculate_multi_year_average(ext_all_vars[i].dam_var[j].nat_inflow_history, 
                        years, 
                        ext_options.history_steps_per_history_year,
                        0,
                        0,
                        &ext_all_vars[i].dam_var[j].calc_my_nat_inflow);
                for(i=0;i<ext_options.history_steps_per_history_year;i++){
                    calculate_multi_year_average(ext_all_vars[i].dam_var[j].inflow_history,
                            years,
                            1,
                            i,
                            ext_options.history_steps_per_history_year - i - 1,
                            &ext_all_vars[i].dam_var[j].calc_inflow[i]);
                    calculate_multi_year_average(ext_all_vars[i].dam_var[j].nat_inflow_history,
                            years,
                            1,
                            i,
                            ext_options.history_steps_per_history_year - i - 1,
                            &ext_all_vars[i].dam_var[j].calc_nat_inflow[i]);
                }
            }
        }        
    }
    
    free(dvar);
    free(ivar);

    // close initial state file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(filenames.init_state.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.init_state.nc_filename);
    }
}

