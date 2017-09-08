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

#ifndef EXT_DRIVER_SHARED_IMAGE_H
#define EXT_DRIVER_SHARED_IMAGE_H

#include <ext_driver_shared_all.h>
#include <ext_mpi.h>
#include <routing.h>
#include <dams.h>

typedef struct{
    bool ROUTING;
    bool DAMS;
}ext_option_struct;

typedef struct {
    char routing[MAXSTRING];
} ext_filenames_struct;

void ext_alloc(void);
void ext_init(void);
void ext_finalize(void);

#endif


