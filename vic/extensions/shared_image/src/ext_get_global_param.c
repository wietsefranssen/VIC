#include <ext_driver_shared_image.h>

void get_routing_type(char *cmdstr)
{
    extern ext_info_struct ext_info;
    
    char                 optstr[MAXSTRING];
    char                 ncvarname[MAXSTRING];

    strcpy(ncvarname, "MISSING");

    sscanf(cmdstr, "%*s %s %s", optstr, ncvarname);
    
    if (strcasecmp("DIRECTION", optstr) == 0) {
        strcpy(ext_info.direction_var, ncvarname);
    }
    else if (strcasecmp("VELOCITY", optstr) == 0) {
        strcpy(ext_info.velocity_var, ncvarname);
    }
    else if (strcasecmp("DIFFUSION", optstr) == 0) {
        strcpy(ext_info.diffusion_var, ncvarname);
    }
    else if (strcasecmp("DISTANCE", optstr) == 0) {
        strcpy(ext_info.distance_var, ncvarname);
    }
    else {
        log_err("Unrecognized routing variable: %s %s", optstr, ncvarname);
    }
}