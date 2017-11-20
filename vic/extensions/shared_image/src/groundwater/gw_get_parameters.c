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
    else if (strcasecmp("GROUNDWATER_TYPE", optstr) == 0) {
        get_groundwater_type(cmdstr);
    }
    
    else {
        return false;
    }
    
    return true;
}

void
get_groundwater_type(char *cmdstr)
{
//    extern ext_filenames_struct ext_filenames;
//    
//    char                 optstr[MAXSTRING];
//    char                 ncvarname[MAXSTRING];
//
//    strcpy(ncvarname, MISSING_S);
//
//    sscanf(cmdstr, "%*s %s %s", optstr, ncvarname);
//    
//    if (strcasecmp("ZWTI", optstr) == 0) {
//        
//    }
//    else {
//        log_err("Unrecognized routing variable: %s %s", optstr, ncvarname);
//    }
}

bool
gw_get_parameters(char *cmdstr)
{        
    return false;
}

void
gw_validate_global_parameters(void)
{
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    if(ext_options.GROUNDWATER){
        if(strcasecmp(ext_filenames.groundwater.nc_filename, MISSING_S) == 0){
            log_err("GROUNDWATER = TRUE but GROUNDWATER_PARAMETERS is missing");
        }
    }
}

void
gw_validate_parameters(void)
{
    
}