#include <vic.h>

void
dam_put_data(void)
{
    extern domain_struct local_domain;
    extern dam_var_struct **dam_var;
    extern dam_con_map_struct *dam_con_map;
    extern double ***out_data;
    extern node            *outvar_types;

    size_t i;    
    size_t j;
    
    // Write to output struct
    int OUT_DAM_VOLUME = list_search_id(outvar_types, "OUT_DAM_VOLUME");
    int OUT_DAM_DISCHARGE = list_search_id(outvar_types, "OUT_DAM_DISCHARGE");
    int OUT_DAM_AREA = list_search_id(outvar_types, "OUT_DAM_AREA");
    int OUT_DAM_HEIGHT = list_search_id(outvar_types, "OUT_DAM_HEIGHT");
    int OUT_DAM_OP_DISCHARGE = list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE");
    int OUT_DAM_OP_VOLUME = list_search_id(outvar_types, "OUT_DAM_OP_VOLUME");
    int OUT_DAM_OP_MONTH = list_search_id(outvar_types, "OUT_DAM_OP_MONTH");

    for(i = 0; i < local_domain.ncells_active; i++){ 
        for(j = 0; j < dam_con_map[i].nd_active; j++){
            out_data[i][OUT_DAM_VOLUME][j] = dam_var[i][j].volume / pow(M_PER_KM, 2);
            out_data[i][OUT_DAM_DISCHARGE][j] = dam_var[i][j].discharge;
            out_data[i][OUT_DAM_AREA][j] = dam_var[i][j].area / pow(M_PER_KM, 2);
            out_data[i][OUT_DAM_HEIGHT][j] = dam_var[i][j].height;
            out_data[i][OUT_DAM_OP_DISCHARGE][j] = dam_var[i][j].op_discharge[0];
            out_data[i][OUT_DAM_OP_VOLUME][j] = dam_var[i][j].op_volume[0] / pow(M_PER_KM, 3);
            out_data[i][OUT_DAM_OP_MONTH][j] = dam_var[i][j].op_year;
        }
    }    
}