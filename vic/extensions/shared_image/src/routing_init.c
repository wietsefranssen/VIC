#include <ext_driver_shared_image.h>

void
get_downstream(size_t id, int direction, size_t *downstream){
    
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    size_t next_total_id;  
    size_t total_id;
            
    total_id = filter_active_cells[id];     

    switch(direction){
        case 3:
            next_total_id = total_id + 1;
            break;
        case 4:
            if(total_id < global_domain.n_nx - 1){
                log_err("Flow direction is going outside of domain");      
            }
            next_total_id = total_id - global_domain.n_nx + 1;
            break;
        case 5:
            if(total_id < global_domain.n_nx){
                log_err("Flow direction is going outside of domain");      
            }
            next_total_id = total_id - global_domain.n_nx;
            break;
        case 6:
            if(total_id < global_domain.n_nx + 1){
                log_err("Flow direction is going outside of domain");      
            }
            next_total_id = total_id - global_domain.n_nx - 1;
            break;
        case 7:
            if(total_id <= 0){
                log_err("Flow direction is going outside of domain");      
            }
            next_total_id = total_id - 1;
            break;
        case 8:
            next_total_id = total_id + global_domain.n_nx - 1;
            break;
        case 1:
            next_total_id = total_id + global_domain.n_nx;
            break;
        case 2:
            next_total_id = total_id + global_domain.n_nx + 1;
            break;
        case 9:
            next_total_id = total_id;
            break;
        default:
            next_total_id = total_id;
            break;
    }
    
    if(next_total_id >= global_domain.ncells_total){
        log_err("Flow direction is going outside of domain");      
    }    
    if(global_domain.locations[next_total_id].global_idx == MISSING_USI){
        next_total_id = total_id;
    }
    
    (*downstream) = global_domain.locations[next_total_id].global_idx;
}

void
set_uh(size_t id, double distance, double velocity, double diffusion){
    extern rout_con_struct *rout_con;
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    
    double *uh_precise = NULL;
    double *uh_cumulative = NULL;
    double uh_sum = 0;
    double time = 0;
    
    size_t i;
        
    uh_precise = malloc((ext_param.UH_MAX_LENGTH * global_param.model_steps_per_day * ext_param.UH_PARTITIONS) * sizeof(*uh_precise));
    check_alloc_status(uh_precise,"Memory allocation error.");    
    uh_cumulative = malloc((ext_param.UH_MAX_LENGTH * global_param.model_steps_per_day * ext_param.UH_PARTITIONS) * sizeof(*uh_cumulative));
    check_alloc_status(uh_cumulative,"Memory allocation error.");
    
    for(i = 0;i < ext_param.UH_MAX_LENGTH * global_param.model_steps_per_day * ext_param.UH_PARTITIONS; i++){         
        time += (SEC_PER_HOUR * HOURS_PER_DAY) / (global_param.model_steps_per_day * ext_param.UH_PARTITIONS);
        
        uh_precise[i] = uh(time, distance, velocity, diffusion);
        uh_sum += uh_precise[i];
    }
    
    for(i = 0;i < ext_param.UH_MAX_LENGTH * global_param.model_steps_per_day * ext_param.UH_PARTITIONS; i++){ 
        uh_precise[i] = uh_precise[i] / uh_sum;
    }
    
    uh_cumulative[0] = uh_precise[0];
    for(i = 1;i < ext_param.UH_MAX_LENGTH * global_param.model_steps_per_day * ext_param.UH_PARTITIONS - 1; i++){
        uh_cumulative[i] = uh_cumulative[i-1] + uh_precise[i];
    }
    uh_cumulative[i]=1.0;
    
    rout_con[id].uh[0] = uh_cumulative[ext_param.UH_PARTITIONS - 1];
    for(i = 1; i < ext_param.UH_MAX_LENGTH * global_param.model_steps_per_day; i++){
        rout_con[id].uh[i] = uh_cumulative[(i+1) * ext_param.UH_PARTITIONS - 1] - uh_cumulative[i * ext_param.UH_PARTITIONS - 1];
    }  
    
    free(uh_precise);
    free(uh_cumulative);
}

double
uh(double time, double distance, double velocity, double diffusion){
    return (distance/(2*time*sqrt(M_PI * time * diffusion))) * 
                exp( -(pow(velocity * time - distance, 2) ) /
                (4*diffusion * time));
}

