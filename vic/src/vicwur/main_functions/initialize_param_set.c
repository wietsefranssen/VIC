#include <vic.h>

/******************************************************************************
 * @brief    Initialize parameters structure.
 *****************************************************************************/
void
initialize_param_set()
{
    extern param_set_struct param_set;

    size_t                  i;

    for (i = 0; i < N_FORCING_TYPES; i++) {
        param_set.force_steps_per_day[i] = 0;
        param_set.TYPE[i].SUPPLIED = false;
        strcpy(param_set.TYPE[i].varname, "MISSING");
    }
}
