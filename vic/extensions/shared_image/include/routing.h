#ifndef ROUTING_H
#define ROUTING_H

#include <ext_driver_shared_image.h>

enum{
    CONSTANT_UH_PARAMETERS,
    FILE_UH_PARAMETERS,
    FILE_UH_TOTAL
};

typedef struct{
    short unsigned int direction;
    short unsigned int Nupstream;
    size_t downstream;
    size_t *upstream;
    
    size_t rank;
    
    double *uh;
}rout_con_struct;

typedef struct{    
    double *discharge;    
}rout_var_struct;

void get_routing_type(char *cmdstr);

void routing_init();
void routing_init_direction(size_t Ncells);
void routing_init_uh(size_t Ncells);
void routing_init_order(size_t Ncells);

size_t get_downstream_id(size_t id, int direction);
size_t get_total_id(size_t active_id);
size_t get_active_id(size_t global_id);
void set_upstream(size_t id, size_t Ncells);
void set_uh(size_t id, double distance, double diffusion, double velocity);
double uh(double time, double distance, double velocity, double diffusion);

void routing_run();

#endif

