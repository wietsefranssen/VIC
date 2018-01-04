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

    strcpy(out_metadata[OUT_WU_COMPENSATION].varname, "OUT_WU_COMPENSATION");
    strcpy(out_metadata[OUT_WU_COMPENSATION].long_name, "sectoral_total_compensation_need");
    strcpy(out_metadata[OUT_WU_COMPENSATION].standard_name,
           "sectoral_total_compensation_need");
    strcpy(out_metadata[OUT_WU_COMPENSATION].units, "m3/s");
    strcpy(out_metadata[OUT_WU_COMPENSATION].description,
           "sectoral_total_compensation_need");

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

    strcpy(out_metadata[OUT_WU_SHORTAGE].varname, "OUT_WU_SHORTAGE");
    strcpy(out_metadata[OUT_WU_SHORTAGE].long_name, "sectoral_shortage");
    strcpy(out_metadata[OUT_WU_SHORTAGE].standard_name,
           "sectoral_shortage");
    strcpy(out_metadata[OUT_WU_SHORTAGE].units, "m3/s");
    strcpy(out_metadata[OUT_WU_SHORTAGE].description,
           "sectoral_shortage");

    out_metadata[OUT_WU_DEMAND].nelem = WU_NSECTORS;
    out_metadata[OUT_WU_WITHDRAWN].nelem = WU_NSECTORS;
    out_metadata[OUT_WU_CONSUMED].nelem = WU_NSECTORS;
    out_metadata[OUT_WU_RETURNED].nelem = WU_NSECTORS;
    out_metadata[OUT_WU_COMPENSATED].nelem = WU_NSECTORS;
    out_metadata[OUT_WU_SHORTAGE].nelem = WU_NSECTORS;
}

void
wu_set_state_meta_data_info(void)
{
    
}