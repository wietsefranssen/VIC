/******************************************************************************
 * @section DESCRIPTION
 *
 * This subroutine initalizes all options before they are called by
 * the model.
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
 * @brief    Initialize all options before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_options()
{
    extern option_struct options;

    size_t               i;
    /** Initialize model option flags **/

    // simulation modes
    options.AboveTreelineVeg = -1;
    options.AERO_RESIST_CANSNOW = AR_406_FULL;
    options.BLOWING = false;
    options.BLOWING_VAR_THRESHOLD = true;
    options.BLOWING_CALC_PROB = true;
    options.BLOWING_SIMPLE = false;
    options.BLOWING_FETCH = true;
    options.BLOWING_SPATIAL_WIND = true;
    options.CARBON = false;
    options.CLOSE_ENERGY = false;
    options.COMPUTE_TREELINE = false;
    options.CONTINUEONERROR = true;
    options.CORRPREC = false;
    options.EQUAL_AREA = false;
    options.EXP_TRANS = true;
    options.FROZEN_SOIL = false;
    options.FULL_ENERGY = false;
    options.GRND_FLUX_TYPE = GF_410;
    options.IMPLICIT = true;
    options.LAKES = false;
    options.LAKE_PROFILE = false;
    options.NOFLUX = false;
    options.QUICK_FLUX = true;
    options.QUICK_SOLVE = false;
    options.RC_MODE = RC_JARVIS;
    options.SHARE_LAYER_MOIST = true;
    options.SNOW_DENSITY = DENS_BRAS;
    options.SPATIAL_FROST = false;
    options.SPATIAL_SNOW = false;
    options.TFALLBACK = true;
    // Model dimensions
    options.Ncanopy = 3;
    options.Nfrost = 1;
    options.Nlayer = 3;
    options.Nnode = 3;
    options.ROOT_ZONES = 0;
    options.ELEV_BAND = 1;
    // input options
    options.ALB_SRC = FROM_VEGLIB;
    options.BASEFLOW = ARNO;
    options.FCAN_SRC = FROM_DEFAULT;
    options.GRID_DECIMAL = 2;
    options.JULY_TAVG_SUPPLIED = false;
    options.LAI_SRC = FROM_VEGLIB;
    options.ORGANIC_FRACT = false;
    options.VEGLIB_FCAN = false;
    options.VEGLIB_PHOTO = false;
    options.VEGPARAM_ALB = false;
    options.VEGPARAM_FCAN = false;
    options.VEGPARAM_LAI = false;
    // state options
    options.STATE_FORMAT = UNSET_FILE_FORMAT;
    options.INIT_STATE = false;
    options.SAVE_STATE = false;
    // output options
    options.Noutstreams = 2;

    // plugins
    options.ROUTING_RVIC = false;
    options.ROUTING_LOHMANN = false;
    options.GROUNDWATER = false;
    options.ROUTING = false;
    options.WATER_USE = false;
    options.IRRIGATION = false;
    options.EFR = false;
    options.DAMS = false;

    // Groundwater options
    options.GW_INIT_FROM_FILE = false;
    // Routing options
    options.ROUTING_TYPE = ROUTING_FALSE;
    options.RIRF_NSTEPS = 0;
    options.GIRF_NSTEPS = 0;
    // Water use options
    options.WU_INPUT_FREQUENCY = WU_INPUT_MONTHLY;
    for (i = 0; i < WU_NSECTORS; i++) {
        options.WU_INPUT_LOCATION[i] = WU_INPUT_NONE;
        options.WU_RETURN_LOCATION[i] = WU_RETURN_SURFACEWATER;
        options.WU_COMPENSATION_TIME[i] = 0;
    }
    options.WU_NINPUT_FROM_FILE = 0;
    options.wu_force_offset = 0;
    // Irrigation options
    options.NIRRTYPES = 0;
    options.NIRRSEASONS = 0;
    // EFR options
    // Dam options
    options.MAXDAMS = 0;
}
