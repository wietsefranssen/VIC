#include <vic.h>

bool
efr_get_global_parameters(char *cmdstr)
{
    extern option_struct options;

    char                 optstr[MAXSTRING];
    char                 flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("EFR", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.EFR = str_to_bool(flgstr);
    }
    else {
        return false;
    }

    return true;
}

void
efr_validate_global_parameters(void)
{
}
