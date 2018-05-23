#include <vic.h>

void
wu_validate_domain(void)
{
    extern filenames_struct filenames;
    
    int status;
    
    // open water use parameter file
    status = nc_open(filenames.water_use.nc_filename, NC_NOWRITE,
                     &(filenames.water_use.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.water_use.nc_filename);

    compare_ncdomain_with_global_domain(&filenames.water_use);

    // close water use parameter file
    status = nc_close(filenames.water_use.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.water_use.nc_filename);
}