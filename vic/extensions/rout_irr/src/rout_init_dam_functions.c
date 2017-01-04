#include <rout.h>

void set_dam_location(){
    //Set dam information based on dam file
    extern module_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    
    dam_unit* dam_temp;
    dam_unit* dam;
    
    FILE *rf;
    char cmdstr[MAXSTRING];
    char optstr[MAXSTRING];
    
    char purpose[MAXSTRING];
    float lat;
    float lon;
    
    size_t i=0;
    size_t j;
    size_t k;
    size_t m;
    size_t x;
    size_t y;
    
    //Used to temporarily save dam information for checking
    dam_temp = malloc(MAX_NR_DAMS * sizeof(*dam_temp));
    check_alloc_status(dam_temp,"Memory allocation error.");
    
    //Read from dam file
    rf=open_file(rout.param.dam_filename,"r");
    fgets(cmdstr, MAXSTRING, rf);
    
    while (!feof(rf)) {        
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0') {
            sscanf(cmdstr, "%s", optstr);
            
            if (optstr[0] == '#') {
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            if(rout.nr_dams>=MAX_NR_DAMS){
                log_warn("there are more reservoirs in the area than is allowed, skipping other reservoirs");
            }
            
            dam = &dam_temp[rout.nr_dams];
            
            //Get the information from the dam file [id,name,year,capacity,purpose,lon,lat]
            sscanf(cmdstr, "%zu %200[^\t] %d %lf %s %f %f",&dam->global_id, dam->name,&dam->activation_year,&dam->capacity,purpose,&lon,&lat);
            
            if(lon ==DAM_NO_DATA || lat==DAM_NO_DATA){
                //reservoir location data not present
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }else if(lat<rout.min_lat - (global_param.resolution/2) || 
                    lon < rout.min_lon - (global_param.resolution/2) || 
                    lat>(rout.min_lat + ((global_domain.n_ny-1) * global_param.resolution) + (global_param.resolution/2)) || 
                    lon>(rout.min_lon + ((global_domain.n_nx-1) * global_param.resolution) + (global_param.resolution/2))){
                //reservoir outside location of interest
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }else if(dam->capacity==DAM_NO_DATA){
                log_warn("Capacity data of reservoir %s (file_id %zu) not present, skipping reservoir",dam->name,dam->global_id);
                //reservoir data not present
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }else if(dam->activation_year==DAM_NO_DATA || dam->activation_year > global_param.endyear){
                //reservoir outside time of interest
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }else{
                x = (size_t)((lon - rout.min_lon)/global_param.resolution);
                y = (size_t)((lat - rout.min_lat)/global_param.resolution);
                if(rout.gridded_cells[x][y]==NULL){
                    //reservoir inside location and time of interest but cell not run by VIC
                    fgets(cmdstr, MAXSTRING, rf);
                    continue;
                }
                
                //Dam is properly within the area and time domain
                dam->capacity *= (pow(M_PER_KM,3));
                dam->cell=rout.gridded_cells[x][y];
                
                if(purpose[0]=='I'){
                    dam->function=DAM_IRR_FUNCTION;
                }else if(purpose[0]=='H'){
                    dam->function=DAM_HYD_FUNCTION;
                }else if(purpose[0]=='C'){
                    dam->function=DAM_CON_FUNCTION;
                }else{
                    log_warn("Purpose of reservoir %s (file_id %zu) not properly defined; default to irrigation",dam->name,dam->global_id);
                    dam->function=DAM_IRR_FUNCTION;                    
                }
                
                rout.nr_dams++;
            }
        }
        fgets(cmdstr, MAXSTRING, rf);
    }
    
    //Allocating reservoirs based on number of reservoirs found
    rout.dams = malloc(rout.nr_dams * sizeof(*rout.dams));
    check_alloc_status(rout.dams,"Memory allocation error.");
    
    
    for(k=0;k<rout.nr_dams;k++){
        //Handle reservoirs in the same cell, they will be combined.
        //Problem is if the construction years or function are different
        //Currently function will default to irrigation and earliest year
        if(dam_temp[k].cell->dam!=NULL){
            dam_temp[k].cell->dam->capacity += dam_temp[k].capacity;
            
            if(dam_temp[k].cell->dam->function!=DAM_IRR_FUNCTION && dam_temp[k].function==DAM_IRR_FUNCTION){
                dam_temp[k].cell->dam->function=DAM_IRR_FUNCTION;
            }
            
            if(dam_temp[k].cell->dam->activation_year > dam_temp[k].activation_year){
                dam_temp[k].cell->dam->activation_year = dam_temp[k].activation_year;
            }
            
            log_warn("Reservoir %s and %s have been combined into reservoir %s because they are in the same cell."
                    " Capacities have been added. If any of the reservoirs had an irrigation function %s has an irrigation function."
                    " If reservoirs had different activation years the earliest activation year will be used for %s in the simulation.",
                    dam_temp[k].cell->dam->name,dam_temp[k].name,dam_temp[k].cell->dam->name,
                    dam_temp[k].cell->dam->name,
                    dam_temp[k].cell->dam->name);
            continue;
        }
        
        rout.dams[i]=dam_temp[k];
        
        rout.dams[i].id = i;
        rout.dams[i].cell->dam=&rout.dams[i];
        
        //Allocate all reservoir values
        rout.dams[i].demand = malloc(DAM_CALC_YEARS_MEAN * sizeof(*rout.dams[i].demand));
        check_alloc_status(rout.dams[i].demand,"Memory allocation error.");
        
        for(j=0;j<DAM_CALC_YEARS_MEAN;j++){
            rout.dams[i].demand[j] = malloc(MONTHS_PER_YEAR * sizeof(*rout.dams[i].demand[j]));  
            check_alloc_status(rout.dams[i].demand[j],"Memory allocation error.");                  
        }
        
        rout.dams[i].inflow = malloc(DAM_CALC_YEARS_MEAN * sizeof(*rout.dams[i].inflow));
        check_alloc_status(rout.dams[i].inflow,"Memory allocation error.");  
        
        for(j=0;j<DAM_CALC_YEARS_MEAN;j++){
            rout.dams[i].inflow[j] = malloc(MONTHS_PER_YEAR * sizeof(*rout.dams[i].inflow[j])); 
            check_alloc_status(rout.dams[i].inflow[j],"Memory allocation error.");              
        }
        
        rout.dams[i].inflow_natural = malloc(DAM_CALC_YEARS_MEAN * sizeof(*rout.dams[i].inflow_natural));
        check_alloc_status(rout.dams[i].inflow_natural,"Memory allocation error.");  
        
        for(j=0;j<DAM_CALC_YEARS_MEAN;j++){
            rout.dams[i].inflow_natural[j] = malloc(MONTHS_PER_YEAR * sizeof(*rout.dams[i].inflow_natural[j])); 
            check_alloc_status(rout.dams[i].inflow_natural[j],"Memory allocation error.");              
        }
        
        //Initialize all reservoir values
        for(y=0;y<DAM_CALC_YEARS_MEAN;y++){
            for(m=0;m<MONTHS_PER_YEAR;m++){
                rout.dams[i].inflow[y][m] = 0.0;
                rout.dams[i].demand[y][m] = 0.0;
                rout.dams[i].inflow_natural[y][m]=0.0;
                
            }
        }
        
        if(rout.dams[i].activation_year<=global_param.startyear){
            rout.dams[i].run=true;
        }else{
            rout.dams[i].run=false;
        }
        
        rout.dams[i].nr_serviced_cells = 0;
        rout.dams[i].current_demand = 0.0;
        rout.dams[i].current_inflow = 0.0;
        rout.dams[i].current_inflow_natural = 0.0;
        
        rout.dams[i].target_release = 0.0;
        
        rout.dams[i].monthly_demand=0.0;
        rout.dams[i].monthly_inflow=0.0;
        rout.dams[i].monthly_inflow_natural=0.0;
        rout.dams[i].annual_demand=0.0;
        rout.dams[i].annual_inflow=0.0;
        rout.dams[i].annual_inflow_natural=0.0;
        
        rout.dams[i].current_storage= rout.dams[i].capacity * RES_PREF_STORAGE; //preferred storage level for the start of the operational year (Hanasaki et al., 2006)
        rout.dams[i].storage_start_operation = rout.dams[i].current_storage;
        
        rout.dams[i].start_operation.day=global_param.startday;
        rout.dams[i].start_operation.month=global_param.startmonth;
        rout.dams[i].start_operation.year=global_param.startyear;
        
        i++;
            
    }
    
    rout.nr_dams=i;
    
    log_info("%zu reservoirs found inside area and time of interest",rout.nr_dams);
    
    free(dam_temp);
}

