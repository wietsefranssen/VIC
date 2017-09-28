#include <ext_driver_shared_image.h>

void
local_water_use_run(){    
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern ext_all_vars_struct *ext_all_vars;
    extern wu_con_struct **wu_con;
    
    size_t i;
    size_t j;
    
    double total_demand;
    double total_withdrawn;
    double available_water;
    
    for(i=0;i<local_domain.ncells_active;i++){
        extern double ***out_data;
        extern wu_hist_struct **wu_hist;
        out_data[i][OUT_IRR_DEMAND][0]=wu_hist[i][WU_IRRIGATION].demand[0];
        out_data[i][OUT_DOM_DEMAND][0]=wu_hist[i][WU_DOMESTIC].demand[0];
        out_data[i][OUT_IND_DEMAND][0]=wu_hist[i][WU_INDUSTRIAL].demand[0];
        
        // Return flow
        for(j=0;j<WU_NSECTORS;j++){            
            if(wu_con[i][j].return_location == WU_RETURN_SURFACEWATER){
                ext_all_vars[i].rout_var.discharge[0] += ext_all_vars[i].wu_var[j].return_flow[0] / global_param.dt;
            }else if(wu_con[i][j].return_location == WU_RETURN_GROUNDWATER){
                ext_all_vars[i].rout_var.discharge[0] += ext_all_vars[i].wu_var[j].return_flow[0] / global_param.dt;
            }
        }
        available_water = ext_all_vars[i].rout_var.discharge[0] * global_param.dt;
        
        // Reset values and gather demand
        total_demand = 0.0;
        for(j=0;j<WU_NSECTORS;j++){
            ext_all_vars[i].wu_var[j].shortage=0.0;
            ext_all_vars[i].wu_var[j].consumed=0.0;
            ext_all_vars[i].wu_var[j].withdrawn=0.0;
            ext_all_vars[i].wu_var[j].returned=0.0;
            
            total_demand += wu_con[i][j].demand;            
        }        
        if(total_demand == 0){
            continue;
        }
        
        // Calculate withdrawal, consumption and return flow
        total_withdrawn = 0.0;
        for(j=0;j<WU_NSECTORS;j++){
            if(total_demand>available_water){
                ext_all_vars[i].wu_var[j].withdrawn = (available_water / total_demand) * wu_con[i][j].demand;
            }else{
                ext_all_vars[i].wu_var[j].withdrawn = wu_con[i][j].demand;
            }       
            
            ext_all_vars[i].wu_var[j].shortage = wu_con[i][j].demand - ext_all_vars[i].wu_var[j].withdrawn;
            ext_all_vars[i].wu_var[j].consumed = ext_all_vars[i].wu_var[j].withdrawn * wu_con[i][j].consumption_factor;
            ext_all_vars[i].wu_var[j].returned = ext_all_vars[i].wu_var[j].withdrawn - ext_all_vars[i].wu_var[j].consumed;
            
            ext_all_vars[i].wu_var[j].return_flow[wu_con[i][j].delay - 1] = ext_all_vars[i].wu_var[j].returned;
            total_withdrawn += ext_all_vars[i].wu_var[j].withdrawn;
        }
        
        // Reduce outflow
        ext_all_vars[i].rout_var.discharge[0] -= total_withdrawn / global_param.dt;
        if(ext_all_vars[i].rout_var.discharge[0] < 0){
            ext_all_vars[i].rout_var.discharge[0]=0;
        }
    }
}