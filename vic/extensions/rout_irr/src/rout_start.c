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
    extern rout_struct rout;
    extern rout_options_struct rout_options;
    
    //Initialize resolution, currently not done by VIC
    //FIXME: should be user-adjustable or integrated in VIC
    global_param.resolution=VIC_RESOLUTION;
    
    //Initialize routing parameters
    rout_options.routing = false;
    rout_options.reservoirs = false;
    rout_options.debug_mode = false;
    rout_options.irrigation = false;
    rout.param_filename[0]=0;
    rout.debug_path[0]=0;
    rout.reservoir_filename[0]=0;
    rout_options.max_days_uh=MAX_DAYS_UH;
    rout_options.flow_velocity_uh=FLOW_VELOCITY_UH;
    rout_options.flow_diffusivity_uh=FLOW_DIFFUSIVITY_UH;
    rout_options.max_distance_irr=MAX_DISTANCE_IRR;
    
    //Get routing parameters from global parameter file
    extern filenames_struct filenames;
    extern filep_struct     filep;
    extern int              mpi_rank;
    if (mpi_rank == VIC_MPI_ROOT) {
        filep.globalparam = open_file(filenames.global, "r");
        get_global_param_rout(filep.globalparam);
    }
    
    //Check for unrealistic routing options
    check_routing_options();
    
    //Display routing options
    display_routing_options();
}

void
get_global_param_rout(FILE *gp)
{
    extern rout_struct rout;
    extern rout_options_struct rout_options;
    char               cmdstr[MAXSTRING];
    char               optstr[MAXSTRING];
    char               flgstr[MAXSTRING];

    rewind(gp);
    fgets(cmdstr, MAXSTRING, gp);

    while (!feof(gp)) {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0') {
            sscanf(cmdstr, "%s", optstr);

            //Handle case of comment line in which '#' is indented
            if (optstr[0] == '#') {
                fgets(cmdstr, MAXSTRING, gp);
                continue;
            }

            //Get routing parameters
            if (strcasecmp("ROUTING", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout_options.routing=str_to_bool(flgstr);
            }
            if (strcasecmp("IRRIGATION", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout_options.irrigation=str_to_bool(flgstr);
            }
            if (strcasecmp("RESERVOIRS", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout_options.reservoirs=str_to_bool(flgstr);
            }
            if (strcasecmp("ROUTING_DEBUG_MODE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout_options.debug_mode=str_to_bool(flgstr);
            }
            if (strcasecmp("ROUT_PARAM", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", rout.param_filename);
            } 
            if (strcasecmp("ROUT_RESERVOIR", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", rout.reservoir_filename);
            }
            if (strcasecmp("ROUT_DEBUG_PARAM", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", rout.debug_path);
            }
            if (strcasecmp("ROUT_UH_MAX_DAYS", optstr) == 0) {
                sscanf(cmdstr, "%*s %zu", &rout_options.max_days_uh);
            }
            if (strcasecmp("ROUT_UH_FLOW_VELOCITY", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &rout_options.flow_velocity_uh);
            }
            if (strcasecmp("ROUT_UH_FLOW_DIFFUSIVITY", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &rout_options.flow_diffusivity_uh);
            }
            if (strcasecmp("ROUT_IRR_MAX_DISTANCE", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &rout_options.max_distance_irr);
            }
        }
        fgets(cmdstr, MAXSTRING, gp);
    }
}

void check_routing_options(){
    extern rout_options_struct rout_options;
    extern rout_struct rout;
    
    if(rout_options.routing){
        if(rout.param_filename[0]==0){
            log_warn("No routing input files, but routing is selected. Setting ROUTING to FALSE..."); 
            rout_options.routing=false;
        }
    }else{
        if(rout_options.irrigation){
            log_warn("Irrigation is selected, but routing is not selected. Setting IRRIGATION to FALSE...");
            rout_options.irrigation=false; 
        }
        if(rout_options.reservoirs){
            log_warn("Reservoirs is selected, but routing is not selected. Setting RESERVOIRS to FALSE..."); 
            rout_options.reservoirs=false;
        }
    } 
    
    if(rout_options.reservoirs){
        if(rout.reservoir_filename[0]==0){
            log_warn("No reservoir input files, but reservoirs is selected. Setting RESERVOIRS to FALSE..."); 
            rout_options.reservoirs=false;
        }
    }
    if(rout_options.debug_mode){
        if(rout.debug_path[0]==0){
            log_warn("No debug output path, but debug is selected. Setting ROUTING_DEBUG_MODE to FALSE..."); 
            rout_options.debug_mode=false;
        }
    }
    if(rout_options.max_days_uh<1){
        log_warn("ROUT_UH_MAX_DAYS was smaller than 1. Setting ROUT_UH_MAX_DAYS to %d",MAX_DAYS_UH); 
            rout_options.max_days_uh=MAX_DAYS_UH;
    }
    if(rout_options.flow_velocity_uh<=0){
        log_warn("ROUT_UH_FLOW_VELOCITY was smaller than or equal to 0. Setting ROUT_UH_FLOW_VELOCITY to %.2f",FLOW_VELOCITY_UH); 
            rout_options.flow_velocity_uh=FLOW_VELOCITY_UH;
    }
    if(rout_options.flow_diffusivity_uh<=0){
        log_warn("ROUT_UH_FLOW_DIFFUSIVITY was smaller than or equal to 0. Setting ROUT_UH_FLOW_DIFFUSIVITY to %.2f",FLOW_DIFFUSIVITY_UH); 
            rout_options.flow_diffusivity_uh=FLOW_DIFFUSIVITY_UH;
    }
    if(rout_options.max_distance_irr<=0){
        log_warn("IRR_MAX_DISTANCE was smaller than or equal to 0. Setting IRR_MAX_DISTANCE to %.1f",MAX_DISTANCE_IRR); 
            rout_options.max_distance_irr=MAX_DISTANCE_IRR;
    }
}

void display_routing_options(){
    extern rout_options_struct rout_options;

    fprintf(LOG_DEST, "Current Routing Settings\n");
    if(rout_options.routing){
        fprintf(LOG_DEST, "ROUTING\t\t\t\tTRUE\n");
    }else{
        fprintf(LOG_DEST, "ROUTING\t\t\t\tFALSE\n");
    }
    if(rout_options.irrigation){
        fprintf(LOG_DEST, "IRRIGATION\t\t\tTRUE\n");
    }else{
        fprintf(LOG_DEST, "IRRIGATION\t\t\tFALSE\n");
    }
    if(rout_options.reservoirs){
        fprintf(LOG_DEST, "RESERVOIRS\t\t\tTRUE\n");
    }else{
        fprintf(LOG_DEST, "RESERVOIRS\t\t\tFALSE\n");
    }
    if(rout_options.debug_mode){
        fprintf(LOG_DEST, "ROUTING_DEBUG_MODE\t\tTRUE\n");
    }else{
        fprintf(LOG_DEST, "ROUTING_DEBUG_MODE\t\tFALSE\n");
    }
    fprintf(LOG_DEST, "ROUT_UH_MAX_DAYS\t\t%zu\n",rout_options.max_days_uh);
    fprintf(LOG_DEST, "ROUT_UH_FLOW_VELOCITY\t\t%.2f\n",rout_options.flow_velocity_uh);
    fprintf(LOG_DEST, "ROUT_UH_FLOW_DIFFUSIVITY\t%.1f\n",rout_options.flow_diffusivity_uh);
    fprintf(LOG_DEST, "ROUT_IRR_MAX_DISTANCE\t\t%.1f\n",rout_options.max_distance_irr);
    fprintf(LOG_DEST, "\n");
}