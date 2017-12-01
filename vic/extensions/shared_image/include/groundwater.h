#ifndef GROUNDWATER_H
#define GROUNDWATER_H

#define GW_REF_DEPTH 50
#define GW_DEF_ZWT_INIT 4.0

#include <stdbool.h>
typedef struct{
    double Qr;              /**< drainage to groundwater (mm) */    
    double Qs;              /**< drainage to storage (mm) */    
    double Wa;              /**< water stored in groundwater below soil column (compared to reference) (mm) */    
    double Wt;              /**< total water stored in groundwater (compared to reference) (mm) */    
    double Ws;              /**< water stored below soil column and above water table (mm) */    
    double zwt;             /**< depth of the water table (m) */    
}gw_var_struct;

typedef struct{
    double Qb_max;          /**< maximum subsurface flow rate (mm/day) */    
    double Ka_expt;         /**< exponent a in Niu et al. eqn (2007) for conductivity in groundwater aquifer */ 
    double Sy;              /**< specific yield of aquifer (m/m) */
}gw_con_struct;


void initialize_gw_local_structures(void);
void initialize_gw_var(gw_var_struct *gw_var);
void initialize_gw_con(gw_con_struct *gw_con);

void gw_alloc(void);
void gw_start(void);
bool gw_get_global_parameters(char *cmdstr);
void gw_validate_global_parameters(void);
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

