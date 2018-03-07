#include <vic.h>

void
irr_set_ni_active(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern filenames_struct filenames;
    extern irr_con_map_struct *irr_con_map;
        
    int *ivar;
    
    size_t i;
    
    size_t  d2count[2];
    size_t  d2start[2];
    
    // Get active irrigated vegetation    
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error."); 
        
    get_scatter_nc_field_int(&(filenames.irrigation), 
            "nirr", d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        irr_con_map[i].ni_active = ivar[i];
    }
    
    free(ivar);
}

void
irr_set_mapping(void)
{
    extern domain_struct local_domain;
    extern filenames_struct filenames;
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern irr_con_map_struct *irr_con_map;
    extern int mpi_rank;
    extern MPI_Comm            MPI_COMM_VIC;
        
    int *ivar;
    int status;
    size_t nirr;
    int cur_veg;
    
    size_t i;
    size_t j;
    
    size_t  d1count;
    size_t  d1start;   
    
    // Get irrigated vegetation classes
    d1start = 0;
    d1count = options.NIRRTYPES;
    
    ivar = malloc(options.NIRRTYPES * sizeof(*ivar));
    check_alloc_status(ivar,"Memory allocation error");
        
    // Gather irrigated vegetation classes
    if(mpi_rank == VIC_MPI_ROOT){    
        get_nc_field_int(&(filenames.irrigation), 
                "irrigated_class", &d1start, &d1count, ivar);
    }
    
    status = MPI_Bcast(&ivar, options.NIRRTYPES, MPI_UNSIGNED_LONG, VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    // Do mapping
    for(i = 0; i < local_domain.ncells_active; i++){
        
        nirr = 0;
        for(j = 0; j < (size_t)options.NIRRTYPES; j++){
            cur_veg = veg_con_map[i].vidx[ivar[j] - 1];
            if(veg_con_map[i].vidx[ivar[j] - 1] != NODATA_VEG){                
                if(nirr >= irr_con_map[i].ni_active){
                    log_err("Number of irrigated vegetation classes does not match vegetation classes");
                }
                
                irr_con_map[i].vidx[j] = cur_veg;
                irr_con_map[i].iidx[j] = nirr;
                nirr ++;                
            } else {
                irr_con_map[i].vidx[j] = NODATA_VEG;
                irr_con_map[i].iidx[j] = NODATA_VEG;
            }
        }
    }
    
    free(ivar);
}

void
irr_set_nseasons(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern filenames_struct filenames;
    extern option_struct options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
        
    int *ivar;
    
    size_t i;
    size_t j;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    // Get active irrigated vegetation    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error."); 
        
    for(j = 0; j < (size_t)options.NIRRTYPES; j++){
        d3start[0] = j;
        
        get_scatter_nc_field_int(&(filenames.irrigation), 
                "nseason", d3start, d3count, ivar);
        
        for(i = 0; i < local_domain.ncells_active; i++){
            if(irr_con_map[i].iidx[j] != NODATA_VEG){
                irr_con[i][irr_con_map[i].iidx[j]].nseasons = ivar[i];
            }
        }
    }
    
    free(ivar);
}

void
irr_alloc(void)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern option_struct options;
    extern filenames_struct filenames;
    extern irr_var_struct ***irr_var;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern int mpi_rank;
    
    int status;
    
    size_t i;
    size_t j;
    
    // open parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_open(filenames.irrigation.nc_filename, NC_NOWRITE,
                         &(filenames.irrigation.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.irrigation.nc_filename);
    }
            
    // Allocate cells
    irr_con_map = malloc(local_domain.ncells_active * sizeof(*irr_con_map));
    check_alloc_status(irr_con_map,"Memory allocation error");
    irr_con = malloc(local_domain.ncells_active * sizeof(*irr_con));
    check_alloc_status(irr_con,"Memory allocation error");
    irr_var = malloc(local_domain.ncells_active * sizeof(*irr_var));
    check_alloc_status(irr_var,"Memory allocation error");
    for(i=0; i<local_domain.ncells_active; i++){  
        irr_con_map[i].ni_types = options.NIRRTYPES;
        irr_con_map[i].vidx = malloc(irr_con_map[i].ni_types * sizeof(*irr_con_map[i].vidx));
        check_alloc_status(irr_con_map[i].vidx,"Memory allocation error");        
        irr_con_map[i].iidx = malloc(irr_con_map[i].ni_types * sizeof(*irr_con_map[i].iidx));
        check_alloc_status(irr_con_map[i].iidx,"Memory allocation error");
    }
    
    irr_set_ni_active(); 
    irr_set_mapping();
     
    // Allocate irrigation vegetation
    for(i=0; i<local_domain.ncells_active; i++){
        irr_con[i] = malloc(irr_con_map[i].ni_active * sizeof(*irr_con[i]));
        check_alloc_status(irr_con[i],"Memory allocation error");
        irr_var[i] = malloc(irr_con_map[i].ni_active * sizeof(*irr_var[i]));
        check_alloc_status(irr_var[i],"Memory allocation error"); 
        for(j=0;j<irr_con_map[i].ni_active;j++){
            irr_var[i][j] = malloc(options.SNOW_BAND * sizeof(*irr_var[i][j]));
            check_alloc_status(irr_var[i][j],"Memory allocation error");
        }   
    }
    
    irr_set_nseasons();
    
    // Allocate seasons
    for(i=0; i<local_domain.ncells_active; i++){
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            irr_con[i][j].season_start = malloc(irr_con[i][j].nseasons * sizeof(*irr_con[i][j].season_start));
            check_alloc_status(irr_con[i][j].season_start,"Memory allocation error"); 
            irr_con[i][j].season_end = malloc(irr_con[i][j].nseasons * sizeof(*irr_con[i][j].season_end));
            check_alloc_status(irr_con[i][j].season_end,"Memory allocation error"); 
        }
    }      

    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(filenames.irrigation.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.irrigation.nc_filename);
    }
}

void
irr_finalize(void)
{
    extern domain_struct local_domain;
    extern irr_var_struct ***irr_var;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    
    size_t i;    
    size_t j;
            
    for(i=0; i<local_domain.ncells_active; i++){        
        for(j=0;j<irr_con_map[i].ni_active;j++){
            free(irr_var[i][j]);
            free(irr_con[i][j].season_start);
            free(irr_con[i][j].season_end);
        }
        free(irr_var[i]);
        free(irr_con_map[i].vidx);
        free(irr_con_map[i].iidx);
        free(irr_con[i]);
    }
    free(irr_con_map);
    free(irr_con);
    free(irr_var);
}