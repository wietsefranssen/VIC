/******************************************************************************
 * @section DESCRIPTION
 *
 * Save model state.
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

#include <vic.h>

void
rout_output_metadata(void)
{    
    extern metadata_struct *out_metadata;
    extern node            *outvar_types;

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DISCHARGE")].varname, "OUT_DISCHARGE");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DISCHARGE")].long_name, "discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DISCHARGE")].standard_name,
           "discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DISCHARGE")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_DISCHARGE")].description,
           "discharge");

    strcpy(out_metadata[list_search_id(outvar_types, "OUT_NAT_DISCHARGE")].varname, "OUT_NAT_DISCHARGE");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_NAT_DISCHARGE")].long_name, "natural_discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_NAT_DISCHARGE")].standard_name,
           "natural_discharge");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_NAT_DISCHARGE")].units, "m3/s");
    strcpy(out_metadata[list_search_id(outvar_types, "OUT_NAT_DISCHARGE")].description,
           "natural_discharge");
    
    out_metadata[list_search_id(outvar_types, "OUT_DISCHARGE")].nelem = 1;
    out_metadata[list_search_id(outvar_types, "OUT_NAT_DISCHARGE")].nelem = 1;
}

void
rout_state_metadata(void)
{    
  
}