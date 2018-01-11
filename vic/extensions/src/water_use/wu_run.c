#include <ext_driver_shared_image.h>

void
wu_run(size_t cur_cell)
{
    extern ext_option_struct ext_options;
    extern wu_con_struct **wu_con;
    extern ext_all_vars_struct *ext_all_vars;
    extern int current;
    extern dmy_struct *dmy;
    
    double total_available;
    double total_compensation;
    double total_demand;
    double withdrawn_fraction;
    
    size_t i;    
    size_t j;
        
    // Reset values
    for(i = 0; i < WU_NSECTORS; i++){                        
        if(dmy[current - 1].day_in_year != dmy[current].day_in_year){
            ext_all_vars[cur_cell].water_use[i].compensation[0] = 0.0;
            cshift(ext_all_vars[cur_cell].water_use[i].compensation, 1, ext_options.WU_COMPENSATION_TIME[i], 1, 1);
        }
        
        ext_all_vars[cur_cell].water_use[i].demand = wu_con[cur_cell][i].demand;
        ext_all_vars[cur_cell].water_use[i].compensated = 0.0;
        ext_all_vars[cur_cell].water_use[i].withdrawn = 0.0;
        ext_all_vars[cur_cell].water_use[i].consumed = 0.0;
        ext_all_vars[cur_cell].water_use[i].returned = 0.0;
        ext_all_vars[cur_cell].water_use[i].compensation_total = 0.0;
    }    
    
    // Get availability
    total_available = ext_all_vars[cur_cell].routing.discharge[0];
    
    // Satisfy compensation
    total_compensation = 0;
    for(i = 0; i < WU_NSECTORS; i++){
        for(j = 0; j < (size_t)ext_options.WU_COMPENSATION_TIME[i]; j++){
            ext_all_vars[cur_cell].water_use[i].compensation_total += ext_all_vars[cur_cell].water_use[i].compensation[j];
        }
        total_compensation += ext_all_vars[cur_cell].water_use[i].compensation_total;
    }
    
    withdrawn_fraction = 0.0;
    if(total_available > 0 && total_compensation > 0){
        withdrawn_fraction = total_available / total_compensation;
        if(withdrawn_fraction > 1){
            withdrawn_fraction = 1.0;
        }
        
        for(i = 0; i < WU_NSECTORS; i++){
            ext_all_vars[cur_cell].water_use[i].compensated = 
                    ext_all_vars[cur_cell].water_use[i].compensation_total * 
                    withdrawn_fraction;
            ext_all_vars[cur_cell].routing.discharge[0] -=
                    ext_all_vars[cur_cell].water_use[i].compensated;
            ext_all_vars[cur_cell].water_use[i].consumed += 
                    ext_all_vars[cur_cell].water_use[i].compensated * 
                    wu_con[cur_cell][i].consumption_fraction;
            ext_all_vars[cur_cell].water_use[i].returned += 
                    ext_all_vars[cur_cell].water_use[i].compensated * 
                    (1 - wu_con[cur_cell][i].consumption_fraction);
        }        

        if(ext_all_vars[cur_cell].routing.discharge[0] < 0){
            ext_all_vars[cur_cell].routing.discharge[0] = 0.0;
        }
    }
    
    // Get availability
    total_available = ext_all_vars[cur_cell].routing.discharge[0];
    
    // Satisfy demand
    total_demand = 0;
    for(i = 0; i < WU_NSECTORS; i++){
        total_demand += ext_all_vars[cur_cell].water_use[i].demand;
    }
    
    withdrawn_fraction = 0.0;
    if(total_available > 0 && total_demand > 0){
        withdrawn_fraction = total_available / total_demand;
        if(withdrawn_fraction > 1){
            withdrawn_fraction = 1.0;
        }
        
        for(i = 0; i < WU_NSECTORS; i++){
            ext_all_vars[cur_cell].water_use[i].withdrawn = 
                    ext_all_vars[cur_cell].water_use[i].demand * 
                    withdrawn_fraction;
            ext_all_vars[cur_cell].routing.discharge[0] -=
                    ext_all_vars[cur_cell].water_use[i].withdrawn;
            ext_all_vars[cur_cell].water_use[i].consumed += 
                    ext_all_vars[cur_cell].water_use[i].withdrawn * 
                    wu_con[cur_cell][i].consumption_fraction;
            ext_all_vars[cur_cell].water_use[i].returned += 
                    ext_all_vars[cur_cell].water_use[i].withdrawn * 
                    (1 - wu_con[cur_cell][i].consumption_fraction);
        }
        
        if(ext_all_vars[cur_cell].routing.discharge[0] < 0){
            ext_all_vars[cur_cell].routing.discharge[0] = 0.0;
        }
    }
    
    // Set current compensation need    
    for(i = 0; i < WU_NSECTORS; i++){
        ext_all_vars[cur_cell].water_use[i].compensation[ext_options.WU_COMPENSATION_TIME[i] - 1] +=
                ext_all_vars[cur_cell].water_use[i].demand * 
                (1 - withdrawn_fraction);
    }
     
    // Return flow
    for(i = 0; i < WU_NSECTORS; i++){
        if(ext_options.WU_RETURN_LOCATION[i] == WU_RETURN_SURFACEWATER){
            ext_all_vars[cur_cell].routing.discharge[0] +=
                    ext_all_vars[cur_cell].water_use[i].returned;
        }else if(ext_options.WU_RETURN_LOCATION[i] == WU_RETURN_GROUNDWATER){
            log_warn("Water use returned to groundwater not yet implemented...");
            ext_all_vars[cur_cell].routing.discharge[0] +=
                    ext_all_vars[cur_cell].water_use[i].returned;
        }else{
            log_err("Unknown water use return location");
        }
    }  
}