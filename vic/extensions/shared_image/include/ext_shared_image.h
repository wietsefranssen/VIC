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

#endif


