#include <ext_driver_shared_image.h>

void
ext_alloc(void)
{    
    extern domain_struct local_domain;
    extern ext_option_struct ext_options;
    extern rout_con_struct *rout_con;
    extern ext_all_vars_struct *ext_all_vars;
    extern size_t *cell_order;
    extern int mpi_decomposition;
    
    size_t i;
        
    ext_all_vars = malloc(local_domain.ncells_active * sizeof(*ext_all_vars));
    check_alloc_status(ext_all_vars, "Memory allocation error");   
    
    if(ext_options.ROUTING){  
        
        rout_con = malloc(local_domain.ncells_active * sizeof(*rout_con));
        check_alloc_status(rout_con, "Memory allocation error");

        for(i=0;i<local_domain.ncells_active;i++){
            rout_con[i].uh = malloc(ext_options.uh_steps * sizeof(*rout_con[i].uh));
            check_alloc_status(rout_con[i].uh, "Memory allocation error");
            ext_all_vars[i].rout_var.discharge = malloc(ext_options.uh_steps * sizeof(*ext_all_vars[i].rout_var.discharge));
            check_alloc_status(ext_all_vars[i].rout_var.discharge, "Memory allocation error");
        }
        
        if(mpi_decomposition == BASIN_DECOMPOSITION){
            cell_order = malloc(local_domain.ncells_active * sizeof(*cell_order));
            check_alloc_status(cell_order, "Memory allocation error");
                
            for(i=0;i<local_domain.ncells_active;i++){
                cell_order[i] = MISSING_USI;
            }
        }
    }
        
    initialize_ext_local_structures();
}