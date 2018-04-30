#include <vic.h>

bool
irr_history(int           varid,
            unsigned int *aggtype)
{
    extern node *outvar_types;

    int          OUT_IRR_NEED = list_search_id(outvar_types, "OUT_IRR_NEED");
    int          OUT_IRR_DEFICIT = list_search_id(outvar_types,
                                                  "OUT_IRR_DEFICIT");

    if (varid == OUT_IRR_NEED || varid == OUT_IRR_DEFICIT) {
        (*aggtype) = AGG_TYPE_SUM;
        return true;
    }

    return false;
}
