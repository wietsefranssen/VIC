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
    
    size_t i;
    size_t x;
    size_t y;
    
    //Allocate memory for rout and rout.cells based on number of active cells    
    rout.cells = malloc(global_domain.ncells_active * sizeof(*rout.cells));
    check_alloc_status(rout.cells,"Memory allocation error.");
    
    rout.sorted_cells = malloc(global_domain.ncells_active * sizeof(*rout.sorted_cells));
    check_alloc_status(rout.sorted_cells,"Memory allocation error.");
    
    for(i=0;i<global_domain.ncells_active;i++){
        rout.sorted_cells[i] = NULL;
    }
    
    rout.gridded_cells = malloc(global_domain.n_nx * sizeof(*rout.gridded_cells));
    check_alloc_status(rout.gridded_cells,"Memory allocation error.");
    
    for(x=0;x<global_domain.n_nx;x++){
        rout.gridded_cells[x] = malloc(global_domain.n_ny * sizeof(*rout.gridded_cells[x]));
        check_alloc_status(rout.gridded_cells[x],"Memory allocation error.");
        
        for(y=0;y<global_domain.n_ny;y++){
            rout.gridded_cells[x][y]= NULL;
        }                
    }
    
    for(i=0;i<global_domain.ncells_active;i++){
        
        rout.cells[i].reservoir=NULL;
        rout.cells[i].downstream=NULL;
                
        rout.cells[i].outflow = malloc(rout.max_days_uh * global_param.model_steps_per_day * sizeof(*rout.cells[i].outflow));
        check_alloc_status(rout.cells[i].outflow,"Memory allocation error.");
        
        rout.cells[i].outflow_natural = malloc(rout.max_days_uh * global_param.model_steps_per_day * sizeof(*rout.cells[i].outflow_natural));
        check_alloc_status(rout.cells[i].outflow_natural,"Memory allocation error.");
        
        rout.cells[i].uh = malloc(rout.max_days_uh * global_param.model_steps_per_day * sizeof(*rout.cells[i].uh));
        check_alloc_status(rout.cells[i].uh,"Memory allocation error.");
    }
}