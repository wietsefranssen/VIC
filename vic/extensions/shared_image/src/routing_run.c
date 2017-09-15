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
routing_gather(rout_con_struct **rout_con_global, ext_all_vars_struct **ext_all_vars_global, double **runoff_global){  
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
    
    // Initialize output structures    
    (*ext_all_vars_global) = malloc(global_domain.ncells_active * sizeof(*ext_all_vars_global));
    check_alloc_status(ext_all_vars_global,"Memory allocation error");
    (*rout_con_global) = malloc(global_domain.ncells_active * sizeof(*rout_con_global));
    check_alloc_status(rout_con_global,"Memory allocation error");
    (*runoff_global) = malloc(global_domain.ncells_active * sizeof(*runoff_global));
    check_alloc_status(runoff_global,"Memory allocation error");
    for(i=0;i<global_domain.ncells_active;i++){
        (*ext_all_vars_global)[i].rout_var.discharge = 
                malloc(global_param.model_steps_per_day * 
                ext_param.UH_MAX_LENGTH * 
                sizeof(*(*ext_all_vars_global)[i].rout_var.discharge));
        check_alloc_status((*ext_all_vars_global)[i].rout_var.discharge,"Memory allocation error");
        (*rout_con_global)[i].uh = 
                malloc(global_param.model_steps_per_day * 
                ext_param.UH_MAX_LENGTH * 
                sizeof(*(*rout_con_global)[i].uh));
        check_alloc_status((*rout_con_global)[i].uh,"Memory allocation error");
    }
    
    // Gather all 1d variables
    dvar_global = malloc(global_domain.ncells_active * sizeof(*dvar_global));
    check_alloc_status(dvar_global,"Memory allocation error");
    dvar_local = malloc(local_domain.ncells_active * sizeof(*dvar_local));
    check_alloc_status(dvar_local,"Memory allocation error");
    ivar_global = malloc(global_domain.ncells_active * sizeof(*ivar_global));
    check_alloc_status(ivar_global,"Memory allocation error");
    ivar_local = malloc(local_domain.ncells_active * sizeof(*ivar_local));
    check_alloc_status(ivar_local,"Memory allocation error");
    
    // Gather runoff
    for(i=0;i<local_domain.ncells_active;i++){
        dvar_local[i] = (out_data[i][OUT_RUNOFF][0] + out_data[i][OUT_BASEFLOW][0]) / global_param.dt;
    }    
    gather_double(dvar_global,dvar_local);        
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){
            (*runoff_global)[i] = dvar_global[i];
        }
    }
    
    // Gather nupstream
    for(i=0;i<local_domain.ncells_active;i++){
        ivar_local[i] = rout_con[i].Nupstream_global;
    }    
    gather_double(dvar_global,dvar_local);        
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){
            (*rout_con_global)[i].Nupstream_global = dvar_global[i];
        }
    }
    
    free(dvar_local);
    free(dvar_global);
    free(ivar_local);
    free(ivar_global);
    
    // Initialize output structures    
    for(i=0;i<global_domain.ncells_active;i++){
        (*rout_con_global)[i].upstream_global = 
                malloc((*rout_con_global)[i].Nupstream_global * 
                sizeof(*(*rout_con_global)[i].upstream_global));
        check_alloc_status((*rout_con_global)[i].upstream_global,"Memory allocation error");
    }
    
    // Gather all 2d variables
    dvar_global_2d = malloc(global_domain.ncells_active * sizeof(*dvar_global_2d));
    check_alloc_status(dvar_global_2d,"Memory allocation error");
    dvar_local_2d = malloc(local_domain.ncells_active * sizeof(*dvar_local_2d));
    check_alloc_status(dvar_local_2d,"Memory allocation error");
    svar_global_2d = malloc(global_domain.ncells_active * sizeof(*svar_global_2d));
    check_alloc_status(svar_global_2d,"Memory allocation error");
    svar_local_2d = malloc(local_domain.ncells_active * sizeof(*svar_local_2d));
    check_alloc_status(svar_local_2d,"Memory allocation error");
    for(i=0;i<local_domain.ncells_active;i++){
        dvar_local_2d[i] = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*dvar_local_2d[i]));
        check_alloc_status(dvar_local_2d[i],"Memory allocation error");
        svar_local_2d[i] = malloc(rout_con[i].Nupstream_global * sizeof(*svar_local_2d[i]));
        check_alloc_status(svar_local_2d[i],"Memory allocation error");
    }
    for(i=0;i<global_domain.ncells_active;i++){
        dvar_global_2d[i] = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*dvar_global_2d[i]));
        check_alloc_status(dvar_global_2d[i],"Memory allocation error");
        svar_global_2d[i] = malloc((*rout_con_global)[i].Nupstream_global * sizeof(*svar_global_2d[i]));
        check_alloc_status(svar_global_2d[i],"Memory allocation error");
    }
    
    // Gather uh
    for(i=0;i<local_domain.ncells_active;i++){
        for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
            dvar_local_2d[i][j] = rout_con[i].uh[j];
        }
    }    
    gather_double_2d(dvar_global_2d,dvar_local_2d,global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH);
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){
            (*rout_con_global)[i].uh[j] = dvar_global_2d[i][j];
        }
    }
    
    // Gather discharge
    for(i=0;i<local_domain.ncells_active;i++){
        for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
            dvar_local_2d[i][j] = ext_all_vars[i].rout_var.discharge[j];
        }
    }    
    gather_double_2d(dvar_global_2d,dvar_local_2d,global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH);
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){
            (*ext_all_vars_global)[i].rout_var.discharge[j] = dvar_global_2d[i][j];
        }
    }
    
    // Gather upstream
    for(i=0;i<local_domain.ncells_active;i++){
        for(j=0;j<rout_con[i].Nupstream_global;j++){
            svar_local_2d[i][j] = rout_con[i].upstream_global[j];
        }
    }    
    gather_sizet_2d(svar_global_2d,svar_local_2d,rout_con[i].Nupstream_global);
    if(mpi_rank == VIC_MPI_ROOT){        
        for(i=0;i<global_domain.ncells_active;i++){
            (*rout_con_global)[i].upstream_global[j] = svar_global_2d[i][j];
        }
    }
        
    for(i=0;i<local_domain.ncells_active;i++){
        free(dvar_local_2d[i]);
        free(svar_local_2d[i]);
    }
    for(i=0;i<global_domain.ncells_active;i++){
        free(dvar_global_2d[i]);
        free(svar_global_2d[i]);
    }
    free(dvar_local_2d);
    free(dvar_global_2d);
    free(svar_local_2d);
    free(svar_global_2d);    
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
        for(i=0;i<local_domain.ncells_active;i++){
            runoff = 0;
            inflow = 0;
    
            runoff = (out_data[i][OUT_RUNOFF][0] + out_data[i][OUT_BASEFLOW][0]) / global_param.dt;            
            for(j=0;j<rout_con[i].Nupstream_local;j++){
                inflow += ext_all_vars[rout_con[i].upstream_local[j]].rout_var.discharge[0];
            }            
                    
            rout(&ext_all_vars[i].rout_var.discharge, rout_con[i].uh, (runoff + inflow),
                    (global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH));            
        }
    }
    
    else if(mpi_decomposition == RANDOM_DECOMPOSITION){
        // Gather necessary data from local nodes        
        routing_gather( &rout_con_global, &ext_all_vars_global,&runoff_global);
              
        if(mpi_rank == VIC_MPI_ROOT){
            for(i=0;i<global_domain.ncells_active;i++){
                inflow = 0;
                
                for(j=0;j<rout_con[i].Nupstream_global;j++){
                    inflow += ext_all_vars_global[rout_con_global[i].upstream_global[j]].rout_var.discharge[0];
                }
                
                rout(&ext_all_vars_global[i].rout_var.discharge, rout_con_global[i].uh, (runoff_global[i] + inflow),
                    (global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH)); 
            }
        }
    }
}