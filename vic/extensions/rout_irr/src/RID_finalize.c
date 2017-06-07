/******************************************************************************
 * @section DESCRIPTION
 *  
 * Free allocated variables for the routing, irrigation and dam module.
 ******************************************************************************/

#include "vic_driver_shared_image.h"
#include "rout.h"

/******************************************************************************
 * @section brief
 *  
 * Free allocated variables for the routing, irrigation and dam module.
 ******************************************************************************/

void RID_finalize(void){
    extern RID_struct RID;
    extern domain_struct global_domain;
    size_t i;
    extern int mpi_rank;
    extern global_param_struct global_param;
    
    if (mpi_rank == VIC_MPI_ROOT) {   
        /*******************************
         Dam module
        *******************************/
        if(global_param.fdams){
            for(i=0;i<RID.nr_dams;i++){            
                free(RID.dams[i].history_demand);
                free(RID.dams[i].history_inflow);
                free(RID.dams[i].history_inflow_natural);
            }
        }

        /*******************************
         Irrigation module
        *******************************/
        if(global_param.firrigation){
            for(i=0;i<RID.nr_irr_cells;i++){
                free(RID.irr_cells[i].crop_index);
                free(RID.irr_cells[i].veg_index);
                free(RID.irr_cells[i].veg_class);

                free(RID.irr_cells[i].demand);
                free(RID.irr_cells[i].moisture);
                free(RID.irr_cells[i].deficit);
                free(RID.irr_cells[i].storage);
                free(RID.irr_cells[i].normal_Ksat);
            }
        }

        /*******************************
         Dam irrigation interaction
        *******************************/
        if(RID.nr_dams>0 && RID.nr_irr_cells>0){        
            for(i=0;i<RID.nr_dams;i++){    
                if(RID.dams[i].function!=DAM_IRR_FUNCTION){
                    continue;
                }

                free(RID.dams[i].serviced_cells);        
            }
        }

        /*******************************
         Routing module
        *******************************/
        for(i=0;i<global_domain.ncells_active;i++){
            free(RID.rout_cells[i].outflow);
            free(RID.rout_cells[i].outflow_natural);
            free(RID.rout_cells[i].uh);

            free(RID.rout_cells[i].upstream);
        }

        /*******************************
         RID module
        *******************************/
        for(i=0;i<global_domain.n_nx;i++){
            free(RID.gridded_cells[i]);
        }
        free(RID.gridded_cells);
        free(RID.sorted_cells);
        free(RID.cells);

        free(RID.rout_cells);
        free(RID.irr_cells);
        free(RID.dams);
    }
}
