#ifndef DAMS_H
#define DAMS_H

#define DAM_HIST_YEARS 3
#define DAM_PREF_VOL_FRAC 0.85

typedef struct{
    size_t nd_active;
}dam_con_map_struct;

typedef struct{
    int year;
    double max_volume;
    double max_area;
    double max_height;
}dam_con_struct;

typedef struct{    
    double volume;
    double area;
    double height;
    
    double *history_flow;
    double *calc_volume;
    
    double total_flow;
    size_t total_steps;
    
    size_t months_running;
}dam_var_struct;

bool dam_get_global_parameters(char *cmdstr);
void dam_validate_global_parameters(void);
void dam_start(void);
void dam_alloc(void);
void initialize_dam_local_structures(void);
void dam_init(void);
void dam_generate_default_state(void);
void dam_run(size_t cur_cell);
double dam_area(double volume, double max_volume, double max_area, double max_height);
double dam_height(double area, double max_height);
void dam_finalize(void);

#endif /* DAMS_H */

