/******************************************************************************
 * @section DESCRIPTION
 *
 * This routine handles the startup tasks.
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

/******************************************************************************
 * @brief    Wrapper function for VIC startup tasks.
 *****************************************************************************/
void
vic_start(void)
{
    int                        status;
    extern filenames_struct    filenames;
    extern filep_struct        filep;
    extern global_param_struct global_param;
    extern MPI_Comm            MPI_COMM_VIC;
    extern MPI_Datatype        mpi_global_struct_type;
    extern MPI_Datatype        mpi_filenames_struct_type;
    extern MPI_Datatype        mpi_option_struct_type;
    extern MPI_Datatype        mpi_param_struct_type;
    extern int                 mpi_rank;
    extern option_struct       options;
    extern parameters_struct   param;
    extern size_t              NF;
    extern size_t              NR;

    // Initialize structures
    initialize_global_structures();

    if (mpi_rank == VIC_MPI_ROOT) {
        // Read the global parameter file
        filep.globalparam = open_file(filenames.global, "r");
        get_global_param(filep.globalparam);
        fclose(filep.globalparam);
        validate_global_param();
        
        // Read the model constants
        if (strcasecmp(filenames.constants, "MISSING")) {
            filep.constants = open_file(filenames.constants, "r");
            get_parameters(filep.constants);
            fclose(filep.constants);
            validate_parameters();
        }
        
        // Start all non specific VIC structures
        start_general();
        
        // Start all plugins
        if (options.ROUTING) {
            rout_start();
        }
        if (options.DAMS) {
            dam_start();
        }
        if (options.IRRIGATION) {
            irr_start();
        }
    }

    // broadcast option, filenames, global_param and param structures
    status = MPI_Bcast(&global_param, 1, mpi_global_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    status = MPI_Bcast(&options, 1, mpi_option_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    status = MPI_Bcast(&param, 1, mpi_param_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    status = MPI_Bcast(&filenames, 1, mpi_filenames_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    // broadcast global values such as NF and NR
    status = MPI_Bcast(&NF, 1, MPI_UNSIGNED_LONG, VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    status = MPI_Bcast(&NR, 1, MPI_UNSIGNED_LONG, VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    // Set Log Destination
    setup_logging(mpi_rank, filenames.log_path, &(filep.logfile));
}
