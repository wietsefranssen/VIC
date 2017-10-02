#include <ext_driver_shared_image.h>

void ext_finalize()
{
    extern ext_option_struct ext_options;
    extern domain_struct local_domain;    
    extern int mpi_decomposition;
    extern int mpi_rank;    
    extern size_t *cell_order_global; 
    extern basin_struct basins;    
    extern size_t *cell_order_local; 
    extern ext_all_vars_struct *ext_all_vars;
    extern rout_con_struct *rout_con;
    extern wu_hist_struct **wu_hist;    
    extern wu_con_struct **wu_con;    
    extern ext_filenames_struct ext_filenames;
    
    size_t i;
    size_t j;
    int status;
    
    if(ext_options.ROUTING){
        for(i=0;i<local_domain.ncells_active;i++){
            free(rout_con[i].upstream);
            free(rout_con[i].uh);

            free(ext_all_vars[i].rout_var.discharge);
        }    
        free(rout_con); 
        free(cell_order_local);
        
        if(mpi_rank == VIC_MPI_ROOT){        
            free(cell_order_global);
    
            if(mpi_decomposition == BASIN_DECOMPOSITION){
                for(i=0;i<basins.Nbasin;i++){
                    free(basins.catchment[i]);
                }
                free(basins.basin_map);
                free(basins.Ncells);
                free(basins.sorted_basins);
                free(basins.catchment);
            }  
        }
    }
    
    if(ext_options.WATER_USE){        
        if (mpi_rank == VIC_MPI_ROOT) {
            status = nc_close(ext_filenames.water_use.nc_id);
            check_nc_status(status, "Error closing %s",
                            ext_filenames.water_use.nc_filename);
        }
        
        for(i=0;i<local_domain.ncells_active;i++){
            for(j=0;j<WU_NSECTORS;j++){
                free(wu_hist[i][j].demand);
                free(wu_hist[i][j].consumption_factor);
                free(ext_all_vars[i].wu_var[j].return_flow);
            }            
            free(wu_con[i]);
            free(wu_hist[i]);
            free(ext_all_vars[i].wu_var);
        }
        free(wu_con);
        free(wu_hist);        
    }
    
    free(ext_all_vars);
}
