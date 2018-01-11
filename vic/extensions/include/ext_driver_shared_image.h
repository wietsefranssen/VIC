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

typedef struct {
    // Groundwater
    char Ka_expt[MAXSTRING];
    char Sy[MAXSTRING];
    char zwt_init[MAXSTRING];
    char Ws_init[MAXSTRING];
    // Routing
    char flow_direction[MAXSTRING];
    char rirf_nsteps[MAXSTRING];
    char river_irf[MAXSTRING];
    char girf_nsteps[MAXSTRING];
    char grid_irf[MAXSTRING];
    // Water use
    char demand[MAXSTRING];
    char consumption_fraction[MAXSTRING];
    // Irrigation    
    char nirr_dim[MAXSTRING];
    char nseason_dim[MAXSTRING];
    char nirr[MAXSTRING];
    char irrigated_class[MAXSTRING];
    char ponded_class[MAXSTRING];
    char nseason[MAXSTRING];
    char season_start[MAXSTRING];
    char season_end[MAXSTRING];
    // Dams
    char ndam_dim[MAXSTRING];
    char ndam[MAXSTRING];
    char dam_year[MAXSTRING];
    char dam_area[MAXSTRING];
    char dam_volume[MAXSTRING];
    char dam_height[MAXSTRING];
}ext_info_struct;

typedef struct {
    nameid_struct groundwater;
    nameid_struct routing;
    nameid_struct water_use;
    nameid_struct irrigation;
    nameid_struct dams;
    
    char water_use_forcing_pfx[MAXSTRING];
    
    ext_info_struct info;
}ext_filenames_struct;

void initialize_dmy(dmy_struct *);
void initialize_ext_info(ext_info_struct *);
void initialize_ext_filenames(ext_filenames_struct *);
void initialize_nameid(nameid_struct *);

void ext_put_data(timer_struct timer);

bool ext_set_nc_var_info(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file);
bool ext_set_nc_var_dimids(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file);
bool wu_set_nc_var_info(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file);
bool wu_set_nc_var_dimids(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file);

void ext_write(void);
void ext_set_nc_output_file_info(nc_file_struct *nc_output_file);
void ext_write_def_dim(nc_file_struct *nc_output_file, stream_struct *stream);
void ext_write_def_dimvar(nc_file_struct *nc_output_file, stream_struct *stream);
void ext_write_put_dimvar(nc_file_struct *nc_output_file, stream_struct *stream);
void wu_set_nc_output_file_info(nc_file_struct *nc_output_file);
void wu_write_def_dim(nc_file_struct *nc_output_file, stream_struct *stream);
void wu_write_def_dimvar(nc_file_struct *nc_output_file, stream_struct *stream);
void wu_write_put_dimvar(nc_file_struct *nc_output_file, stream_struct *stream);

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


