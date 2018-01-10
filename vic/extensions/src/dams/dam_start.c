#include <ext_driver_shared_image.h>

void
dam_start(void)
{
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    
    int status;
    
    // open routing parameter file
    status = nc_open(ext_filenames.dams.nc_filename, NC_NOWRITE,
                     &(ext_filenames.dams.nc_id));
    check_nc_status(status, "Error opening %s",
                    ext_filenames.dams.nc_filename);

    ext_options.MAXDAMS = get_nc_dimension(&(ext_filenames.dams), 
            ext_filenames.info.ndam_dim);

    // close routing parameter file
    status = nc_close(ext_filenames.dams.nc_id);
    check_nc_status(status, "Error closing %s",
                    ext_filenames.dams.nc_filename);    
}