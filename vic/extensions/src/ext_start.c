#include <ext_driver_shared_image.h>

void
ext_start(){
    extern ext_parameters_struct ext_param;
    extern ext_option_struct ext_options;
    extern ext_filenames_struct ext_filenames;
    extern MPI_Datatype mpi_ext_option_struct_type;
    extern MPI_Datatype mpi_ext_param_struct_type;
    extern MPI_Comm MPI_COMM_VIC;
    extern int mpi_rank;
    
    int status;
    
    if(mpi_rank == VIC_MPI_ROOT){      
        ext_validate_parameters();    
        
        ext_options.UH_NSTEPS = get_nc_dimension(&(ext_filenames.routing), 
                ext_filenames.info.uh_nsteps);
    }  
    
    status = MPI_Bcast(&ext_param, 1, mpi_ext_param_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    status = MPI_Bcast(&ext_options, 1, mpi_ext_option_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
}