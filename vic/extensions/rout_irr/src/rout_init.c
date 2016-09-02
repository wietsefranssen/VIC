/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>
#include <netcdf.h>
#include <rout.h>
#include <math.h>
#include <string.h>
#define M_PI 3.14159265358979323846

void rout_init(void){
    extern rout_struct rout;
    
    set_cell_location();
    //make_location_file(rout.debug_path,"location.txt");
    
    set_upstream(rout.param_filename,"flow_direction");
    //make_nr_upstream_file(rout.debug_path,"nr_upstream.txt");
    
    set_uh(rout.param_filename,"flow_distance");
    //make_uh_file(rout.debug_path,"uh.txt");
    
    sort_cells();
    //make_ranked_cells_file(rout.debug_path,"ranked_cells.txt");
    
    set_values();
    
    //only use on small domains since netCDF cannot hold size_t values!
    //----------------------------------------------------------------------
    //make_debug_file(rout.debug_path,"debug.nc");
}

//connects the cells to a location
//and puts them in a grid
void set_cell_location(){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
         
    size_t i;
    size_t j=0;
    double min_lat=DBL_MAX;
    double min_lon=DBL_MAX;
    for(i=0;i<global_domain.ncells_total;i++){
        if(global_domain.locations[i].run){
            rout.cells[j].id=i;
            rout.cells[j].local_id=j;
            rout.cells[j].location=&global_domain.locations[i];
            j++;
        }
        
        if(global_domain.locations[i].latitude<min_lat){
            min_lat=global_domain.locations[i].latitude;
        }
        if(global_domain.locations[i].longitude<min_lon){
            min_lon=global_domain.locations[i].longitude;
        }
    }
    
    for(i=0;i<global_domain.ncells_active;i++){
        size_t x = (size_t)((rout.cells[i].location->longitude - min_lon)/global_param.resolution);
        size_t y = (size_t)((rout.cells[i].location->latitude - min_lat)/global_param.resolution);
        rout.gridded_cells[x][y]=&rout.cells[i];
        rout.cells[i].x=x;
        rout.cells[i].y=y;
    }
}

//determines the upstream cells from the direction map
//and sets the number of upstream cells
void set_upstream(char file_path[], char variable_name[]){
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    int *direction;
    if((direction = malloc(global_domain.ncells_total * sizeof(*direction)))==NULL){
        log_err("Memory allocation for <set_upstream> direction failed!");
    }
    
    size_t start[]={0, 0};
    size_t count[]={global_domain.n_ny, global_domain.n_nx};
    
    get_nc_field_int(file_path,variable_name,start,count,direction);
        
    size_t i;    
    for(i=0;i<global_domain.ncells_total;i++){
        size_t x=(size_t)i%global_domain.n_nx;
        size_t y=(size_t)i/global_domain.n_nx;
        
        if(rout.gridded_cells[x][y]!=NULL){
            int j;
            
            if(direction[i]==-1){
                log_warn("direction of cell (id %zu local_id %zu) is missing, check direction file",rout.gridded_cells[x][y]->id,rout.gridded_cells[x][y]->local_id);
            }else if(direction[i]==1){
                if(y+1<global_domain.n_ny && rout.gridded_cells[x][y+1]!=NULL){
                    for(j=0;j<8;j++){
                        if(rout.gridded_cells[x][y+1]->upstream[j]==NULL){
                            rout.gridded_cells[x][y+1]->upstream[j] = rout.gridded_cells[x][y];  
                            break;
                        }
                    }
                }
            }else if(direction[i]==2){
                if(x+1<global_domain.n_nx && y+1<global_domain.n_ny && rout.gridded_cells[x+1][y+1]!=NULL){
                    for(j=0;j<8;j++){
                        if(rout.gridded_cells[x+1][y+1]->upstream[j]==NULL){
                            rout.gridded_cells[x+1][y+1]->upstream[j] = rout.gridded_cells[x][y];  
                            break;                            
                        }
                    }
                }
            }else if(direction[i]==3){
                if(x+1<global_domain.n_nx && rout.gridded_cells[x+1][y]!=NULL){
                    for(j=0;j<8;j++){
                        if(rout.gridded_cells[x+1][y]->upstream[j]==NULL){
                            rout.gridded_cells[x+1][y]->upstream[j] = rout.gridded_cells[x][y];  
                            break;                            
                        }
                    }
                }
            }else if(direction[i]==4){
                if(x+1<global_domain.n_nx && y>=1 && rout.gridded_cells[x+1][y-1]!=NULL){
                    for(j=0;j<8;j++){
                        if(rout.gridded_cells[x+1][y-1]->upstream[j]==NULL){
                            rout.gridded_cells[x+1][y-1]->upstream[j] = rout.gridded_cells[x][y];  
                            break;                            
                        }
                    }
                }
            }else if(direction[i]==5){
                if(y>=1 && rout.gridded_cells[x][y-1]!=NULL){
                    for(j=0;j<8;j++){
                        if(rout.gridded_cells[x][y-1]->upstream[j]==NULL){
                            rout.gridded_cells[x][y-1]->upstream[j] = rout.gridded_cells[x][y];  
                            break;                            
                        }
                    }
                }
            }else if(direction[i]==6){
                if(y>=1 && x>=1 && rout.gridded_cells[x-1][y-1]!=NULL){
                    for(j=0;j<8;j++){
                        if(rout.gridded_cells[x-1][y-1]->upstream[j]==NULL){
                            rout.gridded_cells[x-1][y-1]->upstream[j] = rout.gridded_cells[x][y];  
                            break;                            
                        }
                    }
                }
            }else if(direction[i]==7){
                if(x>=1 && rout.gridded_cells[x-1][y]!=NULL){
                    for(j=0;j<8;j++){
                        if(rout.gridded_cells[x-1][y]->upstream[j]==NULL){
                            rout.gridded_cells[x-1][y]->upstream[j] = rout.gridded_cells[x][y];  
                            break;                            
                        }
                    }
                }
            }else if(direction[i]==8){
                if(x>=1 && y+1<global_domain.n_ny && rout.gridded_cells[x-1][y+1]!=NULL){
                    for(j=0;j<8;j++){
                        if(rout.gridded_cells[x-1][y+1]->upstream[j]==NULL){
                            rout.gridded_cells[x-1][y+1]->upstream[j] = rout.gridded_cells[x][y];  
                            break;                            
                        }
                    }
                }
            }
        }
    }
    
    for(i=0;i<global_domain.ncells_active;i++){
        rout.cells[i].nr_upstream=0;
        
        int j;
        for(j=0;j<8;j++){
            if(rout.cells[i].upstream[j]!=NULL){
                rout.cells[i].nr_upstream++;
            }
        }
    }
    
    free(direction);
}

