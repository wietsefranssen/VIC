#include <ext_driver_shared_image.h>

void
ext_alloc(void)
{    
    extern domain_struct local_domain;
    extern ext_option_struct ext_options;
    extern rout_con_struct *rout_con;
    extern dam_con_struct **dam_con;
    extern dam_con_map_struct *dam_con_map;
    extern ext_all_vars_struct *ext_all_vars;
    extern size_t *cell_order_local;
    
    size_t i;
    size_t j;
    
    ext_all_vars = malloc(local_domain.ncells_active * sizeof(*ext_all_vars));
    check_alloc_status(ext_all_vars, "Memory allocation error");   
    
    if(ext_options.ROUTING){
        cell_order_local = malloc(local_domain.ncells_active * sizeof(*cell_order_local));
        check_alloc_status(cell_order_local, "Memory allocation error");     
        
        rout_con = malloc(local_domain.ncells_active * sizeof(*rout_con));
        check_alloc_status(rout_con, "Memory allocation error");

        for(i=0;i<local_domain.ncells_active;i++){
            rout_con[i].uh = malloc(ext_options.uh_steps * sizeof(*rout_con[i].uh));
            check_alloc_status(rout_con[i].uh, "Memory allocation error");
            
            ext_all_vars[i].rout_var.discharge = malloc(ext_options.uh_steps * sizeof(*ext_all_vars[i].rout_var.discharge));
            check_alloc_status(ext_all_vars[i].rout_var.discharge, "Memory allocation error");
            
            ext_all_vars[i].efr_var.discharge = malloc(ext_options.uh_steps * sizeof(*ext_all_vars[i].efr_var.discharge));
            check_alloc_status(ext_all_vars[i].efr_var.discharge, "Memory allocation error");
        }
    }
    if(ext_options.DAMS){        
        dam_con_map = malloc(local_domain.ncells_active * sizeof(*dam_con_map));
        check_alloc_status(dam_con_map, "Memory allocation error");
        
        dam_con = malloc(local_domain.ncells_active * sizeof(*dam_con));
        check_alloc_status(dam_con, "Memory allocation error");     
        
        initialize_dam_con_map(dam_con_map);
        debug_ndams();
        
        for(i=0;i<local_domain.ncells_active;i++){
            dam_con[i] = malloc(dam_con_map[i].Ndams * sizeof(*dam_con[i]));
            check_alloc_status(dam_con[i], "Memory allocation error");    
            
            ext_all_vars[i].dam_var = malloc(dam_con_map[i].Ndams * sizeof(*ext_all_vars[i].dam_var));
            check_alloc_status(ext_all_vars[i].dam_var, "Memory allocation error");
            
            for(j=0;j<dam_con_map[i].Ndams;j++){
                ext_all_vars[i].dam_var[j].inflow_history = malloc(ext_options.history_steps * sizeof(*ext_all_vars[i].dam_var[j].inflow_history));
                check_alloc_status(ext_all_vars[i].dam_var[j].inflow_history, "Memory allocation error");
                
                ext_all_vars[i].dam_var[j].nat_inflow_history = malloc(ext_options.history_steps * sizeof(*ext_all_vars[i].dam_var[j].nat_inflow_history));
                check_alloc_status(ext_all_vars[i].dam_var[j].nat_inflow_history, "Memory allocation error");
            }
        }
    }
        
    initialize_ext_local_structures();
}

void
initialize_dam_con_map(dam_con_map_struct *dam_con_map){    
    extern domain_struct local_domain;
    extern ext_filenames_struct ext_filenames;
    extern domain_struct global_domain;
    
    int *ivar = NULL;
    size_t d2start[2];
    size_t d2count[2];
    
    size_t i;
       
    // allocate memory for variables to be stored
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error");

    // initialize starts and counts
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    get_scatter_nc_field_int(&ext_filenames.dams,
                                ext_filenames.info.ndam_var,
                                d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        if(ivar[i] == NODATA_DAM){
            ivar[i]=0;
        }
        dam_con_map[i].Ndams = ivar[i];
    }
        
    free(ivar);
}