#ifndef ROUT_H
#define ROUT_H

#define M3_PER_HM3 (100*100*100)            /**< scalar - m3 per hm3 */
#define UH_STEPS_PER_TIMESTEP 50            /**< scalar - number of steps taken per timestep for precise UH calculation */

#define VIC_RESOLUTION 0.5                  /**< scalar - VIC resolution (FIXME: currently not saved in VIC) */
#define MAX_DAYS_UH 2                       /**< scalar - default maximum number of days an UH is allowed to discharge */
#define FLOW_VELOCITY_UH 1.5                /**< scalar - default flow velocity for UH calculation */
#define FLOW_DIFFUSIVITY_UH 800.0           /**< scalar - default flow diffusivity for UH calculation */
#define DAM_IRR_DISTANCE 5.0                /**< scalar - default maximum distance from the reservoir cells can recieve service */

#define CROP_DATE_DEFAULT 1                 /**< scalar - default value for the crop growing julian date */
#define DAM_NO_DATA -99                     /**< scalar - nodata value for the reservoir file */

#define DAM_IRR_FUNCTION 1                  /**< scalar - value for the irrigation purpose of a dam */
#define DAM_HYD_FUNCTION 2                  /**< scalar - value for the hydropower purpose of a dam */
#define DAM_CON_FUNCTION 3                  /**< scalar - value for the flood control purpose of a dam */

#define DAM_ENV_FLOW_PERC 0.1               /**< scalar - percentage of mean monthly inflow that is environmental flow (Biemans et al., 2012) */
#define DAM_VARIABLE_INFL 0.5               /**< scalar - flow variability factor for external influences (demand or monthly inflow) */
#define DAM_INFL_CHANGE 0.1                 /**< scalar - max change of flow variability factor */
#define RES_PREF_STORAGE 0.85               /**< scalar - percentage of prefered storage volume (Hanasaki et al., 2008) */
#define DAM_HIST_YEARS 20                   /**< scalar - number of years that dams will use to calculate mean inflow and demand values */

#include <vic_driver_shared_image.h>

typedef struct rout_cells rout_cell;
typedef struct irr_cells irr_cell;
typedef struct dam_units dam_unit;
typedef struct serviced_cells serviced_cell;
typedef struct RID_cells RID_cell;
typedef struct RID_structs RID_struct;
typedef struct RID_params RID_param;

struct RID_cells{
    size_t global_domain_id;            /**< scalar - VIC id for this cell in the global domain (including inactive cells) */
    size_t id;                          /**< scalar - VIC id for this cell in the local domain (excluding inactive cells) */
    size_t x;                           /**< scalar - x position of this cell in rout.gridded_cells */
    size_t y;                           /**< scalar - y position of this cell in rout.gridded_cells */
    
    rout_cell *rout;                    /**< pointer - pointer to routing information for this cell */
    irr_cell *irr;                      /**< pointer - pointer to irrigation information for this cell */
    dam_unit *dam;                      /**< pointer - pointer to dam information for this cell */
};

struct rout_cells {
    RID_cell *cell;                  /**< pointer - pointer to cell infromation */
    
    size_t rank;                        /**< scalar - upstream rank of the cell in routing */
    
    size_t nr_upstream;                 /**< scalar - number of upstream cells */
    rout_cell **upstream;               /**< 1d array [nr_upstream] - pointers to upstream routing cells */
    rout_cell *downstream;              /**< pointer - pointer to downstream routing cell */
    
    double *uh;                         /**< 1d array [MAX_DAYS_UH * model_steps_per_day] - value of UH at each timestep */
    double *outflow;                    /**< 1d array [MAX_DAYS_UH * model_steps_per_day] - value of the outflow at each timestep [m3/s] */
    double *outflow_natural;            /**< 1d array [MAX_DAYS_UH * model_steps_per_day] - value of the outflow at each timestep [m3/s] */
};

struct irr_cells {
    RID_cell *cell;                  /**< pointer - pointer to cell infromation */
    
    size_t nr_crops;                    /**< scalar - number of crops */
    size_t *veg_class;                  /**< 1d array [nr_crops] - vegetation class of crops */
    size_t *veg_index;                  /**< 1d array [nr_crops] - vegetation index of crops */
    size_t *crop_index;                 /**< 1d array [nr_crops] - crop index of crops */    
    
    dam_unit *servicing_dam;            /*< pointer - pointer to servicing dam for this cell */
    size_t servicing_dam_cell_index;    /*< scalar - index of this cell in the servicing dam */
};

struct dam_units{
    RID_cell *cell;                     /**< pointer - pointer to cell infromation */
    
    bool run;                           /**< bool - TRUE = run FALSE = do not run (false when reservoir unit is not build yet) */
    
