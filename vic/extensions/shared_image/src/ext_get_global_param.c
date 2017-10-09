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

void get_dam_type(char *cmdstr)
{
    extern ext_filenames_struct ext_filenames;
    
    char                 optstr[MAXSTRING];
    char                 ncvarname[MAXSTRING];

    strcpy(ncvarname, "MISSING");
    
    sscanf(cmdstr, "%*s %s %s", optstr, ncvarname);
    
    if (strcasecmp("NDAMS", optstr) == 0) {
        strcpy(ext_filenames.info.ndam_var, ncvarname);
    }
    else if (strcasecmp("YEAR", optstr) == 0) {
        strcpy(ext_filenames.info.dam_year_var, ncvarname);
    }
    else if (strcasecmp("LAT", optstr) == 0) {
        strcpy(ext_filenames.info.dam_lat_var, ncvarname);
    }
    else if (strcasecmp("LON", optstr) == 0) {
        strcpy(ext_filenames.info.dam_lon_var, ncvarname);
    }
    else if (strcasecmp("VOLUME", optstr) == 0) {
        strcpy(ext_filenames.info.dam_volume_var, ncvarname);
    }
    else if (strcasecmp("AREA", optstr) == 0) {
        strcpy(ext_filenames.info.dam_area_var, ncvarname);
    }
    else if (strcasecmp("HEIGHT", optstr) == 0) {
        strcpy(ext_filenames.info.dam_height_var, ncvarname);
    }
    else {
        log_err("Unrecognized dam variable: %s %s", optstr, ncvarname);
    }
}