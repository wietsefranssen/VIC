#include <vic.h>

void
rout_set_uh(void)
{    
    extern filenames_struct filenames;
    extern option_struct options;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern rout_con_struct *rout_con;
    
    double *dvar = NULL;
    
    size_t i;
    size_t j;
    
    size_t  d3count[3];
    size_t  d3start[3];
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1; 
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx; 
    
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error."); 
    
    for(j = 0; j < options.RIRF_NSTEPS; j++){
        d3start[0] = j;
        
        get_scatter_nc_field_double(&(filenames.routing), 
                "river_irf", d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            rout_con[i].river_irf[j] = dvar[i];
        }
    }
    
    for(j = 0; j < options.GIRF_NSTEPS; j++){
        d3start[0] = j;
        
        get_scatter_nc_field_double(&(filenames.routing), 
                "grid_irf", d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            rout_con[i].grid_irf[j] = dvar[i];
        }
    }
    
    free(dvar);    
}

void
rout_set_direction(void)
{ 
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern filenames_struct filenames;
    extern rout_con_struct *rout_con;
    
    int *ivar;
        
    size_t i; 
    
    size_t  d2count[2];
    size_t  d2start[2];
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error."); 
            
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx; 
    
    get_scatter_nc_field_int(&(filenames.routing), 
            "flow_direction", d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].direction = ivar[i];
    }
    
    free(ivar);
}
size_t
get_downstream_global(size_t id, int direction)
{    
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    size_t current_io_idx;
    size_t downstream_io_idx;
            
    current_io_idx = filter_active_cells[id];     

    switch(direction){
        case 3:
            downstream_io_idx = current_io_idx + 1;
            break;
        case 4:
            if(current_io_idx < global_domain.n_nx - 1){
                log_err("Flow direction is going outside of domain");      
            }
            downstream_io_idx = current_io_idx - global_domain.n_nx + 1;
            break;
        case 5:
            if(current_io_idx < global_domain.n_nx){
                log_err("Flow direction is going outside of domain");      
            }
            downstream_io_idx = current_io_idx - global_domain.n_nx;
            break;
        case 6:
            if(current_io_idx < global_domain.n_nx + 1){
                log_err("Flow direction is going outside of domain");      
            }
            downstream_io_idx = current_io_idx - global_domain.n_nx - 1;
            break;
        case 7:
            if(current_io_idx <= 0){
                log_err("Flow direction is going outside of domain");      
            }
            downstream_io_idx = current_io_idx - 1;
            break;
        case 8:
            downstream_io_idx = current_io_idx + global_domain.n_nx - 1;
            break;
        case 1:
            downstream_io_idx = current_io_idx + global_domain.n_nx;
            break;
        case 2:
            downstream_io_idx = current_io_idx + global_domain.n_nx + 1;
            break;
        case 9:
            downstream_io_idx = current_io_idx;
            break;
        default:
            downstream_io_idx = current_io_idx;
            break;
    }
    
    if(downstream_io_idx >= global_domain.ncells_total){
        log_err("Flow direction is going outside of total domain");      
    }  
           
    if(global_domain.locations[downstream_io_idx].global_idx == MISSING_USI){
        downstream_io_idx = current_io_idx;
    }
    
    return global_domain.locations[downstream_io_idx].global_idx;    
}

