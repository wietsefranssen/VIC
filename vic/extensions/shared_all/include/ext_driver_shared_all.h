
#ifndef EXT_DRIVER_SHARED_ALL_H
#define EXT_DRIVER_SHARED_ALL_H

#include <ext_def.h>

typedef struct{
    double UH_FLOW_VELOCITY;
    double UH_FLOW_DIFFUSION;
    int UH_MAX_LENGTH;
    int UH_PARTITIONS;
    
} ext_parameters_struct;

void initialize_ext_global_structures(void);
void initialize_ext_options(void);
void initialize_ext_filenames(void);
void initialize_ext_parameters(void);

void sizet_sort(size_t *array, size_t *cost, size_t Nelements, bool acending);
void sizet_sort2(size_t *array, int *cost, size_t Nelements, bool acending);
void sizet_swap(size_t i, size_t j, size_t *array);
void int_swap(size_t i, size_t j, int *array);

#endif

