/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dam.h
 * Author: bram
 *
 * Created on October 2, 2017, 11:17 PM
 */

#ifndef DAM_H
#define DAM_H

#define DAM_VSTEP 0.05
#define DAM_PVOLUME 0.85

typedef struct{
    double year;
    double max_volume;
    double max_area;
    double max_height;
    double lat;
    double lon;
}dam_con_struct;

typedef struct{
    size_t Ndams;
}dam_con_map_struct;

typedef struct{
    bool run;
    unsigned int years_running;
    
    double volume;
    double area;
    double height;
    
    double annual_inflow;
    double step_inflow;
    
    double outflow_variability;
    double outflow_offset;    
    
    double discharge;
    
    double inflow_total;
    double *inflow_history;    
    size_t inflow_history_offset;
}dam_var_struct;

void get_dam_type(char *);
void initialize_dam_con_map(dam_con_map_struct *);

void dams_init();

void calculate_dam_surface_area(dam_con_struct dam_con, dam_var_struct *);
void calculate_dam_height(dam_var_struct *);
void calculate_annual_inflow(dam_var_struct *);
void calculate_step_inflow(dam_var_struct *);
void calculate_outflow_variability(dam_var_struct *, dam_con_struct dam_con);
double calculate_volume_needed(dam_var_struct dam_var, double *step_inflow);
void dams_update_step_vars(dam_var_struct *, dam_con_struct dam_con);
void dam_run(dam_con_struct dam_con, dam_var_struct *dam_var, rout_var_struct *rout_var, dmy_struct dmy);

#endif /* DAM_H */

