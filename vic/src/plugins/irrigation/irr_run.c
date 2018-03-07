#include <vic.h>

void
irr_run1(size_t cur_cell)
{
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern dmy_struct *dmy;
    extern size_t current;
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern wu_con_struct **wu_con;
    extern option_struct options;
    
    double moist[options.Nlayer];
    double total_demand;
    double season_day;
    size_t root_layer;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;       
            
    total_demand = 0;            
    for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
        cur_veg = irr_con[cur_cell][i].veg_index;
        
        // Reset values
        for(j = 0; j < options.SNOW_BAND; j++){     
            irr_var[cur_cell][i][j].need = 0.0;
            irr_var[cur_cell][i][j].shortage = false;
        }
        
        // Check irrigation season
        for(j = 0; j < irr_con[cur_cell][i].nseasons; j++){
            season_day = between_jday(irr_con[cur_cell][i].season_start[j],
                irr_con[cur_cell][i].season_end[j],dmy[current].day_in_year);
            
            if(season_day > 0){
                break;
            }
        }        
        if(season_day <= 0.0){
            for(j = 0; j < options.SNOW_BAND; j++){  
                irr_var[cur_cell][i][j].requirement = 0.0;
                all_vars[cur_cell].cell[cur_veg][j].layer[0].Ksat = soil_con[cur_cell].Ksat[0];

                irr_var[cur_cell][i][j].prev_store = 
                    irr_var[cur_cell][i][j].pond_storage; 
                irr_var[cur_cell][i][j].prev_req = 
                    irr_var[cur_cell][i][j].requirement; 
            }

            continue;
        }
        
        // Run irrigated vegetation
        for(j = 0; j < options.SNOW_BAND; j++){
            
            // Get irrigation need and demand         
            if(irr_con[cur_cell][i].ponding){
                if(irr_var[cur_cell][i][j].pond_storage > 
                        irr_con[cur_cell][j].pond_capacity / POND_IRR_CRIT_FRAC || 
                        irr_var[cur_cell][i][j].pond_storage > 
                        irr_var[cur_cell][i][j].prev_store){
                    // moisture content is above critical OR
                    // moisture content is below critical, but increasing           

                    irr_var[cur_cell][i][j].prev_req -=
                            irr_var[cur_cell][i][j].pond_storage - 
                            irr_var[cur_cell][i][j].prev_store;

                    if(irr_var[cur_cell][i][j].prev_req < 0){
                        irr_var[cur_cell][i][j].prev_req = 0.0;
                    }
                } else {
                    // moisture content is below critical, and decreasing

                    irr_var[cur_cell][i][j].need = 
                            irr_con[cur_cell][j].pond_capacity - 
                            irr_var[cur_cell][i][j].pond_storage;
                    irr_var[cur_cell][i][j].requirement = 
                            irr_var[cur_cell][i][j].need - 
                            irr_var[cur_cell][i][j].prev_req; 

                    if(irr_var[cur_cell][i][j].requirement < 0){
                        irr_var[cur_cell][i][j].requirement = 0.0;
                    }

                    irr_var[cur_cell][i][j].prev_req +=
                            irr_var[cur_cell][i][j].requirement;                   
                } 
            }else{
                if(options.SHARE_LAYER_MOIST){
                    /******************************************************************
                       CASE 1: Moisture in layers exceeds Wcr, or Moisture in
                       layer with more than half of the roots exceeds Wcr.

                       Potential evapotranspiration not hindered by soil dryness.  If
                       layer with less than half the roots is dryer than Wcr, extra
                       evaporation is taken from the wetter layer. Otherwise layers
                       contribute to evapotransipration based on root fraction.
                    ******************************************************************/                
                    // Get moisture content of every layer
                    for(k = 0; k < options.Nlayer; k++){
                        moist[k] = 0.0;
                        for (l = 0; l < options.Nfrost; l++) {
                            moist[k] += ((all_vars[cur_cell].cell[cur_veg][j].layer[k].moist -
                              all_vars[cur_cell].cell[cur_veg][j].layer[k].ice[l])
                                    * soil_con[cur_cell].frost_fract[l]);
                        }
                    }
                    
                    // Get major root layer
                    root_layer = 0;
                    for(k = 0; k < options.Nlayer; k++){
                        if(veg_con[cur_cell][cur_veg].root[k] >= 0.5){
                            root_layer = k;
                            break;
                        }
                    }

                    if(moist[root_layer] > soil_con[cur_cell].Wcr[root_layer] / 
                            IRR_CRIT_FRAC || moist[root_layer] > 
                        irr_var[cur_cell][i][j].prev_store){
                        // moisture content is above critical OR
                        // moisture content is below critical, but increasing           

                        irr_var[cur_cell][i][j].prev_req -=
                                moist[root_layer] - 
                                irr_var[cur_cell][i][j].prev_store;

                        if(irr_var[cur_cell][i][j].prev_req < 0){
                            irr_var[cur_cell][i][j].prev_req = 0.0;
                        }
                    } else {
                        // moisture content is below critical, and decreasing

                        irr_var[cur_cell][i][j].need = 
                                (soil_con[cur_cell].Wcr[root_layer] / FIELD_CAP_FRAC) - 
                                all_vars[cur_cell].cell[cur_veg][j].layer[root_layer].moist;
                        irr_var[cur_cell][i][j].requirement = 
                                irr_var[cur_cell][i][j].need - 
                                irr_var[cur_cell][i][j].prev_req; 

                        if(irr_var[cur_cell][i][j].requirement < 0){
                            irr_var[cur_cell][i][j].requirement = 0.0;
                        }

                        irr_var[cur_cell][i][j].prev_req +=
                                irr_var[cur_cell][i][j].requirement;                   
                    } 

                    irr_var[cur_cell][i][j].prev_store = 
                            moist[root_layer];
                } else {
                    log_err("Irrigation without sharing layers not implemented yet");
                }
            }
        }        
    }
                
    if(options.WATER_USE){
        
        total_demand = 0;            
        for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
            cur_veg = irr_con[cur_cell][i].veg_index;

            for(j = 0; j < options.SNOW_BAND; j++){
		    total_demand += irr_var[cur_cell][i][j].requirement *
			    soil_con[cur_cell].AreaFract[j] * 
			    veg_con[cur_cell][cur_veg].Cv;
		}
	}

        wu_con[cur_cell][WU_IRRIGATION].demand = total_demand / 
                    MM_PER_M * local_domain.locations[cur_cell].area / 
                    global_param.dt;
        wu_con[cur_cell][WU_IRRIGATION].consumption_fraction = 1.0; 
    }
}

