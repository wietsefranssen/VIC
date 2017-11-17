#include <ext_driver_shared_image.h>

void
initialize_gw_local_structures(void)
{
    extern domain_struct local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern option_struct options;
    extern ext_all_vars_struct *ext_all_vars;
    extern gw_con_struct *gw_con;
    
    size_t i;
    size_t j;
    size_t k;
    
    for(i=0; i < local_domain.ncells_active; i++){
        initialize_gw_con(&gw_con[i]);
        
        for(j=0; j < veg_con_map[i].nv_active; j++){
            for(k=0; k < options.SNOW_BAND; k++){
                initialize_gw_var(&(ext_all_vars[i].groundwater[j][k]));
            }
        }
    }
}

void
initialize_gw_var(gw_var_struct *gw_var)
{
    gw_var->Qr = 0.0;
    gw_var->Qb = 0.0;
    gw_var->zwt = 5.0;
    gw_var->Wa = (GW_REF_DEPTH - 5) * 0.5 * MM_PER_M;
    gw_var->Wt = gw_var->Wa;
}

void
initialize_gw_con(gw_con_struct *gw_con)
{
    gw_con->Ka_expt = 0.0;
    gw_con->Qb_max = 0.0;
    gw_con->Qb_expt = 0.0;
    gw_con->Sy = 0.0;
}

void
initialize_gw_parameters(ext_parameters_struct *parameters)
{
}

void
initialize_gw_info(ext_info_struct *info)
{
    strcpy(info->Ka_expt, MISSING_S);
    strcpy(info->Qb_max, MISSING_S);
    strcpy(info->Qb_expt, MISSING_S);
    strcpy(info->Sa, MISSING_S);
}

void
initialize_gw_filenames(ext_filenames_struct *filenames)
{
    initialize_nameid(&filenames->groundwater);
}

void
initialize_gw_options(ext_option_struct *options)
{
    options->GROUNDWATER = false;
}