/******************************************************************************
 * @section DESCRIPTION
 *
 * Save model state.
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
#include <routing_rvic.h>

/******************************************************************************
 * @brief    Save model state.
 *****************************************************************************/
void
routing_rvic_store(nc_file_struct *nc_state_file)
{
    extern int                 mpi_rank;
    extern routing_rvic_struct routing_rvic;
    extern node               *state_vars;

    int                        status;
    size_t                     d2start[2];
    nc_var_struct             *nc_var;

    // write state variables
    // routing ring
    if (mpi_rank == VIC_MPI_ROOT) {
        d2start[0] = 0;
        d2start[1] = 0;
        nc_var =
            &(nc_state_file->nc_vars[list_search_id(state_vars,
                                                    "STATE_ROUT_RING")]);

        status =
            nc_put_vara_double(nc_state_file->nc_id, nc_var->nc_varid, d2start,
                               nc_var->nc_counts,
                               routing_rvic.ring);
        check_nc_status(status, "Error writing values.");
    }
}

/******************************************************************************
 * @brief   Setup state file netcdf structure
 *****************************************************************************/
void
routing_rvic_set_nc_state_file_info(nc_file_struct *nc_state_file)
{
    extern routing_rvic_struct routing_rvic;

    // set ids to MISSING
    nc_state_file->outlet_dimid = MISSING;
    nc_state_file->routing_timestep_dimid = MISSING;

    // set dimension sizes
    nc_state_file->outlet_size = routing_rvic.rout_param.n_outlets;
    nc_state_file->routing_timestep_size =
        routing_rvic.rout_param.full_time_length;
}

/******************************************************************************
 * @brief   Setup state variable dimensions, types, etc.
 *****************************************************************************/
void
routing_rvic_set_nc_state_var_info(nc_file_struct *nc)
{
    size_t       j;
    extern node *state_vars;
    int          STATE_ROUT_RING = list_search_id(state_vars, "STATE_ROUT_RING");

    nc->nc_vars[STATE_ROUT_RING].nc_varid = STATE_ROUT_RING;
    for (j = 0; j < MAXDIMS; j++) {
        nc->nc_vars[STATE_ROUT_RING].nc_dimids[j] = -1;
        nc->nc_vars[STATE_ROUT_RING].nc_counts[j] = 0;
    }
    nc->nc_vars[STATE_ROUT_RING].nc_type = NC_DOUBLE;

    nc->nc_vars[STATE_ROUT_RING].nc_dims = 2;
    nc->nc_vars[STATE_ROUT_RING].nc_dimids[0] = nc->routing_timestep_dimid;
    nc->nc_vars[STATE_ROUT_RING].nc_dimids[1] = nc->outlet_dimid;
    nc->nc_vars[STATE_ROUT_RING].nc_counts[0] = nc->routing_timestep_size;
    nc->nc_vars[STATE_ROUT_RING].nc_counts[1] = nc->outlet_size;

    if (nc->nc_vars[STATE_ROUT_RING].nc_dims > MAXDIMS) {
        log_err("Too many dimensions specified in variable %d", STATE_ROUT_RING);
    }
}

/******************************************************************************
 * @brief   Initialize state file by creating dimensions, variables,
            and adding metadata.
 *****************************************************************************/
void
routing_rvic_initialize_state_file(char           *filename,
                                   nc_file_struct *nc_state_file)
{
    int status;

    // Add routing dimensions
    status = nc_def_dim(nc_state_file->nc_id, "outlet",
                        nc_state_file->outlet_size,
                        &(nc_state_file->outlet_dimid));
    check_nc_status(status, "Error defining outlet in %s", filename);

    status = nc_def_dim(nc_state_file->nc_id, "routing_timestep",
                        nc_state_file->routing_timestep_size,
                        &(nc_state_file->routing_timestep_dimid));
    check_nc_status(status, "Error defining routing_timestep in %s", filename);
}
