#include <ext_driver_shared_image.h>

bool
wu_get_global_parameters(char *cmdstr)
{    
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("WATER_USE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        ext_options.WATER_USE = str_to_bool(flgstr);
    } 
    else if (strcasecmp("WATER_USE_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", ext_filenames.water_use.nc_filename);
    }
    
    else {
        return false;
    }
    
    return true;
}

void
wu_validate_global_parameters(void)
{
    extern ext_filenames_struct ext_filenames;
    extern ext_option_struct ext_options;
    
    if(!ext_options.ROUTING){
        log_err("WATER_USE = TRUE but ROUTING = FALSE");
    }
    if(strcasecmp(ext_filenames.water_use.nc_filename, MISSING_S) == 0){
        log_err("WATER_USE = TRUE but WATER_USE_PARAMETERS is missing");
    }
}