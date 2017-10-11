#include <ext_driver_shared_image.h>

void
ext_put_data(ext_all_vars_struct *ext_all_vars,
                double **out_data,
                timer_struct *timer){
    
    extern ext_option_struct ext_options;
    
    rout_var_struct rout_var;
        
    rout_var = ext_all_vars->rout_var;
    
    if(ext_options.ROUTING){
        out_data[OUT_DISCHARGE][0]=rout_var.discharge[0];
        out_data[OUT_NAT_DISCHARGE][0]=rout_var.nat_discharge[0];
    }
    
    out_data[OUT_TIME_EXTRUN_WALL][0] = timer->delta_wall;
    out_data[OUT_TIME_EXTRUN_CPU][0] = timer->delta_cpu;    
}
