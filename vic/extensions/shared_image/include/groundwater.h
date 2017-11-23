#ifndef GROUNDWATER_H
#define GROUNDWATER_H

#include <stdbool.h>

#define DRY_RESIST 1.e20
#define GW_REF_DEPTH 50
#define GW_DEF_ZWT_INIT 10.0

typedef struct{
    double Qr;
    double Wa;
    double Wt;
    double zwt;
}gw_var_struct;

typedef struct{
    double Qb_max;          /**< maximum subsurface flow rate (mm/day) */
    double Sy;              /**< specific yield of aquifer */
    double Ka_expt;         /**< exponent a in Niu et al. eqn (2007) for conductivity in groundwater aquifer */
}gw_con_struct;

void initialize_gw_local_structures(void);
void initialize_gw_var(gw_var_struct *gw_var);
void initialize_gw_con(gw_con_struct *gw_con);

void gw_alloc(void);
void gw_start(void);
bool gw_get_global_parameters(char *cmdstr);
void gw_validate_global_parameters(void);
void gw_validate_parameters(void);
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
#endif

