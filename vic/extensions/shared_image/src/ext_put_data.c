#include <ext_driver_shared_image.h>

void
ext_put_data(ext_all_vars_struct *ext_all_vars,
                double **out_data,
                timer_struct *timer){
    
    extern ext_option_struct ext_options;
    
    rout_var_struct rout_var;
    wu_var_struct *wu_var;
    
    size_t i;
        
    rout_var = ext_all_vars->rout_var;
    wu_var = ext_all_vars->wu_var;
    
    if(ext_options.ROUTING){
        out_data[OUT_DISCHARGE][0]=rout_var.discharge[0];
    }
    if(ext_options.WATER_USE){
        for(i=0;i<WU_NSECTORS;i++){
            out_data[OUT_WU_WITHDRAWAL][i]+=wu_var[i].withdrawn;           
            out_data[OUT_WU_DEMAND][i]+=out_data[OUT_WU_WITHDRAWAL][i]+out_data[OUT_WU_SHORTAGE][i];
            out_data[OUT_WU_CONSUMPTION][i]+=wu_var[i].consumed;
            out_data[OUT_WU_RETURN_FLOW][i]+=wu_var[i].return_flow[0];
            out_data[OUT_WU_SHORTAGE][i]+=wu_var[i].compensation[0];
            out_data[OUT_WU_COMPENSATED][i]+=wu_var[i].compensated;
            out_data[OUT_WU_TO_COMPENSATE][i]+=wu_var[i].to_compensate;
            out_data[OUT_WU_TO_RETURN][i]+=wu_var[i].to_return;
        }
    }
    out_data[OUT_TIME_EXTRUN_WALL][0] = timer->delta_wall;
    out_data[OUT_TIME_EXTRUN_CPU][0] = timer->delta_cpu;    
}
