#include <ext_driver_shared_image.h>

void
ext_init(void){    
    extern ext_filenames_struct ext_filenames;
    extern ext_option_struct ext_options;
    extern int mpi_rank;
    
    int status;
    
    if(ext_options.ROUTING){
        routing_init();
    }
    
    // close external parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(ext_filenames.routing.nc_id);
        check_nc_status(status, "Error closing %s",
                        ext_filenames.routing.nc_filename);
    }
}
