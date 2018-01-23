#include <vic.h>

void
initialize_dam_var(dam_var_struct *dam_var)
{   
    size_t i;
    
    dam_var->area = 0.0;
    dam_var->height = 0.0;
    dam_var->volume = 0.0;
    dam_var->discharge = 0.0;
    dam_var->total_flow = 0.0;
    dam_var->total_steps = 0;
    dam_var->months_running = 0;
    
    for(i = 0; i < DAM_HIST_YEARS * MONTHS_PER_YEAR; i++){
        dam_var->history_flow[i] = 0.0;
    }    
    for(i = 0; i < MONTHS_PER_YEAR; i++){
        dam_var->op_volume[i] = 0.0;
        dam_var->op_discharge[i] = 0.0;
    }
    
//TODO    initialize_dmy(&(dam_var->op_year));
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
    extern dam_var_struct **dam_var;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    size_t i;
    size_t j;
    
    for(i=0; i < local_domain.ncells_active; i++){        
        for(j = 0; j < dam_con_map[i].nd_active; j++){
            initialize_dam_con(&dam_con[i][j]);
            initialize_dam_var(&dam_var[i][j]);
        }
    }
}