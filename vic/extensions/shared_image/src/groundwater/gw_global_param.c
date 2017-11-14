#include <ext_driver_shared_image.h>

void
get_groundwater_type(char *cmdstr)
{
    extern ext_filenames_struct ext_filenames;
    
    char                 optstr[MAXSTRING];
    char                 ncvarname[MAXSTRING];

    strcpy(ncvarname, MISSING_S);

    sscanf(cmdstr, "%*s %s %s", optstr, ncvarname);
    
    if (strcasecmp("DIRECTION", optstr) == 0) {
        
    }
    else {
        log_err("Unrecognized routing variable: %s %s", optstr, ncvarname);
    }
}