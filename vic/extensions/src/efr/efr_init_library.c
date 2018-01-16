#include <ext_driver_shared_image.h>

#include "efr.h"

void
initialize_efr_var(efr_var_struct *efr_var)
{    
    size_t i;
    
    efr_var->ay_flow = 0.0;
    efr_var->am_flow = 0.0;    
    for(i = 0; i < EFR_HIST_YEARS * MONTHS_PER_YEAR; i++){
        efr_var->history_flow[i] = 0.0;
    }
    efr_var->requirement = 0.0;
    
    efr_var->total_flow = 0.0;
    efr_var->total_steps = 0;
    efr_var->months_running = 0;
}

void
initialize_efr_local_structures(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    
    size_t i;
    
    for(i=0; i < local_domain.ncells_active; i++){
        initialize_efr_var(&ext_all_vars[i].efr);
    }
}