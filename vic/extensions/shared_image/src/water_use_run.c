#include <ext_driver_shared_image.h>

void
water_use_run(ext_all_vars_struct *ext_all_vars, wu_con_struct *wu_con){
    
    extern global_param_struct global_param;
    
    size_t i;
    size_t j;
    
    double total_available;
    double total_historical_demand;
    double total_current_demand;
    double historical_demand[WU_NSECTORS];
    double current_demand[WU_NSECTORS];    
    double compensation_storage;
    
    wu_var_struct *wu_var;
    rout_var_struct rout_var;
    
    wu_var = (*ext_all_vars).wu_var;
    rout_var = (*ext_all_vars).rout_var;
            
    // Reset values
    total_available = 0.0;
    total_historical_demand = 0.0;
    total_current_demand = 0.0;

    for(i=0;i<WU_NSECTORS;i++){
        historical_demand[i] = 0.0;
        current_demand[i] = 0.0;

        wu_var[i].consumed=0.0;
        wu_var[i].compensated=0.0;
        wu_var[i].withdrawn=0.0;
        wu_var[i].returned=0.0;
        wu_var[i].to_return=0.0;
        wu_var[i].to_compensate=0.0;           
    }

    // Return flow
    for(i=0;i<WU_NSECTORS;i++){            
        if(wu_con[i].return_location == WU_RETURN_SURFACEWATER){
            rout_var.discharge[0] += wu_var[i].return_flow[0] / global_param.dt;
        }else if(wu_con[i].return_location == WU_RETURN_GROUNDWATER){
            // TODO: implement flow to groundwater                
            rout_var.discharge[0] += wu_var[i].return_flow[0] / global_param.dt;
        }
    }

    // Gather availability
    total_available = rout_var.discharge[0] * global_param.dt;

    // Gather demand
    for(i=0;i<WU_NSECTORS;i++){            
        for(j=0;j<wu_con[i].compenstation_delay;j++){
            historical_demand[i] += wu_var[i].compensation[j];
        }         
        current_demand[i] = wu_con[i].demand;

        total_historical_demand += historical_demand[i];
        total_current_demand += current_demand[i];   
    }

    // First handle compensations from historical shortages
    for(i=0;i<WU_NSECTORS;i++){

        // Calculate withdrawal
        if(total_historical_demand > 0 && total_available > 0){
            if(total_historical_demand>total_available){
                wu_var[i].compensated += (total_available / total_historical_demand) * historical_demand[i];
            }else{
                wu_var[i].compensated += historical_demand[i];
            }       
        }

        // Compensate for most historical shortage first
        compensation_storage = wu_var[i].compensated;
        for(j=0;j<wu_con[i].compenstation_delay;j++){
            if(compensation_storage < wu_var[i].compensation[j]){
                wu_var[i].compensation[j] -= compensation_storage;
                compensation_storage = 0;
            }else{
                compensation_storage -= wu_var[i].compensation[j];
                wu_var[i].compensation[j] = 0;
            }
        }

        // Reduce availability
        total_available -= wu_var[i].compensated;
    }

    // Then handle the current shortage
    for(i=0;i<WU_NSECTORS;i++){

        // Calculate withdrawal
        if(total_current_demand > 0 && total_available > 0){
            if(total_current_demand>total_available){
                wu_var[i].withdrawn += (total_available / total_current_demand) * current_demand[i];
            }else{
                wu_var[i].withdrawn += current_demand[i];
            }       
        }

        // Reduce availability
        total_available -= wu_var[i].withdrawn;
    }

    for(i=0;i<WU_NSECTORS;i++){
        wu_var[i].to_compensate = wu_con[i].demand - wu_var[i].withdrawn;       
        wu_var[i].compensation[wu_con[i].compenstation_delay - 1] = wu_var[i].to_compensate;

        // Withdrawn is all withdrawn water while compensated is 
        // the part of the withdrawn water that is used for 
        // historical compensation
        wu_var[i].withdrawn += wu_var[i].compensated;

        wu_var[i].consumed = wu_var[i].withdrawn * wu_con[i].consumption_factor;
        wu_var[i].to_return = wu_var[i].withdrawn - wu_var[i].consumed;            
        wu_var[i].return_flow[wu_con[i].return_delay - 1] = wu_var[i].to_return;
    }

    // Check results
    if(total_available < 0){
        if(labs(total_available) < DBL_EPSILON){
            total_available=0;
        }else{
            log_err("Too much water has been withdrawn from the available, "
                    "resulting in a negative availability of %lf m3",
                    total_available);
        }
    }

    // Reduce outflow
    rout_var.discharge[0] = total_available / global_param.dt;
}