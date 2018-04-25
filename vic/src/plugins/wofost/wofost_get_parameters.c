#include <vic.h>

bool
wofost_get_global_parameters(char *cmdstr)
{    
    extern option_struct options;
//    extern filenames_struct filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("WOFOST", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.WOFOST = str_to_bool(flgstr);
    } 
    else if (strcasecmp("WOFOST_PARAMETERS", optstr) == 0) {
//        sscanf(cmdstr, "%*s %s", filenames.wofost.nc_filename);
    }
    
    else {
        return false;
    }
    
    return true;
}

void
wofost_validate_global_parameters(void)
{
    
}