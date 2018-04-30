#include <vic.h>

void
dam_generate_default_state(void)
{
    extern domain_struct       local_domain;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct    **dam_con;
    extern dam_var_struct    **dam_var;
    extern dmy_struct         *dmy;
    extern size_t              current;

    size_t                     i;
    size_t                     j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < dam_con_map[i].nd_active; j++) {
            dam_var[i][j].volume =
                dam_con[i][j].max_volume * DAM_PREF_VOL_FRAC;
            dam_var[i][j].area =
                dam_area(dam_var[i][j].volume,
                         dam_con[i][j].max_volume,
                         dam_con[i][j].max_area,
                         dam_con[i][j].max_height);
            dam_var[i][j].height =
                dam_height(dam_var[i][j].area,
                           dam_con[i][j].max_height);
            dam_var[i][j].op_year = dmy[current].month;
        }
    }
}