void
routing_init_downstream() {    
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern ext_filenames_struct ext_filenames;
    extern rout_con_struct *rout_con;
    extern int mpi_rank;
    
    int *direction = NULL;
    size_t *downstream_global = NULL;
    size_t *downstream_local = NULL;
        
    size_t i; 
    size_t j;
    
    size_t  d2count[2];
    size_t  d2start[2];
        
    downstream_local = malloc(local_domain.ncells_active * sizeof(*downstream_local));
    check_alloc_status(downstream_local, "Memory allocation error.");
    
    if(mpi_rank == VIC_MPI_ROOT){
        downstream_global = malloc(global_domain.ncells_active * sizeof(*downstream_global));
        check_alloc_status(downstream_global, "Memory allocation error.");
        direction = malloc(global_domain.ncells_active * sizeof(*direction));
        check_alloc_status(direction, "Memory allocation error.");
        
        d2start[0] = 0;
        d2start[1] = 0;
        d2count[0] = global_domain.n_ny;
        d2count[1] = global_domain.n_nx;

        get_active_nc_field_int(&ext_filenames.routing, ext_filenames.info.direction_var, d2start, d2count,
                         direction);
                
        // Get global downstream
        for (i = 0; i < global_domain.ncells_active; i++) {
            get_downstream(i,direction[i],&downstream_global[i]);
        }
        
        free(direction);
    }    
    scatter_sizet(downstream_global, downstream_local);
    
    // Set global and local downstream cells
    for(i=0; i<local_domain.ncells_active; i++){
        rout_con[i].downstream_global = downstream_local[i];
        
        for(j=0;j<local_domain.ncells_active;j++){
            if(local_domain.locations[j].global_idx == rout_con[i].downstream_global){
                rout_con[i].downstream_local = j;
                break;
            }
        }
    }    
    
    if(mpi_rank == VIC_MPI_ROOT){
        free(downstream_global);
    }
    free(downstream_local);
}

void
routing_init_upstream(){
    extern domain_struct global_domain;
    extern domain_struct local_domain;        
    extern rout_con_struct *rout_con;
    extern int mpi_rank;
    
    size_t *downstream_local = NULL;
    size_t *downstream_global = NULL;
    
    int *nupstream_global = NULL;
    size_t **upstream_global = NULL;
    int *nupstream_local = NULL;
    size_t **upstream_local = NULL;
    size_t upstream[MAX_UPSTREAM];
    
    size_t i;    
    size_t j;
    size_t k;
    
    // Alloc
    if(mpi_rank == VIC_MPI_ROOT){
        downstream_global = malloc(global_domain.ncells_active * sizeof(*downstream_global));
        check_alloc_status(downstream_global, "Memory allocation error.");
        nupstream_global = malloc(global_domain.ncells_active * sizeof(*nupstream_global));
        check_alloc_status(nupstream_global, "Memory allocation error.");
        upstream_global = malloc(global_domain.ncells_active * sizeof(*upstream_global));
        check_alloc_status(upstream_global, "Memory allocation error.");
        for (i = 0; i < global_domain.ncells_active; i++) {
            upstream_global[i] = malloc(MAX_UPSTREAM * sizeof(*upstream_global[i]));
            check_alloc_status(upstream_global[i], "Memory allocation error.");
        }
    }
    downstream_local = malloc(local_domain.ncells_active * sizeof(*downstream_local));
    check_alloc_status(downstream_local, "Memory allocation error.");
    nupstream_local = malloc(local_domain.ncells_active * sizeof(*nupstream_local));
    check_alloc_status(nupstream_local, "Memory allocation error."); 
    upstream_local = malloc(local_domain.ncells_active * sizeof(*upstream_local));
    check_alloc_status(upstream_local, "Memory allocation error."); 
    for (i = 0; i < local_domain.ncells_active; i++) {
        upstream_local[i] = malloc(MAX_UPSTREAM * sizeof(*upstream_local[i]));
        check_alloc_status(upstream_local[i], "Memory allocation error.");
    }
    
    // Get global downstream
    for(i=0; i<local_domain.ncells_active;i++){
        downstream_local[i]=rout_con[i].downstream_global;
    }    
    
    // Gather to master node
    gather_sizet(downstream_global,downstream_local);
    
    // Get global upstream
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){        
            nupstream_global[i] = 0;
            
            for(j=0;j<global_domain.ncells_active;j++){
                if(downstream_global[j] == i && j!=i){
                    upstream_global[i][nupstream_global[i]] = j;
                    nupstream_global[i]++;
                }
            }
        }        
    }
    
    // Scatter to nodes
    scatter_int(nupstream_global, nupstream_local);
    scatter_sizet_2d(upstream_global, upstream_local, MAX_UPSTREAM);
        
    // Set global & local upstream
    for(i=0; i<local_domain.ncells_active; i++){
        // Set global upstream
        rout_con[i].Nupstream_global = nupstream_local[i];        
        rout_con[i].upstream_global = malloc(rout_con[i].Nupstream_global*sizeof(*rout_con[i].upstream_global));
        check_alloc_status(rout_con[i].upstream_global,"Memory allocation error");
        for(j=0;j<rout_con[i].Nupstream_global;j++){
            rout_con[i].upstream_global[j] = upstream_local[i][j];
        }      
        
        // Get local upstream
        rout_con[i].Nupstream_local = 0;
        for(j=0;j<rout_con[i].Nupstream_global;j++){
            for(k=0;k<local_domain.ncells_active;k++){
                if(rout_con[i].upstream_global[j] == local_domain.locations[k].global_idx){
                    upstream[rout_con[i].Nupstream_local] = k;
                    rout_con[i].Nupstream_local++;
                }
            }
        }
            
        // Set local upstream
        rout_con[i].upstream_local = malloc(rout_con[i].Nupstream_local*sizeof(*rout_con[i].upstream_local));
        check_alloc_status(rout_con[i].upstream_local,"Memory allocation error");
        for(j=0;j<rout_con[i].Nupstream_local;j++){
            rout_con[i].upstream_local[j] = upstream[j];
        } 
    }       
    
    // Free
    if(mpi_rank == VIC_MPI_ROOT){
        free(downstream_global);
        free(nupstream_global);        
        for(i=0;i<global_domain.ncells_active;i++){
            free(upstream_global[i]);
        }
        free(upstream_global);  
    }    
    free(downstream_local);
    free(nupstream_local);    
    for(i=0;i<local_domain.ncells_active;i++){
        free(upstream_local[i]);
    }  
    free(upstream_local);    
}
    
