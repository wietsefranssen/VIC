#include <ext_driver_shared_image.h>

void
ext_set_output_met_data_info(){    
    extern metadata_struct out_metadata[N_OUTVAR_TYPES];
    extern ext_option_struct ext_options;
    
    if(ext_options.ROUTING){
        // Extensions
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
    }
    
    if(ext_options.DAMS){
        strcpy(out_metadata[OUT_DAM_VOLUME].varname, "OUT_DAM_VOLUME");
        strcpy(out_metadata[OUT_DAM_VOLUME].long_name, "dam_current_volume");
        strcpy(out_metadata[OUT_DAM_VOLUME].standard_name,
               "dam_current_volume");
        strcpy(out_metadata[OUT_DAM_VOLUME].units, "m3");
        strcpy(out_metadata[OUT_DAM_VOLUME].description,
               "dam_current_volume");

        strcpy(out_metadata[OUT_DAM_MAX_VOLUME].varname, "OUT_DAM_MAX_VOLUME");
        strcpy(out_metadata[OUT_DAM_MAX_VOLUME].long_name, "dam_maximum_volume");
        strcpy(out_metadata[OUT_DAM_MAX_VOLUME].standard_name,
               "dam_maximum_volume");
        strcpy(out_metadata[OUT_DAM_MAX_VOLUME].units, "m3");
        strcpy(out_metadata[OUT_DAM_MAX_VOLUME].description,
               "dam_maximum_volume");

        strcpy(out_metadata[OUT_DAM_FRAC_VOLUME].varname, "OUT_DAM_FRAC_VOLUME");
        strcpy(out_metadata[OUT_DAM_FRAC_VOLUME].long_name, "dam_fractional_volume");
        strcpy(out_metadata[OUT_DAM_FRAC_VOLUME].standard_name,
               "dam_fractional_volume");
        strcpy(out_metadata[OUT_DAM_FRAC_VOLUME].units, "m3");
        strcpy(out_metadata[OUT_DAM_FRAC_VOLUME].description,
               "dam_fractional_volume");

        strcpy(out_metadata[OUT_DAM_AREA].varname, "OUT_DAM_AREA");
        strcpy(out_metadata[OUT_DAM_AREA].long_name, "dam_current_area");
        strcpy(out_metadata[OUT_DAM_AREA].standard_name,
               "dam_current_area");
        strcpy(out_metadata[OUT_DAM_AREA].units, "m2");
        strcpy(out_metadata[OUT_DAM_AREA].description,
               "dam_current_area");

        strcpy(out_metadata[OUT_DAM_MAX_AREA].varname, "OUT_DAM_MAX_AREA");
        strcpy(out_metadata[OUT_DAM_MAX_AREA].long_name, "dam_maximum_area");
        strcpy(out_metadata[OUT_DAM_MAX_AREA].standard_name,
               "dam_maximum_area");
        strcpy(out_metadata[OUT_DAM_MAX_AREA].units, "m2");
        strcpy(out_metadata[OUT_DAM_MAX_AREA].description,
               "dam_maximum_area");

        strcpy(out_metadata[OUT_DAM_FRAC_AREA].varname, "OUT_DAM_FRAC_AREA");
        strcpy(out_metadata[OUT_DAM_FRAC_AREA].long_name, "dam_fractional_area");
        strcpy(out_metadata[OUT_DAM_FRAC_AREA].standard_name,
               "dam_fractional_area");
        strcpy(out_metadata[OUT_DAM_FRAC_AREA].units, "m2");
        strcpy(out_metadata[OUT_DAM_FRAC_AREA].description,
               "dam_fractional_area");

        strcpy(out_metadata[OUT_DAM_HEIGHT].varname, "OUT_DAM_HEIGHT");
        strcpy(out_metadata[OUT_DAM_HEIGHT].long_name, "dam_current_height");
        strcpy(out_metadata[OUT_DAM_HEIGHT].standard_name,
               "dam_current_height");
        strcpy(out_metadata[OUT_DAM_HEIGHT].units, "m");
        strcpy(out_metadata[OUT_DAM_HEIGHT].description,
               "dam_current_height");

        strcpy(out_metadata[OUT_DAM_MAX_HEIGHT].varname, "OUT_DAM_MAX_HEIGHT");
        strcpy(out_metadata[OUT_DAM_MAX_HEIGHT].long_name, "dam_max_height");
        strcpy(out_metadata[OUT_DAM_MAX_HEIGHT].standard_name,
               "dam_max_height");
        strcpy(out_metadata[OUT_DAM_MAX_HEIGHT].units, "m");
        strcpy(out_metadata[OUT_DAM_MAX_HEIGHT].description,
               "dam_max_height");

        strcpy(out_metadata[OUT_DAM_FRAC_HEIGHT].varname, "OUT_DAM_FRAC_HEIGHT");
        strcpy(out_metadata[OUT_DAM_FRAC_HEIGHT].long_name, "dam_fractional_height");
        strcpy(out_metadata[OUT_DAM_FRAC_HEIGHT].standard_name,
               "dam_fractional_height");
        strcpy(out_metadata[OUT_DAM_FRAC_HEIGHT].units, "m");
        strcpy(out_metadata[OUT_DAM_FRAC_HEIGHT].description,
               "dam_fractional_height");

        strcpy(out_metadata[OUT_DAM_DISCHARGE].varname, "OUT_DAM_DISCHARGE");
        strcpy(out_metadata[OUT_DAM_DISCHARGE].long_name, "dam_discharge");
        strcpy(out_metadata[OUT_DAM_DISCHARGE].standard_name,
               "dam_discharge");
        strcpy(out_metadata[OUT_DAM_DISCHARGE].units, "m3/s");
        strcpy(out_metadata[OUT_DAM_DISCHARGE].description,
               "dam_discharge");

        strcpy(out_metadata[OUT_DAM_CALC_DISCHARGE].varname, "OUT_DAM_CALC_DISCHARGE");
        strcpy(out_metadata[OUT_DAM_CALC_DISCHARGE].long_name, "dam_calculated_discharge");
        strcpy(out_metadata[OUT_DAM_CALC_DISCHARGE].standard_name,
               "dam_calculated_discharge");
        strcpy(out_metadata[OUT_DAM_CALC_DISCHARGE].units, "m3/s");
        strcpy(out_metadata[OUT_DAM_CALC_DISCHARGE].description,
               "dam_calculated_discharge");

        strcpy(out_metadata[OUT_DAM_INFLOW].varname, "OUT_DAM_INFLOW");
        strcpy(out_metadata[OUT_DAM_INFLOW].long_name, "dam_inflow");
        strcpy(out_metadata[OUT_DAM_INFLOW].standard_name,
               "dam_inflow");
        strcpy(out_metadata[OUT_DAM_INFLOW].units, "m3/s");
        strcpy(out_metadata[OUT_DAM_INFLOW].description,
               "dam_inflow");

        strcpy(out_metadata[OUT_DAM_CALC_INFLOW].varname, "OUT_DAM_CALC_INFLOW");
        strcpy(out_metadata[OUT_DAM_CALC_INFLOW].long_name, "dam_calculated_inflow");
        strcpy(out_metadata[OUT_DAM_CALC_INFLOW].standard_name,
               "dam_calculated_inflow");
        strcpy(out_metadata[OUT_DAM_CALC_INFLOW].units, "m3/s");
        strcpy(out_metadata[OUT_DAM_CALC_INFLOW].description,
               "dam_calculated_inflow");

        strcpy(out_metadata[OUT_DAM_CALC_NAT_INFLOW].varname, "OUT_DAM_CALC_NAT_INFLOW");
        strcpy(out_metadata[OUT_DAM_CALC_NAT_INFLOW].long_name, "dam_calculated_natural_inflow");
        strcpy(out_metadata[OUT_DAM_CALC_NAT_INFLOW].standard_name,
               "dam_calculated_natural_inflow");
        strcpy(out_metadata[OUT_DAM_CALC_NAT_INFLOW].units, "m3/s");
        strcpy(out_metadata[OUT_DAM_CALC_NAT_INFLOW].description,
               "dam_calculated_natural_inflow");

        strcpy(out_metadata[OUT_DAM_NAT_INFLOW].varname, "OUT_DAM_NAT_INFLOW");
        strcpy(out_metadata[OUT_DAM_NAT_INFLOW].long_name, "dam_natural_inflow");
        strcpy(out_metadata[OUT_DAM_NAT_INFLOW].standard_name,
               "dam_natural_inflow");
        strcpy(out_metadata[OUT_DAM_NAT_INFLOW].units, "m3/s");
        strcpy(out_metadata[OUT_DAM_NAT_INFLOW].description,
               "dam_natural_inflow");
        
        strcpy(out_metadata[OUT_DAM_AMPLITUDE].varname, "OUT_DAM_AMPLITUDE");
        strcpy(out_metadata[OUT_DAM_AMPLITUDE].long_name, "dam_amplitude");
        strcpy(out_metadata[OUT_DAM_AMPLITUDE].standard_name,
               "dam_amplitude");
        strcpy(out_metadata[OUT_DAM_AMPLITUDE].units, "-");
        strcpy(out_metadata[OUT_DAM_AMPLITUDE].description,
               "dam_amplitude");
    
        strcpy(out_metadata[OUT_DAM_OFFSET].varname, "OUT_DAM_OFFSET");
        strcpy(out_metadata[OUT_DAM_OFFSET].long_name, "dam_offset");
        strcpy(out_metadata[OUT_DAM_OFFSET].standard_name,
               "dam_offset");
        strcpy(out_metadata[OUT_DAM_OFFSET].units, "m3/s");
        strcpy(out_metadata[OUT_DAM_OFFSET].description,
               "dam_offset");
    
    }
    
    strcpy(out_metadata[OUT_TIME_EXTRUN_WALL].varname, "OUT_TIME_EXTRUN_WALL");
    strcpy(out_metadata[OUT_TIME_EXTRUN_WALL].long_name, "time_extrun_wall");
    strcpy(out_metadata[OUT_TIME_EXTRUN_WALL].standard_name,
           "ext_run_wall_time");
    strcpy(out_metadata[OUT_TIME_EXTRUN_WALL].units, "seconds");
    strcpy(out_metadata[OUT_TIME_EXTRUN_WALL].description,
           "Wall time spent inside ext_run");

    strcpy(out_metadata[OUT_TIME_EXTRUN_CPU].varname, "OUT_TIME_EXTRUN_CPU");
    strcpy(out_metadata[OUT_TIME_EXTRUN_CPU].long_name, "time_extrun_cpu");
    strcpy(out_metadata[OUT_TIME_EXTRUN_CPU].standard_name, "ext_run_cpu_time");
    strcpy(out_metadata[OUT_TIME_EXTRUN_CPU].units, "seconds");
    strcpy(out_metadata[OUT_TIME_EXTRUN_CPU].description,
           "CPU time spent inside ext_run");
    
    out_metadata[OUT_DAM_VOLUME].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_MAX_VOLUME].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_FRAC_VOLUME].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_AREA].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_MAX_AREA].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_FRAC_AREA].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_HEIGHT].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_MAX_HEIGHT].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_FRAC_HEIGHT].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_DISCHARGE].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_CALC_DISCHARGE].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_INFLOW].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_CALC_INFLOW].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_NAT_INFLOW].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_CALC_NAT_INFLOW].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_AMPLITUDE].nelem = ext_options.ndams;
    out_metadata[OUT_DAM_OFFSET].nelem = ext_options.ndams;
}