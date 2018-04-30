#include <vic.h>

bool
dam_get_global_parameters(char *cmdstr)
{
    extern option_struct    options;
    extern filenames_struct filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("DAMS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.DAMS = str_to_bool(flgstr);
    }
    else if (strcasecmp("DAMS_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", filenames.dams.nc_filename);
    }
    else {
        return false;
    }

    return true;
}

void
dam_validate_global_parameters(void)
{
    extern option_struct    options;
    extern filenames_struct filenames;

    if (!options.ROUTING) {
        log_err("DAMS = TRUE but ROUTING = FALSE");
    }
    if (strcasecmp(filenames.dams.nc_filename, MISSING_S) == 0) {
        log_err("DAMS = TRUE but DAMS_PARAMETERS is missing");
    }
}
