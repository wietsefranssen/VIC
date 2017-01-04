#ifndef ROUT_H
#define ROUT_H

#define M3_PER_HM3 (100*100*100)

#define MIN_DEMAND 0.001
#define MIN_CROP_FACTOR 0.00001
#define BARE_SOIL_VEG_CLASS 11

#define MAX_CROP 10                          /**< scalar - maximum number of crops per cell */
#define CROP_DATE_DEFAULT 1                 /**< scalar - default value for the crop growing julian date */
#define DAM_NO_DATA -99                     /**< scalar - nodata value for the reservoir file */

#define VIC_RESOLUTION 0.5                  /**< scalar - VIC resolution (currently not saved) */

#define MAX_DAYS_UH 2                       /**< scalar - default maximum number of days an UH is allowed to discharge */
#define FLOW_VELOCITY_UH 1.5                /**< scalar - default flow velocity for UH calculation */
#define FLOW_DIFFUSIVITY_UH 800.0           /**< scalar - default flow diffusivity for UH calculation */
#define MAX_DISTANCE_IRR 5.0                /**< scalar - default maximum distance from the reservoir cells can recieve service */

#define UH_STEPS_PER_TIMESTEP 50            /**< scalar - number of steps taken per timestep for precise UH calculation */

#define MAX_NR_DAMS 100                /**< scalar - maximum number of reservoirs included in simulation */
#define CELL_MAX_SERVICE 10                  /**< scalar - maximum reservoirs that can service a cell */
#define DAM_MAX_SERVICE (4 * MAX_DISTANCE_IRR * MAX_DISTANCE_IRR)                 /**< scalar - maximum cells that can get service from a reservoir */

#define DAM_IRR_FUNCTION 1                  /**< scalar - value for the irrigation function of a reservoir */
#define DAM_HYD_FUNCTION 2                  /**< scalar - value for the hydropower function of a reservoir */
#define DAM_CON_FUNCTION 3                  /**< scalar - value for the flow control function of a reservoir */

#define PERC_ENV_INFLOW 0.1                 /**< scalar - percentage of mean inflow that will be environmental flow (Biemans et al., 2012) */
#define RES_PREF_STORAGE 0.85               /**< scalar - percentage of prefered storage volume (Hanasaki et al., 2008) */
#define DAM_CALC_YEARS_MEAN 20               /**< scalar - number of years that reservoirs will use to calculate mean inflow and demand values */

#include <vic_driver_shared_image.h>

typedef struct dam_units dam_unit;
typedef struct rout_cells rout_cell;
typedef struct irr_cells irr_cell;
typedef struct module_cells module_cell;
typedef struct module_structs module_struct;
typedef struct module_params module_param;

struct module_cells{
    size_t global_domain_id;            /**< scalar - VIC id for this cell in the global domain (including inactive cells) */
    size_t id;                          /**< scalar - VIC id for this cell in the local domain (excluding inactive cells) */
    size_t x;                           /**< scalar - x position of this cell in rout.gridded_cells */
    size_t y;                           /**< scalar - y position of this cell in rout.gridded_cells */
    
    rout_cell *rout;
    irr_cell *irr;
    dam_unit *dam;
};

struct rout_cells {
    module_cell *cell;                    /**< pointer - pointer to cell irrigation is located in */
    
    size_t rank;
    
    size_t nr_upstream;                 /**< scalar - number of upstream cells */
    rout_cell **upstream;               /**< 1d array [nr_upstream] - pointers to upstream routing cells */
    rout_cell *downstream;              /**< pointer - pointer to downstream routing cell */
    
    double *uh;                         /**< 1d array [MAX_DAYS_UH * model_steps_per_day] - value of UH at each timestep */
    double *outflow;                    /**< 1d array [MAX_DAYS_UH * model_steps_per_day] - value of the outflow at each timestep [m3/s] */
    double *outflow_natural;            /**< 1d array [MAX_DAYS_UH * model_steps_per_day] - value of the outflow at each timestep [m3/s] */
};

struct irr_cells {
    module_cell *cell;                    /**< pointer - pointer to cell irrigation is located in */
    
