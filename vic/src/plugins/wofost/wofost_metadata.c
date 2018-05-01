#include <vic.h>

void
wofost_set_output_meta_data_info(void)
{    
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_WOFOST_STEMS")].varname, "OUT_WOFOST_STEMS");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_WOFOST_STEMS")].long_name, "wofost_blabla");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_WOFOST_STEMS")].standard_name,
           "wofost_blabla");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_WOFOST_STEMS")].units, "mm");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_WOFOST_STEMS")].description,
           "wofost_blabla");
}

void
wofost_state_metadata(void)
{
    
}