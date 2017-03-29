/******************************************************************************
 * @section DESCRIPTION
 *  
 * Functions used in mpi
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Scatter double variable across nodes
 ******************************************************************************/
void
scatter_var_double(double *dvar,
                   double *local_var)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_map_global_array_offsets;
    extern int          *mpi_map_local_array_sizes;
    extern size_t       *filter_active_cells;
    extern size_t       *mpi_map_mapping_array;
    int                  status;
    double              *dvar_filtered = NULL;
    double              *dvar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        dvar_filtered =
            malloc(global_domain.ncells_active * sizeof(*dvar_filtered));
        check_alloc_status(dvar_filtered, "Memory allocation error.");

        dvar_mapped =
            malloc(global_domain.ncells_active * sizeof(*dvar_mapped));
        check_alloc_status(dvar_mapped, "Memory allocation error.");

        // filter the active cells only
        map(sizeof(double), global_domain.ncells_active, filter_active_cells,
            NULL, dvar, dvar_filtered);
        // map to prepare for MPI_Scatterv
        map(sizeof(double), global_domain.ncells_active, mpi_map_mapping_array,
            NULL, dvar_filtered, dvar_mapped);
        free(dvar_filtered);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(dvar_mapped, mpi_map_local_array_sizes,
                          mpi_map_global_array_offsets, MPI_DOUBLE,
                          local_var, local_domain.ncells_active, MPI_DOUBLE,
                          VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(dvar_mapped);
    }
}

/******************************************************************************
 * @section brief
 *  
 * Gather double variable from nodes
 ******************************************************************************/
void
gather_var_double(double *dvar,
                  double *local_var)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_map_global_array_offsets;
    extern int          *mpi_map_local_array_sizes;
    extern size_t       *filter_active_cells;
    extern size_t       *mpi_map_mapping_array;
    int                  status;
    double              *dvar_gathered = NULL;
    double              *dvar_remapped = NULL;
    size_t               grid_size;
    size_t               i;

    if (mpi_rank == VIC_MPI_ROOT) {
        grid_size = global_domain.n_nx * global_domain.n_ny;
        for (i = 0; i < grid_size; i++) {
            dvar[i] = 0;
        }

        dvar_gathered =
            malloc(global_domain.ncells_active * sizeof(*dvar_gathered));
        check_alloc_status(dvar_gathered, "Memory allocation error.");

        dvar_remapped =
            malloc(global_domain.ncells_active * sizeof(*dvar_remapped));
        check_alloc_status(dvar_remapped, "Memory allocation error.");
    }

    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(local_var, local_domain.ncells_active, MPI_DOUBLE,
                         dvar_gathered, mpi_map_local_array_sizes,
                         mpi_map_global_array_offsets, MPI_DOUBLE,
                         VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof(double), global_domain.ncells_active, NULL,
            mpi_map_mapping_array, dvar_gathered, dvar_remapped);
        // expand to full grid size
        map(sizeof(double), global_domain.ncells_active, NULL,
            filter_active_cells, dvar_remapped, dvar);

        // cleanup
        free(dvar_gathered);
        free(dvar_remapped);
    }
}