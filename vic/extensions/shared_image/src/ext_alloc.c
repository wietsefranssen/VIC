#include <ext_driver_shared_image.h>


void
routing_alloc()
{    
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern rout_con_struct *rout_con;
    extern ext_mpi_option_struct ext_mpi_options;
    extern int mpi_rank;
       
    if(ext_mpi_options.decomposition_method==BASIN_DECOMPOSITION){
        rout_con = malloc(local_domain.ncells_active * sizeof(*rout_con));
        check_alloc_status(rout_con, "Memory allocation error"); 
        
    } else if(ext_mpi_options.decomposition_method==RANDOM_DECOMPOSITION && 
            mpi_rank==VIC_MPI_ROOT){
        rout_con = malloc(global_domain.ncells_active * sizeof(*rout_con));
        check_alloc_status(rout_con, "Memory allocation error");
    }
}

void
ext_alloc(void)
{
    routing_alloc();
}