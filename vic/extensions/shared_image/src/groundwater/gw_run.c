#include <ext_driver_shared_image.h>

void
gw_run(void)
{
    
}

void
groundwater(gw_var_struct *gw_var, soil_con_struct *soil_con){
    
    double z[MAX_LAYERS];
    double z_node[MAX_LAYERS];
    double z_tmp;    
    int lwt;
    
    double ice_frac[MAX_LAYERS];
    double frozen_frac[MAX_LAYERS];
    
    size_t i;
    
    // Set layer and node depths
    z_tmp = 0.0;
    for(i=0; i < options.Nlayer; i++){
        z_tmp += soil_con->depth[i];
        z[i] = z_tmp;
        z_node[i] = z_tmp - (0.5 * soil_con->depth[i]);
    }
    
    // Find layer with phreatic water level    
    for(i=0; i < options.Nlayer; i++){
        if(gw_var->zwt >= z[i]){
            lwt = i;
            break;
        }
    }
    
    
    
    
}