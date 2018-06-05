#ifndef GROUNDWATER_H
#define GROUNDWATER_H

#include <stdbool.h>

typedef struct {
    double recharge;        /**< drainage to groundwater (mm) */
    double Wa;              /**< water stored in groundwater below soil column (compared to reference) (mm) */
    double Wt;              /**< total water stored in groundwater (compared to reference) (mm) */
    double zwt;             /**< depth of the water table (m) */
    double available;       /**< available water (zwt - za_max) (m) */
} gw_var_struct;

typedef struct {
    double Qb_max;          /**< maximum subsurface flow rate (mm/day) */
    double Ka_expt;         /**< exponent a in Niu et al. eqn (2007) for conductivity in groundwater aquifer */
    double Qb_expt;         /**< exponent a in Niu et al. eqn (2007) for baseflow */
    double Sy;              /**< specific yield of aquifer (m/m) */
    double Za_max;          /**< maximum depth of aquifer (m) */
} gw_con_struct;

bool gw_get_global_parameters(char *cmdstr);
void gw_validate_domain(void);
void gw_validate_global_parameters(void);
void gw_alloc(void);
void initialize_gw_local_structures(void);
void gw_init(void);
void gw_set_output_meta_data_info(void);
void gw_set_state_meta_data_info(void);
bool gw_history(int, unsigned int *);
void gw_generate_default_state(void);
void gw_restore(void);
void gw_put_data(void);
void gw_finalize(void);
void gw_add_types(void);

gw_var_struct ***gw_var;
gw_con_struct   *gw_con;

#endif