//sorts cells
void sort_cells(void){
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    //used to see which cells are already sorted
    int *sorted_map;
    if((sorted_map = malloc(global_domain.ncells_active * sizeof(*sorted_map)))==NULL){
        log_err("Memory allocation for <sort_cells> sorted_map failed!");
    }
    
    size_t i;
    for(i=0;i<global_domain.ncells_active;i++){
        sorted_map[i]=0;
    }
    
    
    size_t rank=0;
    int j;
    while(1){
        for(i=0;i<global_domain.ncells_active;i++){
            if(sorted_map[i]==0){
                int count=0;
                for(j=0;j<rout.cells[i].nr_upstream;j++){
                    if(sorted_map[rout.cells[i].upstream[j]->local_id]==0){
                        count++;
                    }
                }

                if(count==0){
                    rout.sorted_cells[rank]=&rout.cells[i];
                    rout.cells[i].rank=rank;
                    rank++;
                }  
            }
        }
        
        for(i=0;i<rank;i++){
            sorted_map[rout.sorted_cells[i]->local_id]=1;
        }
        
        if(rank == global_domain.ncells_active){
            break;
        }else if(rank > global_domain.ncells_active){
            log_warn("rank_cells made %zu loops and escaped because this is more than %zu, the number of active cells",(rank+1),global_domain.ncells_active);
            break;
        }
    }
    
    free(sorted_map);
}

