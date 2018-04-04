#include <vic.h>

void
irr_put_data(void)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern irr_var_struct ***irr_var;
    extern irr_con_map_struct *irr_con_map;
    extern veg_con_struct **veg_con;
    extern soil_con_struct *soil_con;
    extern double ***out_data;
    extern node            *outvar_types;
    
    size_t i;    
    size_t j;
    size_t k;
    size_t cur_veg;
    
    int OUT_IRR_REQUIREMENT = list_search_id(outvar_types, "OUT_IRR_REQUIREMENT");
    int OUT_IRR_NEED = list_search_id(outvar_types, "OUT_IRR_NEED");
    int OUT_IRR_LEFTOVER = list_search_id(outvar_types, "OUT_IRR_LEFTOVER");
    int OUT_IRR_POND_STORAGE = list_search_id(outvar_types, "OUT_IRR_POND_STORAGE");
    int OUT_IRR_SHORTAGE = list_search_id(outvar_types, "OUT_IRR_SHORTAGE");
    
    for(i = 0; i < local_domain.ncells_active; i++){ 
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            cur_veg = irr_con[i][j].veg_index;
            
            for(k = 0; k < options.SNOW_BAND; k++){
                out_data[i][OUT_IRR_REQUIREMENT][0] += irr_var[i][j][k].requirement * 
                    soil_con[i].AreaFract[k] * veg_con[i][cur_veg].Cv;
                out_data[i][OUT_IRR_NEED][0] += irr_var[i][j][k].need * 
                    soil_con[i].AreaFract[k] * veg_con[i][cur_veg].Cv;
                out_data[i][OUT_IRR_LEFTOVER][0] += irr_var[i][j][k].leftover * 
                    soil_con[i].AreaFract[k] * veg_con[i][cur_veg].Cv;
                out_data[i][OUT_IRR_POND_STORAGE][0] += irr_var[i][j][k].pond_storage * 
                    soil_con[i].AreaFract[k] * veg_con[i][cur_veg].Cv;
                out_data[i][OUT_IRR_SHORTAGE][0] += irr_var[i][j][k].shortage * 
                    soil_con[i].AreaFract[k] * veg_con[i][cur_veg].Cv;
            }
        }
    }    
}