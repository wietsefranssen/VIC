#include <ext_driver_shared_image.h>

void
irr_start(void)
{
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    int status;
    
    // open routing parameter file
    status = nc_open(ext_filenames.irrigation.nc_filename, NC_NOWRITE,
                     &(ext_filenames.irrigation.nc_id));
    check_nc_status(status, "Error opening %s",
                    ext_filenames.irrigation.nc_filename);

    ext_options.NIRRTYPES = get_nc_dimension(&(ext_filenames.irrigation), 
            ext_filenames.info.nirr_dim);
    ext_options.NIRRSEASONS = get_nc_dimension(&(ext_filenames.irrigation), 
            ext_filenames.info.nseason_dim);

    // close routing parameter file
    status = nc_close(ext_filenames.irrigation.nc_id);
    check_nc_status(status, "Error closing %s",
                    ext_filenames.irrigation.nc_filename);    
}