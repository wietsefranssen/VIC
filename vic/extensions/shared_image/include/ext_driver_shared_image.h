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
#include <dam.h>
#include <ext_mpi.h>
#include <debug.h>

// Remember to add variables to ext_mpi_support.c
typedef struct{
    double MPI_N_PROCESS_COST;
    double MPI_E_PROCESS_COST;
    
    double UH_FLOW_VELOCITY;
    double UH_FLOW_DIFFUSION;
    int UH_LENGTH;
    int UH_PARTITIONS;    
    
    int DAM_HISTORY;    
    int DAM_HISTORY_LENGTH;
}ext_parameters_struct;

// Remember to add variables to ext_mpi_support.c
typedef struct{
    bool ROUTING;
    bool DAMS;
    
    int UH_PARAMETERS;
    
    size_t uh_steps;
    size_t history_steps;
    size_t model_steps_per_history_step;
    size_t history_steps_per_history_year;
    size_t ndams;
}ext_option_struct;

typedef struct {
    char direction_var[MAXSTRING];
    char velocity_var[MAXSTRING];
    char diffusion_var[MAXSTRING];
    char distance_var[MAXSTRING];
    
    char ndam_var[MAXSTRING];
    char dam_year_var[MAXSTRING];
    char dam_lat_var[MAXSTRING];
    char dam_lon_var[MAXSTRING];
    char dam_volume_var[MAXSTRING];
    char dam_area_var[MAXSTRING];
    char dam_height_var[MAXSTRING];
}ext_info_struct;

typedef struct {
    nameid_struct routing;
    nameid_struct dams;
    
    ext_info_struct info;
}ext_filenames_struct;

typedef struct{
    rout_var_struct rout_var;
    dam_var_struct *dam_var;
}ext_all_vars_struct;

// Master node initialization
void initialize_ext_global_structures(void);
void initialize_ext_options(ext_option_struct *);
void initialize_ext_info(ext_info_struct *);
void initialize_nameid(nameid_struct *);
void initialize_ext_filenames(ext_filenames_struct *);
void initialize_ext_parameters(ext_parameters_struct *);
void initialize_global_cell_order(size_t *);
// Local node initialization
void initialize_ext_local_structures(void);

// Preperations
void ext_start(void);
void validate_ext_parameters(void);
void ext_alloc(void);
void ext_init(void);
void ext_set_state_meta_data_info(void);
void ext_set_output_met_data_info(void);
void ext_populate_model_state(void);
void generate_default_routing_state(ext_all_vars_struct *);
void generate_default_dams_state(ext_all_vars_struct *ext_all_vars, 
        dam_con_struct *dam_con, 
        dam_con_map_struct dam_con_map,
        veg_con_map_struct veg_con_map, 
        veg_con_struct *veg_con, 
        location_struct location);
void ext_restore(void);
// Run
void ext_force(void);
void ext_run(dmy_struct dmy);
void ext_put_data(ext_all_vars_struct *, dam_con_struct *dam_con, dam_con_map_struct dam_con_map, double **, timer_struct *);
// Finalizations
void ext_finalize(void);

void routing_run_alloc(ext_all_vars_struct **ext_all_vars_global, rout_con_struct **rout_con_global, double **runoff_global);
void routing_run_gather(ext_all_vars_struct *ext_all_vars_global, rout_con_struct *rout_con_global, double *runoff_global);
void routing_run_scatter(ext_all_vars_struct *ext_all_vars_global);
void routing_run_free(ext_all_vars_struct *ext_all_vars_global, rout_con_struct *rout_con_global, double *runoff_global);

void routing_run(rout_con_struct rout_con, 
        ext_all_vars_struct *ext_all_vars_this, 
        ext_all_vars_struct *ext_all_vars, 
        double runoff);
void dam_run(dam_con_struct dam_con, 
        dam_var_struct *dam_var, 
        rout_var_struct *rout_var,
        veg_con_struct *veg_con, 
        veg_con_map_struct veg_con_map,
        location_struct location, 
        dmy_struct dmy);

void get_active_nc_field_double(nameid_struct *, char *, size_t *, size_t *, double *);
void get_active_nc_field_float(nameid_struct *, char *, size_t *, size_t *, float *);
void get_active_nc_field_int(nameid_struct *, char *, size_t *, size_t *, int *);

#endif


