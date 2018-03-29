#include <vic_def.h>
#include <vic_general.h>

bool
rout_get_global_parameters(char *cmdstr)
{    
    extern option_struct options;
    extern filenames_struct filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("ROUTING", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        if(strcasecmp("FALSE", flgstr) == 0){
            options.ROUTING = ROUTING_FALSE;
        }else if(strcasecmp("LOCAL", flgstr) == 0){
            options.ROUTING = ROUTING_LOCAL;
        }else if(strcasecmp("GLOBAL", flgstr) == 0){
            options.ROUTING = ROUTING_GLOBAL;
        }else{
            log_err("ROUTING should be FALSE, LOCAL or GLOBAL; %s is unknown", flgstr);
        }
    } 
    else if (strcasecmp("ROUTING_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", filenames.routing.nc_filename);
    }
    
    else {
        return false;
    }
    
    return true;
}

void
rout_validate_global_parameters(void)
{
    extern option_struct options;
    extern filenames_struct filenames;
    extern int mpi_decomposition;
    
    if(mpi_decomposition == MPI_DECOMPOSITION_RANDOM && options.ROUTING == ROUTING_LOCAL){
        log_err("ROUTING = ROUTING_LOCAL but MPI_DECOMPOSITION = MPI_DECOMPOSITION_RANDOM");
    }
    if(strcasecmp(filenames.routing.nc_filename, MISSING_S) == 0){
        log_err("ROUTING = TRUE but ROUTING_PARAMETERS is missing");
    }  
}