/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>
#include <netcdf.h>
#include <rout.h>
#include <math.h>
#include <string.h>
#define M_PI 3.14159265358979323846

void rout_init(void){
    extern rout_struct rout;
    
    //Set cell values based on location and vegetation
    set_cells();
    
    //Set cell crop struct based on irrigated vegetation
    set_cell_irrigate();
    
    //Set cell upstream and downstream cells based on flow direction
    set_upstream_downstream(rout.param_filename,"flow_direction");
    
    //Sort cells upstream to downstream
    sort_cells();
    
    //Set cell unit hydrograph based of flow distance
    set_uh(rout.param_filename,"flow_distance");
    
    //Set reservoir information
    set_reservoirs();
    
    set_naturalized_routing();
    
    //Set reservoir irrigation services
    set_reservoir_service();

    if(rout.fdebug_mode){
        //Make routing debug files
        
        log_info("Making routing debug files...");
        make_location_file(rout.debug_path,"location");
        log_info("Finished cell location file");
        make_nr_upstream_file(rout.debug_path,"nr_upstream");
        log_info("Finished number upstream file");
        make_uh_file(rout.debug_path,"uh");
        log_info("Finished unit hydrograph file");
        make_ranked_cells_file(rout.debug_path,"ranked_cells");
        log_info("Finished cell rank file");
        
        if(rout.reservoirs){
            make_reservoir_file(rout.debug_path,"reservoir");
            log_info("Finished reservoir file");
            make_reservoir_service_file(rout.debug_path,"reservoir_service");
            log_info("Finished reservoir service file");
            make_nr_reservoir_service_file(rout.debug_path,"nr_reservoir_service");
            log_info("Finished number of reservoir service file");
            make_nr_crops_file(rout.debug_path,"nr_crops");
            log_info("Finished number of crops file");
        }
    }
    
}

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
    extern domain_struct global_domain;
    extern veg_con_map_struct *veg_con_map;
    extern rout_struct rout;
    
    size_t i;
    size_t j;
    
    for(i=0;i<global_domain.ncells_active;i++){
        rout.cells[i].irrigate=false;
        rout.cells[i].nr_crop_class=0;
        
        for(j=0;j<rout.nr_crop_classes;j++){
            if(veg_con_map[i].vidx[rout.crop_class[j]]!=NODATA_VEG){
                rout.cells[i].irrigate=true;
                rout.cells[i].nr_crop_class++;
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
    
    if((direction = malloc(global_domain.ncells_active * sizeof(*direction)))==NULL){
        log_err("Memory allocation error!");
    }
    
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
    
    free(direction);
    
    if((upstream_temp = malloc(8 * sizeof(*upstream_temp)))==NULL){
        log_err("Memory allocation error!");
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
        if((rout.cells[i].upstream = malloc(rout.cells[i].nr_upstream * sizeof(*rout.cells[i].upstream)))==NULL){
            log_err("Memory allocation for rout.cells[i].upstream failed!");
        }
        
        for(j=0;j<rout.cells[i].nr_upstream;j++){
            rout.cells[i].upstream[j]=upstream_temp[j];
        }
    }
    
    free(upstream_temp);
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
    
    if((done_map = malloc(global_domain.ncells_active * sizeof(*done_map)))==NULL){
        log_err("Memory allocation error!");
    }
    
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
            log_warn("rank_cells made %zu loops and escaped because this is more than %zu, the number of active cells",(rank+1),global_domain.ncells_active);
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
    
    if((distance = malloc(global_domain.ncells_active * sizeof(*distance)))==NULL){
        log_err("Memory allocation for <set_uh> distance failed!");
    }
    if((velocity = malloc(global_domain.ncells_active * sizeof(*velocity)))==NULL){
        log_err("Memory allocation error!");
    }
    if((diffusivity = malloc(global_domain.ncells_active * sizeof(*diffusivity)))==NULL){
        log_err("Memory allocation error!");
    }
    
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
    
    if((uh_precise = malloc((rout.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_precise)))==NULL){
        log_err("Memory allocation for <set_uh> uh_precise failed!");
    }
    if((uh_cumulative = malloc((rout.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_cumulative)))==NULL){
        log_err("Memory allocation for <set_uh> uh_cumulative failed!");
    }
    
    //Calculate unit hydrograph
    for (i=0;i<global_domain.ncells_active;i++){
        if(distance[i]!=-1){
            size_t time=0;
            uh_sum=0.0;

            //Calculate precise unit hydrograph based on timestep
            for(j=0;j< rout.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
                time += (3600 * 24) / (global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP);
                uh_precise[j]=(distance[i]/(2 * time * sqrt(M_PI * time * rout.flow_diffusivity_uh)))
                        * exp(-(pow(rout.flow_velocity_uh * time - distance[i],2)) / (4 * rout.flow_diffusivity_uh * time));
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
    
    if((reservoir_temp = malloc(MAX_NR_RESERVOIRS * sizeof(*reservoir_temp)))==NULL){
        log_err("Memory allocation error!");
    }
    
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
                log_info("there are more reservoirs in the area than is allowed, skipping other reservoirs");
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
                
                log_info("Capacity data of reservoir %s (file_id %zu) not present",res->name,res->global_id);
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
            
                res->storage_capacity *= 1000000;
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
                    log_info("Purpose of reservoir %s (file_id %zu) not properly defined; default to irrigation",res->name,res->global_id);
                    res->function=RES_IRR_FUNCTION;                    
                }
                
                rout.nr_reservoirs++;
            }

        }
        fgets(cmdstr, MAXSTRING, rf);
    }
    
    //Allocating reservoirs based on number of reservoirs found
    if((rout.reservoirs = malloc(rout.nr_reservoirs * sizeof(*rout.reservoirs)))==NULL){
        log_err("Memory allocation error!");
    }
    
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
            
            log_info("Reservoir %s and %s have been combined into reservoir %s because they are in the same cell."
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
        if((rout.reservoirs[i].demand = malloc(RES_CALC_YEARS_MEAN * sizeof(*rout.reservoirs[i].demand)))!=NULL){
            for(j=0;j<RES_CALC_YEARS_MEAN;j++){
                if((rout.reservoirs[i].demand[j] = malloc(MONTHS_PER_YEAR * sizeof(*rout.reservoirs[i].demand[j])))==NULL){
                    log_err("Memory allocation error!");                       
                }
            }
        }else{
            log_err("Memory allocation error!");     
        }
        if((rout.reservoirs[i].inflow = malloc(RES_CALC_YEARS_MEAN * sizeof(*rout.reservoirs[i].inflow)))!=NULL){
            for(j=0;j<RES_CALC_YEARS_MEAN;j++){
                if((rout.reservoirs[i].inflow[j] = malloc(MONTHS_PER_YEAR * sizeof(*rout.reservoirs[i].inflow[j])))==NULL){
                    log_err("Memory allocation error!");                       
                }
            }
        }else{
            log_err("Memory allocation error!");     
        }
        
        if(rout.naturalized_flow){
            if((rout.reservoirs[i].inflow_natural = malloc(RES_CALC_YEARS_MEAN * sizeof(*rout.reservoirs[i].inflow_natural)))!=NULL){
                for(j=0;j<RES_CALC_YEARS_MEAN;j++){
                    if((rout.reservoirs[i].inflow_natural[j] = malloc(MONTHS_PER_YEAR * sizeof(*rout.reservoirs[i].inflow_natural[j])))==NULL){
                        log_err("Memory allocation error!");                       
                    }
                }
            }else{
                log_err("Memory allocation error!");     
            }
        }else{
            if((rout.reservoirs[i].inflow_natural = malloc(RES_CALC_YEARS_MEAN * sizeof(*rout.reservoirs[i].inflow_natural)))!=NULL){
                for(j=0;j<RES_CALC_YEARS_MEAN;j++){
                    if((rout.reservoirs[i].inflow_natural[j] = malloc(0))==NULL){
                        log_err("Memory allocation error!");                       
                    }
                }
            }else{
                log_err("Memory allocation error!");     
            }
        }
        
        //Initialize all reservoir values
        for(y=0;y<RES_CALC_YEARS_MEAN;y++){
            for(m=0;m<MONTHS_PER_YEAR;m++){
                rout.reservoirs[i].inflow[y][m] = 0.0;
                rout.reservoirs[i].demand[y][m] = 0.0;
                if(rout.naturalized_flow){
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
     * It does so by checking if reservoirs are found in serie,
     * meaning one reservoir discharges into another reservoir.
     * If so the inflow of a reservoir is influenced by the outflow
     * of other reservoirs, which hinders the environmental flow
     * calculations (percentage of natural flow). Therefore the 
     * routing options are set to calculate naturalized flows
     * as well.    
     */
    extern rout_struct rout;
    extern rout_struct rout;
    
    size_t i;
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
                log_info("Reservoirs are found in serie and therefore routing is done twice. "
                        "Second routing calculates natural streamflow "
                        "which is needed for environmental flow calculation");
                if(rout.fdebug_mode){
                    fprintf(LOG_DEST, "Reservoir %s is in serie with reservoir %s\n",
                            rout.reservoirs[i].name,current_cell->reservoir->name);
                }
                rout.naturalized_flow=true;
                return;
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
            rout.reservoirs[i].prev_soil_moisture=malloc(0);
            check_alloc_status(rout.reservoirs[i].prev_soil_moisture,"Memory allocation error.");
            
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

            if(distance(irrigated_cells_temp[j],rout.reservoirs[i].cell)>rout.max_distance_irr){
                continue;
            }
             
            service_cell_temp[i][rout.reservoirs[i].nr_serviced_cells]=irrigated_cells_temp[j];
            
            rout.reservoirs[i].nr_serviced_cells++;
            irrigated_cells_temp[j]->nr_servicing_reservoirs++;
        }
    }
    
    for(i=0;i<global_domain.ncells_active;i++){
        //Allocate cell values based on number of servicing reservoirs
        
        if((rout.cells[i].servicing_reservoirs = malloc(rout.cells[i].nr_servicing_reservoirs * sizeof(*rout.cells[i].servicing_reservoirs)))==NULL){
            log_err("Memory allocation error!");            
        }
        
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
            rout.reservoirs[i].cell_demand[j] = malloc(service_cell_temp[i][j]->nr_crop_class * sizeof(*rout.reservoirs[i].cell_demand[j]));
            check_alloc_status(rout.reservoirs[i].cell_demand[j],"Memory allocation error.");
        }
        
        rout.reservoirs[i].prev_soil_moisture = malloc(rout.reservoirs[i].nr_serviced_cells * sizeof(*rout.reservoirs[i].prev_soil_moisture));
        check_alloc_status(rout.reservoirs[i].prev_soil_moisture,"Memory allocation error.");
        
        for(j=0;j<rout.reservoirs[i].nr_serviced_cells;j++){
            rout.reservoirs[i].prev_soil_moisture[j] = malloc(service_cell_temp[i][j]->nr_crop_class * sizeof(*rout.reservoirs[i].prev_soil_moisture[j]));
            check_alloc_status(rout.reservoirs[i].prev_soil_moisture[j],"Memory allocation error.");
        }
        
        for(j=0;j<rout.reservoirs[i].nr_serviced_cells;j++){
            rout.reservoirs[i].serviced_cells[j]=service_cell_temp[i][j];
            
            for(k=0;k<rout.reservoirs[i].serviced_cells[j]->nr_crop_class;k++){
                rout.reservoirs[i].cell_demand[j][k]=0.0;
            }
            
            for(k=0;k<service_cell_temp[i][j]->nr_crop_class;k++){
                rout.reservoirs[i].prev_soil_moisture[j][k]=0.0;
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

double distance(rout_cell* from, rout_cell* to){
    //Calculate the euclidian distance from one cell to another
    double a = (double)from->x - (double)to->x;
    double b = (double)from->y - (double)to->y;
    return(sqrt(pow(a,2)+pow(b,2)));
}

void make_location_file(char file_path[], char file_name[]){
    //makes a file with the VIC id of every cell in a grid
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu",rout.gridded_cells[x][y-1]->id);
                    if(rout.gridded_cells[x][y-1]->id < 10){
                        fprintf(file,"   ;");
                    }else if(rout.gridded_cells[x][y-1]->id < 100){
                        fprintf(file,"  ;");
                    }else if(rout.gridded_cells[x][y-1]->id < 1000){
                        fprintf(file," ;");
                    }else{
                        fprintf(file,";");
                    }
                }else{
                    fprintf(file,"    ;");                    
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_nr_upstream_file(char file_path[], char file_name[]){
    //makes a file with the number of upstream cells of every cell in a grid
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu;",rout.gridded_cells[x][y-1]->nr_upstream);
                }else{
                    fprintf(file," ;");                    
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_ranked_cells_file(char file_path[], char file_name[]){
    //makes a file with the rank of every cell in a grid
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    size_t **gridded_ranks_temp;
    size_t x;
    size_t y;
    if((gridded_ranks_temp=malloc(global_domain.n_nx * sizeof(*gridded_ranks_temp)))!=NULL){
        for(x=0;x<global_domain.n_nx;x++){
             if((gridded_ranks_temp[x]=malloc(global_domain.n_ny * sizeof(*gridded_ranks_temp[x])))==NULL){
                 log_err("Memory allocation error!");
             }
        }
    }else{
        log_err("Memory allocation error!");
    }
    
    for(x=0;x<global_domain.ncells_active;x++){
        gridded_ranks_temp[rout.sorted_cells[x]->x][rout.sorted_cells[x]->y]=x;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    fprintf(file,"%zu",gridded_ranks_temp[x][y-1]);
                    if(gridded_ranks_temp[x][y-1] < 10){
                        fprintf(file,"  ;");
                    }else if(gridded_ranks_temp[x][y-1] < 100){
                        fprintf(file," ;");
                    }else{
                        fprintf(file,";");
                    }
                }else{
                    fprintf(file,"   ;");                    
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
        
    for(x=0;x<global_domain.n_nx;x++){
        free(gridded_ranks_temp[x]);
    }
    free(gridded_ranks_temp);
}

void make_uh_file(char file_path[], char file_name[]){
    //makes a file with unit hydrograph values of every cell
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern rout_struct rout;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    double sum=0.0;
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t i;
        size_t j;
        for(i=0;i<global_domain.ncells_active;i++){
            fprintf(file,"Cell = %zu -> ",rout.cells[i].id);
            for(j=0;j<rout.max_days_uh * global_param.model_steps_per_day;j++){
                fprintf(file,"%2f;",rout.cells[i].uh[j]);
                sum+=rout.cells[i].uh[j];
            }
            fprintf(file," sum = %2f\n",sum);
            sum=0.0;
        }
        fclose(file);
    }
}

void make_reservoir_file(char file_path[], char file_name[]){
    //makes a file with the reservoir id of every reservoir in a grid
    //other cells are shown as XX
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
    strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t i;
        fprintf(file,"Reservoirs found in location and time of interest:\n");
        for(i=0;i<rout.nr_reservoirs;i++){
            char purpose[MAXSTRING];
            if(rout.reservoirs[i].function==RES_IRR_FUNCTION){
                strncpy(purpose, "irrigation", 100);
            }else if(rout.reservoirs[i].function==RES_HYD_FUNCTION){
                strncpy(purpose, "hydropower", 100);
            }else if(rout.reservoirs[i].function==RES_CON_FUNCTION){
                strncpy(purpose, "flow control", 100);
            }else{
                strncpy(purpose, "unknown", 100);
            }
            fprintf(file,"Reservoir %zu %s activation year %d  storage capacity %.2f puropse %s",
                    rout.reservoirs[i].id,rout.reservoirs[i].name,rout.reservoirs[i].activation_year,
                    rout.reservoirs[i].storage_capacity,purpose);
            if(rout.reservoirs[i].function==RES_IRR_FUNCTION){
                fprintf(file," servicing cells %zu",rout.reservoirs[i].nr_serviced_cells);
            }
            fprintf(file,"\n");
        }        
        fprintf(file,"\n");
        
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL && rout.gridded_cells[x][y-1]->reservoir!=NULL){
                    fprintf(file,"%zu",rout.gridded_cells[x][y-1]->reservoir->id);
                    if(rout.gridded_cells[x][y-1]->reservoir->id < 10){
                        fprintf(file,"  ;");
                    }else if(rout.gridded_cells[x][y-1]->reservoir->id < 100){
                        fprintf(file," ;");
                    }else{
                        fprintf(file,";");
                    }
                }else{
                    if(rout.gridded_cells[x][y-1]!=NULL){
                        fprintf(file," XX;");
                    }else{
                        fprintf(file,"   ;");                          
                    }                  
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_reservoir_service_file(char file_path[], char file_name[]){
    //makes a file with the servicing reservoir id of every cell in a grid
    //other cells are shown as XX
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    
    bool done;
    size_t r;
    for(r=0;r<rout.nr_reservoirs;r++){
                       
        if(rout.reservoirs[r].function!=RES_IRR_FUNCTION){
            continue;
        }
        
        strcpy(full_path, file_path);
        strcat(full_path, file_name);
        strcat(full_path, "_");
        strcat(full_path, rout.reservoirs[r].name);
        strcat(full_path, ".txt");

        if((file = fopen(full_path, "w"))!=NULL){
            size_t x;
            size_t y;
            for(y=global_domain.n_ny;y>0;y--){
                for(x=0;x<global_domain.n_nx;x++){
                    done=false;
                    if(rout.gridded_cells[x][y-1]!=NULL){
                        
                        if(rout.gridded_cells[x][y-1]->reservoir!=NULL && rout.gridded_cells[x][y-1]->reservoir->id == r){
                            fprintf(file," OO;");
                            continue;
                        }
                        
                        size_t i;
                        for(i=0;i<rout.gridded_cells[x][y-1]->nr_servicing_reservoirs;i++){
                            
                            if(rout.gridded_cells[x][y-1]->servicing_reservoirs[i]->id==r){
                                
                                fprintf(file,"%zu",rout.gridded_cells[x][y-1]->servicing_reservoirs[i]->id);
                                if(rout.gridded_cells[x][y-1]->servicing_reservoirs[i]->id < 10){
                                    fprintf(file,"  ;");
                                }else if(rout.gridded_cells[x][y-1]->servicing_reservoirs[i]->id < 100){
                                    fprintf(file," ;");
                                }else{
                                    fprintf(file,";");
                                }
                                
                                done=true;
                                break;
                            }
                        }
                        
                        if(!done){
                            fprintf(file," XX;");
                        }
                    }else{
                        fprintf(file,"   ;");                          
                    }
                }
            fprintf(file,"\n");
            }
        fclose(file);
        }
    }
}

void make_nr_reservoir_service_file(char file_path[], char file_name[]){
    //makes a file with the number of servicing reservoirs of every cell in a grid
    //cells without irrigated vegetation are shown as XX
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
        strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    if(rout.gridded_cells[x][y-1]->irrigate){
                        fprintf(file,"%zu",rout.gridded_cells[x][y-1]->nr_servicing_reservoirs);
                        if(rout.gridded_cells[x][y-1]->nr_servicing_reservoirs < 10){
                            fprintf(file,"  ;");
                        }else if(rout.gridded_cells[x][y-1]->nr_servicing_reservoirs < 100){
                            fprintf(file," ;");
                        }else{
                            fprintf(file,";");
                        }
                    }else{
                    fprintf(file," XX;");
                    }
                }else{
                    fprintf(file,"   ;");            
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

void make_nr_crops_file(char file_path[], char file_name[]){
    //makes a file with the number of servicing reservoirs of every cell in a grid
    //cells without irrigated vegetation are shown as XX
    
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    size_t path_length = strlen(file_path);
    size_t file_length = strlen(file_name);
    if(path_length+file_length >= MAXSTRING-1){
        log_info("Debug file path and name (%zu + %zu) is too large for buffer (%d)",path_length,file_length,MAXSTRING);
        return;
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, file_path);
    strcat(full_path, file_name);
    strcat(full_path, ".txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        size_t x;
        size_t y;
        for(y=global_domain.n_ny;y>0;y--){
            for(x=0;x<global_domain.n_nx;x++){
                if(rout.gridded_cells[x][y-1]!=NULL){
                    if(rout.gridded_cells[x][y-1]->irrigate){
                        fprintf(file,"%zu",rout.gridded_cells[x][y-1]->nr_crop_class);
                        if(rout.gridded_cells[x][y-1]->nr_crop_class < 10){
                            fprintf(file,"  ;");
                        }else if(rout.gridded_cells[x][y-1]->nr_crop_class < 100){
                            fprintf(file," ;");
                        }else{
                            fprintf(file,";");
                        }
                    }else{
                    fprintf(file," XX;");
                    }
                }else{
                    fprintf(file,"   ;");            
                }
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}