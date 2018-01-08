#include <ext_driver_shared_image.h>

bool
irr_get_global_parameters(char *cmdstr)
{    
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        ext_options.IRRIGATION = str_to_bool(flgstr);
    } 
    else if (strcasecmp("IRRIGATION_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", ext_filenames.irrigation.nc_filename);
    }
    
    else {
        return false;
    }
    
    return true;
}

void
irr_validate_global_parameters(void)
{
    extern ext_option_struct ext_options;
    
    if(!ext_options.WATER_USE){
        log_err("IRRIGATION = TRUE but RWATER_USE = FALSE");
    }  
}