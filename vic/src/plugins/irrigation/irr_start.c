#include <vic.h>

void
irr_start(void)
{
    extern option_struct    options;
    extern filenames_struct filenames;

    int                     status;

    // open routing parameter file
    status = nc_open(filenames.irrigation.nc_filename, NC_NOWRITE,
                     &(filenames.irrigation.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.irrigation.nc_filename);

    options.NIRRTYPES = get_nc_dimension(&(filenames.irrigation),
                                         "irr_classes");
    options.NIRRSEASONS = get_nc_dimension(&(filenames.irrigation),
                                           "grow_seasons");

    // close routing parameter file
    status = nc_close(filenames.irrigation.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.irrigation.nc_filename);
}

void
irr_validate_domain(void)
{
    extern filenames_struct filenames;

    int                     status;

    // open routing parameter file
    status = nc_open(filenames.irrigation.nc_filename, NC_NOWRITE,
                     &(filenames.irrigation.nc_id));
    check_nc_status(status, "Error opening %s",
                    filenames.irrigation.nc_filename);
    
    compare_ncdomain_with_global_domain(&filenames.irrigation);

    // close routing parameter file
    status = nc_close(filenames.irrigation.nc_id);
    check_nc_status(status, "Error closing %s",
                    filenames.irrigation.nc_filename);
}
