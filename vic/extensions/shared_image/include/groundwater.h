#ifndef GROUNDWATER_H
#define GROUNDWATER_H

#include <ext_driver_shared_image.h>

#define DRY_RESIST 1.e20

typedef struct{
    double Qr;
    double Qb;
    double Wa;
    double Ws;
    double zwt;
}gw_var_struct;

typedef struct{
    double Sa;
    double Qb_max;
    double Qb_expt;
    double Ka_expt;
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

void gw_runoff(cell_data_struct  *cell,
       energy_bal_struct *energy,
       gw_var_struct     *gw_var,
       soil_con_struct   *soil_con,
       gw_con_struct     *gw_con,
       double             ppt,
       double            *frost_fract,
       int                Nnodes);
void gw_surface_fluxes(bool                 overstory,
               double               BareAlbedo,
               double               ice0,
               double               moist0,
               double               surf_atten,
               double              *Melt,
               double              *Le,
               double              *aero_resist,
               double              *displacement,
               double              *gauge_correction,
               double              *out_prec,
               double              *out_rain,
               double              *out_snow,
               double              *ref_height,
               double              *roughness,
               double              *snow_inflow,
               double              *wind,
               double              *root,
               size_t               Nlayers,
               size_t               Nveg,
               unsigned short       band,
               double               dp,
               unsigned short       iveg,
               unsigned short       veg_class,
               force_data_struct   *force,
               dmy_struct          *dmy,
               energy_bal_struct   *energy,
               global_param_struct *gp,
               cell_data_struct    *cell,
               snow_data_struct    *snow,
               gw_var_struct       *gw_var,
               gw_con_struct       *gw_con,
               soil_con_struct     *soil_con,
               veg_var_struct      *veg_var,
               double               lag_one,
               double               sigma_slope,
               double               fetch,
               double              *CanopLayerBnd);

#endif

