#include <ext_driver_shared_image.h>

void
gw_generate_default_state(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern veg_con_map_struct *veg_con_map;
    extern soil_con_struct *soil_con;
    extern option_struct options;
    extern ext_all_vars_struct *ext_all_vars;
    extern all_vars_struct *all_vars;
    extern gw_con_struct *gw_con;
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    size_t m;
    size_t n;
    
    double                    *dvar = NULL;    
    size_t                     d4count[4];
    size_t                     d4start[4];
    int status;
    
    double z_tmp;
    double ice;
    double eff_porosity;
    bool in_column;
    
    if(ext_options.GW_INIT_FROM_FILE){

        dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
        check_alloc_status(dvar, "Memory allocation error.");
        
        d4start[0] = 0;
        d4start[1] = 0;
        d4start[2] = 0;
        d4start[3] = 0;
        d4count[0] = 1;
        d4count[1] = 1;
        d4count[2] = global_domain.n_ny;
        d4count[3] = global_domain.n_nx;

        // open parameter file
        status = nc_open(ext_filenames.groundwater.nc_filename, NC_NOWRITE,
                         &(ext_filenames.groundwater.nc_id));
        check_nc_status(status, "Error opening %s",
                        ext_filenames.groundwater.nc_filename);
    
        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            d4start[0] = j;
            for (k = 0; k < options.SNOW_BAND; k++) {
                d4start[1] = k;
                get_scatter_nc_field_double(&(ext_filenames.groundwater),
                        ext_filenames.info.zwt_init, d4start, d4count, dvar);
                for (i = 0; i < local_domain.ncells_active; i++) {
                    ext_all_vars[i].groundwater[j][k].zwt = (double) dvar[i];
                }
            }
        }
        
        // close parameter file
        status = nc_close(ext_filenames.groundwater.nc_id);
        check_nc_status(status, "Error closing %s",
                    ext_filenames.groundwater.nc_filename);
        
    }else{
        for(i=0; i<local_domain.ncells_active; i++){
            for(j=0; j<veg_con_map[i].nv_active; j++){
                for(k=0; k<options.SNOW_BAND; k++){
                    ext_all_vars[i].groundwater[j][k].zwt = GW_DEF_ZWT_INIT;
                }                
            }
        }
    }
    
    in_column = false;
    for(i=0; i<local_domain.ncells_active; i++){ 
        for(j=0; j<veg_con_map[i].nv_active; j++){
            for(k=0; k<options.SNOW_BAND; k++){   
                z_tmp = 0.0;    
                for(l=0; l<options.Nlayer; l++){
                    z_tmp += soil_con[i].depth[l];
                    if(ext_all_vars[i].groundwater[j][k].zwt < z_tmp){
                        // groundwater table is in layer
                        
                        // add water for current layer
                        ice = 0.0;
                        for(m=0; m<options.Nfrost; m++){
                            ice += all_vars[i].cell[j][k].layer[l].ice[m];
                        }                        
                        eff_porosity = (soil_con[i].max_moist[l] - ice) / 
                                soil_con[i].depth[l];
                        
                        ext_all_vars[i].groundwater[j][k].Wt +=
                                z_tmp - ext_all_vars[i].groundwater[j][k].zwt * 
                                eff_porosity * MM_PER_M;
                        
                        // add water for lower layers
                        for(n=l + 1; n<options.Nlayer; n++){  
                            z_tmp += soil_con[i].depth[n];
                            
                            ice = 0.0;
                            for(m=0; m<options.Nfrost; m++){
                                ice += all_vars[i].cell[j][k].layer[l].ice[m];
                            }                        
                            eff_porosity = (soil_con[i].max_moist[l] - ice) / 
                                    soil_con[i].depth[l];

                            ext_all_vars[i].groundwater[j][k].Wt +=
                                    soil_con[i].depth[n] * 
                                    eff_porosity * MM_PER_M;
                        }
                        
                        // add water for aquifer
                        ext_all_vars[i].groundwater[j][k].Wa = 
                                (GW_REF_DEPTH - z_tmp) * 
                                gw_con[i].Sy * MM_PER_M;
                        ext_all_vars[i].groundwater[j][k].Wt +=
                                ext_all_vars[i].groundwater[j][k].Wa;
                        
                        in_column = true;
                        break;
                    }
                }
                if(!in_column){
                    ext_all_vars[i].groundwater[j][k].Wt = 
                            (GW_REF_DEPTH - ext_all_vars[i].groundwater[j][k].zwt) * 
                            gw_con[i].Sy * MM_PER_M;
                    ext_all_vars[i].groundwater[j][k].Wa = 
                            ext_all_vars[i].groundwater[j][k].Wt;
                }
            }
        }
    }
}

void
gw_restore(void)
{
//    extern domain_struct global_domain;
//    extern domain_struct local_domain;
//    
//    double                    *dvar = NULL;
//    int                       *ivar = NULL;
//    
//    size_t                     d3count[3];
//    size_t                     d3start[3];
//    size_t                     d4count[4];
//    size_t                     d4start[4];
//    
//    d3start[0] = 0;
//    d3start[1] = 0;
//    d3start[2] = 0;
//    d3count[0] = 1;
//    d3count[1] = global_domain.n_ny;
//    d3count[2] = global_domain.n_nx;
//    
//    d4start[0] = 0;
//    d4start[1] = 0;
//    d4start[2] = 0;
//    d4start[3] = 0;
//    d4count[0] = 1;
//    d4count[1] = 1;
//    d4count[2] = global_domain.n_ny;
//    d4count[3] = global_domain.n_nx;
//        
//    // Allocate
//    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
//    check_alloc_status(dvar, "Memory allocation error");
//    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
//    check_alloc_status(ivar, "Memory allocation error");
//    
//    // Read variables from state file
//    
//    
//    // Free
//    free(dvar);
//    free(ivar);
}