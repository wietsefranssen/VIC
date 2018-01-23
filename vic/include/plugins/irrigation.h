#ifndef IRRIGATION_H
#define IRRIGATION_H

#include <stddef.h>

#include <stdbool.h>

#define FIELD_CAP_FRAC 0.7
#define IRRIGATION_FRAC 0.9
#define POND_FRAC 0.3
#define POND_DEF_CAPACITY 20

typedef struct{
    size_t ni_types;
    size_t ni_active;
    
    int *vidx;
    int *iidx;
}irr_con_map_struct;

typedef struct{
    bool ponding;
    double pond_capacity;
    
    size_t nseasons;
    int *season_start;
    int *season_end;
}irr_con_struct;

typedef struct{
    double pond_storage;    
    double leftover;  
    
    double prev_demand;
    double prev_avail_moist;
    
    double requirement;
    double need;
}irr_var_struct;

bool irr_get_global_parameters(char *cmdstr);
void irr_validate_global_parameters(void);
void irr_start(void);
void irr_output_metadata(void);
void irr_state_metadata(void);
void irr_alloc(void);
void initialize_irr_local_structures(void);
void irr_init(void);
void irr_generate_default_state(void);
void irr_run1(size_t cur_cell);
void irr_run2(size_t cur_cell);
void irr_put_data(void);
void irr_finalize(void);
void irr_add_types(void);

irr_var_struct ***irr_var;
irr_con_struct **irr_con;
irr_con_map_struct *irr_con_map;

#endif /* IRRIGATION_H */

