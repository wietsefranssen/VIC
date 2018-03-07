#include <vic.h>

void
irr_set_vegetation(void)
{
    extern domain_struct local_domain;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    
    size_t i;
    size_t j;
    
    for(i = 0; i < local_domain.ncells_active; i++){
        for(j = 0; j < irr_con_map[i].ni_types; j++){
            if(irr_con_map[i].iidx[j] != NODATA_VEG){
                irr_con[i][irr_con_map[i].iidx[j]].veg_index = irr_con_map[i].vidx[j];
            }
        }
    }
}

void
irr_set_seasons(void)
{    
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern filenames_struct filenames;
    extern option_struct options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern veg_con_struct **veg_con;
        
    double *dvar;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    
    size_t  d3count[3];
    size_t  d3start[3];
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
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx; 
    
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error."); 
        
    for(k = 0; k < (size_t)options.NIRRSEASONS; k++){
        d4start[0] = k;
        for(j = 0; j < (size_t)options.NIRRTYPES; j++){
            d4start[1] = j;
        
            get_scatter_nc_field_double(&(filenames.irrigation), 
                    "season_start", d4start, d4count, dvar);

            for(i = 0; i < local_domain.ncells_active; i++){
                if(irr_con_map[i].iidx[j] != NODATA_VEG && 
                        k < irr_con[i][irr_con_map[i].iidx[j]].nseasons){
                    irr_con[i][irr_con_map[i].iidx[j]].season_start[k] = dvar[i];
                }
            }

            get_scatter_nc_field_double(&(filenames.irrigation), 
                    "season_end", d4start, d4count, dvar);

            for(i = 0; i < local_domain.ncells_active; i++){
                if(irr_con_map[i].iidx[j] != NODATA_VEG && 
                        k < irr_con[i][irr_con_map[i].iidx[j]].nseasons){
                    irr_con[i][irr_con_map[i].iidx[j]].season_end[k] = dvar[i];
                }
            }

            get_scatter_nc_field_double(&(filenames.irrigation), 
                    "season_offset", d3start, d3count, dvar);

            for(i = 0; i < local_domain.ncells_active; i++){
                if(irr_con_map[i].iidx[j] != NODATA_VEG){
                    irr_con[i][irr_con_map[i].iidx[j]].season_offset = dvar[i];
                }
            }
        }
    }
    
    // Apply offset
    for(i = 0; i < local_domain.ncells_active; i++){
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            if(irr_con[i][j].season_offset < 0.0){
                log_err("Season offset is smaller than 0, can only be larger");
            }
            
            for(k = 0; k < irr_con[i][j].nseasons; k++){
                irr_con[i][j].season_start[k] -= irr_con[i][j].season_offset;
                if(irr_con[i][j].season_start[k] < 0){
                    irr_con[i][j].season_start[k] += DAYS_PER_JYEAR;
                }
                irr_con[i][j].season_end[k] -= irr_con[i][j].season_offset;
                if(irr_con[i][j].season_end[k] < 0){
                    irr_con[i][j].season_end[k] += DAYS_PER_JYEAR;
                }
            }
        }
    }
    
    
    // Check for overlap
    for(i = 0; i < local_domain.ncells_active; i++){
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            for(k = 0; k < irr_con[i][j].nseasons; k++){
                for(l = 0; l < irr_con[i][j].nseasons; l++){
                    
                    if(k != l && 
                            (between_jday(irr_con[i][j].season_start[l],
                            irr_con[i][j].season_end[l],
                            irr_con[i][j].season_start[k]) > 0 ||
                            between_jday(irr_con[i][j].season_start[l],
                            irr_con[i][j].season_end[l],
                            irr_con[i][j].season_end[k]) > 0)){
                        log_info("Cell %zu; crop %zu [veg_index %d, veg_class %d]; season %zu [%.2f - %.2f] and %zu [%.2f - %.2f]",
                                i,j,irr_con[i][j].veg_index,veg_con[i][irr_con[i][j].veg_index].veg_class,
                                k,irr_con[i][j].season_start[k],irr_con[i][j].season_end[k],
                                l,irr_con[i][j].season_start[l],irr_con[i][j].season_end[l]);
                        log_err("Irrigated calendars are overlapping");
                    }
                }
            }
        }
    }
    
    free(dvar);  
}

void
irr_set_ponding(void)
{
    extern domain_struct local_domain;
    extern filenames_struct filenames;
    extern option_struct options;
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
    d1count = options.NIRRTYPES;
    
    ivar = malloc(options.NIRRTYPES * sizeof(*ivar));
    check_alloc_status(ivar,"Memory allocation error");
    
    // Gather ponded vegetation classes
    if(mpi_rank == VIC_MPI_ROOT){        
        get_nc_field_int(&(filenames.irrigation), 
                "ponded_class", &d1start, &d1count, ivar);
    }
    
    status = MPI_Bcast(ivar, options.NIRRTYPES, MPI_INT, VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    // Do mapping
    for(i = 0; i < local_domain.ncells_active; i++){        
        for(j = 0; j < (size_t)options.NIRRTYPES; j++){
            if(ivar[j] == 1 && irr_con_map[i].iidx[j] != NODATA_VEG){
                irr_con[i][irr_con_map[i].iidx[j]].ponding = true;
                irr_con[i][irr_con_map[i].iidx[j]].pond_capacity = POND_CAPACITY;
            }
        }
    }
    
    free(ivar);   
}

void
irr_init(void)
{    
    extern filenames_struct filenames;
    extern int mpi_rank;
    
    int status;
    
    // open parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_open(filenames.irrigation.nc_filename, NC_NOWRITE,
                         &(filenames.irrigation.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.irrigation.nc_filename);
    }
    
    irr_set_vegetation();
    irr_set_seasons();
    irr_set_ponding();
    
    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(filenames.irrigation.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.irrigation.nc_filename);
    }
}