void
routing_init_uh(){    
    extern ext_filenames_struct ext_filenames;
    extern ext_parameters_struct ext_param;
    extern ext_option_struct ext_options;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    
    double *velocity_array = NULL;
    double *distance_array = NULL;
    double *diffusion_array = NULL;
    
    double velocity;
    double distance;
    double diffusion;
    
    size_t i;
    size_t  d2count[2];
    size_t  d2start[2];
    
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx; 
    
    distance_array = malloc(local_domain.ncells_active * sizeof(*distance_array));
    check_alloc_status(distance_array, "Memory allocation error.");                    
    velocity_array = malloc(local_domain.ncells_active * sizeof(*velocity_array));
    check_alloc_status(velocity_array, "Memory allocation error.");
    diffusion_array = malloc(local_domain.ncells_active * sizeof(*diffusion_array));
    check_alloc_status(diffusion_array, "Memory allocation error.");   
        
    get_scatter_nc_field_double(&ext_filenames.routing, ext_filenames.info.distance_var, d2start, d2count,
                     distance_array);
    
    if(ext_options.UH_PARAMETERS == FILE_UH_PARAMETERS){     
        get_scatter_nc_field_double(&ext_filenames.routing, ext_filenames.info.velocity_var, d2start, d2count,
                         velocity_array);
        get_scatter_nc_field_double(&ext_filenames.routing, ext_filenames.info.diffusion_var, d2start, d2count,
                         diffusion_array);        
    }
    
    for(i=0;i<local_domain.ncells_active;i++){       
        distance = distance_array[i];
        
        if(ext_options.UH_PARAMETERS == FILE_UH_PARAMETERS){
            velocity = velocity_array[i];
            diffusion = diffusion_array[i];            
        }
        else if(ext_options.UH_PARAMETERS == CONSTANT_UH_PARAMETERS){
            velocity = ext_param.UH_FLOW_VELOCITY;
            diffusion = ext_param.UH_FLOW_DIFFUSION;   
        }
        
        set_uh(i,distance,velocity,diffusion);
    }
    
    free(distance_array);
    free(velocity_array);
    free(diffusion_array);
    
}

