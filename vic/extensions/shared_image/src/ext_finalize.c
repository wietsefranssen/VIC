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
    extern dam_con_struct **dam_con;
    extern dam_con_map_struct *dam_con_map;
    
    size_t i;
    size_t j;
    
    if(ext_options.ROUTING){
        for(i=0;i<local_domain.ncells_active;i++){
            free(rout_con[i].upstream);
            free(rout_con[i].uh);

            free(ext_all_vars[i].rout_var.discharge);
            free(ext_all_vars[i].rout_var.nat_discharge);
        }    
        
        free(rout_con);        
        free(cell_order_local);
        
        if(mpi_rank == VIC_MPI_ROOT){        
            free(cell_order_global);
        }
    }
    
    if(ext_options.DAMS){
        for(i=0;i<local_domain.ncells_active;i++){            
            for(j=0;j<dam_con_map[i].Ndams;j++){
                free(ext_all_vars[i].dam_var[j].inflow_history);
                free(ext_all_vars[i].dam_var[j].calc_discharge);
                free(ext_all_vars[i].dam_var[j].nat_inflow_history);
            }
        }
            
        for(i=0;i<local_domain.ncells_active;i++){ 
            free(dam_con[i]);
            free(ext_all_vars[i].dam_var);
        }
        
        free(dam_con);
        free(dam_con_map);
    }
        
    free(ext_all_vars);
}
