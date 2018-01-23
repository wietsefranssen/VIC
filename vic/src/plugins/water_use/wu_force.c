#include <vic.h>

void
wu_force(void)
{
    extern size_t              current;
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern dmy_struct         *dmy;
    extern option_struct options;
    extern filenames_struct filenames;
    extern wu_con_struct **wu_con;
    extern int mpi_rank;
    
    bool force;
    int status;
    
    double *dvar;
    
    size_t  d4count[4];
    size_t  d4start[4];
    
    size_t i;
    size_t j;
    
    if(options.WU_NINPUT_FROM_FILE > 0){    
        if (current == 0 ) {
            // calculate offset
            if(options.WU_INPUT_FREQUENCY == WU_INPUT_DAILY){
                options.wu_force_offset = dmy[current].day_in_year - 1;
            } else if (options.WU_INPUT_FREQUENCY == WU_INPUT_MONTHLY){
                options.wu_force_offset = dmy[current].month - 1;
            } else if (options.WU_INPUT_FREQUENCY == WU_INPUT_YEARLY){
                options.wu_force_offset = 0;
            } else {
                log_err("Water use forcing input frequency has not been implemented");
            }

            if (mpi_rank == VIC_MPI_ROOT) {  
                // open new forcing file
                sprintf(filenames.water_use.nc_filename, "%s%4d.nc",
                        filenames.water_use_forcing_pfx, dmy[current].year);        
                status = nc_open(filenames.water_use.nc_filename, NC_NOWRITE,
                                 &(filenames.water_use.nc_id));
                check_nc_status(status, "Error opening %s",
                                filenames.water_use.nc_filename);
            }
        }else if (current > 0 && (dmy[current].year != dmy[current - 1].year)) {
            // reset offset
            options.wu_force_offset = 0;

            if (mpi_rank == VIC_MPI_ROOT) {            
                // close previous forcing file
                status = nc_close(filenames.water_use.nc_id);
                check_nc_status(status, "Error closing %s",
                                filenames.water_use.nc_filename);

                // open new forcing file
                sprintf(filenames.water_use.nc_filename, "%s%4d.nc",
                        filenames.water_use_forcing_pfx, dmy[current].year);        
                status = nc_open(filenames.water_use.nc_filename, NC_NOWRITE,
                                 &(filenames.water_use.nc_id));
                check_nc_status(status, "Error opening %s",
                                filenames.water_use.nc_filename);
            }
        }

        force = false;
        if(options.WU_INPUT_FREQUENCY == WU_INPUT_DAILY){
            if (dmy[current].day_in_year != dmy[current - 1].day_in_year) {
                force = true;
            }
        } else if (options.WU_INPUT_FREQUENCY == WU_INPUT_MONTHLY){
            if (dmy[current].month != dmy[current - 1].month) {
                force = true;
            }
        } else if (options.WU_INPUT_FREQUENCY == WU_INPUT_YEARLY){
            if (dmy[current].year != dmy[current - 1].year) {
                force = true;
            }
        }

        if(force){        
            d4start[0] = 0;
            d4start[1] = 0;
            d4start[2] = options.wu_force_offset;
            d4start[3] = 0;
            d4count[0] = global_domain.n_ny;
            d4count[1] = global_domain.n_nx; 
            d4count[2] = 1; 
            d4count[3] = 1; 

            dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
            check_alloc_status(dvar, "Memory allocation error."); 

            for(j = 0; j < WU_NSECTORS; j++){
                if(options.WU_INPUT_LOCATION[j] == WU_INPUT_FROM_FILE){
                    d4start[3] = j;

                    get_scatter_nc_field_double(&(filenames.water_use), 
                        "consumption_fraction", d4start, d4count, dvar);
                    for (i = 0; i < local_domain.ncells_active; i++) {
                        wu_con[i][j].consumption_fraction = dvar[i];
                    }

                    get_scatter_nc_field_double(&(filenames.water_use), 
                        "demand", d4start, d4count, dvar);
                    for (i = 0; i < local_domain.ncells_active; i++) {
                        wu_con[i][j].demand = dvar[i];
                    }
                }
            }

            options.wu_force_offset++;
        }
    }
}