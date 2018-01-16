#include <ext_driver_shared_image.h>

void
efr_put_data(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern double ***out_data;
    
    size_t i;  
    
    for(i = 0; i < local_domain.ncells_active; i++){ 
        out_data[i][OUT_EFR_REQUIREMENT][0] = ext_all_vars[i].efr.requirement;
    }    
}