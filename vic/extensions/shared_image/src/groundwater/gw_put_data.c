#include <ext_driver_shared_image.h>

void
gw_put_data(void)
{
    extern domain_struct local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern option_struct options;
    extern ext_all_vars_struct *ext_all_vars;
    extern double ***out_data;
    
    size_t i;
    size_t j;
    size_t k;
    
    double veg_frac;
    double snow_frac;
    
    for(i = 0; i < local_domain.ncells_active; i++){        
        for(j=0; j < veg_con_map[i].nv_active; j++){
            veg_frac = veg_con[i][j].Cv;
            
            for(k=0; k < options.SNOW_BAND; k++){
                snow_frac = soil_con[i].AreaFract[k];
                        
                out_data[i][OUT_GW_ZWT][0] += ext_all_vars[i].groundwater[j][k].zwt * 
                        veg_frac * snow_frac;
                out_data[i][OUT_GW_QR][0] += ext_all_vars[i].groundwater[j][k].Qr * 
                        veg_frac * snow_frac;
                out_data[i][OUT_GW_WA][0] += ext_all_vars[i].groundwater[j][k].Wa * 
                        veg_frac * snow_frac;
                out_data[i][OUT_GW_WT][0] += ext_all_vars[i].groundwater[j][k].Wt * 
                        veg_frac * snow_frac;
                out_data[i][OUT_GW_WS][0] += ext_all_vars[i].groundwater[j][k].Ws * 
                        veg_frac * snow_frac;
            }
        }
    }
}
