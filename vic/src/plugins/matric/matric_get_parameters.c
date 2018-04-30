#include <vic.h>

bool
matric_get_global_parameters(char *cmdstr)
{
    extern option_struct options;

    char                 optstr[MAXSTRING];
    char                 flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("MATRIC", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.MATRIC = str_to_bool(flgstr);
    }
    else {
        return false;
    }

    return true;
}
