#ifndef ROUT_H
#define ROUT_H

#define M3_PER_HM3 (100*100*100)            /**< scalar - m3 per hm3 */
#define UH_STEPS_PER_TIMESTEP 50            /**< scalar - number of steps taken per timestep for precise UH calculation */

#define VIC_RESOLUTION 0.5                  /**< scalar - VIC resolution (FIXME: currently not saved in VIC) */

#define DEF_UH_DAYS 2                       /**< scalar - default maximum number of days an UH is allowed to discharge */
#define DEF_FLOW_VEL 1.5                    /**< scalar - default flow velocity for UH calculation */
#define DEF_FLOW_DIF 800.0                  /**< scalar - default flow diffusivity for UH calculation */
#define DEF_IRR_DIST 5.0                    /**< scalar - default maximum distance from the dam cells can recieve service */
#define DEF_CROP_DATE 1                     /**< scalar - default value for the crop growing julian date */

#define MAX_POND_STORAGE 150                /**< scalar - maximum ponding storage [mm] */
#define MIN_POND_FRAC 0.25                  /**< scalar - ponding fraction after which irrigation is needed [-] */
#define AVAILABLE_IRR_FRAC 0.3              /**< scalar - fraction of local available water that can be used [-] */
#define POND_KSAT 40                        /**< scalar - saturated hydraulic conductivity of ponds [mm/d] */

#define DAM_NO_DATA -99                     /**< scalar - nodata value for the dam file */
#define DAM_IRR_FUNCTION 1                  /**< scalar - value for the irrigation purpose of a dam */
#define DAM_HYD_FUNCTION 2                  /**< scalar - value for the hydropower purpose of a dam */
#define DAM_CON_FUNCTION 3                  /**< scalar - value for the flood control purpose of a dam */

#define DAM_ENV_FLOW_HIGH 0.4               /**< scalar - percentage of mean monthly inflow that is environmental flow (Pastor et al., 2014) */
#define DAM_ENV_FLOW_INT 0.3                /**< scalar - percentage of mean monthly inflow that is environmental flow (Pastor et al., 2014) */
#define DAM_ENV_FLOW_LOW 0.3                /**< scalar - percentage of mean monthly inflow that is environmental flow (Pastor et al., 2014) */
#define ENV_HIGH_FLOW_PERC 0.8              /**< scalar - percentage of mean annual inflow that is considered high (Pastor et al., 2014) */
#define ENV_LOW_FLOW_PERC 0.4               /**< scalar - percentage of mean monthly inflow that is considered low (Pastor et al., 2014) */

#define DAM_EXT_INF_DEFAULT 0.5             /**< scalar - flow variability factor for external influences (demand or monthly inflow) */
#define DAM_EXT_INF_CHANGE 0.1              /**< scalar - change of external influence on flow variability factor */
#define DAM_MIN_EXT_INF 0.1                 /**< scalar - minimum of external influence on flow variability factor */
#define DAM_MAX_EXT_INF 0.9                 /**< scalar - maximum of external influence on flow variability factor */

#define DAM_PREF_STORE 0.85                 /**< scalar - percentage of prefered storage volume (Hanasaki et al., 2006) */
#define DAM_HIST_YEARS 20                   /**< scalar - number of years that dams will use to calculate mean inflow and demand values */
#define DAM_MIN_PREF_STORE 0.05             /**< scalar - prefered minimum storage of a dam */
#define DAM_MAX_PREF_STORE 0.95             /**< scalar - prefered maximum storage of a dam */

#define RES_POT_FRAC 0.7                    /**< scalar - fraction of potential evapotranspiration that is evaporation in reservoirs (Houman, 1973) */

#include <vic_driver_shared_image.h>

typedef struct rout_cells rout_cell;
typedef struct irr_cells irr_cell;
typedef struct dam_units dam_unit;
//typedef struct serviced_cells serviced_cell;
typedef struct RID_cells RID_cell;
typedef struct RID_structs RID_struct;
typedef struct RID_params RID_param;

struct RID_params{
    bool firrigation;                   /**< bool - TRUE = do irrigation FALSE = do not do irrigation */
    bool fdams;                         /**< bool - TRUE = use dams during routing FALSE = do not use dmas */
    bool fdebug_mode;                   /**< bool - TRUE = use debugging during routing FALSE = do not use debugging */
    
    char param_filename[MAXSTRING];     /**< string - file path and name of routing paramaters */
    char dam_filename[MAXSTRING];       /**< string - file path and name of dam information */
    char debug_path[MAXSTRING];         /**< string - file path of debug files */
    
    //routing
    size_t max_days_uh;                 /**< scalar - maximum number of days a unit hydrograph is allowed to produce discharge */
    double flow_velocity_uh;            /**< scalar - flow velocity of the unit hydrograph */
    double flow_diffusivity_uh;         /**< scalar - flow diffusivity of the unit hydrograph */
    
