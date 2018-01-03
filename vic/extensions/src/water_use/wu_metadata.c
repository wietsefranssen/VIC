#include <ext_driver_shared_image.h>

void
wu_set_output_meta_data_info(void)
{    
    extern metadata_struct out_metadata[N_OUTVAR_TYPES];
    
    strcpy(out_metadata[OUT_WU_DEMAND].varname, "OUT_WU_DEMAND");
    strcpy(out_metadata[OUT_WU_DEMAND].long_name, "sectoral_demand");
    strcpy(out_metadata[OUT_WU_DEMAND].standard_name,
           "sectoral_demand");
    strcpy(out_metadata[OUT_WU_DEMAND].units, "m3/s");
    strcpy(out_metadata[OUT_WU_DEMAND].description,
           "sectoral_demand");

    strcpy(out_metadata[OUT_WU_CONSUMP_FRAC].varname, "OUT_WU_CONSUMP_FRAC");
    strcpy(out_metadata[OUT_WU_CONSUMP_FRAC].long_name, "sectoral_consumption_fraction");
    strcpy(out_metadata[OUT_WU_CONSUMP_FRAC].standard_name,
           "sectoral_consumption_fraction");
    strcpy(out_metadata[OUT_WU_CONSUMP_FRAC].units, "m3/s");
    strcpy(out_metadata[OUT_WU_CONSUMP_FRAC].description,
           "sectoral_consumption_fraction");

    strcpy(out_metadata[OUT_WU_WITHDRAWN].varname, "OUT_WU_WITHDRAWN");
    strcpy(out_metadata[OUT_WU_WITHDRAWN].long_name, "sectoral_withdrawals");
    strcpy(out_metadata[OUT_WU_WITHDRAWN].standard_name,
           "sectoral_withdrawals");
    strcpy(out_metadata[OUT_WU_WITHDRAWN].units, "m3/s");
    strcpy(out_metadata[OUT_WU_WITHDRAWN].description,
           "sectoral_withdrawals");

    strcpy(out_metadata[OUT_WU_CONSUMED].varname, "OUT_WU_CONSUMED");
    strcpy(out_metadata[OUT_WU_CONSUMED].long_name, "sectoral_consumption");
    strcpy(out_metadata[OUT_WU_CONSUMED].standard_name,
           "sectoral_consumption");
    strcpy(out_metadata[OUT_WU_CONSUMED].units, "m3/s");
    strcpy(out_metadata[OUT_WU_CONSUMED].description,
           "sectoral_consumption");

    strcpy(out_metadata[OUT_WU_RETURNED].varname, "OUT_WU_RETURNED");
    strcpy(out_metadata[OUT_WU_RETURNED].long_name, "sectoral_return_flow");
    strcpy(out_metadata[OUT_WU_RETURNED].standard_name,
           "sectoral_return_flow");
    strcpy(out_metadata[OUT_WU_RETURNED].units, "m3/s");
    strcpy(out_metadata[OUT_WU_RETURNED].description,
           "sectoral_return_flow");

    strcpy(out_metadata[OUT_WU_COMPENSATED].varname, "OUT_WU_COMPENSATED");
    strcpy(out_metadata[OUT_WU_COMPENSATED].long_name, "sectoral_compensation");
    strcpy(out_metadata[OUT_WU_COMPENSATED].standard_name,
           "sectoral_compensation");
    strcpy(out_metadata[OUT_WU_COMPENSATED].units, "m3/s");
    strcpy(out_metadata[OUT_WU_COMPENSATED].description,
           "sectoral_compensation");

    strcpy(out_metadata[OUT_WU_TOCOMPENSATE].varname, "OUT_WU_TOCOMPENSATE");
    strcpy(out_metadata[OUT_WU_TOCOMPENSATE].long_name, "sectoral_to_compensate");
    strcpy(out_metadata[OUT_WU_TOCOMPENSATE].standard_name,
           "sectoral_to_compensate");
    strcpy(out_metadata[OUT_WU_TOCOMPENSATE].units, "m3/s");
    strcpy(out_metadata[OUT_WU_TOCOMPENSATE].description,
           "sectoral_to_compensate");

    strcpy(out_metadata[OUT_WU_TORETURN].varname, "OUT_WU_TORETURN");
    strcpy(out_metadata[OUT_WU_TORETURN].long_name, "sectoral_to_return");
    strcpy(out_metadata[OUT_WU_TORETURN].standard_name,
           "sectoral_to_return");
    strcpy(out_metadata[OUT_WU_TORETURN].units, "m3/s");
    strcpy(out_metadata[OUT_WU_TORETURN].description,
           "sectoral_to_return");

    strcpy(out_metadata[OUT_WU_AVAILABLE].varname, "OUT_WU_AVAILABLE");
    strcpy(out_metadata[OUT_WU_AVAILABLE].long_name, "sectoral_availability");
    strcpy(out_metadata[OUT_WU_AVAILABLE].standard_name,
           "sectoral_availability");
    strcpy(out_metadata[OUT_WU_AVAILABLE].units, "m3/s");
    strcpy(out_metadata[OUT_WU_AVAILABLE].description,
           "sectoral_availability");

    strcpy(out_metadata[OUT_WU_DISCHARGE].varname, "OUT_WU_DISCHARGE");
    strcpy(out_metadata[OUT_WU_DISCHARGE].long_name, "sectoral_discharge");
    strcpy(out_metadata[OUT_WU_DISCHARGE].standard_name,
           "sectoral_discharge");
    strcpy(out_metadata[OUT_WU_DISCHARGE].units, "m3/s");
    strcpy(out_metadata[OUT_WU_DISCHARGE].description,
           "sectoral_discharge");

    strcpy(out_metadata[OUT_WU_SHORTAGE].varname, "OUT_WU_SHORTAGE");
    strcpy(out_metadata[OUT_WU_SHORTAGE].long_name, "sectoral_shortage");
    strcpy(out_metadata[OUT_WU_SHORTAGE].standard_name,
           "sectoral_shortage");
    strcpy(out_metadata[OUT_WU_SHORTAGE].units, "m3/s");
    strcpy(out_metadata[OUT_WU_SHORTAGE].description,
           "sectoral_shortage");

    out_metadata[OUT_WU_DEMAND].nelem = WU_NSECTORS; 
    out_metadata[OUT_WU_CONSUMP_FRAC].nelem = WU_NSECTORS; 
    out_metadata[OUT_WU_WITHDRAWN].nelem = WU_NSECTORS; 
    out_metadata[OUT_WU_CONSUMED].nelem = WU_NSECTORS; 
    out_metadata[OUT_WU_RETURNED].nelem = WU_NSECTORS; 
    out_metadata[OUT_WU_COMPENSATED].nelem = WU_NSECTORS; 
    out_metadata[OUT_WU_TOCOMPENSATE].nelem = WU_NSECTORS; 
    out_metadata[OUT_WU_TORETURN].nelem = WU_NSECTORS;
    out_metadata[OUT_WU_AVAILABLE].nelem = WU_NSECTORS;
    out_metadata[OUT_WU_DISCHARGE].nelem = WU_NSECTORS;
    out_metadata[OUT_WU_SHORTAGE].nelem = WU_NSECTORS;  
}

void
wu_set_state_meta_data_info(void)
{
    
}