#include <ext_driver_shared_image.h>

void
rout_set_output_meta_data_info(void)
{    
    extern metadata_struct out_metadata[N_OUTVAR_TYPES];
    
    strcpy(out_metadata[OUT_DISCHARGE].varname, "OUT_DISCHARGE");
    strcpy(out_metadata[OUT_DISCHARGE].long_name, "discharge");
    strcpy(out_metadata[OUT_DISCHARGE].standard_name,
           "discharge");
    strcpy(out_metadata[OUT_DISCHARGE].units, "m3/s");
    strcpy(out_metadata[OUT_DISCHARGE].description,
           "discharge");

    strcpy(out_metadata[OUT_NAT_DISCHARGE].varname, "OUT_NAT_DISCHARGE");
    strcpy(out_metadata[OUT_NAT_DISCHARGE].long_name, "natural_discharge");
    strcpy(out_metadata[OUT_NAT_DISCHARGE].standard_name,
           "natural_discharge");
    strcpy(out_metadata[OUT_NAT_DISCHARGE].units, "m3/s");
    strcpy(out_metadata[OUT_NAT_DISCHARGE].description,
           "natural_discharge");
    
    out_metadata[OUT_DISCHARGE].nelem = 1;
    out_metadata[OUT_NAT_DISCHARGE].nelem = 1;
}

void
rout_set_state_meta_data_info(void)
{
    
}