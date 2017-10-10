#include <ext_driver_shared_image.h>

void
ext_set_output_met_data_info(){    
    extern metadata_struct out_metadata[N_OUTVAR_TYPES];
    
    // Extensions
    /* Discharge [m3/s] */
    strcpy(out_metadata[OUT_DISCHARGE].varname, "OUT_DISCHARGE");
    strcpy(out_metadata[OUT_DISCHARGE].long_name, "discharge");
    strcpy(out_metadata[OUT_DISCHARGE].standard_name,
           "discharge");
    strcpy(out_metadata[OUT_DISCHARGE].units, "m3/s");
    strcpy(out_metadata[OUT_DISCHARGE].description,
           "discharge");
    
    /* Natural discharge [m3/s] */
    strcpy(out_metadata[OUT_NAT_DISCHARGE].varname, "OUT_NAT_DISCHARGE");
    strcpy(out_metadata[OUT_NAT_DISCHARGE].long_name, "natural_discharge");
    strcpy(out_metadata[OUT_NAT_DISCHARGE].standard_name,
           "natural_discharge");
    strcpy(out_metadata[OUT_NAT_DISCHARGE].units, "m3/s");
    strcpy(out_metadata[OUT_NAT_DISCHARGE].description,
           "natural_discharge");
    
    /* Wall time spent inside ext_run [seconds] */
    strcpy(out_metadata[OUT_TIME_EXTRUN_WALL].varname, "OUT_TIME_EXTRUN_WALL");
    strcpy(out_metadata[OUT_TIME_EXTRUN_WALL].long_name, "time_extrun_wall");
    strcpy(out_metadata[OUT_TIME_EXTRUN_WALL].standard_name,
           "ext_run_wall_time");
    strcpy(out_metadata[OUT_TIME_EXTRUN_WALL].units, "seconds");
    strcpy(out_metadata[OUT_TIME_EXTRUN_WALL].description,
           "Wall time spent inside ext_run");

    /* CPU time spent inside ext_run [seconds] */
    strcpy(out_metadata[OUT_TIME_EXTRUN_CPU].varname, "OUT_TIME_EXTRUN_CPU");
    strcpy(out_metadata[OUT_TIME_EXTRUN_CPU].long_name, "time_extrun_cpu");
    strcpy(out_metadata[OUT_TIME_EXTRUN_CPU].standard_name, "ext_run_cpu_time");
    strcpy(out_metadata[OUT_TIME_EXTRUN_CPU].units, "seconds");
    strcpy(out_metadata[OUT_TIME_EXTRUN_CPU].description,
           "CPU time spent inside ext_run");
}