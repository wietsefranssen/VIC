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

void rout_init(void){
    extern rout_struct rout;
    
    size_t x;
    size_t y;
    size_t t;
    
    for (y=0;y<rout.y_size;y++){
        for(x=0;x<rout.x_size;x++){
            rout.cells[y][x].x=x;
            rout.cells[y][x].y=y;
            
            if(rout.cells[y][x].nr_upstream==-1){
                rout.cells[y][x].active=0;
            }else{
                rout.cells[y][x].active=1;
            }
            
            for(t=0;t<rout.total_time;t++){
                rout.cells[y][x].outflow[t]=0.0;
            }
            
            /*for (ui = 0; ui < local_domain.ncells_active; ui++) {
                if (lat[x][y] == local_domain.locations[ui].latitude &&
                    lon[x][y] == local_domain.locations[ui].longitude) {
                    rout.cells[x][y].vic_id = ui;
                }
            }*/
        }
    }
        
    set_upstream("/home/bram/VIC5.0Routing/VICTestSetup/input/VIC_Params/RVIC_input_NL.nc","flow_direction");
    
    //debug
    make_upstream_file("/home/bram/VIC5.0Routing/VICTestSetup/debug_output/upstream.txt");
    
    rank_cells();
}

void set_upstream(char file_path[], char variable_name[]){
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
            int i;
            if(direction[y][x]==1){
                if(y+1<rout.y_size){
                    for(i=0;i<rout.cells[y+1][x].nr_upstream;i++){
                        if(rout.cells[y+1][x].upstream[i]==NULL){
                            rout.cells[y+1][x].upstream[i]=&rout.cells[y][x];
                            break;
                        }
                    }
                }
            }else if(direction[y][x]==2){
                if(y+1<rout.y_size && x+1<rout.x_size){
                    for(i=0;i<rout.cells[y+1][x+1].nr_upstream;i++){
                        if(rout.cells[y+1][x+1].upstream[i]==NULL){
                            rout.cells[y+1][x+1].upstream[i]=&rout.cells[y][x];
                            break;
                        }
                    }
                }
            }else if(direction[y][x]==3){
                if(x+1<rout.x_size){
                    for(i=0;i<rout.cells[y][x+1].nr_upstream;i++){
                        if(rout.cells[y][x+1].upstream[i]==NULL){
                            rout.cells[y][x+1].upstream[i]=&rout.cells[y][x];
                            break;
                        }
                    }
                }
            }else if(direction[y][x]==4){
                if(y>=1 && x+1<rout.x_size){
                    for(i=0;i<rout.cells[y-1][x+1].nr_upstream;i++){
                        if(rout.cells[y-1][x+1].upstream[i]==NULL){
                            rout.cells[y-1][x+1].upstream[i]=&rout.cells[y][x];
                            break;
                        }
                    }
                }
            }else if(direction[y][x]==5){
                if(y>=1){
                    for(i=0;i<rout.cells[y-1][x].nr_upstream;i++){
                        if(rout.cells[y-1][x].upstream[i]==NULL){
                            rout.cells[y-1][x].upstream[i]=&rout.cells[y][x];
                            break;
                        }
                    }
                }
            }else if(direction[y][x]==6){
                if(y>=1 && x>=1){
                    for(i=0;i<rout.cells[y-1][x-1].nr_upstream;i++){
                        if(rout.cells[y-1][x-1].upstream[i]==NULL){
                            rout.cells[y-1][x-1].upstream[i]=&rout.cells[y][x];
                            break;
                        }
                    }
                }
            }else if(direction[y][x]==7){
                if(x>=1){
                    for(i=0;i<rout.cells[y][x-1].nr_upstream;i++){
                        if(rout.cells[y][x-1].upstream[i]==NULL){
                            rout.cells[y][x-1].upstream[i]=&rout.cells[y][x];
                            break;
                        }
                    }
                }
            }else if(direction[y][x]==8){
                if(y+1<rout.y_size && x>=1){
                    for(i=0;i<rout.cells[y+1][x-1].nr_upstream;i++){
                        if(rout.cells[y+1][x-1].upstream[i]==NULL){
                            rout.cells[y+1][x-1].upstream[i]=&rout.cells[y][x];
                            break;
                        }
                    }
                }
            }
        }
    }
}

void rank_cells(void){
    extern rout_struct rout;
    extern domain_struct local_domain;
    
    int ranked_map[rout.y_size][rout.x_size]; // used to see which cells are already ranked
    
    size_t x;
    size_t y;
    int i;
    size_t ui;
    size_t rank=0;
    
    for(y=0; y<rout.y_size;y++){
        for(x=0;x<rout.x_size;x++){
            ranked_map[y][x]=0;
        }
    }
    
    while(1){
        for(y=0; y<rout.y_size;y++){
            for(x=0;x<rout.x_size;x++){
                if(rout.cells[y][x].active==1 && ranked_map[y][x]==0){
                    int count=0;
                    for(i=0;i<rout.cells[y][x].nr_upstream;i++){
                        if(ranked_map[rout.cells[y][x].upstream[i]->y][rout.cells[y][x].upstream[i]->x]==0){
                            count++;
                        }
                    }

                    if(count==0){
                        rout.ranked_cells[rank]=&rout.cells[y][x];
                        rank++;
                    }        
                }
            }
        }
        
        for(ui=0;ui<rank;ui++){
            ranked_map[rout.ranked_cells[ui]->y][rout.ranked_cells[ui]->x]=1;
        }
        
        if(rank == local_domain.ncells_active-1){
            log_info("Finished ranking cells");
            break;
        }else if(rank > local_domain.ncells_active-1){
            log_warn("rank_cells made %zu loops and escaped because this is more than %zu, the number of active cells",(rank+1),local_domain.ncells_active);
            break;
        }
    }
}

void make_upstream_file(char file_path[]){
    extern rout_struct rout;
    
    FILE *file;
    
    if((file = fopen(file_path, "w"))!=NULL){
        size_t x;
        size_t y;
        int i;
        for(y=rout.y_size;y>0;y--){
            for(x=0;x<rout.x_size;x++){
                for(i=0;i<rout.cells[y-1][x].nr_upstream;i++){
                    fprintf(file,"(%zu,%zu)",rout.cells[y-1][x].upstream[i]->y,rout.cells[y-1][x].upstream[i]->x);
                }
                fprintf(file,"; ");
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}