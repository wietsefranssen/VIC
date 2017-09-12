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
#include <ext_mpi.h>
#include <routing.h>

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
    char routing[MAXSTRING];
    
    ext_info_struct info;
}ext_filenames_struct;

typedef struct{
    rout_var_struct rout_var;
}ext_all_vars_struct;

void initialize_ext_global_structures(void);
void initialize_ext_options(ext_option_struct *options);
void initialize_ext_filenames(ext_filenames_struct *filenames);
void initialize_ext_parameters(ext_parameters_struct *parameters);

void ext_alloc(void);
void ext_init(void);
void ext_run(dmy_struct *dmy);
void ext_finalize(void);

#endif


