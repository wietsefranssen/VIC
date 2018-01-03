#include <ext_driver_shared_image.h>

bool
rout_get_global_parameters(char *cmdstr)
{    
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("ROUTING", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        ext_options.ROUTING = str_to_bool(flgstr);
    } 
    else if (strcasecmp("ROUTING_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", ext_filenames.routing.nc_filename);
    }
    else if (strcasecmp("ROUTING_UH_LENGTH", optstr) == 0) {
        sscanf(cmdstr, "%*s %i", ext_options.UH_NSTEPS);
    }
    
    else {
        return false;
    }
    
    return true;
}

void
rout_validate_global_parameters(void)
{
    extern ext_filenames_struct ext_filenames;
    extern ext_option_struct ext_options;
    
    if(strcasecmp(ext_filenames.routing.nc_filename, MISSING_S) == 0){
        log_err("ROUTING = TRUE but ROUTING_PARAMETERS is missing");
    }    
    if (ext_options.UH_NSTEPS <= 0) {
        log_err("ROUT_UH_LENGTH must be defined on the interval [0,Inf) (days)")
    }
}