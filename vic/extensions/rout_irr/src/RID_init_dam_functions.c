/******************************************************************************
 * @section DESCRIPTION
 *  
 * Functions used in rout_init
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Set dam information from file. The file contains information on: id,
 * name, capacity, build year, purpose and location. Checks are made to
 * exclude dams outside area and time of interest, dams in the same cell are
 * combined.
 ******************************************************************************/

void set_dam_information(){
    extern RID_struct RID;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    
    size_t id_tmp;
    char name_tmp[MAXSTRING];
    int purp_tmp;
    double cap_tmp;
    double area_tmp;
    int year_tmp;
    float lat_tmp;
    float lon_tmp;
    
    dam_unit *dams;
    dam_unit *dams2;
    
    FILE *rf;
    char cmdstr[MAXSTRING];
    char optstr[MAXSTRING];
        
    size_t nr_dams;
    size_t iDam;
    size_t i;
    size_t j;
    size_t x;
    size_t y;
    bool combine;
        
    rf=open_file(RID.param.dam_filename,"r");
    fgets(cmdstr, MAXSTRING, rf);
    
    /*******************************
     Find number of dams. Handle cases of dam outside of area or time
    *******************************/    
    nr_dams=0;
    while (!feof(rf)) {        
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0') {
            sscanf(cmdstr, "%s", optstr);
            
            if (optstr[0] == '#') {
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            sscanf(cmdstr, "%*u %*200[^\t] %d %lf %*f %*u %f %f",&year_tmp,&cap_tmp,&lon_tmp,&lat_tmp);
            
            if(lat_tmp<RID.min_lat - (global_param.resolution/2) || 
                    lon_tmp < RID.min_lon - (global_param.resolution/2) || 
                    lat_tmp > (RID.min_lat + ((global_domain.n_ny-1) * global_param.resolution) + (global_param.resolution/2)) || 
                    lon_tmp > (RID.min_lon + ((global_domain.n_nx-1) * global_param.resolution) + (global_param.resolution/2)) ||
                    lat_tmp==DAM_NO_DATA || lon_tmp==DAM_NO_DATA){
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            if(year_tmp > global_param.endyear || year_tmp==DAM_NO_DATA){
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            x = (size_t)((lon_tmp - (RID.min_lon-(global_param.resolution/2)))/global_param.resolution);
            y = (size_t)((lat_tmp - (RID.min_lat-(global_param.resolution/2)))/global_param.resolution);
            if(RID.gridded_cells[x][y]==NULL){
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            if(cap_tmp<=0 || cap_tmp==DAM_NO_DATA){
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            nr_dams++;            
        }
        fgets(cmdstr, MAXSTRING, rf);
    }
        
    dams = malloc(nr_dams * sizeof(*dams));
    check_alloc_status(dams,"Memory allocation error.");
    dams2 = malloc(nr_dams * sizeof(*dams2));
    check_alloc_status(dams2,"Memory allocation error.");
    
    for(i=0;i<nr_dams;i++){
        dams[i].name[0]=0;
        dams[i].capacity=0;
        dams[i].activation_year=9999;
        dams[i].function=DAM_HYD_FUNCTION;
    }
    
    
    /*******************************
     Assign dam information
    *******************************/  
    i=0;
    rewind(rf);
    fgets(cmdstr, MAXSTRING, rf);
    while (!feof(rf)) {        
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0') {
            sscanf(cmdstr, "%s", optstr);
            
            if (optstr[0] == '#') {
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            sscanf(cmdstr, "%zu %200[^\t] %d %lf %lf %d %f %f",&id_tmp,name_tmp,&year_tmp,&cap_tmp, &area_tmp ,&purp_tmp,&lon_tmp,&lat_tmp);
            
            if(lat_tmp<RID.min_lat - (global_param.resolution/2) || 
                    lon_tmp < RID.min_lon - (global_param.resolution/2) || 
                    lat_tmp > (RID.min_lat + ((global_domain.n_ny-1) * global_param.resolution) + (global_param.resolution/2)) || 
                    lon_tmp > (RID.min_lon + ((global_domain.n_nx-1) * global_param.resolution) + (global_param.resolution/2)) ||
                    lat_tmp==DAM_NO_DATA || lon_tmp==DAM_NO_DATA){
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            if(year_tmp > global_param.endyear || year_tmp==DAM_NO_DATA){
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            x = (size_t)((lon_tmp - (RID.min_lon-(global_param.resolution/2)))/global_param.resolution);
            y = (size_t)((lat_tmp - (RID.min_lat-(global_param.resolution/2)))/global_param.resolution);
            if(RID.gridded_cells[x][y]==NULL){
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            if(cap_tmp<=0 || cap_tmp==DAM_NO_DATA){
                debug("Dam capacity is missing");
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            if((purp_tmp!=DAM_HYD_FUNCTION && purp_tmp!=DAM_IRR_FUNCTION &&
                    purp_tmp!=DAM_CON_FUNCTION) || purp_tmp==DAM_NO_DATA){
                debug("Defaulting dam to hydropower function");
                purp_tmp = DAM_HYD_FUNCTION;
            }
            
            if(area_tmp==DAM_NO_DATA){
                //Takeuchi (1997) formula for estimating reservoir area
                debug("Approximating dam area due to missing data");
                area_tmp = pow((cap_tmp / 9.208),(1/1.114));
            }
            
            dams[i].global_id=id_tmp;            
            strncpy(dams[i].name,name_tmp,MAXSTRING);
            dams[i].capacity=cap_tmp * 1000000;
            dams[i].area=area_tmp * 1000000;
            dams[i].activation_year=year_tmp;
            dams[i].function=(size_t)purp_tmp;
            dams[i].cell = RID.gridded_cells[x][y];
            i++;          
            
            if(i>nr_dams){
                break;
            }
        }
        fgets(cmdstr, MAXSTRING, rf);
    }
    
    /*******************************
     Check dams located on the same cell
    *******************************/
    iDam=0;
    for(i=0;i<nr_dams;i++){
        
        combine=false;
        for(j=0;j<RID.nr_dams;j++){
            if(dams[i].cell==dams2[j].cell){
                log_warn("\ndam %s and %s have been combined into dam %s because they are in the same cell.\n"
                    "Capacities and areas have been added. Purpose is based on the biggest dam.\n"
                    "If the dams have different activation years the earliest activation year is used.\n",
                    dams[i].name,dams2[j].name,dams2[j].name);

                if(dams2[j].capacity < dams[i].capacity){
                    dams2[j].function = dams[i].function;
                }

                dams2[j].capacity += dams[i].capacity;
                dams2[j].area += dams[i].area;

                if(dams2[j].activation_year > dams[i].activation_year){
                    dams2[j].activation_year = dams[i].activation_year;
                }

                combine=true;
                break;
            }
        }

        if(combine){
            continue;
        }

        dams2[iDam]=dams[i];
        RID.nr_dams++;            
        iDam++;
    }
    
    
    /*******************************
     Assign dams and allocate & initialize values
    *******************************/
    RID.dams = malloc(RID.nr_dams * sizeof(*RID.dams));
    check_alloc_status(RID.dams,"Memory allocation error.");
    
    for(i=0;i<RID.nr_dams;i++){
        RID.dams[i]=dams2[i];
        RID.dams[i].cell->dam=&RID.dams[i];
        
        RID.dams[i].demand = malloc(DAM_HIST_YEARS * MONTHS_PER_YEAR * sizeof(*RID.dams[i].demand));
        check_alloc_status(RID.dams[i].demand,"Memory allocation error.");
        
        RID.dams[i].inflow = malloc(DAM_HIST_YEARS * MONTHS_PER_YEAR * sizeof(*RID.dams[i].inflow));
        check_alloc_status(RID.dams[i].inflow,"Memory allocation error.");
        
        RID.dams[i].inflow_natural = malloc(DAM_HIST_YEARS * MONTHS_PER_YEAR * sizeof(*RID.dams[i].inflow_natural));
        check_alloc_status(RID.dams[i].inflow_natural,"Memory allocation error.");
        
        if(RID.dams[i].activation_year<=global_param.startyear){
            RID.dams[i].run=true;
        }else{
            RID.dams[i].run=false;
        }
        
        RID.dams[i].release = 0.0;   
        RID.dams[i].previous_release = 0.0;
        
        RID.dams[i].ext_influence_factor=DAM_EXT_INF_DEFAULT;
        RID.dams[i].extreme_stor=0;
        RID.dams[i].irrigated_area=0;
        
        //preferred storage level for the start of the operational year (Hanasaki et al., 2006)
        RID.dams[i].current_storage= RID.dams[i].capacity * DAM_PREF_STORE; 
        RID.dams[i].storage_start_operation = RID.dams[i].current_storage;
        
        RID.dams[i].start_operation.day=global_param.startday;
        RID.dams[i].start_operation.month=global_param.startmonth;
        RID.dams[i].start_operation.year=global_param.startyear; 
        
        RID.dams[i].total_demand = 0.0;
        RID.dams[i].total_inflow = 0.0;
        RID.dams[i].total_inflow_natural = 0.0;
        
        for(j=0;j<DAM_HIST_YEARS * MONTHS_PER_YEAR;j++){
            RID.dams[i].inflow[j] = 0.0;
            RID.dams[i].demand[j] = 0.0;
            RID.dams[i].inflow_natural[j]=0.0;
        }
        
        RID.dams[i].monthly_demand=0.0;
        RID.dams[i].monthly_inflow=0.0;
        RID.dams[i].monthly_inflow_natural=0.0;
        
        RID.dams[i].annual_demand=0.0;
        RID.dams[i].annual_inflow=0.0;
        RID.dams[i].annual_inflow_natural=0.0;
                
        RID.dams[i].nr_serviced_cells = 0;           
    }
        
    log_info("%zu dams found inside area and time of interest",RID.nr_dams);
    
    free(dams);
    free(dams2);
}

/******************************************************************************
 * @section brief
 * 
 * If dams are located in series, upstream dams influence the inflow of
 * downstream dams. Therefore to calculate the environmental release
 * (10% of the natural outflow) routing has to occur twice. The second routing
 * will ignore dams and therefore calculate the natural outflow.
 ******************************************************************************/

void set_dam_natural_routing(){
    extern RID_struct RID;
    
    size_t i;
    RID_cell* current_cell;
    
    for(i=0;i<RID.nr_dams;i++){
        current_cell = RID.dams[i].cell;
        
        while(current_cell->rout->downstream!=NULL && current_cell->rout->downstream->cell!=current_cell){
            current_cell = current_cell->rout->downstream->cell;
            if(current_cell->dam!=NULL){
                log_info("Dams are found in series and therefore routing is done twice. "
                        "Second routing calculates natural stream flow "
                        "which is needed for environmental flow calculation");
                RID.param.fnaturalized_flow=true;
                break;
            }
        }
        if(RID.param.fnaturalized_flow){
            break;
        }
    }
}