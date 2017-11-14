#ifndef GROUNDWATER_H
#define GROUNDWATER_H

#include <ext_driver_shared_image.h>

#define DRY_RESIST 1.e20

typedef struct{
    
}gw_var_struct;

typedef struct{
    
}gw_con_struct;

void initialize_gw_local_structures(void);
void initialize_gw_var(gw_var_struct *gw_var);
void initialize_gw_con(gw_con_struct *gw_con);

void gw_alloc(void);
void gw_start(void);
void validate_gw_parameters(void);
void get_groundwater_type(char *cmdstr);
void gw_set_output_meta_data_info(void);
void gw_set_state_meta_data_info(void);
void gw_init(void);
void gw_generate_default_state(void);
void gw_restore(void);
void gw_update_step_vars(void);
void gw_run(void);
void gw_put_data(void);
void gw_finalize(void);

void calculate_matric_potential();

#endif