    size_t nr_crops;
    size_t *veg_class;
    size_t *veg_index;
    size_t *crop_index;
    double *max_cv;                 /**< 2d array [nr_active_cells][nr_crop_classes] - maximum vegetation fraction for irrigated crops */
    
    size_t nr_servicing_dams;     /**< scalar - number of reservoirs servicing this cell */
    dam_unit **servicing_dams; /**< 1d array [nr_servicing_reservoirs] - pointer to reservoirs servicing this cell */
    
};

struct dam_units{
    module_cell *cell;                    /**< pointer - pointer to cell dam is located in */
    size_t id;                          /**< scalar - rout id of this reservoir */
    size_t global_id;                   /**< scalar - global id of this reservoir (from input file) */
    
    bool run;                           /**< bool - TRUE = run FALSE = do not run (false when reservoir unit is not build yet) */
    
    size_t nr_serviced_cells;           /**< scalar - number of serviced cells */
    module_cell **serviced_cells;         /**< 1d array [nr_serviced_cells] - pointer to serviced cells */
    double **cell_demand;               /**< 1d array [nr_serviced_cells] - demand of serviced cells [mm] */
    double **prev_deficit;        /**< 2d array [nr_servicing_cells] [nr_irrigated_vegetation] - yearly averaged daily demand [m3] for this year */
    
    int activation_year;                /**< scalar - activation year of the reservoir */
    char name[MAXSTRING];               /**< string - name of the reservoir */
    size_t function;                    /**< scalar - function of the reservoir (RES_IRR_FUNCTION = irrigation RES_HYD_FUNCTION = hydropower RES_CON_FUNCTION = flow control) */
    double capacity;            /**< scalar - storage capacity of reservoir [m3] */
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
    
    double target_release;              /**< scalar - this month target release */
    
};

struct module_params{
    bool firrigation;                        /**< bool - TRUE = do irrigation FALSE = do not do irrigation */
    bool fdams;                        /**< bool - TRUE = use reservoirs during routing FALSE = do not use reservoirs */
    bool fdebug_mode;                        /**< bool - TRUE = use debugging during routing FALSE = do not use debugging */
    bool fpot_irrigation;                    /**< bool - TRUE = use potential irrigation FALSE = do not use potential irrigation */
    
    char param_filename[MAXSTRING];         /**< string - file path and name of routing paramaters */
    char debug_path[MAXSTRING - 30];        /**< string - file path of debug files */
    char dam_filename[MAXSTRING];     /**< string - file path and name of reservoir information */
    
    //routing
    size_t max_days_uh;                     /**< scalar - maximum number of days a unit hydrograph is allowed to discharge */
    double flow_velocity_uh;                /**< scalar - flow velocity of the unit hydrograph */
    double flow_diffusivity_uh;             /**< scalar - flow diffusivity of the unit hydrograph */
    
    //irrigation
    size_t nr_crop_classes;                 /**< scalar - number of vegetation classes that are irrigated crops */
    size_t *crop_class;                     /**< 1d array [nr_crop_classes] - vegetation classes which are irrigated crops */
    
    unsigned short int *crop_sow;                         /**< scalar - day of year when crop growth starts */
    unsigned short int *crop_developed;                     /**< scalar - day of year when crop growth reaches its peak */
    unsigned short int *crop_matured;                          /**< scalar - day of year when crop growth starts declining */
    unsigned short int *crop_harvest;                           /**< scalar - day of year when crop growth ends */
    
    //dams
    bool fnaturalized_flow;                  /**< bool - TRUE = do both normal and naturalized routing FALSE = do not do double routing */
    double max_distance_irr;                /**< scalar - maximum cell distance a cell can be irrigated from a reservoir */
};

struct module_structs {
    module_param param;                     /**< module parameters */
    
    double min_lon;                         /**< scalar - minimum longitude in domain [degree] */
    double min_lat;                         /**< scalar - minimum latitude in domain [degree] */
    
    module_cell *cells;                     /**< 1d array [nr_active_cells] - module cells */  
    module_cell **sorted_cells;             /**< 1d array [nr_active_cells] - pointers to sorted module cells */
    module_cell ***gridded_cells;           /**< 2d array [n_nx][n_ny] - pointer to gridded module cells */
    
    rout_cell *rout_cells;                  /**< 1d array [nr_active_cells] - routed cells */
    
