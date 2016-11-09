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
    extern option_struct options;
    extern rout_sa_struct rout_sa;
    extern domain_struct global_domain;
    
    rout_sa.n_active=global_domain.ncells_active;
    rout_sa.n_total=global_domain.ncells_total;
    rout_sa.nx=global_domain.n_nx;
    rout_sa.ny=global_domain.n_ny;
        
    size_t temp_crop_class[options.NVEGTYPES];
    
    initialize_routing_options();
    
    //Get routing parameters from global parameter file
    extern filenames_struct filenames;
    extern filep_struct     filep;
    extern int              mpi_rank;
    if (mpi_rank == VIC_MPI_ROOT) {
        filep.globalparam = open_file(filenames.global, "r");
        get_global_param_rout(filep.globalparam,temp_crop_class);
    }
    
    //Check for unrealistic routing options
    check_routing_options(temp_crop_class);
    
    //Display routing options
    display_routing_options();
}

void
initialize_routing_options(void){
    extern global_param_struct global_param;
    extern rout_struct rout;
    
    //Initialize resolution, currently not done by VIC
    //FIXME: should be user-adjustable or integrated in VIC
    global_param.resolution=VIC_RESOLUTION;
    
    //Initialize routing parameters
    rout.firrigation = false;
    rout.freservoirs = false;
    rout.fdebug_mode = false;
    
    rout.param_filename[0]=0;
    rout.debug_path[0]=0;
    rout.reservoir_filename[0]=0;
    
    rout.fuh_file=false;
    
    rout.max_days_uh=MAX_DAYS_UH;
    rout.flow_velocity_uh=FLOW_VELOCITY_UH;
    rout.flow_diffusivity_uh=FLOW_DIFFUSIVITY_UH;
    rout.max_distance_irr=MAX_DISTANCE_IRR;
    rout.fnaturalized_flow=false;
    
    rout.crop_developed=CROP_DATE_DEFAULT;
    rout.crop_end=CROP_DATE_DEFAULT;
    rout.crop_late=CROP_DATE_DEFAULT;
    rout.crop_start=CROP_DATE_DEFAULT;
    
    rout.nr_crop_classes=0;
}

