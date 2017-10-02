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