void set_uh(char file_path[], char variable_name[]){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    
    double *distance_total;
    if((distance_total = malloc(global_domain.ncells_total * sizeof(*distance_total)))==NULL){
        log_err("Memory allocation for <set_uh> distance_total failed!");
    }
    double *distance;
    if((distance = malloc(global_domain.ncells_active * sizeof(*distance)))==NULL){
        log_err("Memory allocation for <set_uh> distance failed!");
    }
    double *uh_precise;
    if((uh_precise = malloc((UH_MAX_DAYS * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_precise)))==NULL){
        log_err("Memory allocation for <set_uh> uh_precise failed!");
    }
    double *uh_cumulative;
    if((uh_cumulative = malloc((UH_MAX_DAYS * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_cumulative)))==NULL){
        log_err("Memory allocation for <set_uh> uh_cumulative failed!");
    }
    double uh_sum;
    
    //FIXME: make the following values adjustable (and understandable)
    const double overland_flow_velocity=1;
    const double overland_flow_diffusivity=2000;
    
    size_t start[]={0, 0};
    size_t count[]={global_domain.n_ny, global_domain.n_nx};
    
    get_nc_field_double(file_path,variable_name,start,count,distance_total);
    
    size_t i;
    size_t j=0;
    for(i=0;i<global_domain.ncells_total;i++){
        size_t x=(size_t)i%global_domain.n_nx;
        size_t y=(size_t)i/global_domain.n_nx;
        
        if(rout.gridded_cells[x][y]!=NULL){
            distance[j]=distance_total[i];
            if(distance[j]==-1){
                log_warn("distance of cell id %zu local_id %zu is missing, check distance file",rout.gridded_cells[x][y]->id,rout.gridded_cells[x][y]->local_id);
            }
            j++;
        }
    }
    
    for (i=0;i<global_domain.ncells_active;i++){
        if(distance[i]!=-1){
            size_t time=0;
            uh_sum=0.0;

            //calculate precise unit hydrograph based on timestep
            for(j=0;j< UH_MAX_DAYS * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
                time += (3600 * 24) / (global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP);
                uh_precise[j]=(distance[i]/(2 * time * sqrt(M_PI * time * overland_flow_diffusivity)))
                        * exp(-(pow(overland_flow_velocity * time - distance[i],2)) / (4 * overland_flow_diffusivity * time));
                uh_sum += uh_precise[j];
            }

            //normalize unit hydrograph so sum is 1 and make cumulative unit hydrograph
            for(j=0;j< UH_MAX_DAYS * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
                uh_precise[j] = uh_precise[j] / uh_sum;
                if(j>0){
                    uh_cumulative[j] = uh_cumulative [j-1] + uh_precise[j];
                    if(uh_cumulative[j]>1){
                        uh_cumulative[j]=1.0;
                    }
                }else{
                    uh_cumulative[j] = uh_precise[j];
                }
            }
            
            //make final daily unit hydrograph based on cumulative unity hydrograph
            for(j=0;j< UH_MAX_DAYS * global_param.model_steps_per_day;j++){
                if(j<(UH_MAX_DAYS * global_param.model_steps_per_day)- 1){
                    rout.cells[i].uh[j]=uh_cumulative[(j+1) * UH_STEPS_PER_TIMESTEP] - uh_cumulative[j * UH_STEPS_PER_TIMESTEP];
                }else{
                    rout.cells[i].uh[j]=uh_cumulative[((j+1) * UH_STEPS_PER_TIMESTEP)-1] - uh_cumulative[j * UH_STEPS_PER_TIMESTEP];
                }
            }
        }
    }
    
    free(distance_total);
    free(distance);
    free(uh_cumulative);
    free(uh_precise);
}

void set_values(){
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern rout_struct rout;
    
    size_t i;
    size_t j;
    for(i=0;i<global_domain.ncells_active;i++){
        for(j=0;j<UH_MAX_DAYS * global_param.model_steps_per_day;j++){
            rout.cells[i].outflow[j]=0.0;
        }
    }
}

