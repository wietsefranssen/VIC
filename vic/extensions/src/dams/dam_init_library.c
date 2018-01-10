#include <ext_driver_shared_image.h>

void
initialize_dam_var(dam_var_struct *dam_var)
{   
    size_t i;
    
    dam_var->area = 0.0;
    dam_var->height = 0.0;
    dam_var->volume = 0.0;
    dam_var->total_flow = 0.0;
    dam_var->total_steps = 0;
    dam_var->months_running = 0;
    
    for(i = 0; i < DAM_HIST_YEARS * MONTHS_PER_YEAR; i++){
        dam_var->history_flow[i] = 0.0;
    }    
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        dam_var->calc_volume[i] = 0.0;
    }
}

void
initialize_dam_con(dam_con_struct *dam_con)
{    
    dam_con->year = 0;
    dam_con->max_area = 0;
    dam_con->max_height = 0;
    dam_con->max_volume = 0;
}

void
initialize_dam_local_structures(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    size_t i;
    size_t j;
    
    for(i=0; i < local_domain.ncells_active; i++){        
        for(j = 0; j < dam_con_map[i].nd_active; j++){
            initialize_dam_con(&dam_con[i][j]);
            initialize_dam_var(&ext_all_vars[i].dams[j]);
        }
    }
}