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

#define VIC_RESOLUTION 0.5;

#define UH_STEPS_PER_TIMESTEP 20
#define UH_MAX_DAYS 2

#include <vic_driver_shared_image.h>

typedef struct rout_cells {
    //id (also for VIC)
    size_t id;
    //gridnumber on longitude axis
    size_t x;
    //gridnumber on latitude axis
    size_t y;
    //rank in sorting of cell
    size_t rank;
    //pointer to struct holding location information
    location_struct *location;
    //number of upstream cells
    int nr_upstream;
    //1d array with uh-values for every timestep
    double *uh;                 
    //1d array with outflow values for every uh-timestep
    double *outflow;
    //1d array with cell pointers to upstream cells
    struct rout_cells **upstream;    
}rout_cell;

typedef struct rout_structs {
    //1d array with active cells
    rout_cell *cells;
    //1d array with cell pointers sorted upstream to downstream
    rout_cell **sorted_cells;
    //2d array with cell pointers in a grid
    rout_cell ***gridded_cells;
}rout_struct;

void rout_start(void);
void rout_alloc(void);
void rout_init(void);
void rout_run(size_t time_step);
void rout_write(void);    
void rout_finalize(void); 

void set_cell_location(void);
void sort_cells(void);
void set_upstream(char file_path[], char variable_name[]);
void set_uh(char file_path[], char variable_name[]);

void make_location_file(char file_path[]);
void make_nr_upstream_file(char file_path[]);
void make_ranked_cells_file(char file_path[]);
void make_uh_file(char file_path[]);
void make_debug_file(char file_path[]);

#endif /* ROUT_H */

