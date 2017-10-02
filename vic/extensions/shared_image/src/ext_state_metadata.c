#include <ext_driver_shared_image.h>

void
ext_set_state_meta_data_info(){
    extern ext_option_struct ext_options;
    extern metadata_struct state_metadata[N_STATE_VARS];
    
    if(ext_options.ROUTING){
        // STATE_DISCHARGE
        strcpy(state_metadata[STATE_DISCHARGE].varname, "STATE_DISCHARGE");
        strcpy(state_metadata[STATE_DISCHARGE].long_name, "discharge");
        strcpy(state_metadata[STATE_DISCHARGE].standard_name,
               "discharge");
        strcpy(state_metadata[STATE_DISCHARGE].units, "m3/s");
        strcpy(state_metadata[STATE_DISCHARGE].description,
               "discharge");
    }
}