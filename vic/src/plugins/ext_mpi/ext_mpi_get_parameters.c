#include <vic_def.h>

bool
mpi_get_global_parameters(char *cmdstr)
{    
    extern int mpi_decomposition;
    
    char                       optstr[MAXSTRING];
    char                       flgstr[MAXSTRING];
    
    sscanf(cmdstr, "%s", optstr);
        
    if (strcasecmp("MPI_DECOMPOSITION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        if(strcasecmp("BASIN", flgstr) == 0){
            mpi_decomposition = MPI_DECOMPOSITION_BASIN;
        }else if(strcasecmp("RANDOM", flgstr) == 0){
            mpi_decomposition = MPI_DECOMPOSITION_RANDOM;
        }else{
            log_err("MPI_DECOMPOSITION should be BASIN or RANDOM; %s is unknown", flgstr);
        }
    }
    else {
        return false;
    }
    
    return true;
}

void
mpi_validate_global_parameters(void)
{
    extern option_struct options;
    extern int mpi_decomposition;
    
    if(mpi_decomposition == MPI_DECOMPOSITION_BASIN){
        if(!options.ROUTING){
            log_err("MPI_DECOMPOSITION = MPI_DECOMPOSITION_BASIN but ROUTING = FALSE");
        }
    }
}