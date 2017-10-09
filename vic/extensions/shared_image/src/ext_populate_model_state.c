#include <ext_driver_shared_image.h>

/******************************************************************************
 * @brief    This function handles tasks related to populating model state.
 *****************************************************************************/
void
ext_populate_model_state(){    
    extern option_struct    options;
    extern domain_struct local_domain;
    extern ext_option_struct    ext_options;
    extern ext_all_vars_struct *ext_all_vars;
    extern dam_con_struct **dam_con;
    extern dam_con_map_struct *dam_con_map;
    
    size_t i;
    
    // read the model state from the netcdf file if there is one
    if (options.INIT_STATE) {
        ext_restore();
    }
    else{
        for (i = 0; i < local_domain.ncells_active; i++) {
            if(ext_options.ROUTING){
                generate_default_routing_state(&ext_all_vars[i]);
            }
            if(ext_options.DAMS){
                generate_default_dams_state(&ext_all_vars[i],dam_con[i],dam_con_map[i]);
            }
        }
    }
}

void
generate_default_dams_state(ext_all_vars_struct *ext_all_vars, dam_con_struct *dam_con, dam_con_map_struct dam_con_map){
    extern ext_option_struct ext_options;
    
    size_t i;
    size_t j;
    
    dam_var_struct dam_var;
        
    for(i=0;i<dam_con_map.Ndams;i++){
        dam_var = ext_all_vars->dam_var[i];
        
        dam_var.volume = 0.85 * dam_con[i].max_volume;
        calculate_dam_surface_area(dam_con[i], &dam_var);
        calculate_dam_height(&dam_var);
        
        dam_var.inflow_total = 0.0;
        dam_var.inflow_history_offset = 0.0;
        dam_var.discharge = 0.0;
        dam_var.outflow_offset = 0.0;
        dam_var.outflow_variability = 0.5;
        dam_var.run = false;
        dam_var.years_running = 0;
        for(j=0;j<ext_options.history_steps;j++){
            dam_var.inflow_history[j] = 0.0;
        }    
    }
}

void
generate_default_routing_state(ext_all_vars_struct *ext_all_vars){
    extern ext_option_struct ext_options;
    
    size_t i;
    
    rout_var_struct rout_var;
    
    rout_var = ext_all_vars->rout_var;
    
    for(i=0;i<ext_options.uh_steps;i++){
        rout_var.discharge[i] = 0.0;
    }
}