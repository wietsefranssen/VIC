#include <ext_driver_shared_image.h>

void
dam_put_data(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern dam_con_map_struct *dam_con_map;
    extern double ***out_data;
    
    size_t i;    
    size_t j;
    
    for(i = 0; i < local_domain.ncells_active; i++){ 
        for(j = 0; j < dam_con_map[i].nd_active; j++){
            out_data[i][OUT_DAM_VOLUME][j] = ext_all_vars[i].dams[j].volume / pow(M_PER_KM, 3);
            out_data[i][OUT_DAM_DISCHARGE][j] = ext_all_vars[i].dams[j].discharge;
            out_data[i][OUT_DAM_AREA][j] = ext_all_vars[i].dams[j].area / pow(M_PER_KM, 2);
            out_data[i][OUT_DAM_HEIGHT][j] = ext_all_vars[i].dams[j].height;
            out_data[i][OUT_DAM_OP_DISCHARGE][j] = ext_all_vars[i].dams[j].op_discharge[0];
            out_data[i][OUT_DAM_OP_VOLUME][j] = ext_all_vars[i].dams[j].op_volume[0] / pow(M_PER_KM, 3);
            out_data[i][OUT_DAM_OP_MONTH][j] = ext_all_vars[i].dams[j].op_year.month;
        }
    }    
}