void
irr_run2(size_t cur_cell)
{
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern all_vars_struct *all_vars;
    extern option_struct options;
    extern irr_con_map_struct *irr_con_map;
    extern irr_con_struct **irr_con;
    extern irr_var_struct ***irr_var;
    extern wu_var_struct **wu_var;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern option_struct options;
    
    double irrigation_available;
    double irrigation_need;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    
    for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
        cur_veg = irr_con_map[cur_cell].vidx[i];
        
        for(j = 0; j < options.SNOW_BAND; j++){    
            
            if(irr_var[cur_cell][i][j].leftover > 0){
                // Leftover water for irrigation
                
                if(irr_con[cur_cell][i].ponding){
                    irrigation_need = irr_con[cur_cell][i].pond_capacity - 
                           irr_var[cur_cell][i][j].pond_storage;

                    if(irr_var[cur_cell][i][j].leftover >
                            irrigation_need){
                        // Leftover too much
                        irr_var[cur_cell][i][j].pond_storage = 
                                irr_con[cur_cell][i].pond_capacity;
                        irr_var[cur_cell][i][j].leftover -=
                                irrigation_need;
                    }else{
                        // Leftover too little
                        irr_var[cur_cell][i][j].pond_storage += 
                                irr_var[cur_cell][i][j].leftover;
                        irr_var[cur_cell][i][j].leftover = 0;
                    }                      
                }else{
                    irrigation_need = soil_con[cur_cell].max_moist[0] - 
                           all_vars[cur_cell].cell[cur_veg][j].layer[0].moist;

                    if(irr_var[cur_cell][i][j].leftover >
                            irrigation_need){
                        // Leftover too much
                        all_vars[cur_cell].cell[cur_veg][j].layer[0].moist = 
                                soil_con[cur_cell].max_moist[0];
                        irr_var[cur_cell][i][j].leftover -=
                                irrigation_need;
                    }else{
                        // Leftover too little
                        all_vars[cur_cell].cell[cur_veg][j].layer[0].moist += 
                                irr_var[cur_cell][i][j].leftover;
                        irr_var[cur_cell][i][j].leftover = 0.0;
                    }         
                }
            }
            
            if(irr_var[cur_cell][i][j].pond_storage > 0){
                // Pond water for irrigation
                
                irrigation_need = soil_con[cur_cell].max_moist[0] - 
                        all_vars[cur_cell].cell[cur_veg][j].layer[0].moist;

                 if(irr_var[cur_cell][i][j].pond_storage >
                         irrigation_need){
                     // Pond storage too much
                     all_vars[cur_cell].cell[cur_veg][j].layer[0].moist = 
                             soil_con[cur_cell].max_moist[0];
                     irr_var[cur_cell][i][j].pond_storage -=
                             irrigation_need;
                 }else{
                     // Pond storage too little
                     all_vars[cur_cell].cell[cur_veg][j].layer[0].moist += 
                             irr_var[cur_cell][i][j].pond_storage;
                     irr_var[cur_cell][i][j].pond_storage = 0.0;
                 }
            }
                        
            if(options.WATER_USE){
                irrigation_available = 
                        wu_var[cur_cell][WU_IRRIGATION].consumed *
                        global_param.dt / local_domain.locations[cur_cell].area /
                        veg_con[cur_cell][cur_veg].Cv * MM_PER_M;

                if(irrigation_available > 0){                
                    // Received water for irrigation   

                    if(irr_con[cur_cell][i].ponding){
                        irrigation_need = irr_con[cur_cell][i].pond_capacity - 
                               irr_var[cur_cell][i][j].pond_storage;

                        if(irrigation_available > irrigation_need){
                            // Received too much
                            irr_var[cur_cell][i][j].pond_storage = 
                                    irr_con[cur_cell][i].pond_capacity;
                            irr_var[cur_cell][i][j].leftover =
                                    irrigation_available -
                                    irrigation_need;
                        }else{
                            // Received too little
                            irr_var[cur_cell][i][j].pond_storage += 
                                    irrigation_available;
                        }
                    }else{
                        irrigation_need = soil_con[cur_cell].max_moist[0] - 
                               all_vars[cur_cell].cell[cur_veg][j].layer[0].moist;

                        if(irrigation_available > irrigation_need){
                            // Received too much
                            all_vars[cur_cell].cell[cur_veg][j].layer[0].moist = 
                                    soil_con[cur_cell].max_moist[0];
                            irr_var[cur_cell][i][j].leftover =
                                    irrigation_available -
                                    irrigation_need;
                        }else{
                            // Received too little
                            all_vars[cur_cell].cell[cur_veg][j].layer[0].moist += 
                                    irrigation_available;
                            irrigation_available = 0.0;
                        }
                    }
                }
            }
        }
    }
}            
            