void make_debug_file(char file_path[], char file_name[]){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    
    int nr_upstream[global_domain.n_nx][global_domain.n_ny];
    int rank[global_domain.n_nx][global_domain.n_ny];
    int id[global_domain.n_nx][global_domain.n_ny];
    double uh[global_domain.n_nx][global_domain.n_ny][UH_MAX_DAYS * global_param.model_steps_per_day];
    
    size_t x;
    size_t y;
    size_t t;
    for(x=0;x<global_domain.n_nx;x++){
        for(y=0;y<global_domain.n_ny;y++){
            nr_upstream[x][y]=-1;
            rank[x][y]=-1;
            id[x][y]=-1;
            for(t=0;t<UH_MAX_DAYS * global_param.model_steps_per_day;t++){
                uh[x][y][t]=-1.0;
            }
        }
    }
    
    for(y=0;y<global_domain.n_ny;y++){
        for(x=0;x<global_domain.n_nx;x++){
            if(rout.gridded_cells[x][y]!=NULL){
                nr_upstream[x][global_domain.n_ny - y-1]=(int)rout.gridded_cells[x][y]->nr_upstream;

                size_t r;
                for(r=0;r<global_domain.ncells_active;r++){
                    if(rout.sorted_cells[r]==rout.gridded_cells[x][y]){
                        rank[x][global_domain.n_ny - y-1]=(int)r;
                    }
                }

                id[x][global_domain.n_ny - y-1]=(int)rout.gridded_cells[x][y]->id;
                for(t=0;t<UH_MAX_DAYS * global_param.model_steps_per_day;t++){
                    uh[x][global_domain.n_ny - y-1][t]=rout.gridded_cells[x][y]->uh[t];
                }
            }
        }
    }
    
    int nc_status, nc_id, nc_nr_upstream_varid,nc_rank_varid,nc_vic_id_varid,nc_uh_varid,x_dimid,y_dimid,uh_dimid;
    int dimids[3];
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
    
    if((nc_status = nc_create(full_path, NC_CLOBBER, &nc_id))){
        log_err("Unable to create .nc file");
    }
    if((nc_status = nc_def_dim(nc_id, "x_axis", global_domain.n_nx, &x_dimid))){
        log_err("Unable to define dimension");
    }
    if((nc_status = nc_def_dim(nc_id, "y_axis", global_domain.n_ny, &y_dimid))){
        log_err("Unable to define dimension");
    }
    if((nc_status = nc_def_dim(nc_id, "uh_length", (UH_MAX_DAYS * global_param.model_steps_per_day), &uh_dimid))){
        log_err("Unable to define dimension");
    }
    dimids[0]=x_dimid;
    dimids[1]=y_dimid;
    dimids[2]=uh_dimid;
    
    if((nc_status = nc_def_var(nc_id,"nr_upstream",NC_INT,2,dimids,&nc_nr_upstream_varid))){
        log_err("Unable to define variable");
    }
    if((nc_status = nc_def_var(nc_id,"rank",NC_INT,2,dimids,&nc_rank_varid))){
        log_err("Unable to define variable");
    }
    if((nc_status = nc_def_var(nc_id,"vic_id",NC_INT,2,dimids,&nc_vic_id_varid))){
        log_err("Unable to define variable");
    }
    if((nc_status = nc_def_var(nc_id,"uh",NC_DOUBLE,3,dimids,&nc_uh_varid))){
        log_err("Unable to define variable");
    }
    if((nc_status = nc_enddef(nc_id))){
        log_err("Unable to end defining .nc file");
    }
    
    if((nc_status = nc_put_var_int(nc_id,nc_nr_upstream_varid,&nr_upstream[0][0]))){
        log_err("Unable to put variable in .nc file");
    }
    if((nc_status = nc_put_var_int(nc_id,nc_rank_varid,&rank[0][0]))){
        log_err("Unable to put variable in .nc file");
    }
    if((nc_status = nc_put_var_int(nc_id,nc_vic_id_varid,&id[0][0]))){
        log_err("Unable to put variable in .nc file");
    }
    if((nc_status = nc_put_var_double(nc_id,nc_uh_varid,&uh[0][0][0]))){
        log_err("Unable to put variable in .nc file");
    }
    
    /*static int missing_value[] = {0, 8};
    if((nc_status = nc_put_att_int(nc_id,nc_nr_upstream_varid,"valid_range",NC_INT,2,missing_value))){
        log_err("Unable to define missing value");
    }*/
    
    if((nc_status = nc_close(nc_id))){
        log_err("Unable to close .nc file");
    }
}

//create ascii file with id's in a grid
void make_location_file(char file_path[], char file_name[]){
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

//create ascii file with the number of upstream cells in a grid
void make_nr_upstream_file(char file_path[], char file_name[]){
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
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%d;",rout.gridded_cells[x][y-1]->nr_upstream);
                }else{
                    fprintf(file," ;");                    
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

//create ascii file with the cell rankings in a grid
void make_ranked_cells_file(char file_path[], char file_name[]){
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
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu",rout.gridded_cells[x][y-1]->rank);
                    if(rout.gridded_cells[x][y-1]->rank < 10){
                        fprintf(file,"  ;");
                    }else if(rout.gridded_cells[x][y-1]->rank < 100){
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
}

void make_uh_file(char file_path[], char file_name[]){
    extern rout_struct rout;
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
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t i;
        size_t j;
        for(i=0;i<global_domain.ncells_active;i++){
            fprintf(file,"Cell = %zu -> ",rout.cells[i].id);
            for(j=0;j<UH_MAX_DAYS * global_param.model_steps_per_day;j++){
                fprintf(file,"%2f;",rout.cells[i].uh[j]);
                sum+=rout.cells[i].uh[j];
            }
            fprintf(file," sum = %2f\n",sum);
            sum=0.0;
        }
        fclose(file);
    }
}