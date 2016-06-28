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
    
    int netCDF_responce, nc_id, nc_varid;
    int nc_dimid[3];
    //char nc_dimname[NC_MAX_NAME+1];
    size_t nc_length;
    
    //get field data from .nc file
    if((netCDF_responce = nc_open(rout.fileName, NC_NOWRITE, &nc_id))){
        log_err("Unable to open netCDF file");
    }
    
    if((netCDF_responce = nc_inq_varid(nc_id, "uh", &nc_varid))){
        log_err("Unable to get variable id of uh of netCDF file");
    }
    if((netCDF_responce = nc_inq_var(nc_id, nc_varid, NULL, NULL, NULL, nc_dimid, NULL))){
        log_err("Unable to get dimension id of uh of netCDF file");
    }
    if((netCDF_responce = nc_inq_dim(nc_id, nc_dimid[0], NULL, &nc_length))){
        log_err("Unable to get x dimension length of uh of netCDF file");
    }else{
        rout.x_size=(int)nc_length;
    }
    if((netCDF_responce = nc_inq_dim(nc_id, nc_dimid[1], NULL, &nc_length))){
        log_err("Unable to get y dimension length of uh ofnetCDF file");
    }else{
        rout.y_size=(int)nc_length;
    }
    if((netCDF_responce = nc_inq_dim(nc_id, nc_dimid[2], NULL, &nc_length))){
        log_err("Unable to get z dimension length of uh of netCDF file");
    }else{
        rout.uh_length=(int)nc_length;
    }
    
    if((netCDF_responce = nc_inq_varid(nc_id, "total_time", &nc_varid))){
        log_err("Unable to get variable id of total_time of netCDF file");
    }
    if((netCDF_responce = nc_get_var_int(nc_id, nc_varid, &rout.total_time))){
        log_err("Unable to get total_time of netCDF file");
    }
        
    
    //allocate memory based on data
    rout.cells = malloc(rout.y_size * sizeof(*rout.cells));
    if(rout.cells != NULL){
        int i;
        for (i=0;i<rout.y_size;i++){
            rout.cells[i] = malloc(rout.x_size * sizeof(**rout.cells));
            if(rout.cells[i]!=NULL){
                int j;
                for(j=0;j<rout.x_size;j++){
                    rout.cells[i][j] = malloc(sizeof(***rout.cells));
                    if(rout.cells[i][j]==NULL){
                        log_err("Memory allocation for rout.cells[i][j] failed!")
                    }
                }
            }else{
                log_err("Memory allocation for rout.cells[i] failed!");
            }
        }
    }else{
        log_err("Memory allocation for rout.cells failed!");
    }
    
    rout.ranked_cells = malloc(rout.y_size * rout.x_size * sizeof(*rout.ranked_cells));
    if(rout.ranked_cells==NULL){
        log_err("Memory allocation for rout.ranked_cells failed!");
    }
    
    int nr_upstream[rout.x_size][rout.y_size];
    if((netCDF_responce = nc_inq_varid(nc_id, "nr_upstream", &nc_varid))){
        log_err("Unable to get variable id of nr_upstream of netCDF file");
    }
    if((netCDF_responce = nc_get_var_int(nc_id,nc_varid,&nr_upstream[0][0]))){
        log_err("Unable to get nr_upstream of netCDF file");
    }
    
    int x;
    int y;
    for (y=0;y<rout.y_size;y++){
        for(x=0;x<rout.x_size;x++){            
            rout.cells[x][y]->discharge = malloc(rout.total_time * sizeof(*rout.cells[x][y]->discharge));
            if(rout.cells[x][y]->discharge==NULL){
                log_err("Memory allocation for rout.cells->discharge failed!");
            }

            rout.cells[x][y]->uh = malloc(rout.uh_length * sizeof(*rout.cells[x][y]->uh));
            if(rout.cells[x][y]->uh==NULL){
                log_err("Memory allocation for rout.cells->uh failed!");
            }
            
            rout.cells[x][y]->nr_upstream = nr_upstream[x][y];
            rout.cells[x][y]->upstream = malloc(rout.cells[x][y]->nr_upstream * sizeof(*rout.cells[x][y]->upstream));
            if(rout.cells[x][y]->upstream==NULL){
                log_err("Memory allocation for rout.cells[x][y].upstream failed!");
            }
        }
    }
    
    if((netCDF_responce = nc_close(nc_id))){
        log_err("Unable to close netCDF file");
    }
}