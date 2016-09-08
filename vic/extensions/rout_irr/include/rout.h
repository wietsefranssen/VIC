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

//currently the resolution of vic is somehow not stored in
//global_params, therefore a constant
#define VIC_RESOLUTION 0.5;

//accuracy of the final unit hydrograph is determined by the
//number of interpolation steps, increases simulation time 
//of unit hydrographs linearly
#define UH_STEPS_PER_TIMESTEP 20

//maximum number of days the unit hydrograph is allowed to discharge
#define UH_MAX_DAYS 2

//vegetation class that can be irrigated, currently only 1 class
//can be irrigated which is the "cropland" class
#define VEG_IRR_CLASS 11

#include <vic_driver_shared_image.h>

typedef struct rout_cells {
    //id for the global_domain
    size_t global_domain_id;
    //id used by VIC
    size_t id;
    //gridnumber on longitude axis
    size_t x;
    //gridnumber on latitude axis
    size_t y;
    //rank in sorting of cell
    size_t rank;
    //pointer to struct holding location information
    location_struct *location;
    //pointer to struct holding reservoir information
    //reservoir_unit * reservoir;
    //1d array of pointers to reservoirs servicing this cell
    //reservoir_unit ** servicing_reservoir;
    //number of upstream cells
    size_t nr_upstream;
    //1d array with pointers to upstream cells
    struct rout_cells **upstream; 
    //1d array with uh-values
    double *uh;    
    //1d array with outflow values
    double *outflow;
}rout_cell;

typedef struct reservoir_units{
    //pointer to struct holding cell information
    rout_cell *cell;
    //volume of the reservoir
    double storage_capacity;
    //current volume of the reservoir;
    double current_storage_capacity;
    //number of serviced cells
    size_t nr_serviced_cells;
    //1d array with cell pointers of serviced cells
    rout_cell **serviced_cells;
    //year of dam activation
    size_t activation_year;        
}reservoir_unit;

typedef struct rout_structs {
    //1d array with active cells
    rout_cell *cells;
    //number of reservoirs
    size_t nr_reservoirs;
    //1d array with active reservoirs
    reservoir_unit *reservoirs;
    //1d array with cell pointers sorted upstream to downstream
    rout_cell **sorted_cells;
    //2d array with cell pointers in a grid
    rout_cell ***gridded_cells;
    //char array storing the location of input files
    char param_filename[MAXSTRING];
    //char array storing the location of debug files
    char debug_path[MAXSTRING - 30];
    //char array storing the location of reservoir input files
    char reservoir_filename[MAXSTRING];
}rout_struct;

//get data from parameter file
void rout_start(void);
//allocate memory
void rout_alloc(void);
//initialize values
void rout_init(void);
//run routing scheme per timestep
void rout_run(void);
//write away data
void rout_write(void);
//cleaning up
void rout_finalize(void); 

//get location of input files from global paramater file
void get_global_param_rout(FILE *gp);
//set the x/y, lat/lon and (vic)id of a cell and put them in a grid
void set_cell_location(void);
//find and save upstream cells (needs direction map)
void set_upstream(char file_path[], char variable_name[]);
//make unit-hydrograph (needs flow distance map)
void set_uh(char file_path[], char variable_name[]);
//sort cells upstream to downstream
void sort_cells(void);
//set values for start of simulation
void set_values(void);

//debug file for id's and the grid
void make_location_file(char file_path[], char file_name[]);
//debug file for number of upstream cells
void make_nr_upstream_file(char file_path[], char file_name[]);
//debug file for the sorted rank of cells
void make_ranked_cells_file(char file_path[], char file_name[]);
//debug file for the unit-hydrograph of cells
void make_uh_file(char file_path[], char file_name[]);
//debug file in netCDF with all data
void make_debug_file(char file_path[], char file_name[]);
//debug file for the out_data struct
void make_out_discharge_file(char file_path[], char file_name[]);
#endif /* ROUT_H */

