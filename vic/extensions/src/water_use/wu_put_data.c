#include <ext_driver_shared_image.h>

void
wu_put_data(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern double ***out_data;
    
    size_t i;    
    size_t j;
    
    for(i = 0; i < local_domain.ncells_active; i++){ 
        for(j = 0; j < WU_NSECTORS; j++){
            out_data[i][OUT_WU_DEMAND][0] = ext_all_vars[i].water_use[j].demand;
            out_data[i][OUT_WU_COMPENSATION][0] = ext_all_vars[i].water_use[j].compensation_total;
            out_data[i][OUT_WU_COMPENSATED][0] = ext_all_vars[i].water_use[j].compensated;
            out_data[i][OUT_WU_WITHDRAWN][0] = ext_all_vars[i].water_use[j].withdrawn;
            out_data[i][OUT_WU_CONSUMED][0] = ext_all_vars[i].water_use[j].consumed;
            out_data[i][OUT_WU_RETURNED][0] = ext_all_vars[i].water_use[j].returned;
            out_data[i][OUT_WU_SHORTAGE][0] = ext_all_vars[i].water_use[j].compensation[0];
        }
    }    
}