void set_dam_natural_routing(){
    //Set natural routing if dams are found in series
    //Because environmental release is based on natural inflow, and dams are in series, 
    //inflow is no longer natural but influenced by upstream dams
    //Therefore routing has to be done twice (2nd time while ignoring dams)
    //to calculate natural inflow
    extern module_struct rout;
    
    size_t i;
    module_cell* current_cell;
    
    for(i=0;i<rout.nr_dams;i++){
        current_cell = rout.dams[i].cell;
        
        while(current_cell->rout->downstream!=NULL && current_cell->rout->downstream->cell!=current_cell){
            //Follow the reservoir downstream
            current_cell = current_cell->rout->downstream->cell;
            if(current_cell->dam!=NULL){
                log_info("Reservoirs are found in series and therefore routing is done twice. "
                        "Second routing calculates natural stream flow "
                        "which is needed for environmental flow calculation");
                rout.param.fnaturalized_flow=true;
                break;
            }
        }
        if(rout.param.fnaturalized_flow){
            break;
        }
    }
}
    
void set_dam_irr_service(){
    //Set dam irrigation 
    //Set serviced cells for each dam
    //and servicing dams for each cell
    extern module_struct rout;
    extern soil_con_struct *soil_con;
    extern module_struct rout;
        
    size_t i;
    size_t j;
    size_t k;

    for(i=0;i<rout.nr_dams;i++){        
        if(rout.dams[i].function!=DAM_IRR_FUNCTION){
            continue;
        }
        
        for(j=0;j<rout.nr_irr_cells;j++){
            if(rout.dams[i].nr_serviced_cells >= DAM_MAX_SERVICE){
                log_warn("cell %zu not added to reservoir %zu because of maximum service capacity",rout.irr_cells[j].cell->id,rout.dams[i].id);
                continue;
            }
            
             if(rout.irr_cells[j].nr_servicing_dams >= CELL_MAX_SERVICE){
                log_warn("reservoir %zu not added to cell %zu because of maximum servicing reservoirs",rout.dams[i].id,rout.irr_cells[j].cell->id);
                continue;
            }
             
            if(soil_con[rout.irr_cells[j].cell->id].elevation >= soil_con[rout.dams[i].cell->id].elevation){
                //elevation of cell should be lower than the dam
                continue;
            }

            if(distance(rout.irr_cells[j].cell->x,rout.irr_cells[j].cell->y,
                    rout.dams[i].cell->x,rout.dams[i].cell->y)
                    >rout.param.max_distance_irr){
                continue;
            }
            
            rout.dams[i].nr_serviced_cells++;
            rout.irr_cells[j].nr_servicing_dams++;
        }
        
        //Allocate memory based on number of cells serviced
        rout.dams[i].serviced_cells=malloc(rout.dams[i].nr_serviced_cells * sizeof(*rout.dams[i].serviced_cells));
        check_alloc_status(rout.dams[i].serviced_cells,"Memory allocation error.");
        rout.dams[i].cell_demand=malloc(rout.dams[i].nr_serviced_cells * sizeof(*rout.dams[i].cell_demand));
        check_alloc_status(rout.dams[i].cell_demand,"Memory allocation error.");
        rout.dams[i].prev_deficit=malloc(rout.dams[i].nr_serviced_cells * sizeof(*rout.dams[i].prev_deficit));
        check_alloc_status(rout.dams[i].prev_deficit,"Memory allocation error.");
        
        k=0;
        for(j=0;j<rout.nr_irr_cells;j++){
            if(rout.dams[i].nr_serviced_cells >= DAM_MAX_SERVICE){
                continue;
            }
            
             if(rout.irr_cells[j].nr_servicing_dams >= CELL_MAX_SERVICE){
                continue;
            }
             
            if(soil_con[rout.irr_cells[j].cell->id].elevation >= soil_con[rout.dams[i].cell->id].elevation){
               continue;
            }

            if(distance(rout.irr_cells[j].cell->x,rout.irr_cells[j].cell->y,
                    rout.dams[i].cell->x,rout.dams[i].cell->y)
                    >rout.param.max_distance_irr){
                continue;
            }
            
            //Dam and cell are connected
            rout.dams[i].serviced_cells[k]=rout.irr_cells[j].cell;
            
            //Allocate memory based on number of crops in irrigated cell
            rout.dams[i].cell_demand[k]=malloc(rout.dams[i].serviced_cells[k]->irr->nr_crops * sizeof(*rout.dams[i].cell_demand[k]));
            check_alloc_status(rout.dams[i].cell_demand[k],"Memory allocation error.");
            rout.dams[i].prev_deficit[k]=malloc(rout.dams[i].serviced_cells[k]->irr->nr_crops * sizeof(*rout.dams[i].prev_deficit[k]));
            check_alloc_status(rout.dams[i].prev_deficit[k],"Memory allocation error.");
             
            k++;
        }
    }
    
    for(j=0;j<rout.nr_irr_cells;j++){
        
        //Allocate memory based on number of dams servicing
        rout.irr_cells[j].servicing_dams=malloc(rout.irr_cells[j].nr_servicing_dams * sizeof(*rout.irr_cells[j].servicing_dams));
        check_alloc_status(rout.irr_cells[j].servicing_dams,"Memory allocation error.");
        
        k=0;
        
        for(i=0;i<rout.nr_dams;i++){       
            if(rout.dams[i].function!=DAM_IRR_FUNCTION){
                continue;
            }
            
            if(rout.dams[i].nr_serviced_cells >= DAM_MAX_SERVICE){
                continue;
            }
            
             if(rout.irr_cells[j].nr_servicing_dams >= CELL_MAX_SERVICE){
                continue;
            }
             
            if(soil_con[rout.irr_cells[j].cell->id].elevation >= soil_con[rout.dams[i].cell->id].elevation){
               continue;
            }

            if(distance(rout.irr_cells[j].cell->x,rout.irr_cells[j].cell->y,
                    rout.dams[i].cell->x,rout.dams[i].cell->y)
                    >rout.param.max_distance_irr){
                continue;
            }
            
            //Cell and dam are connected
            rout.irr_cells[j].servicing_dams[k]=&rout.dams[i];
            k++;
        }
    }
}
