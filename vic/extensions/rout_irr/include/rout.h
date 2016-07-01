/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   rout.h
 * Author: bram
 *
 * Created on June 27, 2016, 12:32 PM
 */

#ifndef ROUT_H
#define ROUT_H

#include <vic_def.h>

typedef struct rout_cells {
    size_t x;
    size_t y;
    size_t vic_id;
    
    int active;
    
    struct rout_cells **upstream; //Arrary of pointer pointing to struct routCell pointing to upstream cells
    int nr_upstream;            //int containing the number of upstream cells
    double *uh;                 //Points to array of doubles containing Unit Hydrograph
    double *outflow;          //Points to array of doubles containing outflow
    
    //double *water_demand;      //Points to array of doubles containing water demand
    //double *local_water_use     //Points to array of doubles containing local water withdrawal
    //double *reservoir_water_use //Points to array of doubles containing reservoir water withdrawal
    //int nr_reservoirs
    //struct rout_reservoir **reservoir       //Points to struct routDam containing the reservoir of this cell
    //struct rout_cell *downstream
}rout_cell;

typedef struct rout_structs {
    rout_cell **cells;     //Points to 2d array of struct routCell containing all cells in area
    rout_cell **ranked_cells;
    //char fileName[MAXSTRING];      //char containing the routing filename
    size_t x_size;                     //int containing x size of area
    size_t y_size;                     //int containing y size of area
    size_t total_time;                 //int containing the total timesteps
    size_t uh_length;                  //int containing the length of an Unit Hydrograph
}rout_struct;

void rout_start(void);      // read global parameters for routing
void rout_alloc(void);      // allocate memory
void rout_init(void);       // initialize model parameters from parameter files
void rout_run(size_t time_step);        // run routing over the domain
void rout_write(void);      // write routine for routing
void rout_finalize(void);   // clean up routine for routing
void rank_cells(void);
void calculate_nr_upstream(char file_path[], char variable_name[]);
void make_nr_upstream_file(char file_path[]);
void make_upstream_file(char file_path[]);
void set_upstream(char file_path[], char variable_name[]);

#endif /* ROUT_H */

