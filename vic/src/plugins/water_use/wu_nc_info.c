#include <vic.h>

bool
wu_set_nc_var_info(int                varid,
                   unsigned short int dtype,
                   nc_file_struct    *nc_file,
                   nc_var_struct     *nc_var)
{
    extern node *outvar_types;

    // set datatype
    nc_var->nc_type = get_nc_dtype(dtype);

    int OUT_WU_DEMAND = list_search_id(outvar_types, "OUT_WU_DEMAND");
    int OUT_WU_WITHDRAWN = list_search_id(outvar_types, "OUT_WU_WITHDRAWN");
    int OUT_WU_CONSUMED = list_search_id(outvar_types, "OUT_WU_CONSUMED");
    int OUT_WU_RETURNED = list_search_id(outvar_types, "OUT_WU_RETURNED");

    if (varid == OUT_WU_DEMAND ||
        varid == OUT_WU_RETURNED ||
        varid == OUT_WU_WITHDRAWN ||
        varid == OUT_WU_CONSUMED) {
        nc_var->nc_dims = 4;
        nc_var->nc_counts[1] = nc_file->sector_size;
        nc_var->nc_counts[2] = nc_file->nj_size;
        nc_var->nc_counts[3] = nc_file->ni_size;
        return true;
    }
    else {
        return false;
    }
}

bool
wu_set_nc_var_dimids(int             varid,
                     nc_file_struct *nc_file,
                     nc_var_struct  *nc_var)
{
    extern node *outvar_types;

    int          OUT_WU_DEMAND = list_search_id(outvar_types, "OUT_WU_DEMAND");
    int          OUT_WU_WITHDRAWN = list_search_id(outvar_types,
                                                   "OUT_WU_WITHDRAWN");
    int          OUT_WU_CONSUMED = list_search_id(outvar_types,
                                                  "OUT_WU_CONSUMED");
    int          OUT_WU_RETURNED = list_search_id(outvar_types,
                                                  "OUT_WU_RETURNED");

    if (varid == OUT_WU_DEMAND ||
        varid == OUT_WU_RETURNED ||
        varid == OUT_WU_WITHDRAWN ||
        varid == OUT_WU_CONSUMED) {
        nc_var->nc_dimids[0] = nc_file->time_dimid;
        nc_var->nc_dimids[1] = nc_file->sector_dimid;
        nc_var->nc_dimids[2] = nc_file->nj_dimid;
        nc_var->nc_dimids[3] = nc_file->ni_dimid;
        return true;
    }
    else {
        return false;
    }
}
