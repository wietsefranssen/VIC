#include <ext_driver_shared_image.h>

void
irr_set_mapping(void)
{
    extern domain_struct local_domain;
    extern ext_filenames_struct ext_filenames;
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern irr_con_map_struct *irr_con_map;
    extern int mpi_rank;
    extern MPI_Comm            MPI_COMM_VIC;
    
    int *irr_veg_class;
    size_t nirr;
    
    int *ivar;
    int status;
    
    size_t i;
    size_t j;
    
    size_t  d1count;
    size_t  d1start;
    
    d1start = 0;
    d1count = options.NVEGTYPES;
    
    irr_veg_class = malloc(options.NVEGTYPES * sizeof(*irr_veg_class));
    check_alloc_status(irr_veg_class,"Memory allocation error");

    ivar = malloc(options.NVEGTYPES * sizeof(*ivar));
    check_alloc_status(ivar,"Memory allocation error");
        
    // Gather irrigated vegetation classes
    if(mpi_rank == VIC_MPI_ROOT){    
        get_nc_field_int(&(ext_filenames.irrigation), 
                ext_filenames.info.irrigated_class, &d1start, &d1count, ivar);
        for (i = 0; i < options.NVEGTYPES; i++) {
            irr_veg_class[i] = ivar[i];
        }    
    }
    
    status = MPI_Bcast(&irr_veg_class, options.NVEGTYPES, MPI_UNSIGNED_LONG, VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    // Do mapping
    for(i = 0; i < local_domain.ncells_active; i++){
        nirr = 0;
        
        for(j = 0; j < options.NVEGTYPES; j++){
            if(irr_veg_class[j] == 1 && 
                    veg_con_map[i].vidx[j] != NODATA_VEG){
                
                if(nirr > irr_con_map[i].ni_active){
                    log_err("Number of irrigated vegetation classes does not match vegetation classes");
                }
                
                irr_con_map[i].vidx[nirr] = veg_con_map[i].vidx[j];
                nirr++;
            }
        }
    }
    
    free(irr_veg_class);   
    free(ivar);   
}

void
irr_set_ponding(void)
{
    extern domain_struct local_domain;
    extern ext_filenames_struct ext_filenames;
    extern option_struct options;
    extern veg_con_map_struct *veg_con_map;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern int mpi_rank;
    extern MPI_Comm            MPI_COMM_VIC;
    
    int *pond_veg_class;
    size_t nirr;
    
    int *ivar;
    int status;
    
    size_t i;
    size_t j;
    
    size_t  d1count;
    size_t  d1start;
    
    d1start = 0;
    d1count = options.NVEGTYPES;
    
    pond_veg_class = malloc(options.NVEGTYPES * sizeof(*pond_veg_class));
    check_alloc_status(pond_veg_class,"Memory allocation error");
    
    ivar = malloc(options.NVEGTYPES * sizeof(*ivar));
    check_alloc_status(ivar,"Memory allocation error");
    
    // Gather ponded vegetation classes
    if(mpi_rank == VIC_MPI_ROOT){        
        get_nc_field_int(&(ext_filenames.irrigation), 
                ext_filenames.info.ponded_class, &d1start, &d1count, ivar);
        for (i = 0; i < options.NVEGTYPES; i++) {
            pond_veg_class[i] = ivar[i];
        }      
    }
    
    status = MPI_Bcast(&pond_veg_class, options.NVEGTYPES, MPI_UNSIGNED_LONG, VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    // Do mapping
    for(i = 0; i < local_domain.ncells_active; i++){
        nirr = 0;
        
        for(j = 0; j < options.NVEGTYPES; j++){
            if(pond_veg_class[j] == 1 &&
                    veg_con_map[i].vidx[j] != NODATA_VEG){
                
                if(nirr > irr_con_map[i].ni_active){
                    log_err("Number of irrigated vegetation classes does not match vegetation classes");
                }
                
                if(irr_con_map[i].vidx[nirr] == NODATA_VEG){
                    log_err("Ponded vegetation classes does not match irrigated vegetation classes");
                }
                
                irr_con[i][nirr].ponding = true;
                nirr++;
            }
        }
    }
    
    free(pond_veg_class);   
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
    
    irr_set_mapping();
    irr_set_ponding();
    
    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(ext_filenames.irrigation.nc_id);
        check_nc_status(status, "Error closing %s",
                        ext_filenames.irrigation.nc_filename);
    }
}