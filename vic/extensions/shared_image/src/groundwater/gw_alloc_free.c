#include <ext_driver_shared_image.h>

void
gw_alloc(void)
{
    extern domain_struct local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern option_struct options;
    extern gw_con_struct *gw_con;
    extern ext_all_vars_struct *ext_all_vars;
    
    size_t i;
    size_t j;
    
    gw_con = malloc(local_domain.ncells_active * sizeof(*gw_con));
    check_alloc_status(gw_con,"Memory allocation error");
        
    for(i=0; i<local_domain.ncells_active; i++){    
        ext_all_vars[i].groundwater = malloc(veg_con_map[i].nv_active * sizeof(*ext_all_vars[i].groundwater));
        check_alloc_status(ext_all_vars[i].groundwater,"Memory allocation error");
        
        for(j=0;j<veg_con_map[i].nv_active;j++){
            ext_all_vars[i].groundwater[j] = malloc(options.SNOW_BAND * sizeof(*ext_all_vars[i].groundwater[j]));
            check_alloc_status(ext_all_vars[i].groundwater[j],"Memory allocation error");
        }
    }   
}

void
gw_finalize(void)
{
    extern domain_struct local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern gw_con_struct *gw_con;
    extern ext_all_vars_struct *ext_all_vars;
    
    size_t i;    
    size_t j;
    
    for(i=0; i<local_domain.ncells_active; i++){        
        for(j=0;j<veg_con_map[i].nv_active;j++){
            free(ext_all_vars[i].groundwater[j]);
        }
        free(ext_all_vars[i].groundwater);
    }
    free(gw_con);
}