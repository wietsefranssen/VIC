/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <ext_shared_image.h>

void get_basins(char *nc_name, basin_struct *basins){
    extern domain_struct global_domain;
    
    int    *direction = NULL;
    
    size_t  Nriver = 0;
    size_t *river = NULL;
    int cur_direction;
    size_t cur_cell;
    size_t next_cell;
    
    size_t  i;
    size_t  j;
    size_t  d2count[2];
    size_t  d2start[2];
    
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;
    
    direction = malloc(global_domain.ncells_total * sizeof(*direction));
    check_alloc_status(direction, "Memory allocation error.");
    river = malloc(global_domain.ncells_total * sizeof(*river));
    check_alloc_status(river, "Memory allocation error.");
    
    basins->basin_map = malloc(global_domain.ncells_total * sizeof(*basins->basin_map));
    check_alloc_status(basins->basin_map, "Memory allocation error.");
    
    for (i = 0; i < global_domain.ncells_total; i++) {
        direction[i] = NODATA_DIRECTION;
        basins->basin_map[i] = NODATA_BASIN;
    }
    
    get_nc_field_int(nc_name, "flow_direction", d2start, d2count,
                     direction);
    
    // -1 = unavailable
    // 1 = north
    // 2 = north-east
    // 3 = east
    // etc.
    // 9 = outflow
    basins->Nbasin = 0;
    Nriver=0;
    for (i = 0; i < global_domain.ncells_total; i++) {
            if(global_domain.locations[i].run){        

            Nriver=0;
            cur_cell = next_cell = i;        

            if(direction[cur_cell] == NODATA_DIRECTION){
                continue;
            }

            while(true){    

                cur_direction = direction[cur_cell];
                river[Nriver]=cur_cell; 
                Nriver++;          

                if(basins->basin_map[cur_cell]!=NODATA_BASIN){
                    for(j=0;j<Nriver;j++){
                        basins->basin_map[river[j]]=basins->basin_map[cur_cell];
                    }
                    break;
                }

                switch(cur_direction){
                    case 1:
                        if(i<global_domain.n_nx){
                            log_err("Flow direction is going outside of domain");                        
                        }
                        next_cell = i + global_domain.n_nx;
                        break;
                    case 2:
                        if(i<global_domain.n_nx){
                            log_err("Flow direction is going outside of domain");                        
                        }
                        next_cell = i + global_domain.n_nx + 1;
                        break;
                    case 3:
                        next_cell = i + 1;
                        break;
                    case 4:
                        next_cell = i - global_domain.n_nx + 1;
                        break;
                    case 5:
                        next_cell = i - global_domain.n_nx;
                        break;
                    case 6:
                        next_cell = i - global_domain.n_nx - 1;
                        break;
                    case 7:
                        next_cell = i - 1;
                        break;
                    case 8:
                        if(i<global_domain.n_nx){
                            log_err("Flow direction is going outside of domain");                        
                        }
                        next_cell = i + global_domain.n_nx - 1;
                        break;
                    case 9:
                        next_cell = i;
                        break;
                    default:
                        log_err("Unknown flow direction in file")
                        break;
                }   

                if(next_cell >= global_domain.ncells_total){
                    log_err("Flow direction is going outside of domain");
                }    

                if(next_cell == cur_cell){
                    for(j=0;j<Nriver;j++){
                        basins->basin_map[river[j]]=basins->Nbasin;
                    }

                    basins->Nbasin++;
                    break;
                }

                cur_cell = next_cell;
            }
        }
    }
        
    
    
    basins->Ncells = malloc(basins->Nbasin * sizeof(*basins->Ncells));
    check_alloc_status(basins->Ncells, "Memory allocation error.");
    basins->sorted_basins = malloc(basins->Nbasin * sizeof(*basins->sorted_basins));
    check_alloc_status(basins->sorted_basins, "Memory allocation error.");
        
    // sort basins by size
    for(i=0;i<basins->Nbasin;i++){
        basins->sorted_basins[i]=i;
    }     
    sizet_sort(basins->sorted_basins,basins->Ncells,basins->Nbasin,false);

    for(i=0;i<basins->Nbasin;i++){
        basins->Ncells[i]=0;
    }
    
    for (i = 0; i < global_domain.ncells_total; i++) {
        if(basins->basin_map[i]!=NODATA_BASIN){
            basins->Ncells[basins->basin_map[i]]++;
        }
    }
        
    basins->catchment = malloc(basins->Nbasin * sizeof(*basins->catchment));
    check_alloc_status(basins->catchment, "Memory allocation error.");
    for(i=0;i<basins->Nbasin;i++){
        basins->catchment[i] = malloc(basins->Ncells[i] * sizeof(basins->catchment[i]));
        check_alloc_status(basins->catchment[i], "Memory allocation error.");
    }
        
    for(i=0;i<basins->Nbasin;i++){
        basins->Ncells[i]=0;
        for(j=0;j<basins->Ncells[i];j++){
            basins->catchment[i][j]=0;
        }
    }
    
    for (i = 0; i < global_domain.ncells_total; i++) {
        if(basins->basin_map[i]!=NODATA_BASIN){
            basins->catchment[basins->basin_map[i]][basins->Ncells[basins->basin_map[i]]] = i;
            basins->Ncells[basins->basin_map[i]]++;
        }
    }
}