#include <ext_driver_shared_image.h>

void
calculate_dam_surface_area(dam_con_struct dam_con, dam_var_struct *dam_var){
    // Calculate surface area based on Kaveh et al 2013
    double N = ( 2 * dam_con.max_volume ) / ( dam_con.max_height * dam_con.max_area );
    dam_var->area = dam_con.max_area * pow(( dam_var->volume / dam_con.max_volume ), ((2-N)/2));
    if(dam_var->area > dam_con.max_area){
        dam_var->area = dam_con.max_area;
    }
}

void
calculate_dam_height(dam_con_struct dam_con, dam_var_struct *dam_var){
    // Calculate dam height based on Liebe et al 2005
    dam_var->height = sqrt(dam_var->area) * (1 / 19.45);    
    if(dam_var->height > dam_con.max_height){
        dam_var->height = dam_con.max_height;
    }
}

void
adapt_cv(dam_var_struct *dam_var, veg_con_map_struct veg_con_map, veg_con_struct *veg_con, location_struct location){
    
    extern option_struct options;
    
    size_t i;
        
    dam_var->Cd = dam_var->area / location.area;
    
    for(i=0;i<options.NVEGTYPES;i++){
        if(veg_con_map.vidx[i] != NODATA_VEG){
            veg_con[veg_con_map.vidx[i]].Cv = veg_con_map.Cv[i] * (1 - dam_var->Cd);
        }
    }
}

void
dam_run(dam_con_struct dam_con, dam_var_struct *dam_var, rout_var_struct *rout_var,
        veg_con_struct *veg_con, veg_con_map_struct veg_con_map,
        force_data_struct force, location_struct location, dmy_struct dmy){
    extern global_param_struct global_param;
                
    // River discharge is saved for history
    dam_var->inflow_total += rout_var->discharge[0];
    dam_var->nat_inflow_total += rout_var->nat_discharge[0];
    
    // Check if dam is run
    if(!dam_var->run){
        if(dam_con.year <= dmy.year){
            dam_var->run = true;
        }else{
            return;
        }
    }
    
    // Store for output    
    dam_var->inflow = rout_var->discharge[0];
    dam_var->nat_inflow = rout_var->nat_discharge[0];
    
    // River discharge adds to dam reservoir volume
    dam_var->volume += dam_var->inflow * global_param.dt;
    rout_var->discharge[0] = 0;
    
    // Reduce discharge if not available
    dam_var->discharge = dam_var->calc_discharge[0]; 
    if(dam_var->discharge * global_param.dt  > dam_var->volume){
        dam_var->discharge = dam_var->volume / global_param.dt;
    }
    dam_var->volume -= dam_var->discharge * global_param.dt;
    
    // Increase discharge with overflow
    if(dam_var->volume > dam_con.max_volume){
        dam_var->discharge += (dam_var->volume - dam_con.max_volume) / global_param.dt;
        dam_var->volume = dam_con.max_volume;
    }
    
    // Discharge
    rout_var->discharge[0] = dam_var->discharge;
    
    // Recalculate dam water area and height
    calculate_dam_surface_area(dam_con,dam_var);
    calculate_dam_height(dam_con,dam_var);
    
    // Reduce or increase vegetation area (also calculate Cd)
    adapt_cv(dam_var,veg_con_map,veg_con,location);
    
    // Calculate evaporation
    
    
}

void
calculate_dam_evap(force_data_struct force, dam_var_struct dam_var, double epsilon){
    extern int NR;
    extern global_param_struct global_param;
    
    double sw_in;
    double dam_wind;
    
    double Ts;
    double Tmean;
    double drag;
    double eog;
    double qlake;
    double qair;
    double del_q;
    double evaporation;
    double del_T;
    double sens_heat_flux;
    double latent_heat_flux;
    double lw_net;
    size_t i;
    
    sw_in = force.shortwave[NR] * (1 - ALB_WATER);
    dam_wind = force.wind[NR] * log((2. + DAM_ZWATER) / DAM_ZWATER) / log(
                global_param.wind_h / DAM_ZWATER);
    
    Ts = dam_var->temperature[0];
    Tmean = MISSING;
    
    while ((fabs(Tmean - Ts) > epsilon) && i < DAM_MAX_ITER) {
        if(i > 0) {
            Ts = Tmean;
        }
        
        drag = lkdrag(Ts + CONST_TKFRZ, force.air_temp[NR] + CONST_TKFRZ, dam_wind, DAM_ZWATER,
                           global_param.wind_h);
        
        // Based on Handbook of Hydrology eq. 4.2.2
        eog = .611 * exp(17.269 * Ts / (Ts + 237.3));
        
        qlake = CONST_EPS * (eog / (force.pressure[NR] - 0.378 * eog));
        qair = CONST_EPS * (force.vp[NR] / (force.pressure[NR] - 0.378 * force.vp[NR]));
        del_q = qair - qlake;
        
        // Based on Hostetler 1991 and Brutsaert 1982
        evaporation = -1 * drag * dam_wind * force.density[NR] * del_q;        
        del_T = force.air_temp[NR] + CONST_TKFRZ - Ts + CONST_TKFRZ;
        
        sens_heat_flux = drag * dam_wind * force.density[NR] * CONST_CPMAIR;
        sens_heat_flux = sens_heat_flux * del_T;
        
        latent_heat_flux = calc_latent_heat_of_vaporization(force.air_temp[NR]);
        latent_heat_flux = -1. * (evaporation) * latent_heat_flux;
        
        lw_net = force.longwave[NR] - calc_outgoing_longwave(Ts + CONST_TKFRZ, EMISS_WATER);
        
        
        i++;
        
    }
    
    
}