    //irrigation
    size_t nr_crops;                    /**< scalar - number of vegetation classes that are irrigated crops */
    size_t *crop_class;                 /**< 1d array [nr_crop_classes] - vegetation classes which are irrigated crops */
    unsigned short int *start_irr;      /**< 1d array [nr_crop_classes] - day of year when crop growth starts */
    unsigned short int *end_irr;        /**< 1d array [nr_crop_classes] - day of year when crop growth ends */
    
    //dams
    bool fnaturalized_flow;             /**< bool - TRUE = do both normal and naturalized routing FALSE = do not do double routing */
    double dam_irr_distance;            /**< scalar - maximum cell distance a cell can be irrigated from a dam */
};

struct RID_structs {
    RID_param param;                    /**< module parameters */
    
    double min_lon;                     /**< scalar - minimum longitude in domain [degree] */
    double min_lat;                     /**< scalar - minimum latitude in domain [degree] */
    
    RID_cell *cells;                    /**< 1d array [nr_active_cells] - module cells */  
    RID_cell **sorted_cells;            /**< 1d array [nr_active_cells] - pointers to sorted module cells */
    RID_cell ***gridded_cells;          /**< 2d array [n_nx][n_ny] - pointer to gridded module cells */
    
    //routing
    rout_cell *rout_cells;              /**< 1d array [nr_active_cells] - routed cells */     
    
    //irrigation
    size_t nr_irr_cells;                /**<scalar - number of irrigated cells */
    irr_cell *irr_cells;                /**< 1d array [nr_irr_cells] - irrigated cells */ 
    
    //dams
    size_t nr_dams;                     /**< scalar - number of dam units */
    dam_unit *dams;                     /**< 1d array [nr_dams] - dam units */  
};

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
    RID_cell *cell;                     /**< pointer - pointer to cell infromation */
    
    size_t rank;                        /**< scalar - upstream rank of the cell in routing */
    
    size_t nr_upstream;                 /**< scalar - number of upstream cells */
    rout_cell **upstream;               /**< 1d array [nr_upstream] - pointers to upstream routing cells */
    rout_cell *downstream;              /**< pointer - pointer to downstream routing cell */
    
    double *uh;                         /**< 1d array [max_days_uh * model_steps_per_day] - value of UH at each timestep */
    double *outflow;                    /**< 1d array [max_days_uh * model_steps_per_day] - value of the outflow at each timestep [m3/s] */
    double *outflow_natural;            /**< 1d array [max_days_uh * model_steps_per_day] - value of the natural outflow at each timestep [m3/s] */
};

struct irr_cells {
    RID_cell *cell;                     /**< pointer - pointer to cell infromation */
    
    size_t nr_crops;                    /**< scalar - number of crops */
    size_t *veg_class;                  /**< 1d array [nr_crops] - vegetation class of crops */
    size_t *veg_index;                  /**< 1d array [nr_crops] - vegetation index of crops */
    size_t *crop_index;                 /**< 1d array [nr_crops] - crop index of crops */    
    
    double *demand;                     /**< 1d array [nr_crops] - demand of crops [m3] */    
    double *moisture;                   /**< 1d array [nr_crops] - moisture content of crops [mm] */    
    double *deficit;                    /**< 1d array [nr_crops] - previous demand of crops [m3] */    
    double *storage;                    /**< 1d array [nr_crops] - water storage of crops [mm] */ 
    double *normal_Ksat;                /**< 1d array [nr_crops] - regular Ksat of crops [mm/d] */ 
    
    dam_unit *servicing_dam;            /**< pointer - pointer to servicing dam infromation */
};

struct dam_units{
    RID_cell *cell;                     /**< pointer - pointer to cell infromation */
    bool run;                           /**< bool - TRUE = run FALSE = do not run (false when dam is not build yet) */
    
    size_t global_id;                   /**< scalar - global id of dam (from input file) */
    int activation_year;                /**< scalar - activation year of dam */
    char name[MAXSTRING];               /**< string - name of dam */
    size_t function;                    /**< scalar - function of dam (DAM_IRR_FUNCTION = irrigation DAM_HYD_FUNCTION = hydropower DAM_CON_FUNCTION = flood control) */
    double capacity;                    /**< scalar - storage capacity of dam [m3] */
    double area;                        /**< scalar - area of dam [m2] */
    double irrigated_area;              /**< scalar - irrigated area of dam [m2] */    
    
    double total_inflow;                /**< scalar - current total inflow [m3] */
    double total_inflow_natural;        /**< scalar - current total natural inflow [m3] */
    double total_demand;                /**< scalar - current total demand [m3] */
    
