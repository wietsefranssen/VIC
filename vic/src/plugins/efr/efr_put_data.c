#include <vic.h>

void
efr_put_data(void)
{
    extern domain_struct   local_domain;
    extern efr_var_struct *efr_var;
    extern double       ***out_data;
    extern node           *outvar_types;

    size_t                 i;

    int                    OUT_EFR_REQUIREMENT = list_search_id(
        outvar_types, "OUT_EFR_REQUIREMENT");

    for (i = 0; i < local_domain.ncells_active; i++) {
        out_data[i][OUT_EFR_REQUIREMENT][0] = efr_var[i].requirement;
    }
}
