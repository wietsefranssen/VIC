#include <ext_driver_shared_image.h>

void
ext_put_data(ext_all_vars_struct *ext_all_vars,
                double **out_data,
                timer_struct *timer){
    
    extern ext_option_struct ext_options;
    
    rout_var_struct rout_var;
    wu_var_struct *wu_var;
        
    rout_var = ext_all_vars->rout_var;
    wu_var = ext_all_vars->wu_var;
    
    if(ext_options.ROUTING){
        out_data[OUT_DISCHARGE][0]=rout_var.discharge[0];
    }
    if(ext_options.WATER_USE){
        out_data[OUT_IRR_WITHDRAWAL][0]=wu_var[WU_IRRIGATION].withdrawn;
        out_data[OUT_DOM_WITHDRAWAL][0]=wu_var[WU_DOMESTIC].withdrawn;
        out_data[OUT_IND_WITHDRAWAL][0]=wu_var[WU_INDUSTRIAL].withdrawn;
        out_data[OUT_IRR_CONSUMPTION][0]=wu_var[WU_IRRIGATION].consumed;
        out_data[OUT_DOM_CONSUMPTION][0]=wu_var[WU_DOMESTIC].consumed;
        out_data[OUT_IND_CONSUMPTION][0]=wu_var[WU_INDUSTRIAL].consumed;
        out_data[OUT_IRR_DEMAND][0]=wu_var[WU_IRRIGATION].withdrawn + wu_var[WU_IRRIGATION].shortage;
        out_data[OUT_DOM_DEMAND][0]=wu_var[WU_DOMESTIC].consumed + wu_var[WU_DOMESTIC].shortage;
        out_data[OUT_IND_DEMAND][0]=wu_var[WU_INDUSTRIAL].consumed + wu_var[WU_INDUSTRIAL].shortage;
        out_data[OUT_IRR_SHORTAGE][0]=wu_var[WU_IRRIGATION].shortage;
        out_data[OUT_DOM_SHORTAGE][0]=wu_var[WU_DOMESTIC].shortage;
        out_data[OUT_IND_SHORTAGE][0]=wu_var[WU_INDUSTRIAL].shortage;
        out_data[OUT_IRR_RETURN][0]=wu_var[WU_IRRIGATION].return_flow[0];
        out_data[OUT_DOM_RETURN][0]=wu_var[WU_DOMESTIC].return_flow[0];
        out_data[OUT_IND_RETURN][0]=wu_var[WU_INDUSTRIAL].return_flow[0];
        
        out_data[OUT_TOT_WITHDRAWAL][0]=
                out_data[OUT_IRR_WITHDRAWAL][0]+out_data[OUT_DOM_WITHDRAWAL][0]+out_data[OUT_IND_WITHDRAWAL][0];
        out_data[OUT_TOT_CONSUMPTION][0]=
                out_data[OUT_IRR_CONSUMPTION][0]+out_data[OUT_DOM_CONSUMPTION][0]+out_data[OUT_IND_CONSUMPTION][0];
        out_data[OUT_TOT_DEMAND][0]=
                out_data[OUT_IRR_DEMAND][0]+out_data[OUT_DOM_DEMAND][0]+out_data[OUT_IND_DEMAND][0];
        out_data[OUT_TOT_SHORTAGE][0]=
                out_data[OUT_IRR_SHORTAGE][0]+out_data[OUT_DOM_SHORTAGE][0]+out_data[OUT_IND_SHORTAGE][0];
        out_data[OUT_TOT_RETURN][0]=
                out_data[OUT_IRR_RETURN][0]+out_data[OUT_DOM_RETURN][0]+out_data[OUT_IND_RETURN][0];  
    }
    out_data[OUT_TIME_EXTRUN_WALL][0] = timer->delta_wall;
    out_data[OUT_TIME_EXTRUN_CPU][0] = timer->delta_cpu;    
}
