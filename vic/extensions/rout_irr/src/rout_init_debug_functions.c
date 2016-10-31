#include <rout.h>

void make_location_file(char file_path[], char file_name[]){
    //makes a file with the VIC id of every cell in a grid
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu",rout.gridded_cells[x][y-1]->id);
                    if(rout.gridded_cells[x][y-1]->id < 10){
                        fprintf(file,"   ;");
                    }else if(rout.gridded_cells[x][y-1]->id < 100){
                        fprintf(file,"  ;");
                    }else if(rout.gridded_cells[x][y-1]->id < 1000){
                        fprintf(file," ;");
                    }else{
                        fprintf(file,";");
                    }
                }else{
                    fprintf(file,"    ;");                    
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_global_location_file(char file_path[], char file_name[]){
    //makes a file with the VIC id of every cell in a grid
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu",rout.gridded_cells[x][y-1]->global_domain_id);
                    if(rout.gridded_cells[x][y-1]->global_domain_id < 10){
                        fprintf(file,"   ;");
                    }else if(rout.gridded_cells[x][y-1]->global_domain_id < 100){
                        fprintf(file,"  ;");
                    }else if(rout.gridded_cells[x][y-1]->global_domain_id < 1000){
                        fprintf(file," ;");
                    }else{
                        fprintf(file,";");
                    }
                }else{
                    fprintf(file,"    ;");                    
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_nr_upstream_file(char file_path[], char file_name[]){
    //makes a file with the number of upstream cells of every cell in a grid
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu;",rout.gridded_cells[x][y-1]->nr_upstream);
                }else{
                    fprintf(file," ;");                    
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_ranked_cells_file(char file_path[], char file_name[]){
    //makes a file with the rank of every cell in a grid
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    size_t **gridded_ranks_temp;
    size_t x;
    size_t y;
    if((gridded_ranks_temp=malloc(global_domain.n_nx * sizeof(*gridded_ranks_temp)))!=NULL){
        for(x=0;x<global_domain.n_nx;x++){
             if((gridded_ranks_temp[x]=malloc(global_domain.n_ny * sizeof(*gridded_ranks_temp[x])))==NULL){
                 log_err("Memory allocation error!");
             }
        }
    }else{
        log_err("Memory allocation error!");
    }
    
    for(x=0;x<global_domain.ncells_active;x++){
        gridded_ranks_temp[rout.sorted_cells[x]->x][rout.sorted_cells[x]->y]=x;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu",gridded_ranks_temp[x][y-1]);
                    if(gridded_ranks_temp[x][y-1] < 10){
                        fprintf(file,"  ;");
                    }else if(gridded_ranks_temp[x][y-1] < 100){
                        fprintf(file," ;");
                    }else{
                        fprintf(file,";");
                    }
                }else{
                    fprintf(file,"   ;");                    
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
        
    for(x=0;x<global_domain.n_nx;x++){
        free(gridded_ranks_temp[x]);
    }
    free(gridded_ranks_temp);
}

void make_uh_file(char file_path[], char file_name[]){
    //makes a file with unit hydrograph values of every cell
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern rout_struct rout;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    double sum=0.0;
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t i;
        size_t j;
        for(i=0;i<global_domain.ncells_active;i++){
            fprintf(file,"Cell = %zu -> ",rout.cells[i].id);
            for(j=0;j<rout.max_days_uh * global_param.model_steps_per_day;j++){
                fprintf(file,"%2f;",rout.cells[i].uh[j]);
                sum+=rout.cells[i].uh[j];
            }
            fprintf(file," sum = %2f\n",sum);
            sum=0.0;
        }
        fclose(file);
    }
}

void make_reservoir_file(char file_path[], char file_name[]){
    //makes a file with the reservoir id of every reservoir in a grid
    //other cells are shown as XX
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
    strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t i;
        fprintf(file,"Reservoirs found in location and time of interest:\n");
        for(i=0;i<rout.nr_reservoirs;i++){
            char purpose[MAXSTRING];
            if(rout.reservoirs[i].function==RES_IRR_FUNCTION){
                strncpy(purpose, "irrigation", 100);
            }else if(rout.reservoirs[i].function==RES_HYD_FUNCTION){
                strncpy(purpose, "hydropower", 100);
            }else if(rout.reservoirs[i].function==RES_CON_FUNCTION){
                strncpy(purpose, "flow control", 100);
            }else{
                strncpy(purpose, "unknown", 100);
            }
            fprintf(file,"Reservoir %zu %s activation year %d  storage capacity %.2f puropse %s",
                    rout.reservoirs[i].id,rout.reservoirs[i].name,rout.reservoirs[i].activation_year,
                    rout.reservoirs[i].storage_capacity,purpose);
            if(rout.reservoirs[i].function==RES_IRR_FUNCTION){
                fprintf(file," servicing cells %zu",rout.reservoirs[i].nr_serviced_cells);
            }
            fprintf(file,"\n");
        }        
        fprintf(file,"\n");
        
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL && rout.gridded_cells[x][y-1]->reservoir!=NULL){
                    fprintf(file,"%zu",rout.gridded_cells[x][y-1]->reservoir->id);
                    if(rout.gridded_cells[x][y-1]->reservoir->id < 10){
                        fprintf(file,"  ;");
                    }else if(rout.gridded_cells[x][y-1]->reservoir->id < 100){
                        fprintf(file," ;");
                    }else{
                        fprintf(file,";");
                    }
                }else{
                    if(rout.gridded_cells[x][y-1]!=NULL){
                        fprintf(file," XX;");
                    }else{
                        fprintf(file,"   ;");                          
                    }                  
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_reservoir_service_file(char file_path[], char file_name[]){
    //makes a file with the servicing reservoir id of every cell in a grid
    //other cells are shown as XX
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    
    bool done;
    size_t r;
    for(r=0;r<rout.nr_reservoirs;r++){
                       
        if(rout.reservoirs[r].function!=RES_IRR_FUNCTION){
            continue;
        }
        
        strcpy(full_path, file_path);
        strcat(full_path, file_name);
        strcat(full_path, "_");
        strcat(full_path, rout.reservoirs[r].name);
        strcat(full_path, ".txt");

        if((file = fopen(full_path, "w"))!=NULL){
            size_t x;
            size_t y;
            for(y=global_domain.n_ny;y>0;y--){
                for(x=0;x<global_domain.n_nx;x++){
                    done=false;
                    if(rout.gridded_cells[x][y-1]!=NULL){
                        
                        if(rout.gridded_cells[x][y-1]->reservoir!=NULL && rout.gridded_cells[x][y-1]->reservoir->id == r){
                            fprintf(file," OO;");
                            continue;
                        }
                        
                        size_t i;
                        for(i=0;i<rout.gridded_cells[x][y-1]->nr_servicing_reservoirs;i++){
                            
                            if(rout.gridded_cells[x][y-1]->servicing_reservoirs[i]->id==r){
                                
                                fprintf(file,"%zu",rout.gridded_cells[x][y-1]->servicing_reservoirs[i]->id);
                                if(rout.gridded_cells[x][y-1]->servicing_reservoirs[i]->id < 10){
                                    fprintf(file,"  ;");
                                }else if(rout.gridded_cells[x][y-1]->servicing_reservoirs[i]->id < 100){
                                    fprintf(file," ;");
                                }else{
                                    fprintf(file,";");
                                }
                                
                                done=true;
                                break;
                            }
                        }
                        
                        if(!done){
                            fprintf(file," XX;");
                        }
                    }else{
                        fprintf(file,"   ;");                          
                    }
                }
            fprintf(file,"\n");
            }
        fclose(file);
        }
    }
}

void make_nr_reservoir_service_file(char file_path[], char file_name[]){
    //makes a file with the number of servicing reservoirs of every cell in a grid
    //cells without irrigated vegetation are shown as XX
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    if(rout.gridded_cells[x][y-1]->irrigate){
                        fprintf(file,"%zu",rout.gridded_cells[x][y-1]->nr_servicing_reservoirs);
                        if(rout.gridded_cells[x][y-1]->nr_servicing_reservoirs < 10){
                            fprintf(file,"  ;");
                        }else if(rout.gridded_cells[x][y-1]->nr_servicing_reservoirs < 100){
                            fprintf(file," ;");
                        }else{
                            fprintf(file,";");
                        }
                    }else{
                    fprintf(file," XX;");
                    }
                }else{
                    fprintf(file,"   ;");            
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_nr_crops_file(char file_path[], char file_name[]){
    //makes a file with the number of servicing reservoirs of every cell in a grid
    //cells without irrigated vegetation are shown as XX
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
    strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    if(rout.gridded_cells[x][y-1]->irrigate){
                        fprintf(file,"%zu",rout.gridded_cells[x][y-1]->nr_crops);
                        if(rout.gridded_cells[x][y-1]->nr_crops < 10){
                            fprintf(file,"  ;");
                        }else if(rout.gridded_cells[x][y-1]->nr_crops < 100){
                            fprintf(file," ;");
                        }else{
                            fprintf(file,";");
                        }
                    }else{
                    fprintf(file," XX;");
                    }
                }else{
                    fprintf(file,"   ;");            
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}