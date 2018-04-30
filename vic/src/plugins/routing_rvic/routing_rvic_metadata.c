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
routing_rvic_output_metadata()
{
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;

    /* river discharge [m3 s-1] */
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_DISCHARGE")].varname,
           "OUT_DISCHARGE");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_DISCHARGE")].long_name,
           "water_volume_transport_in_river_channel");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_DISCHARGE")].standard_name,
           "river_discharge");
    strcpy(out_metadata[list_search_id(outvar_types,
                                       "OUT_DISCHARGE")].units, "m3 s-1");
    strcpy(
        out_metadata[list_search_id(outvar_types,
                                    "OUT_DISCHARGE")].description,
        "The water flux or volume transport in rivers is the amount of water flowing in the river channel and flood plain. 'Water' means water in all phases");
}

/******************************************************************************
 * @brief    Save model state.
 *****************************************************************************/
void
routing_rvic_state_metadata()
{
    extern metadata_struct *state_metadata;
    extern node            *state_vars;

    // STATE_ROUT_RING
    strcpy(state_metadata[list_search_id(state_vars,
                                         "STATE_ROUT_RING")].varname,
           "STATE_ROUT_RING");
    strcpy(state_metadata[list_search_id(state_vars,
                                         "STATE_ROUT_RING")].long_name,
           "routing_ring");
    strcpy(state_metadata[list_search_id(state_vars,
                                         "STATE_ROUT_RING")].standard_name,
           "routing_ring");
    strcpy(state_metadata[list_search_id(state_vars,
                                         "STATE_ROUT_RING")].units, "-");
    strcpy(state_metadata[list_search_id(state_vars,
                                         "STATE_ROUT_RING")].description,
           "unit hydrographs in the routing ring");
}
