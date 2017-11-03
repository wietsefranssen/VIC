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

#define DAM_ASTEP 0.01
#define DAM_MIN_PVOLUME 0.15
#define DAM_MAX_PVOLUME 0.85
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
    
    double inflow_total;
    double nat_inflow_total;
        
    size_t history_offset;
    double *inflow_history;
    double *nat_inflow_history;    
    double *calc_inflow;
    double *calc_nat_inflow;
    double calc_my_inflow;
    double calc_my_nat_inflow;
        
    double amplitude;
    double offset;
    double *calc_discharge;
    double *calc_efr;
    
    double discharge_cor;
    
    double discharge;    
    double inflow;
    double nat_inflow;
    
    dmy_struct op_year;
}dam_var_struct;

void get_dam_type(char *);

void initialize_dam_con_map(dam_con_map_struct *);
void initialize_dam_con(dam_con_struct *);
void initialize_dam_var(dam_var_struct *dam_var);

void dams_init();

void calculate_dam_surface_area(dam_con_struct dam_con, dam_var_struct *);
void calculate_dam_height(dam_var_struct *);
void calculate_multi_year_average(double *history, 
        size_t repetitions, 
        size_t length, 
        size_t offset, 
        size_t skip, 
        double *average);
void calculate_operational_year(double my_inflow, 
        double *ms_inflow,
        size_t nsteps,
        size_t days_per_step, 
        dmy_struct *op_dmy);
void calculate_optimal_discharge(double max_volume, 
        double cur_volume, 
        double my_inflow, 
        double *ms_inflow, 
        double *discharge, 
        double *amplitude, 
        double *offset);
double calculate_volume_needed(double amplitude, 
        double offset, 
        double my_inflow, 
        double *ms_inflow, 
        size_t history_length, 
        size_t model_steps_per_history);
double get_amplitude_discharge(double my_inflow, 
        double ms_inflow, 
        double amplitude, 
        double offset);
void calculate_efr(double *flow, 
        size_t nsteps, 
        double annual_flow, 
        double *efr);
double calculate_efr_fraction(double flow, 
        double annual_flow);
void calculate_corrected_discharge(double *discharge, 
        double *efr, 
        size_t nsteps, 
        double my_inflow);
void dams_update_step_vars(dam_var_struct *, dam_con_struct dam_con);

#endif /* DAM_H */

