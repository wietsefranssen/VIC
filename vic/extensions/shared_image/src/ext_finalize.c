#include <ext_driver_shared_image.h>

void ext_finalize()
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    
    extern int mpi_rank;
    extern int mpi_decomposition;
    
    extern size_t *mpi_map_mapping_array_reverse;
    extern basin_struct basins;
    
    extern ext_all_vars_struct *ext_all_vars;
    extern size_t *cell_order; 
    extern rout_con_struct *rout_con;
    
    size_t i;
    
    if(mpi_decomposition == BASIN_DECOMPOSITION){
        for(i=0;i<local_domain.ncells_active;i++){
            free(rout_con[i].upstream);
            free(rout_con[i].uh);
            free(ext_all_vars[i].rout_var.discharge);
        }
        free(rout_con);
    } else if (mpi_decomposition == RANDOM_DECOMPOSITION &&
            mpi_rank == VIC_MPI_ROOT){
        for(i=0;i<global_domain.ncells_active;i++){
            free(rout_con[i].upstream);
            free(rout_con[i].uh);
            free(ext_all_vars[i].rout_var.discharge);
        }
        free(rout_con);
    }
    
    if(basins.Nbasin > 0){
        for(i=0;i<basins.Nbasin;i++){
            free(basins.catchment);
        }
        free(basins.basin_map);
        free(basins.Ncells);
        free(basins.sorted_basins);
        free(basins.catchment);
    }    
    
    if(mpi_rank == VIC_MPI_ROOT){
        free(mpi_map_mapping_array_reverse);
    }
    
    free(cell_order);
}
