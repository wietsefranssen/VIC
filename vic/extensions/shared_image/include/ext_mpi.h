/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ext_mpi.h
 * Author: bram
 *
 * Created on November 14, 2017, 12:19 PM
 */

#ifndef EXT_MPI_H
#define EXT_MPI_H

#include <vic_mpi.h>
#include <ext_driver_shared_image.h>

void create_MPI_ext_option_struct_type(MPI_Datatype *mpi_type);
void create_MPI_ext_parameters_struct_type(MPI_Datatype *mpi_type);

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

