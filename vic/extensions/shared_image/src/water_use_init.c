#include <ext_driver_shared_image.h>

void
water_use_init(){
    extern domain_struct local_domain;
    extern wu_con_struct **wu_con;
    extern ext_parameters_struct ext_param;
    extern ext_filenames_struct ext_filenames;
    extern int mpi_rank;
    
    int status;
    size_t i;
    size_t j;
    
    for(i=0;i<local_domain.ncells_active;i++){
        for(j=0;j<WU_NSECTORS;j++){
            wu_con[i][j].delay = ext_param.RETURN_DELAY[j];
            wu_con[i][j].return_location = ext_param.RETURN_LOCATION[j];
        }
    }
                
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(ext_filenames.water_use.nc_filename, NC_NOWRITE,
                         &(ext_filenames.water_use.nc_id));
        check_nc_status(status, "Error opening %s",
                        ext_filenames.water_use.nc_filename);
    }   
}