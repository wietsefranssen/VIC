#include <ext_driver_shared_image.h>

#include "efr.h"

void
efr_alloc(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    
    size_t i;
    
    for(i=0; i<local_domain.ncells_active; i++){
        ext_all_vars[i].efr.history_flow = malloc(EFR_HIST_YEARS * MONTHS_PER_YEAR * sizeof(*ext_all_vars[i].efr.history_flow));
        check_alloc_status(ext_all_vars[i].efr.history_flow,"Memory allocation error"); 
    }    
}

void
efr_finalize(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    
    size_t i;    
            
    for(i=0; i<local_domain.ncells_active; i++){  
        free(ext_all_vars[i].efr.history_flow);
    }
}