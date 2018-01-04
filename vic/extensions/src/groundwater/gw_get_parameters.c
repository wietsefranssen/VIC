#include <ext_driver_shared_image.h>

bool
gw_get_global_parameters(char *cmdstr)
{    
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("GROUNDWATER", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        ext_options.GROUNDWATER = str_to_bool(flgstr);
    } 
    else if (strcasecmp("GROUNDWATER_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", ext_filenames.groundwater.nc_filename);
    }
    else if (strcasecmp("GROUNDWATER_INIT", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        if(strcasecmp("DEFAULT", flgstr) == 0){
            ext_options.GW_INIT_FROM_FILE = false;
        }else if(strcasecmp("FROM_FILE", flgstr) == 0){
            ext_options.GW_INIT_FROM_FILE = true;
        }else{
            log_err("GROUNDWATER_INIT should be DEFAULT or FROM_FILE; %s is unknown", flgstr);
        }
    }
    
    else {
        return false;
    }
    
    return true;
}

void
gw_validate_global_parameters(void)
{
    extern ext_filenames_struct ext_filenames;
    
    if(strcasecmp(ext_filenames.groundwater.nc_filename, MISSING_S) == 0){
        log_err("GROUNDWATER = TRUE but GROUNDWATER_PARAMETERS is missing");
    }
}