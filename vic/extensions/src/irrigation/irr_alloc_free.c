#include <ext_driver_shared_image.h>

void
irr_alloc(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern option_struct options;
    extern ext_filenames_struct ext_filenames;
    extern ext_all_vars_struct *ext_all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern int mpi_rank;
    
    int *ivar;
    int status;
    
    size_t i;
    size_t j;
    
    size_t  d2count[2];
    size_t  d2start[2];
    
    irr_con_map = malloc(local_domain.ncells_active * sizeof(*irr_con_map));
    check_alloc_status(irr_con_map,"Memory allocation error");
    
    // open parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_open(ext_filenames.irrigation.nc_filename, NC_NOWRITE,
                         &(ext_filenames.irrigation.nc_id));
        check_nc_status(status, "Error opening %s",
                        ext_filenames.irrigation.nc_filename);
    }
    
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error."); 
        
    get_scatter_nc_field_int(&(ext_filenames.irrigation), 
            ext_filenames.info.nirr, d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        irr_con_map[i].ni_active = ivar[i];
    }

    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(ext_filenames.irrigation.nc_id);
        check_nc_status(status, "Error closing %s",
                        ext_filenames.irrigation.nc_filename);
    }
    
    irr_con = malloc(local_domain.ncells_active * sizeof(*irr_con));
    check_alloc_status(irr_con,"Memory allocation error");
    
    for(i=0; i<local_domain.ncells_active; i++){        
        irr_con_map[i].vidx = malloc(irr_con_map[i].ni_active * sizeof(*irr_con_map[i].vidx));
        check_alloc_status(irr_con_map[i].vidx,"Memory allocation error");
        
        irr_con[i] = malloc(irr_con_map[i].ni_active * sizeof(*irr_con[i]));
        check_alloc_status(irr_con[i],"Memory allocation error");
        
        ext_all_vars[i].irrigation = malloc(irr_con_map[i].ni_active * sizeof(*ext_all_vars[i].irrigation));
        check_alloc_status(ext_all_vars[i].irrigation,"Memory allocation error");
        
        for(j=0;j<irr_con_map[i].ni_active;j++){
            ext_all_vars[i].irrigation[j] = malloc(options.SNOW_BAND * sizeof(*ext_all_vars[i].irrigation[j]));
            check_alloc_status(ext_all_vars[i].irrigation[j],"Memory allocation error");
        }
    }   
    
    free(ivar);
}

void
irr_finalize(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    
    size_t i;    
    size_t j;
            
    for(i=0; i<local_domain.ncells_active; i++){        
        for(j=0;j<irr_con_map[i].ni_active;j++){
            free(ext_all_vars[i].irrigation[j]);
        }
        free(irr_con_map[i].vidx);
        free(irr_con[i]);
        free(ext_all_vars[i].irrigation);
    }
    free(irr_con_map);
    free(irr_con);
}