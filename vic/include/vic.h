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

#ifndef VIC_H
#define VIC_H

#include <vic_run.h>
#include <vic_version.h>

#include <vic_image_log.h>
#include <vic_mpi.h>

#include <netcdf.h>

#include <vic_general.h>
#include <vic_support.h>
#include <routing_lohmann.h>
#include <routing_rvic.h>

typedef struct node {
    int id;
    char name[MAXSTRING];
    int data;
    struct node *next;
} node;

int N_STATE_VARS_ALL;
int N_OUTVAR_TYPES_ALL;

node *list_create(node *next, char *name);
node *list_prepend(node *head, char *name);
node *list_search(node *head, int id);
int list_search_id(node *head, char *name);
void list_dispose(node *head);
void list_free(node *head);
int list_count(node *head);
node *list_insertion_sort(node *head);
node *list_reverse(node *head);
void list_print(node *head);
node *list_add_ids(node *head, int start);

#endif
