#include <ext_driver_shared_image.h>

void
gw_put_data(void)
{
    extern domain_struct local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern option_struct options;
    extern ext_all_vars_struct *ext_all_vars;
    extern double ***out_data;
    
    size_t i;
    size_t j;
    size_t k;
    
    for(i = 0; i < local_domain.ncells_active; i++){        
        for(j=0; j < veg_con_map[i].nv_active; j++){
            for(k=0; k < options.SNOW_BAND; k++){
                out_data[i][OUT_GW_ZWT][0] = ext_all_vars[i].groundwater[j][k].zwt;
                out_data[i][OUT_GW_QR][0] = ext_all_vars[i].groundwater[j][k].Qr;
                out_data[i][OUT_GW_WA][0] = ext_all_vars[i].groundwater[j][k].Wa;
                out_data[i][OUT_GW_WT][0] = ext_all_vars[i].groundwater[j][k].Wt;
            }
        }
    }
}
