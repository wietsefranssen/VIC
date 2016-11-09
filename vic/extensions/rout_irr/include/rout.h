#ifndef ROUT_H
#define ROUT_H

#define MAX_CROP 10                          /**< scalar - maximum number of crops per cell */
#define CROP_DATE_DEFAULT 1                 /**< scalar - default value for the crop growing julian date */
#define RES_NO_DATA -99                     /**< scalar - nodata value for the reservoir file */

#define VIC_RESOLUTION 0.5                  /**< scalar - VIC resolution (currently not saved) */

#define MAX_DAYS_UH 2                       /**< scalar - default maximum number of days an UH is allowed to discharge */
#define FLOW_VELOCITY_UH 1.5                /**< scalar - default flow velocity for UH calculation */
#define FLOW_DIFFUSIVITY_UH 800.0           /**< scalar - default flow diffusivity for UH calculation */
#define MAX_DISTANCE_IRR 5.0                /**< scalar - default maximum distance from the reservoir cells can recieve service */

#define UH_STEPS_PER_TIMESTEP 50            /**< scalar - number of steps taken per timestep for precise UH calculation */

#define MAX_NR_RESERVOIRS 100                /**< scalar - maximum number of reservoirs included in simulation */
#define CELL_MAX_SERVICE 10                  /**< scalar - maximum reservoirs that can service a cell */
#define RES_MAX_SERVICE (MAX_DISTANCE_IRR * MAX_DISTANCE_IRR)                 /**< scalar - maximum cells that can get service from a reservoir */

#define RES_IRR_FUNCTION 1                  /**< scalar - value for the irrigation function of a reservoir */
#define RES_HYD_FUNCTION 2                  /**< scalar - value for the hydropower function of a reservoir */
#define RES_CON_FUNCTION 3                  /**< scalar - value for the flow control function of a reservoir */

#define PERC_ENV_INFLOW 0.1                 /**< scalar - percentage of mean inflow that will be environmental flow (Biemans et al., 2012) */
#define RES_PREF_STORAGE 0.85               /**< scalar - percentage of prefered storage volume (Hanasaki et al., 2008) */
#define RES_CALC_YEARS_MEAN 20               /**< scalar - number of years that reservoirs will use to calculate mean inflow and demand values */

#include <vic_driver_shared_image.h>

typedef struct reservoir_units reservoir_unit;
typedef struct rout_cells rout_cell;
typedef struct rout_structs rout_struct;
typedef struct rout_sa_structs rout_sa_struct;

struct rout_cells {
    size_t global_domain_id;            /**< scalar - VIC id for this cell in the global domain (including inactive cells) */
    size_t id;                          /**< scalar - VIC id for this cell in the local domain (excluding inactive cells) */
    size_t x;                           /**< scalar - x position of this cell in rout.gridded_cells */
    size_t y;                           /**< scalar - y position of this cell in rout.gridded_cells */
    
    size_t nr_upstream;                 /**< scalar - number of upstream cells */
    rout_cell **upstream;               /**< 1d array [nr_upstream] - pointers to upstream routing cells */
    rout_cell *downstream;              /**< pointer - pointer to downstream routing cell */
    
    bool irrigate;                      /**< bool - TRUE = irrigate FALSE = do not irrigate */
    size_t nr_crops;                    /**< scalar - number of crops in this cell */
    
    reservoir_unit *reservoir;          /**< pointer - pointer to reservoir unit located in this cell */
    
    size_t nr_servicing_reservoirs;     /**< scalar - number of reservoirs servicing this cell */
    reservoir_unit **servicing_reservoirs; /**< 1d array [nr_servicing_reservoirs] - pointer to reservoirs servicing this cell */
    
    double *uh;                         /**< 1d array [MAX_DAYS_UH * model_steps_per_day] - value of UH at each timestep */
    double *outflow;                    /**< 1d array [MAX_DAYS_UH * model_steps_per_day] - value of the outflow at each timestep [m3/s] */
    double *outflow_natural;            /**< 1d array [MAX_DAYS_UH * model_steps_per_day] - value of the outflow at each timestep [m3/s] */
};

