#ifndef EXT_MPI_H
#define EXT_MPI_H

#include <vic_mpi.h>
#include <ext_driver_shared_image.h>

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

void initialize_ext_mpi();
void create_MPI_ext_option_struct_type(MPI_Datatype *mpi_type);
void create_MPI_ext_filenames_struct_type(MPI_Datatype *mpi_type);
void create_MPI_ext_parameters_struct_type(MPI_Datatype *mpi_type);
void create_MPI_ext_info_struct_type(MPI_Datatype *mpi_type);

void mpi_init();
void initialize_basins(basin_struct *);
        
void get_basins(char *nc_name, char *direction_var, basin_struct *basins);
void mpi_map_decomp_domain(size_t ncells, size_t mpi_size,
                           int **mpi_map_local_array_sizes,
                           int **mpi_map_global_array_offsets,
                           size_t **mpi_map_mapping_array);
void mpi_map_decomp_domain_random(size_t ncells, size_t mpi_size,
                           int **mpi_map_local_array_sizes,
                           int **mpi_map_global_array_offsets,
                           size_t **mpi_map_mapping_array);
void mpi_map_decomp_domain_basin(size_t ncells, size_t mpi_size,
                           int **mpi_map_local_array_sizes,
                           int **mpi_map_global_array_offsets,
                           size_t **mpi_map_mapping_array,
                           basin_struct *basins);

void gather_double(double *dvar, double *var_local);
void gather_double_2d(double **dvar, double **var_local, int depth);
void gather_int(int *ivar, int *var_local);
void gather_int_2d(int *ivar, int *var_local, int depth);
void gather_sizet(size_t *svar, size_t *var_local);
void gather_sizet_2d(size_t **svar, size_t **var_local, int depth);
void scatter_double(double *dvar, double *var_local);
void scatter_double_2d(double *dvar, double *var_local, int depth);
void scatter_int(int *ivar, int *var_local);
void scatter_int_2d(int *ivar, int *var_local, int depth);
void scatter_sizet(size_t *svar, size_t *var_local);
void scatter_sizet_2d(size_t **svar, size_t **var_local, int depth);


#endif /* EXT_MPI_H */

