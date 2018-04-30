#include <vic.h>

bool
gw_history(int           varid,
           unsigned int *aggtype)
{
    extern node *outvar_types;

    int          OUT_GW_ZWT = list_search_id(outvar_types, "OUT_GW_ZWT");
    int          OUT_GW_RECHARGE = list_search_id(outvar_types,
                                                  "OUT_GW_RECHARGE");
    int          OUT_GW_WA = list_search_id(outvar_types, "OUT_GW_WA");
    int          OUT_GW_WT = list_search_id(outvar_types, "OUT_GW_WT");
    int          OUT_GW_AVAIL = list_search_id(outvar_types, "OUT_GW_AVAIL");

    if (varid == OUT_GW_RECHARGE) {
        (*aggtype) = AGG_TYPE_SUM;
        return true;
    }
    else if (varid == OUT_GW_WT || varid == OUT_GW_WA ||
             varid == OUT_GW_ZWT || varid == OUT_GW_AVAIL) {
        (*aggtype) = AGG_TYPE_END;
        return true;
    }

    return false;
}
