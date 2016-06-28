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
    extern rout_struct rout;
    extern filenames_struct filenames;
    extern filep_struct     filep;
    extern int              mpi_rank;

    if (mpi_rank == 0) {
        filep.globalparam = open_file(filenames.global, "r");
        FILE *gp;
        gp = filep.globalparam;
    
        char cmdstr[MAXSTRING];
        char optstr[MAXSTRING];

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
                    sscanf(cmdstr, "%*s %s", rout.fileName);
                    break;
                }
            }
            fgets(cmdstr, MAXSTRING, gp);
        }
    }
}