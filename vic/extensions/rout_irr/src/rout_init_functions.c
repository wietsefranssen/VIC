#include <rout.h>

void set_cells(){
    /*The function instantiates the cells in rout.cells
     *      
     * It does so by gathering the information from VIC,
     * and instantiates all the variables of a cell.
     */
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    
    rout.nr_reservoirs=0;
    rout.min_lat=DBL_MAX;
    rout.min_lon=DBL_MAX;
    
    size_t i;
    size_t j=0;
    size_t x;
    size_t y;
    
    for(i=0;i<global_domain.ncells_total;i++){
        if(global_domain.locations[i].run){
            rout.cells[j].global_domain_id=i;
            rout.cells[j].id=j;
            j++;
        }
        
        if(global_domain.locations[i].latitude<rout.min_lat){
            rout.min_lat=global_domain.locations[i].latitude;
        }
        if(global_domain.locations[i].longitude<rout.min_lon){
            rout.min_lon=global_domain.locations[i].longitude;
        }
    }
    
    for(i=0;i<global_domain.ncells_active;i++){
        x = (size_t)((local_domain.locations[i].longitude - rout.min_lon)/global_param.resolution);
        y = (size_t)((local_domain.locations[i].latitude - rout.min_lat)/global_param.resolution);
        rout.gridded_cells[x][y]=&rout.cells[i];
        rout.cells[i].x=x;
        rout.cells[i].y=y;
        
        for(j=0;j<rout.max_days_uh * global_param.model_steps_per_day;j++){
            rout.cells[i].uh[j]=0.0;
            rout.cells[i].outflow[j]=0.0;
            rout.cells[i].outflow_natural[j]=0.0;
        }
        
        rout.cells[i].nr_servicing_reservoirs=0;
        rout.cells[i].nr_upstream=0;
        rout.cells[i].servicing_reservoirs=malloc(0);
    }
}

void set_cell_irrigate(){
    /*The function determines irrigation requirements per cell
     *      
     * It does so by checking for each of the specified crops
     * if the crop is present in the cell. If so irrigation
     * is set to true and nr_crop_classes is increased for the cell
     */
    extern domain_struct global_domain;
    extern veg_con_map_struct *veg_con_map;
    extern rout_struct rout;
    
    size_t i;
    size_t j;
    
    for(i=0;i<global_domain.ncells_active;i++){
        rout.cells[i].irrigate=false;
        rout.cells[i].nr_crops=0;
        
        for(j=0;j<rout.nr_crop_classes;j++){
            if(veg_con_map[i].vidx[rout.crop_class[j]]!=NODATA_VEG){
                rout.cells[i].irrigate=true;
                rout.cells[i].nr_crops++;
            }
        }
    }
}

