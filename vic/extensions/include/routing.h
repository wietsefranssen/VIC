#ifndef ROUTING_H
#define ROUTING_H

#include <ext_driver_shared_image.h>

#define MAX_UPSTREAM 8

enum{
    CONSTANT_UH_PARAMETERS,
    FILE_UH_PARAMETERS,
    FILE_UH_TOTAL
};

typedef struct{
    size_t *basin_map;
    size_t *sorted_basins;
    size_t Nbasin;
    size_t *Ncells;
    size_t **catchment;
}basin_struct;

typedef struct{
    int direction;
    size_t downstream;
    
    int Nupstream;
    size_t *upstream;
    
    double *uh;
}rout_con_struct;

typedef struct{    
    double *discharge;
    double *nat_discharge;
}rout_var_struct;

void get_basins(basin_struct *basins);

void initialize_rout_local_structures(void);
void rout_init(void);

void efr_init();

void routing_update_step_vars(rout_var_struct *);
void routing_put_data(rout_var_struct rout_var, double **);

void rout(double **discharge, double *uh, double quantity, int uh_length);
void get_downstream(size_t id, int direction, size_t *downstream);
void set_upstream(size_t id, size_t Ncells);
void set_uh(size_t id, double distance, double diffusion, double velocity);
double uh(double time, double distance, double velocity, double diffusion);

#endif