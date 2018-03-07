#ifndef EFR_H
#define EFR_H

#define EFR_HIST_YEARS 3
#define EFR_LOW_FLOW_FRAC 0.4
#define EFR_LOW_DEMAND_FRAC 0.6
#define EFR_HIGH_FLOW_FRAC 0.8
#define EFR_HIGH_DEMAND_FRAC 0.3

typedef struct{
    double ay_flow;
    double am_flow;
    double history_flow[EFR_HIST_YEARS * MONTHS_PER_YEAR];
    
    double requirement;
    
    double total_flow;
    size_t total_steps;
    
    size_t months_running;
}efr_var_struct;

bool efr_get_global_parameters(char *cmdstr);
void efr_validate_global_parameters(void);
void efr_set_output_meta_data_info(void);
void efr_set_state_meta_data_info(void);
void efr_alloc(void);
void initialize_efr_local_structures(void);
void efr_run(size_t cur_cell);
void efr_put_data(void);
void efr_finalize(void);
void efr_add_types(void);

efr_var_struct *efr_var;

#endif /* EFR_H */