struct reservoir_units{
    bool run;                           /**< bool - TRUE = run FALSE = do not run (false when reservoir unit is not build yet) */
    size_t id;                          /**< scalar - rout id of this reservoir */
    size_t global_id;                   /**< scalar - global id of this reservoir (from input file) */
    
    rout_cell *cell;                    /**< pointer - pointer to cell reservoir is located in */
    
    size_t nr_serviced_cells;           /**< scalar - number of serviced cells */
    rout_cell **serviced_cells;         /**< 1d array [nr_serviced_cells] - pointer to serviced cells */
    double **cell_demand;               /**< 1d array [nr_serviced_cells] - demand of serviced cells [mm] */
    
    int activation_year;                /**< scalar - activation year of the reservoir */
    char name[MAXSTRING];               /**< string - name of the reservoir */
    size_t function;                    /**< scalar - function of the reservoir (RES_IRR_FUNCTION = irrigation RES_HYD_FUNCTION = hydropower RES_CON_FUNCTION = flow control) */
    double storage_capacity;            /**< scalar - storage capacity of reservoir [m3] */
    double current_storage;             /**< scalar - current storage of reservoir [m3] */
    
    dmy_struct start_operation;         /**< dmy_struct - start of the operation year of this reservoir */
    double storage_start_operation;     /**< scalar - storage at the start of the operation year of this reservoir [m3] */
    
    double current_inflow;              /**< scalar - current monthly averaged daily inflow [m3] */
    double **inflow;                    /**< 2d array [RES_CALC_YEARS_MEAN][MONTHS_PER_YEAR] - total monthly averaged daily inflow per year/month [m3] */
    double current_inflow_natural;      /**< scalar - current monthly averaged daily inflow [m3] */
    double **inflow_natural;            /**< 2d array [RES_CALC_YEARS_MEAN][MONTHS_PER_YEAR] - total monthly averaged daily inflow per year/month [m3] */
    double current_demand;              /**< scalar - current month demand [m3] */
    double **demand;                    /**< 2d array [RES_CALC_YEARS_MEAN][MONTHS_PER_YEAR] - total monthly averaged daily demand per year/month [m3] */
    
    double monthly_inflow;              /**< scalar - monthly averaged daily inflow [m3] for this month */
    double monthly_inflow_natural;      /**< scalar - monthly averaged daily inflow [m3] for this month */
    double monthly_demand;              /**< scalar - monthly averaged daily demand [m3] for this month */
    double annual_inflow;               /**< scalar - yearly averaged daily demand [m3] for this year */
    double annual_inflow_natural;       /**< scalar - yearly averaged daily demand [m3] for this year */
    double annual_demand;               /**< scalar - yearly averaged daily demand [m3] for this year */
    
    double **prev_deficit;        /**< 2d array [nr_servicing_cells] [nr_irrigated_vegetation] - yearly averaged daily demand [m3] for this year */
    double target_release;              /**< scalar - this month target release */
    
};

struct rout_structs {
    bool firrigation;                        /**< bool - TRUE = do irrigation FALSE = do not do irrigation */
    bool freservoirs;                        /**< bool - TRUE = use reservoirs during routing FALSE = do not use reservoirs */
    bool fdebug_mode;                        /**< bool - TRUE = use debugging during routing FALSE = do not use debugging */
    bool fpot_irrigation;                    /**< bool - TRUE = use potential irrigation FALSE = do not use potential irrigation */
    
    bool fuh_file;
    size_t max_days_uh;                     /**< scalar - maximum number of days a unit hydrograph is allowed to discharge */
    double flow_velocity_uh;                /**< scalar - flow velocity of the unit hydrograph */
    double flow_diffusivity_uh;             /**< scalar - flow diffusivity of the unit hydrograph */
    double max_distance_irr;                /**< scalar - maximum cell distance a cell can be irrigated from a reservoir */
    
    int crop_start;                         /**< scalar - day of year when crop growth starts */
    int crop_developed;                     /**< scalar - day of year when crop growth reaches its peak */
    int crop_late;                          /**< scalar - day of year when crop growth starts declining */
    int crop_end;                           /**< scalar - day of year when crop growth ends */
    
    size_t nr_crop_classes;                 /**< scalar - number of vegetation classes that are irrigated crops */
    size_t *crop_class;                     /**< 1d array [nr_crop_classes] - vegetation classes which are irrigated crops */    
    
