#ifndef EXT_DEF_H
#define EXT_DEF_H

#include <stdbool.h>
#include <groundwater.h>
#include <routing.h>
#include <water_use.h>

#define VIC_RESOLUTION 0.5

// Remember to add variables to ext_mpi_support.c
typedef struct{
    
}ext_parameters_struct;

// Remember to add variables to ext_mpi_support.c
typedef struct{
    bool GROUNDWATER;
    bool ROUTING;
    bool WATER_USE;
    
    bool GW_INIT_FROM_FILE;    
    size_t UH_NSTEPS;
    int WU_INPUT_FREQUENCY;
    int WU_INPUT_LOCATION[WU_NSECTORS];
    int WU_RETURN_LOCATION[WU_NSECTORS];
    int WU_COMPENSATION_TIME[WU_NSECTORS];
    size_t WU_NINPUT_FROM_FILE;
    
    size_t wu_force_offset;
}ext_option_struct;

typedef struct{
    gw_var_struct **groundwater;
    rout_var_struct routing;
    wu_var_struct *water_use;
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
void ext_validate_global_parameters(void);

bool ext_get_parameters(char *optstr);
void ext_validate_parameters(void);
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

