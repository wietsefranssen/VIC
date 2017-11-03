#include <ext_driver_shared_image.h>

void
calculate_dam_surface_area(dam_con_struct dam_con, dam_var_struct *dam_var){
    // Calculate surface area based on Kaveh et al 2013
    double N = ( 2 * dam_con.max_volume ) / ( dam_con.max_height * dam_con.max_area );
    dam_var->area = dam_con.max_area * pow(( dam_var->volume / dam_con.max_volume ), ((2-N)/2));
}

void
calculate_dam_height(dam_var_struct *dam_var){
    // Calculate dam height based on Liebe et al 2005
    dam_var->height = sqrt(dam_var->area) * (1 / 19.45);
}

void
dam_run(dam_con_struct dam_con, 
        dam_var_struct *dam_var, 
        rout_var_struct *rout_var){
    extern global_param_struct global_param;
    extern dmy_struct *dmy;
    extern size_t current;
    
    // River discharge is saved for history
    dam_var->inflow_total += rout_var->discharge[0];
    dam_var->nat_inflow_total += rout_var->nat_discharge[0];
    
    // Store for output    
    dam_var->inflow = rout_var->discharge[0];
    dam_var->nat_inflow = rout_var->nat_discharge[0];
    
    // Check if dam is run
    if(!dam_var->run){
        if(dam_con.year <= dmy[current].year){
            dam_var->run = true;
        }else{
            return;
        }
    }
    
    // River discharge adds to dam reservoir volume
    dam_var->volume += dam_var->inflow * global_param.dt;
    rout_var->discharge[0] = 0;
    
    if(dam_var->calc_discharge[0] > 0){
        dam_var->discharge = dam_var->calc_discharge[0];        
        
        // Reduce discharge if not available 
        if(dam_var->discharge > dam_var->volume / global_param.dt ){
            dam_var->discharge = dam_var->volume / global_param.dt;
        }
        dam_var->volume -= dam_var->discharge * global_param.dt;
    }
    
    // Increase discharge with overflow
    if(dam_var->volume > dam_con.max_volume){
        dam_var->discharge += (dam_var->volume - dam_con.max_volume) / global_param.dt;
        dam_var->volume = dam_con.max_volume;
    }
    
    // Discharge
    rout_var->discharge[0] = dam_var->discharge;
    
    // Recalculate dam water area and height
    calculate_dam_surface_area(dam_con,dam_var);
    calculate_dam_height(dam_var);
}