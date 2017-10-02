#include <ext_driver_shared_image.h>

void get_routing_type(char *cmdstr)
{
    extern ext_filenames_struct ext_filenames;
    
    char                 optstr[MAXSTRING];
    char                 ncvarname[MAXSTRING];

    strcpy(ncvarname, "MISSING");

    sscanf(cmdstr, "%*s %s %s", optstr, ncvarname);
    
    if (strcasecmp("DIRECTION", optstr) == 0) {
        strcpy(ext_filenames.info.direction_var, ncvarname);
    }
    else if (strcasecmp("VELOCITY", optstr) == 0) {
        strcpy(ext_filenames.info.velocity_var, ncvarname);
    }
    else if (strcasecmp("DIFFUSION", optstr) == 0) {
        strcpy(ext_filenames.info.diffusion_var, ncvarname);
    }
    else if (strcasecmp("DISTANCE", optstr) == 0) {
        strcpy(ext_filenames.info.distance_var, ncvarname);
    }
    else {
        log_err("Unrecognized routing variable: %s %s", optstr, ncvarname);
    }
}