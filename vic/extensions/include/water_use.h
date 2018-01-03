#ifndef WATER_USE_H
#define WATER_USE_H

#include <ext_driver_shared_image.h>

#define MAX_RETURN_STEPS 50
#define MAX_COMPENSATION_STEPS 100

enum{
    WU_IRRIGATION,
    WU_ENVIRONMENTAL,
    WU_NSECTORS
};

enum{
    WU_RETURN_GROUNDWATER,
    WU_RETURN_SURFACEWATER,
    WU_RETURN_NLOCATIONS
};

enum{
    WU_INPUT_YEARLY,
    WU_INPUT_MONTHLY,
    WU_INPUT_DAILY,
    WU_INPUT_NFREQUENCIES
};

typedef struct{
    double demand;
    double consumption_fraction;
    
    int return_location;
    int input_interval;
    size_t force_offset;
}wu_con_struct;

typedef struct{
    double withdrawn;
    double consumed;
    double compensated;
    
    double *return_flow;
    double *compensation;
    
    double available;
    double discharge;
}wu_var_struct;

void initialize_wu_local_structures(void);
void initialize_wu_con(wu_con_struct *);
void initialize_wu_var(wu_var_struct *);

void load_wu_forcing_data(size_t d3start[3], size_t d3count[3], double *dvar);

void water_use_update_step_vars(wu_var_struct *, size_t return_steps, size_t compensation_steps);
void water_use_put_data(wu_var_struct *,
        wu_con_struct *,
        double **out_data);
void water_use_run(wu_con_struct *, wu_var_struct *, rout_var_struct *);


#endif