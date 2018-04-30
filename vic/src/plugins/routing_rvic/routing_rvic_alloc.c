/******************************************************************************
 * @section DESCRIPTION
 *
 * Allocate memory for Routing structures.
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

#include <routing_rvic.h>

/******************************************************************************
 * @brief    Allocate memory for Routing structures.
 *****************************************************************************/
void
routing_rvic_alloc(void)
{
    extern int mpi_rank;

    if (mpi_rank == VIC_MPI_ROOT) {
        extern domain_struct       global_domain;
        extern routing_rvic_struct routing_rvic;
        int                        ivar;
        size_t                     d1count[1];
        size_t                     d1start[1];
        extern filenames_struct    filenames;
        int                        status;

        // open parameter file
        status = nc_open(filenames.rout_params.nc_filename, NC_NOWRITE,
                         &(filenames.rout_params.nc_id));
        check_nc_status(status, "Error opening %s",
                        filenames.rout_params.nc_filename);

        d1count[0] = 0;
        d1start[0] = 1;

        // Get some values and dimensions
        get_nc_field_int(&(filenames.rout_params),
                         "full_time_length",
                         d1start,
                         d1count,
                         &ivar);
        routing_rvic.rout_param.full_time_length = (int) ivar;

        routing_rvic.rout_param.n_timesteps =
            get_nc_dimension(&(filenames.rout_params),
                             "timesteps");
        routing_rvic.rout_param.n_outlets =
            get_nc_dimension(&(filenames.rout_params),
                             "outlets");
        routing_rvic.rout_param.n_sources =
            get_nc_dimension(&(filenames.rout_params),
                             "sources");

        // Allocate memory in rout param_struct
        routing_rvic.rout_param.source2outlet_ind = malloc(
            routing_rvic.rout_param.n_sources *
            sizeof(*routing_rvic.rout_param.source2outlet_ind));
        check_alloc_status(routing_rvic.rout_param.source2outlet_ind,
                           "Memory allocation error.");

        routing_rvic.rout_param.source_time_offset = malloc(
            routing_rvic.rout_param.n_sources *
            sizeof(*routing_rvic.rout_param.source_time_offset));
        check_alloc_status(routing_rvic.rout_param.source_time_offset,
                           "Memory allocation error.");

        routing_rvic.rout_param.source_x_ind = malloc(
            routing_rvic.rout_param.n_sources *
            sizeof(*routing_rvic.rout_param.source_x_ind));
        check_alloc_status(routing_rvic.rout_param.source_x_ind,
                           "Memory allocation error.");

        routing_rvic.rout_param.source_y_ind = malloc(
            routing_rvic.rout_param.n_sources *
            sizeof(*routing_rvic.rout_param.source_y_ind));
        check_alloc_status(routing_rvic.rout_param.source_y_ind,
                           "Memory allocation error.");

        routing_rvic.rout_param.source_lat = malloc(
            routing_rvic.rout_param.n_sources *
            sizeof(*routing_rvic.rout_param.source_lat));
        check_alloc_status(routing_rvic.rout_param.source_lat,
                           "Memory allocation error.");

        routing_rvic.rout_param.source_lon = malloc(
            routing_rvic.rout_param.n_sources *
            sizeof(*routing_rvic.rout_param.source_lon));
        check_alloc_status(routing_rvic.rout_param.source_lon,
                           "Memory allocation error.");

        routing_rvic.rout_param.source_VIC_index = malloc(
            routing_rvic.rout_param.n_sources *
            sizeof(*routing_rvic.rout_param.source_VIC_index));
        check_alloc_status(routing_rvic.rout_param.source_VIC_index,
                           "Memory allocation error.");

        routing_rvic.rout_param.outlet_lat = malloc(
            routing_rvic.rout_param.n_outlets *
            sizeof(*routing_rvic.rout_param.outlet_lat));
        check_alloc_status(routing_rvic.rout_param.outlet_lat,
                           "Memory allocation error.");

        routing_rvic.rout_param.outlet_lon = malloc(
            routing_rvic.rout_param.n_outlets *
            sizeof(*routing_rvic.rout_param.outlet_lon));
        check_alloc_status(routing_rvic.rout_param.outlet_lon,
                           "Memory allocation error.");

        routing_rvic.rout_param.outlet_VIC_index = malloc(
            routing_rvic.rout_param.n_outlets *
            sizeof(routing_rvic.rout_param.outlet_VIC_index));
        check_alloc_status(routing_rvic.rout_param.outlet_VIC_index,
                           "Memory allocation error.");

        routing_rvic.rout_param.unit_hydrograph = malloc(
            routing_rvic.rout_param.n_sources *
            routing_rvic.rout_param.n_timesteps *
            sizeof(*routing_rvic.rout_param.unit_hydrograph));
        check_alloc_status(routing_rvic.rout_param.unit_hydrograph,
                           "Memory allocation error.");

        routing_rvic.rout_param.aggrunin =
            malloc(
                global_domain.ncells_total *
                sizeof(*routing_rvic.rout_param.aggrunin));
        check_alloc_status(routing_rvic.rout_param.aggrunin,
                           "Memory allocation error.");

        routing_rvic.discharge =
            malloc(global_domain.ncells_total * sizeof(*routing_rvic.discharge));
        check_alloc_status(routing_rvic.discharge, "Memory allocation error.");

        // Allocate memory for the ring
        routing_rvic.ring = malloc(
            routing_rvic.rout_param.full_time_length *
            routing_rvic.rout_param.n_outlets *
            sizeof(*routing_rvic.ring));
        check_alloc_status(routing_rvic.ring, "Memory allocation error.");
    }
}
