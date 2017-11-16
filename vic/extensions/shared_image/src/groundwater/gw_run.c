#include <ext_driver_shared_image.h>

void
gw_run(void)
{
    
}

void
groundwater(gw_var_struct *gw_var, soil_con_struct soil_con, gw_con_struct gw_con, 
        double *Kl, double *matric_pot,
        double *dt_baseflow, double *dt_recharge,
        double *liq, double *ice){
    
    int lwt;
    int bot;
    double Ka;
    double Wr;
    double Kl_avg;
    
    size_t i;
    
    // Initialize values
    z_tmp = 0;
    for(i=0;i<options.Nlayer;i++){
        z_tmp += soil_con.depth[i];
        z[i] = z_tmp;
        z_node[i] = z_tmp - 0.5 * soil_con.depth[i];
        eff_porosity[i] = ((soil_con.depth[i] * soil_con.porosity[i]) - ice[i]) /
                soil_con.depth[i];
    }
    
    // Find the layer with the phreatic level
    lwt = -1;
    for(i=0;i<options.Nlayer;i++){
        if(gw_var->zwt >= z[i]){
            lwt=i;
            break;
        }
    }
        
    // Calculate conductivity
    if(lwt == -1){
        // The phreatic level is below the soil column
        bot = options.Nlayer - 1;
        Ka = (Kl[bot] * 
                (1 - exp(-gw_con.Ka_expt * (gw_var->zwt - z_node[bot])))) / 
                (gw_con.Ka_expt * (gw_var->zwt - z_node[bot]));                
    }else{
        // The phreatic level is in the soil column
        if(lwt == 0){
            bot = lwt;
        }else{
            bot = lwt - 1;
        }        
        Ka = Kl[bot];
    }
    
    // Calculate recharge and baseflow
    *dt_recharge = -Ka * 
            (-gw_var->zwt - (matric_pot[bot] - z_node[bot])) / 
            (gw_var->zwt - z_node[bot]);
    
    *dt_baseflow = gw_con->Qb_max * exp(-gw_con->Qb_expt * gw_var->zwt);
    
    // Calculate water storage and water table
    gw_var->Wt += *dt_recharge - *dt_baseflow;    
    if(lwt == -1){
        gw_var->Wa = gw_var->Wt;
        gw_var->zwt = REF_ZWT - gw_var->Wt / gw_con->Sy;
    }else{
        gw_var->Wa = REF_ZWT * gw_con->Sy;
        Wr = gw_var->Wt - gw_var->Wa;
        for(i=options.Nlayer - 1; i>lwt; i--){
            Wr -= soil_con.depth[i] * eff_porosity[i];
        }
        gw_var->zwt = z[lwt] + Wr / eff_porosity[i];
    }
    
    // Calculate new moisture contents    
    if(lwt == -1){
        liq[bot] -= *dt_recharge;
    }else{
        Kl_avg = 0.0;
        for(i = lwt; i < options.Nlayer - 1; i++){
            Kl_avg += Kl[i] * soil_con->depth[i];
        }
        for(i = lwt; i < options.Nlayer - 1; i++){
            liq[i] -= *dt_baseflow * (Kl[i] * soil_con->depth[i] / Kl_avg);
        }
    }
    
}