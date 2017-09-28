#ifndef ROUTING_H
#define ROUTING_H

#include <ext_driver_shared_image.h>

enum{
    CONSTANT_UH_PARAMETERS,
    FILE_UH_PARAMETERS,
    FILE_UH_TOTAL
};

typedef struct{
    size_t downstream_global;
    size_t downstream_local;
    
    short unsigned int Nupstream_global;
    short unsigned int Nupstream_local;
    size_t *upstream_global;
    size_t *upstream_local;
    
    double *uh;
}rout_con_struct;

typedef struct{    
    double *discharge;    
}rout_var_struct;

void get_routing_type(char *cmdstr);

void routing_init();
void routing_init_downstream();
void routing_init_uh();
void routing_init_order();
void get_downstream(size_t id, int direction, size_t *downstream);
void set_upstream(size_t id, size_t Ncells);
void set_uh(size_t id, double distance, double diffusion, double velocity);
double uh(double time, double distance, double velocity, double diffusion);

void routing_run();
void rout(double **discharge, double *uh, double quantity, int uh_length);

#endif

