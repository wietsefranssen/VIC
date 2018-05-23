#include <vic.h>

void
gw_validate_domain(void)
{
    extern filenames_struct filenames;
    
    int status;
    
    // open routing parameter file
    status = nc_open(filenames.groundwater.nc_filename, NC_NOWRITE,
                     &(filenames.groundwater.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.groundwater.nc_filename);
    
    compare_ncdomain_with_global_domain(&filenames.groundwater);

    // close routing parameter file
    status = nc_close(filenames.groundwater.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.groundwater.nc_filename);
}