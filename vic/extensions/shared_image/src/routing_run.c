#include <ext_driver_shared_image.h>

void
rout(double **discharge, double *uh, double quantity, int uh_length){
    
    int i;
    
    // Compute discharge    
    for(i=0;i< uh_length;i++){
        (*discharge)[i] += quantity * uh[i];
    }    
}

void 
routing_gather(rout_con_struct *rout_con_global, ext_all_vars_struct *ext_all_vars_global, double *runoff_global){  
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern rout_con_struct *rout_con;
    extern ext_all_vars_struct *ext_all_vars;
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    extern double ***out_data;
    extern int mpi_rank;
    
    double *dvar_global = NULL;
    double *dvar_local = NULL;
    int *ivar_global = NULL;
    int *ivar_local = NULL;
    double **dvar_global_2d = NULL;
    double **dvar_local_2d = NULL;
    size_t **svar_global_2d = NULL;
    size_t **svar_local_2d = NULL;
    
    size_t i;
    size_t j;
        
    // Alloc
    if(mpi_rank == VIC_MPI_ROOT){
        dvar_global = malloc(global_domain.ncells_active * sizeof(*dvar_global));
        check_alloc_status(dvar_global,"Memory allocation error");
        ivar_global = malloc(global_domain.ncells_active * sizeof(*ivar_global));
        check_alloc_status(ivar_global,"Memory allocation error");
    }
    dvar_local = malloc(local_domain.ncells_active * sizeof(*dvar_local));
    check_alloc_status(dvar_local,"Memory allocation error");
    ivar_local = malloc(local_domain.ncells_active * sizeof(*ivar_local));
    check_alloc_status(ivar_local,"Memory allocation error");
    
    // Get local runoff
    for(i=0;i<local_domain.ncells_active;i++){
        dvar_local[i] = (out_data[i][OUT_RUNOFF][0] + out_data[i][OUT_BASEFLOW][0]) * local_domain.locations[i].area / (MM_PER_M * global_param.dt);
    }        
    // Gather runoff
    gather_double(dvar_global,dvar_local); 
    // Set runoff on master node
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){
            runoff_global[i] = dvar_global[i];
        }
    }
    
    // Get local nupstream
    for(i=0;i<local_domain.ncells_active;i++){
        ivar_local[i] = rout_con[i].Nupstream_global;
    }    
    // Gather nupstream
    gather_int(ivar_global,ivar_local); 
    // Set nupstream on master node
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){
            rout_con_global[i].Nupstream_global = ivar_global[i];
        }
    }
    
    // Free
    if(mpi_rank == VIC_MPI_ROOT){  
        free(dvar_global);
        free(ivar_global);
    }
    free(dvar_local);
    free(ivar_local);
    
    // Initialize output structures    
    for(i=0;i<global_domain.ncells_active;i++){
        rout_con_global[i].upstream_global = 
                malloc(rout_con_global[i].Nupstream_global * 
                sizeof(*rout_con_global[i].upstream_global));
        check_alloc_status(rout_con_global[i].upstream_global,"Memory allocation error");
    }
    
    // Alloc
    if(mpi_rank == VIC_MPI_ROOT){  
        dvar_global_2d = malloc(global_domain.ncells_active * sizeof(*dvar_global_2d));
        check_alloc_status(dvar_global_2d,"Memory allocation error");
        svar_global_2d = malloc(global_domain.ncells_active * sizeof(*svar_global_2d));
        check_alloc_status(svar_global_2d,"Memory allocation error");
        for(i=0;i<global_domain.ncells_active;i++){
            dvar_global_2d[i] = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*dvar_global_2d[i]));
            check_alloc_status(dvar_global_2d[i],"Memory allocation error");
            svar_global_2d[i] = malloc(MAX_UPSTREAM * sizeof(*svar_global_2d[i]));
            check_alloc_status(svar_global_2d[i],"Memory allocation error");
            for(j=0;j<MAX_UPSTREAM;j++){
                svar_global_2d[i][j]=MISSING_USI;
            }
        } 
    }
    dvar_local_2d = malloc(local_domain.ncells_active * sizeof(*dvar_local_2d));
    check_alloc_status(dvar_local_2d,"Memory allocation error");
    svar_local_2d = malloc(local_domain.ncells_active * sizeof(*svar_local_2d));
    check_alloc_status(svar_local_2d,"Memory allocation error");
    for(i=0;i<local_domain.ncells_active;i++){
        dvar_local_2d[i] = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*dvar_local_2d[i]));
        check_alloc_status(dvar_local_2d[i],"Memory allocation error");
        svar_local_2d[i] = malloc(MAX_UPSTREAM * sizeof(*svar_local_2d[i]));
        check_alloc_status(svar_local_2d[i],"Memory allocation error");
        for(j=0;j<MAX_UPSTREAM;j++){
            svar_local_2d[i][j]=MISSING_USI;
        }
    }
    
    // Get uh from local nodes
    for(i=0;i<local_domain.ncells_active;i++){
        for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
            dvar_local_2d[i][j] = rout_con[i].uh[j];
        }
    }    
    // Gather uh
    gather_double_2d(dvar_global_2d,dvar_local_2d,global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH);
    // Set uh on master node
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){
            for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
                rout_con_global[i].uh[j] = dvar_global_2d[i][j];
            }
        }
    }
    
    // Get discharge from local nodes
    for(i=0;i<local_domain.ncells_active;i++){
        for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
            dvar_local_2d[i][j] = ext_all_vars[i].rout_var.discharge[j];
        }
    }    
    // Gather discharge
    gather_double_2d(dvar_global_2d,dvar_local_2d,global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH);
    // Set discharge on master node
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){
            for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
                ext_all_vars_global[i].rout_var.discharge[j] = dvar_global_2d[i][j];
            }
        }
    }
    
    // Get local upstream
    for(i=0;i<local_domain.ncells_active;i++){
        for(j=0;j<rout_con[i].Nupstream_global;j++){
            svar_local_2d[i][j] = rout_con[i].upstream_global[j];
        }
    } 
    // Gather upstream
    gather_sizet_2d(svar_global_2d,svar_local_2d,MAX_UPSTREAM);
    // Set upstream on master node
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){
            for(j=0;j<rout_con_global[i].Nupstream_global;j++){
                rout_con_global[i].upstream_global[j] = svar_global_2d[i][j];
            }
        }
    }
        
    // Free
    if(mpi_rank == VIC_MPI_ROOT){  
        for(i=0;i<global_domain.ncells_active;i++){
            free(dvar_global_2d[i]);
            free(svar_global_2d[i]);
        }
        free(dvar_global_2d);
        free(svar_global_2d);
    }
    for(i=0;i<local_domain.ncells_active;i++){
        free(dvar_local_2d[i]);
        free(svar_local_2d[i]);
    }
    free(dvar_local_2d);
    free(svar_local_2d);    
}

