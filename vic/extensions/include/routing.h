#ifndef ROUTING_H
#define ROUTING_H

#include <stdbool.h>

#define MAX_UPSTREAM 8

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
    size_t Nupstream;
    size_t *upstream;    
    double *river_irf;
    double *grid_irf;
}rout_con_struct;

typedef struct{    
    double *discharge;
    double *nat_discharge;
}rout_var_struct;

void get_basins(basin_struct *basins);
bool rout_get_global_parameters(char *cmdstr);
void rout_validate_global_parameters(void);
void rout_start(void);
void rout_alloc(void);
void initialize_rout_local_structures(void);
void rout_init(void);
void rout_set_output_meta_data_info(void);
void rout_set_state_meta_data_info(void);
void rout_run(size_t cur_cell);
void rout_put_data(void);
void rout_finalize(void);

size_t get_downstream_global(size_t id, int direction);
size_t get_downstream_local(size_t id, int direction);
void rout(double quantity, double *uh, double *discharge, size_t length);

#endif