/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   routing.h
 * Author: bram
 *
 * Created on September 5, 2017, 5:46 AM
 */

#ifndef ROUTING_H
#define ROUTING_H

#define M3_PER_HM3 (100*100*100)            /**< scalar - m3 per hm3 */
#define UH_STEPS_PER_TIMESTEP 50            /**< scalar - number of steps taken per timestep for precise UH calculation */

#define VIC_RESOLUTION 0.5                  /**< scalar - VIC resolution (FIXME: currently not saved in VIC) */
#define MAX_DAYS_UH 2                       /**< scalar - default maximum number of days an UH is allowed to discharge */
#define FLOW_VELOCITY_UH 1.5                /**< scalar - default flow velocity for UH calculation */
#define FLOW_DIFFUSIVITY_UH 800.0           /**< scalar - default flow diffusivity for UH calculation */

#define MAX_UPSTREAM 8

#define NODATA_BASIN 99999
#define NODATA_DIRECTION -1

typedef struct{
    short unsigned int direction;
    short unsigned int Nupstream;
    size_t downstream;
    size_t upstream[MAX_UPSTREAM];
}rout_con_struct;

typedef struct{
    size_t *basin_map;
    size_t *sorted_basins;
    size_t Nbasin;
    size_t *Ncells;
    size_t **catchment;
}basin_struct;

void get_basins(char *nc_name, basin_struct *basins);

#endif

