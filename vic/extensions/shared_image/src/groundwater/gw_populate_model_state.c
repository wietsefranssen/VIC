#include <ext_driver_shared_image.h>

void
gw_generate_default_state(void)
{
    extern domain_struct local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern option_struct options;
    extern gw_con_struct *gw_con;
    extern ext_all_vars_struct *ext_all_vars;
    
    size_t i;
    size_t j;
    size_t k;
    
    for(i=0; i<local_domain.ncells_active; i++){
        for(j=0; j<veg_con_map[i].nv_active; j++){
            for(k=0; k<options.SNOW_BAND; k++){
                ext_all_vars[i].groundwater[j][k].zwt = 5.0;
                ext_all_vars[i].groundwater[j][k].Wa = 
                        (GW_REF_DEPTH - ext_all_vars[i].groundwater[j][k].zwt) * 
                        gw_con[i].Sy * MM_PER_M;
                ext_all_vars[i].groundwater[j][k].Wt = ext_all_vars[i].groundwater[j][k].Wa;
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