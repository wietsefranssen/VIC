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
    
    set_cells();
    make_location_file(rout.debug_path,"location");
    
    set_upstream_downstream(rout.param_filename,"flow_direction");
    make_nr_upstream_file(rout.debug_path,"nr_upstream");
    
    set_uh(rout.param_filename,"flow_distance");
    make_uh_file(rout.debug_path,"uh");
    
    sort_cells();
    make_ranked_cells_file(rout.debug_path,"ranked_cells");
    
    set_reservoirs();
    make_reservoir_file(rout.debug_path,"reservoir");
    
    set_reservoir_river();
    make_reservoir_river_file(rout.debug_path,"reservoir_river");
    
    set_reservoir_service();
    make_reservoir_service_file(rout.debug_path,"reservoir_service");
    make_nr_reservoir_service_file(rout.debug_path,"nr_reservoir_service");
    
}

//connects the cells to a location
//and puts them in a grid
void set_cells(){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern veg_con_struct **veg_con;
    
    size_t i;
    size_t j=0;
    rout.min_lat=DBL_MAX;
    rout.min_lon=DBL_MAX;
        
    rout.nr_irrigated_cells=0;
    rout.nr_reservoirs=0;
    
    for(i=0;i<global_domain.ncells_total;i++){
        if(global_domain.locations[i].run){
            rout.cells[j].global_domain_id=i;
            rout.cells[j].id=j;
            rout.cells[j].location=&global_domain.locations[i];
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
        
        size_t x = (size_t)((rout.cells[i].location->longitude - rout.min_lon)/global_param.resolution);
        size_t y = (size_t)((rout.cells[i].location->latitude - rout.min_lat)/global_param.resolution);
        rout.gridded_cells[x][y]=&rout.cells[i];
        
        rout.cells[i].x=x;
        rout.cells[i].y=y;
        
        rout.cells[i].nr_servicing_reservoirs=0;
        rout.cells[i].nr_upstream=0;
        rout.cells[i].rank=0;
        
        size_t j;
        for(j=0;j<UH_MAX_DAYS * global_param.model_steps_per_day;j++){
            rout.cells[i].outflow[j]=0.0;
        }
        
        rout.cells[i].irrigate=false;
        rout.cells[i].irr_veg_nr=0;
        
        size_t iVeg;
        for(iVeg=0;iVeg<=veg_con[rout.cells[i].id][0].vegetat_type_num;iVeg++){
            if(veg_con[rout.cells[i].id][iVeg].veg_class==VEG_IRR_CLASS){
                rout.cells[i].irrigate=true;
                rout.cells[i].irr_veg_nr=iVeg;
                rout.nr_irrigated_cells++;
                break;
            }
        }
    }
    
    if((rout.irrigated_cells = malloc(rout.nr_irrigated_cells * sizeof(*rout.irrigated_cells)))==NULL){
        log_err("Memory allocation error!");     
    }
    
    j=0;
    for(i=0;i<global_domain.ncells_active;i++){
        if(rout.cells[i].irrigate){
            rout.irrigated_cells[j]=&rout.cells[i];
            j++;
        }
    }
}

void set_upstream_downstream(char file_path[], char variable_name[]){
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    int *direction;
    if((direction = malloc(global_domain.ncells_total * sizeof(*direction)))==NULL){
        log_err("Memory allocation for <set_upstream> direction failed!");
    }
    
    rout_cell** upstream_temp;
    if((upstream_temp = malloc(8 * sizeof(*upstream_temp)))==NULL){
        log_err("Memory allocation error!");
    }
    
    size_t start[]={0, 0};
    size_t count[]={global_domain.n_ny, global_domain.n_nx};
    
    get_nc_field_int(file_path,variable_name,start,count,direction);
        
    size_t i;    
    for(i=0;i<global_domain.ncells_total;i++){
        size_t x=(size_t)i%global_domain.n_nx;
        size_t y=(size_t)i/global_domain.n_nx;
        
        if(rout.gridded_cells[x][y]==NULL){
            continue;
        }
            
        if(direction[i]==-1){
            log_warn("direction of cell (global_id %zu local_id %zu) is missing, check direction file",rout.gridded_cells[x][y]->global_domain_id,rout.gridded_cells[x][y]->id);
            continue;
        }
        
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
    
    for(i=0;i<global_domain.ncells_active;i++){
        size_t x=rout.cells[i].x;
        size_t y=rout.cells[i].y;
        
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
        
        if((rout.cells[i].upstream = malloc(rout.cells[i].nr_upstream * sizeof(*rout.cells[i].upstream)))==NULL){
            log_err("Memory allocation for rout.cells[i].upstream failed!");
        }
        
        size_t j;
        for(j=0;j<rout.cells[i].nr_upstream;j++){
            rout.cells[i].upstream[j]=upstream_temp[j];
        }
    }
    
    free(direction);
    free(upstream_temp);
}

//sorts cells
void sort_cells(void){
    extern rout_struct rout;
    extern domain_struct global_domain;
    
    //used to see which cells are already sorted
    int *sorted_map;
    if((sorted_map = malloc(global_domain.ncells_active * sizeof(*sorted_map)))==NULL){
        log_err("Memory allocation for <sort_cells> sorted_map failed!");
    }
    
    size_t i;
    for(i=0;i<global_domain.ncells_active;i++){
        sorted_map[i]=0;
    }
    
    
    size_t rank=0;
    size_t j;
    while(1){
        for(i=0;i<global_domain.ncells_active;i++){
            if(sorted_map[i]==0){
                int count=0;
                for(j=0;j<rout.cells[i].nr_upstream;j++){
                    if(sorted_map[rout.cells[i].upstream[j]->id]==0){
                        count++;
                    }
                }

                if(count==0){
                    rout.sorted_cells[rank]=&rout.cells[i];
                    rout.cells[i].rank=rank;
                    rank++;
                }  
            }
        }
        
        for(i=0;i<rank;i++){
            sorted_map[rout.sorted_cells[i]->id]=1;
        }
        
        if(rank == global_domain.ncells_active){
            break;
        }else if(rank > global_domain.ncells_active){
            log_warn("rank_cells made %zu loops and escaped because this is more than %zu, the number of active cells",(rank+1),global_domain.ncells_active);
            break;
        }
    }
    
    free(sorted_map);
}

void set_uh(char file_path[], char variable_name[]){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    
    double *distance_total;
    if((distance_total = malloc(global_domain.ncells_total * sizeof(*distance_total)))==NULL){
        log_err("Memory allocation for <set_uh> distance_total failed!");
    }
    double *distance;
    if((distance = malloc(global_domain.ncells_active * sizeof(*distance)))==NULL){
        log_err("Memory allocation for <set_uh> distance failed!");
    }
    double *uh_precise;
    if((uh_precise = malloc((UH_MAX_DAYS * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_precise)))==NULL){
        log_err("Memory allocation for <set_uh> uh_precise failed!");
    }
    double *uh_cumulative;
    if((uh_cumulative = malloc((UH_MAX_DAYS * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_cumulative)))==NULL){
        log_err("Memory allocation for <set_uh> uh_cumulative failed!");
    }
    double uh_sum;
    
    size_t start[]={0, 0};
    size_t count[]={global_domain.n_ny, global_domain.n_nx};
    
    get_nc_field_double(file_path,variable_name,start,count,distance_total);
    
    size_t i;
    size_t j=0;
    for(i=0;i<global_domain.ncells_total;i++){
        size_t x=(size_t)i%global_domain.n_nx;
        size_t y=(size_t)i/global_domain.n_nx;
        
        if(rout.gridded_cells[x][y]!=NULL){
            distance[j]=distance_total[i];
            if(distance[j]==-1){
                log_warn("distance of cell id %zu local_id %zu is missing, check distance file",rout.gridded_cells[x][y]->global_domain_id,rout.gridded_cells[x][y]->id);
            }
            j++;
        }
    }
    
    for (i=0;i<global_domain.ncells_active;i++){
        if(distance[i]!=-1){
            size_t time=0;
            uh_sum=0.0;

            //calculate precise unit hydrograph based on timestep
            for(j=0;j< UH_MAX_DAYS * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
                time += (3600 * 24) / (global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP);
                uh_precise[j]=(distance[i]/(2 * time * sqrt(M_PI * time * OVERLAND_FLOW_DIFFUSION)))
                        * exp(-(pow(OVERLAND_FLOW_VELOCITY * time - distance[i],2)) / (4 * OVERLAND_FLOW_DIFFUSION * time));
                uh_sum += uh_precise[j];
            }

            //normalize unit hydrograph so sum is 1 and make cumulative unit hydrograph
            for(j=0;j< UH_MAX_DAYS * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
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
            
            //make final daily unit hydrograph based on cumulative unity hydrograph
            for(j=0;j< UH_MAX_DAYS * global_param.model_steps_per_day;j++){
                if(j<(UH_MAX_DAYS * global_param.model_steps_per_day)- 1){
                    rout.cells[i].uh[j]=uh_cumulative[(j+1) * UH_STEPS_PER_TIMESTEP] - uh_cumulative[j * UH_STEPS_PER_TIMESTEP];
                }else{
                    rout.cells[i].uh[j]=uh_cumulative[((j+1) * UH_STEPS_PER_TIMESTEP)-1] - uh_cumulative[j * UH_STEPS_PER_TIMESTEP];
                }
            }
        }
    }
    
    free(distance_total);
    free(distance);
    free(uh_cumulative);
    free(uh_precise);
}

void set_reservoirs(){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    
    reservoir_unit* reservoir_temp;
    if((reservoir_temp = malloc(MAX_NR_RESERVOIRS * sizeof(*reservoir_temp)))==NULL){
        log_err("Memory allocation error!");
    }
    
    FILE *rf;
    
    char cmdstr[MAXSTRING];
    char optstr[MAXSTRING];
    
    rf=open_file(rout.reservoir_filename,"r");
    fgets(cmdstr, MAXSTRING, rf);
    
    size_t j=0;
    while (!feof(rf)) {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0') {
            sscanf(cmdstr, "%s", optstr);

            /* Handle case of comment line in which '#' is indented */
            if (optstr[0] == '#') {
                fgets(cmdstr, MAXSTRING, rf);
                continue;
            }
            
            j++;
            
            if(rout.nr_reservoirs>=MAX_NR_RESERVOIRS){
                log_info("there are more reservoirs in the area than is allowed, skipping all reservoirs above %d",MAX_NR_RESERVOIRS);
            }
                        
            char purpose[MAXSTRING];
            float lat;
            float lon;
            reservoir_unit* res = &reservoir_temp[rout.nr_reservoirs];
            
            sscanf(cmdstr, "%*d %s %d %lf %lf %lf %s %lf %lf %f %f", res->name,&res->activation_year,&res->height,&res->storage_capacity,&res->area,purpose,&res->installation_capacity,&res->annual_energy,&lon,&lat);
            
            if(     lat<rout.min_lat - (global_param.resolution/2) || 
                    lon < rout.min_lon - (global_param.resolution/2) || 
                    lat>(rout.min_lat + ((global_domain.n_ny-1) * global_param.resolution) + (global_param.resolution/2)) || 
                    lon>(rout.min_lon + ((global_domain.n_nx-1) * global_param.resolution) + (global_param.resolution/2))){
                
                //reservoir outside location of interest
                fgets(cmdstr, MAXSTRING, rf);
                continue;
                
            }else{
                size_t x = (size_t)((lon - rout.min_lon)/global_param.resolution);
                size_t y = (size_t)((lat - rout.min_lat)/global_param.resolution);
                
                if(rout.gridded_cells[x][y]==NULL){
                    
                    //reservoir inside location of interest but cell not run by VIC
                    fgets(cmdstr, MAXSTRING, rf);
                    continue;
                }
               
                //FIXME: allow for multiple reservoirs in the same cell
                res->id = rout.nr_reservoirs;
                res->cell = rout.gridded_cells[x][y];
                
                if(purpose[0]=='I'){
                    res->function=RES_IRR_FUNCTION;
                }else if(purpose[0]=='H'){
                    res->function=RES_HYD_FUNCTION;
                }else if(purpose[0]=='C'){
                    res->function=RES_CON_FUNCTION;
                }else{
                    log_warn("the function of reservoir %zu %s is not definend; defeault to irrigation",j,res->name);
                    res->function=RES_IRR_FUNCTION;                    
                }
                
                rout.nr_reservoirs++;
            }

        }
        fgets(cmdstr, MAXSTRING, rf);
    }
    
    log_info("%zu reservoirs INSIDE area of interest, %zu OUTSIDE area of interest",rout.nr_reservoirs,j-rout.nr_reservoirs);
    
    if((rout.reservoirs = malloc(rout.nr_reservoirs * sizeof(*rout.reservoirs)))==NULL){
        log_err("Memory allocation error!");
    }
    
    size_t iRes;
    reservoir_unit *current_reservoir;
    for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
        rout.reservoirs[iRes]=reservoir_temp[iRes];
        current_reservoir = &rout.reservoirs[iRes];
                
        current_reservoir->run=false;
        current_reservoir->cell->reservoir=current_reservoir;
                
        if((current_reservoir->mean_monthly_demand = malloc(RES_CALC_YEARS_MEAN * sizeof(*current_reservoir->mean_monthly_demand)))!=NULL){
            size_t i;
            for(i=0;i<RES_CALC_YEARS_MEAN;i++){
                if((current_reservoir->mean_monthly_demand[i] = malloc(MONTHS_PER_YEAR * sizeof(*current_reservoir->mean_monthly_demand[i])))==NULL){
                    log_err("Memory allocation error!");                       
                }
            }
        }else{
            log_err("Memory allocation error!");     
        }
        if((current_reservoir->mean_monthly_inflow = malloc(RES_CALC_YEARS_MEAN * sizeof(*current_reservoir->mean_monthly_inflow)))!=NULL){
            size_t i;
            for(i=0;i<RES_CALC_YEARS_MEAN;i++){
                if((current_reservoir->mean_monthly_inflow[i] = malloc(MONTHS_PER_YEAR * sizeof(*current_reservoir->mean_monthly_inflow[i])))==NULL){
                    log_err("Memory allocation error!");                       
                }
            }
        }else{
            log_err("Memory allocation error!");     
        }
        
        if((current_reservoir->mean_annual_demand = malloc(RES_CALC_YEARS_MEAN * sizeof(*current_reservoir->mean_annual_demand)))==NULL){
            log_err("Memory allocation error!");     
        }
        if((current_reservoir->mean_annual_inflow = malloc(RES_CALC_YEARS_MEAN * sizeof(*current_reservoir->mean_annual_inflow)))==NULL){
            log_err("Memory allocation error!");     
        }
        
        size_t i;
        for(i=0;i<RES_CALC_YEARS_MEAN;i++){
            current_reservoir->mean_annual_inflow[i] = 0.0;
            current_reservoir->mean_annual_demand[i] = 0.0;
            
            size_t j;
            for(j=0;j<MONTHS_PER_YEAR;j++){
                current_reservoir->mean_monthly_inflow[i][j] = 0.0;
                current_reservoir->mean_monthly_demand[i][j] = 0.0;
                
            }
        }
        
        current_reservoir->current_mean_annual_demand = 0.0;
        current_reservoir->current_mean_annual_inflow = 0.0;
        current_reservoir->current_mean_monthly_demand = 0.0;
        current_reservoir->current_mean_monthly_inflow = 0.0;
        
        current_reservoir->river_length=0;
        current_reservoir->nr_serviced_cells=0;
        
        current_reservoir->current_storage=current_reservoir->storage_capacity * RES_PREF_STORAGE; //preferred storage level for the start of the operational year (Hanasaki et al., 2006)
        current_reservoir->storage_start_operation = current_reservoir->current_storage;
        
        current_reservoir->start_operation.day=global_param.startday;
        current_reservoir->start_operation.month=global_param.startmonth;
        current_reservoir->start_operation.year=global_param.startyear;
        
        
        log_info("Reservoir %zu %s, activation year %d",current_reservoir->id,current_reservoir->name,current_reservoir->activation_year);
    }
    
    free(reservoir_temp);
}

void set_reservoir_river(){
    extern rout_struct rout;
    
    rout_cell** river_temp;
    if((river_temp = malloc(RES_MAX_RIVER * sizeof(*river_temp)))==NULL){
        log_err("Memory allocation error!");
    }
    
    reservoir_unit* current_reservoir;
    size_t iRes;
    for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
       current_reservoir = &rout.reservoirs[iRes];
        
        rout_cell* current_cell;
        if(current_reservoir->function == RES_IRR_FUNCTION){
            current_cell=current_reservoir->cell;

            size_t i;
            for(i=0;i<RES_MAX_RIVER;i++){
                current_cell=current_cell->downstream;
                
                river_temp[i]=current_cell;
                current_reservoir->river_length++;

                if(current_cell->downstream==NULL){
                    break;
                }
            }
        }
        
        if((current_reservoir->river = malloc(current_reservoir->river_length * sizeof(*current_reservoir->river)))==NULL){
            log_err("Memory allocation error!");
        }
        
        size_t i;
        for(i=0;i<current_reservoir->river_length;i++){
            current_reservoir->river[i] = river_temp[i];
        }
    }
    
    free(river_temp);
}
    
