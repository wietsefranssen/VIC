#ifndef WATER_USE_H
#define WATER_USE_H

#include <stdbool.h>
#include <vic_driver_shared_image.h>

#define MAX_RETURN_STEPS 50
#define MAX_COMPENSATION_STEPS 100

enum{
    WU_IRRIGATION,
    WU_DOMESTIC,
    WU_INDUSTRIAL,
    WU_ENERGY,
    WU_LIVESTOCK,
    WU_ENVIRONMENTAL,
    WU_NSECTORS
};

enum{
    WU_RETURN_GROUNDWATER,
    WU_RETURN_SURFACEWATER,
    WU_RETURN_NLOCATIONS
};

enum{
    WU_INPUT_YEARLY,
    WU_INPUT_MONTHLY,
    WU_INPUT_DAILY,
    WU_INPUT_NFREQUENCIES
};

enum{
    WU_INPUT_FROM_FILE,
    WU_INPUT_CALCULATE,
    WU_INPUT_NONE,
    WU_INPUT_NLOCATIONS
};

typedef struct{
    double demand;
    double consumption_fraction;
}wu_con_struct;

typedef struct{
    double withdrawn;
    double consumed;
    double compensated;
    
    double compensation_total;
    double *compensation;
    
    double demand;
    double returned;
}wu_var_struct;

bool wu_get_global_parameters(char *cmdstr);
void wu_validate_global_parameters(void);
void wu_alloc(void);
void initialize_wu_local_structures(void);
void wu_set_output_meta_data_info(void);
void wu_set_state_meta_data_info(void);
bool wu_set_nc_var_info(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file);
bool wu_set_nc_var_dimids(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file);
void wu_force(void);
void wu_run(size_t cur_cell);
void wu_set_nc_output_file_info(nc_file_struct *nc_output_file);
void wu_write_def_dim(nc_file_struct *nc_output_file, stream_struct *stream);
void wu_write_def_dimvar(nc_file_struct *nc_output_file, stream_struct *stream);
void wu_write_put_dimvar(nc_file_struct *nc_output_file, stream_struct *stream);
void wu_finalize(void);


#endif