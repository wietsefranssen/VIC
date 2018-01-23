#ifndef GROUNDWATER_H
#define GROUNDWATER_H

#include <stddef.h>

#include <stdbool.h>

#define GW_REF_DEPTH 50
#define GW_DEF_ZWT_INIT 4.0

typedef struct{
    double recharge;        /**< drainage to groundwater (mm) */
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

bool gw_get_global_parameters(char *cmdstr);
void gw_validate_global_parameters(void);
void gw_alloc(void);
void initialize_gw_local_structures(void);
void gw_init(void);
void gw_output_metadata(void);
void gw_state_metadata(void);
void gw_generate_default_state(void);
void gw_restore(void);
void gw_put_data(void);
void gw_finalize(void);
void gw_add_types(void);

gw_var_struct ***gw_var;
gw_con_struct *gw_con;

#endif