void set_upstream_downstream(char file_path[], char variable_name[]){
    /* The function sets the upstream and downstream pointers of each cell.
     * 
     * It does so by using the flow direction specified in the NETcdf file.
     * If the direction points to an active cell, the downstream pointer is
     * set to that cell. Afterwards the upstream cells are computed from 
     * these downstream cells.
     */
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    int *direction;
    rout_cell** upstream_temp;
    
    size_t i;
    size_t j;
    size_t x;
    size_t y;
    
    direction = malloc(global_domain.ncells_active * sizeof(*direction));
    check_alloc_status(direction,"Memory allocation error.");
    
    upstream_temp = malloc(8 * sizeof(*upstream_temp));
    check_alloc_status(upstream_temp,"Memory allocation error.");
    
    //Get data from NETcdf file
    size_t start[]={0, 0};
    size_t count[]={global_domain.n_ny, global_domain.n_nx};
    get_scatter_nc_field_int(file_path,variable_name,start,count,direction);
          
    for(i=0;i<global_domain.ncells_active;i++){        
        x=rout.cells[i].x;
        y=rout.cells[i].y;
            
        if(direction[i]==-1){
            log_warn("direction of cell (global_id %zu local_id %zu) is missing, check direction file",rout.gridded_cells[x][y]->global_domain_id,rout.gridded_cells[x][y]->id);
            continue;
        }
        
        /* Set downstream cell based on direction 
         * If direction is pointing to an active cell
         * 1=North
         *  2=North-east
         *  3=East
         *  etc.
         */
        
        if(direction[i]==1){
            if(y+1<global_domain.n_ny && rout.gridded_cells[x][y+1]!=NULL){
                rout.gridded_cells[x][y]->downstream=rout.gridded_cells[x][y+1];
            }
        }else if(direction[i]==2){
            if(x+1<global_domain.n_nx && y+1<global_domain.n_ny && rout.gridded_cells[x+1][y+1]!=NULL){
                rout.gridded_cells[x][y]->downstream=rout.gridded_cells[x+1][y+1];
            }
        }else if(direction[i]==3){
            if(x+1<global_domain.n_nx && rout.gridded_cells[x+1][y]!=NULL){
                rout.gridded_cells[x][y]->downstream=rout.gridded_cells[x+1][y];
            }
        }else if(direction[i]==4){
            if(x+1<global_domain.n_nx && y>=1 && rout.gridded_cells[x+1][y-1]!=NULL){
                rout.gridded_cells[x][y]->downstream=rout.gridded_cells[x+1][y-1];
            }
        }else if(direction[i]==5){
            if(y>=1 && rout.gridded_cells[x][y-1]!=NULL){
                rout.gridded_cells[x][y]->downstream=rout.gridded_cells[x][y-1];
            }
        }else if(direction[i]==6){
            if(y>=1 && x>=1 && rout.gridded_cells[x-1][y-1]!=NULL){
                rout.gridded_cells[x][y]->downstream=rout.gridded_cells[x-1][y-1];
            }
        }else if(direction[i]==7){
            if(x>=1 && rout.gridded_cells[x-1][y]!=NULL){
                rout.gridded_cells[x][y]->downstream=rout.gridded_cells[x-1][y];
            }
        }else if(direction[i]==8){
            if(x>=1 && y+1<global_domain.n_ny && rout.gridded_cells[x-1][y+1]!=NULL){
                rout.gridded_cells[x][y]->downstream=rout.gridded_cells[x-1][y+1];
            }
        }
    }
    
    /*
     *Set upstream cells based on downstream cells
     *Go through each neighbor of the current cell
     *If the neighbors' downstream cell is the current cell
     *Set the neighbor as upstream from the current cell.
     */
    
    for(i=0;i<global_domain.ncells_active;i++){
        x=rout.cells[i].x;
        y=rout.cells[i].y;
        
        if(y+1<global_domain.n_ny && rout.gridded_cells[x][y+1]!=NULL){
            if(rout.gridded_cells[x][y+1]->downstream == rout.gridded_cells[x][y]){
                upstream_temp[rout.gridded_cells[x][y]->nr_upstream]=rout.gridded_cells[x][y+1];
                rout.gridded_cells[x][y]->nr_upstream++;
            }
        }
        if(x+1<global_domain.n_nx && y+1<global_domain.n_ny && rout.gridded_cells[x+1][y+1]!=NULL){
            if(rout.gridded_cells[x+1][y+1]->downstream == rout.gridded_cells[x][y]){
                upstream_temp[rout.gridded_cells[x][y]->nr_upstream]=rout.gridded_cells[x+1][y+1];
                rout.gridded_cells[x][y]->nr_upstream++;
            }
        }
        if(x+1<global_domain.n_nx && rout.gridded_cells[x+1][y]!=NULL){
            if(rout.gridded_cells[x+1][y]->downstream == rout.gridded_cells[x][y]){
                upstream_temp[rout.gridded_cells[x][y]->nr_upstream]=rout.gridded_cells[x+1][y];
                rout.gridded_cells[x][y]->nr_upstream++;
            }
        }
        if(x+1<global_domain.n_nx && y>=1 && rout.gridded_cells[x+1][y-1]!=NULL){
            if(rout.gridded_cells[x+1][y-1]->downstream == rout.gridded_cells[x][y]){
                upstream_temp[rout.gridded_cells[x][y]->nr_upstream]=rout.gridded_cells[x+1][y-1];
                rout.gridded_cells[x][y]->nr_upstream++;
            }
        }
        if(y>=1 && rout.gridded_cells[x][y-1]!=NULL){
            if(rout.gridded_cells[x][y-1]->downstream == rout.gridded_cells[x][y]){
                upstream_temp[rout.gridded_cells[x][y]->nr_upstream]=rout.gridded_cells[x][y-1];
                rout.gridded_cells[x][y]->nr_upstream++;
            }
        }
        if(y>=1 && x>=1 && rout.gridded_cells[x-1][y-1]!=NULL){
            if(rout.gridded_cells[x-1][y-1]->downstream == rout.gridded_cells[x][y]){
                upstream_temp[rout.gridded_cells[x][y]->nr_upstream]=rout.gridded_cells[x-1][y-1];
                rout.gridded_cells[x][y]->nr_upstream++;
            }
        }
        if(x>=1 && rout.gridded_cells[x-1][y]!=NULL){
            if(rout.gridded_cells[x-1][y]->downstream == rout.gridded_cells[x][y]){
                upstream_temp[rout.gridded_cells[x][y]->nr_upstream]=rout.gridded_cells[x-1][y];
                rout.gridded_cells[x][y]->nr_upstream++;
            }
        }
        if(x>=1 && y+1<global_domain.n_ny && rout.gridded_cells[x-1][y+1]!=NULL){
            if(rout.gridded_cells[x-1][y+1]->downstream == rout.gridded_cells[x][y]){
                upstream_temp[rout.gridded_cells[x][y]->nr_upstream]=rout.gridded_cells[x-1][y+1];
                rout.gridded_cells[x][y]->nr_upstream++;
            }
        }
        
        //Allocate upstream cells based on number of upstream cells
        rout.cells[i].upstream = malloc(rout.cells[i].nr_upstream * sizeof(*rout.cells[i].upstream));
        check_alloc_status(rout.cells[i].upstream,"Memory allocation error.");
        
        for(j=0;j<rout.cells[i].nr_upstream;j++){
            rout.cells[i].upstream[j]=upstream_temp[j];
        }
    }
    
    free(upstream_temp);
    free(direction);
}

