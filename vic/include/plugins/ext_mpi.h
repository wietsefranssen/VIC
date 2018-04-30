#ifndef EXT_MPI_H
#define EXT_MPI_H

#include <mpi.h>

enum {
    MPI_DECOMPOSITION_BASIN,
    MPI_DECOMPOSITION_RANDOM,
    MPI_DECOMPOSITION_FILE,
    MPI_NDECOMPOSITIONS
};

void initialize_ext_mpi();

bool mpi_get_global_parameters(char *cmdstr);
void mpi_validate_global_parameters(void);

void mpi_map_decomp_domain_basin(size_t ncells, size_t mpi_size,
                                 int **mpi_map_local_array_sizes,
                                 int **mpi_map_global_array_offsets,
                                 size_t **mpi_map_mapping_array);
void mpi_map_decomp_domain_file(size_t ncells, size_t mpi_size,
                                int **mpi_map_local_array_sizes,
                                int **mpi_map_global_array_offsets,
                                size_t **mpi_map_mapping_array);

#endif
