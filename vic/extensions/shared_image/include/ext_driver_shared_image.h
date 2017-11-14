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
#include <groundwater.h>
#include <ext_debug.h>
#include <ext_mpi.h>

// Remember to add variables to ext_mpi_support.c
typedef struct{
}ext_parameters_struct;

// Remember to add variables to ext_mpi_support.c
typedef struct{
    bool GROUNDWATER;
}ext_option_struct;

typedef struct {
}ext_info_struct;

typedef struct {
    nameid_struct groundwater;
    
    ext_info_struct info;
}ext_filenames_struct;

typedef struct{
    gw_var_struct *groundwater;
}ext_all_vars_struct;

void initialize_ext_mpi();
void initialize_ext_global_structures(void);
void initialize_ext_options(ext_option_struct *);
void initialize_ext_info(ext_info_struct *);
void initialize_nameid(nameid_struct *);
void initialize_ext_filenames(ext_filenames_struct *);
void initialize_ext_parameters(ext_parameters_struct *);
void initialize_ext_local_structures(void);
void initialize_ext_all_vars(ext_all_vars_struct *);

void initialize_gw_parameters(ext_parameters_struct *parameters);
void initialize_gw_info(ext_info_struct *info);
void initialize_gw_filenames(ext_filenames_struct *filenames);
void initialize_gw_options(ext_option_struct *options);

bool ext_get_global_param(char *opstr);
void ext_check_global_param(void);

bool ext_get_parameters(char *optstr);
void validate_ext_parameters(void);
void ext_start(void);
void ext_alloc(void);
void ext_init(void);

void ext_set_state_meta_data_info(void);
void ext_set_output_met_data_info(void);

void ext_populate_model_state(void);
void ext_generate_default_state(void);
void ext_restore(void);

void ext_force(void);
void ext_update_step_vars(void);
void ext_run(void);
void ext_put_data(timer_struct timer);

bool ext_set_nc_var_info(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file);
bool ext_set_nc_var_dimids(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file);

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