void sort_cells(void){
    /* The function sorts cells from upstream to downstream.
     * 
     * It does so by finding the number of upstream cells per cell,
     * and if the number of upstream cells is 0 it will rank the cell.
     * After a cell is ranked it is excluded in the following search
     * for upstream cells. The ranked cells are stored in order in 
     * rout.sorted_cells for use in rout_run().
     */
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    bool *done_map;
    
    size_t i;
    size_t rank=0;
    size_t j;
    
    done_map = malloc(global_domain.ncells_active * sizeof(*done_map));
    check_alloc_status(done_map,"Memory allocation error.");
    
    for(i=0;i<global_domain.ncells_active;i++){
        done_map[i]=false;
    }
    
    while(true){
        for(i=0;i<global_domain.ncells_active;i++){
            if(!done_map[i]){
                //If current cell has not been done
                
                int count=0;
                for(j=0;j<rout.cells[i].nr_upstream;j++){
                    if(!done_map[rout.cells[i].upstream[j]->id]){
                        //If upstream cell has not been done
                        
                        count++;
                    }
                }

                if(count==0){
                    //Meaning there are no (not-done) cells upstream of this cell
                    
                    rout.sorted_cells[rank]=&rout.cells[i];
                    rank++;
                }  
            }
        }
        
        for(i=0;i<rank;i++){
            done_map[rout.sorted_cells[i]->id]=true;
        }
        
        if(rank == global_domain.ncells_active){
            //all cells have been ranked so break out of the while loop
            break;
        }else if(rank > global_domain.ncells_active){
            log_err("rank_cells made %zu loops and escaped because this is more than %zu, the number of active cells",(rank+1),global_domain.ncells_active);
            break;
        }
    }
    
    free(done_map);
}

