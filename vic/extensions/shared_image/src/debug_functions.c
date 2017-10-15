#include <ext_driver_shared_image.h>

void
debug_map_nc_sizet(char *path, size_t *data, size_t fill_value){    
    extern domain_struct global_domain;
    
    int new_data[global_domain.ncells_active];
    
    size_t i;
    
    for(i=0;i<global_domain.ncells_active;i++){
        new_data[i]=(int)data[i];
    }
    
    debug_map_nc_int(path,new_data,(int)fill_value);
}

void
debug_map_nc_int(char *path, int *data, int fill_value){
    extern domain_struct global_domain;
    
    int status;
    int nc_id;
    int lat_id;
    int lon_id;
    int lat_var_id;
    int lon_var_id;
    int dimids[2];
    size_t dstart[2];
    size_t dcount[2];
    int var_id;
    int new_data[global_domain.ncells_total];
    double lat_data[global_domain.ncells_total];
    double lon_data[global_domain.ncells_total];
    
    size_t i;
    size_t j;
    
    j=0;
    for(i=0;i<global_domain.ncells_total;i++){
        if(global_domain.locations[i].run){
            new_data[i]=data[j];
            j++;
        }else{
            new_data[i]=fill_value;
        }
        
        lat_data[i] = global_domain.locations[i].latitude;
        lon_data[i] = global_domain.locations[i].longitude;
    }
    
    status = nc_create(path,NC_NETCDF4,&nc_id);
    check_nc_status(status, "Error creating debug file %s",path);
    
    status = nc_def_dim(nc_id, "lat", global_domain.n_ny,
                        &lat_id);
    check_nc_status(status, "Error defining lat in %s",path);
    status = nc_def_dim(nc_id, "lon", global_domain.n_nx,
                        &lon_id);
    check_nc_status(status, "Error defining lon in %s",path);
    
    dimids[0]=lat_id;
    dimids[1]=lon_id;
    
    status = nc_def_var(nc_id, "lat", NC_DOUBLE, 2,
                        dimids, &lat_var_id);
    check_nc_status(status, "Error defining lat var in %s",path);
    status = nc_put_att_text(nc_id, lat_var_id, "units",
                             strlen("degrees_north"), "degrees_north");
    check_nc_status(status, "Error defining lat var units in %s",path);
    
    status = nc_def_var(nc_id, "lon", NC_DOUBLE, 2,
                        dimids, &lon_var_id);
    check_nc_status(status, "Error defining lon var in %s",path);
    status = nc_put_att_text(nc_id, lon_var_id, "units",
                             strlen("degrees_east"), "degrees_east");
    check_nc_status(status, "Error defining lon var units in %s",path);
    
    status = nc_def_var(nc_id, "variable", NC_INT, 2,
                        dimids, &var_id);
    check_nc_status(status, "Error defining variable in %s",path);
    status = nc_put_att_int(nc_id, var_id, "_FillValue", 
                            NC_INT, 1, &fill_value);
    check_nc_status(status, "Error defining variable fill value in %s",path);
    
    status = nc_enddef(nc_id);
    check_nc_status(status, "Error ending defining in %s",path);
    
    dstart[0]=0;
    dstart[1]=0;
    dcount[0]=global_domain.n_ny;
    dcount[1]=global_domain.n_nx;
    
    status = nc_put_vara_double(nc_id,lat_var_id,dstart,dcount,lat_data);
    check_nc_status(status, "Error putting lat data %s",path);
    
    status = nc_put_vara_double(nc_id,lon_var_id,dstart,dcount,lon_data);
    check_nc_status(status, "Error putting lon data %s",path);
    
    status = nc_put_vara_int(nc_id,var_id,dstart,dcount,new_data);
    check_nc_status(status, "Error putting data %s",path);
    
    nc_close(nc_id);
    check_nc_status(status, "Error closing %s",path); 
}

