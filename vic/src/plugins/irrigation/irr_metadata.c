#include <vic.h>

void
irr_set_output_meta_data_info(void)
{    
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_REQUIREMENT")].varname, "OUT_IRR_REQUIREMENT");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_REQUIREMENT")].long_name, "irrigation_requirement");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_REQUIREMENT")].standard_name,
           "irrigation_requirement");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_REQUIREMENT")].units, "mm");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_REQUIREMENT")].description,
           "irrigation_requirement");
    
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_NEED")].varname, "OUT_IRR_NEED");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_NEED")].long_name, "irrigation_need");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_NEED")].standard_name,
           "irrigation_need");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_NEED")].units, "mm");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_NEED")].description,
           "irrigation_need");
    
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_POND_STORAGE")].varname, "OUT_IRR_POND_STORAGE");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_POND_STORAGE")].long_name, "irrigation_pond_storage");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_POND_STORAGE")].standard_name,
           "irrigation_pond_storage");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_POND_STORAGE")].units, "mm");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_POND_STORAGE")].description,
           "irrigation_pond_storage");
    
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_LEFTOVER")].varname, "OUT_IRR_LEFTOVER");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_LEFTOVER")].long_name, "irrigation_leftover");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_LEFTOVER")].standard_name,
           "irrigation_leftover");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_LEFTOVER")].units, "mm");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_LEFTOVER")].description,
           "irrigation_leftover");
    
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_SHORTAGE")].varname, "OUT_IRR_SHORTAGE");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_SHORTAGE")].long_name, "irrigation_shortage");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_SHORTAGE")].standard_name,
           "irrigation_shortage");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_SHORTAGE")].units, "mm");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_IRR_SHORTAGE")].description,
           "irrigation_shortage");
}

void
irr_state_metadata(void)
{
    
}