void
routing_scatter(ext_all_vars_struct *ext_all_vars_global){
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    extern ext_all_vars_struct *ext_all_vars;
    extern int mpi_rank;
    
    double **discharge_local;
    double **discharge_global;
    
    size_t i;
    size_t j;
    
    // Alloc output        
    if(mpi_rank == VIC_MPI_ROOT){
        discharge_global = malloc(global_domain.ncells_active * sizeof(*discharge_global));
        check_alloc_status(discharge_global,"Memory allocation error");
        for(i=0;i<global_domain.ncells_active;i++){
            discharge_global[i] = 
                    malloc(global_param.model_steps_per_day * 
                    ext_param.UH_MAX_LENGTH * 
                    sizeof(*discharge_global[i]));
            check_alloc_status(discharge_global[i],"Memory allocation error");
        }
    }
    discharge_local = malloc(local_domain.ncells_active * sizeof(*discharge_local));
    check_alloc_status(discharge_local,"Memory allocation error");
    for(i=0;i<local_domain.ncells_active;i++){
        discharge_local[i] = 
                malloc(global_param.model_steps_per_day * 
                ext_param.UH_MAX_LENGTH * 
                sizeof(*discharge_local[i]));
        check_alloc_status(discharge_local[i],"Memory allocation error");
    }

    // Get global discharge
    if(mpi_rank == VIC_MPI_ROOT){
        for(i=0;i<global_domain.ncells_active;i++){
            for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
                discharge_global[i][j]=ext_all_vars_global[i].rout_var.discharge[j];
            }
        }
    }
    // Scatter output to local nodes
    scatter_double_2d(discharge_global,discharge_local,global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH);
    // Set local discharge
    for(i=0;i<local_domain.ncells_active;i++){
        for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
            ext_all_vars[i].rout_var.discharge[j]=discharge_local[i][j];
        }
    }

    // Free
    if(mpi_rank == VIC_MPI_ROOT){
        for(i=0;i<global_domain.ncells_active;i++){
            free(discharge_global[i]);
        }
        free(discharge_global);
    }
    for(i=0;i<local_domain.ncells_active;i++){
        free(discharge_local[i]);
    }
    free(discharge_local);       
}

