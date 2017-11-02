#ifndef EXT_DRIVER_SHARED_ALL_H
#define EXT_DRIVER_SHARED_ALL_H

#include <vic_driver_shared_all.h>
#include <ext_def.h>

void sizet_sort(size_t *array, size_t *cost, size_t Nelements, bool acending);
void sizet_sort2(size_t *array, int *cost, size_t Nelements, bool acending);
void double_flip(double *array, size_t Nelements);
void sizet_swap(size_t i, size_t j, size_t *array);
void int_swap(size_t i, size_t j, int *array);
void double_swap(size_t i, size_t j, double *array);

void cshift(double *data, int nx, int ny, int axis, int direction);

double no_leap_day_in_year_from_dmy(dmy_struct dmy);
double day_in_year_from_dmy(dmy_struct dmy);
void dmy_from_day_in_year(double day_in_year, int year, dmy_struct *dmy);

#endif

