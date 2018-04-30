/******************************************************************************
 * @section DESCRIPTION
 *
 * Allocate memory for VIC structures.
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
 * @brief    Allocate memory for VIC structures.
 *****************************************************************************/
void
vic_alloc(void)
{
    extern option_struct options;
    extern int           N_STATE_VARS_ALL;
    extern int           N_OUTVAR_TYPES_ALL;
    extern node         *outvar_types;
    extern node         *state_vars;
    extern int           mpi_rank;

    N_STATE_VARS_ALL = N_STATE_VARS;
    N_OUTVAR_TYPES_ALL = N_OUTVAR_TYPES;

    if (options.ROUTING_RVIC) {
        routing_rvic_add_types();
    }
    if (options.DAMS) {
        dam_add_types();
    }
    if (options.ROUTING) {
        rout_add_types();
    }
    if (options.IRRIGATION) {
        irr_add_types();
    }
    if (options.EFR) {
        efr_add_types();
    }
    if (options.WATER_USE) {
        wu_add_types();
    }
    if (options.GROUNDWATER) {
        gw_add_types();
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        list_print(outvar_types);
        list_print(state_vars);
    }

    // Allocate memory for all non specific VIC structures
    alloc_general();

    // Allocate memory for routing
    if (options.ROUTING_RVIC) {
        routing_rvic_alloc();
    }
    if (options.DAMS) {
        dam_alloc();
    }
    if (options.ROUTING) {
        rout_alloc();
    }
    if (options.EFR) {
        efr_alloc();
    }
    if (options.WATER_USE) {
        wu_alloc();
    }
    if (options.GROUNDWATER) {
        gw_alloc();
    }
}
