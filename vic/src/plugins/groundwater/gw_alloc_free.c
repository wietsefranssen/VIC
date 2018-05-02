#include <vic.h>

void
gw_alloc(void)
{
    extern domain_struct       local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern gw_con_struct      *gw_con;
    extern gw_var_struct    ***gw_var;

    size_t                     i;
    size_t                     j;

    gw_var = malloc(local_domain.ncells_active * sizeof(*gw_var));
    check_alloc_status(gw_var, "Memory allocation error");

    gw_con = malloc(local_domain.ncells_active * sizeof(*gw_con));
    check_alloc_status(gw_con, "Memory allocation error");

    for (i = 0; i < local_domain.ncells_active; i++) {
        gw_var[i] = malloc(veg_con_map[i].nv_active * sizeof(*gw_var[i]));
        check_alloc_status(gw_var[i], "Memory allocation error");

        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            gw_var[i][j] = malloc(elev_con_map[i].ne_active * sizeof(*gw_var[i][j]));
            check_alloc_status(gw_var[i][j], "Memory allocation error");
        }
    }
}

void
gw_finalize(void)
{
    extern domain_struct       local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern gw_con_struct      *gw_con;
    extern gw_var_struct    ***gw_var;

    size_t                     i;
    size_t                     j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            free(gw_var[i][j]);
        }
        free(gw_var[i]);
    }
    free(gw_con);
    free(gw_var);
}