void set_uh(char file_path[], char variable_name[]){
    /* The function calculates the unit hydrograph per timestep per cell
     * 
     * It does so by using the specified velocity, diffusivity and flow
     * distance per cell in the equations of Lohmann et al. (1998).
     * First a precise unit hydrograph is calculated (more timesteps),
     * then it is normalized and summed over the actual model timestep.
     * This ends up in a unit hydrograph with information about which fraction
     * of runoff to release per timestep.
     */
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern rout_struct rout;
    
    float *velocity;
    float *diffusivity;
    double *distance;
    double *uh_precise;
    double *uh_cumulative;
    double uh_sum;
    
    size_t i;
    size_t j=0;
    
    distance = malloc(global_domain.ncells_active * sizeof(*distance));
    check_alloc_status(distance,"Memory allocation error.");
    
    velocity = malloc(global_domain.ncells_active * sizeof(*velocity));
    check_alloc_status(velocity,"Memory allocation error.");
    
    diffusivity = malloc(global_domain.ncells_active * sizeof(*diffusivity));
    check_alloc_status(diffusivity,"Memory allocation error.");
    
    uh_precise = malloc((rout.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_precise));
    check_alloc_status(uh_precise,"Memory allocation error.");
    
    uh_cumulative = malloc((rout.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_cumulative));
    check_alloc_status(uh_cumulative,"Memory allocation error.");
    
    //Get data from NETcdf files (or input)
    size_t start[]={0, 0};
    size_t count[]={global_domain.n_ny, global_domain.n_nx};
    get_scatter_nc_field_double(file_path,variable_name,start,count,distance);
    
    if(rout.fuh_file){
        get_scatter_nc_field_float(file_path,"flow_velocity",start,count,velocity);
        get_scatter_nc_field_float(file_path,"flow_diffusivity",start,count,diffusivity);
    }else{
        for(i=0;i<global_domain.ncells_active;i++){
            velocity[i] = rout.flow_velocity_uh;
            diffusivity[i] = rout.flow_diffusivity_uh;
        }
    }
    
    
    //Calculate unit hydrograph
    for (i=0;i<global_domain.ncells_active;i++){
        if(distance[i]!=-1){
            size_t time=0;
            uh_sum=0.0;
            
            //Calculate precise unit hydrograph based on timestep
            for(j=0;j< rout.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
                time += (SEC_PER_HOUR * HOURS_PER_DAY) / (global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP);
                uh_precise[j]=(distance[i]/(2 * time * sqrt(M_PI * time * diffusivity[i])))
                        * exp(-(pow(velocity[i] * time - distance[i],2)) / (4 * diffusivity[i] * time));
                uh_sum += uh_precise[j];
            }

            //Normalize unit hydrograph so sum is 1 and make cumulative unit hydrograph
            for(j=0;j< rout.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
                uh_precise[j] = uh_precise[j] / uh_sum;
                if(j>0){
                    uh_cumulative[j] = uh_cumulative [j-1] + uh_precise[j];
                    if(uh_cumulative[j]>1){
                        uh_cumulative[j]=1.0;
                    }
                }else{
                    uh_cumulative[j] = uh_precise[j];
                }
            }
            
            //Make final time-step unit hydrograph based on cumulative unity hydrograph
            for(j=0;j< rout.max_days_uh * global_param.model_steps_per_day;j++){
                if(j<(rout.max_days_uh * global_param.model_steps_per_day)- 1){
                    rout.cells[i].uh[j]=uh_cumulative[(j+1) * UH_STEPS_PER_TIMESTEP] - uh_cumulative[j * UH_STEPS_PER_TIMESTEP];
                }else{
                    rout.cells[i].uh[j]=uh_cumulative[((j+1) * UH_STEPS_PER_TIMESTEP)-1] - uh_cumulative[j * UH_STEPS_PER_TIMESTEP];
                }
            }
        }
    }
    
    free(distance);
    free(velocity);
    free(diffusivity);
    free(uh_cumulative);
    free(uh_precise);
}

