#ifndef EXT_DRIVER_SHARED_IMAGE_H
#define EXT_DRIVER_SHARED_IMAGE_H

#include <vic_driver_shared_image.h>
#include <ext_driver_shared_all.h>
#include <ext_mpi.h>
#include <routing.h>

typedef struct{
    double UH_FLOW_VELOCITY;
    double UH_FLOW_DIFFUSION;
    int UH_MAX_LENGTH;
    int UH_PARTITIONS;
    
    double MPI_N_PROCESS_COST;
    double MPI_E_PROCESS_COST;
}ext_parameters_struct;

typedef struct{
    bool ROUTING;
    
    int MPI_DECOMPOSITION;
}ext_option_struct;

typedef struct {
    char routing[MAXSTRING];
} ext_filenames_struct;

void initialize_ext_global_structures(void);
void initialize_ext_options(ext_option_struct *options);
void initialize_ext_filenames(ext_filenames_struct *filenames);
void initialize_ext_parameters(ext_parameters_struct *parameters);

void ext_alloc(void);
void ext_init(void);
void ext_finalize(void);

#endif