void
routing_init_order(){
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern rout_con_struct *rout_con;
    extern size_t *cell_order_global;
    extern size_t *cell_order_local;
    extern int mpi_rank;
    
    int *nupstream_global = NULL;
    size_t **upstream_global = NULL;
    int *nupstream_local = NULL;
    size_t **upstream_local = NULL;
    
    bool *done_global;
    bool *done_local;
    
    size_t i;
    size_t j;
    size_t upstream_id;
    size_t rank;
    
    // Alloc & init
    if(mpi_rank == VIC_MPI_ROOT){
        nupstream_global = malloc(global_domain.ncells_active * sizeof(*nupstream_global));
        check_alloc_status(nupstream_global, "Memory allocation error.");
        upstream_global = malloc(global_domain.ncells_active * sizeof(*upstream_global));
        check_alloc_status(upstream_global, "Memory allocation error.");
        for (i = 0; i < global_domain.ncells_active; i++) {
            upstream_global[i] = malloc(MAX_UPSTREAM * sizeof(*upstream_global[i]));
            check_alloc_status(upstream_global[i], "Memory allocation error.");
        }
        done_global = malloc(global_domain.ncells_active * sizeof(*done_global));
        check_alloc_status(done_global,"Memory allocation error");        
        for(i=0;i<global_domain.ncells_active;i++){
            done_global[i]=false;
        }
    }
    nupstream_local = malloc(local_domain.ncells_active * sizeof(*nupstream_local));
    check_alloc_status(nupstream_local, "Memory allocation error."); 
    upstream_local = malloc(local_domain.ncells_active * sizeof(*upstream_local));
    check_alloc_status(upstream_local, "Memory allocation error."); 
    for (i = 0; i < local_domain.ncells_active; i++) {
        upstream_local[i] = malloc(MAX_UPSTREAM * sizeof(*upstream_local[i]));
        check_alloc_status(upstream_local[i], "Memory allocation error.");
    }
    done_local = malloc(local_domain.ncells_active * sizeof(*done_local));
    check_alloc_status(done_local,"Memory allocation error");
    for(i=0;i<local_domain.ncells_active;i++){
        done_local[i]=false;
    }   
    
    // Set local upstream
    for(i=0;i<local_domain.ncells_active;i++){
        nupstream_local[i] = rout_con[i].Nupstream_global;
        for(j=0;j<rout_con[i].Nupstream_global;j++){
            upstream_local[i][j] = rout_con[i].upstream_global[j];
        }
    }
    
    // Gather upstream to master node
    gather_int(nupstream_global, nupstream_local);
    gather_sizet_2d(upstream_global, upstream_local, MAX_UPSTREAM);
    
    // Set cell_order_global for master node
    if(mpi_rank == VIC_MPI_ROOT){        
        // Set global upstream cells
        rank=0;
        while(true){
            for(i=0;i<global_domain.ncells_active;i++){
                if(done_global[i]){
                   continue; 
                }

                // count number of upstream cells that are not processed yet
                for(j=0;j<(size_t) nupstream_global[i];j++){
                    upstream_id = upstream_global[i][j];
                    if(!done_global[upstream_id]){
                        continue;
                    }
                }

                // if no upstream, add as next order
                cell_order_global[rank]=i;
                done_global[i]=true;                                
                rank++;

                if(rank>global_domain.ncells_active){
                    log_err("Error in ordering and ranking cells");
                }
            }

            if (rank == global_domain.ncells_active){
                break;
            }
        }
    }
    
    // Set cell_order_local for node
    rank=0;
    while(true){
        for(i=0;i<local_domain.ncells_active;i++){
            if(done_local[i]){
               continue; 
            }

            // count number of upstream cells that are not processed yet
            for(j=0;j<rout_con[i].Nupstream_local;j++){
                upstream_id = rout_con[i].upstream_local[j];
                if(!done_local[upstream_id]){
                    continue;
                }
            }

            // if no upstream, add as next order
            cell_order_local[rank]=i;
            done_local[i]=true;                                
            rank++;

            if(rank>local_domain.ncells_active){
                log_err("Error in ordering and ranking cells");
            }
        }

        if (rank == local_domain.ncells_active){
            break;
        }
    }
    
    // Free
    if(mpi_rank == VIC_MPI_ROOT){
        free(nupstream_global);        
        for(i=0;i<global_domain.ncells_active;i++){
            free(upstream_global[i]);
        }
        free(upstream_global);  
        free(done_global);   
    }
    free(nupstream_local);    
    for(i=0;i<local_domain.ncells_active;i++){
        free(upstream_local[i]);
    }  
    free(upstream_local);  
    free(done_local);
}

void
routing_init(){        
    routing_init_uh();
    routing_init_downstream();
    routing_init_upstream();
    routing_init_order();
    
    debug_nupstream();
    debug_downstream();
    debug_id();
}

