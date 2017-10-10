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

#define DAM_ASTEP 0.05
#define DAM_PVOLUME 0.85
#define DAM_EFR_MINF 0.4
#define DAM_EFR_MAXF 0.8
#define DAM_EFR_MINR 0.6
#define DAM_EFR_MAXR 0.3

typedef struct{
    int year;
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
    double annual_nat_inflow;
    double step_nat_inflow;    
    double discharge_amplitude;
    double discharge_offset;
    double discharge;
    
    double inflow_total;
    double nat_inflow_total;
    double *inflow_history; 
    double *nat_inflow_history;
    size_t inflow_offset;
    
    dmy_struct op_year;
}dam_var_struct;

void get_dam_type(char *);

void initialize_dam_con_map(dam_con_map_struct *);
void initialize_dam_con(dam_con_struct *);
void initialize_dam_var(dam_var_struct *dam_var);

void dams_init();

void calculate_dam_surface_area(dam_con_struct dam_con, dam_var_struct *);
void calculate_dam_height(dam_var_struct *);
void calculate_annual_inflow(dam_var_struct *);
void calculate_annual_nat_inflow(dam_var_struct *);
void calculate_step_inflow(dam_var_struct *);
void calculate_step_nat_inflow(dam_var_struct *);
void calculate_discharge_amplitude(dam_var_struct *, dam_con_struct dam_con);
void calculate_discharge_offset(dam_var_struct *, dam_con_struct dam_con);
double calculate_volume_needed(dam_var_struct dam_var, double *inflow, double *efr);
double calculate_efr(double natural_inflow, double natural_annual_inflow);
void dams_update_step_vars(dam_var_struct *, dam_con_struct dam_con);

#endif /* DAM_H */

