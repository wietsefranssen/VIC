/******************************************************************************
 * @section DESCRIPTION
 *  
 * Functions used in rout_init
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Set the cell information. ID is set to match VIC ID, cells are placed
 * in grid and set their coordinates in the grid.
 ******************************************************************************/

void set_cell_location(){
    extern RID_struct RID;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    
    RID.min_lat=DBL_MAX;
    RID.min_lon=DBL_MAX;
    
    size_t i;
    size_t j;
    size_t x;
    size_t y;
    
    j=0;
    
    for(i=0;i<global_domain.ncells_total;i++){
        if(global_domain.locations[i].run){
            RID.cells[j].global_domain_id=i;
            RID.cells[j].id=j;
            j++;
        }
        
        if(global_domain.locations[i].latitude<RID.min_lat){
            RID.min_lat=global_domain.locations[i].latitude;
        }
        if(global_domain.locations[i].longitude<RID.min_lon){
            RID.min_lon=global_domain.locations[i].longitude;
        }
    }
    
    for(i=0;i<global_domain.ncells_active;i++){
        x = (size_t)((global_domain.locations[RID.cells[i].global_domain_id].longitude - RID.min_lon)/global_param.resolution);
        y = (size_t)((global_domain.locations[RID.cells[i].global_domain_id].latitude - RID.min_lat)/global_param.resolution);
        RID.gridded_cells[x][y]=&RID.cells[i];
        RID.cells[i].x=x;
        RID.cells[i].y=y;
    }
}

/******************************************************************************
 * @section brief
 *  
 * Set the unit-hydrograph for the routing module. Unit hydro-graphs are first
 * calculated precisely and later aggregated to match the time-step of VIC
 ******************************************************************************/

