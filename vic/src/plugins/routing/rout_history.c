#include <vic.h>

bool
rout_history(int           varid,
             unsigned int *aggtype)
{
    extern node *outvar_types;

    int          OUT_STREAM_MOIST = list_search_id(outvar_types,
                                                   "OUT_STREAM_MOIST");

    if (varid == OUT_STREAM_MOIST) {
        (*aggtype) = AGG_TYPE_END;
        return true;
    }

    return false;
}
