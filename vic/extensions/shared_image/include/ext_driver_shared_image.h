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
#include <routing.h>
#include <ext_mpi.h>
#include <debug.h>

typedef struct{
    double UH_FLOW_VELOCITY;
    double UH_FLOW_DIFFUSION;
    int UH_MAX_LENGTH;
    int UH_PARTITIONS;
    
    double MPI_N_PROCESS_COST;
    double MPI_E_PROCESS_COST;
}ext_parameters_struct;

typedef struct{
    bool ROUTING;    
    
    int UH_PARAMETERS;
}ext_option_struct;

typedef struct {
    char direction_var[MAXSTRING];
    char velocity_var[MAXSTRING];
    char diffusion_var[MAXSTRING];
    char distance_var[MAXSTRING];
}ext_info_struct;

typedef struct {
    nameid_struct routing;
    
    ext_info_struct info;
}ext_filenames_struct;

typedef struct{
    rout_var_struct rout_var;
}ext_all_vars_struct;

void initialize_ext_global_structures(void);
void initialize_ext_options(ext_option_struct *);
void initialize_ext_info(ext_info_struct *);
void initialize_ext_filenames(ext_filenames_struct *);
void initialize_ext_parameters(ext_parameters_struct *);

void initialize_global_cell_order(size_t *);

void initialize_ext_local_structures(void);
void initialize_rout_con(rout_con_struct *);
void initialize_ext_all_vars(ext_all_vars_struct *);
void initialize_local_cell_order(size_t *);

void ext_start(void);
void ext_alloc(void);
void ext_init(void);
void ext_update_step_vars(ext_all_vars_struct *);
void ext_run();
void ext_put_data();
void ext_finalize(void);

void routing_gather(rout_con_struct *rout_con, ext_all_vars_struct *ext_all_vars, double *runoff);

void get_active_nc_field_double(nameid_struct   *nc_nameid,
                    char   *var_name,
                    size_t *start,
                    size_t *count,
                    double *var);
void get_active_nc_field_float(nameid_struct   *nc_nameid,
            char   *var_name,
            size_t *start,
            size_t *count,
            float *var);
void get_active_nc_field_int(nameid_struct   *nc_nameid,
            char   *var_name,
            size_t *start,
            size_t *count,
            int *var);

#endif


