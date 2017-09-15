#include <ext_driver_shared_image.h>

void
ext_alloc(void)
{    
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    extern rout_con_struct *rout_con;
    extern ext_all_vars_struct *ext_all_vars;
    extern size_t *mpi_map_mapping_array_reverse;
    extern size_t *cell_order_local;
       
    size_t i;
    
    mpi_map_mapping_array_reverse = malloc(global_domain.ncells_active * sizeof (*mpi_map_mapping_array_reverse));
    check_alloc_status(mpi_map_mapping_array_reverse,"Memory allocation error");
    
    ext_all_vars = malloc(local_domain.ncells_active * sizeof(*ext_all_vars));
    check_alloc_status(ext_all_vars, "Memory allocation error");        
        
    rout_con = malloc(local_domain.ncells_active * sizeof(*rout_con));
    check_alloc_status(rout_con, "Memory allocation error");

    for(i=0;i<local_domain.ncells_active;i++){
        rout_con[i].uh = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*rout_con[i].uh));
        check_alloc_status(rout_con[i].uh, "Memory allocation error");
        ext_all_vars[i].rout_var.discharge = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*ext_all_vars[i].rout_var.discharge));
        check_alloc_status(ext_all_vars[i].rout_var.discharge, "Memory allocation error");
    }

    cell_order_local = malloc(local_domain.ncells_active * sizeof(*cell_order_local));
    check_alloc_status(cell_order_local, "Memory allocation error");
        
    initialize_ext_local_structures();
}