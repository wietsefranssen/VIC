#include <ext_driver_shared_image.h>

void
rout_start(void)
{
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    int status;
    
    // open routing parameter file
    status = nc_open(ext_filenames.routing.nc_filename, NC_NOWRITE,
                     &(ext_filenames.routing.nc_id));
    check_nc_status(status, "Error opening %s",
                    ext_filenames.routing.nc_filename);

    ext_options.UH_NSTEPS = get_nc_dimension(&(ext_filenames.routing), 
            ext_filenames.info.uh_nsteps);

    // close routing parameter file
    status = nc_close(ext_filenames.routing.nc_id);
    check_nc_status(status, "Error closing %s",
                    ext_filenames.routing.nc_filename);
}