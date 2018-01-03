#include <ext_driver_shared_image.h>

void
initialize_wu_local_structures(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern wu_con_struct *wu_con;
    
    size_t i;
    size_t j;
    
    for(i=0; i < local_domain.ncells_active; i++){
        initialize_wu_con(&wu_con[i]);
        for(j=0; j<WU_NSECTORS; j++){
            initialize_wu_var(&(ext_all_vars[i].water_use[j]));
        }
    }
}

void
initialize_wu_var(wu_var_struct *wu_var)
{
    wu_var->available = 0.0;
    wu_var->compensated = 0.0;
    wu_var->consumed = 0.0; 
    wu_var->discharge = 0.0; 
    wu_var->withdrawn = 0.0;
    wu_var->compensation = NULL;
    wu_var->return_flow = NULL;
}

void
initialize_wu_con(wu_con_struct *wu_con)
{
    wu_con->consumption_fraction = 0.0; 
    wu_con->demand = 0.0; 
    wu_con->force_offset = 0; 
    wu_con->input_interval = WU_INPUT_YEARLY; 
    wu_con->return_location = WU_RETURN_SURFACEWATER;
}