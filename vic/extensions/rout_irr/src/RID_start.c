/******************************************************************************
 * @section DESCRIPTION
 *  
 * Set and display options for the routing, irrigation and dam module
 ******************************************************************************/

#include <rout.h>
#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>

/******************************************************************************
 * @section brief
 *  
 * Set and display options for the routing, irrigation and dam module
 ******************************************************************************/
void RID_start(void){
    extern int              mpi_rank;
    extern filenames_struct filenames;
    extern filep_struct     filep;
        
    if (mpi_rank == VIC_MPI_ROOT) {
        size_t nr_crops;
        size_t **crop_info;
    
        default_module_options();
    
        filep.globalparam = open_file(filenames.global, "r");
        get_module_options(filep.globalparam,&nr_crops,&crop_info);
    
        check_module_options(nr_crops,crop_info);
    
        display_module_options();
    }
}

/******************************************************************************
 * @section brief
 *  
 * Set options for the routing, irrigation and dam module to their default 
 * value
 ******************************************************************************/
void default_module_options(void){
    extern global_param_struct global_param;
    extern RID_struct RID;
    
    //FIXME: resolution should be user-adjustable and set in VIC
    global_param.resolution=VIC_RESOLUTION;
    
    RID.param.firrigation = false;
    RID.param.fdams = false;
    RID.param.fdebug_mode = false;
    
    RID.param.param_filename[0]=0;
    RID.param.debug_path[0]=0;
    RID.param.dam_filename[0]=0;
    
    RID.param.flow_velocity=DEF_FLOW_VEL;
    RID.param.flow_diffusivity=DEF_FLOW_DIF;
    
    RID.param.nr_crops=0;
    RID.param.crop_ksat=DEF_CROP_KSAT;
    
    RID.param.fnaturalized_flow=false;
    RID.param.fenv_flow=true;
    RID.param.dam_irr_distance=DEF_IRR_DIST;
}

/******************************************************************************
 * @section brief
 *  
 * Get options for the routing, irrigation and dam module from the global
 * parameter file
 ******************************************************************************/
