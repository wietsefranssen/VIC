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

#include <vic_driver_shared_image.h>
#include <ext_driver_shared_all.h>
#include <ext_debug.h>
#include <ext_mpi.h>

#define GW_DEFAULT_ZWT 50

typedef struct {
    char Qb_max[MAXSTRING];
    char Ka_expt[MAXSTRING];
    char Sy[MAXSTRING];
    char zwt_init[MAXSTRING];
    char Ws_init[MAXSTRING];
}ext_info_struct;

typedef struct {
    nameid_struct groundwater;
    
    ext_info_struct info;
}ext_filenames_struct;

void initialize_ext_info(ext_info_struct *);
void initialize_ext_filenames(ext_filenames_struct *);

void initialize_gw_info(ext_info_struct *info);
void initialize_gw_filenames(ext_filenames_struct *filenames);

void ext_put_data(timer_struct timer);

bool ext_set_nc_var_info(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file);
bool ext_set_nc_var_dimids(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file);
void initialize_nameid(nameid_struct *);

void ext_write(void);
void ext_set_nc_output_file_info(nc_file_struct *nc_output_file);
void ext_write_def_dim(void);
void ext_write_def_dimvar(void);
void ext_write_put_dimvar(void);

void ext_store(void);
void ext_set_nc_state_file_info(nc_file_struct *nc_state_file);
bool ext_set_nc_state_var_info(int statevar);
void ext_store_def_dim(void);
void ext_store_def_dimvar(void);
void ext_store_put_dimvar(void);

void ext_finalize(void);

void get_active_nc_field_double(nameid_struct *, char *, size_t *, size_t *, double *);
void get_active_nc_field_float(nameid_struct *, char *, size_t *, size_t *, float *);
void get_active_nc_field_int(nameid_struct *, char *, size_t *, size_t *, int *);

#endif


