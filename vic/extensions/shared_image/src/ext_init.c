#include <ext_driver_shared_image.h>

void
routing_local_init(char *nc_name)
{
    extern size_t *filter_active_cells;
    extern size_t *mpi_map_mapping_array;
    extern size_t *mpi_map_mapping_array_reverse;
    extern int *mpi_map_global_array_offsets;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern rout_con_struct *rout_con;
    extern int mpi_rank;
    
    int    *direction = NULL;
    size_t global_active_id;
    size_t global_total_id;
    size_t next_global_total_id;
    size_t next_global_active_id;
    size_t next_local_active_id;
    
    size_t upstream[MAX_UPSTREAM];
    size_t Nupstream;
    
    size_t  i;
    size_t  j;
    size_t  d2count[2];
    size_t  d2start[2];
    
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;
    
    direction = malloc(local_domain.ncells_active * sizeof(*direction));
    check_alloc_status(direction, "Memory allocation error.");

    for (i = 0; i < local_domain.ncells_active; i++) {
        direction[i] = NODATA_DIRECTION;
    }

    get_scatter_nc_field_int(nc_name, "flow_direction", d2start, d2count,
                     direction);
    
    // get downstream cells
    for (i = 0; i < local_domain.ncells_active; i++) {
        global_active_id = mpi_map_mapping_array[mpi_map_global_array_offsets[mpi_rank] + i];
        global_total_id = filter_active_cells[global_active_id];     
        
        if(direction[i] == NODATA_DIRECTION){
            log_err("Flow direction is not present");
        }         

        switch(direction[i]){
            case 3:
                next_global_total_id = global_total_id + 1;
                break;
            case 4:
                if(global_total_id<global_domain.n_nx){
                    log_err("Flow direction is going outside of domain");                        
                }
                next_global_total_id = global_total_id - global_domain.n_nx + 1;
                break;
            case 5:
                if(global_total_id<global_domain.n_nx){
                    log_err("Flow direction is going outside of domain");                        
                }
                next_global_total_id = global_total_id - global_domain.n_nx;
                break;
            case 6:
                if(global_total_id<global_domain.n_nx){
                    log_err("Flow direction is going outside of domain");                        
                }
                next_global_total_id = global_total_id - global_domain.n_nx - 1;
                break;
            case 7:
                next_global_total_id = global_total_id - 1;
                break;
            case 8:
                next_global_total_id = global_total_id + global_domain.n_nx - 1;
                break;
            case 1:
                next_global_total_id = global_total_id + global_domain.n_nx;
                break;
            case 2:
                next_global_total_id = global_total_id + global_domain.n_nx + 1;
                break;
            case 9:
                next_global_total_id = global_total_id;
                break;
            default:
                log_err("Unknown flow direction in file")
                break;
        }   
        
        next_global_active_id = global_domain.locations[next_global_total_id].global_idx;
        next_local_active_id = mpi_map_mapping_array_reverse[next_global_active_id];
        
        rout_con[i].downstream=next_local_active_id;        
        rout_con[i].direction=direction[i];
    }
    
    // get upstream cells    
    for (i = 0; i < local_domain.ncells_active; i++) {        
        for(j=0;j<MAX_UPSTREAM;j++){
            upstream[j]=0;
        }
        Nupstream=0;
        
        for(j=0;j<local_domain.ncells_active;j++){
            if(rout_con[j].downstream==i){
                upstream[Nupstream]=j;
                Nupstream++;
            }
        }
        
        rout_con[i].upstream = malloc(Nupstream * sizeof(*rout_con[i].upstream));
        check_alloc_status(rout_con[i].upstream, "Memory allocation error");
        
        for(j=0;j<Nupstream;j++){
            rout_con[i].upstream[j]=upstream[j];
        }
        rout_con[i].Nupstream=Nupstream;
    }
}

void
routing_global_init(char *nc_name)
{
    extern domain_struct global_domain;
    extern rout_con_struct *rout_con;
    
    int    *direction = NULL;
    size_t next_global_total_id;
    size_t next_global_active_id;
        
    size_t upstream[MAX_UPSTREAM];
    size_t Nupstream;
    
    size_t  i;
    size_t  j;
    size_t  d2count[2];
    size_t  d2start[2];
    
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;
    
    direction = malloc(global_domain.ncells_total * sizeof(*direction));
    check_alloc_status(direction, "Memory allocation error.");

    for (i = 0; i < global_domain.ncells_total; i++) {
        direction[i] = NODATA_DIRECTION;
    }

    get_nc_field_int(nc_name, "flow_direction", d2start, d2count,
                     direction);
    
    j=0;
    for (i = 0; i < global_domain.ncells_total; i++) {
        if(!global_domain.locations[i].run){
            continue;
        }
        
        if(direction[i] == NODATA_DIRECTION){
            log_err("Flow direction is not present");
        }         

        switch(direction[i]){
            case 3:
                next_global_total_id = i + 1;
                break;
            case 4:
                if(i<global_domain.n_nx){
                    log_err("Flow direction is going outside of domain");                        
                }
                next_global_total_id = i - global_domain.n_nx + 1;
                break;
            case 5:
                if(i<global_domain.n_nx){
                    log_err("Flow direction is going outside of domain");                        
                }
                next_global_total_id = i - global_domain.n_nx;
                break;
            case 6:
                if(i<global_domain.n_nx){
                    log_err("Flow direction is going outside of domain");                        
                }
                next_global_total_id = i - global_domain.n_nx - 1;
                break;
            case 7:
                next_global_total_id = i - 1;
                break;
            case 8:
                next_global_total_id = i + global_domain.n_nx - 1;
                break;
            case 1:
                next_global_total_id = i + global_domain.n_nx;
                break;
            case 2:
                next_global_total_id = i + global_domain.n_nx + 1;
                break;
            case 9:
                next_global_total_id = i;
                break;
            default:
                log_err("Unknown flow direction in file")
                break;
        }   
        
        next_global_active_id = global_domain.locations[next_global_total_id].global_idx;
        
        rout_con[j].downstream=next_global_active_id;        
        rout_con[j].direction=direction[i];
        
        j++;
    }
    
    // get upstream cells    
    for (i = 0; i < global_domain.ncells_active; i++) {
        for(j=0;j<MAX_UPSTREAM;j++){
            upstream[j]=0;
        }
        Nupstream=0;
        
        for(j=0;j<global_domain.ncells_active;j++){
            if(rout_con[j].downstream==i){
                upstream[Nupstream]=j;
                Nupstream++;
            }
        }
        
        rout_con[i].upstream = malloc(Nupstream * sizeof(*rout_con[i].upstream));
        check_alloc_status(rout_con[i].upstream, "Memory allocation error");
        
        for(j=0;j<Nupstream;j++){
            rout_con[i].upstream[j]=upstream[j];
        }
        rout_con[i].Nupstream=Nupstream;
    }    
}

void
routing_init(char *nc_name)
{    
    extern ext_option_struct ext_options;
    extern int mpi_rank;
       
    if(ext_options.MPI_DECOMPOSITION==BASIN_DECOMPOSITION){
        routing_local_init(nc_name);  
        
    } else if(ext_options.MPI_DECOMPOSITION==RANDOM_DECOMPOSITION && 
            mpi_rank==VIC_MPI_ROOT){
        routing_global_init(nc_name);        
    }
}

void
ext_init(void)
{    
    extern ext_filenames_struct ext_filenames;
    routing_init(ext_filenames.routing);
}