void
routing_run(){
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    
    extern ext_all_vars_struct *ext_all_vars;
    extern rout_con_struct *rout_con;
    extern double ***out_data;
    
    extern int mpi_rank;
    extern int mpi_decomposition;  
    
    ext_all_vars_struct *ext_all_vars_global;
    rout_con_struct *rout_con_global;
    double *runoff_global;
    
    double runoff;
    double inflow;
    
    size_t i;
    size_t j;
    
    if(mpi_decomposition == BASIN_DECOMPOSITION){
        // Rout on local nodes
        for(i=0;i<local_domain.ncells_active;i++){
            runoff = 0;
            inflow = 0;
            
            // Gather runoff and inflow
            runoff = (out_data[i][OUT_RUNOFF][0] + out_data[i][OUT_BASEFLOW][0]) * local_domain.locations[i].area / (MM_PER_M * global_param.dt);            
            for(j=0;j<rout_con[i].Nupstream_local;j++){
                inflow += ext_all_vars[rout_con[i].upstream_local[j]].rout_var.discharge[0];
            }            
                    
            rout(&ext_all_vars[i].rout_var.discharge, rout_con[i].uh, (runoff + inflow),
                    (global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH));            
        }
    }    
    else if(mpi_decomposition == RANDOM_DECOMPOSITION){
        
        // Alloc gather structures on master node  
        if(mpi_rank == VIC_MPI_ROOT){
            ext_all_vars_global = malloc(global_domain.ncells_active * sizeof(*ext_all_vars_global));
            check_alloc_status(ext_all_vars_global,"Memory allocation error");
            rout_con_global = malloc(global_domain.ncells_active * sizeof(*rout_con_global));
            check_alloc_status(rout_con_global,"Memory allocation error");
            runoff_global = malloc(global_domain.ncells_active * sizeof(*runoff_global));
            check_alloc_status(runoff_global,"Memory allocation error");
            for(i=0;i<global_domain.ncells_active;i++){
                ext_all_vars_global[i].rout_var.discharge = 
                        malloc(global_param.model_steps_per_day * 
                        ext_param.UH_MAX_LENGTH * 
                        sizeof(*ext_all_vars_global[i].rout_var.discharge));
                check_alloc_status(ext_all_vars_global[i].rout_var.discharge,"Memory allocation error");
                rout_con_global[i].uh = 
                        malloc(global_param.model_steps_per_day * 
                        ext_param.UH_MAX_LENGTH * 
                        sizeof(*rout_con_global[i].uh));
                check_alloc_status(rout_con_global[i].uh,"Memory allocation error");
                
                initialize_rout_con(&rout_con_global[i]);
            }
        }
        
        // Gather necessary data from local nodes        
        routing_gather(rout_con_global, ext_all_vars_global, runoff_global);
              
        // Rout on master node
        if(mpi_rank == VIC_MPI_ROOT){
            for(i=0;i<global_domain.ncells_active;i++){
                inflow = 0;
                
                // Gather inflow
                for(j=0;j<rout_con_global[i].Nupstream_global;j++){
                    inflow += ext_all_vars_global[rout_con_global[i].upstream_global[j]].rout_var.discharge[0];
                }
                
                rout(&ext_all_vars_global[i].rout_var.discharge, rout_con_global[i].uh, (runoff_global[i] + inflow),
                    (global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH)); 
            }
        }
        
        routing_scatter(ext_all_vars_global);        
        
        // Free        
        if(mpi_rank == VIC_MPI_ROOT){
            for(i=0;i<global_domain.ncells_active;i++){
                free(ext_all_vars_global[i].rout_var.discharge);
                free(rout_con_global[i].uh);
            }
            free(ext_all_vars_global);
            free(rout_con_global);
            free(runoff_global);
        } 
    }
}