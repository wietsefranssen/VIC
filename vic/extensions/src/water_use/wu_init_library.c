#include <ext_driver_shared_image.h>

void
initialize_wu_var(wu_var_struct *wu_var)
{
    extern ext_option_struct ext_options;
    
    size_t i;
    size_t j;
    
    for(i = 0; i < WU_NSECTORS; i++){
        wu_var[i].demand = 0.0;
        wu_var[i].compensated = 0.0;
        wu_var[i].consumed = 0.0; 
        wu_var[i].returned = 0.0; 
        wu_var[i].withdrawn = 0.0;
        wu_var[i].compensation_total = 0.0;
        for(j = 0; j < (size_t)ext_options.WU_COMPENSATION_TIME[i]; j++){
            wu_var[i].compensation[j] = 0.0;
        }
    }
}

void
initialize_wu_con(wu_con_struct *wu_con)
{
    size_t i;
    
    for(i = 0; i < WU_NSECTORS; i++){
        wu_con[i].consumption_fraction = 1.0; 
        wu_con[i].demand = 0.0;
    }
}

void
initialize_wu_local_structures(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern wu_con_struct **wu_con;
    
    size_t i;
    
    for(i=0; i < local_domain.ncells_active; i++){
        initialize_wu_con(wu_con[i]);
        initialize_wu_var(ext_all_vars[i].water_use);
    }
}