#include <ext_driver_shared_image.h>

void
debug_file_sizet(char *path, size_t *data){
    extern domain_struct global_domain;
    
    FILE *file;
    
    size_t active_id;
    size_t i;
    
    if((file = fopen(path, "w"))!=NULL){
        for(i=0;i<global_domain.ncells_total;i++){
            active_id = global_domain.locations[i].global_idx;
            
            if(i % global_domain.n_nx == 0){
                fprintf(file,"\n");
            }
            
            if(active_id==MISSING_USI){
                fprintf(file,"   ; ");
                continue;
            }            
            
            fprintf(file,"%zu",data[active_id]);
            if(data[active_id]<10){
                fprintf(file,"  ; ");
            }else if(data[active_id]<100){
                fprintf(file," ; ");
            }else{
                fprintf(file,"; ");
            }
        }
        fclose(file);
    }
}

void
debug_file_int(char *path, int *data){
    extern domain_struct global_domain;
    
    FILE *file;
    
    size_t active_id;
    size_t i;
    
    if((file = fopen(path, "w"))!=NULL){
        for(i=0;i<global_domain.ncells_total;i++){
            active_id = global_domain.locations[i].global_idx;
            
            if(i % global_domain.n_nx == 0){
                fprintf(file,"\n");
            }
            
            if(active_id==MISSING_USI){
                fprintf(file,"   ; ");
                continue;
            }            
            
            fprintf(file,"%d",data[active_id]);
            if(data[active_id]<10){
                fprintf(file,"  ; ");
            }else if(data[active_id]<100){
                fprintf(file," ; ");
            }else{
                fprintf(file,"; ");
            }
        }
        fclose(file);
    }
}

void
debug_downstream(){  
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern rout_con_struct *rout_con;
    extern int mpi_rank;
    
    size_t *svar_global = NULL;
    size_t *svar_local = NULL;
    
    size_t i;
    
    // Alloc
    if(mpi_rank == VIC_MPI_ROOT){
        svar_global = malloc(global_domain.ncells_active * sizeof(*svar_global));
        check_alloc_status(svar_global,"Memory allocation error");
    }
    svar_local = malloc(local_domain.ncells_active * sizeof(*svar_local));
    check_alloc_status(svar_local,"Memory allocation error");
    
    // Set local downstream
    for(i=0;i<local_domain.ncells_active;i++){
        svar_local[i] = rout_con[i].downstream_global;
    }
    
    // Gather downstream to master node
    gather_sizet(svar_global,svar_local);
    
    // Make debug file
    if(mpi_rank == VIC_MPI_ROOT){
        debug_file_sizet("./debug_output/global_downstream",svar_global);
    }   
    
    // Set local downstream
    for(i=0;i<local_domain.ncells_active;i++){
        svar_local[i] = rout_con[i].downstream_local;
    }
    
    // Gather downstream to master node
    gather_sizet(svar_global,svar_local);
        
    // Make debug file
    if(mpi_rank == VIC_MPI_ROOT){
        debug_file_sizet("./debug_output/local_downstream",svar_global);
    }   
    
    // Free
    free(svar_global);
    free(svar_local);
}

void
debug_nupstream(){  
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern rout_con_struct *rout_con;
    extern int mpi_rank;
    
    int *ivar_global = NULL;
    int *ivar_local = NULL;
    
    size_t i;
    
    ivar_global = malloc(global_domain.ncells_active * sizeof(*ivar_global));
    check_alloc_status(ivar_global,"Memory allocation error");
    ivar_local = malloc(local_domain.ncells_active * sizeof(*ivar_local));
    check_alloc_status(ivar_local,"Memory allocation error");
    
    for(i=0;i<local_domain.ncells_active;i++){
        ivar_local[i] = rout_con[i].Nupstream_global;
    }
    gather_int(ivar_global,ivar_local);
    if(mpi_rank == VIC_MPI_ROOT){
        debug_file_int("./debug_output/global_nupstream",ivar_global);
    }
    
    for(i=0;i<local_domain.ncells_active;i++){
        ivar_local[i] = rout_con[i].Nupstream_local;
    }
    gather_int(ivar_global,ivar_local);
    if(mpi_rank == VIC_MPI_ROOT){
        debug_file_int("./debug_output/local_nupstream",ivar_global);
    }          
    
    free(ivar_global);
    free(ivar_local);        
}

void
debug_id(){
    extern domain_struct global_domain;
    extern int mpi_rank;
    
    size_t *svar_global = NULL;
    
    size_t i;
    
    svar_global = malloc(global_domain.ncells_active * sizeof(*svar_global));
    check_alloc_status(svar_global,"Memory allocation error");
    
    if(mpi_rank == VIC_MPI_ROOT){
        for(i=0;i<global_domain.ncells_active;i++){
            svar_global[i] = i;
        }
        debug_file_sizet("./debug_output/global_id",svar_global);
    }       
    
    free(svar_global);
}