#include <ext_driver_shared_image.h>

void
irr_generate_default_state(void)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern ext_all_vars_struct *ext_all_vars;
    
    size_t i;
    size_t j;
    size_t k;
    
    size_t cur_veg;
    
    for(i = 0; i < local_domain.ncells_active; i++){
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            cur_veg = irr_con_map[i].vidx[j];

            for(k = 0; k < options.SNOW_BAND; k++){
                ext_all_vars[i].irrigation[j][k].prev_root_moist = all_vars[i].cell[cur_veg][k].layer[0].moist;
            }
        }
    }    
}