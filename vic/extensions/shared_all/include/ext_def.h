/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ext_def.h
 * Author: bram
 *
 * Created on September 7, 2017, 12:28 AM
 */

#ifndef EXT_DEF_H
#define EXT_DEF_H

#include <stdbool.h>
#include <groundwater.h>

#define VIC_RESOLUTION 0.5

// Remember to add variables to ext_mpi_support.c
typedef struct{
    
}ext_parameters_struct;

// Remember to add variables to ext_mpi_support.c
typedef struct{
    bool GROUNDWATER;
}ext_option_struct;

typedef struct{
    gw_var_struct **groundwater;
}ext_all_vars_struct;

void initialize_ext_mpi();
void initialize_ext_global_structures(void);
void initialize_ext_options(ext_option_struct *);
void initialize_ext_parameters(ext_parameters_struct *);
void initialize_ext_local_structures(void);
void initialize_ext_all_vars(ext_all_vars_struct *);

void initialize_gw_parameters(ext_parameters_struct *parameters);
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

#endif /* EXT_DEF_H */