void set_reservoir_service(){ 
    extern rout_struct rout;
    extern soil_con_struct *soil_con;
    
    reservoir_unit*** service_res_temp;
    if((service_res_temp = malloc(rout.nr_irrigated_cells * sizeof(*service_res_temp)))!=NULL){
        size_t i;
        for(i=0;i<rout.nr_irrigated_cells;i++){
            if((service_res_temp[i] = malloc(CELL_MAX_SERVICE * sizeof(*service_res_temp[i])))==NULL){
                log_err("Memory allocation error!");
            }
        }
    }else{
        log_err("Memory allocation error!");
    }
    
    rout_cell*** service_cell_temp;
    if((service_cell_temp = malloc(rout.nr_reservoirs * sizeof(*service_cell_temp)))!=NULL){
        size_t i;
        for(i=0;i<rout.nr_reservoirs;i++){
            if((service_cell_temp[i] = malloc(RES_MAX_SERVICE * sizeof(*service_cell_temp[i])))==NULL){
                log_err("Memory allocation error!");
            }
        }
    }else{
        log_err("Memory allocation error!");
    }
   
    reservoir_unit *current_reservoir;
    size_t iRes;
    for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
        current_reservoir = &rout.reservoirs[iRes];
        
        if(current_reservoir->function!=RES_IRR_FUNCTION){
            continue;
        }
        
        if(current_reservoir->nr_serviced_cells >= RES_MAX_SERVICE){
            log_warn("a cell is not added to reservoir %zu because of maximum service capacity in reservoir",current_reservoir->id);
            continue;
        }

        rout_cell *current_cell;
        size_t iCell;
        for(iCell=0;iCell<rout.nr_irrigated_cells;iCell++){
             current_cell = rout.irrigated_cells[iCell];
             
             if(current_cell->nr_servicing_reservoirs >= CELL_MAX_SERVICE){
                log_warn("a reservoir is not added to cell %zu because of maximum servicing reservoirs in cell",current_cell->global_domain_id);
                continue;
            }
             
            if(soil_con[current_cell->id].elevation >= soil_con[current_reservoir->cell->id].elevation){
                continue;
            }

            size_t iRiv;
            rout_cell *current_river;
            for(iRiv=0;iRiv<current_reservoir->river_length;iRiv++){
                current_river=current_reservoir->river[iRiv];

                if(distance(current_cell,current_river)<=IRR_RIVER_DISTANCE){

                    service_res_temp[iCell][current_cell->nr_servicing_reservoirs]=current_reservoir;
                    service_cell_temp[iRes][current_reservoir->nr_serviced_cells]=current_cell;

                    current_cell->nr_servicing_reservoirs++;          
                    current_reservoir->nr_serviced_cells++;

                    break;
                }
            }
        }
    }
    
    
    for(iRes=0;iRes<rout.nr_reservoirs;iRes++){
        current_reservoir = &rout.reservoirs[iRes];
        
        if((current_reservoir->serviced_cells = malloc(current_reservoir->nr_serviced_cells * sizeof(*current_reservoir->serviced_cells)))==NULL){
            log_err("Memory allocation error!");            
        }
        if((current_reservoir->demand = malloc(current_reservoir->nr_serviced_cells * sizeof(*current_reservoir->demand)))==NULL){
            log_err("Memory allocation error!");            
        }
        
        size_t i;
        for(i=0;i<current_reservoir->nr_serviced_cells;i++){
            current_reservoir->serviced_cells[i]=service_cell_temp[iRes][i];
            current_reservoir->demand[i]=0.0;
        }
    }
    
    size_t iCell;
    rout_cell* current_cell;
    for(iCell=0;iCell<rout.nr_irrigated_cells;iCell++){
        current_cell = rout.irrigated_cells[iCell];
        
        if((current_cell->servicing_reservoirs = malloc(current_cell->nr_servicing_reservoirs * sizeof(*current_cell->servicing_reservoirs)))==NULL){
            log_err("Memory allocation error!");
        }
        
        size_t i;
        for(i=0;i<current_cell->nr_servicing_reservoirs;i++){
            current_cell->servicing_reservoirs[i]=service_res_temp[iCell][i];
        }
    }
    
    free(service_cell_temp);
    free(service_res_temp);
}

