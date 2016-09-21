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

//used in uh calculation FIXME: make these adjustable
#define OVERLAND_FLOW_VELOCITY 1.5
#define OVERLAND_FLOW_DIFFUSION 800

//vegetation class that can be irrigated, currently only 1 class
//can be irrigated which is the "cropland" class
#define VEG_IRR_CLASS 10

//the maximum number of reservoirs inlcuded in the simulation
#define MAX_NR_RESERVOIRS 50

//defining the different reservoir functions
#define RES_IRR_FUNCTION 1
#define RES_HYD_FUNCTION 2
#define RES_CON_FUNCTION 3

//defining the maximum of cells that can be serviced by
//and the length of the river of
//one reservoir, used for allocation
#define CELL_MAX_SERVICE 5
#define RES_MAX_SERVICE 100
#define RES_MAX_RIVER 100

//maximum distance between the serviced cell and the river
#define IRR_RIVER_DISTANCE 5

//percentage of mean monthly inflow that is considered
//minimum environmental flow
#define PERC_ENV_INFLOW 0.1

#define RES_PREF_STORAGE 0.85
#define RES_CALC_YEARS_MEAN 1

#include <vic_driver_shared_image.h>

typedef struct reservoir_units reservoir_unit;
typedef struct rout_cells rout_cell;
typedef struct rout_structs rout_struct;

struct rout_cells {
    //id for the global_domain
    size_t global_domain_id;
    //id used by VIC
    size_t id;
    //gridnumber on longitude axis
    size_t x;
    //gridnumber on latitude axis
    size_t y;
    
    //pointer to struct holding location information
    location_struct *location;
    
    bool irrigate;
    size_t irr_veg_nr;
    
    //number of upstream cells
    size_t nr_upstream;
    //1d array with pointers to upstream cells
    rout_cell **upstream; 
    //pointer to downstream cell
    rout_cell *downstream;    
    //rank in sorting of cell
    size_t rank;
    
    //pointer to struct holding reservoir information
    reservoir_unit *reservoir;
    //number of servicing reservoirs
    size_t nr_servicing_reservoirs;
    //1d array of pointers to reservoirs servicing this cell
    reservoir_unit **servicing_reservoirs;
    
    //1d array with uh-values
    double *uh;    
    //1d array with outflow values
    double *outflow;
};

struct reservoir_units{
    //bool to determine if the reservoir is run
    bool run;
    //id used by routing
    size_t id;
    //pointer to struct holding cell information
    rout_cell *cell;
        
    //length of the reservoir river
    size_t river_length;
    //1d array with cell pointer of river cells
    rout_cell **river;
    
    //number of serviced cells
    size_t nr_serviced_cells;
    //1d array with cell pointers of serviced cells
    rout_cell **serviced_cells;
    //1d array with demand for each sericed cell
    double *demand;
    
    //FIXME: see if these values are usefull
    int activation_year;
    char name[MAXSTRING];
    size_t function;
    double area;
    double height;
    double installation_capacity;
    double annual_energy;
    double storage_capacity;
    double current_storage;
    
    dmy_struct start_operation;
    double storage_start_operation;
    
    double current_mean_monthly_inflow;
    double** mean_monthly_inflow;
    double current_mean_annual_inflow;
    double* mean_annual_inflow;
    
    double current_mean_monthly_demand;
    double** mean_monthly_demand;
    double current_mean_annual_demand;
    double* mean_annual_demand;
    
};

struct rout_structs {
    //the minimum lon and lat for the global domain
    double min_lon;
    double min_lat;
    
    //char array storing the location of input files
    char param_filename[MAXSTRING];
    //char array storing the location of debug files
    char debug_path[MAXSTRING - 30];
    //char array storing the location of reservoir input files
    char reservoir_filename[MAXSTRING];  
    
    //1d array with active cells
    rout_cell *cells;
    //1d array with cell pointers sorted upstream to downstream
    rout_cell **sorted_cells;
    //2d array with cell pointers in a grid
    rout_cell ***gridded_cells;
    
    size_t nr_irrigated_cells;
    rout_cell **irrigated_cells;
    
    //number of reservoirs
    size_t nr_reservoirs;
    //1d array with active reservoirs
    reservoir_unit *reservoirs;
    //1d array with reservoir pointers sorted downstream to upstream
    reservoir_unit **sorted_reservoirs;
};

//get data from parameter file
void rout_start(void);
//allocate memory
void rout_alloc(void);
//initialize values
void rout_init(void);
//run routing scheme per timestep
void rout_run(dmy_struct* current_dmy);
//write away data
void rout_write(void);
//cleaning up
void rout_finalize(void); 

//get location of input files from global paramater file
void get_global_param_rout(FILE *gp);
//set the x/y, lat/lon and (vic)id of a cell and put them in a grid
void set_cells(void);
//find and save upstream cells (needs direction map)
void set_upstream_downstream(char file_path[], char variable_name[]);
//make unit-hydrograph (needs flow distance map)
void set_uh(char file_path[], char variable_name[]);
//instantiate the reservoirs
void set_reservoirs(void);
//determine the river comming from the reservoir
void set_reservoir_river(void);
//determine which cells are serviced by which reservoir
void set_reservoir_service(void);
//sort cells upstream to downstream
void sort_cells(void);

//function for calculating the distance between cells
double distance(rout_cell* from, rout_cell* to);

//debug file for id's and the grid
void make_location_file(char file_path[], char file_name[]);
//debug file for number of upstream cells
void make_nr_upstream_file(char file_path[], char file_name[]);
//debug file for the sorted rank of cells
void make_ranked_cells_file(char file_path[], char file_name[]);
//debug file for the unit-hydrograph of cells
void make_uh_file(char file_path[], char file_name[]);
//debug file for reservoirs of cells
void make_reservoir_file(char file_path[], char file_name[]);
//debug file for reservoir service of cells
void make_reservoir_river_file(char file_path[], char file_name[]);
//debug file for reservoir service of cells
void make_reservoir_service_file(char file_path[], char file_name[]);
//debug file for nr of reservoir service of cells
void make_nr_reservoir_service_file(char file_path[], char file_name[]);
//debug file in netCDF with all data
void make_debug_file(char file_path[], char file_name[]);
//debug file for the out_data struct
void make_out_discharge_file(char file_path[], char file_name[]);

void calculate_reservoir_values(reservoir_unit* current_reservoir, dmy_struct* current_dmy);
void reset_reservoir_run(reservoir_unit* current_reservoir, dmy_struct* current_dmy);
void reset_reservoir_demand(reservoir_unit* current_reservoir);

void shift_outflow_array(rout_cell* current_cell);

double get_moisture_content(rout_cell* current_cell);
double get_irrigation_demand(rout_cell* current_cell, double moisture_content);
double do_runoff_irrigation(double* irrigation_demand, double available_runoff_water);
double do_river_irrigation(double* irrigation_demand, double available_river_water);
void distribute_demand_among_reservoirs(rout_cell* current_cell, double irrigation_demand);

double get_reservoir_demand(reservoir_unit * current_reservoir);
double do_reservoir_operation(reservoir_unit* current_reservoir);
double do_reservoir_irrigation(double target_release, double total_demand, double demand);
double do_overflow(reservoir_unit* current_reservoir,double total_added_reservoir_water);

int is_leap_year(int year);
int nr_days_per_month(int month, int year);

#endif /* ROUT_H */

