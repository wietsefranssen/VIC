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

#define FALSE 0
#define TRUE 1

#include <vic_def.h>

typedef struct rout_cells {
    struct rout_cells **upstream; //Arrary of pointer pointing to struct routCell pointing to upstream cells
    int nr_upstream;            //int containing the number of upstream cells
    double *uh;                 //Points to array of doubles containing Unit Hydrograph
    double *discharge;          //Points to array of doubles containing discharge
    //MAYBE DO NOT SAVE DISCHARGE EVERY TIMESTEP, BUT ONLY FOR CURRENT TIMESTEP!
    
    //double *water_demand;      //Points to array of doubles containing water demand
    //double *local_water_use     //Points to array of doubles containing local water withdrawal
    //double *reservoir_water_use //Points to array of doubles containing reservoir water withdrawal
    //int nr_reservoirs
    //struct rout_reservoir **reservoir       //Points to struct routDam containing the reservoir of this cell
    //struct rout_cell *downstream
}rout_cell;

typedef struct rout_structs {
    rout_cell ***cells;     //Points to 2d array of struct routCell containing all cells in area
    rout_cell **ranked_cells;
    char fileName[MAXSTRING];      //char containing the routing filename
    int x_size;                     //int containing x size of area
    int y_size;                     //int containing y size of area
    int total_time;                 //int containing the total timesteps
    int uh_length;                  //int containing the length of an Unit Hydrograph
}rout_struct;

void rout_start(void);      // read global parameters for routing
void rout_alloc(void);      // allocate memory
void rout_init(void);       // initialize model parameters from parameter files
void rout_run(void);        // run routing over the domain
void rout_write(void);      // write routine for routing
void rout_finalize(void);   // clean up routine for routing

#endif /* ROUT_H */

