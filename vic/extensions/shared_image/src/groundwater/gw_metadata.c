#include <ext_driver_shared_image.h>

void
gw_set_output_meta_data_info(void)
{    
    extern metadata_struct out_metadata[N_OUTVAR_TYPES];
    
    strcpy(out_metadata[OUT_GW_ZWT].varname, "OUT_GW_ZWT");
    strcpy(out_metadata[OUT_GW_ZWT].long_name, "groundwater table depth");
    strcpy(out_metadata[OUT_GW_ZWT].standard_name,
           "groundwater table depth");
    strcpy(out_metadata[OUT_GW_ZWT].units, "m");
    strcpy(out_metadata[OUT_GW_ZWT].description,
           "groundwater table depth");

    strcpy(out_metadata[OUT_GW_QR].varname, "OUT_GW_QR");
    strcpy(out_metadata[OUT_GW_QR].long_name, "groundwater recharge");
    strcpy(out_metadata[OUT_GW_QR].standard_name,
           "groundwater recharge");
    strcpy(out_metadata[OUT_GW_QR].units, "mm");
    strcpy(out_metadata[OUT_GW_QR].description,
           "groundwater recharge");
    
    strcpy(out_metadata[OUT_GW_QB].varname, "OUT_GW_QB");
    strcpy(out_metadata[OUT_GW_QB].long_name, "groundwater baseflow");
    strcpy(out_metadata[OUT_GW_QB].standard_name,
           "groundwater baseflow");
    strcpy(out_metadata[OUT_GW_QB].units, "mm");
    strcpy(out_metadata[OUT_GW_QB].description,
           "groundwater baseflow");
    
    strcpy(out_metadata[OUT_GW_WA].varname, "OUT_GW_WA");
    strcpy(out_metadata[OUT_GW_WA].long_name, "Reference aquifer storage");
    strcpy(out_metadata[OUT_GW_WA].standard_name,
           "Reference aquifer storage");
    strcpy(out_metadata[OUT_GW_WA].units, "mm");
    strcpy(out_metadata[OUT_GW_WA].description,
           "Reference aquifer storage (aquifer layer only)");
    
    strcpy(out_metadata[OUT_GW_WT].varname, "OUT_GW_WT");
    strcpy(out_metadata[OUT_GW_WT].long_name, "Reference groundwater storage");
    strcpy(out_metadata[OUT_GW_WT].standard_name,
           "Reference groundwater storage");
    strcpy(out_metadata[OUT_GW_WT].units, "mm");
    strcpy(out_metadata[OUT_GW_WT].description,
           "Reference groundwater storage (aquifer and soil layers combined)");
        
    out_metadata[OUT_GW_ZWT].nelem = 1;
    out_metadata[OUT_GW_QR].nelem = 1;
    out_metadata[OUT_GW_QB].nelem = 1;
    out_metadata[OUT_GW_WA].nelem = 1;
    out_metadata[OUT_GW_WT].nelem = 1;
}

void
gw_set_state_meta_data_info(void)
{
    
}