double distance(rout_cell* from, rout_cell* to){
    double a = (double)from->x - (double)to->x;
    double b = (double)from->y - (double)to->y;
    return(sqrt(pow(a,2)+pow(b,2)));
}

void make_location_file(char file_path[], char file_name[]){
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
                    fprintf(file,"%zu",rout.gridded_cells[x][y-1]->rank);
                    if(rout.gridded_cells[x][y-1]->rank < 10){
                        fprintf(file,"  ;");
                    }else if(rout.gridded_cells[x][y-1]->rank < 100){
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
}

void make_uh_file(char file_path[], char file_name[]){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    
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
            for(j=0;j<UH_MAX_DAYS * global_param.model_steps_per_day;j++){
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

void make_reservoir_river_file(char file_path[], char file_name[]){
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
    
    
    size_t r;
    for(r=0;r<rout.nr_reservoirs;r++){
        
        if(rout.reservoirs[r].function!=RES_IRR_FUNCTION){
            continue;
        }
        
        strcpy(full_path, file_path);
        strcat(full_path, file_name);
        strcat(full_path,"_");
        strcat(full_path,rout.reservoirs[r].name);
        strcat(full_path,".txt");
        
        if((file = fopen(full_path, "w"))!=NULL){
            size_t x;
            size_t y;
            bool done;
            for(y=global_domain.n_ny;y>0;y--){
                for(x=0;x<global_domain.n_nx;x++){
                    done=false;
                    if(rout.gridded_cells[x][y-1]!=NULL){                      
                        size_t n;
                        for(n=0;n<rout.reservoirs[r].river_length;n++){
                            if(rout.reservoirs[r].river[n]==rout.gridded_cells[x][y-1]){
                               fprintf(file,"%zu",rout.reservoirs[r].id);
                               if(rout.reservoirs[r].id < 10){
                                    fprintf(file,"  ;");
                                }else if(rout.reservoirs[r].id < 100){
                                    fprintf(file," ;");
                                }else{
                                    fprintf(file,";");
                                }
                                done=true;
                                break;
                            }
                        }
                        if(!done){
                            if(rout.gridded_cells[x][y-1]->reservoir!=NULL && rout.gridded_cells[x][y-1]->reservoir->id == r){
                                fprintf(file," OO;");                                
                            }else{
                                fprintf(file," XX;");                                
                            }                          
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

void make_reservoir_service_file(char file_path[], char file_name[]){
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
                    fprintf(file,"%zu",rout.gridded_cells[x][y-1]->nr_servicing_reservoirs);
                    if(rout.gridded_cells[x][y-1]->nr_servicing_reservoirs < 10){
                        fprintf(file,"  ;");
                    }else if(rout.gridded_cells[x][y-1]->nr_servicing_reservoirs < 100){
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
}