size_t
get_downstream_local(size_t id, int direction)
{    
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern size_t *filter_active_cells;
    
    size_t current_io_idx;
    size_t downstream_io_idx;  
    
    size_t i;
            
    current_io_idx = filter_active_cells[id];     

    switch(direction){
        case 3:
            downstream_io_idx = current_io_idx + 1;
            break;
        case 4:
            if(current_io_idx < global_domain.n_nx - 1){
                log_err("Flow direction is going outside of domain");      
            }
            downstream_io_idx = current_io_idx - global_domain.n_nx + 1;
            break;
        case 5:
            if(current_io_idx < global_domain.n_nx){
                log_err("Flow direction is going outside of domain");      
            }
            downstream_io_idx = current_io_idx - global_domain.n_nx;
            break;
        case 6:
            if(current_io_idx < global_domain.n_nx + 1){
                log_err("Flow direction is going outside of domain");      
            }
            downstream_io_idx = current_io_idx - global_domain.n_nx - 1;
            break;
        case 7:
            if(current_io_idx <= 0){
                log_err("Flow direction is going outside of domain");      
            }
            downstream_io_idx = current_io_idx - 1;
            break;
        case 8:
            downstream_io_idx = current_io_idx + global_domain.n_nx - 1;
            break;
        case 1:
            downstream_io_idx = current_io_idx + global_domain.n_nx;
            break;
        case 2:
            downstream_io_idx = current_io_idx + global_domain.n_nx + 1;
            break;
        case 9:
            downstream_io_idx = current_io_idx;
            break;
        default:
            downstream_io_idx = current_io_idx;
            break;
    }
    
    if(downstream_io_idx >= global_domain.ncells_total){
        log_err("Flow direction is going outside of total domain");      
    }  
           
    for(i=0; i<local_domain.ncells_active; i++){
        if(local_domain.locations[i].io_idx == downstream_io_idx){
            return i;
        }
    }
    
    return id;    
}

void
rout_set_downstream(void) 
{   
    extern domain_struct local_domain;
    extern rout_con_struct *rout_con;
    
    size_t i;
              
    for(i=0; i<local_domain.ncells_active; i++){
        rout_con[i].downstream = get_downstream_local(i, rout_con[i].direction);
    }    
}

void
rout_set_upstream(void)
{
    extern domain_struct local_domain;        
    extern rout_con_struct *rout_con;
    
    size_t upstream[MAX_UPSTREAM];
    
    size_t i;
    size_t j;
    
    for(i=0;i<local_domain.ncells_active;i++){
        for(j=0;j<local_domain.ncells_active;j++){
            if(rout_con[j].downstream == i && i!=j){
                upstream[rout_con[i].Nupstream] = j;
                rout_con[i].Nupstream++;
            }
        }
    
        rout_con[i].upstream = malloc(rout_con[i].Nupstream * sizeof(*rout_con[i].upstream));
        check_alloc_status(rout_con[i].upstream, "Memory allocation error.");
        
        for(j=0;j<rout_con[i].Nupstream;j++){
            rout_con[i].upstream[j] = upstream[j];
        }
    }   
}
    
void
rout_set_order()
{
    extern domain_struct local_domain;
    extern rout_con_struct *rout_con;
    extern size_t *routing_order;
    
    bool *done_map;
    size_t rank;
    bool has_upstream;
    
    size_t i;
    size_t j;
    
    done_map = malloc(local_domain.ncells_active * sizeof(*done_map));
    check_alloc_status(done_map,"Memory allocation error");

    for(i=0;i<local_domain.ncells_active;i++){
        done_map[i]=false;
    }   

    // Set cell_order_local for node
    rank=0;
    while(rank < local_domain.ncells_active){
        for(i=0;i<local_domain.ncells_active;i++){
            if(done_map[i]){
               continue; 
            }

            // count number of upstream cells that are not processed yet
            has_upstream = false;
            for(j=0;j<rout_con[i].Nupstream;j++){
                if(!done_map[rout_con[i].upstream[j]]){
                    has_upstream = true;
                    break;
                }
            }

            if(has_upstream){
                continue;
            } 

            // if no upstream, add as next order
            routing_order[rank]=i;
            done_map[i]=true;                                
            rank++;

            if(rank>local_domain.ncells_active){
                log_err("Error in ordering and ranking cells");
            }
        }
    }

    free(done_map);
}

void
rout_init(void)
{    
    extern filenames_struct filenames;
    extern int mpi_rank;
    
    int status;
    
    // open parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_open(filenames.routing.nc_filename, NC_NOWRITE,
                         &(filenames.routing.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.routing.nc_filename);
    }
    
    rout_set_uh();
    rout_set_direction();
    rout_set_downstream();
    rout_set_upstream();
    rout_set_order();
    
    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(filenames.routing.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.routing.nc_filename);
    }
}