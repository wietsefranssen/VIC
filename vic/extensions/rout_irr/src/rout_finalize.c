#include "vic_driver_shared_image.h"
#include "rout.h"


void rout_finalize(void){
    extern module_struct rout;
    extern domain_struct global_domain;
    size_t i;
    size_t j;
    
    if(rout.param.fdams){
        for(i=0;i<rout.nr_dams;i++){
            for(j=0;j<rout.dams[i].nr_serviced_cells;j++){
                free(rout.dams[i].cell_demand[j]);
                free(rout.dams[i].prev_deficit[j]);
            }
            free(rout.dams[i].cell_demand);
            free(rout.dams[i].prev_deficit);
            
            for(j=0;j<DAM_CALC_YEARS_MEAN;j++){
                free(rout.dams[i].demand[j]);
                free(rout.dams[i].inflow[j]);
                free(rout.dams[i].inflow_natural[j]);
            }
            free(rout.dams[i].demand);
            free(rout.dams[i].inflow);
            free(rout.dams[i].inflow_natural);
            
            if(rout.param.firrigation){
                free(rout.dams[i].serviced_cells);
            }
        }
        free(rout.dams);
    }
    
    if(rout.param.firrigation){
        for(i=0;i<rout.nr_irr_cells;i++){
            free(rout.irr_cells[i].crop_index);
            free(rout.irr_cells[i].veg_index);
            free(rout.irr_cells[i].veg_class);
            free(rout.irr_cells[i].max_cv);
            
            if(rout.param.fdams){
                free(rout.irr_cells[i].servicing_dams);
            }
        }
        
        free(rout.irr_cells);
    }
    
    for(i=0;i<global_domain.ncells_active;i++){
        free(rout.rout_cells[i].outflow);
        free(rout.rout_cells[i].outflow_natural);
        free(rout.rout_cells[i].uh);
        
        free(rout.rout_cells[i].upstream);
    }
    free(rout.rout_cells);
    
    for(i=0;i<global_domain.n_nx;i++){
        free(rout.gridded_cells[i]);
    }
    free(rout.gridded_cells);
    free(rout.sorted_cells);
    free(rout.cells);
    
    free(rout.param.crop_class);
    free(rout.param.crop_sow);
    free(rout.param.crop_developed);
    free(rout.param.crop_matured);
    free(rout.param.crop_harvest);
}
