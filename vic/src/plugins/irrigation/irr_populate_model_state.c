#include <vic.h>

void
irr_generate_default_state(void)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern irr_var_struct ***irr_var;
    
    size_t i;
    size_t j;
    size_t k;
    
    size_t cur_veg;
    
    for(i = 0; i < local_domain.ncells_active; i++){
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            cur_veg = irr_con_map[i].vidx[j];

            for(k = 0; k < options.SNOW_BAND; k++){
                irr_var[i][j][k].prev_avail_moist = all_vars[i].cell[cur_veg][k].layer[0].moist;
            }
        }
    }    
}