    size_t nr_dams;                         /**< scalar - number of dam units */
    dam_unit *dams;                         /**< 1d array [nr_reservoirs] - dam units */
    
    size_t nr_irr_cells;                    /**<scalar - number of irrigated cells */
    irr_cell *irr_cells;                    /**< 1d array [nr_irr_cells] - irrigated cells */
};

void rout_start(void);
void rout_alloc(void);
void rout_init(void);
void rout_run(dmy_struct* current_dmy);
void rout_write(void);
void rout_finalize(void); 

//rout.start functions
void initialize_routing_options(void);
void get_global_param_rout(FILE *gp,size_t temp_crop_class[], unsigned short int temp_crop_season[][4]);
void check_routing_options(size_t temp_crop_class[], unsigned short int temp_crop_season[][4]);
void display_routing_options(void);

//rout.init functions
void init_routing(void);
void set_cell_location(void);
void set_cell_uh(char variable_name[]);
void set_cell_downstream(char variable_name[]);
void set_cell_upstream(void);
void set_cell_rank(void);

void init_irr(void);
void set_irr(void);
void set_irr_crops(void);    

void init_dams(void);
void set_dam_location(void);
void set_dam_natural_routing(void);

void init_dam_irr(void);
void set_dam_irr_service(void);

//rout.run functions
void gather_runoff_inflow(module_cell *cur_cell, double *runoff, double *inflow, bool naturalized);

void do_irrigation(module_cell *cur_cell, double *inflow, double *runoff, dmy_struct *current_dmy);
void change_crop_fraction(module_cell* cur_cell, dmy_struct* current_dmy);
double get_moisture_content(size_t id, size_t veg_index);
double get_irrigation_demand(size_t id, double moisture_content, size_t veg_index);
double do_source_irrigation(double available, double irrigation_demand, double irrigation_demand_total);
void distribute_demand_among_dams(module_cell* cur_cell, double irrigation_demand, size_t iCrop);
void add_moisture_content(module_cell* cur_cell, size_t veg_index, double new_content);
void check_irrigation(double *end_value, double start_value, double added_water);

void do_routing(module_cell* cur_cell, double inflow, double runoff);
void shift_outflow_array(module_cell* current_cell);
void do_uh_routing(module_cell* cur_cell, double inflow, double runoff);
void do_dam_routing(module_cell* cur_cell, double inflow, double runoff);

void do_dam_operation(dam_unit *cur_dam, dmy_struct *cur_dmy);
void reset_dam_demand(dam_unit *reservoir);
double get_total_dam_demand(dam_unit *dam);
void do_routing(module_cell* cur_cell, double inflow, double runoff);
void get_all_mean_dam_values(dam_unit* current_reservoir, dmy_struct* current_dmy, 
        double mean_monthly_inflow[MONTHS_PER_YEAR], double mean_monthly_demand[MONTHS_PER_YEAR], double mean_monthly_inflow_natural[MONTHS_PER_YEAR],
        double* mean_annual_inflow, double* mean_annual_demand, double* mean_annual_inflow_natural);
double do_reservoir_operation(dam_unit current_reservoir);
void calculate_dam_values(dam_unit* current_reservoir, dmy_struct* current_dmy);
void set_dam_deficit(dam_unit* cur_res, module_cell* cur_cell, double moisture_content,size_t iCell, size_t iCrop, size_t vidx);
void set_dam_demand(dam_unit* cur_res, module_cell* cur_cell,size_t iCell, size_t iCrop);

//other functions
void make_location_file(char file_path[], char file_name[]);
void make_global_location_file(char file_path[], char file_name[]);
void make_nr_upstream_file(char file_path[], char file_name[]);
void make_ranked_cells_file(char file_path[], char file_name[]);
void make_uh_file(char file_path[], char file_name[]);
void make_dam_file(char file_path[], char file_name[]);
void make_dam_service_file(char file_path[], char file_name[]);
void make_nr_dam_service_file(char file_path[], char file_name[]);
void make_nr_crops_file(char file_path[], char file_name[]);

int is_leap_year(int year);
int nr_days_per_month(int month, int year);
double distance(size_t from_x, size_t from_y, size_t to_x, size_t to_y);


#endif /* ROUT_H */

