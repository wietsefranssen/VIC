#include <ext_driver_shared_image.h>

void
dam_generate_default_state(void)
{
    extern domain_struct local_domain;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    extern ext_all_vars_struct *ext_all_vars;
    
    size_t i;
    size_t j;
    
    for(i = 0; i < local_domain.ncells_active; i++){
        for(j = 0; j < dam_con_map[i].nd_active; j++){
            ext_all_vars[i].dams[j].volume = 
                    dam_con[i][j].max_volume * DAM_PREF_VOL_FRAC;
            ext_all_vars[i].dams[j].area = 
                    dam_area(ext_all_vars[i].dams[j].volume,
                    dam_con[i][j].max_volume,
                    dam_con[i][j].max_area,
                    dam_con[i][j].max_height);
            ext_all_vars[i].dams[j].height = 
                    dam_height(ext_all_vars[i].dams[j].area,
                    dam_con[i][j].max_height);
        }
    }    
}