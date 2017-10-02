#include <ext_driver_shared_image.h>

void
ext_alloc(void)
{    
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    extern ext_option_struct ext_options;
    extern rout_con_struct *rout_con;
    extern wu_con_struct **wu_con;
    extern wu_hist_struct **wu_hist;
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
            rout_con[i].uh = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*rout_con[i].uh));
            check_alloc_status(rout_con[i].uh, "Memory allocation error");
            ext_all_vars[i].rout_var.discharge = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*ext_all_vars[i].rout_var.discharge));
            check_alloc_status(ext_all_vars[i].rout_var.discharge, "Memory allocation error");
        }
    }
    if(ext_options.WATER_USE){
        wu_con = malloc(local_domain.ncells_active * sizeof(*wu_con));
        check_alloc_status(wu_con, "Memory allocation error");
        
        wu_hist = malloc(local_domain.ncells_active * sizeof(*wu_hist));
        check_alloc_status(wu_hist, "Memory allocation error");
        
        for(i=0;i<local_domain.ncells_active;i++){
            wu_con[i] = malloc(WU_NSECTORS * sizeof(*wu_con[i]));
            check_alloc_status(wu_con[i], "Memory allocation error");
            
            wu_hist[i] = malloc(WU_NSECTORS * sizeof(*wu_hist[i]));
            check_alloc_status(wu_hist[i], "Memory allocation error");
            
            ext_all_vars[i].wu_var = malloc(WU_NSECTORS * sizeof(*ext_all_vars[i].wu_var));
            check_alloc_status(ext_all_vars[i].wu_var, "Memory allocation error");
            
            for(j=0;j<WU_NSECTORS;j++){
                ext_all_vars[i].wu_var[j].return_flow = malloc(ext_param.RETURN_DELAY[j] * sizeof(*ext_all_vars[i].wu_var[j].return_flow));
                check_alloc_status(ext_all_vars[i].wu_var[j].return_flow, "Memory allocation error");
                
                ext_all_vars[i].wu_var[j].compensation = malloc(ext_param.COMPENSATION_DELAY[j] * sizeof(*ext_all_vars[i].wu_var[j].compensation));
                check_alloc_status(ext_all_vars[i].wu_var[j].compensation, "Memory allocation error");
                
                wu_hist[i][j].demand = malloc(global_param.model_steps_per_day * DAYS_PER_LYEAR * sizeof(*wu_hist[i][j].demand));
                check_alloc_status(wu_hist[i][j].demand, "Memory allocation error");
                
                wu_hist[i][j].consumption_factor = malloc(global_param.model_steps_per_day * DAYS_PER_LYEAR * sizeof(*wu_hist[i][j].consumption_factor));
                check_alloc_status(wu_hist[i][j].consumption_factor, "Memory allocation error");
            }            
        }
    }
        
    initialize_ext_local_structures();
}