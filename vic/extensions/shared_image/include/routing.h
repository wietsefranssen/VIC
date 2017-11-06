#ifndef ROUTING_H
#define ROUTING_H

#include <ext_driver_shared_image.h>

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
    size_t downstream;    
    short unsigned int Nupstream;
    size_t *upstream;
    
    double *uh;
}rout_con_struct;

typedef struct{    
    double *discharge;    
}rout_var_struct;

void get_routing_type(char *cmdstr);

void get_basins(nameid_struct *nc_nameid, char *direction_var, basin_struct *basins);

void routing_init();
void routing_init_downstream();
void routing_init_uh();
void routing_init_order();
void get_downstream(size_t id, int direction, size_t *downstream);
void set_upstream(size_t id, size_t Ncells);
void set_uh(size_t id, double distance, double diffusion, double velocity);
double uh(double time, double distance, double velocity, double diffusion);

void routing(double **discharge, double *uh, double quantity, int uh_length);

#endif

