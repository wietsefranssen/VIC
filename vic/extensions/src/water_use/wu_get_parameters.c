#include <ext_driver_shared_image.h>

bool
wu_get_global_parameters(char *cmdstr)
{    
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    int cur_sector;
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("WATER_USE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        ext_options.WATER_USE = str_to_bool(flgstr);
    } 
    else if (strcasecmp("WATER_USE_FORCING", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", ext_filenames.water_use_forcing_pfx);
    }
    else if (strcasecmp("WATER_USE_INPUT_FREQUENCY", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        if(strcasecmp("DAILY", flgstr) == 0){
            ext_options.WU_INPUT_FREQUENCY = WU_INPUT_DAILY;
        }else if(strcasecmp("MONTHLY", flgstr) == 0){
            ext_options.WU_INPUT_FREQUENCY = WU_INPUT_MONTHLY;
        }else if(strcasecmp("YEARLY", flgstr) == 0){
            ext_options.WU_INPUT_FREQUENCY = WU_INPUT_YEARLY;
        }else{
            log_err("WATER_USE_INPUT_FREQUENCY should be DAILY, MONTHLY or YEARLY; %s is unknown", flgstr);
        }
    }
    else if (strcasecmp("WU_SECTOR", optstr) == 0) {
        sscanf(cmdstr, "%*s %s %*s %*s %*d", flgstr);
        if(strcasecmp("IRRIGATION", flgstr) == 0){
            cur_sector = WU_IRRIGATION;
        }else if(strcasecmp("DOMESTIC", flgstr) == 0){
            cur_sector = WU_DOMESTIC;
        }else if(strcasecmp("INDUSTRIAL", flgstr) == 0){
            cur_sector = WU_INDUSTRIAL;
        }else if(strcasecmp("ENERGY", flgstr) == 0){
            cur_sector = WU_ENERGY;
        }else if(strcasecmp("LIVESTOCK", flgstr) == 0){
            cur_sector = WU_LIVESTOCK;
        }else if(strcasecmp("ENVIRONMENTAL", flgstr) == 0){
            cur_sector = WU_ENVIRONMENTAL;
        }else{
            log_err("WU_SECTOR SECTOR should be IRRIGATION, DOMESTIC,"
                    "INDUSTRIAL, ENERGY, LIVESTOCK or ENVIRONMENTAL; %s is unknown", flgstr);
        }
        
        sscanf(cmdstr, "%*s %*s %s %*s %*d", flgstr);
        if(strcasecmp("CALCULATE", flgstr) == 0){
            ext_options.WU_INPUT_LOCATION[cur_sector] = WU_INPUT_CALCULATE;
        }else if(strcasecmp("NONE", flgstr) == 0){
            ext_options.WU_INPUT_LOCATION[cur_sector] = WU_INPUT_NONE;
        }else if(strcasecmp("FROM_FILE", flgstr) == 0){
            ext_options.WU_INPUT_LOCATION[cur_sector] = WU_INPUT_FROM_FILE;
            ext_options.WU_NINPUT_FROM_FILE++;
        }else{
            log_err("WU_SECTOR INPUT should be CALCULATE, NONE or FROM_FILE; %s is unknown", flgstr);
        }
        
        sscanf(cmdstr, "%*s %*s %*s %s %*d", flgstr);
        if(strcasecmp("SURFACEWATER", flgstr) == 0){
            ext_options.WU_RETURN_LOCATION[cur_sector] = WU_RETURN_SURFACEWATER;
        }else if(strcasecmp("GROUNDWATER", flgstr) == 0){
            ext_options.WU_RETURN_LOCATION[cur_sector] = WU_RETURN_GROUNDWATER;
        }else{
            log_err("WU_SECTOR RETURN_LOCATION should be SURFACEWATER or GROUNDWATER; %s is unknown", flgstr);
        }
        
        sscanf(cmdstr, "%*s %*s %*s %*s %d", &ext_options.WU_COMPENSATION_TIME[cur_sector]);
    }
    
    else {
        return false;
    }
    
    return true;
}

void
wu_validate_global_parameters(void)
{
    extern ext_filenames_struct ext_filenames;
    extern ext_option_struct ext_options;
    
    size_t i;
    
    if(!ext_options.ROUTING){
        log_err("WATER_USE = TRUE but ROUTING = FALSE");
    }
    if(strcasecmp(ext_filenames.water_use_forcing_pfx, MISSING_S) == 0 &&
            ext_options.WU_NINPUT_FROM_FILE > 0){
        log_err("WATER_USE = TRUE but WATER_USE_FORCING is missing");
    }
    for(i = 0; i < WU_NSECTORS; i++){
        if(ext_options.WU_COMPENSATION_TIME[i] < 0){
            log_err("WATER_USE_SECTOR COMPENSATION_TIME must be defined on the interval [0,inf) (days)");
        }
    }
}