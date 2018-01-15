/******************************************************************************
 * @section DESCRIPTION
 *
 * clean up functions for routing extension
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
 * @brief    Finalize RVIC by freeing memory.
 *****************************************************************************/
void
routing_rvic_finalize(void)
{
    extern routing_rvic_struct routing_rvic;

    free(routing_rvic.rout_param.source2outlet_ind);
    free(routing_rvic.rout_param.source_time_offset);
    free(routing_rvic.rout_param.source_x_ind);
    free(routing_rvic.rout_param.source_y_ind);
    free(routing_rvic.rout_param.source_lat);
    free(routing_rvic.rout_param.source_lon);
    free(routing_rvic.rout_param.source_VIC_index);
    free(routing_rvic.rout_param.outlet_lat);
    free(routing_rvic.rout_param.outlet_lon);
    free(routing_rvic.rout_param.outlet_VIC_index);
    free(routing_rvic.rout_param.unit_hydrograph);
    free(routing_rvic.rout_param.aggrunin);
    free(routing_rvic.discharge);
    free(routing_rvic.ring);
}