    double ext_influence_factor;        /**< scalar - flow variability factor for external influences (demand or monthly inflow) */
    size_t extreme_stor;                /**< scalar - days this year dam storage was extreme (full or empty) */
    
    double target_release;              /**< scalar - this months target release */
    double previous_release;            /**< scalar - outflow to be released next timestep */
    dmy_struct start_operation;         /**< dmy_struct - start of the operation year of this reservoir */
    double storage_start_operation;     /**< scalar - storage at the start of the operation year of this reservoir [m3] */
    double current_storage;             /**< scalar - current storage of reservoir [m3] */    
    
    size_t global_id;                   /**< scalar - global id of dam (from input file) */
    int activation_year;                /**< scalar - activation year of the reservoir */
    char name[MAXSTRING];               /**< string - name of the reservoir */
    size_t function;                    /**< scalar - function of the reservoir (DAM_IRR_FUNCTION = irrigation DAM_HYD_FUNCTION = hydropower DAM_CON_FUNCTION = flood control) */
    double capacity;                    /**< scalar - storage capacity of reservoir [m3] */    
    
    double total_inflow;               /**< scalar - current total inflow [m3] */
    double total_inflow_natural;       /**< scalar - current total natural inflow [m3] */
    double total_demand;               /**< scalar - current total demand [m3] */
    
    double *inflow;                    /**< 2d array [DAM_CALC_YEARS_MEAN * MONTHS_PER_YEAR] - averaged daily inflow for a month [m3] */
    double *inflow_natural;            /**< 2d array [DAM_CALC_YEARS_MEAN * MONTHS_PER_YEAR] - averaged daily inflow for a month [m3] */
    double *demand;                    /**< 2d array [DAM_CALC_YEARS_MEAN * MONTHS_PER_YEAR] - averaged daily demand for a month [m3] */
    
    double monthly_inflow;              /**< scalar - multi-year averaged daily inflow [m3] for current month */
    double monthly_inflow_natural;      /**< scalar - multi-year averaged daily inflow [m3] for current month */
    double monthly_demand;              /**< scalar - multi-year averaged daily demand [m3] for current month */
    
    double annual_inflow;               /**< scalar - multi-year averaged daily demand [m3] for current year */
    double annual_inflow_natural;       /**< scalar - multi-year averaged daily demand [m3] for current year */
    double annual_demand;               /**< scalar - multi-year averaged daily demand [m3] for current year */
    
    size_t nr_serviced_cells;           /**< scalar - number of serviced cells */
    serviced_cell *serviced_cells;      /**< 1d array [nr_serviced_cells] - serviced cells */
};

struct serviced_cells{
    RID_cell *cell;                    /**< pointer - pointer to cell information */
    double *moisture_content;          /**< 1d array [nr_crops] - moisture content of serviced cell's crops [mm] */ 
    double *demand_crop;               /**< 1d array [nr_crops] - demand of serviced cell's crops [m3] */ 
    double *deficit;                    /**< 1d array [nr_crops] - previous demand of serviced cell's crops [m3] */ 
    
};

struct RID_params{
    bool firrigation;                   /**< bool - TRUE = do irrigation FALSE = do not do irrigation */
    bool fpot_irrigation;               /**< bool - TRUE = use potential irrigation FALSE = do not use potential irrigation */
    bool fdams;                         /**< bool - TRUE = use reservoirs during routing FALSE = do not use reservoirs */
    bool fdebug_mode;                   /**< bool - TRUE = use debugging during routing FALSE = do not use debugging */
    
    char param_filename[MAXSTRING];     /**< string - file path and name of routing paramaters */
    char dam_filename[MAXSTRING];       /**< string - file path and name of reservoir information */
    char debug_path[MAXSTRING - 30];    /**< string - file path of debug files */
    
    //routing
    size_t max_days_uh;                 /**< scalar - maximum number of days a unit hydrograph is allowed to produce discharge */
    double flow_velocity_uh;            /**< scalar - flow velocity of the unit hydrograph */
    double flow_diffusivity_uh;         /**< scalar - flow diffusivity of the unit hydrograph */
    
    //irrigation
    size_t nr_crops;                    /**< scalar - number of vegetation classes that are irrigated crops */
    size_t *crop_class;                 /**< 1d array [nr_crop_classes] - vegetation classes which are irrigated crops */
    unsigned short int *crop_sow;       /**< 1d array [nr_crop_classes] - day of year when crop growth starts */
    unsigned short int *crop_harvest;   /**< 1d array [nr_crop_classes] - day of year when crop growth ends */
    
    //dams
    bool fnaturalized_flow;             /**< bool - TRUE = do both normal and naturalized routing FALSE = do not do double routing */
    double dam_irr_distance;            /**< scalar - maximum cell distance a cell can be irrigated from a reservoir */
};

