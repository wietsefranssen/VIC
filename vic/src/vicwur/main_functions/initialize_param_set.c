
#include <vic.h>

/******************************************************************************
 * @brief    Initialize parameters structure.
 *****************************************************************************/
void
initialize_param_set()
{
    extern param_set_struct param_set;
    
    size_t i;
    
    for(i = 0; i < N_FORCING_TYPES; i++){
        param_set.FORCE_DT[i] = 0;
        param_set.FORCE_ENDIAN[i] = 0;
        param_set.FORCE_FORMAT[i] = BINARY;
        param_set.FORCE_INDEX[i][0] = 0;
        param_set.N_TYPES[i] = 0;
        param_set.force_steps_per_day[i] = 0;
        param_set.TYPE[i].N_ELEM = 0;
        param_set.TYPE[i].SIGNED = false;
        param_set.TYPE[i].SUPPLIED = false;
        param_set.TYPE[i].multiplier = 0.0;
        strcpy(param_set.TYPE[i].varname, "MISSING");
    }
}