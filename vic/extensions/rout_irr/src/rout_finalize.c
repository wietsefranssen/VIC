/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "vic_driver_shared_image.h"
#include "rout.h"


void rout_finalize(void){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern rout_options_struct rout_options;
    
    if(!rout_options.routing){
        return;
    }
    
    //free rout_cells
    size_t i;
    for(i=0;i<global_domain.ncells_active;i++){
        free(rout.cells[i].uh);
        free(rout.cells[i].outflow);
        free(rout.cells[i].upstream);
        free(rout.cells[i].servicing_reservoirs);
    }
    
    //free reservoir_unit
    size_t j;
    for(i=0;i<rout.nr_reservoirs;i++){
        free(rout.reservoirs[i].serviced_cells);
        free(rout.reservoirs[i].cell_demand);
        for(j=0;j<RES_CALC_YEARS_MEAN;j++){
            free(rout.reservoirs[i].inflow[j]);
            free(rout.reservoirs[i].demand[j]);
        }
        free(rout.reservoirs[i].inflow);
        free(rout.reservoirs[i].demand);
    }
    
    //free rout_struct
    for(i=0;i<global_domain.n_nx;i++){
        free(rout.gridded_cells[i]);
    }
    free(rout.gridded_cells);        
    free(rout.sorted_cells);
    free(rout.cells);
    free(rout.reservoirs);
}