void set_reservoirs(){
    /* The function instantiates the reservoirs in rout.reservoirs.
     * 
     * It does so by collecting the information in the reservoir file,
     * checking whether the reservoir is within our domain- and time of
     * interest, and finally adding the reservoir to the structure.
     * All reservoir values will also be initialized
     */
    extern rout_struct rout;
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    
    reservoir_unit* reservoir_temp;
    reservoir_unit* res;
    
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
    
    if(!rout.freservoirs){
        rout.reservoirs=malloc(0);
        check_alloc_status(rout.reservoirs,"Memory allocation error.");
        return;
    }
    
    reservoir_temp = malloc(MAX_NR_RESERVOIRS * sizeof(*reservoir_temp));
    check_alloc_status(reservoir_temp,"Memory allocation error.");
    
    //Read from reservoir file
    rf=open_file(rout.reservoir_filename,"r");
    fgets(cmdstr, MAXSTRING, rf);
    
    while (!feof(rf)) {        
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0') {
            sscanf(cmdstr, "%s", optstr);
            
            if (optstr[0] == '#') {
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            if(rout.nr_reservoirs>=MAX_NR_RESERVOIRS){
                log_warn("there are more reservoirs in the area than is allowed, skipping other reservoirs");
            }
                        
            res = &reservoir_temp[rout.nr_reservoirs];
            sscanf(cmdstr, "%zu %200[^\t] %d %lf %s %f %f",&res->global_id, res->name,&res->activation_year,&res->storage_capacity,purpose,&lon,&lat);
            
            if(lon ==RES_NO_DATA || lat==RES_NO_DATA){
                
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
            }else if(res->storage_capacity==RES_NO_DATA){
                
                log_warn("Capacity data of reservoir %s (file_id %zu) not present, skipping reservoir",res->name,res->global_id);
                //reservoir data not present
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }else if(res->activation_year==RES_NO_DATA || res->activation_year > global_param.endyear){
                
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
            
                res->storage_capacity *= (pow(M_PER_KM,3));
                if(res->activation_year==RES_NO_DATA){
                    res->activation_year=global_param.startyear-1;
                }
                
                res->cell = rout.gridded_cells[x][y]; 
                
                if(purpose[0]=='I'){
                    res->function=RES_IRR_FUNCTION;
                }else if(purpose[0]=='H'){
                    res->function=RES_HYD_FUNCTION;
                }else if(purpose[0]=='C'){
                    res->function=RES_CON_FUNCTION;
                }else{
                    log_warn("Purpose of reservoir %s (file_id %zu) not properly defined; default to irrigation",res->name,res->global_id);
                    res->function=RES_IRR_FUNCTION;                    
                }
                
                rout.nr_reservoirs++;
            }

        }
        fgets(cmdstr, MAXSTRING, rf);
    }
    
    //Allocating reservoirs based on number of reservoirs found
    rout.reservoirs = malloc(rout.nr_reservoirs * sizeof(*rout.reservoirs));
    check_alloc_status(rout.reservoirs,"Memory allocation error.");
    
    
    for(k=0;k<rout.nr_reservoirs;k++){
        
        //Handle reservoirs in the same cell, they will be combined. 
        //Problem is if the construction years or function are different.
        //Currently function will default to irrigation 
        //and earliest year will be chosen.
        if(reservoir_temp[k].cell->reservoir!=NULL){
            reservoir_temp[k].cell->reservoir->storage_capacity += reservoir_temp[k].storage_capacity;
            
            if(reservoir_temp[k].cell->reservoir->function!=RES_IRR_FUNCTION && reservoir_temp[k].function==RES_IRR_FUNCTION){
                reservoir_temp[k].cell->reservoir->function=RES_IRR_FUNCTION;
            }
            
            if(reservoir_temp[k].cell->reservoir->activation_year > reservoir_temp[k].activation_year){
                reservoir_temp[k].cell->reservoir->activation_year = reservoir_temp[k].activation_year;
            }
            
            log_warn("Reservoir %s and %s have been combined into reservoir %s because they are in the same cell."
                    " Capacities have been added. If any of the reservoirs had an irrigation function %s has an irrigation function."
                    " If reservoirs had different activation years the earliest activation year will be used for %s in the simulation.",
                    reservoir_temp[k].cell->reservoir->name,reservoir_temp[k].name,reservoir_temp[k].cell->reservoir->name,
                    reservoir_temp[k].cell->reservoir->name,
                    reservoir_temp[k].cell->reservoir->name);
            continue;
        }
        
        rout.reservoirs[i]=reservoir_temp[k];
        
        rout.reservoirs[i].id = i;       
        rout.reservoirs[i].run=false;
        rout.reservoirs[i].cell->reservoir=&rout.reservoirs[i];
        
        //Allocate all reservoir values
        rout.reservoirs[i].demand = malloc(RES_CALC_YEARS_MEAN * sizeof(*rout.reservoirs[i].demand));
        check_alloc_status(rout.reservoirs[i].demand,"Memory allocation error.");
        
        for(j=0;j<RES_CALC_YEARS_MEAN;j++){
            rout.reservoirs[i].demand[j] = malloc(MONTHS_PER_YEAR * sizeof(*rout.reservoirs[i].demand[j]));  
            check_alloc_status(rout.reservoirs[i].demand[j],"Memory allocation error.");                  
        }
        
        rout.reservoirs[i].inflow = malloc(RES_CALC_YEARS_MEAN * sizeof(*rout.reservoirs[i].inflow));
        check_alloc_status(rout.reservoirs[i].inflow,"Memory allocation error.");  
        
        for(j=0;j<RES_CALC_YEARS_MEAN;j++){
            rout.reservoirs[i].inflow[j] = malloc(MONTHS_PER_YEAR * sizeof(*rout.reservoirs[i].inflow[j])); 
            check_alloc_status(rout.reservoirs[i].inflow[j],"Memory allocation error.");              
        }
        
        //Initialize all reservoir values
        for(y=0;y<RES_CALC_YEARS_MEAN;y++){
            for(m=0;m<MONTHS_PER_YEAR;m++){
                rout.reservoirs[i].inflow[y][m] = 0.0;
                rout.reservoirs[i].demand[y][m] = 0.0;
                if(rout.fnaturalized_flow){
                    rout.reservoirs[i].inflow_natural[y][m]=0.0;
                }
                
            }
        }
        
        rout.reservoirs[i].nr_serviced_cells = 0;
        rout.reservoirs[i].current_demand = 0.0;
        rout.reservoirs[i].current_inflow = 0.0;
        rout.reservoirs[i].current_inflow_natural = 0.0;
        
        rout.reservoirs[i].target_release = 0.0;
        
        rout.reservoirs[i].monthly_demand=0.0;
        rout.reservoirs[i].monthly_inflow=0.0;
        rout.reservoirs[i].monthly_inflow_natural=0.0;
        rout.reservoirs[i].annual_demand=0.0;
        rout.reservoirs[i].annual_inflow=0.0;
        rout.reservoirs[i].annual_inflow_natural=0.0;
        
        rout.reservoirs[i].current_storage= rout.reservoirs[i].storage_capacity * RES_PREF_STORAGE; //preferred storage level for the start of the operational year (Hanasaki et al., 2006)
        rout.reservoirs[i].storage_start_operation = rout.reservoirs[i].current_storage;
        
        rout.reservoirs[i].start_operation.day=global_param.startday;
        rout.reservoirs[i].start_operation.month=global_param.startmonth;
        rout.reservoirs[i].start_operation.year=global_param.startyear;
        
        i++;
            
    }
    
    rout.nr_reservoirs=i;
    
    log_info("%zu reservoirs found inside area and time of interest",rout.nr_reservoirs);
    if(rout.fdebug_mode){        
        fprintf(LOG_DEST, "Current Routing Reservoirs\n");
        for(i=0;i<rout.nr_reservoirs;i++){
            fprintf(LOG_DEST, "RESERVOIR %zu\t%s\t(file-id %zu):\tyear %d\tcapacity %.1f\tpurpose %zu\n",
                    rout.reservoirs[i].id,rout.reservoirs[i].name,rout.reservoirs[i].global_id,rout.reservoirs[i].activation_year,
                    rout.reservoirs[i].storage_capacity,rout.reservoirs[i].function);
        }
        if(rout.nr_reservoirs==0){
            fprintf(LOG_DEST, "--No reservoirs found--\n");
        }
    }
    
    free(reservoir_temp);
}

