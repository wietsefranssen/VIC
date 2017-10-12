/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   debug.h
 * Author: bram
 *
 * Created on September 15, 2017, 5:41 AM
 */

#include <ext_driver_shared_image.h>

#ifndef DEBUG_H
#define DEBUG_H

void debug_nc_int(char *path, int *data, int fill_value);
void debug_nc_sizet(char *path, size_t *data, size_t fill_value);
void debug_file_int(char *path, int *data);
void debug_file_sizet(char *path, size_t *data);
void debug_nupstream();
void debug_downstream();
void debug_id();
void debug_basins();
void debug_node_domain();
void debug_basins2();

#endif /* DEBUG_H */

