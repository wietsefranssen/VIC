#ifndef GROUNDWATER_H
#define GROUNDWATER_H

#include <stdbool.h>

#define DRY_RESIST 1.e20
#define GW_REF_DEPTH 50

typedef struct{
    double Qr;
    double Qb;
    double Wa;
    double Wt;
    double zwt;
}gw_var_struct;

typedef struct{
    double Sy;
    double Qb_max;
    double Qb_expt;
    double Ka_expt;
    double Fp_expt;
}gw_con_struct;

void initialize_gw_local_structures(void);
void initialize_gw_var(gw_var_struct *gw_var);
void initialize_gw_con(gw_con_struct *gw_con);

void gw_alloc(void);
void gw_start(void);
bool gw_get_global_param(char *cmdstr);
void gw_check_global_param(void);
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
#endif

