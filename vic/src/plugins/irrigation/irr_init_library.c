#include <vic.h>

void
initialize_irr_var(irr_var_struct *irr_var)
{    
    irr_var->pond_storage = 0.0;
    irr_var->leftover = 0.0;
    irr_var->prev_req = 0.0;
    irr_var->prev_store = 0.0;
    irr_var->prev_short = 0.0;
    irr_var->requirement = 0.0;
    irr_var->need = 0.0;
    irr_var->deficit = 0.0;
    irr_var->shortage = 0.0;
}

void
initialize_irr_con(irr_con_struct *irr_con)
{
    size_t i;
    
    irr_con->veg_index = NODATA_VEG;
    irr_con->ponding = false;
    irr_con->pond_capacity = 0.0;
    irr_con->season_offset = 0.0;
    for(i = 0; i < irr_con->nseasons; i++){
        irr_con->season_start[i] = 0.0;
        irr_con->season_end[i] = 0.0;
    }
}

void
initialize_irr_local_structures(void)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern irr_var_struct ***irr_var;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    
    size_t i;
    size_t j;
    size_t k;
    
    for(i=0; i < local_domain.ncells_active; i++){        
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            initialize_irr_con(&irr_con[i][j]);
        
            for(k=0; k < options.SNOW_BAND; k++){
                initialize_irr_var(&irr_var[i][j][k]);
            }
        }
    }
}