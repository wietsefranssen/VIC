#include "vic_driver_shared_image.h"
#include "rout.h"


void rout_finalize(void){
    extern rout_struct rout;
    extern domain_struct global_domain;
    size_t i;
    size_t j;
    
    //make_sensitivity_files();
    
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
        free(rout.reservoirs[i].prev_deficit);
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
    extern rout_sa_struct rout_sa;
    
    char nc_name[] = "output/fluxes.1981-01-01.nc";
    char dim_name[]= "time";
    
    FILE *file;
    char full_path [MAXSTRING];
    
    double *discharge_rm;
    double **res_store;
    double *dvar;
    size_t i;
    size_t j;
    
    size_t simulation_time = get_nc_dimension(nc_name,dim_name);
    //size_t discharge_station[]={21,21,27,16};
    size_t river_mouth_id = 251;
    
    dvar = malloc(rout_sa.n_total * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");
    
    discharge_rm = malloc(simulation_time * sizeof(*discharge_rm));
    check_alloc_status(discharge_rm, "Memory allocation error.");
            
    res_store = malloc(rout_sa.n_total * sizeof(*res_store));
    check_alloc_status(res_store, "Memory allocation error.");
        
    for(i=0;i<rout_sa.n_total;i++){
        res_store[i]=malloc(simulation_time * sizeof(*res_store[i]));
        check_alloc_status(res_store,"Memory allocation error.");
    }
        
    /*FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, rout.debug_path);
    strcat(full_path, "station_discharge");
    strcat(full_path, ".csv");
    
    if((file = fopen(full_path, "w"))!=NULL){
        fprintf(file,"VIC discharge at discharge station locations [m/s]\n");
        size_t curi = 0;
        curi = rout.gridded_cells[discharge_station[0]][discharge_station[1]]->global_domain_id;
        curi = rout_sa.nx * discharge_station[1] + discharge_station[0];
        fprintf(file,"Upstream discharge station (102.62/17.93)\t");
        for(i=0;i<simulation_time;i++){
            fprintf(file,"%.1f\t",discharge[curi][i]);
        }
        fprintf(file,"\n");
        curi = rout.gridded_cells[discharge_station[2]][discharge_station[3]]->global_domain_id;
        fprintf(file,"Downstream discharge station (105.80/15.12)\t");
        for(i=0;i<simulation_time;i++){
            fprintf(file,"%.1f\t",discharge[curi][i]);
        }
        fclose(file);
    }*/
    
    
    size_t start[]={0, 0, 0};
    size_t count[]={1,rout_sa.ny, rout_sa.nx};
    
    //Get data from NETcdf file
    for(i=0;i<simulation_time;i++){
        start[0]=i;
        get_nc_field_double("output/fluxes.1981-01-01.nc","OUT_RES_STORE",start,count,dvar);
        for(j=0;j<rout_sa.n_total;j++){
            res_store[j][i]=(double)dvar[j];
        }
    }
    
    strcpy(full_path, rout.debug_path);
    strcat(full_path, "reservoir_storage");
    strcat(full_path, ".csv");
    
    if((file = fopen(full_path, "w"))!=NULL){
        fprintf(file,"VIC reservoir storage [-]\n");
        size_t curi = 0;
        
        for(j=0;j<rout.nr_reservoirs;j++){
            curi = rout.reservoirs[j].cell->global_domain_id;
            fprintf(file,"Reservoir %s\t",
                    rout.reservoirs[j].name);
            for(i=0;i<simulation_time;i++){
                fprintf(file,"%.1f\t",res_store[curi][i]);
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
    
    //Get data from NETcdf file
    for(i=0;i<simulation_time;i++){
        fprintf(LOG_DEST,"Daystep\t%zu\tof\t%zu,\tinserting values",i,simulation_time);
        start[0]=i;
        get_nc_field_double("output/fluxes.1981-01-01.nc","OUT_DISCHARGE",start,count,dvar);
        discharge_rm[i]=(double)dvar[river_mouth_id];
    }
    
    strcpy(full_path, rout.debug_path);
    strcat(full_path, "river_mouth_discharge");
    strcat(full_path, ".csv");
    
    if((file = fopen(full_path, "w"))!=NULL){
        fprintf(file,"VIC discharge at river mouth [m/s]\n");
        fprintf(file,"River Mouth (106.75/10.25)\n");
        for(i=0;i<simulation_time;i++){
            fprintf(file,"%.1f\n",discharge_rm[i]);
        }
        fprintf(file,"\n");
        fclose(file);
    }
    
    
}