#include <vic.h>

void
dam_set_output_meta_data_info(void)
{    
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;
    extern option_struct    options;
    
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].varname, "OUT_DAM_VOLUME");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].long_name, "dam_water_volume");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].standard_name,
           "dam_water_volume");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].units, "km3");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].description,
           "dam_water_volume");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].varname, "OUT_DAM_DISCHARGE");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].long_name, "dam_discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].standard_name,
           "dam_discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].description,
           "dam_discharge");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].varname, "OUT_DAM_AREA");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].long_name, "dam_water_area");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].standard_name,
           "dam_water_area");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].units, "km2");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].description,
           "dam_water_area");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].varname, "OUT_DAM_HEIGHT");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].long_name, "dam_water_height");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].standard_name,
           "dam_water_height");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].units, "m");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].description,
           "dam_water_height");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].varname, "OUT_DAM_OP_DISCHARGE");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].long_name, "dam_operational_discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].standard_name,
           "dam_operational_discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].description,
           "dam_operational_discharge");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].varname, "OUT_DAM_OP_VOLUME");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].long_name, "dam_operational_volume");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].standard_name,
           "dam_operational_volume");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].units, "km3");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].description,
           "dam_operational_volume");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].varname, "OUT_DAM_OP_MONTH");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].long_name, "dam_operational_month");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].standard_name,
           "dam_operational_month");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].units, "#");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].description,
           "dam_operational_month");

    out_metadata[list_search_id(outvar_types, "OUT_DAM_VOLUME")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_DISCHARGE")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_AREA")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_HEIGHT")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_DISCHARGE")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_VOLUME")].nelem = options.MAXDAMS;
    out_metadata[list_search_id(outvar_types, "OUT_DAM_OP_MONTH")].nelem = options.MAXDAMS;
}

void
dam_state_metadata(void)
{
    
}