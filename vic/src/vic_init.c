/******************************************************************************
 * @section DESCRIPTION
 *
 * Initialize model parameters
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
 * @brief    Initialize model parameters
 *****************************************************************************/
void
vic_init(void)
{
    extern option_struct     options;

    // Initialize all non specific VIC structures
    init_general();
    // Initialize routing
    if (options.ROUTING_RVIC) {
        routing_rvic_init();
    }
    if (options.DAMS) {
        initialize_dam_local_structures();
        dam_init();
    }
    if (options.ROUTING != ROUTING_FALSE) {
        initialize_rout_local_structures();
        rout_init();
    }
    if (options.EFR) {
        initialize_efr_local_structures();
    }
    if (options.WATER_USE) {
        initialize_wu_local_structures();
    }
    if (options.IRRIGATION) {
        irr_alloc();
        initialize_irr_local_structures();
        irr_init();
    }
    if (options.GROUNDWATER) {
        initialize_gw_local_structures();
        gw_init();
    }
}
