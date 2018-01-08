#include <ext_driver_shared_image.h>

void
irr_run1(size_t cur_cell)
{
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern all_vars_struct *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern ext_all_vars_struct *ext_all_vars;
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern wu_con_struct **wu_con;
    extern option_struct options;
    
    double moist[options.Nlayer];    
    double irrigation_need;
    double demand;
    size_t root_layer;
    size_t cur_veg;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;       
            
    wu_con[cur_cell][WU_IRRIGATION].demand = 0;
            
    for(i = 0; i < irr_con_map[cur_cell].ni_active; i++){
        cur_veg = irr_con_map[cur_cell].vidx[i];
        
        for(j = 0; j < options.SNOW_BAND; j++){
            
            // Get moisture content of every layer
            for(k = 0; k < options.Nlayer; k++){
                moist[k] = 0.0;
                for (l = 0; l < options.Nfrost; l++) {
                    moist[k] += ((all_vars[cur_cell].cell[cur_veg][j].layer[k].moist -
                      all_vars[cur_cell].cell[cur_veg][j].layer[k].ice[l])
                            * soil_con[cur_cell].frost_fract[l]);
                }
            }
            
            // Get irrigation need and demand
            irrigation_need = 0.0;
            demand = 0.0;
            if(options.SHARE_LAYER_MOIST){
                /******************************************************************
                   CASE 1: Moisture in layers exceeds Wcr, or Moisture in
                   layer with more than half of the roots exceeds Wcr.

                   Potential evapotranspiration not hindered by soil dryness.  If
                   layer with less than half the roots is dryer than Wcr, extra
                   evaporation is taken from the wetter layer. Otherwise layers
                   contribute to evapotransipration based on root fraction.
                ******************************************************************/                
                // Get major root layer
                root_layer = 0;
                for(k = 0; k < options.Nlayer; k++){
                    if(veg_con[cur_cell][cur_veg].root[k] >= 0.5){
                        root_layer = k;
                        break;
                    }
                }
                
                if(moist[root_layer] > soil_con[cur_cell].Wcr[root_layer] / 
                        IRRIGATION_FRAC || moist[root_layer] > 
                    ext_all_vars[cur_cell].irrigation[i][j].prev_root_moist){
                    // moisture content is above critical OR
                    // moisture content is below critical, but increasing           
                    
                    ext_all_vars[cur_cell].irrigation[i][j].prev_demand -=
                            moist[root_layer] - 
                            ext_all_vars[cur_cell].irrigation[i][j].prev_root_moist;
                    
                    if(ext_all_vars[cur_cell].irrigation[i][j].prev_demand < 0){
                        ext_all_vars[cur_cell].irrigation[i][j].prev_demand = 0.0;
                    }
                } else {
                    // moisture content is below critical, and decreasing
                    
                    irrigation_need = (soil_con[cur_cell].Wcr[root_layer] / FIELD_CAP_FRAC) - 
                            all_vars[cur_cell].cell[cur_veg][j].layer[root_layer].moist;
                    demand = irrigation_need - 
                            ext_all_vars[cur_cell].irrigation[i][j].prev_demand; 
                    
                    if(demand < 0){
                        demand = 0.0;
                    }
                    
                    ext_all_vars[cur_cell].irrigation[i][j].prev_demand +=
                            demand;                   
                } 
                
                ext_all_vars[cur_cell].irrigation[i][j].prev_root_moist = 
                        moist[root_layer];
            } else {
                log_err("Irrigation without sharing layers not implemented yet");
            }
            
            wu_con[cur_cell][WU_IRRIGATION].demand += demand / MM_PER_M *
                    local_domain.locations[cur_cell].area * 
                    veg_con[cur_cell][cur_veg].Cv / global_param.dt;
        }        
    }
}

void
irr_run2(size_t cur_cell)
{
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern all_vars_struct *all_vars;
    extern irr_con_map_struct *irr_con_map;
    extern ext_all_vars_struct *ext_all_vars;
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
            
            if(ext_all_vars[cur_cell].irrigation[i][j].leftover > 0){
                // Leftover water for irrigation
                
                irrigation_need = soil_con[cur_cell].max_moist[0] - 
                       all_vars[cur_cell].cell[cur_veg][j].layer[0].moist;
                
                if(ext_all_vars[cur_cell].irrigation[i][j].leftover >
                        irrigation_need){
                    // Leftover too much, add to leftover
                    all_vars[cur_cell].cell[cur_veg][j].layer[0].moist = 
                            soil_con[cur_cell].max_moist[0];
                    ext_all_vars[cur_cell].irrigation[i][j].leftover =
                            ext_all_vars[cur_cell].irrigation[i][j].leftover -
                            irrigation_need;
                }else{
                    // Leftover too little, fill up
                    all_vars[cur_cell].cell[cur_veg][j].layer[0].moist += 
                            ext_all_vars[cur_cell].irrigation[i][j].leftover;
                    ext_all_vars[cur_cell].irrigation[i][j].leftover = 0;
                }                
            }
            
            irrigation_available = 
                    ext_all_vars[cur_cell].water_use[WU_IRRIGATION].consumed *
                    global_param.dt / local_domain.locations[cur_cell].area /
                    veg_con[cur_cell][cur_veg].Cv * MM_PER_M;
            
            if(irrigation_available > 0){                
                // Received water for irrigation   
                
                irrigation_need = soil_con[cur_cell].max_moist[0] - 
                       all_vars[cur_cell].cell[cur_veg][j].layer[0].moist;
                
                if(irrigation_available > irrigation_need){
                    // Received too much, add to leftover
                    all_vars[cur_cell].cell[cur_veg][j].layer[0].moist = 
                            soil_con[cur_cell].max_moist[0];
                    ext_all_vars[cur_cell].irrigation[i][j].leftover =
                            irrigation_available -
                            irrigation_need;
                }else{
                    // Received too little, fill up
                    all_vars[cur_cell].cell[cur_veg][j].layer[0].moist += 
                            irrigation_available;
                }
            }    
        }
    }
}

// Backup                
//                /*********************************************************************
//                   CASE 2: Independent evapotranspirations
//
//                   Evapotranspiration is restricted by low soil moisture. Evaporation
//                   is computed independantly from each soil layer.
//                *********************************************************************/
//                // Get irrigation need and demand
//                for(k = 0; k < options.Nlayer; k++){
//                    if(moist[k] > soil_con[cur_cell].Wcr[k] / IRRIGATION_FRAC){
//                        // moisture content is above critical
//                        
//                        ext_all_vars[cur_cell].irrigation[i][j].prev_demand = 
//                                0.0;
//                    }else if(moist[k] > 
//                        ext_all_vars[cur_cell].irrigation[i][j].prev_root_moist){
//                        // moisture content is below critical, but increasing
//                        
//                        ext_all_vars[cur_cell].irrigation[i][j].prev_demand -=
//                                moist[k] - 
//                                ext_all_vars[cur_cell].irrigation[i][j].prev_root_moist;
//                    } else {
//                        // moisture content is below critical, and decreasing
//                        
//                        irrigation_need = (soil_con[cur_cell].Wcr[0] / FIELD_CAP_FRAC) - 
//                                all_vars[cur_cell].cell[cur_veg][j].layer[0].moist;
//                        demand = irrigation_need - 
//                                ext_all_vars[cur_cell].irrigation[i][j].prev_demand;                    
//                        ext_all_vars[cur_cell].irrigation[i][j].prev_demand =
//                                demand;                   
//                    } 
//
//                    ext_all_vars[cur_cell].irrigation[i][j].prev_root_moist = 
//                            moist[k];
//                }                
            