/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <rout.h>
#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>
#include <netcdf.h>

//define routing variables
void rout_alloc(void){
    extern rout_struct rout;
    extern global_param_struct global_param;
    extern domain_struct local_domain;
    
    rout.x_size=get_nc_dimension("/home/bram/VIC5.0Routing/VICTestSetup/input/VIC_Params/RVIC_input_NL.nc","lon");
    rout.y_size=get_nc_dimension("/home/bram/VIC5.0Routing/VICTestSetup/input/VIC_Params/RVIC_input_NL.nc","lat");
    rout.total_time=global_param.nrecs; 
    rout.uh_length=96;
    
    //allocate memory based on data
    rout.cells = malloc(rout.y_size * sizeof(*rout.cells));
    if(rout.cells != NULL){
        size_t i;
        for (i=0;i<rout.y_size;i++){
            rout.cells[i] = malloc(rout.x_size * sizeof(**rout.cells));
            if(rout.cells[i]==NULL){
                log_err("Memory allocation for rout.cells[i] failed!");
            }
        }
    }else{
        log_err("Memory allocation for rout.cells failed!");
    }
    
    rout.ranked_cells = malloc(local_domain.ncells_active * sizeof(*rout.ranked_cells));
    if(rout.ranked_cells!=NULL){
        size_t i;
        for(i=0;i<local_domain.ncells_active;i++){
            rout.ranked_cells[i]=NULL;
        }
    }else{
        log_err("Memory allocation for rout.ranked_cells failed!");
    }
    
    size_t x;
    size_t y;
    for (y=0;y<rout.y_size;y++){
        for(x=0;x<rout.x_size;x++){
            rout.cells[y][x].nr_upstream=0;
        }
    }
    
    calculate_nr_upstream("/home/bram/VIC5.0Routing/VICTestSetup/input/VIC_Params/RVIC_input_NL.nc","flow_direction");
    
    //debug file
    make_nr_upstream_file("/home/bram/VIC5.0Routing/VICTestSetup/debug_output/nr_upstream.txt");
    
    for (y=0;y<rout.y_size;y++){
        for(x=0;x<rout.x_size;x++){            
            rout.cells[y][x].outflow = malloc(rout.total_time * sizeof(*rout.cells[y][x].outflow));
            if(rout.cells[y][x].outflow==NULL){
                log_err("Memory allocation for rout.cells[y][x].outflow failed!");
            }
            
            rout.cells[y][x].uh = malloc(rout.uh_length * sizeof(*rout.cells[y][x].uh));
            if(rout.cells[y][x].uh==NULL){
                log_err("Memory allocation for rout.cells[y][x].uh failed!");
            }
            
            if(rout.cells[y][x].nr_upstream>0){
                rout.cells[y][x].upstream = malloc(rout.cells[y][x].nr_upstream * sizeof(*rout.cells[y][x].upstream));
                if(rout.cells[y][x].upstream!=NULL){
                    int i;
                    for(i=0;i<rout.cells[y][x].nr_upstream;i++){
                        rout.cells[y][x].upstream[i]=NULL;
                    }
                }else{
                    log_err("Memory allocation for rout.cells[y][x].upstream failed!");
                }
            }
        }
    }
}

void calculate_nr_upstream(char file_path[], char variable_name[]){
    extern rout_struct rout;
    
    int direction[rout.y_size][rout.x_size];
    
    int nc_status, nc_id, nc_varid;
    if((nc_status = nc_open(file_path,NC_NOWRITE,&nc_id))){
        log_err("Unable to open nc file");
    }
    if((nc_status = nc_inq_varid(nc_id,variable_name,&nc_varid))){
        log_err("Unable to inquire variable id from nc file");
    }
    if((nc_status = nc_get_var_int(nc_id,nc_varid,&direction[0][0]))){
        log_err("Unable to get variable from nc file");
    }
    if((nc_status=nc_close(nc_id))){
        log_err("Unable to close nc file");
    }
    
    size_t x;
    size_t y;
    for(y=0; y<rout.y_size;y++){
        for(x=0; x<rout.x_size; x++){
            if(direction[y][x]==1){
                if(y+1<rout.y_size){
                    rout.cells[y+1][x].nr_upstream+=1;
                }
            }else if(direction[y][x]==2){
                if(y+1<rout.y_size && x+1<rout.x_size){
                    rout.cells[y+1][x+1].nr_upstream+=1;
                }
            }else if(direction[y][x]==3){
                if(x+1<rout.x_size){
                    rout.cells[y][x+1].nr_upstream+=1;
                }
            }else if(direction[y][x]==4){
                if(y>=1 && x+1<rout.x_size){
                    rout.cells[y-1][x+1].nr_upstream+=1;
                }
            }else if(direction[y][x]==5){
                if(y>=1){
                    rout.cells[y-1][x].nr_upstream+=1;
                }
            }else if(direction[y][x]==6){
                if(y>=1 && x>=1){
                    rout.cells[y-1][x-1].nr_upstream+=1;
                }
            }else if(direction[y][x]==7){
                if(x>=1){
                    rout.cells[y][x-1].nr_upstream+=1;
                }
            }else if(direction[y][x]==8){
                if(y+1<rout.y_size && x>=1){
                    rout.cells[y+1][x-1].nr_upstream+=1;
                }
            }else if(direction[y][x]==-1){
                rout.cells[y][x].nr_upstream= -1;
            }
        }
    }
}

void make_nr_upstream_file(char file_path[]){
    extern rout_struct rout;
    
    FILE *file;
    
    if((file = fopen(file_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=rout.y_size;y>0;y--){
            for(x=0;x<rout.x_size;x++){
                fprintf(file,"%d ",rout.cells[y-1][x].nr_upstream);
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}