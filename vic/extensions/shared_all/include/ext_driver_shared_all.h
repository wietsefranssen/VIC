
#ifndef EXT_DRIVER_SHARED_ALL_H
#define EXT_DRIVER_SHARED_ALL_H

#include <ext_def.h>

void sizet_sort(size_t *array, size_t *cost, size_t Nelements, bool acending);
void sizet_sort2(size_t *array, int *cost, size_t Nelements, bool acending);
void sizet_swap(size_t i, size_t j, size_t *array);
void int_swap(size_t i, size_t j, int *array);

#endif

