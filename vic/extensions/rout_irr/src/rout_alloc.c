/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <rout.h>
#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>

void rout_alloc(void){    
    extern rout_struct rout;
    extern global_param_struct global_param;
    extern domain_struct global_domain;
    
    //Allocate memory for rout and rout.cells based on number of active cells    
    if((rout.cells = malloc(global_domain.ncells_active * sizeof(*rout.cells)))==NULL){
        log_err("Memory allocation for rout.cells failed!");
    }
    
    if((rout.sorted_cells = malloc(global_domain.ncells_active * sizeof(*rout.sorted_cells)))!=NULL){
        size_t i;
        for(i=0;i<global_domain.ncells_active;i++){
            rout.sorted_cells[i] = NULL;
        }
    }else{
        log_err("Memory allocation for rout.sorted_cells failed!");
    }
    
    if((rout.gridded_cells = malloc(global_domain.n_nx * sizeof(*rout.gridded_cells)))!=NULL){
        size_t x;
        for(x=0;x<global_domain.n_nx;x++){
            if((rout.gridded_cells[x] = malloc(global_domain.n_ny * sizeof(*rout.gridded_cells[x])))!=NULL){
                size_t y;
                for(y=0;y<global_domain.n_ny;y++){
                    rout.gridded_cells[x][y]= NULL;
                }                
            }else{
                log_err("Memory allocation for rout.gridded_cells[x] failed!");
            }
        }
    }else{
        log_err("Memory allocation for rout.gridded_cells failed!");
    }
    
    size_t i;
    for(i=0;i<global_domain.ncells_active;i++){
        
        rout.cells[i].reservoir=NULL;
        rout.cells[i].downstream=NULL;
                
        if((rout.cells[i].outflow = malloc(rout.max_days_uh * global_param.model_steps_per_day * sizeof(*rout.cells[i].outflow)))==NULL){
            log_err("Memory allocation for rout.cells[i].outflow failed!");
        }
        
        if((rout.cells[i].outflow_natural = malloc(rout.max_days_uh * global_param.model_steps_per_day * sizeof(*rout.cells[i].outflow_natural)))==NULL){
            log_err("Memory allocation for rout.cells[i].outflow failed!");
        }
        
        if((rout.cells[i].uh = malloc(rout.max_days_uh * global_param.model_steps_per_day * sizeof(*rout.cells[i].uh)))==NULL){
            log_err("Memory allocation for rout.cells[i].uh failed!");
        }
    }
}