#ifndef EXT_MPI_H
#define EXT_MPI_H

#include <ext_driver_shared_image.h>
#include <vic_mpi.h>

enum{
    CALCULATE_DECOMPOSITION,
    BASIN_DECOMPOSITION,
    RANDOM_DECOMPOSITION
};

typedef struct{
    size_t *basin_map;
    size_t *sorted_basins;
    size_t Nbasin;
    size_t *Ncells;
    size_t **catchment;
}basin_struct;

void get_basins(char *nc_name, basin_struct *basins);
void mpi_map_decomp_domain(size_t ncells, size_t mpi_size,
                           int **mpi_map_local_array_sizes,
                           int **mpi_map_global_array_offsets,
                           size_t **mpi_map_mapping_array,
                           size_t **mpi_map_mapping_array_reverse);
void mpi_map_decomp_domain_random(size_t ncells, size_t mpi_size,
                           int **mpi_map_local_array_sizes,
                           int **mpi_map_global_array_offsets,
                           size_t **mpi_map_mapping_array);
void mpi_map_decomp_domain_basin(size_t ncells, size_t mpi_size,
                           int **mpi_map_local_array_sizes,
                           int **mpi_map_global_array_offsets,
                           size_t **mpi_map_mapping_array,
                           basin_struct *basins);

#endif /* EXT_MPI_H */

