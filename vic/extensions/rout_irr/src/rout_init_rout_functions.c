#include <rout.h>

void set_cell_location(){
    //Set cell location in grid based on longitude and latitude from VIC
    extern module_struct rout;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    
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
    }
}

void set_cell_uh(char variable_name[]){
    //Set the unit hydrograph based on the distance in the routing file,
    //and the flow velocity and -diffusion specified in the global parameter file
    extern module_struct rout;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern module_struct rout;
    
    double *distance;
    double *uh_precise;
    double *uh_cumulative;
    double uh_sum;
    
    size_t i;
    size_t j=0;
    
    distance = malloc(global_domain.ncells_active * sizeof(*distance));
    check_alloc_status(distance,"Memory allocation error.");
    
    uh_precise = malloc((rout.param.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_precise));
    check_alloc_status(uh_precise,"Memory allocation error.");
    
    uh_cumulative = malloc((rout.param.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_cumulative));
    check_alloc_status(uh_cumulative,"Memory allocation error.");
    
    size_t start[]={0, 0};
    size_t count[]={global_domain.n_ny, global_domain.n_nx};
    get_scatter_nc_field_double(rout.param.param_filename,variable_name,start,count,distance);    
    
    //Calculate unit hydrograph
    for (i=0;i<global_domain.ncells_active;i++){
        size_t time=0;
        uh_sum=0.0;
            
        if(distance[i]==-1){
            log_warn("distance of cell (global_id %zu local_id %zu) is missing, check distance file",rout.cells[i].global_domain_id,rout.cells[i].id);
            continue;
        }

        //Calculate precise unit hydrograph based on the steps per timestep, also save sum
        for(j=0;j< rout.param.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
            time += (SEC_PER_HOUR * HOURS_PER_DAY) / (global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP);
            //Lohmann et al. (1994) equation 
            uh_precise[j]=(distance[i]/(2 * time * sqrt(M_PI * time * rout.param.flow_diffusivity_uh)))
                    * exp(-(pow(rout.param.flow_velocity_uh * time - distance[i],2)) / (4 * rout.param.flow_diffusivity_uh * time));
            uh_sum += uh_precise[j];
        }

        //Normalize unit hydrograph so sum is 1 and make cumulative unit hydrograph
        for(j=0;j< rout.param.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
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
        for(j=0;j< rout.param.max_days_uh * global_param.model_steps_per_day;j++){
            if(j<(rout.param.max_days_uh * global_param.model_steps_per_day)- 1){
                rout.cells[i].rout->uh[j]=uh_cumulative[(j+1) * UH_STEPS_PER_TIMESTEP] - uh_cumulative[j * UH_STEPS_PER_TIMESTEP];
            }else{
                rout.cells[i].rout->uh[j]=uh_cumulative[((j+1) * UH_STEPS_PER_TIMESTEP)-1] - uh_cumulative[j * UH_STEPS_PER_TIMESTEP];
            }
            rout.cells[i].rout->outflow[j]=0.0;
            rout.cells[i].rout->outflow_natural[j]=0.0;
        }
    }
    
    free(distance);
    free(uh_cumulative);
    free(uh_precise);
}


void set_cell_downstream(char variable_name[]){
    //Set the cells downstream cell based on the direction from the parameter file
    extern module_struct rout;
    extern domain_struct global_domain;
    
    int *direction;
    
    size_t i;
    size_t x;
    size_t y;
    
    direction = malloc(global_domain.ncells_active * sizeof(*direction));
    check_alloc_status(direction,"Memory allocation error.");
    
    //Get data from NETcdf file
    size_t start[]={0, 0};
    size_t count[]={global_domain.n_ny, global_domain.n_nx};
    get_scatter_nc_field_int(rout.param.param_filename,variable_name,start,count,direction);
          
    for(i=0;i<global_domain.ncells_active;i++){        
        x=rout.cells[i].x;
        y=rout.cells[i].y;
            
        if(direction[i]==-1){
            log_warn("direction of cell (global_id %zu local_id %zu) is missing, check direction file",rout.gridded_cells[x][y]->global_domain_id,rout.gridded_cells[x][y]->id);
            continue;
        }
        
        //Set downstream cell based on direction,
        //if direction is pointing to an active cell
        //1=north
        //2=north-east
        //3=east
        //4=south-east
        //etc.
        if(direction[i]==1){
            if(y+1<global_domain.n_ny && rout.gridded_cells[x][y+1]!=NULL){
                rout.gridded_cells[x][y]->rout->downstream=rout.gridded_cells[x][y+1]->rout;
            }
        }else if(direction[i]==2){
            if(x+1<global_domain.n_nx && y+1<global_domain.n_ny && rout.gridded_cells[x+1][y+1]!=NULL){
                rout.gridded_cells[x][y]->rout->downstream=rout.gridded_cells[x+1][y+1]->rout;
            }
        }else if(direction[i]==3){
            if(x+1<global_domain.n_nx && rout.gridded_cells[x+1][y]!=NULL){
                rout.gridded_cells[x][y]->rout->downstream=rout.gridded_cells[x+1][y]->rout;
            }
        }else if(direction[i]==4){
            if(x+1<global_domain.n_nx && y>=1 && rout.gridded_cells[x+1][y-1]!=NULL){
                rout.gridded_cells[x][y]->rout->downstream=rout.gridded_cells[x+1][y-1]->rout;
            }
        }else if(direction[i]==5){
            if(y>=1 && rout.gridded_cells[x][y-1]!=NULL){
                rout.gridded_cells[x][y]->rout->downstream=rout.gridded_cells[x][y-1]->rout;
            }
        }else if(direction[i]==6){
            if(y>=1 && x>=1 && rout.gridded_cells[x-1][y-1]!=NULL){
                rout.gridded_cells[x][y]->rout->downstream=rout.gridded_cells[x-1][y-1]->rout;
            }
        }else if(direction[i]==7){
            if(x>=1 && rout.gridded_cells[x-1][y]!=NULL){
                rout.gridded_cells[x][y]->rout->downstream=rout.gridded_cells[x-1][y]->rout;
            }
        }else if(direction[i]==8){
            if(x>=1 && y+1<global_domain.n_ny && rout.gridded_cells[x-1][y+1]!=NULL){
                rout.gridded_cells[x][y]->rout->downstream=rout.gridded_cells[x-1][y+1]->rout;
            }
        }
    }
    
    free(direction);
}

void set_cell_upstream(){    
    //Set cell upstream files based on the neighboring cells downstream cell
    extern module_struct rout;
    extern domain_struct global_domain;
    
    rout_cell** upstream_temp;
    
    size_t i;
    size_t j;
    size_t x;
    size_t y;
    
    upstream_temp = malloc(8 * sizeof(*upstream_temp));
    check_alloc_status(upstream_temp,"Memory allocation error.");
    
    for(i=0;i<global_domain.ncells_active;i++){
        x=rout.cells[i].x;
        y=rout.cells[i].y;
        
        rout.gridded_cells[x][y]->rout->nr_upstream=0;
        
        //If neighboring cell is run by VIC, check his downstream cell
        //If you are its downstream cell, set the neighboring cell as an upstream cell        
        if(y+1<global_domain.n_ny && rout.gridded_cells[x][y+1]!=NULL){
            if(rout.gridded_cells[x][y+1]->rout->downstream == rout.gridded_cells[x][y]->rout){
                upstream_temp[rout.gridded_cells[x][y]->rout->nr_upstream]=rout.gridded_cells[x][y+1]->rout;
                rout.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(x+1<global_domain.n_nx && y+1<global_domain.n_ny && rout.gridded_cells[x+1][y+1]!=NULL){
            if(rout.gridded_cells[x+1][y+1]->rout->downstream == rout.gridded_cells[x][y]->rout){
                upstream_temp[rout.gridded_cells[x][y]->rout->nr_upstream]=rout.gridded_cells[x+1][y+1]->rout;
                rout.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(x+1<global_domain.n_nx && rout.gridded_cells[x+1][y]!=NULL){
            if(rout.gridded_cells[x+1][y]->rout->downstream == rout.gridded_cells[x][y]->rout){
                upstream_temp[rout.gridded_cells[x][y]->rout->nr_upstream]=rout.gridded_cells[x+1][y]->rout;
                rout.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(x+1<global_domain.n_nx && y>=1 && rout.gridded_cells[x+1][y-1]!=NULL){
            if(rout.gridded_cells[x+1][y-1]->rout->downstream == rout.gridded_cells[x][y]->rout){
                upstream_temp[rout.gridded_cells[x][y]->rout->nr_upstream]=rout.gridded_cells[x+1][y-1]->rout;
                rout.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(y>=1 && rout.gridded_cells[x][y-1]!=NULL){
            if(rout.gridded_cells[x][y-1]->rout->downstream == rout.gridded_cells[x][y]->rout){
                upstream_temp[rout.gridded_cells[x][y]->rout->nr_upstream]=rout.gridded_cells[x][y-1]->rout;
                rout.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(y>=1 && x>=1 && rout.gridded_cells[x-1][y-1]!=NULL){
            if(rout.gridded_cells[x-1][y-1]->rout->downstream == rout.gridded_cells[x][y]->rout){
                upstream_temp[rout.gridded_cells[x][y]->rout->nr_upstream]=rout.gridded_cells[x-1][y-1]->rout;
                rout.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(x>=1 && rout.gridded_cells[x-1][y]!=NULL){
            if(rout.gridded_cells[x-1][y]->rout->downstream == rout.gridded_cells[x][y]->rout){
                upstream_temp[rout.gridded_cells[x][y]->rout->nr_upstream]=rout.gridded_cells[x-1][y]->rout;
                rout.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(x>=1 && y+1<global_domain.n_ny && rout.gridded_cells[x-1][y+1]!=NULL){
            if(rout.gridded_cells[x-1][y+1]->rout->downstream == rout.gridded_cells[x][y]->rout){
                upstream_temp[rout.gridded_cells[x][y]->rout->nr_upstream]=rout.gridded_cells[x-1][y+1]->rout;
                rout.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        
        //Allocate upstream cells based on number of upstream cells
        rout.cells[i].rout->upstream = malloc(rout.cells[i].rout->nr_upstream * sizeof(*rout.cells[i].rout->upstream));
        check_alloc_status(rout.cells[i].rout->upstream,"Memory allocation error.");
        
        for(j=0;j<rout.cells[i].rout->nr_upstream;j++){
            rout.cells[i].rout->upstream[j]=upstream_temp[j];
        }
    }
    
    free(upstream_temp);
}

void set_cell_rank(void){
    //Set the upstream to downstream rank of cells,
    //and put cells in rout.sorted_cells for use in rout.run    
    extern module_struct rout;
    extern domain_struct global_domain;
    
    bool *done_map;
    
    size_t i;
    size_t rank=0;
    size_t j;
    
    //Done_map is used for flagging cells that have been ranked already
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
                for(j=0;j<rout.cells[i].rout->nr_upstream;j++){
                    if(!done_map[rout.cells[i].rout->upstream[j]->cell->id]){
                        //If upstream cell has not been done
                        count++;
                    }
                }

                if(count==0){
                    //Meaning there are no (not-done) cells upstream of this cell
                    rout.sorted_cells[rank]=&rout.cells[i];
                    rout.cells[i].rout->rank=rank;
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