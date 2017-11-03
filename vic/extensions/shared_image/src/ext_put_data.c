#include <ext_driver_shared_image.h>

void
ext_put_data(ext_all_vars_struct *ext_all_vars,
                dam_con_struct *dam_con,
                dam_con_map_struct dam_con_map,
                double **out_data,
                timer_struct *timer){
    
    extern ext_option_struct ext_options;
    
    rout_var_struct rout_var;
    dam_var_struct *dam_var;
    
    size_t i;
        
    rout_var = ext_all_vars->rout_var;
    dam_var = ext_all_vars->dam_var;
    
    if(ext_options.ROUTING){
        out_data[OUT_DISCHARGE][0]=rout_var.discharge[0];
        out_data[OUT_NAT_DISCHARGE][0]=rout_var.nat_discharge[0];
    }
    if(ext_options.DAMS){
        for(i=0;i<dam_con_map.Ndams;i++){
            out_data[OUT_DAM_VOLUME][i] = dam_var[i].volume;
            out_data[OUT_DAM_MAX_VOLUME][i] = dam_con[i].max_volume;
            out_data[OUT_DAM_FRAC_VOLUME][i] = 
                    out_data[OUT_DAM_VOLUME][i] / out_data[OUT_DAM_MAX_VOLUME][i];
            out_data[OUT_DAM_AREA][i] = dam_var[i].area;
            out_data[OUT_DAM_MAX_AREA][i] = dam_con[i].max_area;
            out_data[OUT_DAM_FRAC_AREA][i] = 
                    out_data[OUT_DAM_AREA][i] / out_data[OUT_DAM_MAX_AREA][i];
            out_data[OUT_DAM_HEIGHT][i] = dam_var[i].height;
            out_data[OUT_DAM_MAX_HEIGHT][i] = dam_con[i].max_height;
            out_data[OUT_DAM_FRAC_HEIGHT][i] = 
                    out_data[OUT_DAM_HEIGHT][i] / out_data[OUT_DAM_MAX_HEIGHT][i];
            
            out_data[OUT_DAM_DISCHARGE][i] = dam_var[i].discharge;
            out_data[OUT_DAM_CALC_DISCHARGE][i] = dam_var[i].calc_discharge[0];
            out_data[OUT_DAM_CALC_INFLOW][i] = dam_var->calc_inflow[0];
            out_data[OUT_DAM_CALC_NAT_INFLOW][i] = dam_var->calc_nat_inflow[0]; 
        
            out_data[OUT_DAM_INFLOW][i] = dam_var[i].inflow;
            out_data[OUT_DAM_NAT_INFLOW][i] = dam_var[i].nat_inflow;
            
            out_data[OUT_DAM_AMPLITUDE][i] = dam_var->amplitude;
            out_data[OUT_DAM_OFFSET][i] = dam_var->offset;                       
        }
    }
    
    out_data[OUT_TIME_EXTRUN_WALL][0] = timer->delta_wall;
    out_data[OUT_TIME_EXTRUN_CPU][0] = timer->delta_cpu;    
}