void set_naturalized_routing(){
    /* This function determines whether natural routing is done
     * 
     * It does so by checking if reservoirs are found in series,
     * meaning one reservoir discharges into another reservoir.
     * If so the inflow of a reservoir is influenced by the outflow
     * of other reservoirs, which hinders the environmental flow
     * calculations (percentage of natural flow). Therefore the 
     * routing options are set to calculate naturalized flows
     * as well.    
     */
    extern rout_struct rout;
    
    size_t i;
    size_t j;
    rout_cell* current_cell;    
    
    if(!rout.freservoirs){
        return;
    }
    
    for(i=0;i<rout.nr_reservoirs;i++){
        current_cell = rout.reservoirs[i].cell;
        
        while(current_cell->downstream!=NULL && current_cell->downstream!=current_cell){
            //Follow the reservoir downstream 
            
            current_cell = current_cell->downstream;
            if(current_cell->reservoir!=NULL){
                log_info("Reservoirs are found in series and therefore routing is done twice. "
                        "Second routing calculates natural stream flow "
                        "which is needed for environmental flow calculation");
                rout.fnaturalized_flow=true;
                break;
            }
        }
        if(rout.fnaturalized_flow){
            break;
        }
    }
    
    if(rout.fnaturalized_flow){
        for(i=0;i<rout.nr_reservoirs;i++){
            rout.reservoirs[i].inflow_natural = malloc(RES_CALC_YEARS_MEAN * sizeof(*rout.reservoirs[i].inflow_natural));
            check_alloc_status(rout.reservoirs[i].inflow_natural,"Memory allocation error.");  

            for(j=0;j<RES_CALC_YEARS_MEAN;j++){
                rout.reservoirs[i].inflow_natural[j] = malloc(MONTHS_PER_YEAR * sizeof(*rout.reservoirs[i].inflow_natural[j])); 
                check_alloc_status(rout.reservoirs[i].inflow_natural[j],"Memory allocation error.");              
            }
        }
    }else{
        for(i=0;i<rout.nr_reservoirs;i++){
            rout.reservoirs[i].inflow_natural = malloc(RES_CALC_YEARS_MEAN * sizeof(*rout.reservoirs[i].inflow_natural));
            check_alloc_status(rout.reservoirs[i].inflow_natural,"Memory allocation error.");  

            for(j=0;j<RES_CALC_YEARS_MEAN;j++){
                rout.reservoirs[i].inflow_natural[j] = malloc(0); 
                check_alloc_status(rout.reservoirs[i].inflow_natural[j],"Memory allocation error.");              
            }
        }
    }
}
    
