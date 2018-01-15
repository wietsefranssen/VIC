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

#include <vic.h>

/******************************************************************************
 * @brief    Read the VIC model global control file, getting values for
 *           global parameters, model options, and debugging controls.
 *****************************************************************************/
bool
routing_rvic_get_global_param(char *optstr, char *flgstr, char *cmdstr) {
    extern option_struct options;
    extern filenames_struct    filenames;

    if (strcasecmp("ROUTING", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", flgstr);
        if (strcasecmp("RVIC", flgstr) == 0) {
            options.ROUTING_RVIC = true;
            return 1;
        } else if (strcasecmp("OFF", flgstr) == 0) {
            options.ROUTING_RVIC = false;
            return 1;
        } else {
            return 0;
        }
    }
    else if (strcasecmp("ROUT_PARAM", optstr) == 0) {
        sscanf(cmdstr, "%*s %s", filenames.rout_params.nc_filename);
        return 1;
    }
    return 0;
}