    bool fnaturalized_flow;                  /**< bool - TRUE = do both normal and naturalized routing FALSE = do not do double routing */
    
    char param_filename[MAXSTRING];         /**< string - file path and name of routing paramaters */
    char debug_path[MAXSTRING - 30];        /**< string - file path of debug files */
    char reservoir_filename[MAXSTRING];     /**< string - file path and name of reservoir information */
    
    double min_lon;                         /**< scalar - minimum longitude in domain [degree] */
    double min_lat;                         /**< scalar - minimum latitude in domain [degree] */
    
    rout_cell *cells;                       /**< 1d array [nr_active_cells] - active routing cells */
    rout_cell **sorted_cells;               /**< 1d array [nr_active_cells] - pointers to sorted routing cells */
    rout_cell ***gridded_cells;             /**< 2d array [n_nx][n_ny] - pointer to gridded routing cells */
    
    size_t nr_reservoirs;                   /**< scalar - number of reservoir units */
    reservoir_unit *reservoirs;             /**< 1d array [nr_reservoirs] - reservoir units */
};

struct rout_sa_structs{
    size_t n_active;
    size_t n_total;
    size_t nx;
    size_t ny;
    
};

void rout_start(void);
void rout_alloc(void);
void rout_init(void);
void rout_run(dmy_struct* current_dmy);
void rout_write(void);
void rout_finalize(void); 

void initialize_routing_options(void);
void get_global_param_rout(FILE *gp,size_t temp_crop_class[]);
void check_routing_options(size_t temp_crop_class[]);
void display_routing_options(void);

void set_cells(void);
void set_cell_irrigate(void);
void set_upstream_downstream(char file_path[], char variable_name[]);
void sort_cells(void);
void set_uh(char file_path[], char variable_name[]);
void set_reservoirs(void);
void set_naturalized_routing(void);
void set_reservoir_service(void);
double distance(size_t from_x, size_t from_y, size_t to_x, size_t to_y);

void make_location_file(char file_path[], char file_name[]);
void make_global_location_file(char file_path[], char file_name[]);
void make_nr_upstream_file(char file_path[], char file_name[]);
void make_ranked_cells_file(char file_path[], char file_name[]);
void make_uh_file(char file_path[], char file_name[]);
void make_reservoir_file(char file_path[], char file_name[]);
void make_reservoir_service_file(char file_path[], char file_name[]);
void make_nr_reservoir_service_file(char file_path[], char file_name[]);
void make_nr_crops_file(char file_path[], char file_name[]);

void calculate_reservoir_values(reservoir_unit* current_reservoir, dmy_struct* current_dmy);
void shift_outflow_array(rout_cell* current_cell);
double get_moisture_content(rout_cell current_cell,size_t iveg);
void distribute_demand_among_reservoirs(rout_cell* current_cell, double irrigation_demand, size_t iVeg);
void change_crop_fraction(rout_cell* cur_cell, dmy_struct* current_dmy);
void reset_reservoirs(dmy_struct* current_dmy);
void add_moisture_content(rout_cell* cur_cell, size_t vidx, double new_content);
void do_routing(rout_cell* cur_cell, double inflow, double runoff);
void get_all_mean_reservoir_values(reservoir_unit* current_reservoir, dmy_struct* current_dmy, 
        double mean_monthly_inflow[MONTHS_PER_YEAR], double mean_monthly_demand[MONTHS_PER_YEAR], double mean_monthly_inflow_natural[MONTHS_PER_YEAR],
        double* mean_annual_inflow, double* mean_annual_demand, double* mean_annual_inflow_natural);
double do_reservoir_operation(reservoir_unit current_reservoir);

int is_leap_year(int year);
int nr_days_per_month(int month, int year);

void make_sensitivity_files(void);
void set_reservoir_deficit(reservoir_unit* cur_res, rout_cell* cur_cell, double moisture_content,size_t iCell, size_t iCrop, size_t vidx);
void set_reservoir_demand(reservoir_unit* cur_res, rout_cell* cur_cell,size_t iCell, size_t iCrop);

#endif /* ROUT_H */

