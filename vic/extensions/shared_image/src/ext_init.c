#include <ext_driver_shared_image.h>

void
mpi_init(){
    extern domain_struct local_domain;
    extern size_t *mpi_map_mapping_array_reverse;
    extern size_t *mpi_map_mapping_array;
    extern int *mpi_map_global_array_offsets;
    extern int mpi_rank;
    
    size_t global_active_id;
    size_t i;
    size_t k;      
    
    k=0;
    for(i=0;i<local_domain.ncells_active;i++){
        global_active_id = mpi_map_mapping_array[mpi_map_global_array_offsets[mpi_rank] + i];
        mpi_map_mapping_array_reverse[global_active_id] = i;             
        k++;
    }
}

void
ext_init(void){    
    mpi_init();
    routing_init();
}
