#include <ext_driver_shared_image.h>

void
ext_start(){
    extern domain_struct global_domain;
    extern ext_parameters_struct ext_param;
    extern ext_option_struct ext_options;
    extern size_t *cell_order_global;
    extern MPI_Datatype mpi_ext_option_struct_type;
    extern MPI_Datatype mpi_ext_param_struct_type;
    extern int mpi_decomposition;
    extern MPI_Comm            MPI_COMM_VIC;
    extern int mpi_rank;
    
    int status;
        
    if(ext_options.ROUTING){
        if(mpi_rank == VIC_MPI_ROOT){        
            cell_order_global = malloc(global_domain.ncells_active * 
                                         sizeof(*cell_order_global));
            check_alloc_status(cell_order_global, "Memory allocation error");   

            initialize_global_cell_order(cell_order_global);
        }  
    }
    
    status = MPI_Bcast(&ext_param, 1, mpi_ext_param_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    status = MPI_Bcast(&ext_options, 1, mpi_ext_option_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    status = MPI_Bcast(&mpi_decomposition, 1, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
}