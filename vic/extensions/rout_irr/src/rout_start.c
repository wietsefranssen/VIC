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
        
    size_t temp_crop_class[options.NVEGTYPES];
    unsigned short int temp_crop_season[options.NVEGTYPES][4];
    
    //Initialize parameters to default values
    initialize_routing_options();
    
    //Get parameters from global parameter file
    extern filenames_struct filenames;
    extern filep_struct     filep;
    extern int              mpi_rank;
    if (mpi_rank == VIC_MPI_ROOT) {
        filep.globalparam = open_file(filenames.global, "r");
        get_global_param_rout(filep.globalparam,temp_crop_class,temp_crop_season);
    }
    
    //Check for unrealistic parameters
    check_routing_options(temp_crop_class,temp_crop_season);
    
    //Display parameters
    display_routing_options();
}

void
initialize_routing_options(void){
    extern global_param_struct global_param;
    extern module_struct rout;
    
    //Initialize resolution, currently not done by VIC
    //FIXME: should be user-adjustable or integrated in VIC
    global_param.resolution=VIC_RESOLUTION;
    
    //Initialize parameters
    rout.param.firrigation = false;
    rout.param.fdams = false;
    rout.param.fdebug_mode = false;
    
    rout.param.param_filename[0]=0;
    rout.param.debug_path[0]=0;
    rout.param.dam_filename[0]=0;
    
    rout.param.max_days_uh=MAX_DAYS_UH;
    rout.param.flow_velocity_uh=FLOW_VELOCITY_UH;
    rout.param.flow_diffusivity_uh=FLOW_DIFFUSIVITY_UH;
    
    rout.param.nr_crop_classes=0;
    
    rout.param.max_distance_irr=MAX_DISTANCE_IRR;
    rout.param.fnaturalized_flow=false;
    
}

