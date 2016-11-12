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
    //Allocate memory for rout, rout.cells, rout.sorted_cells, rout.gridded_cells and rout.rout_cells
    //All of these depend on the number of active cells, which is known
    extern module_struct rout;
    extern global_param_struct global_param;
    extern domain_struct global_domain;
    
    size_t i;
    size_t x;
    size_t y;
    
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
    
    rout.rout_cells = malloc(global_domain.ncells_active * sizeof(*rout.rout_cells));
    check_alloc_status(rout.rout_cells,"Memory allocation error.");
    
    for(i=0;i<global_domain.ncells_active;i++){
        //Each rout_cell is connected to a module_cell since all cells have routing
        rout.cells[i].rout=&rout.rout_cells[i];
        rout.rout_cells[i].cell=&rout.cells[i];
        
        //Allocate memory for the outflow, natural outflow and unit hydrograph
        rout.cells[i].rout->outflow=malloc(rout.param.max_days_uh * global_param.model_steps_per_day * sizeof(*rout.cells[i].rout->outflow));
        check_alloc_status(rout.cells[i].rout->outflow,"Memory allocation error.");
        rout.cells[i].rout->outflow_natural=malloc(rout.param.max_days_uh * global_param.model_steps_per_day * sizeof(*rout.cells[i].rout->outflow_natural));
        check_alloc_status(rout.cells[i].rout->outflow_natural,"Memory allocation error.");
        rout.cells[i].rout->uh=malloc(rout.param.max_days_uh * global_param.model_steps_per_day * sizeof(*rout.cells[i].rout->uh));
        check_alloc_status(rout.cells[i].rout->uh,"Memory allocation error.");
        
        rout.cells[i].dam=NULL;
        rout.cells[i].irr=NULL;
    }
    
    rout.nr_irr_cells=0;
    rout.nr_dams=0;
    
}