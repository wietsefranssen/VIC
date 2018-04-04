#include <vic.h>

bool
irr_history(int varid, unsigned int *aggtype)
{
    extern node            *outvar_types;
        
    int OUT_IRR_REQUIREMENT = list_search_id(outvar_types, "OUT_IRR_REQUIREMENT");
    int OUT_IRR_NEED = list_search_id(outvar_types, "OUT_IRR_NEED");
    int OUT_IRR_LEFTOVER = list_search_id(outvar_types, "OUT_IRR_LEFTOVER");
    int OUT_IRR_POND_STORAGE = list_search_id(outvar_types, "OUT_IRR_POND_STORAGE");
    int OUT_IRR_SHORTAGE = list_search_id(outvar_types, "OUT_IRR_SHORTAGE");
    
    if(varid == OUT_IRR_NEED || varid == OUT_IRR_SHORTAGE){
        (*aggtype) = AGG_TYPE_SUM;
        return true;
    } else if(varid == OUT_IRR_REQUIREMENT || varid == OUT_IRR_LEFTOVER ||
            varid == OUT_IRR_POND_STORAGE){
        (*aggtype) = AGG_TYPE_END;
        return true;
    }
    
    return false;
}