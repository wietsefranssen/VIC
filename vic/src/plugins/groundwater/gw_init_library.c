#include <vic.h>

void
initialize_gw_var(gw_var_struct *gw_var)
{
    gw_var->recharge = 0.0;
    gw_var->zwt = 0.0;
    gw_var->Wa = 0.0;
    gw_var->Wt = 0.0;
    gw_var->available = 0.0;
}

void
initialize_gw_con(gw_con_struct *gw_con)
{
    gw_con->Ka_expt = 0.0;
    gw_con->Qb_max = 0.0;
    gw_con->Qb_expt = 0.0;
    gw_con->Sy = 0.0;
    gw_con->Za_max = 0.0;
}

void
initialize_gw_local_structures(void)
{
    extern domain_struct       local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern gw_var_struct    ***gw_var;
    extern gw_con_struct      *gw_con;

    size_t                     i;
    size_t                     j;
    size_t                     k;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_gw_con(&gw_con[i]);

        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            for (k = 0; k < elev_con_map[i].ne_active; k++) {
                initialize_gw_var(&(gw_var[i][j][k]));
            }
        }
    }
}
