#include <ext_driver_shared_image.h>

void
ext_set_state_meta_data_info(){
    extern ext_option_struct ext_options;
    extern metadata_struct state_metadata[N_STATE_VARS];
    
    if(ext_options.ROUTING){
        strcpy(state_metadata[STATE_DISCHARGE].varname, "STATE_DISCHARGE");
        strcpy(state_metadata[STATE_DISCHARGE].long_name, "discharge");
        strcpy(state_metadata[STATE_DISCHARGE].standard_name,
               "discharge");
        strcpy(state_metadata[STATE_DISCHARGE].units, "m3/s");
        strcpy(state_metadata[STATE_DISCHARGE].description,
               "discharge");
        
        strcpy(state_metadata[STATE_NAT_DISCHARGE].varname, "STATE_NAT_DISCHARGE");
        strcpy(state_metadata[STATE_NAT_DISCHARGE].long_name, "natural_discharge");
        strcpy(state_metadata[STATE_NAT_DISCHARGE].standard_name,
               "natural_discharge");
        strcpy(state_metadata[STATE_NAT_DISCHARGE].units, "m3/s");
        strcpy(state_metadata[STATE_NAT_DISCHARGE].description,
               "natural_discharge");
    }
    
    if(ext_options.DAMS){
        strcpy(state_metadata[STATE_DAM_VOLUME].varname, "STATE_DAM_VOLUME");
        strcpy(state_metadata[STATE_DAM_VOLUME].long_name, "dam_volume");
        strcpy(state_metadata[STATE_DAM_VOLUME].standard_name,
               "volume");
        strcpy(state_metadata[STATE_DAM_VOLUME].units, "m3");
        strcpy(state_metadata[STATE_DAM_VOLUME].description,
               "volume");
        
        strcpy(state_metadata[STATE_DAM_INFLOW_HIS].varname, "STATE_DAM_INFLOW_HIS");
        strcpy(state_metadata[STATE_DAM_INFLOW_HIS].long_name, "dam_inflow_history");
        strcpy(state_metadata[STATE_DAM_INFLOW_HIS].standard_name,
               "dam_inflow_history");
        strcpy(state_metadata[STATE_DAM_INFLOW_HIS].units, "m3/s");
        strcpy(state_metadata[STATE_DAM_INFLOW_HIS].description,
               "dam_inflow_history");
        
        strcpy(state_metadata[STATE_DAM_NAT_INFLOW_HIS].varname, "STATE_DAM_NAT_INFLOW_HIS");
        strcpy(state_metadata[STATE_DAM_NAT_INFLOW_HIS].long_name, "dam_natural_inflow_history");
        strcpy(state_metadata[STATE_DAM_NAT_INFLOW_HIS].standard_name,
               "dam_natural_inflow_history");
        strcpy(state_metadata[STATE_DAM_NAT_INFLOW_HIS].units, "m3/s");
        strcpy(state_metadata[STATE_DAM_NAT_INFLOW_HIS].description,
               "dam_natural_inflow_history");
        
        strcpy(state_metadata[STATE_DAM_CALC_DISCHARGE].varname, "STATE_DAM_CALC_DISCHARGE");
        strcpy(state_metadata[STATE_DAM_CALC_DISCHARGE].long_name, "dam_calculated_discharge");
        strcpy(state_metadata[STATE_DAM_CALC_DISCHARGE].standard_name,
               "dam_calculated_discharge");
        strcpy(state_metadata[STATE_DAM_CALC_DISCHARGE].units, "m3/s");
        strcpy(state_metadata[STATE_DAM_CALC_DISCHARGE].description,
               "dam_calculated_discharge");
        
        strcpy(state_metadata[STATE_DAM_YEARS_RUNNING].varname, "STATE_DAM_YEARS_RUNNING");
        strcpy(state_metadata[STATE_DAM_YEARS_RUNNING].long_name, "dam_years_running");
        strcpy(state_metadata[STATE_DAM_YEARS_RUNNING].standard_name,
               "dam_years_running");
        strcpy(state_metadata[STATE_DAM_YEARS_RUNNING].units, "years");
        strcpy(state_metadata[STATE_DAM_YEARS_RUNNING].description,
               "dam_years_running");
        
        strcpy(state_metadata[STATE_DAM_OY_SEC].varname, "STATE_DAM_OY_SEC");
        strcpy(state_metadata[STATE_DAM_OY_SEC].long_name, "dam_op_year_sec");
        strcpy(state_metadata[STATE_DAM_OY_SEC].standard_name,
               "dam_op_year_sec");
        strcpy(state_metadata[STATE_DAM_OY_SEC].units, "sec");
        strcpy(state_metadata[STATE_DAM_OY_SEC].description,
               "dam_op_year_sec");
        
        strcpy(state_metadata[STATE_DAM_OY_DAY].varname, "STATE_DAM_OY_DAY");
        strcpy(state_metadata[STATE_DAM_OY_DAY].long_name, "dam_op_year_day");
        strcpy(state_metadata[STATE_DAM_OY_DAY].standard_name,
               "dam_op_year_day");
        strcpy(state_metadata[STATE_DAM_OY_DAY].units, "day");
        strcpy(state_metadata[STATE_DAM_OY_DAY].description,
               "dam_op_year_day");
        
        strcpy(state_metadata[STATE_DAM_OY_MONTH].varname, "STATE_DAM_OY_MONTH");
        strcpy(state_metadata[STATE_DAM_OY_MONTH].long_name, "dam_op_year_month");
        strcpy(state_metadata[STATE_DAM_OY_MONTH].standard_name,
               "dam_op_year_month");
        strcpy(state_metadata[STATE_DAM_OY_MONTH].units, "month");
        strcpy(state_metadata[STATE_DAM_OY_MONTH].description,
               "dam_op_year_month");
        
        strcpy(state_metadata[STATE_DAM_OY_YEAR].varname, "STATE_DAM_OY_YEAR");
        strcpy(state_metadata[STATE_DAM_OY_YEAR].long_name, "dam_op_year_year");
        strcpy(state_metadata[STATE_DAM_OY_YEAR].standard_name,
               "dam_op_year_year");
        strcpy(state_metadata[STATE_DAM_OY_YEAR].units, "year");
        strcpy(state_metadata[STATE_DAM_OY_YEAR].description,
               "dam_op_year_year");
        
        strcpy(state_metadata[STATE_DAM_INFLOW_TOTAL].varname, "STATE_DAM_INFLOW_TOTAL");
        strcpy(state_metadata[STATE_DAM_INFLOW_TOTAL].long_name, "dam_total_inflow");
        strcpy(state_metadata[STATE_DAM_INFLOW_TOTAL].standard_name,
               "dam_total_inflow");
        strcpy(state_metadata[STATE_DAM_INFLOW_TOTAL].units, "m3/s");
        strcpy(state_metadata[STATE_DAM_INFLOW_TOTAL].description,
               "dam_total_inflow");
        
        strcpy(state_metadata[STATE_DAM_NAT_INFLOW_TOTAL].varname, "STATE_DAM_NAT_INFLOW_TOTAL");
        strcpy(state_metadata[STATE_DAM_NAT_INFLOW_TOTAL].long_name, "dam_total_natural_inflow");
        strcpy(state_metadata[STATE_DAM_NAT_INFLOW_TOTAL].standard_name,
               "dam_total_natural_inflow");
        strcpy(state_metadata[STATE_DAM_NAT_INFLOW_TOTAL].units, "m3/s");
        strcpy(state_metadata[STATE_DAM_NAT_INFLOW_TOTAL].description,
               "dam_total_natural_inflow");
        
        strcpy(state_metadata[STATE_DAM_HIS_OFFSET].varname, "STATE_DAM_HIS_OFFSET");
        strcpy(state_metadata[STATE_DAM_HIS_OFFSET].long_name, "dam_offset_history");
        strcpy(state_metadata[STATE_DAM_HIS_OFFSET].standard_name,
               "dam_offset_history");
        strcpy(state_metadata[STATE_DAM_HIS_OFFSET].units, "steps");
        strcpy(state_metadata[STATE_DAM_HIS_OFFSET].description,
               "dam_offset_history");
        
        
    }
}