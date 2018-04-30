#include <vic.h>

#include "vic_mpi.h"

bool
mpi_get_global_parameters(char *cmdstr)
{
    extern int              mpi_decomposition;
    extern filenames_struct filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];

    sscanf(cmdstr, "%s", optstr);

    if (strcasecmp("MPI_DECOMPOSITION", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        if (strcasecmp("BASIN", flgstr) == 0) {
            mpi_decomposition = MPI_DECOMPOSITION_BASIN;
        }
        else if (strcasecmp("RANDOM", flgstr) == 0) {
            mpi_decomposition = MPI_DECOMPOSITION_RANDOM;
        }
        else if (strcasecmp("FILE", flgstr) == 0) {
            mpi_decomposition = MPI_DECOMPOSITION_FILE;
        }
        else {
            log_err(
                "MPI_DECOMPOSITION should be BASIN, FILE or RANDOM; %s is unknown",
                flgstr);
        }
    }
    else if (strcasecmp("MPI_DECOMPOSITION_FILE", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", filenames.mpi.nc_filename);
    }
    else {
        return false;
    }

    return true;
}

void
mpi_validate_global_parameters(void)
{
    extern option_struct    options;
    extern int              mpi_decomposition;
    extern filenames_struct filenames;

    if (mpi_decomposition == MPI_DECOMPOSITION_BASIN) {
        if (!options.ROUTING) {
            log_err("MPI_DECOMPOSITION = MPI_DECOMPOSITION_BASIN but "
                    "ROUTING = FALSE");
        }
    }
    else if (mpi_decomposition == MPI_DECOMPOSITION_FILE) {
        if (strcasecmp(filenames.mpi.nc_filename, MISSING_S) == 0) {
            log_err("MPI_DECOMPOSITION = MPI_DECOMPOSITION_FILE but "
                    "MPI_DECOMPOSITION_FILE = MISSING");
        }
    }
}
