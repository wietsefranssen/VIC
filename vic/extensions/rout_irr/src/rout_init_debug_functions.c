#include <rout.h>


void make_location_file(char file_path[], char file_name[]){    
    extern module_struct rout;
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
    extern module_struct rout;
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
    extern module_struct rout;
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
                    fprintf(file,"%zu;",rout.gridded_cells[x][y-1]->rout->nr_upstream);
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
    extern module_struct rout;
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
                    fprintf(file,"%zu",rout.gridded_cells[x][y-1]->rout->rank);
                    if(rout.gridded_cells[x][y-1]->rout->rank < 10){
                        fprintf(file,"   ;");
                    }else if(rout.gridded_cells[x][y-1]->rout->rank < 100){
                        fprintf(file,"  ;");
                    }else if(rout.gridded_cells[x][y-1]->rout->rank < 1000){
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

void make_uh_file(char file_path[], char file_name[]){    
    extern module_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern module_struct rout;
    
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
            for(j=0;j<rout.param.max_days_uh * global_param.model_steps_per_day;j++){
                fprintf(file,"%2f;",rout.cells[i].rout->uh[j]);
                sum+=rout.cells[i].rout->uh[j];
            }
            fprintf(file," sum = %2f\n",sum);
            sum=0.0;
        }
        fclose(file);
    }
}

void make_dam_file(char file_path[], char file_name[]){    
    extern module_struct rout;
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
        for(i=0;i<rout.nr_dams;i++){
            char purpose[MAXSTRING];
            if(rout.dams[i].function==DAM_IRR_FUNCTION){
                strncpy(purpose, "irrigation", 100);
            }else if(rout.dams[i].function==DAM_HYD_FUNCTION){
                strncpy(purpose, "hydropower", 100);
            }else if(rout.dams[i].function==DAM_CON_FUNCTION){
                strncpy(purpose, "flow control", 100);
            }else{
                strncpy(purpose, "unknown", 100);
            }
            fprintf(file,"Reservoir %zu %s activation year %d  storage capacity %.2f puropse %s",
                    rout.dams[i].id,rout.dams[i].name,rout.dams[i].activation_year,
                    rout.dams[i].capacity,purpose);
            fprintf(file,"\n");
        }        
        fprintf(file,"\n");
        
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL && rout.gridded_cells[x][y-1]->dam!=NULL){
                    fprintf(file,"%zu",rout.gridded_cells[x][y-1]->dam->id);
                    if(rout.gridded_cells[x][y-1]->dam->id < 10){
                        fprintf(file,"  ;");
                    }else if(rout.gridded_cells[x][y-1]->dam->id < 100){
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

void make_dam_service_file(char file_path[], char file_name[]){    
    extern module_struct rout;
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
    for(r=0;r<rout.nr_dams;r++){
                       
        if(rout.dams[r].function!=DAM_IRR_FUNCTION){
            continue;
        }
        
        strcpy(full_path, file_path);
        strcat(full_path, file_name);
        strcat(full_path, "_");
        strcat(full_path, rout.dams[r].name);
        strcat(full_path, ".txt");

        if((file = fopen(full_path, "w"))!=NULL){
            size_t x;
            size_t y;
            for(y=global_domain.n_ny;y>0;y--){
                for(x=0;x<global_domain.n_nx;x++){
                    done=false;
                    if(rout.gridded_cells[x][y-1]==NULL){
                        fprintf(file,"   ;");
                        continue;
                    }
                        
                    if(rout.gridded_cells[x][y-1]->dam!=NULL && rout.gridded_cells[x][y-1]->dam->id == r){
                        fprintf(file," OO;");
                        continue;
                    }

                    if(rout.gridded_cells[x][y-1]->irr==NULL){
                        fprintf(file," XX;");
                        continue;
                    }

                    size_t i;
                    for(i=0;i<rout.gridded_cells[x][y-1]->irr->nr_servicing_dams;i++){

                        if(rout.gridded_cells[x][y-1]->irr->servicing_dams[i]->id==r){

                            fprintf(file,"%zu",rout.gridded_cells[x][y-1]->irr->servicing_dams[i]->id);
                            if(rout.gridded_cells[x][y-1]->irr->servicing_dams[i]->id < 10){
                                fprintf(file,"  ;");
                            }else if(rout.gridded_cells[x][y-1]->irr->servicing_dams[i]->id < 100){
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
                }
            fprintf(file,"\n");
            }
        fclose(file);
        }
    }
}

void make_nr_dam_service_file(char file_path[], char file_name[]){    
    extern module_struct rout;
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
                if(rout.gridded_cells[x][y-1]==NULL){
                    fprintf(file,"   ;"); 
                    continue;
                }
                
                if(rout.gridded_cells[x][y-1]->irr==NULL){
                        fprintf(file," XX;");
                        continue;
                }
                    
                fprintf(file,"%zu",rout.gridded_cells[x][y-1]->irr->nr_servicing_dams);
                if(rout.gridded_cells[x][y-1]->irr->nr_servicing_dams < 10){
                    fprintf(file,"  ;");
                }else if(rout.gridded_cells[x][y-1]->irr->nr_servicing_dams < 100){
                    fprintf(file," ;");
                }else{
                    fprintf(file,";");
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_nr_crops_file(char file_path[], char file_name[]){    
    extern module_struct rout;
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
                if(rout.gridded_cells[x][y-1]==NULL){
                    fprintf(file,"   ;"); 
                    continue;
                }
                
                if(rout.gridded_cells[x][y-1]->irr==NULL){
                        fprintf(file," XX;");
                        continue;
                }
                
                fprintf(file,"%zu",rout.gridded_cells[x][y-1]->irr->nr_crops);
                if(rout.gridded_cells[x][y-1]->irr->nr_crops < 10){
                    fprintf(file,"  ;");
                }else if(rout.gridded_cells[x][y-1]->irr->nr_crops < 100){
                    fprintf(file," ;");
                }else{
                    fprintf(file,";");
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}