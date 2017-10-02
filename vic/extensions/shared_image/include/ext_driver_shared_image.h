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
#include <water_use.h>
#include <ext_mpi.h>
#include <debug.h>

typedef struct{
    double UH_FLOW_VELOCITY;
    double UH_FLOW_DIFFUSION;
    int UH_MAX_LENGTH;
    int UH_PARTITIONS;
    
    double MPI_N_PROCESS_COST;
    double MPI_E_PROCESS_COST;
    
    unsigned short int RETURN_LOCATION[WU_NSECTORS];
    unsigned short int RETURN_DELAY[WU_NSECTORS];
    unsigned short int COMPENSATION_DELAY[WU_NSECTORS];
        
    unsigned int forceoffset;
    unsigned short int wu_hist_offset;
}ext_parameters_struct;

typedef struct{
    
}ext_global_param_struct;

typedef struct{
    bool ROUTING;
    bool WATER_USE;
    bool DAMS;
    
    int UH_PARAMETERS;
}ext_option_struct;

typedef struct {
    char direction_var[MAXSTRING];
    char velocity_var[MAXSTRING];
    char diffusion_var[MAXSTRING];
    char distance_var[MAXSTRING];
    
    char irr_demand_var[MAXSTRING];
    char dom_demand_var[MAXSTRING];
    char ind_demand_var[MAXSTRING];
    char irr_cons_var[MAXSTRING];
    char dom_cons_var[MAXSTRING];
    char ind_cons_var[MAXSTRING];
    
    char dam_name_var[MAXSTRING];
    char dam_year_var[MAXSTRING];
    char dam_lat_var[MAXSTRING];
    char dam_lon_var[MAXSTRING];
    char dam_volume_var[MAXSTRING];
    char dam_area_var[MAXSTRING];
    char dam_height_var[MAXSTRING];
}ext_info_struct;

typedef struct {
    nameid_struct routing;
    nameid_struct water_use;
    nameid_struct dams;
    
    ext_info_struct info;
}ext_filenames_struct;

typedef struct{
    rout_var_struct rout_var;
    wu_var_struct *wu_var;
    dam_var_struct *dam_var;
}ext_all_vars_struct;

void initialize_ext_global_structures(void);
void initialize_ext_options(ext_option_struct *);
void initialize_ext_info(ext_info_struct *);
void initialize_nameid(nameid_struct *);
void initialize_ext_filenames(ext_filenames_struct *);
void initialize_ext_parameters(ext_parameters_struct *);

void initialize_global_cell_order(size_t *);

void initialize_ext_local_structures(void);
void initialize_rout_con(rout_con_struct *);
void initialize_wu_con(wu_con_struct **);
void initialize_wu_hist_struct(wu_hist_struct **);
void initialize_ext_all_vars(ext_all_vars_struct *);

void initialize_local_cell_order(size_t *);

void ext_start(void);
void ext_alloc(void);
void ext_init(void);
void ext_force(void);
void routing_update_step_vars(ext_all_vars_struct *);
void water_use_update_step_vars(ext_all_vars_struct *, wu_con_struct *, wu_hist_struct *);
void ext_run(void);
void ext_put_data(ext_all_vars_struct *ext_all_vars,
                double **out_data,
                timer_struct *timer);
void ext_finalize(void);

void routing_run_alloc(ext_all_vars_struct **ext_all_vars_global, rout_con_struct **rout_con_global, double **runoff_global);
void routing_run_gather(ext_all_vars_struct *ext_all_vars_global, rout_con_struct *rout_con_global, double *runoff_global);
void routing_run_scatter(ext_all_vars_struct *ext_all_vars_global);
void routing_run_free(ext_all_vars_struct *ext_all_vars_global, rout_con_struct *rout_con_global, double *runoff_global);
void routing_run(rout_con_struct rout_con, ext_all_vars_struct *ext_all_vars_this, ext_all_vars_struct *ext_all_vars, double runoff);

void water_use_run(ext_all_vars_struct *ext_all_vars, wu_con_struct *wu_con);

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


