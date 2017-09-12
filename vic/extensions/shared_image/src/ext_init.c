#include <ext_driver_shared_image.h>

size_t
get_total_id(size_t active_id){
    extern size_t *filter_active_cells;
    extern size_t *mpi_map_mapping_array;
    extern int *mpi_map_global_array_offsets;
    extern int mpi_rank;
    extern int mpi_decomposition;
    
    size_t global_active_id;
    size_t total_id;
    
    if(mpi_decomposition == BASIN_DECOMPOSITION){
        global_active_id = mpi_map_mapping_array[mpi_map_global_array_offsets[mpi_rank] + active_id];
        total_id = filter_active_cells[global_active_id];        
    }else if(mpi_decomposition == RANDOM_DECOMPOSITION){
        total_id = filter_active_cells[active_id];
    }
    
    return total_id;
}

size_t
get_active_id(size_t global_id){
    extern domain_struct global_domain;
    extern size_t *mpi_map_mapping_array_reverse;
    extern int mpi_decomposition;
    
    size_t global_active_id;
    size_t active_id;
    
    if(mpi_decomposition == BASIN_DECOMPOSITION){
        global_active_id = global_domain.locations[global_id].global_idx;
        active_id = mpi_map_mapping_array_reverse[global_active_id];      
    }else if(mpi_decomposition == RANDOM_DECOMPOSITION){
        active_id = global_domain.locations[global_id].global_idx;
    }
    
    return active_id;
}

size_t
get_downstream_id(size_t id, int direction){
    
    extern domain_struct global_domain;
    
    size_t next_id;    
    
    if(direction == NODATA_DIRECTION){
        log_err("Flow direction is not present");
    }         

    switch(direction){
        case 3:
            next_id = id + 1;
            break;
        case 4:
            if(id<global_domain.n_nx){
                log_err("Flow direction is going outside of domain");                        
            }
            next_id = id - global_domain.n_nx + 1;
            break;
        case 5:
            if(id<global_domain.n_nx){
                log_err("Flow direction is going outside of domain");                        
            }
            next_id = id - global_domain.n_nx;
            break;
        case 6:
            if(id<global_domain.n_nx){
                log_err("Flow direction is going outside of domain");                        
            }
            next_id = id - global_domain.n_nx - 1;
            break;
        case 7:
            next_id = id - 1;
            break;
        case 8:
            next_id = id + global_domain.n_nx - 1;
            break;
        case 1:
            next_id = id + global_domain.n_nx;
            break;
        case 2:
            next_id = id + global_domain.n_nx + 1;
            break;
        case 9:
            next_id = id;
            break;
        default:
            log_err("Unknown flow direction")
            break;
    }
    
    return next_id;
}

void
set_upstream(size_t id, size_t Ncells){
    extern rout_con_struct *rout_con;
    
    size_t upstream[MAX_UPSTREAM];
    size_t Nupstream;
    
    size_t i;
    
    Nupstream = 0;
    for(i=0;i<Ncells;i++){
        if(rout_con[i].downstream==id){
            upstream[Nupstream] = i;
            Nupstream++;
        }
    }
        
    rout_con[id].upstream = malloc(Nupstream * sizeof(*rout_con[id].upstream));
    check_alloc_status(rout_con[id].upstream, "Memory allocation error");

    for(i=0;i<Nupstream;i++){
        rout_con[id].upstream[i]=upstream[i];
    }
    rout_con[id].Nupstream=Nupstream;
}

