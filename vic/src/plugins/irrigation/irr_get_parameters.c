#include <vic.h>

bool
irr_get_global_parameters(char *cmdstr)
{
    extern option_struct    options;
    extern filenames_struct filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("IRRIGATION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.IRRIGATION = str_to_bool(flgstr);
    }
    else if (strcasecmp("IRRIGATION_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", filenames.irrigation.nc_filename);
    }
    else {
        return false;
    }

    return true;
}

void
irr_validate_global_parameters(void)
{
}