    double *inflow;                     /**< 1d array [DAM_CALC_YEARS_MEAN * MONTHS_PER_YEAR] - averaged time-step inflow for a month [m3/ts] */
    double *inflow_natural;             /**< 1d array [DAM_CALC_YEARS_MEAN * MONTHS_PER_YEAR] - averaged time-step inflow for a month [m3/ts] */
    double *demand;                     /**< 1d array [DAM_CALC_YEARS_MEAN * MONTHS_PER_YEAR] - averaged time-step demand for a month [m3/ts] */
    
    double ext_influence_factor;        /**< scalar - factor for external influences [m3/m3] */
    size_t extreme_stor;                /**< scalar - days this year dam storage was extreme (full or empty) [#] */
    
    double monthly_inflow;              /**< scalar - multi-year averaged time-step inflow [m3/ts] for current month */
    double monthly_inflow_natural;      /**< scalar - multi-year averaged time-step inflow [m3/ts] for current month */
    double monthly_demand;              /**< scalar - multi-year averaged time-step demand [m3/ts] for current month */
    
    double annual_inflow;               /**< scalar - multi-year averaged time-step demand [m3/ts] for current year */
    double annual_inflow_natural;       /**< scalar - multi-year averaged time-step demand [m3/ts] for current year */
    double annual_demand;               /**< scalar - multi-year averaged time-step demand [m3/ts] for current year */
    
    double release;                     /**< scalar - this months release [m3/ts] */
    double environmental_release;       /**< scalar - this months environmental release [m3/ts] */
    double previous_release;            /**< scalar - outflow to be released next timestep [m3] */
    
    size_t nr_serviced_cells;           /**< scalar - number of serviced cells */
    irr_cell **serviced_cells;          /**< 1d array [nr_serviced_cells] - pointer to serviced cells */
    
    dmy_struct start_operation;         /**< dmy_struct - start of the operation year of dam */
    double storage_start_operation;     /**< scalar - storage at the start of the operation year of dam [m3] */
    double current_storage;             /**< scalar - current storage of dam [m3] */ 
};

struct serviced_cells{
    irr_cell *cell;                     /**< pointer - pointer to irrigatoin cell information */
    dam_unit *dam;                      /**< pointer - pointer to irrigatoin dam information */
    double *moisture_content;           /**< 1d array [nr_crops] - moisture content of serviced cell's crops [mm] */ 
    double *demand_crop;                /**< 1d array [nr_crops] - demand of serviced cell's crops [m3] */ 
    double *deficit;                    /**< 1d array [nr_crops] - previous demand of serviced cell's crops [m3] */     
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
void do_irrigation_module(irr_cell *cur_irr, dmy_struct *cur_dmy);
bool in_irrigation_season(size_t crop_index, size_t current_julian_day);
void set_crop_ksat(bool irr_season, irr_cell *cur_irr, size_t crop_index);
void get_moisture_content(size_t cell_id, size_t veg_index, double *moisture_content);
void get_storage_infiltration(size_t cell_id, double *storage, double *infiltration, double moisture_content);
void increase_moisture_content(size_t cell_id, size_t veg_index, double *moisture_content, double irrigation_crop);
void get_demand(irr_cell *cur_irr, size_t veg_index, double *demand_crop, double moisture_content);
void get_irrigation(double *irrigation_crop, double demand_cell, double demand_crop, double available_water);
void increase_storage_content(size_t cell_id, size_t veg_index, double *storage, double increase);
void update_demand_and_irrigation(double irrigation_crop, double *demand_cell, double *demand_crop, double *available_water);

//Dam module
void do_dam_flow(dam_unit *cur_dam);
void do_dam_history_module(dam_unit *cur_dam, dmy_struct *cur_dmy);
void update_dam_history_day(dam_unit* cur_dam);
void update_dam_history_month(dam_unit* cur_dam, dmy_struct* cur_dmy);
void calculate_target_release(dam_unit* cur_dam);
void update_dam_history_year(dam_unit* cur_dam, dmy_struct* cur_dmy);
void calculate_operational_year(dam_unit* cur_dam, dmy_struct *cur_dmy);

void do_dam_module(dam_unit *cur_dam, dmy_struct *cur_dmy);
void get_actual_release(dam_unit* cur_dam, double *actual_release);
void get_demand_cells(double *demand_cells, double *demand_cell, double demand_crop);
void get_dam_irrigation(double demand_cells, double demand_crop, double *irrigation_crop, double available_water);
void update_dam_demand_and_irrigation(double *demand_cells, double *demand_cell, double *demand_crop, double *irrigation_cells, double *irrigation_cell, double irrigation_crop, double *available_water);
void do_dam_irrigation(size_t cell_id, size_t veg_index, double *moisture_content, double irrigation_crop);
void set_deficit(double *deficit, double *demand);
void get_dam_evaporation(dam_unit* cur_dam, double *evaporation);
void get_dam_overflow(dam_unit *cur_dam, double *overflow, double actual_release, double irrigation_cells, double evaporation);
void do_dam_release(dam_unit* cur_dam, double actual_release, double irrigation_release, double overflow, double evaporation);

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

