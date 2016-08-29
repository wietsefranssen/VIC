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
    
    size_t i;
    for(i=0;i<global_domain.ncells_active;i++){
        free(rout.cells[i].uh);
        free(rout.cells[i].outflow);
        free(rout.cells[i].upstream);
    }
       
    size_t x;
    for(x=0;x<global_domain.n_nx;x++){
        free(rout.gridded_cells[x]);
    }
    free(rout.gridded_cells);
        
    free(rout.sorted_cells);
    free(rout.cells);
}