#include <ext_driver_shared_image.h>

void
rout_put_data(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern double ***out_data;
    
    size_t i;    
    
    for(i = 0; i < local_domain.ncells_active; i++){ 
        out_data[i][OUT_DISCHARGE][0] = ext_all_vars[i].routing.discharge[0];
        out_data[i][OUT_NAT_DISCHARGE][0] = ext_all_vars[i].routing.nat_discharge[0];
    }    
}