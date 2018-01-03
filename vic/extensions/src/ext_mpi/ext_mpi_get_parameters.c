#include <ext_driver_shared_image.h>

bool
ext_mpi_get_global_parameters(char *cmdstr)
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
            log_err("MPI_DECOMPOSITION should be BASIN or RANDOM");
        }
    }
    else {
        return false;
    }
    
    return true;
}

void
ext_mpi_validate_global_parameters(void)
{
    extern ext_option_struct ext_options;
    extern int mpi_decomposition;
    
    if(mpi_decomposition == MPI_DECOMPOSITION_BASIN){
        if(!ext_options.ROUTING){
            log_err("MPI_DECOMPOSITION = MPI_DECOMPOSITION_BASIN but ROUTING = FALSE");
        }
    }
}