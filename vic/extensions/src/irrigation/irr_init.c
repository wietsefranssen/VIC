#include <ext_driver_shared_image.h>

void
irr_set_seasons(void)
{    
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern ext_filenames_struct ext_filenames;
    extern ext_option_struct ext_options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
        
    int *ivar;
    
    size_t i;
    size_t j;
    size_t k;
    
    size_t  d4count[4];
    size_t  d4start[4];
    
    // Get active irrigated vegetation    
    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error."); 
        
    for(k = 0; k < (size_t)ext_options.NIRRSEASONS; k++){
        d4start[0] = k;
        for(j = 0; j < (size_t)ext_options.NIRRTYPES; j++){
            d4start[1] = j;
        
            get_scatter_nc_field_int(&(ext_filenames.irrigation), 
                    ext_filenames.info.season_start, d4start, d4count, ivar);

            for(i = 0; i < local_domain.ncells_active; i++){
                if(irr_con_map[i].iidx[j] != NODATA_VEG && 
                        k < irr_con[i][irr_con_map[i].iidx[j]].nseasons){
                    irr_con[i][irr_con_map[i].iidx[j]].season_start[k].day_in_year = ivar[i];
                }
            }

            get_scatter_nc_field_int(&(ext_filenames.irrigation), 
                    ext_filenames.info.season_end, d4start, d4count, ivar);

            for(i = 0; i < local_domain.ncells_active; i++){
                if(irr_con_map[i].iidx[j] != NODATA_VEG && 
                        k < irr_con[i][irr_con_map[i].iidx[j]].nseasons){
                    irr_con[i][irr_con_map[i].iidx[j]].season_end[k].day_in_year = ivar[i];
                }
            }
        }
    }
    
    for(i = 0; i < local_domain.ncells_active; i++){
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            for(k = 0; k < irr_con[i][j].nseasons; k++){
                irr_con[i][j].season_start[k] = 
                        dmy_from_diy(irr_con[i][j].season_start[k].day_in_year);
                irr_con[i][j].season_end[k] = 
                        dmy_from_diy(irr_con[i][j].season_end[k].day_in_year);
            }
        }
    }
    
    free(ivar);  
}

void
irr_set_ponding(void)
{
    extern domain_struct local_domain;
    extern ext_filenames_struct ext_filenames;
    extern ext_option_struct ext_options;
    extern option_struct options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern int mpi_rank;
    extern MPI_Comm            MPI_COMM_VIC;
    
    int *ivar;
    int status;
    
    size_t i;
    size_t j;
    
    size_t  d1count;
    size_t  d1start;
    
    d1start = 0;
    d1count = ext_options.NIRRTYPES;
    
    ivar = malloc(ext_options.NIRRTYPES * sizeof(*ivar));
    check_alloc_status(ivar,"Memory allocation error");
    
    // Gather ponded vegetation classes
    if(mpi_rank == VIC_MPI_ROOT){        
        get_nc_field_int(&(ext_filenames.irrigation), 
                ext_filenames.info.ponded_class, &d1start, &d1count, ivar);
    }
    
    status = MPI_Bcast(&ivar, options.NVEGTYPES, MPI_UNSIGNED_LONG, VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    // Do mapping
    for(i = 0; i < local_domain.ncells_active; i++){        
        for(j = 0; j < ext_options.NIRRTYPES; j++){
            if(ivar[j] == 1 && irr_con_map[i].iidx[j] != NODATA_VEG){
                irr_con[i][irr_con_map[i].iidx[j]].ponding = true;
                irr_con[i][irr_con_map[i].iidx[j]].pond_capacity = POND_DEF_CAPACITY;
            }
        }
    }
    
    free(ivar);   
}

void
irr_init(void)
{    
    extern ext_filenames_struct ext_filenames;
    extern int mpi_rank;
    
    int status;
    
    // open parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_open(ext_filenames.irrigation.nc_filename, NC_NOWRITE,
                         &(ext_filenames.irrigation.nc_id));
        check_nc_status(status, "Error opening %s",
                        ext_filenames.irrigation.nc_filename);
    }
    
    irr_set_seasons();
    irr_set_ponding();
    
    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(ext_filenames.irrigation.nc_id);
        check_nc_status(status, "Error closing %s",
                        ext_filenames.irrigation.nc_filename);
    }
}