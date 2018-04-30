#include <vic.h>

void
efr_set_output_meta_data_info(void)
{
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;

    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_EFR_REQUIREMENT")].varname,
           "OUT_EFR_REQUIREMENT");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_EFR_REQUIREMENT")].long_name,
           "environmental_flow_requirement");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_EFR_REQUIREMENT")].standard_name,
           "environmental_flow_requirement");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_EFR_REQUIREMENT")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_EFR_REQUIREMENT")].description,
           "environmental_flow_requirement");
}

void
efr_state_metadata(void)
{
}