void
get_global_param_rout(FILE *gp, size_t temp_crop_class[])
{
    extern rout_struct rout;
    extern option_struct options;
    
    char               cmdstr[MAXSTRING];
    char               optstr[MAXSTRING];
    char               flgstr[MAXSTRING];
    
    size_t class;
    bool duplicate;
    char source[MAXSTRING];
    
    size_t i;

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
            if (strcasecmp("IRRIGATION", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout.firrigation=str_to_bool(flgstr);
            }
            if (strcasecmp("RESERVOIRS", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout.freservoirs=str_to_bool(flgstr);
            }
            if (strcasecmp("ROUTING_DEBUG_MODE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout.fdebug_mode=str_to_bool(flgstr);
            }
            if (strcasecmp("POTENTIAL_IRRIGATION", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout.fpot_irrigation=str_to_bool(flgstr);
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
                sscanf(cmdstr, "%*s %zu", &rout.max_days_uh);
            }
            if (strcasecmp("ROUT_UH_FLOW_VELOCITY", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &rout.flow_velocity_uh);
            }
            if (strcasecmp("ROUT_UH_FLOW_DIFFUSIVITY", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &rout.flow_diffusivity_uh);
            }
            if (strcasecmp("ROUT_IRR_MAX_DISTANCE", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &rout.max_distance_irr);
            }
            if (strcasecmp("ROUT_UH_PARAM", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", source);
                if(strcmp(source,"FROM_PARAM_FILE")==0){
                    rout.fuh_file=true;
                }
            }
            if (strcasecmp("CROP_START", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &rout.crop_start);
            }
            if (strcasecmp("CROP_DEVELOPED", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &rout.crop_developed);
            }
            if (strcasecmp("CROP_LATE", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &rout.crop_late);
            }
            if (strcasecmp("CROP_END", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &rout.crop_end);
            }
            if (strcasecmp("CROP_CLASS", optstr) == 0) {
                class=0;
                sscanf(cmdstr, "%*s %zu", &class);
                
                if(rout.nr_crop_classes>=options.NVEGTYPES){
                    log_warn("More crop classes given than available vegetation classes, skipping class %zu",class);
                }else if(class>options.NVEGTYPES){
                    log_warn("Crop class %zu is not an available vegetation class, skipping class %zu",class,class);
                }else{
                    duplicate=false;
                    for(i=0;i<rout.nr_crop_classes;i++){
                        if(temp_crop_class[i]==class-1){
                            duplicate=true;
                        }
                    }
                    
                    if(duplicate){
                        log_warn("Crop class %zu has already been assigned as a crop, removing duplicate",class);
                    }else{
                        temp_crop_class[rout.nr_crop_classes]=class-1;
                        rout.nr_crop_classes++;
                    }
                }
            }
        }
        fgets(cmdstr, MAXSTRING, gp);
    }
}

void check_routing_options(size_t temp_crop_class[]){
    extern rout_struct rout;
    extern rout_struct rout;
    
    size_t i;
    
    if(rout.param_filename[0]==0){
        log_err("No routing input files, exiting simulation...");
    } 
    if(rout.fpot_irrigation){
        if(!rout.firrigation){
            log_warn("No irrigation, but potential irrigation is selected. Setting POTENTIAL_IRRIGATION to FALSE...");
            rout.fpot_irrigation=false;
        }
        if(rout.freservoirs){
            log_warn("Reservoir irrigation, but potential irrigation is selected. Setting RESERVOIRS to FALSE...");
            rout.freservoirs=false;
        }
    }
    if(rout.firrigation){
        if(rout.nr_crop_classes==0){
            log_warn("No crop classes given, but irrigation is selected. Setting IRRIGATION to FALSE...");
            rout.firrigation=false;
        }
    }
    if(rout.freservoirs){
        if(rout.reservoir_filename[0]==0){
            log_warn("No reservoir input files, but reservoirs is selected. Setting RESERVOIRS to FALSE..."); 
            rout.freservoirs=false;
        }
    }
    if(rout.fdebug_mode){
        if(rout.debug_path[0]==0){
            log_warn("No debug output path, but debug is selected. Setting ROUTING_DEBUG_MODE to FALSE..."); 
            rout.fdebug_mode=false;
        }
    }
    
    if(rout.max_days_uh<1){
        log_warn("ROUT_UH_MAX_DAYS was smaller than 1. Setting ROUT_UH_MAX_DAYS to %d",MAX_DAYS_UH); 
            rout.max_days_uh=MAX_DAYS_UH;
    }
    if(rout.flow_velocity_uh<=0){
        log_warn("ROUT_UH_FLOW_VELOCITY was smaller than or equal to 0. Setting ROUT_UH_FLOW_VELOCITY to %.2f",FLOW_VELOCITY_UH); 
            rout.flow_velocity_uh=FLOW_VELOCITY_UH;
    }
    if(rout.flow_diffusivity_uh<=0){
        log_warn("ROUT_UH_FLOW_DIFFUSIVITY was smaller than or equal to 0. Setting ROUT_UH_FLOW_DIFFUSIVITY to %.2f",FLOW_DIFFUSIVITY_UH); 
            rout.flow_diffusivity_uh=FLOW_DIFFUSIVITY_UH;
    }
    if(rout.max_distance_irr<=0){
        log_warn("IRR_MAX_DISTANCE was smaller than or equal to 0. Setting IRR_MAX_DISTANCE to %.1f",MAX_DISTANCE_IRR); 
            rout.max_distance_irr=MAX_DISTANCE_IRR;
    }
    if(rout.crop_developed<=0 || rout.crop_developed>DAYS_PER_YEAR ||
            rout.crop_late<=0 || rout.crop_late>DAYS_PER_YEAR ||
            rout.crop_end<=0 || rout.crop_developed>DAYS_PER_YEAR ||
            rout.crop_start<=0 || rout.crop_start>DAYS_PER_YEAR){
        log_warn("Crop growing days were outside of a realistic range. Setting all CROP dates to %d",CROP_DATE_DEFAULT); 
            
        rout.crop_developed=CROP_DATE_DEFAULT;
        rout.crop_end=CROP_DATE_DEFAULT;
        rout.crop_late=CROP_DATE_DEFAULT;
        rout.crop_start=CROP_DATE_DEFAULT;
    }
    
    rout.crop_class=malloc(rout.nr_crop_classes * sizeof(rout.crop_class));
    check_alloc_status(rout.crop_class,"Memory allocation error.");
    
    for(i=0;i<rout.nr_crop_classes;i++){
        rout.crop_class[i]=temp_crop_class[i];
    }
}

void display_routing_options(){
    extern rout_struct rout;
    
    size_t i;
    
    fprintf(LOG_DEST, "Current Routing Settings\n");
    if(rout.firrigation){
        fprintf(LOG_DEST, "IRRIGATION\t\t\tTRUE\n");
    }else{
        fprintf(LOG_DEST, "IRRIGATION\t\t\tFALSE\n");
    }
    if(rout.fpot_irrigation){
        fprintf(LOG_DEST, "POTENTIAL_IRRIGATION\t\tTRUE\n");
    }else{
        fprintf(LOG_DEST, "POTENTIAL_IRRIGATION\t\tFALSE\n");
    }
    if(rout.freservoirs){
        fprintf(LOG_DEST, "RESERVOIRS\t\t\tTRUE\n");
    }else{
        fprintf(LOG_DEST, "RESERVOIRS\t\t\tFALSE\n");
    }
    if(rout.fdebug_mode){
        fprintf(LOG_DEST, "ROUTING_DEBUG_MODE\t\tTRUE\n");
    }else{
        fprintf(LOG_DEST, "ROUTING_DEBUG_MODE\t\tFALSE\n");
    }
    if(rout.fuh_file){
        fprintf(LOG_DEST, "ROUT_UH_PARAM\t\t\tFROM_PARAM_FILE\n");
    }else{
        fprintf(LOG_DEST, "ROUT_UH_PARAM\t\t\tFROM_DEFAULT\n");
        fprintf(LOG_DEST, "ROUT_UH_MAX_DAYS\t\t%zu\n",rout.max_days_uh);
        fprintf(LOG_DEST, "ROUT_UH_FLOW_VELOCITY\t\t%.2f\n",rout.flow_velocity_uh);
        fprintf(LOG_DEST, "ROUT_UH_FLOW_DIFFUSIVITY\t%.1f\n",rout.flow_diffusivity_uh);
        fprintf(LOG_DEST, "ROUT_IRR_MAX_DISTANCE\t\t%.1f\n",rout.max_distance_irr);
    }
    fprintf(LOG_DEST, "CROP_START\t\t\t%d\n",rout.crop_start);
    fprintf(LOG_DEST, "CROP_DEVELOPED\t\t\t%d\n",rout.crop_developed);
    fprintf(LOG_DEST, "CROP_LATE\t\t\t%d\n",rout.crop_late);
    fprintf(LOG_DEST, "CROP_END\t\t\t%d\n",rout.crop_end);
    fprintf(LOG_DEST, "NR_CROP_CLASSES\t\t\t%zu\n",rout.nr_crop_classes);
    for(i=0;i<rout.nr_crop_classes;i++){
        fprintf(LOG_DEST, "CROP_CLASS\t\t\t%zu\n",rout.crop_class[i]);
    }
    fprintf(LOG_DEST, "\n");
}