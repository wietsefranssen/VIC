#include <ext_driver_shared_image.h>

bool
dam_get_global_parameters(char *cmdstr)
{    
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("DAMS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        ext_options.DAMS = str_to_bool(flgstr);
    } 
    else if (strcasecmp("DAMS_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", ext_filenames.dams.nc_filename);
    }
    
    else {
        return false;
    }
    
    return true;
}

void
dam_validate_global_parameters(void)
{
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    if(!ext_options.ROUTING){
        log_err("DAMS = TRUE but ROUTING = FALSE");
    }  
    if(strcasecmp(ext_filenames.dams.nc_filename, MISSING_S) == 0){
        log_err("DAMS = TRUE but DAMS_PARAMETERS is missing");
    }  
}