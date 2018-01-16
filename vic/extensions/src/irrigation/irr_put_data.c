#include <ext_driver_shared_image.h>

void
irr_put_data(void)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern ext_all_vars_struct *ext_all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern veg_con_struct **veg_con;
    extern soil_con_struct *soil_con;
    extern double ***out_data;
    
    size_t i;    
    size_t j;
    size_t k;
    size_t cur_veg;
    
    for(i = 0; i < local_domain.ncells_active; i++){ 
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            cur_veg = irr_con_map[i].vidx[j];
            
            for(k = 0; k < options.SNOW_BAND; k++){
                out_data[i][OUT_IRR_REQUIREMENT][0] += ext_all_vars[i].irrigation[j][k].requirement * 
                    soil_con[i].AreaFract[j] * veg_con[i][cur_veg].Cv;
                out_data[i][OUT_IRR_NEED][0] += ext_all_vars[i].irrigation[j][k].need * 
                    soil_con[i].AreaFract[j] * veg_con[i][cur_veg].Cv;
                out_data[i][OUT_IRR_LEFTOVER][0] += ext_all_vars[i].irrigation[j][k].leftover * 
                    soil_con[i].AreaFract[j] * veg_con[i][cur_veg].Cv;
                out_data[i][OUT_IRR_POND_STORAGE][0] += ext_all_vars[i].irrigation[j][k].pond_storage * 
                    soil_con[i].AreaFract[j] * veg_con[i][cur_veg].Cv;
            }
        }
    }    
}