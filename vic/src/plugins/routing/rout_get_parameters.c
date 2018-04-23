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
        options.ROUTING = str_to_bool(flgstr);
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
    
    if(mpi_decomposition == MPI_DECOMPOSITION_RANDOM){
        options.ROUTING_TYPE = ROUTING_RANDOM;
    } else if(mpi_decomposition == MPI_DECOMPOSITION_BASIN){
        options.ROUTING_TYPE = ROUTING_BASIN;
    } else if(mpi_decomposition == MPI_DECOMPOSITION_FILE){
        log_info("MPI_DECOMPOSITION = FILE and ROUTING = TRUE; "
                "Make sure that the decomposition file follows the routing "
                "flow paths, otherwise errors might occur.");
        options.ROUTING_TYPE = ROUTING_BASIN;
    } else {
        log_err("Unknown mpi_decomposition for routing");
    }
    
    if(strcasecmp(filenames.routing.nc_filename, MISSING_S) == 0){
        log_err("ROUTING = TRUE but ROUTING_PARAMETERS is missing");
    }  
}