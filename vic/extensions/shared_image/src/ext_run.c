#include <ext_driver_shared_image.h>

void
ext_run(dmy_struct *dmy_current)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern double ***out_data;
    
    timer_struct               timer;
    size_t i;
        
    for(i=0;i<local_domain.ncells_active;i++){
        ext_update_step_vars(&ext_all_vars[i]);
    }
    
    timer_start(&timer);
    routing_run();          
    timer_stop(&timer);
    
    for(i=0;i<local_domain.ncells_active;i++){
        ext_put_data(&ext_all_vars[i], out_data[i], &timer);            
    }
}