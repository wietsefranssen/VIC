/******************************************************************************
 * @section DESCRIPTION
 *  
 * Allocate non-dynamic variables for the routing, irrigation and dam module.
 * These variables are the cells, routing cells, unit hydro-graphs, 
 * outflow and natural outflow
 ******************************************************************************/

#include <rout.h>
#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>

/******************************************************************************
 * @section brief
 *  
 * Allocate non-dynamic variables for the routing, irrigation and dam module.
 * These variables are the cells, routing cells, unit hydro-graphs, 
 * outflow and natural outflow
 ******************************************************************************/
void RID_alloc(void){
    extern RID_struct RID;
    extern global_param_struct global_param;
    extern domain_struct global_domain;
    extern int mpi_rank;
    
    size_t i;
    size_t x;
    size_t y;
    
    if (mpi_rank == VIC_MPI_ROOT) {
        /*******************************
         Allocate routing structures
        *******************************/
        RID.rout_cells = malloc(global_domain.ncells_active * sizeof(*RID.rout_cells));
        check_alloc_status(RID.rout_cells,"Memory allocation error.");
        for(i=0;i<global_domain.ncells_active;i++){
            RID.rout_cells[i].outflow=malloc(MAX_UH_DAYS * global_param.model_steps_per_day * sizeof(*RID.rout_cells[i].outflow));
            check_alloc_status(RID.rout_cells[i].outflow,"Memory allocation error.");
            RID.rout_cells[i].outflow_natural=malloc(MAX_UH_DAYS * global_param.model_steps_per_day * sizeof(*RID.rout_cells[i].outflow_natural));
            check_alloc_status(RID.rout_cells[i].outflow_natural,"Memory allocation error.");
            RID.rout_cells[i].uh=malloc(MAX_UH_DAYS * global_param.model_steps_per_day * sizeof(*RID.rout_cells[i].uh));
            check_alloc_status(RID.rout_cells[i].uh,"Memory allocation error.");
        }
                
        //RID.rout_run_bas = malloc(global_domain.ncells_active * sizeof(*RID.rout_run_bas));
        //check_alloc_status(RID.rout_run_bas,"Memory allocation error.");
                
        /*******************************
         Allocate RID cells, sorted cells and gridded cells
        *******************************/
        RID.cells = malloc(global_domain.ncells_active * sizeof(*RID.cells));
        check_alloc_status(RID.cells,"Memory allocation error.");

        RID.sorted_cells = malloc(global_domain.ncells_active * sizeof(*RID.sorted_cells));
        check_alloc_status(RID.sorted_cells,"Memory allocation error.");
        for(i=0;i<global_domain.ncells_active;i++){
            RID.sorted_cells[i] = NULL;
        }

        RID.gridded_cells = malloc(global_domain.n_nx * sizeof(*RID.gridded_cells));
        check_alloc_status(RID.gridded_cells,"Memory allocation error.");
        for(x=0;x<global_domain.n_nx;x++){
            RID.gridded_cells[x] = malloc(global_domain.n_ny * sizeof(*RID.gridded_cells[x]));
            check_alloc_status(RID.gridded_cells[x],"Memory allocation error.");
            for(y=0;y<global_domain.n_ny;y++){
                RID.gridded_cells[x][y]= NULL;
            }                
        }
    }
}