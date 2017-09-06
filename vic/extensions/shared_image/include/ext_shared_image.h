/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ext_shared_image.h
 * Author: bram
 *
 * Created on September 5, 2017, 5:44 AM
 */

#ifndef EXT_SHARED_IMAGE_H
#define EXT_SHARED_IMAGE_H

#include <vic_driver_shared_image.h>
#include <routing.h>
#include <dams.h>

typedef struct{
    bool ROUTING;
    bool DAMS;
}ext_option_struct;

typedef struct {
    char routing[MAXSTRING];
} ext_filenames_struct;

void initialise_extention_options(void);
void initialise_extension_filenames(void);
void ext_start(void);
void sizet_sort(size_t *array, size_t *cost, size_t Nelements, bool acending);
void sizet_sort2(size_t *array, int *cost, size_t Nelements, bool acending);
void sizet_swap(size_t i, size_t j, size_t *array);
void int_swap(size_t i, size_t j, int *array);
void make_basin_map_file(basin_struct *basins);

#endif


