#include <vic.h>

void
gw_put_data(void)
{
    extern domain_struct       local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern elev_con_map_struct *elev_con_map;
    extern soil_con_struct    *soil_con;
    extern veg_con_struct    **veg_con;
    extern gw_var_struct    ***gw_var;
    extern double           ***out_data;
    extern node               *outvar_types;

    size_t                     i;
    size_t                     j;
    size_t                     k;

    double                     veg_frac;
    double                     snow_frac;

    int                        OUT_GW_ZWT = list_search_id(outvar_types,
                                                           "OUT_GW_ZWT");
    int                        OUT_GW_RECHARGE = list_search_id(
        outvar_types, "OUT_GW_RECHARGE");
    int                        OUT_GW_WA = list_search_id(outvar_types,
                                                          "OUT_GW_WA");
    int                        OUT_GW_WT = list_search_id(outvar_types,
                                                          "OUT_GW_WT");
    int                        OUT_GW_AVAIL = list_search_id(outvar_types,
                                                             "OUT_GW_AVAIL");

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < veg_con_map[i].nv_active; j++) {
            veg_frac = veg_con[i][j].Cv;

            for (k = 0; k < elev_con_map[i].ne_active; k++) {
                snow_frac = soil_con[i].AreaFract[k];

                out_data[i][OUT_GW_ZWT][0] += gw_var[i][j][k].zwt *
                                              veg_frac * snow_frac;
                out_data[i][OUT_GW_RECHARGE][0] += gw_var[i][j][k].recharge *
                                                   veg_frac * snow_frac;
                out_data[i][OUT_GW_WA][0] += gw_var[i][j][k].Wa *
                                             veg_frac * snow_frac;
                out_data[i][OUT_GW_WT][0] += gw_var[i][j][k].Wt *
                                             veg_frac * snow_frac;
                out_data[i][OUT_GW_AVAIL][0] += gw_var[i][j][k].available *
                                                veg_frac * snow_frac;
            }
        }
    }
}
