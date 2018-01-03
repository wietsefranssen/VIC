#include <ext_driver_shared_image.h>

bool
rout_get_global_parameters(char *cmdstr)
{    
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    sscanf(cmdstr, "%s", optstr);
    
    if (strcasecmp("ROUTING", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        ext_options.ROUTING = str_to_bool(flgstr);
    } 
    else if (strcasecmp("ROUTING_PARAMETERS", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", ext_filenames.routing.nc_filename);
    }
    
    else {
        return false;
    }
    
    return true;
}

void
rout_validate_global_parameters(void)
{
    extern ext_filenames_struct ext_filenames;
    extern int mpi_decomposition;
    
    if(mpi_decomposition == MPI_DECOMPOSITION_RANDOM){
        log_err("ROUTING = TRUE but MPI_DECOMPOSITION = MPI_DECOMPOSITION_RANDOM");
    }
    if(strcasecmp(ext_filenames.routing.nc_filename, MISSING_S) == 0){
        log_err("ROUTING = TRUE but ROUTING_PARAMETERS is missing");
    }  
}