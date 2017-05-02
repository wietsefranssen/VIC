#include <rout.h>


void make_location_file(char file_path[], char file_name[]){    
    extern RID_struct RID;
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
                if(RID.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu",RID.gridded_cells[x][y-1]->id);
                    if(RID.gridded_cells[x][y-1]->id < 10){
                        fprintf(file,"   ;");
                    }else if(RID.gridded_cells[x][y-1]->id < 100){
                        fprintf(file,"  ;");
                    }else if(RID.gridded_cells[x][y-1]->id < 1000){
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
    extern RID_struct RID;
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
                if(RID.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu",RID.gridded_cells[x][y-1]->global_domain_id);
                    if(RID.gridded_cells[x][y-1]->global_domain_id < 10){
                        fprintf(file,"   ;");
                    }else if(RID.gridded_cells[x][y-1]->global_domain_id < 100){
                        fprintf(file,"  ;");
                    }else if(RID.gridded_cells[x][y-1]->global_domain_id < 1000){
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
    extern RID_struct RID;
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
                if(RID.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu;",RID.gridded_cells[x][y-1]->rout->nr_upstream);
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
    extern RID_struct RID;
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
                if(RID.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu",RID.gridded_cells[x][y-1]->rout->rank);
                    if(RID.gridded_cells[x][y-1]->rout->rank < 10){
                        fprintf(file,"   ;");
                    }else if(RID.gridded_cells[x][y-1]->rout->rank < 100){
                        fprintf(file,"  ;");
                    }else if(RID.gridded_cells[x][y-1]->rout->rank < 1000){
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
    extern RID_struct RID;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    
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
            fprintf(file,"Cell = %zu -> ",RID.cells[i].id);
            for(j=0;j<MAX_UH_DAYS * global_param.model_steps_per_day;j++){
                fprintf(file,"%2f;",RID.cells[i].rout->uh[j]);
                sum+=RID.cells[i].rout->uh[j];
            }
            fprintf(file," sum = %2f\n",sum);
            sum=0.0;
        }
        fclose(file);
    }
}

void make_dam_file(char file_path[], char file_name[]){    
    extern RID_struct RID;
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
        fprintf(file,"Dams found in location and time of interest:\n");
        for(i=0;i<RID.nr_dams;i++){
            char purpose[MAXSTRING];
            if(RID.dams[i].function==DAM_IRR_FUNCTION){
                strncpy(purpose, "irrigation", 100);
            }else if(RID.dams[i].function==DAM_HYD_FUNCTION){
                strncpy(purpose, "hydropower", 100);
            }else if(RID.dams[i].function==DAM_CON_FUNCTION){
                strncpy(purpose, "flow control", 100);
            }else{
                strncpy(purpose, "unknown", 100);
            }
            fprintf(file,"Dam %zu %s \tactivation year %d \tstorage capacity %.2f \tpuropse %s",
                    RID.dams[i].global_id,RID.dams[i].name,RID.dams[i].activation_year,
                    RID.dams[i].capacity,purpose);
            fprintf(file,"\n");
        }        
        fprintf(file,"\n");
        
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(RID.gridded_cells[x][y-1]!=NULL && RID.gridded_cells[x][y-1]->dam!=NULL){
                    fprintf(file,"%zu",RID.gridded_cells[x][y-1]->dam->global_id);
                    if(RID.gridded_cells[x][y-1]->dam->global_id < 10){
                        fprintf(file,"   ;");
                    }else if(RID.gridded_cells[x][y-1]->dam->global_id < 100){
                        fprintf(file,"  ;");
                    }else if(RID.gridded_cells[x][y-1]->dam->global_id < 1000){
                        fprintf(file," ;");
                    }else{
                        fprintf(file,";");
                    }
                }else{
                    if(RID.gridded_cells[x][y-1]!=NULL){
                        fprintf(file,"  XX;");
                    }else{
                        fprintf(file,"    ;");                          
                    }                  
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_dam_service_file(char file_path[], char file_name[]){    
    extern RID_struct RID;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    
    size_t r;
    for(r=0;r<RID.nr_dams;r++){
                       
        if(RID.dams[r].function!=DAM_IRR_FUNCTION){
            continue;
        }
        
        strcpy(full_path, file_path);
        strcat(full_path, file_name);
        strcat(full_path, "_");
        strcat(full_path, RID.dams[r].name);
        strcat(full_path, ".txt");

        if((file = fopen(full_path, "w"))!=NULL){
            size_t x;
            size_t y;
            for(y=global_domain.n_ny;y>0;y--){
                for(x=0;x<global_domain.n_nx;x++){
                    if(RID.gridded_cells[x][y-1]==NULL){
                        fprintf(file,"   ;");
                        continue;
                    }

                    if(RID.gridded_cells[x][y-1]->irr==NULL){
                        if(RID.gridded_cells[x][y-1]->dam!=NULL && RID.gridded_cells[x][y-1]->dam->global_id == RID.dams[r].global_id){
                            fprintf(file," OO;");
                        }else{
                            fprintf(file," XX;");
                        }
                        continue;
                    }
                        
                    if(RID.gridded_cells[x][y-1]->irr->servicing_dam!=NULL){
                        if(RID.gridded_cells[x][y-1]->irr->servicing_dam->global_id==RID.dams[r].global_id){
                            
                            if(RID.gridded_cells[x][y-1]->dam!=NULL && RID.gridded_cells[x][y-1]->dam->global_id == RID.dams[r].global_id){
                                fprintf(file," O~;");
                            }else{
                                fprintf(file," ~~;");
                            }  
                        }else{
                            if(RID.gridded_cells[x][y-1]->dam!=NULL && RID.gridded_cells[x][y-1]->dam->global_id == RID.dams[r].global_id){
                                fprintf(file," OO;");
                            }else{
                                fprintf(file," XX;");
                            }  
                        }
                    }else{
                        if(RID.gridded_cells[x][y-1]->dam!=NULL && RID.gridded_cells[x][y-1]->dam->global_id == RID.dams[r].global_id){
                            fprintf(file," OO;");
                        }else{
                            fprintf(file," XX;");
                        }  
                    }
                }
            fprintf(file,"\n");
            }
        fclose(file);
        }
    }
}

void make_nr_crops_file(char file_path[], char file_name[]){    
    extern RID_struct RID;
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
                if(RID.gridded_cells[x][y-1]==NULL){
                    fprintf(file,"   ;"); 
                    continue;
                }
                
                if(RID.gridded_cells[x][y-1]->irr==NULL){
                        fprintf(file," XX;");
                        continue;
                }
                
                fprintf(file,"%zu",RID.gridded_cells[x][y-1]->irr->nr_crops);
                if(RID.gridded_cells[x][y-1]->irr->nr_crops < 10){
                    fprintf(file,"  ;");
                }else if(RID.gridded_cells[x][y-1]->irr->nr_crops < 100){
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