void set_reservoir_service(){
    /* The function determines the cells each reservoir supplies with water
     * 
     * It does so by finding each irrigated cell within the irrigation
     * distance and below the reservoir elevation.
     * Cells also get a pointer to their servicing reservoir
     * which is calculated later.     
     */
    extern rout_struct rout;
    extern soil_con_struct *soil_con;
    extern domain_struct global_domain;
    extern rout_struct rout;
      
    size_t nr_irrigated_cells=0;
    rout_cell **irrigated_cells_temp;
    rout_cell*** service_cell_temp;
        
    size_t i;
    size_t j;
    size_t k;
    
    if(!rout.freservoirs){
        return;
    }
    
    if(!rout.firrigation){
        for(i=0;i<rout.nr_reservoirs;i++){
            rout.reservoirs[i].serviced_cells=malloc(0);
            check_alloc_status(rout.reservoirs[i].serviced_cells,"Memory allocation error.");
            rout.reservoirs[i].cell_demand=malloc(0);
            check_alloc_status(rout.reservoirs[i].cell_demand,"Memory allocation error.");
            rout.reservoirs[i].prev_deficit=malloc(0);
            check_alloc_status(rout.reservoirs[i].prev_deficit,"Memory allocation error.");
            
        }
    }
    
    irrigated_cells_temp = malloc(global_domain.ncells_active * sizeof(*irrigated_cells_temp));
    check_alloc_status(irrigated_cells_temp,"Memory allocation error");
    
    //Find irrigated cells
    for(i=0;i<global_domain.ncells_active;i++){
        if(rout.cells[i].irrigate){
            irrigated_cells_temp[nr_irrigated_cells]=&rout.cells[i];
            nr_irrigated_cells++;
        }
    }
    
    service_cell_temp = malloc(rout.nr_reservoirs * sizeof(*service_cell_temp));
    check_alloc_status(service_cell_temp,"Memory allocation error.");
    
    for(i=0;i<rout.nr_reservoirs;i++){
        service_cell_temp[i] = malloc(RES_MAX_SERVICE * sizeof(*service_cell_temp[i]));
        check_alloc_status(service_cell_temp[i],"Memory allocation error.");
    }

    for(i=0;i<rout.nr_reservoirs;i++){        
        if(rout.reservoirs[i].function!=RES_IRR_FUNCTION){
            continue;
        }
        
        if(rout.reservoirs[i].nr_serviced_cells >= RES_MAX_SERVICE){
            log_warn("a cell is not added to reservoir %zu because of maximum service capacity in reservoir",rout.reservoirs[i].id);
            continue;
        }
        
        for(j=0;j<nr_irrigated_cells;j++){
             
             if(irrigated_cells_temp[j]->nr_servicing_reservoirs >= CELL_MAX_SERVICE){
                log_warn("a reservoir is not added to cell %zu because of maximum servicing reservoirs in cell",irrigated_cells_temp[j]->id);
                continue;
            }
             
            if(soil_con[irrigated_cells_temp[j]->id].elevation >= soil_con[rout.reservoirs[i].cell->id].elevation){
                continue;
            }

            if(distance(irrigated_cells_temp[j]->x,irrigated_cells_temp[j]->y,
                    rout.reservoirs[i].cell->x,rout.reservoirs[i].cell->y)
                    >rout.max_distance_irr){
                continue;
            }
             
            service_cell_temp[i][rout.reservoirs[i].nr_serviced_cells]=irrigated_cells_temp[j];
            
            rout.reservoirs[i].nr_serviced_cells++;
            irrigated_cells_temp[j]->nr_servicing_reservoirs++;
        }
    }
    
    for(i=0;i<global_domain.ncells_active;i++){
        //Allocate cell values based on number of servicing reservoirs
        
        rout.cells[i].servicing_reservoirs = malloc(rout.cells[i].nr_servicing_reservoirs * sizeof(*rout.cells[i].servicing_reservoirs));
        check_alloc_status(rout.cells[i].servicing_reservoirs,"Memory allocation error.");
        
        for(j=0;j<rout.cells[i].nr_servicing_reservoirs;j++){
            rout.cells[i].servicing_reservoirs[j]=NULL;
        }
    }
    
    for(i=0;i<rout.nr_reservoirs;i++){
        //Allocate reservoir values based on number of serviced cells
        
        rout.reservoirs[i].serviced_cells = malloc(rout.reservoirs[i].nr_serviced_cells * sizeof(*rout.reservoirs[i].serviced_cells));
        check_alloc_status(rout.reservoirs[i].serviced_cells,"Memory allocation error.");
        
        rout.reservoirs[i].cell_demand = malloc(rout.reservoirs[i].nr_serviced_cells * sizeof(*rout.reservoirs[i].cell_demand));
        check_alloc_status(rout.reservoirs[i].cell_demand,"Memory allocation error.");
        
        for(j=0;j<rout.reservoirs[i].nr_serviced_cells;j++){
            rout.reservoirs[i].cell_demand[j] = malloc(service_cell_temp[i][j]->nr_crops * sizeof(*rout.reservoirs[i].cell_demand[j]));
            check_alloc_status(rout.reservoirs[i].cell_demand[j],"Memory allocation error.");
        }
        
        rout.reservoirs[i].prev_deficit = malloc(rout.reservoirs[i].nr_serviced_cells * sizeof(*rout.reservoirs[i].prev_deficit));
        check_alloc_status(rout.reservoirs[i].prev_deficit,"Memory allocation error.");
        
        for(j=0;j<rout.reservoirs[i].nr_serviced_cells;j++){
            rout.reservoirs[i].prev_deficit[j] = malloc(service_cell_temp[i][j]->nr_crops * sizeof(*rout.reservoirs[i].prev_deficit[j]));
            check_alloc_status(rout.reservoirs[i].prev_deficit[j],"Memory allocation error.");
        }
        
        for(j=0;j<rout.reservoirs[i].nr_serviced_cells;j++){
            rout.reservoirs[i].serviced_cells[j]=service_cell_temp[i][j];
            
            for(k=0;k<rout.reservoirs[i].serviced_cells[j]->nr_crops;k++){
                rout.reservoirs[i].cell_demand[j][k]=0.0;
            }
            
            for(k=0;k<service_cell_temp[i][j]->nr_crops;k++){
                rout.reservoirs[i].prev_deficit[j][k]=0.0;
            }
            
            for(k=0;k<rout.reservoirs[i].serviced_cells[j]->nr_servicing_reservoirs;k++){
                if(rout.reservoirs[i].serviced_cells[j]->servicing_reservoirs[k]==NULL){
                    //Add reservoirs to cells
                    
                    rout.reservoirs[i].serviced_cells[j]->servicing_reservoirs[k] = &rout.reservoirs[i];
                    break;
                }
            }
        }
    }
            
    for(i=0;i<rout.nr_reservoirs;i++){
       free(service_cell_temp[i]);
    }
    free(service_cell_temp);
    free(irrigated_cells_temp);
}