void set_cell_uh(char variable_name[]){
    extern RID_struct RID;
    extern domain_struct global_domain;
    extern global_param_struct global_param;
    extern RID_struct RID;
    extern size_t       *filter_active_cells;
    
    double *distance;
    double *uh_precise;
    double *uh_cumulative;
    double uh_sum;
    double *dvar;
    
    size_t i;
    size_t j=0;
    
    distance = malloc(global_domain.ncells_active * sizeof(*distance));
    check_alloc_status(distance,"Memory allocation error.");
    
    uh_precise = malloc((RID.param.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_precise));
    check_alloc_status(uh_precise,"Memory allocation error.");
    
    uh_cumulative = malloc((RID.param.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP) * sizeof(*uh_cumulative));
    check_alloc_status(uh_cumulative,"Memory allocation error.");
    
    dvar = malloc(global_domain.ncells_total * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");
    
    size_t start[]={0, 0};
    size_t count[]={global_domain.n_ny, global_domain.n_nx};
    //get_scatter_nc_field_double(RID.param.param_filename,variable_name,start,count,distance);    
    
    
    get_nc_field_double(RID.param.param_filename,variable_name,start,count, dvar);
    map(sizeof(double), global_domain.ncells_active, filter_active_cells, NULL, dvar, distance);
    
    for (i=0;i<global_domain.ncells_active;i++){
        size_t time=0;
        uh_sum=0.0;

            
        /*******************************
         Precise unit-hydrograph calculation, also saving sums (Lohmann et al. (1994) equation)
        *******************************/
        for(j=0;j< RID.param.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
            time += (SEC_PER_HOUR * HOURS_PER_DAY) / (global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP);
            uh_precise[j]=(distance[i]/(2 * time * sqrt(M_PI * time * RID.param.flow_diffusivity_uh)))
                    * exp(-(pow(RID.param.flow_velocity_uh * time - distance[i],2)) / (4 * RID.param.flow_diffusivity_uh * time));
            uh_sum += uh_precise[j];
        }

        /*******************************
         Normalizing unit hydro-graphs over extent
        *******************************/
        for(j=0;j< RID.param.max_days_uh * global_param.model_steps_per_day * UH_STEPS_PER_TIMESTEP;j++){
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

        /*******************************
         Aggregate unit hydro-graph over VIC time-step
        *******************************/
        for(j=0;j< RID.param.max_days_uh * global_param.model_steps_per_day;j++){
            if(j<(RID.param.max_days_uh * global_param.model_steps_per_day)- 1){
                RID.cells[i].rout->uh[j]=uh_cumulative[(j+1) * UH_STEPS_PER_TIMESTEP] - uh_cumulative[j * UH_STEPS_PER_TIMESTEP];
            }else{
                RID.cells[i].rout->uh[j]=uh_cumulative[((j+1) * UH_STEPS_PER_TIMESTEP)-1] - uh_cumulative[j * UH_STEPS_PER_TIMESTEP];
            }
            RID.cells[i].rout->outflow[j]=0.0;
            RID.cells[i].rout->outflow_natural[j]=0.0;
        }
    }
    
    free(distance);
    free(uh_precise);
    free(uh_cumulative);
}

/******************************************************************************
 * @section brief
 *  
 * Set the pointer to downstream cells based on direction map.
 ******************************************************************************/

void set_cell_downstream(char variable_name[]){
    extern RID_struct RID;
    extern domain_struct global_domain;
    extern size_t       *filter_active_cells;
    
    int *direction;
    int *ivar;
    
    size_t i;
    size_t x;
    size_t y;
    
    direction = malloc(global_domain.ncells_active * sizeof(*direction));
    check_alloc_status(direction,"Memory allocation error.");
    
    ivar = malloc(global_domain.ncells_total * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");
    
    size_t start[]={0, 0};
    size_t count[]={global_domain.n_ny, global_domain.n_nx};
    //get_scatter_nc_field_int(RID.param.param_filename,variable_name,start,count,direction);
          
    get_nc_field_int(RID.param.param_filename,variable_name,start,count, ivar);
    map(sizeof(int), global_domain.ncells_active, filter_active_cells, NULL, ivar, direction);
        
    for(i=0;i<global_domain.ncells_active;i++){        
        x=RID.cells[i].x;
        y=RID.cells[i].y;
        
        /*******************************
         1=north
         2=north-east
         3=east
         4=south-east
         etc.
        *******************************/
        if(direction[i]==1){
            if(y+1<global_domain.n_ny && RID.gridded_cells[x][y+1]!=NULL){
                RID.gridded_cells[x][y]->rout->downstream=RID.gridded_cells[x][y+1]->rout;
            }
        }else if(direction[i]==2){
            if(x+1<global_domain.n_nx && y+1<global_domain.n_ny && RID.gridded_cells[x+1][y+1]!=NULL){
                RID.gridded_cells[x][y]->rout->downstream=RID.gridded_cells[x+1][y+1]->rout;
            }
        }else if(direction[i]==3){
            if(x+1<global_domain.n_nx && RID.gridded_cells[x+1][y]!=NULL){
                RID.gridded_cells[x][y]->rout->downstream=RID.gridded_cells[x+1][y]->rout;
            }
        }else if(direction[i]==4){
            if(x+1<global_domain.n_nx && y>=1 && RID.gridded_cells[x+1][y-1]!=NULL){
                RID.gridded_cells[x][y]->rout->downstream=RID.gridded_cells[x+1][y-1]->rout;
            }
        }else if(direction[i]==5){
            if(y>=1 && RID.gridded_cells[x][y-1]!=NULL){
                RID.gridded_cells[x][y]->rout->downstream=RID.gridded_cells[x][y-1]->rout;
            }
        }else if(direction[i]==6){
            if(y>=1 && x>=1 && RID.gridded_cells[x-1][y-1]!=NULL){
                RID.gridded_cells[x][y]->rout->downstream=RID.gridded_cells[x-1][y-1]->rout;
            }
        }else if(direction[i]==7){
            if(x>=1 && RID.gridded_cells[x-1][y]!=NULL){
                RID.gridded_cells[x][y]->rout->downstream=RID.gridded_cells[x-1][y]->rout;
            }
        }else if(direction[i]==8){
            if(x>=1 && y+1<global_domain.n_ny && RID.gridded_cells[x-1][y+1]!=NULL){
                RID.gridded_cells[x][y]->rout->downstream=RID.gridded_cells[x-1][y+1]->rout;
            }
        }
    }
    
    free(direction);
}

/******************************************************************************
 * @section brief
 *  
 * Set the pointer to upstream cells based on previously defined
 * downstream cells.
 ******************************************************************************/

void set_cell_upstream(){
    extern RID_struct RID;
    extern domain_struct global_domain;
    
    rout_cell** upstream_cells;
    
    size_t i;
    size_t j;
    size_t x;
    size_t y;
    
    upstream_cells = malloc(8 * sizeof(*upstream_cells));
    check_alloc_status(upstream_cells,"Memory allocation error.");
    
    for(i=0;i<global_domain.ncells_active;i++){
        x=RID.cells[i].x;
        y=RID.cells[i].y;
        
        RID.gridded_cells[x][y]->rout->nr_upstream=0;
             
        if(y+1<global_domain.n_ny && RID.gridded_cells[x][y+1]!=NULL){
            if(RID.gridded_cells[x][y+1]->rout->downstream == RID.gridded_cells[x][y]->rout){
                upstream_cells[RID.gridded_cells[x][y]->rout->nr_upstream]=RID.gridded_cells[x][y+1]->rout;
                RID.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(x+1<global_domain.n_nx && y+1<global_domain.n_ny && RID.gridded_cells[x+1][y+1]!=NULL){
            if(RID.gridded_cells[x+1][y+1]->rout->downstream == RID.gridded_cells[x][y]->rout){
                upstream_cells[RID.gridded_cells[x][y]->rout->nr_upstream]=RID.gridded_cells[x+1][y+1]->rout;
                RID.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(x+1<global_domain.n_nx && RID.gridded_cells[x+1][y]!=NULL){
            if(RID.gridded_cells[x+1][y]->rout->downstream == RID.gridded_cells[x][y]->rout){
                upstream_cells[RID.gridded_cells[x][y]->rout->nr_upstream]=RID.gridded_cells[x+1][y]->rout;
                RID.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(x+1<global_domain.n_nx && y>=1 && RID.gridded_cells[x+1][y-1]!=NULL){
            if(RID.gridded_cells[x+1][y-1]->rout->downstream == RID.gridded_cells[x][y]->rout){
                upstream_cells[RID.gridded_cells[x][y]->rout->nr_upstream]=RID.gridded_cells[x+1][y-1]->rout;
                RID.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(y>=1 && RID.gridded_cells[x][y-1]!=NULL){
            if(RID.gridded_cells[x][y-1]->rout->downstream == RID.gridded_cells[x][y]->rout){
                upstream_cells[RID.gridded_cells[x][y]->rout->nr_upstream]=RID.gridded_cells[x][y-1]->rout;
                RID.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(y>=1 && x>=1 && RID.gridded_cells[x-1][y-1]!=NULL){
            if(RID.gridded_cells[x-1][y-1]->rout->downstream == RID.gridded_cells[x][y]->rout){
                upstream_cells[RID.gridded_cells[x][y]->rout->nr_upstream]=RID.gridded_cells[x-1][y-1]->rout;
                RID.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(x>=1 && RID.gridded_cells[x-1][y]!=NULL){
            if(RID.gridded_cells[x-1][y]->rout->downstream == RID.gridded_cells[x][y]->rout){
                upstream_cells[RID.gridded_cells[x][y]->rout->nr_upstream]=RID.gridded_cells[x-1][y]->rout;
                RID.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        if(x>=1 && y+1<global_domain.n_ny && RID.gridded_cells[x-1][y+1]!=NULL){
            if(RID.gridded_cells[x-1][y+1]->rout->downstream == RID.gridded_cells[x][y]->rout){
                upstream_cells[RID.gridded_cells[x][y]->rout->nr_upstream]=RID.gridded_cells[x-1][y+1]->rout;
                RID.gridded_cells[x][y]->rout->nr_upstream++;
            }
        }
        
        
        RID.cells[i].rout->upstream = malloc(RID.cells[i].rout->nr_upstream * sizeof(*RID.cells[i].rout->upstream));
        check_alloc_status(RID.cells[i].rout->upstream,"Memory allocation error.");
        
        for(j=0;j<RID.cells[i].rout->nr_upstream;j++){
            RID.cells[i].rout->upstream[j]=upstream_cells[j];
        }
    }
    
    free(upstream_cells);
}

/******************************************************************************
 * @section brief
 *  
 * Set the upstream to downstream rank of cells and fill sorted_cells.
 * This is done by checking the number of upstream cells, if there are no
 * upstream cells then assign the current rank and remove the cell from the
 * algorithm. As cells are removed other cells will be freed of upstream cells.
 * Continue until finished.
 ******************************************************************************/

void set_cell_rank(void){ 
    extern RID_struct RID;
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
                
                int count=0;
                for(j=0;j<RID.cells[i].rout->nr_upstream;j++){
                    if(!done_map[RID.cells[i].rout->upstream[j]->cell->id]){
                        count++;
                    }
                }

                if(count==0){
                    RID.sorted_cells[rank]=&RID.cells[i];
                    RID.cells[i].rout->rank=rank;
                    rank++;
                }  
            }
        }
        
        for(i=0;i<rank;i++){
            done_map[RID.sorted_cells[i]->id]=true;
        }
        
        if(rank == global_domain.ncells_active){
            break;
        }else if(rank > global_domain.ncells_active){
            log_err("rank_cells made %zu loops and escaped because this is more than %zu, the number of active cells",(rank+1),global_domain.ncells_active);
            break;
        }
    }
    
    free(done_map);
}