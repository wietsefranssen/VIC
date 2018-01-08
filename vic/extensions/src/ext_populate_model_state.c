#include <ext_driver_shared_image.h>

/******************************************************************************
 * @brief    This function handles tasks related to populating model state.
 *****************************************************************************/
void
ext_populate_model_state()
{    
    extern option_struct    options;
    
    // read the model state from the netcdf file if there is one
    if (options.INIT_STATE) {
        ext_restore();
    }
    else{
        ext_generate_default_state();
    }
}

/******************************************************************************
 * @brief    Populate model state with default values
 *****************************************************************************/
void
ext_generate_default_state(void)
{
    extern ext_option_struct    ext_options;
    
    if(ext_options.GROUNDWATER){
        gw_generate_default_state();
    }
    if(ext_options.IRRIGATION){
        irr_generate_default_state();
    }
}

/******************************************************************************
 * @brief    Populate model state from state file
 *****************************************************************************/
void
ext_restore(){
    extern int                 mpi_rank;
    extern ext_option_struct    ext_options;
    extern filenames_struct    filenames;

    int                        status;    
    
    // open initial state file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(filenames.init_state.nc_filename, NC_NOWRITE,
                         &(filenames.init_state.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.init_state.nc_filename);
    }

    // validate state file dimensions and coordinate variables
    check_init_state_file();
        
    if(ext_options.GROUNDWATER){
        gw_restore();
    }

    // close initial state file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(filenames.init_state.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.init_state.nc_filename);
    }
}
