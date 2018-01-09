#ifndef EXT_DRIVER_SHARED_ALL_H
#define EXT_DRIVER_SHARED_ALL_H

#include <stdbool.h>
#include <ext_debug.h>
#include <vic_def.h>

bool between_dmy(dmy_struct start, dmy_struct end, dmy_struct current);
dmy_struct dmy_from_diy(int diy);

void cshift(double *data, int nx, int ny, int axis, int direction);

void sizet_sort(size_t *array, size_t *cost, size_t Nelements, bool acending);
void sizet_sort2(size_t *array, int *cost, size_t Nelements, bool acending);
void double_flip(double *array, size_t Nelements);
void sizet_swap(size_t i, size_t j, size_t *array);
void int_swap(size_t i, size_t j, int *array);
void double_swap(size_t i, size_t j, double *array);

void gather_double(double *dvar, double *var_local);
void gather_double_2d(double **dvar, double **var_local, int depth);
void gather_int(int *ivar, int *var_local);
void gather_int_2d(int **ivar, int **var_local, int depth);
void gather_sizet(size_t *svar, size_t *var_local);
void gather_sizet_2d(size_t **svar, size_t **var_local, int depth);
void scatter_double(double *dvar, double *var_local);
void scatter_double_2d(double **dvar, double **var_local, int depth);
void scatter_int(int *ivar, int *var_local);
void scatter_int_2d(int **ivar, int **var_local, int depth);
void scatter_sizet(size_t *svar, size_t *var_local);
void scatter_sizet_2d(size_t **svar, size_t **var_local, int depth);

#endif