void
set_uh(size_t id, double distance, double diffusion, double velocity){
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
routing_init_direction(size_t Ncells) {    
    extern domain_struct global_domain;
    extern ext_filenames_struct ext_filenames;
    extern rout_con_struct *rout_con;
       
    int    *direction = NULL;
    
    size_t current_id;
    size_t downstream_id;
    
    size_t i;    
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

    get_nc_field_int(ext_filenames.routing, ext_filenames.info.direction_var, d2start, d2count,
                     direction);
    
    for(i=0;i<Ncells;i++){
        current_id = get_total_id(i);
        rout_con[i].direction = direction[current_id];
        
        downstream_id = get_downstream_id(current_id,direction[current_id]);        
        rout_con[i].downstream = get_active_id(downstream_id);
    }
    
    for(i=0;i<Ncells;i++){
        set_upstream(i, Ncells);
    }
    
    free(direction);
}
    
void
routing_init_uh(size_t Ncells){    
    extern ext_filenames_struct ext_filenames;
    extern ext_parameters_struct ext_param;
    extern ext_option_struct ext_options;
    extern domain_struct global_domain;
    
    double *velocity_array = NULL;
    double *distance_array = NULL;
    double *diffusion_array = NULL;
    
    size_t current_id;
    
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

    distance_array = malloc(global_domain.ncells_total * sizeof(*distance_array));
    check_alloc_status(distance_array, "Memory allocation error.");
    
    get_nc_field_double(ext_filenames.routing, ext_filenames.info.distance_var, d2start, d2count,
                     distance_array);
    
    if(ext_options.UH_PARAMETERS == FILE_UH_PARAMETERS){                  
        velocity_array = malloc(global_domain.ncells_total * sizeof(*velocity_array));
        check_alloc_status(velocity_array, "Memory allocation error.");
        diffusion_array = malloc(global_domain.ncells_total * sizeof(*diffusion_array));
        check_alloc_status(diffusion_array, "Memory allocation error.");
        
        get_nc_field_double(ext_filenames.routing, ext_filenames.info.velocity_var, d2start, d2count,
                         velocity_array);
        get_nc_field_double(ext_filenames.routing, ext_filenames.info.diffusion_var, d2start, d2count,
                         diffusion_array);        
    }
    
    for(i=0;i<Ncells;i++){
        current_id = get_total_id(i);
        
        distance = distance_array[current_id];
        
        if(ext_options.UH_PARAMETERS == FILE_UH_PARAMETERS){
            velocity = velocity_array[current_id];
            diffusion = diffusion_array[current_id];            
        }else if(ext_options.UH_PARAMETERS == CONSTANT_UH_PARAMETERS){
            velocity = ext_param.UH_FLOW_VELOCITY;
            diffusion = ext_param.UH_FLOW_DIFFUSION;   
        }
        
        set_uh(i,distance,velocity,diffusion);
    }
    
    free(distance_array);
    if(ext_options.UH_PARAMETERS == FILE_UH_PARAMETERS){ 
        free(velocity_array);
        free(diffusion_array);
    }
    
}

void
routing_init_order(size_t Ncells){
    extern rout_con_struct *rout_con;
    extern size_t *cell_order;
    
    bool *done;
    
    size_t i;
    size_t j;
    size_t Nupstream_active;
    size_t upstream_id;
    size_t rank = 0;
    
    done = malloc(Ncells * sizeof(*done));
    check_alloc_status(done,"Memory allocation error");
    
    for(i=0;i<Ncells;i++){
        done[i]=false;
    }
    
    while(true){
        for(i=0;i<Ncells;i++){
            if(done[i]){
               continue; 
            }
            
            // count number of upstream cells that are not processed yet
            Nupstream_active=0;
            for(j=0;j<rout_con[i].Nupstream;j++){
                upstream_id = rout_con[i].upstream[j];
                if(!done[upstream_id]){
                    Nupstream_active++;
                }
            }
            
            // if no upstream, add as next order
            if(Nupstream_active == 0){
                cell_order[rank]=i;
                rout_con[i].rank=rank;
                done[i]=true;                                
                rank++;
                
                if(rank>=Ncells){
                    log_err("Error in ordering and ranking cells");
                }
            }
        }
        
        if (rank == Ncells - 1){
            break;
        }
    }
    
    free(done);
}

void
routing_init(){
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern int mpi_decomposition;
    extern int mpi_rank;
    
    if(mpi_decomposition == BASIN_DECOMPOSITION){
        routing_init_direction(local_domain.ncells_active);
        routing_init_uh(local_domain.ncells_active);
        routing_init_order(local_domain.ncells_active);
    }else if(mpi_decomposition == RANDOM_DECOMPOSITION && 
            mpi_rank==VIC_MPI_ROOT){
        routing_init_direction(global_domain.ncells_active);
        routing_init_uh(global_domain.ncells_active);
        routing_init_order(global_domain.ncells_active);
    }
    
}

void
ext_init(void){    
    routing_init();
}
