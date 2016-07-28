/******************************************************************************
 * @section DESCRIPTION
 *
 * Initialize routing model parameters
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2014 The Land Surface Hydrology Group, Department of Civil
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

#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>

#include <rout.h>

/******************************************************************************
 * @brief    Initialize routing model parameters
 *****************************************************************************/
void
rout_init(void)
{
    extern int mpi_rank;
    if (mpi_rank == VIC_MPI_ROOT) {
        extern rout_struct   rout;
        extern domain_struct global_domain;

        int                 *ivar = NULL;
        double              *dvar = NULL;
        double              lat_first, lon_first;
        int                 inv_reslat, inv_reslon;
        size_t              offset_x, offset_y;

        size_t               i, j;
        size_t               i1start;
        size_t               d3count[3];
        size_t               d3start[3];

        i1start = 0;

        d3start[0] = 0;
        d3start[1] = 0;
        d3start[2] = 0;
        d3count[0] = rout.rout_param.nTimesteps;
        d3count[1] = rout.rout_param.nSources;
        d3count[2] = 1; // tracers dimension

        struct timespec tstart={0,0}, tend={0,0};
        clock_gettime(CLOCK_MONOTONIC, &tstart);

        // allocate memory for variables to be read
        ivar = (int *) malloc(sizeof(int) * rout.rout_param.nSources);
        if (ivar == NULL) {
            log_err("Memory allocation error in vic_init().");
        }

        //clock_gettime(CLOCK_MONOTONIC, &tend); log_info("time: %.5f seconds\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)); clock_gettime(CLOCK_MONOTONIC, &tstart);
        // allocate memory for variables to be read
        dvar = (double *) malloc(
            rout.rout_param.nTimesteps * rout.rout_param.nSources *
            sizeof(double));
        if (dvar == NULL) {
            log_err("Memory allocation error in vic_init().");
        }

        // The Ring
        for (j = 0; j < rout.rout_param.nOutlets; j++) {
            for (i = 0; i < rout.rout_param.nTimesteps; i++) {
                rout.ring[j * rout.rout_param.nTimesteps + i] = 0.0;
            }
        }

        // discharge
        for (j = 0; j < global_domain.ncells_active; j++) {
            rout.discharge[j] = 0.0;
        }

        // source2outlet_ind: source to outlet index mapping
        get_nc_field_int(rout.param_filename,
                         "source2outlet_ind",
                         &i1start, &rout.rout_param.nSources, ivar);
        for (i = 0; i < rout.rout_param.nSources; i++) {
            rout.rout_param.source2outlet_ind[i] = (int) ivar[i];
        }

        // source_time_offset: Number of leading timesteps ommited
        get_nc_field_int(rout.param_filename,
                         "source_time_offset",
                         &i1start, &rout.rout_param.nSources, ivar);
        for (i = 0; i < rout.rout_param.nSources; i++) {
            rout.rout_param.source_time_offset[i] = (int) ivar[i];
        }

        // source_x_ind: x grid coordinate of source grid cell
        get_nc_field_int(rout.param_filename,
                         "source_x_ind",
                         &i1start, &rout.rout_param.nSources, ivar);
        for (i = 0; i < rout.rout_param.nSources; i++) {
            rout.rout_param.source_x_ind[i] = (int) ivar[i];
        }

        // source_y_ind: y grid coordinate of source grid cell
        get_nc_field_int(rout.param_filename,
                         "source_y_ind",
                         &i1start, &rout.rout_param.nSources, ivar);
        for (i = 0; i < rout.rout_param.nSources; i++) {
            rout.rout_param.source_y_ind[i] = (int) ivar[i];
        }

        // source_lat: Latitude coordinate of source grid cell
        get_nc_field_double(rout.param_filename,
                            "source_lat",
                            &i1start, &rout.rout_param.nSources, dvar);
        for (i = 0; i < rout.rout_param.nSources; i++) {
            rout.rout_param.source_lat[i] = (double) dvar[i];
        }

        // source_lon: Longitude coordinate of source grid cell
        get_nc_field_double(rout.param_filename,
                            "source_lon",
                            &i1start, &rout.rout_param.nSources, dvar);
        for (i = 0; i < rout.rout_param.nSources; i++) {
            rout.rout_param.source_lon[i] = (double) dvar[i];
        }

        // outlet_lat: Latitude coordinate of source grid cell
        get_nc_field_double(rout.param_filename,
                            "outlet_lat",
                            &i1start, &rout.rout_param.nOutlets, dvar);
        for (i = 0; i < rout.rout_param.nOutlets; i++) {
            rout.rout_param.outlet_lat[i] = (double) dvar[i];
        }

        //clock_gettime(CLOCK_MONOTONIC, &tend); log_info("time: %.5f seconds\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)); clock_gettime(CLOCK_MONOTONIC, &tstart);
        // outlet_lon: Longitude coordinate of source grid cell
        get_nc_field_double(rout.param_filename,
                            "outlet_lon",
                            &i1start, &rout.rout_param.nOutlets, dvar);
        for (i = 0; i < rout.rout_param.nOutlets; i++) {
            rout.rout_param.outlet_lon[i] = (double) dvar[i];
        }

        //clock_gettime(CLOCK_MONOTONIC, &tend); log_info("time: %.5f seconds\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)); clock_gettime(CLOCK_MONOTONIC, &tstart);
        // Unit Hydrograph:
        get_nc_field_double(rout.param_filename,
                            "unit_hydrograph",
                            d3start, d3count, dvar);
        for (i = 0;
             i < (rout.rout_param.nTimesteps * rout.rout_param.nSources);
             i++) {
            rout.rout_param.unit_hydrograph[i] = (double) dvar[i];
        }

        clock_gettime(CLOCK_MONOTONIC, &tstart);
        // TODO: Check inbouwen: wat als er geen VIC gridcell bestaat voor een Rout source?!
        // Mapping: Let the routing-source index numbers correspond to the VIC index numbers
        size_t iSource;
//        for (iSource = 0; iSource < rout.rout_param.nSources; iSource++) {
//            for (i = 0; i < global_domain.ncells_total; i++) {
//                if (rout.rout_param.source_lat[iSource] ==
//                    global_domain.locations[i].latitude &&
//                    rout.rout_param.source_lon[iSource] ==
//                    global_domain.locations[i].longitude) {
//                    rout.rout_param.source_VIC_index[iSource] = i;
//                    break;
//                }
//            }
//        }
        
        lat_first = global_domain.locations[0].latitude;
        lon_first = global_domain.locations[0].longitude;
        
        inv_reslat = 1./abs(global_domain.locations[0].latitude - global_domain.locations[global_domain.n_nx].latitude);
        inv_reslon = 1./abs(global_domain.locations[0].longitude - global_domain.locations[1].longitude);
        
        
        for (iSource = 0; iSource < rout.rout_param.nSources; iSource++) {
            offset_x = (rout.rout_param.source_lat[iSource] -lat_first)*inv_reslat;
            offset_y = (rout.rout_param.source_lon[iSource] -lon_first)*inv_reslon;

            rout.rout_param.source_VIC_index[iSource] = offset_x*global_domain.n_nx + offset_y;
        }

        clock_gettime(CLOCK_MONOTONIC, &tend); log_info("time: %.5f seconds\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)); clock_gettime(CLOCK_MONOTONIC, &tstart);
        // Check source index of VIC gridcell
        for (iSource = 0; iSource < rout.rout_param.nSources; iSource++) {
            if ((size_t)rout.rout_param.source_VIC_index[iSource] < 0 ||
                (size_t)rout.rout_param.source_VIC_index[iSource] >
                global_domain.ncells_total) {
                log_err("invalid source, index of VIC gridcell");
            }
        }

        //clock_gettime(CLOCK_MONOTONIC, &tend); log_info("time: %.5f seconds\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)); clock_gettime(CLOCK_MONOTONIC, &tstart);
        // Mapping: Let the routing-outlet index numbers correspond to the VIC index numbers
        size_t iOutlet;
        for (iOutlet = 0; iOutlet < rout.rout_param.nOutlets; iOutlet++) {
            for (i = 0; i < global_domain.ncells_total; i++) {
                if (rout.rout_param.outlet_lat[iOutlet] ==
                    global_domain.locations[i].latitude &&
                    rout.rout_param.outlet_lon[iOutlet] ==
                    global_domain.locations[i].longitude) {
                    rout.rout_param.outlet_VIC_index[iOutlet] = i;
                    //break;
                }
            }
        }

        //clock_gettime(CLOCK_MONOTONIC, &tend); log_info("time: %.5f seconds\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)); clock_gettime(CLOCK_MONOTONIC, &tstart);
        // Check outlet index of VIC gridcell
        for (iOutlet = 0; iOutlet < rout.rout_param.nOutlets; iOutlet++) {
            if ((size_t)rout.rout_param.outlet_VIC_index[iOutlet] < 0 ||
                (size_t)rout.rout_param.outlet_VIC_index[iOutlet] >
                global_domain.ncells_total) {
                log_err("invalid outlet, index of VIC gridcell");
            }
        }

        //clock_gettime(CLOCK_MONOTONIC, &tend); log_info("time: %.5f seconds\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)); clock_gettime(CLOCK_MONOTONIC, &tstart);
        // cleanup
        free(ivar);
        free(dvar);
        //clock_gettime(CLOCK_MONOTONIC, &tend); log_info("time: %.5f seconds\n",((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)); clock_gettime(CLOCK_MONOTONIC, &tstart);
    }
    log_err("stop");
}
