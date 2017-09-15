/******************************************************************************
 * @section DESCRIPTION
 *
 * Functions to support reading from netCDF field.
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

#include <ext_driver_shared_image.h>

/******************************************************************************
 * @brief    Read double precision netCDF field from file.
 *****************************************************************************/
void
get_active_nc_field_double(char   *nc_name,
                    char   *var_name,
                    size_t *start,
                    size_t *count,
                    double *var)
{
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    double *dvar = NULL;
    
    dvar =
            malloc(global_domain.ncells_total * sizeof(*dvar));
        check_alloc_status(dvar, "Memory allocation error.");
    
    get_nc_field_double(nc_name, var_name, start, count, dvar);
        
    // filter the active cells only
    map(sizeof(double), global_domain.ncells_active, filter_active_cells, NULL,
        dvar, var);
}

/******************************************************************************
 * @brief    Read single precision netCDF field from file.
 *****************************************************************************/
void
get_active_nc_field_float(char   *nc_name,
                   char   *var_name,
                   size_t *start,
                   size_t *count,
                   float  *var)
{
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    float *fvar = NULL;
    
    fvar =
            malloc(global_domain.ncells_total * sizeof(*fvar));
        check_alloc_status(fvar, "Memory allocation error.");
    
    get_nc_field_float(nc_name, var_name, start, count, fvar);
        
    // filter the active cells only
    map(sizeof(float), global_domain.ncells_active, filter_active_cells, NULL,
        fvar, var);
}

/******************************************************************************
 * @brief    Read integer netCDF field from file.
 *****************************************************************************/
void
get_active_nc_field_int(char   *nc_name,
                 char   *var_name,
                 size_t *start,
                 size_t *count,
                 int    *var)
{
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    int *ivar = NULL;
    
    ivar =
            malloc(global_domain.ncells_total * sizeof(*ivar));
        check_alloc_status(ivar, "Memory allocation error.");
    
    get_nc_field_int(nc_name, var_name, start, count, ivar);
        
    // filter the active cells only
    map(sizeof(int), global_domain.ncells_active, filter_active_cells, NULL,
        ivar, var);
}
