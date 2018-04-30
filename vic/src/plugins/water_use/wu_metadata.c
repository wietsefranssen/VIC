#include <vic.h>

void
wu_set_output_meta_data_info(void)
{
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;

    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_DEMAND")].varname,
           "OUT_WU_DEMAND");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_DEMAND")].long_name,
           "sectoral_demand");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_DEMAND")].standard_name,
           "sectoral_demand");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_DEMAND")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_DEMAND")].description,
           "sectoral_demand");

    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_WITHDRAWN")].varname,
           "OUT_WU_WITHDRAWN");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_WITHDRAWN")].long_name,
           "sectoral_withdrawals");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_WITHDRAWN")].standard_name,
           "sectoral_withdrawals");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_WITHDRAWN")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_WITHDRAWN")].description,
           "sectoral_withdrawals");

    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_CONSUMED")].varname,
           "OUT_WU_CONSUMED");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_CONSUMED")].long_name,
           "sectoral_consumption");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_CONSUMED")].standard_name,
           "sectoral_consumption");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_CONSUMED")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_CONSUMED")].description,
           "sectoral_consumption");

    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_RETURNED")].varname,
           "OUT_WU_RETURNED");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_RETURNED")].long_name,
           "sectoral_return_flow");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_RETURNED")].standard_name,
           "sectoral_return_flow");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_RETURNED")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_WU_RETURNED")].description,
           "sectoral_return_flow");

    out_metadata[list_search_id(outvar_types,
                                "OUT_WU_DEMAND")].nelem = WU_NSECTORS;
    out_metadata[list_search_id(outvar_types,
                                "OUT_WU_WITHDRAWN")].nelem = WU_NSECTORS;
    out_metadata[list_search_id(outvar_types,
                                "OUT_WU_CONSUMED")].nelem = WU_NSECTORS;
    out_metadata[list_search_id(outvar_types,
                                "OUT_WU_RETURNED")].nelem = WU_NSECTORS;
}

void
wu_state_metadata(void)
{
}
