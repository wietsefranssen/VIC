/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <rout.h>
#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>

void
rout_start(void)
{
    extern global_param_struct global_param;
    
    global_param.resolution=VIC_RESOLUTION;
    
    extern filenames_struct filenames;
    extern filep_struct     filep;
    extern int              mpi_rank;

    if (mpi_rank == VIC_MPI_ROOT) {
        // read global settings
        filep.globalparam = open_file(filenames.global, "r");
        get_global_param_rout(filep.globalparam);
    }
}

void
get_global_param_rout(FILE *gp)
{
    extern rout_struct rout;
    char               cmdstr[MAXSTRING];
    char               optstr[MAXSTRING];

    /** Read through global control file to find parameters **/
    rewind(gp);
    fgets(cmdstr, MAXSTRING, gp);

    while (!feof(gp)) {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0') {
            sscanf(cmdstr, "%s", optstr);

            /* Handle case of comment line in which '#' is indented */
            if (optstr[0] == '#') {
                fgets(cmdstr, MAXSTRING, gp);
                continue;
            }

            /*************************************
               Get Model Global Parameters
            *************************************/
            if (strcasecmp("ROUT_PARAM", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", rout.param_filename);
            } 
            if (strcasecmp("ROUT_DEBUG_PARAM", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", rout.debug_path);
                break;
            }
        }
        fgets(cmdstr, MAXSTRING, gp);
    }
}