struct RID_structs {
    RID_param param;                        /**< module parameters */
    
    double min_lon;                         /**< scalar - minimum longitude in domain [degree] */
    double min_lat;                         /**< scalar - minimum latitude in domain [degree] */
    
    RID_cell *cells;                        /**< 1d array [nr_active_cells] - module cells */  
    RID_cell **sorted_cells;                /**< 1d array [nr_active_cells] - pointers to sorted module cells */
    RID_cell ***gridded_cells;              /**< 2d array [n_nx][n_ny] - pointer to gridded module cells */
    
    rout_cell *rout_cells;                  /**< 1d array [nr_active_cells] - routed cells */    
    size_t nr_dams;                         /**< scalar - number of dam units */
    dam_unit *dams;                         /**< 1d array [nr_reservoirs] - dam units */    
    size_t nr_irr_cells;                    /**<scalar - number of irrigated cells */
    irr_cell *irr_cells;                    /**< 1d array [nr_irr_cells] - irrigated cells */
};


/*******************************
 rout_start
*******************************/
void RID_start(void);
void default_module_options(void);
void get_module_options(FILE *gp, size_t *nr_crops, size_t ***crop_info);
void check_module_options(size_t nr_crops, size_t **crop_info);
void display_module_options(void);

/*******************************
 RID_alloc
*******************************/
void RID_alloc(void);

/*******************************
 rout_init
*******************************/
void RID_init(void);

void init_module(void);
void init_routing(void);
void init_irr(void);
void init_dams(void);
void init_dam_irr(void);

//Routing module
void set_cell_location(void);
void set_cell_uh(char variable_name[]);
void set_cell_downstream(char variable_name[]);
void set_cell_upstream(void);
void set_cell_rank(void);

//Irrigation module
void set_irr(void);
void set_irr_crops(void); 

//Dam module
void set_dam_information(void);
void set_dam_natural_routing(void);

//Dam irrigation module
void set_dam_irr_service(void);


/*******************************
 rout_run
*******************************/
void RID_run(dmy_struct* current_dmy);

//Routing module
void do_routing_module(RID_cell *cur_cell);
void gather_runoff_inflow(RID_cell *cur_cell, double *runoff, double *inflow, bool naturalized);
void shift_outflow_array(RID_cell* current_cell);
void do_routing(RID_cell* cur_cell, double runoff, double inflow, bool naturalized);

//Irrigation module
void do_irrigation_module(RID_cell *cur_cell, dmy_struct *cur_dmy);
bool in_irrigation_season(size_t crop_index, size_t current_julian_day);
void get_moisture_content(size_t cell_id, size_t veg_index, double *moisture_content);
void get_irrigation_demand(size_t cell_id, size_t veg_index, double moisture_content, double *demand_crop);
void do_irrigation(RID_cell *cur_cell, double demand_crop[], double *demand_cell, double moisture_content[]);
void update_servicing_dam_values(RID_cell *cur_cell, double demand_crop[], double moisture_content[]);

//Dam module
void do_dam_flow(dam_unit *cur_dam);
void do_dam_module(dam_unit *cur_dam, dmy_struct *cur_dmy);
void update_dam_history_day(dam_unit* cur_dam, dmy_struct *cur_dmy);
void update_dam_history_month(dam_unit* cur_dam, dmy_struct* cur_dmy);
void calculate_target_release(dam_unit* cur_dam);
void update_dam_history_year(dam_unit* cur_dam, dmy_struct* cur_dmy);
void calculate_operational_year(dam_unit* cur_dam, dmy_struct *cur_dmy);
void calculate_actual_release(dam_unit* cur_dam, double *actual_release);
void do_dam_irrigation(dam_unit* cur_dam, double *actual_release, double *irrigation_release);
void calculate_defict(dam_unit* cur_dam);
void do_dam_release(dam_unit* cur_dam, double actual_release, double irrigation_release);

/*******************************
 rout_write and rout_finalize
*******************************/
void RID_write(void);
void RID_finalize(void);

/*******************************
 Debug functions
*******************************/
void make_location_file(char file_path[], char file_name[]);
void make_global_location_file(char file_path[], char file_name[]);
void make_nr_upstream_file(char file_path[], char file_name[]);
void make_ranked_cells_file(char file_path[], char file_name[]);
void make_uh_file(char file_path[], char file_name[]);
void make_dam_file(char file_path[], char file_name[]);
void make_dam_service_file(char file_path[], char file_name[]);
void make_nr_crops_file(char file_path[], char file_name[]);

/*******************************
 Extra functions
*******************************/
int is_leap_year(int year);
int nr_days_in_month(int month, int year);
double distance(size_t from_x, size_t from_y, size_t to_x, size_t to_y);

#endif /* ROUT_H */

