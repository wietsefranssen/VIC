/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "vic_driver_shared_image.h"
#include "rout.h"


void rout_finalize(void){
    extern rout_struct rout;
    extern domain_struct global_domain;
    size_t i;
    size_t j;
    
    make_sensitivity_files();
    
    //free rout_cells
    for(i=0;i<global_domain.ncells_active;i++){
        free(rout.cells[i].uh);
        free(rout.cells[i].outflow);
        free(rout.cells[i].upstream);
        free(rout.cells[i].servicing_reservoirs);
    }
    
    //free reservoir_unit
    for(i=0;i<rout.nr_reservoirs;i++){
        free(rout.reservoirs[i].serviced_cells);
        free(rout.reservoirs[i].cell_demand);
        free(rout.reservoirs[i].prev_soil_moisture);
        for(j=0;j<RES_CALC_YEARS_MEAN;j++){
            free(rout.reservoirs[i].inflow[j]);
            free(rout.reservoirs[i].demand[j]);
            free(rout.reservoirs[i].inflow_natural[j]);
        }
        free(rout.reservoirs[i].inflow);
        free(rout.reservoirs[i].demand);
        free(rout.reservoirs[i].inflow_natural);
    }
    
    //free rout_struct
    for(i=0;i<global_domain.n_nx;i++){
        free(rout.gridded_cells[i]);
    }
    free(rout.gridded_cells);        
    free(rout.sorted_cells);
    free(rout.cells);
    free(rout.reservoirs);
    
}

void make_sensitivity_files(){
    extern rout_struct rout;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    
    char nc_name[] = "output/fluxes.1981-01-01.nc";
    char dim_name[]= "time";
    
    size_t simulation_time = get_nc_dimension(nc_name,dim_name);
        
    double **discharge;
    double **res_store;
    double *ivar;
    size_t i;
    size_t j;
    
    ivar = malloc(global_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");
    
    discharge = malloc(global_domain.ncells_active * sizeof(*discharge));
    check_alloc_status(discharge, "Memory allocation error.");
        
    for(i=0;i<global_domain.ncells_active;i++){
        discharge[i]=malloc(simulation_time * sizeof(*discharge[i]));
        check_alloc_status(discharge,"Memory allocation error.");
    }
    
    res_store = malloc(global_domain.ncells_active * sizeof(*res_store));
    check_alloc_status(res_store, "Memory allocation error.");
        
    for(i=0;i<global_domain.ncells_active;i++){
        res_store[i]=malloc(simulation_time * sizeof(*res_store[i]));
        check_alloc_status(res_store,"Memory allocation error.");
    }
    
    size_t discharge_station[]={21,21,27,16};
    
    //Get data from NETcdf file
    size_t start[]={0, 0, 0};
    size_t count[]={1,global_domain.n_ny, global_domain.n_nx};
    for(i=0;i<simulation_time;i++){
        start[0]=i;
        get_scatter_nc_field_double("output/fluxes.1981-01-01.nc","OUT_DISCHARGE",start,count,ivar);
        for(j=0;j<global_domain.ncells_active;j++){
            discharge[j][i]=(double)ivar[j];
        }
    }
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, rout.debug_path);
    strcat(full_path, "discharge");
    strcat(full_path, ".csv");
    
    if((file = fopen(full_path, "w"))!=NULL){
        fprintf(file,"VIC discharge at discharge station locations [m/s]\n");
        size_t curi = 0;
        curi = rout.gridded_cells[discharge_station[0]][discharge_station[1]]->id;
        fprintf(file,"Upstream discharge station (102.62/17.93)\t");
        for(i=0;i<simulation_time;i++){
            fprintf(file,"%.1f\t",discharge[curi][i]);
        }
        fprintf(file,"\n");
        curi = rout.gridded_cells[discharge_station[2]][discharge_station[3]]->id;
        fprintf(file,"Downstream discharge station (105.80/15.12)\t");
        for(i=0;i<simulation_time;i++){
            fprintf(file,"%.1f\t",discharge[curi][i]);
        }
        fclose(file);
    }

    //Get data from NETcdf file
    for(i=0;i<simulation_time;i++){
        start[0]=i;
        get_scatter_nc_field_double("output/fluxes.1981-01-01.nc","OUT_RES_STORE",start,count,ivar);
        for(j=0;j<global_domain.ncells_active;j++){
            res_store[j][i]=(double)ivar[j];
        }
    }
    
    strcpy(full_path, rout.debug_path);
    strcat(full_path, "reservoir_storage");
    strcat(full_path, ".csv");
    
    if((file = fopen(full_path, "w"))!=NULL){
        fprintf(file,"VIC reservoir storage [-]\n");
        size_t curi = 0;
        
        for(j=0;j<rout.nr_reservoirs;j++){
            curi = rout.reservoirs[j].cell->id;
            fprintf(file,"Reservoir %s (%.1f/%.1f)\t",
                    rout.reservoirs[j].name,
                    local_domain.locations[rout.reservoirs[j].cell->id].longitude,
                    local_domain.locations[rout.reservoirs[j].cell->id].latitude);
            for(i=0;i<simulation_time;i++){
                fprintf(file,"%.1f\t",res_store[curi][i]);
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
    
}