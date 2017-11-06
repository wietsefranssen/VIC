#include <ext_driver_shared_image.h>

void ext_finalize()
{
    extern ext_option_struct ext_options;
    extern domain_struct local_domain;    
    extern int mpi_rank;    
    extern size_t *cell_order; 
    extern basin_struct basins;
    extern ext_all_vars_struct *ext_all_vars;
    extern rout_con_struct *rout_con;
    
    size_t i;
    
    if(ext_options.ROUTING){
        for(i=0;i<local_domain.ncells_active;i++){
            free(rout_con[i].upstream);
            free(rout_con[i].uh);

            free(ext_all_vars[i].rout_var.discharge);
        }    
        free(rout_con); 
        free(cell_order);
        
        if(mpi_rank == VIC_MPI_ROOT){
            for(i=0;i<basins.Nbasin;i++){
                free(basins.catchment[i]);
            }
            free(basins.basin_map);
            free(basins.Ncells);
            free(basins.sorted_basins);
            free(basins.catchment);
        }
    }
        
    free(ext_all_vars);
}