void
get_global_param_rout(FILE *gp, size_t temp_crop_class[], unsigned short int temp_crop_season[][4])
{
    extern module_struct rout;
    extern option_struct options;
    
    char               cmdstr[MAXSTRING];
    char               optstr[MAXSTRING];
    char               flgstr[MAXSTRING];
    
    size_t class;
    unsigned short int crop_sow;
    unsigned short int crop_developed;
    unsigned short int crop_matured;
    unsigned short int crop_harvest;
    bool duplicate;
    
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

            //Get parameters
            if (strcasecmp("IRRIGATION", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout.param.firrigation=str_to_bool(flgstr);
            }
            if (strcasecmp("DAMS", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout.param.fdams=str_to_bool(flgstr);
            }
            if (strcasecmp("ROUTING_DEBUG_MODE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout.param.fdebug_mode=str_to_bool(flgstr);
            }
            if (strcasecmp("POTENTIAL_IRRIGATION", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                rout.param.fpot_irrigation=str_to_bool(flgstr);
            }
            if (strcasecmp("ROUT_PARAM", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", rout.param.param_filename);
            } 
            if (strcasecmp("DAM_PARAM", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", rout.param.dam_filename);
            }
            if (strcasecmp("DEBUG_OUTPUT", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", rout.param.debug_path);
            }
            if (strcasecmp("UH_MAX_DAYS", optstr) == 0) {
                sscanf(cmdstr, "%*s %zu", &rout.param.max_days_uh);
            }
            if (strcasecmp("UH_FLOW_VELOCITY", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &rout.param.flow_velocity_uh);
            }
            if (strcasecmp("UH_FLOW_DIFFUSION", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &rout.param.flow_diffusivity_uh);
            }
            if (strcasecmp("DAM_IRR_DISTANCE", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &rout.param.max_distance_irr);
            }
            if (strcasecmp("CROP_CLASS", optstr) == 0) {
                class=0;
                crop_sow=0;
                crop_developed=0;
                crop_matured=0;
                crop_harvest=0;
                sscanf(cmdstr, "%*s %zu %hu %hu %hu %hu", &class, &crop_sow, &crop_developed, &crop_matured, &crop_harvest);
                
                if(rout.param.nr_crop_classes>=options.NVEGTYPES){
                    //Check if the number of crop classes exceeds the number of vegetation classes
                    log_warn("More crop classes given than available vegetation classes, skipping class %zu",class);
                }else if(class>options.NVEGTYPES){
                    //Check if the crop class is within the range of vegetation classes
                    log_warn("Crop class %zu is not an available vegetation class, skipping class %zu",class,class);
                }else{
                    //Check if the crop class has already been defined
                    duplicate=false;
                    for(i=0;i<rout.param.nr_crop_classes;i++){
                        if(temp_crop_class[i]==class-1){
                            duplicate=true;
                        }
                    }
                    
                    if(duplicate){
                        log_warn("Crop class %zu has already been assigned as a crop, removing duplicate",class);
                    }else{
                        temp_crop_class[rout.param.nr_crop_classes]=class-1;
                        temp_crop_season[rout.param.nr_crop_classes][0]=crop_sow;
                        temp_crop_season[rout.param.nr_crop_classes][1]=crop_developed;
                        temp_crop_season[rout.param.nr_crop_classes][2]=crop_matured;
                        temp_crop_season[rout.param.nr_crop_classes][3]=crop_harvest;
                        rout.param.nr_crop_classes++;
                    }
                }
            }
        }
        fgets(cmdstr, MAXSTRING, gp);
    }
}

void check_routing_options(size_t temp_crop_class[], unsigned short int temp_crop_season[][4]){
    extern module_struct rout;
    extern module_struct rout;
    
    size_t i;
    
    //Display all parameters
    if(rout.param.param_filename[0]==0){
        log_err("No routing input files, exiting simulation...");
    } 
    if(rout.param.fpot_irrigation){
        if(!rout.param.firrigation){
            log_warn("No irrigation, but potential irrigation is selected. Setting POTENTIAL_IRRIGATION to FALSE...");
            rout.param.fpot_irrigation=false;
        }
    }
    if(rout.param.firrigation){
        if(rout.param.nr_crop_classes==0){
            log_warn("No crop classes given, but irrigation is selected. Setting IRRIGATION to FALSE...");
            rout.param.firrigation=false;
        }
    }
    if(rout.param.fdams){
        if(rout.param.dam_filename[0]==0){
            log_warn("No reservoir input files, but reservoirs is selected. Setting DAMS to FALSE..."); 
            rout.param.fdams=false;
        }
    }
    if(rout.param.fdebug_mode){
        if(rout.param.debug_path[0]==0){
            log_warn("No debug output path, but debug is selected. Setting ROUTING_DEBUG_MODE to FALSE..."); 
            rout.param.fdebug_mode=false;
        }
    }
    
    if(rout.param.max_days_uh<1){
        log_warn("ROUT_UH_MAX_DAYS was smaller than 1. Setting UH_MAX_DAYS to %d",MAX_DAYS_UH); 
            rout.param.max_days_uh=MAX_DAYS_UH;
    }
    if(rout.param.flow_velocity_uh<=0){
        log_warn("ROUT_UH_FLOW_VELOCITY was smaller than or equal to 0. Setting UH_FLOW_VELOCITY to %.2f",FLOW_VELOCITY_UH); 
            rout.param.flow_velocity_uh=FLOW_VELOCITY_UH;
    }
    if(rout.param.flow_diffusivity_uh<=0){
        log_warn("ROUT_UH_FLOW_DIFFUSIVITY was smaller than or equal to 0. Setting UH_FLOW_DIFFUSIVITY to %.2f",FLOW_DIFFUSIVITY_UH); 
            rout.param.flow_diffusivity_uh=FLOW_DIFFUSIVITY_UH;
    }
    if(rout.param.max_distance_irr<=0){
        log_warn("IRR_MAX_DISTANCE was smaller than or equal to 0. Setting DAM_IRR_DISTANCE to %.1f",MAX_DISTANCE_IRR); 
            rout.param.max_distance_irr=MAX_DISTANCE_IRR;
    }
    
    rout.param.crop_class=malloc(rout.param.nr_crop_classes * sizeof(*rout.param.crop_class));
    check_alloc_status(rout.param.crop_class,"Memory allocation error.");
    rout.param.crop_sow=malloc(rout.param.nr_crop_classes * sizeof(*rout.param.crop_sow));
    check_alloc_status(rout.param.crop_sow,"Memory allocation error.");
    rout.param.crop_developed=malloc(rout.param.nr_crop_classes * sizeof(*rout.param.crop_developed));
    check_alloc_status(rout.param.crop_developed,"Memory allocation error.");
    rout.param.crop_matured=malloc(rout.param.nr_crop_classes * sizeof(*rout.param.crop_matured));
    check_alloc_status(rout.param.crop_matured,"Memory allocation error.");
    rout.param.crop_harvest=malloc(rout.param.nr_crop_classes * sizeof(*rout.param.crop_harvest));
    check_alloc_status(rout.param.crop_harvest,"Memory allocation error.");
    
    for(i=0;i<rout.param.nr_crop_classes;i++){
        rout.param.crop_class[i]=temp_crop_class[i];
        rout.param.crop_sow[i]=temp_crop_season[i][0];
        rout.param.crop_developed[i]=temp_crop_season[i][1];
        rout.param.crop_matured[i]=temp_crop_season[i][2];
        rout.param.crop_harvest[i]=temp_crop_season[i][3];
    }
}

void display_routing_options(){
    extern module_struct rout;
    
    size_t i;
    
    fprintf(LOG_DEST, "Current Routing Settings\n");

    fprintf(LOG_DEST, "UH_MAX_DAYS\t\t\t%zu\n",rout.param.max_days_uh);
    fprintf(LOG_DEST, "UH_FLOW_VELOCITY\t\t%.2f\n",rout.param.flow_velocity_uh);
    fprintf(LOG_DEST, "UH_FLOW_DIFFUSION\t\t%.1f\n",rout.param.flow_diffusivity_uh);
    
    if(rout.param.fdebug_mode){
        fprintf(LOG_DEST, "ROUTING_DEBUG_MODE\t\tTRUE\n");
    }
    
    if(rout.param.firrigation){
        fprintf(LOG_DEST, "\nCurrent Irrigation Settings\n");
        fprintf(LOG_DEST, "IRRIGATION\t\t\tTRUE\n");
        
        if(rout.param.fpot_irrigation){
            fprintf(LOG_DEST, "POTENTIAL_IRRIGATION\t\tTRUE\n");
        }else{
            fprintf(LOG_DEST, "POTENTIAL_IRRIGATION\t\tFALSE\n");
        }
        fprintf(LOG_DEST, "CROP_CLASS\tCROP_SOW\tCROP_DEVELOPED\tCROP_MATURED\tCROP_HARVEST\n");
        for(i=0;i<rout.param.nr_crop_classes;i++){
            fprintf(LOG_DEST, "%zu\t\t%hu\t\t%hu\t\t%hu\t\t%hu\n",
                    rout.param.crop_class[i]+1,rout.param.crop_sow[i],rout.param.crop_developed[i],rout.param.crop_matured[i],rout.param.crop_harvest[i]);
        }
    }else{
        fprintf(LOG_DEST, "\nIRRIGATION\t\t\tFALSE\n");
    }
    if(rout.param.fdams){
        fprintf(LOG_DEST, "\nCurrent Dam Settings\n");
        fprintf(LOG_DEST, "DAMS\t\t\t\tTRUE\n");
        fprintf(LOG_DEST, "DAM_IRR_DISTANCE\t\t%.1f\n",rout.param.max_distance_irr);
    }else{
        fprintf(LOG_DEST, "\nDAMS\t\t\t\tFALSE\n");
    }
}