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
    
    int netCDF_responce, nc_id, nc_varid;
    
    if((netCDF_responce = nc_open(rout.fileName, NC_NOWRITE, &nc_id))){
        log_err("Unable to open netCDF file");
    }
    
    int upstream[rout.x_size][rout.y_size][8];
    if((netCDF_responce = nc_inq_varid(nc_id, "upstream", &nc_varid))){
        log_err("Unable to get variable id of upstream of netCDF file");
    }
    if((netCDF_responce = nc_get_var_int(nc_id,nc_varid,&upstream[0][0][0]))){
        log_err("Unable to get upstream of netCDF file");
    }
    
    int uh[rout.x_size][rout.y_size][rout.uh_length];
    if((netCDF_responce = nc_inq_varid(nc_id, "uh", &nc_varid))){
        log_err("Unable to get variable id of uh of netCDF file");
    }
    if((netCDF_responce = nc_get_var_int(nc_id,nc_varid,&uh[0][0][0]))){
        log_err("Unable to get uh of netCDF file");
    }
           
    int x;
    int y;
    int t;
    int i;
    
    RankCells();
    
    for (y=0;y<rout.y_size;y++){
        for(x=0;x<rout.x_size;x++){
            
            for (i=0;i<rout.uh_length;i++){
                rout.cells[x][y]->uh[i]=uh[x][y][i];
            }
            
            for(i=0;i<rout.cells[x][y]->nr_upstream;i++){
                if(upstream[x][y][i]==1){
                    rout.cells[x][y]->upstream[i]=rout.cells[x][y-1];
                }
                if(upstream[x][y][i]==2){
                    rout.cells[x][y]->upstream[i]=rout.cells[x+1][y-1];
                }
                if(upstream[x][y][i]==3){
                    rout.cells[x][y]->upstream[i]=rout.cells[x+1][y];
                }
                if(upstream[x][y][i]==4){
                    rout.cells[x][y]->upstream[i]=rout.cells[x+1][y+1];
                }
                if(upstream[x][y][i]==5){
                    rout.cells[x][y]->upstream[i]=rout.cells[x][y+1];
                }
                if(upstream[x][y][i]==6){
                    rout.cells[x][y]->upstream[i]=rout.cells[x-1][y+1];
                }
                if(upstream[x][y][i]==7){
                    rout.cells[x][y]->upstream[i]=rout.cells[x-1][y];
                }
                if(upstream[x][y][i]==8){
                    rout.cells[x][y]->upstream[i]=rout.cells[x-1][y-1];
                }
            }
            
            for(t=0;t<rout.total_time;t++){
                rout.cells[x][y]->discharge[t]=0.0;
            }
        }
    }
        
    if((netCDF_responce = nc_close(nc_id))){
        log_err("Unable to close netCDF file");
    }
    
    free(upstream);
    free(uh);
}

void RankCells(void){
    extern rout_struct rout;
    //KLOPT NOG NIET   
    int x;
    int y;
    int i;
    int rank=1;
    for(i=0;i<8;i++){
        for(x=0; x<rout.x_size;x++){
            for(y=0;y<rout.y_size;y++){
                if(rout.cells[x][y]->nr_upstream==i){
                    rout.ranked_cells[rank]=rout.cells[x][y];
                    rank++;
                }
            }
        }
    }
}