void get_module_options(FILE *gp, size_t *nr_crops, size_t ***crop_info){
    extern RID_struct RID;
    extern option_struct options;
    
    char               cmdstr[MAXSTRING];
    char               optstr[MAXSTRING];
    char               flgstr[MAXSTRING];
        
    size_t i;
    size_t iCrop;
    
    /*******************************
     Count the number of crops and allocate space to store information
    *******************************/
    
    *nr_crops = 0;
    
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
            
            if (strcasecmp("CROP_CLASS", optstr) == 0) {
                (*nr_crops)++;
            }
        }
            
        fgets(cmdstr, MAXSTRING, gp);
    }
    
    *crop_info = malloc(*nr_crops * sizeof(*(*crop_info)));
    check_alloc_status((*crop_info),"Memory allocation error.");
    for(i=0;i<*nr_crops;i++){
        (*crop_info)[i]=malloc(3 * sizeof(*(*crop_info[i])));
        check_alloc_status((*crop_info)[i],"Memory allocation error.");
        
        //set default values for crop vegetation class, 
        //sow date and harvest date
        (*crop_info)[i][0]=options.NVEGTYPES+1;
        (*crop_info)[i][1]=DAYS_PER_YEAR+1;
        (*crop_info)[i][2]=DAYS_PER_YEAR+1;
    }
        
    /*******************************
     Get parameter values
    *******************************/
    
    iCrop=0;
    
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

            if (strcasecmp("IRRIGATION", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                RID.param.firrigation=str_to_bool(flgstr);
            }
            if (strcasecmp("DAMS", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                RID.param.fdams=str_to_bool(flgstr);
            }
            if (strcasecmp("DEBUG_MODE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                RID.param.fdebug_mode=str_to_bool(flgstr);
            }
            
            if (strcasecmp("ROUT_PARAM", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", RID.param.param_filename);
            } 
            if (strcasecmp("DAM_PARAM", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", RID.param.dam_filename);
            }
            if (strcasecmp("DEBUG_OUTPUT", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", RID.param.debug_path);
            }
            
            if (strcasecmp("UH_FLOW_VELOCITY", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &RID.param.flow_velocity);
            }
            if (strcasecmp("UH_FLOW_DIFFUSION", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &RID.param.flow_diffusivity);
            }
            if (strcasecmp("CROP_KSAT", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &RID.param.crop_ksat);
            }
            if (strcasecmp("DAM_IRR_DISTANCE", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &RID.param.dam_irr_distance);
            }
            if (strcasecmp("DAM_ENV_RELEASE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                RID.param.fenv_flow=str_to_bool(flgstr);
            }
            
            if (strcasecmp("CROP_CLASS", optstr) == 0) {
                sscanf(cmdstr, "%*s %zu %zu %zu", &(*crop_info)[iCrop][0], &(*crop_info)[iCrop][1], &(*crop_info)[iCrop][2]);
                iCrop++;
            }
        }
        fgets(cmdstr, MAXSTRING, gp);
    }
}    

/******************************************************************************
 * @section brief
 *  
 * Check options for the routing, irrigation and dam module from the global
 * parameter file
 ******************************************************************************/               
void check_module_options(size_t nr_crops, size_t **crop_info){
    extern RID_struct RID;
    extern option_struct options;
    
    size_t i;
    size_t j;
    bool duplicate;
        
    /*******************************
     Check crop parameters
    *******************************/
        
    for(i=0;i<nr_crops;i++){
        
        duplicate=false;
    
        if(crop_info[i][0]>=options.NVEGTYPES){
            log_warn("Crop vegetation class does not exist, ignoring crop with vegetation class %zu",crop_info[i][0]);
            continue;
        }
        
        for(j=0;j<nr_crops;j++){
            if(j==i){
                continue;
            }
            
            if(crop_info[i][0]==crop_info[j][0]){
                
                if(crop_info[i][1]<crop_info[i][2]){
                    if((crop_info[j][1]<=crop_info[i][2] && crop_info[j][1]>=crop_info[i][1]) || 
                            (crop_info[j][2]>=crop_info[i][1] && crop_info[j][2]<=crop_info[i][2])){
                        log_warn("Crop vegetation class has same growing season, ignoring crop with vegetation class %zu",crop_info[i][0]);
                        duplicate=true;
                        break;
                    }
                }else{
                    if((crop_info[j][1]>=crop_info[i][2] && crop_info[j][1]<=crop_info[i][1]) || 
                            (crop_info[j][2]<=crop_info[i][1] && crop_info[j][2]>=crop_info[i][2])){
                        log_warn("Crop vegetation class has same growing season, ignoring crop with vegetation class %zu",crop_info[i][0]);
                        duplicate=true;
                        break;
                    }
                }
            }
        }
        
        if(duplicate){
            continue;
        }
        
        if(crop_info[i][1]>365 || crop_info[i][1]<1 || crop_info[i][2]>365 || crop_info[i][2]<1){
            log_warn("Crop has sowing or harvesting dates that are incorrect, ignoring crop with vegetation class %zu",crop_info[i][0]);
            continue;
        }
        
        RID.param.nr_crops++;
    }
    
    RID.param.crop_class=malloc(RID.param.nr_crops * sizeof(*RID.param.crop_class));
    check_alloc_status(RID.param.crop_class,"Memory allocation error.");
    RID.param.start_irr=malloc(RID.param.nr_crops * sizeof(*RID.param.start_irr));
    check_alloc_status(RID.param.start_irr,"Memory allocation error.");
    RID.param.end_irr=malloc(RID.param.nr_crops * sizeof(*RID.param.end_irr));
    check_alloc_status(RID.param.end_irr,"Memory allocation error.");
    
    for(i=0;i<nr_crops;i++){
        
        duplicate=false;
    
        if(crop_info[i][0]>=options.NVEGTYPES){
            continue;
        }
        
        
        for(j=0;j<nr_crops;j++){
            if(j==i){
                continue;
            }
            
            if(crop_info[i][0]==crop_info[j][0]){
                
                if(crop_info[i][1]<crop_info[i][2]){
                    if((crop_info[j][1]<=crop_info[i][2] && crop_info[j][1]>=crop_info[i][1]) || 
                            (crop_info[j][2]>=crop_info[i][1] && crop_info[j][2]<=crop_info[i][2])){
                        duplicate=true;
                        break;
                    }
                }else{
                    if((crop_info[j][1]>=crop_info[i][2] && crop_info[j][1]<=crop_info[i][1]) || 
                            (crop_info[j][2]<=crop_info[i][1] && crop_info[j][2]>=crop_info[i][2])){
                        duplicate=true;
                        break;
                    }
                }
            }
        }
        
        if(duplicate){
            continue;
        }
        
        if(crop_info[i][1]>365 || crop_info[i][1]<1 || crop_info[i][2]>365 || crop_info[i][2]<1){
            continue;
        }
        
        RID.param.crop_class[i]=crop_info[i][0]-1;
        RID.param.start_irr[i]=crop_info[i][1];
        RID.param.end_irr[i]=crop_info[i][2];
    }
    
    for(i=0;i<nr_crops;i++){
        free(crop_info[i]);
    }
    free(crop_info);
    
    /*******************************
     Check other parameters
    *******************************/
    if(RID.param.param_filename[0]==0){
        log_err("No routing input files, exiting simulation...");
    }
    if(RID.param.firrigation){
        if(RID.param.nr_crops==0){
            log_warn("No crops given, but irrigation is selected. Setting IRRIGATION to FALSE...");
            RID.param.firrigation=false;
        }
    }
    if(RID.param.fdams){
        if(RID.param.dam_filename[0]==0){
            log_warn("No dam input files, but dams are selected. Setting DAMS to FALSE..."); 
            RID.param.fdams=false;
        }
    }
    if(RID.param.fdebug_mode){
        if(RID.param.debug_path[0]==0){
            log_warn("No debug output path, but debug is selected. Setting DEBUG_MODE to FALSE..."); 
            RID.param.fdebug_mode=false;
        }
    }
    
    if(RID.param.flow_velocity<=0){
        log_warn("ROUT_UH_FLOW_VELOCITY was smaller than or equal to 0. Setting UH_FLOW_VELOCITY to %.2f",DEF_FLOW_VEL); 
            RID.param.flow_velocity=DEF_FLOW_VEL;
    }
    if(RID.param.flow_diffusivity<=0){
        log_warn("ROUT_UH_FLOW_DIFFUSIVITY was smaller than or equal to 0. Setting UH_FLOW_DIFFUSIVITY to %.2f",DEF_FLOW_DIF); 
            RID.param.flow_diffusivity=DEF_FLOW_DIF;
    }
    if(RID.param.crop_ksat<=0){
        log_warn("CROP_KSAT was smaller than or equal to 0. Setting CROP_KSAT to %.1f",DEF_CROP_KSAT); 
            RID.param.crop_ksat=DEF_CROP_KSAT;
    }
    if(RID.param.dam_irr_distance<=0){
        log_warn("DAM_IRR_DISTANCE was smaller than or equal to 0. Setting DAM_IRR_DISTANCE to %.1f",DEF_IRR_DIST); 
            RID.param.dam_irr_distance=DEF_IRR_DIST;
    }
}

/******************************************************************************
 * @section brief
 *  
 * Display options for the routing, irrigation and dam module from the global
 * parameter file
 ******************************************************************************/
void display_module_options(){
    extern RID_struct RID;
    
    size_t i;
    
    fprintf(LOG_DEST, "Current Routing Settings\n");

    fprintf(LOG_DEST, "UH_FLOW_VELOCITY\t\t%.2f\n",RID.param.flow_velocity);
    fprintf(LOG_DEST, "UH_FLOW_DIFFUSION\t\t%.1f\n",RID.param.flow_diffusivity);
    
    if(RID.param.fdebug_mode){
        fprintf(LOG_DEST, "DEBUG_MODE\t\tTRUE\n");
    }else{
        fprintf(LOG_DEST, "DEBUG_MODE\t\tFALSE\n");
    }
    
    if(RID.param.firrigation){
        fprintf(LOG_DEST, "\nCurrent Irrigation Settings\n");
        fprintf(LOG_DEST, "IRRIGATION\t\t\tTRUE\n");
        
        fprintf(LOG_DEST, "CROP_CLASS\tCROP_SOW\tCROP_DEVELOPED\tCROP_MATURED\tCROP_HARVEST\n");
        for(i=0;i<RID.param.nr_crops;i++){
            fprintf(LOG_DEST, "%zu\t\t%hu\t\t%hu\n",
                    RID.param.crop_class[i]+1,RID.param.start_irr[i],RID.param.end_irr[i]);
        }
        fprintf(LOG_DEST, "CROP_KSAT\t\t%.1f\n",RID.param.crop_ksat);
    }else{
        fprintf(LOG_DEST, "\nIRRIGATION\t\t\tFALSE\n");
    }
    if(RID.param.fdams){
        fprintf(LOG_DEST, "\nCurrent Dam Settings\n");
        fprintf(LOG_DEST, "DAMS\t\t\t\tTRUE\n");
        fprintf(LOG_DEST, "DAM_IRR_DISTANCE\t\t%.1f\n",RID.param.dam_irr_distance);
        if(RID.param.fenv_flow){
            fprintf(LOG_DEST, "DAM_ENV_FLOW\t\t\tTRUE\n");
        }else{
            fprintf(LOG_DEST, "DAM_ENV_FLOW\t\t\tFALSE\n");
        }
    }else{
        fprintf(LOG_DEST, "\nDAMS\t\t\t\tFALSE\n");
    }
}