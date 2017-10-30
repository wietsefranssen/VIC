#include <ext_driver_shared_image.h>

void
ext_start(){
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    extern ext_option_struct ext_options;
    extern MPI_Datatype mpi_ext_option_struct_type;
    extern MPI_Datatype mpi_ext_param_struct_type;
    extern size_t *cell_order;
    extern int mpi_decomposition;
    extern MPI_Comm            MPI_COMM_VIC;
    extern int mpi_rank;
    
    int status;
    size_t i;
    
    if(mpi_rank == VIC_MPI_ROOT){     
        if(ext_options.ROUTING){            
            validate_ext_parameters();
            
            // Calculate derived option variables
            ext_options.uh_steps = global_param.model_steps_per_day * ext_param.UH_LENGTH;
            
            // Allocate global variables
            if(mpi_decomposition == RANDOM_DECOMPOSITION){
                cell_order = malloc(global_domain.ncells_active * sizeof(*cell_order));
                check_alloc_status(cell_order, "Memory allocation error");
                
                for(i=0;i<global_domain.ncells_active;i++){
                    cell_order[i] = MISSING_USI;
                }
            }
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
            
    if(ext_options.ROUTING){
        debug_basins();
        debug_node_domain();
    }
}