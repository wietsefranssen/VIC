/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ext_mpi.h
 * Author: bram
 *
 * Created on September 7, 2017, 12:08 AM
 */

#ifndef EXT_MPI_H
#define EXT_MPI_H

#include <vic_driver_shared_image.h>
#include <ext_driver_shared_image.h>

enum{
    BASIN_DECOMPOSITION,
    RANDOM_DECOMPOSITION
};

typedef struct{
    int decomposition_method;
}ext_mpi_option_struct;

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

