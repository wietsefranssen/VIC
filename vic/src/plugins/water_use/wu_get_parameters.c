#include <vic.h>

int
wu_get_sector_id(char *flgstr)
{    
    if(strcasecmp("IRR", flgstr) == 0){
        return WU_IRRIGATION;
    }else if(strcasecmp("DOM", flgstr) == 0){
        return WU_DOMESTIC;
    }else if(strcasecmp("IND", flgstr) == 0){
        return WU_INDUSTRIAL;
    }else if(strcasecmp("ENE", flgstr) == 0){
        return WU_ENERGY;
    }else if(strcasecmp("LIV", flgstr) == 0){
        return WU_LIVESTOCK;
    }else if(strcasecmp("ENV", flgstr) == 0){
        return WU_ENVIRONMENTAL;
    }else{
        log_err("WU_SECTOR SECTOR should be IRR(IGATION), DOM(ESTIC),"
                "IND(USTRIAL), ENE(RGY), LIV(ESTOCK) or ENV(IRONMENTAL); %s is unknown", flgstr);
    }
}

bool
wu_get_global_parameters(char *cmdstr)
{    
    extern option_struct options;
    extern filenames_struct filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    int cur_sector;
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("WATER_USE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        options.WATER_USE = str_to_bool(flgstr);
    } 
    else if (strcasecmp("WATER_USE_FORCING", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", filenames.water_use_forcing_pfx);
    }
    else if (strcasecmp("WATER_USE_INPUT_FREQUENCY", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        if(strcasecmp("DAILY", flgstr) == 0){
            options.WU_INPUT_FREQUENCY = WU_INPUT_DAILY;
        }else if(strcasecmp("MONTHLY", flgstr) == 0){
            options.WU_INPUT_FREQUENCY = WU_INPUT_MONTHLY;
        }else if(strcasecmp("YEARLY", flgstr) == 0){
            options.WU_INPUT_FREQUENCY = WU_INPUT_YEARLY;
        }else{
            log_err("WATER_USE_INPUT_FREQUENCY should be DAILY, MONTHLY or YEARLY; %s is unknown", flgstr);
        }
    }
    else if (strcasecmp("WU_SECTOR", optstr) == 0) {
        sscanf(cmdstr, "%*s %s %*s %*s %*d", flgstr);
        cur_sector = wu_get_sector_id(flgstr);
        
        sscanf(cmdstr, "%*s %*s %s %*s %*d", flgstr);
        if(strcasecmp("CALCULATE", flgstr) == 0){
            options.WU_INPUT_LOCATION[cur_sector] = WU_INPUT_CALCULATE;
        }else if(strcasecmp("NONE", flgstr) == 0){
            options.WU_INPUT_LOCATION[cur_sector] = WU_INPUT_NONE;
        }else if(strcasecmp("FROM_FILE", flgstr) == 0){
            options.WU_INPUT_LOCATION[cur_sector] = WU_INPUT_FROM_FILE;
            options.WU_NINPUT_FROM_FILE++;
        }else{
            log_err("WU_SECTOR INPUT should be CALCULATE, NONE or FROM_FILE; %s is unknown", flgstr);
        }
        
        sscanf(cmdstr, "%*s %*s %*s %s %*d", flgstr);
        if(strcasecmp("SURFACEWATER", flgstr) == 0){
            options.WU_RETURN_LOCATION[cur_sector] = WU_RETURN_SURFACEWATER;
        }else if(strcasecmp("GROUNDWATER", flgstr) == 0){
            options.WU_RETURN_LOCATION[cur_sector] = WU_RETURN_GROUNDWATER;
        }else{
            log_err("WU_SECTOR RETURN_LOCATION should be SURFACEWATER or GROUNDWATER; %s is unknown", flgstr);
        }
        
        sscanf(cmdstr, "%*s %*s %*s %*s %d", &options.WU_COMPENSATION_TIME[cur_sector]);
    }
    
    else if (strcasecmp("WU_STRATEGY", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        if(strcasecmp("EQUAL", flgstr) == 0){
            options.WU_STRATEGY = WU_STRATEGY_EQUAL;
        }else if(strcasecmp("PRIORITY", flgstr) == 0){
            options.WU_STRATEGY = WU_STRATEGY_PRIORITY;
            
            sscanf(cmdstr, "%*s %*s %s %*s %*s %*s %*s %*s", flgstr);
            options.WU_PRIORITY[0] = wu_get_sector_id(flgstr);
            sscanf(cmdstr, "%*s %*s %*s %s %*s %*s %*s %*s", flgstr);
            options.WU_PRIORITY[1] = wu_get_sector_id(flgstr);
            sscanf(cmdstr, "%*s %*s %*s %*s %s %*s %*s %*s", flgstr);
            options.WU_PRIORITY[2] = wu_get_sector_id(flgstr);
            sscanf(cmdstr, "%*s %*s %*s %*s %*s %s %*s %*s", flgstr);
            options.WU_PRIORITY[3] = wu_get_sector_id(flgstr);
            sscanf(cmdstr, "%*s %*s %*s %*s %*s %*s %s %*s", flgstr);
            options.WU_PRIORITY[4] = wu_get_sector_id(flgstr);
            sscanf(cmdstr, "%*s %*s %*s %*s %*s %*s %*s %s", flgstr);
            options.WU_PRIORITY[5] = wu_get_sector_id(flgstr);
        }else{
            log_err("WU_STRATEGY should be EQUAL or PRIORITY,; %s is unknown", flgstr);
        }
    }    
    
    else {
        return false;
    }
    
    return true;
}

void
wu_validate_global_parameters(void)
{
    extern filenames_struct filenames;
    extern option_struct options;
    
    size_t i;
    
    if(!options.ROUTING){
        log_err("WATER_USE = TRUE but ROUTING = FALSE");
    }
    if(strcasecmp(filenames.water_use_forcing_pfx, MISSING_S) == 0 &&
            options.WU_NINPUT_FROM_FILE > 0){
        log_err("WATER_USE = TRUE but WATER_USE_FORCING is missing");
    }
    for(i = 0; i < WU_NSECTORS; i++){
        if(options.WU_COMPENSATION_TIME[i] < 0){
            log_err("WATER_USE_SECTOR COMPENSATION_TIME must be defined on the interval [0,inf) (days)");
        }
    }
}