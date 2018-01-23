#include <vic.h>

void
rout_run(size_t cur_cell)
{
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern option_struct options;
    extern rout_var_struct *rout_var;
    extern rout_con_struct *rout_con;
    extern double ***out_data;
    
    double inflow;
    double nat_inflow;
    double runoff;
    
    size_t i;
                  
    rout_var[cur_cell].discharge[0] = 0.0;
    rout_var[cur_cell].nat_discharge[0] = 0.0;
    cshift(rout_var[cur_cell].discharge, 1, options.RIRF_NSTEPS, 1, 1);
    cshift(rout_var[cur_cell].nat_discharge, 1, options.RIRF_NSTEPS, 1, 1);

    inflow = 0;
    for(i = 0; i < rout_con[cur_cell].Nupstream; i++){
        inflow += rout_var[rout_con[cur_cell].upstream[i]].discharge[0];
    }  

    nat_inflow = 0;
    for(i = 0; i < rout_con[cur_cell].Nupstream; i++){
        nat_inflow += rout_var[rout_con[cur_cell].upstream[i]].nat_discharge[0];
    }        

    runoff = 0;
    runoff += (out_data[cur_cell][OUT_RUNOFF][0] + out_data[cur_cell][OUT_BASEFLOW][0]) / 
            MM_PER_M * local_domain.locations[cur_cell].area / global_param.dt;

    rout(inflow, rout_con[cur_cell].river_irf, rout_var[cur_cell].discharge, options.RIRF_NSTEPS);  
    rout(runoff, rout_con[cur_cell].grid_irf, rout_var[cur_cell].discharge, options.GIRF_NSTEPS);  
    rout(nat_inflow, rout_con[cur_cell].river_irf, rout_var[cur_cell].nat_discharge, options.RIRF_NSTEPS);
    rout(runoff, rout_con[cur_cell].grid_irf, rout_var[cur_cell].nat_discharge, options.GIRF_NSTEPS);
}