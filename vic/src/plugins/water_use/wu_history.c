#include <vic.h>

bool
wu_history(int varid, unsigned int *aggtype)
{
    extern node            *outvar_types;
    
    int OUT_WU_DEMAND = list_search_id(outvar_types, "OUT_WU_DEMAND");
    int OUT_WU_WITHDRAWN = list_search_id(outvar_types, "OUT_WU_WITHDRAWN");
    int OUT_WU_CONSUMED = list_search_id(outvar_types, "OUT_WU_CONSUMED");
    int OUT_WU_RETURNED = list_search_id(outvar_types, "OUT_WU_RETURNED");
    
    if(varid == OUT_WU_DEMAND || varid == OUT_WU_WITHDRAWN ||
            varid == OUT_WU_CONSUMED || varid == OUT_WU_RETURNED){
        (*aggtype) = AGG_TYPE_SUM;
        return true;
    }
    
    return false;
}