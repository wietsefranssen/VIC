/******************************************************************************
 * @section DESCRIPTION
 *
 * This routine handles the startup tasks.
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#include <vic.h>

/******************************************************************************
 * @brief    Wrapper function for VIC domain tasks.
 *****************************************************************************/
void
vic_domain(void)
{
    int                        local_ncells_active;
    int                        status;
    location_struct           *mapped_locations = NULL;
    location_struct           *active_locations = NULL;
    size_t                     i;
    extern size_t             *filter_active_cells;
    extern size_t             *mpi_map_mapping_array;
    extern option_struct       options;
    extern filenames_struct    filenames;
    extern domain_struct       global_domain;
    extern domain_struct       local_domain;
    extern MPI_Comm            MPI_COMM_VIC;
    extern MPI_Datatype        mpi_location_struct_type;
    extern int                *mpi_map_local_array_sizes;
    extern int                *mpi_map_global_array_offsets;
    extern int                 mpi_rank;
    extern int                 mpi_size;
    extern int                 mpi_decomposition;
    size_t                     j;
    
    if (mpi_rank == VIC_MPI_ROOT) {
        // open domain file
        status = nc_open(filenames.domain.nc_filename, NC_NOWRITE,
                         &(filenames.domain.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.domain.nc_filename);
        
        // read domain info
        get_global_domain(&(filenames.domain), &global_domain);
        
        // close domain file
        status = nc_close(filenames.domain.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.domain.nc_filename);
        
        // Validate domain of all non specific VIC structures
        validate_domain();
        
        // Validate domain of all plugins
        if (options.GROUNDWATER) {
            gw_validate_domain();
        }
        if (options.ROUTING) {
            rout_validate_domain();
        }
        if (options.DAMS) {
            dam_validate_domain();
        }
        if (options.IRRIGATION) {
            irr_validate_domain();
        }
        if (options.WATER_USE) {
            wu_validate_domain();
        }
        
        // open parameter file
        status = nc_open(filenames.params.nc_filename, NC_NOWRITE,
                         &(filenames.params.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.params.nc_filename);
        
        // add the number of vegetation type to the location info in the
        // global domain struct. This just makes life easier
        add_nveg_to_global_domain(&(filenames.params), &global_domain);
        // add the number of elevation bands to the location info in the
        // global domain struct. This just makes life easier
        add_nelev_to_global_domain(&(filenames.params), &global_domain);

        // close parameter file
        status = nc_close(filenames.params.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.params.nc_filename);
        
        // get the indices for the active cells (used in reading and writing)
        filter_active_cells = malloc(global_domain.ncells_active *
                                     sizeof(*filter_active_cells));
        check_alloc_status(filter_active_cells, "Memory allocation error");

        j = 0;
        for (i = 0; i < global_domain.ncells_total; i++) {
            if (global_domain.locations[i].run) {
                filter_active_cells[j] = global_domain.locations[i].io_idx;
                j++;
            }
        }
        
        // decompose the mask
        if (mpi_decomposition == MPI_DECOMPOSITION_RANDOM) {
            mpi_map_decomp_domain(global_domain.ncells_active, mpi_size,
                                  &mpi_map_local_array_sizes,
                                  &mpi_map_global_array_offsets,
                                  &mpi_map_mapping_array);
        }
        else if (mpi_decomposition == MPI_DECOMPOSITION_BASIN) {
            mpi_map_decomp_domain_basin(global_domain.ncells_active, mpi_size,
                                        &mpi_map_local_array_sizes,
                                        &mpi_map_global_array_offsets,
                                        &mpi_map_mapping_array);
        }
        else if (mpi_decomposition == MPI_DECOMPOSITION_FILE) {
            mpi_map_decomp_domain_file(global_domain.ncells_active, mpi_size,
                                       &mpi_map_local_array_sizes,
                                       &mpi_map_global_array_offsets,
                                       &mpi_map_mapping_array);
        }
        else {
            log_err("Unknown mpi decomposition method");
        }
        
        // log decomposition
        for (i = 0; i < (size_t)mpi_size; i++) {
            log_info(
                "Mpi decomposition node %zu: %d - %d (%.3f) [offset - size (fraction of total)]",
                i, mpi_map_global_array_offsets[i],
                mpi_map_local_array_sizes[i],
                ((float)mpi_map_local_array_sizes[i] /
                      (float)global_domain.ncells_active));
        }
        for (i = 0; i < (size_t)mpi_size; i++) {
            if (mpi_map_local_array_sizes[i] <= 0) {
                log_err(
                    "Mpi decomposition size node %zu <= 0; please check your decomposition method",
                    i);
            }
        }
    }

    // broadcast mpi_decomposition and size
    status = MPI_Bcast(&mpi_decomposition, 1, MPI_INT, VIC_MPI_ROOT,
                       MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    status = MPI_Scatter(mpi_map_local_array_sizes, 1, MPI_INT,
                         &local_ncells_active, 1, MPI_INT, VIC_MPI_ROOT,
                         MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    // setup the local domain_structs
    local_domain.ncells_active = (size_t) local_ncells_active;
    local_domain.locations = malloc(local_domain.ncells_active *
                                    sizeof(*local_domain.locations));
    check_alloc_status(local_domain.locations, "malloc error");
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_location(&(local_domain.locations[i]));
    }

    // map the location vector to a temporary array so they can be scattered
    if (mpi_rank == VIC_MPI_ROOT) {
        mapped_locations = malloc(global_domain.ncells_active *
                                  sizeof(*mapped_locations));
        check_alloc_status(mapped_locations, "malloc error");
        
        for (i = 0; i < global_domain.ncells_active; i++) {
            initialize_location(&(mapped_locations[i]));
        }

        active_locations = (location_struct *) malloc(
            global_domain.ncells_active * sizeof(location_struct));
        check_alloc_status(active_locations, "malloc error");
        
        for (i = 0; i < global_domain.ncells_active; i++) {
            initialize_location(&(active_locations[i]));
        }

        for (i = 0, j = 0; i < global_domain.ncells_total; i++) {
            if (global_domain.locations[i].run) {
                active_locations[j] = global_domain.locations[i];
                j++;
            }
        }

        map(sizeof(location_struct), global_domain.ncells_active,
            mpi_map_mapping_array, NULL, active_locations,
            mapped_locations);
    }

    // Scatter the locations
    status = MPI_Scatterv(mapped_locations, mpi_map_local_array_sizes,
                          mpi_map_global_array_offsets,
                          mpi_location_struct_type,
                          local_domain.locations, local_domain.ncells_active,
                          mpi_location_struct_type,
                          VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    // Set the local index value
    for (i = 0; i < (size_t) local_domain.ncells_active; i++) {
        local_domain.locations[i].local_idx = i;
    }

    // cleanup
    if (mpi_rank == VIC_MPI_ROOT) {
        free(mapped_locations);
        free(active_locations);
    }
}