/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <ext_shared_image.h>

void get_basins(char *nc_name, basin_struct *basins){
    extern domain_struct global_domain;
    
    int    *direction = NULL;
    size_t *basin_map = NULL;
    
    size_t  Nriver = 0;
    size_t *river = NULL;
    size_t next_cell;
    
    size_t  i;
    size_t  d2count[2];
    size_t  d2start[2];
    
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain->n_ny;
    d2count[1] = global_domain->n_nx;
    
    direction = malloc(global_domain->ncells_total * sizeof(*direction));
    check_alloc_status(direction, "Memory allocation error.");
    basin_map = malloc(global_domain->ncells_total * sizeof(*basin_map));
    check_alloc_status(basin_map, "Memory allocation error.");
    river = malloc(global_domain->ncells_total * sizeof(*river));
    check_alloc_status(river, "Memory allocation error.");
    
    for (i = 0; i < global_domain->ncells_total; i++) {
        direction[i]=-1;
        river[i] = 999999;
        basin_map[i] = 999999;
    }
    
    get_nc_field_int(nc_name, global_domain->info.mask_var, d2start, d2count,
                     direction);
    
    basins->Nbasin = 0;
    Nriver=0;
    // -1 = unavailable
    // 1 = north
    // 2 = north-east
    // 3 = east
    // etc.
    // 9 = outflow
    for (i = 0; i < global_domain->ncells_total; i++) {
        switch(direction){
            case -1:
                break;
            case 0:
                next_cell = i;
                break;
            case 1:
                next_cell = i - global_domain.n_nx;
                break;
            case 2:
                next_cell = i - global_domain.n_nx + 1;
                break;
            case 3:
                next_cell = i + 1;
                break;
            case 4:
                next_cell = i + global_domain.n_nx + 1;
                break;
            case 5:
                next_cell = i + global_domain.n_nx;
                break;
            case 6:
                next_cell = i + global_domain.n_nx - 1;
                break;
            case 7:
                next_cell = i - 1;
                break;
            case 8:
                next_cell = i - global_domain.n_nx - 1;
                break;
            case 9:
                next_cell = i;
                break;
            default:
                log_err("Unknown flow direction in file")
                break;
        }
    }
}