void
debug_map_file_sizet(char *path, size_t *data){
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
debug_map_file_int(char *path, int *data){
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
        svar_local[i] = rout_con[i].downstream;
    }
    
    // Gather downstream to master node
    gather_sizet(svar_global,svar_local);
    
    // Make debug file
    if(mpi_rank == VIC_MPI_ROOT){
        debug_map_file_sizet("./debug_output/downstream",svar_global);
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
        ivar_local[i] = rout_con[i].Nupstream;
    }
    gather_int(ivar_global,ivar_local);
    if(mpi_rank == VIC_MPI_ROOT){
        debug_map_file_int("./debug_output/nupstream",ivar_global);
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
        debug_map_file_sizet("./debug_output/id",svar_global);
        debug_map_nc_sizet("./debug_output/id.nc",svar_global,NODATA_BASIN);
    }       
    
    free(svar_global);
}

void
debug_basins(){
    extern basin_struct basins;
    extern int mpi_rank;
    
    if(mpi_rank == VIC_MPI_ROOT){
        debug_map_file_sizet("./debug_output/basins",basins.basin_map);
        debug_map_nc_sizet("./debug_output/basins.nc",basins.basin_map,NODATA_BASIN);
    }    
}

void
debug_node_domain(){
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
        
    int *ivar_global = NULL;
    int *ivar_local = NULL;
    
    size_t i;
    
    ivar_global = malloc(global_domain.ncells_active * sizeof(*ivar_global));
    check_alloc_status(ivar_global,"Memory allocation error");
    ivar_local = malloc(local_domain.ncells_active * sizeof(*ivar_local));
    check_alloc_status(ivar_local,"Memory allocation error");
    
    for(i=0;i<global_domain.ncells_active;i++){
        ivar_global[i]=-1;
    }
    
    for(i=0;i<local_domain.ncells_active;i++){
        ivar_local[i] = mpi_rank;
    }
    gather_int(ivar_global,ivar_local);
    if(mpi_rank == VIC_MPI_ROOT){
        debug_map_nc_int("./debug_output/node_domain.nc",ivar_global,-1);
    }
        
    free(ivar_global);
    free(ivar_local);
}

void
debug_uh(){
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern rout_con_struct *rout_con;
    extern ext_option_struct ext_options;
    extern int mpi_rank;
    
    double **uh_local;
    double **uh_global;
    
    FILE *file;
    
    double uh_sum;
    
    size_t i;
    size_t j;
    
    // Alloc
    if(mpi_rank == VIC_MPI_ROOT){
        uh_global = malloc(global_domain.ncells_active * sizeof(*uh_global));
        check_alloc_status(uh_global,"Memory allocation error");
        for(i=0;i<global_domain.ncells_active;i++){
            uh_global[i] = malloc(ext_options.uh_steps * sizeof(*uh_global[i]));
            check_alloc_status(uh_global[i],"Memory allocation error");
        }
    }
    uh_local = malloc(local_domain.ncells_active * sizeof(*uh_local));
    check_alloc_status(uh_local,"Memory allocation error");
    for(i=0;i<local_domain.ncells_active;i++){
        uh_local[i] = malloc(ext_options.uh_steps * sizeof(*uh_local[i]));
        check_alloc_status(uh_local[i],"Memory allocation error");
    }
    
    for(i=0;i<local_domain.ncells_active;i++){
        for(j=0;j<ext_options.uh_steps;j++){
            uh_local[i][j] = rout_con[i].uh[j];
        }
    }
    gather_double_2d(uh_global,uh_local,ext_options.uh_steps);    
    if(mpi_rank == VIC_MPI_ROOT){    
        if((file = fopen("./debug_output/uh", "w"))!=NULL){
            for(i=0;i<global_domain.ncells_active;i++){
                uh_sum = 0.0;
                for(j=0;j<ext_options.uh_steps;j++){
                    fprintf(file,"%.2f; ",uh_global[i][j]);
                    uh_sum += uh_global[i][j];
                }
                fprintf(file,"SUM = %.2f; ",uh_sum);
                fprintf(file,"\n");
            }
            fclose(file);
        }
    }
    
        
    if(mpi_rank == VIC_MPI_ROOT){
        free(uh_global);
    }
    free(uh_local);
}