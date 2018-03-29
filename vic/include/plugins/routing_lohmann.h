/******************************************************************************
 * @section DESCRIPTION
 *
 * Header file for rvic routing routines
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
#ifndef ROUTING_LOHMANN_H
#define ROUTING_LOHMANN_H

#include <vic_def.h>
#include <vic.h>

/******************************************************************************
 * @brief   Routing Structs
 *****************************************************************************/
typedef struct {
    size_t full_time_length;                  /*scalar - total number of timesteps*/
    size_t n_timesteps;                        /*scalar - number of timesteps*/
    size_t n_sources;                          /*scalar - number of sources*/
    size_t n_outlets;                          /*scalar - length of subset*/
    size_t *source2outlet_ind;                /*1d array - source to outlet mapping*/
    int *source_y_ind;                        /*1d array - source y location*/
    int *source_x_ind;                        /*1d array - source x location*/
    double *source_lat;                       /*1d array - Latitude coordinate of source grid cell*/
    double *source_lon;                       /*1d array - Longitude coordinate of source grid cell*/
    double *outlet_lat;                       /*1d array - Latitude coordinate of outlet grid cell*/
    double *outlet_lon;                       /*1d array - Longitude coordinate of outlet grid cell*/
    int *source_VIC_index;                    /*1d array - mapping of routing-source index to VIC index*/
    int *outlet_VIC_index;                    /*1d array - mapping of routing-outlet index to VIC index*/
    int *source_time_offset;                  /*1d array - source time offset*/
    double *unit_hydrograph;                  /*2d array[times][sources] - unit hydrographs*/
    double *aggrunin;                         /*2d array[ysize][xsize] - vic runoff flux*/
} routing_lohmann_param_struct;

/******************************************************************************
 * @brief   main routing Struct
 *****************************************************************************/
typedef struct {
    routing_lohmann_param_struct rout_param;
    double *ring;
    double *discharge;
} routing_lohmann_struct;

bool routing_lohmann_get_global_param(char *);

#endif