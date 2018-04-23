/******************************************************************************
 * @section DESCRIPTION
 *
 * Header file for vic_driver_shared_all routines
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#ifndef VIC_SUPPORT_H
#define VIC_SUPPORT_H

#include <stdbool.h>
#include <vic_def.h>

bool between_dmy(dmy_struct start, dmy_struct end, dmy_struct current);
bool between_dmy(dmy_struct start, dmy_struct end, dmy_struct current);
dmy_struct dmy_from_diy(int diy);

double array_average(double *array, size_t repetitions, size_t length, size_t offset, size_t skip);

void sizet_sort(size_t *array, size_t *cost, size_t Nelements, bool acending, bool sort_cost);
void sizet_sort2(size_t *array, int *cost, size_t Nelements, bool acending);
void double_flip(double *array, size_t Nelements);
void sizet_swap(size_t i, size_t j, size_t *array);
void int_swap(size_t i, size_t j, int *array);
void double_swap(size_t i, size_t j, double *array);

int get_active_nc_field_double(nameid_struct *, char *, size_t *, size_t *, double *);
int get_active_nc_field_float(nameid_struct *, char *, size_t *, size_t *, float *);
int get_active_nc_field_int(nameid_struct *, char *, size_t *, size_t *, int *);

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

void cshift(double *, int, int, int, int);

#endif
