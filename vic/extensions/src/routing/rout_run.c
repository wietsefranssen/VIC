#include <ext_driver_shared_image.h>

void
rout_run(size_t cur_cell)
{
    extern domain_struct local_domain;
    extern global_param_struct global_param;
    extern ext_option_struct ext_options;
    extern ext_all_vars_struct *ext_all_vars;
    extern rout_con_struct *rout_con;
    extern double ***out_data;
    
    double inflow;
    double nat_inflow;
    double runoff;
    
    size_t i;
                  
    ext_all_vars[cur_cell].routing.discharge[0] = 0.0;
    ext_all_vars[cur_cell].routing.nat_discharge[0] = 0.0;
    cshift(ext_all_vars[cur_cell].routing.discharge, 1, ext_options.UH_NSTEPS, 1, 1);
    cshift(ext_all_vars[cur_cell].routing.nat_discharge, 1, ext_options.UH_NSTEPS, 1, 1);

    inflow = 0;
    for(i = 0; i < rout_con[cur_cell].Nupstream; i++){
        inflow += ext_all_vars[rout_con[cur_cell].upstream[i]].routing.discharge[0];
    }  

    nat_inflow = 0;
    for(i = 0; i < rout_con[cur_cell].Nupstream; i++){
        nat_inflow += ext_all_vars[rout_con[cur_cell].upstream[i]].routing.nat_discharge[0];
    }        

    runoff = 0;
    runoff += (out_data[cur_cell][OUT_RUNOFF][0] + out_data[cur_cell][OUT_BASEFLOW][0]) / 
            MM_PER_M * local_domain.locations[cur_cell].area / global_param.dt;

    rout((runoff + inflow), rout_con[cur_cell].uh, ext_all_vars[cur_cell].routing.discharge, ext_options.UH_NSTEPS);  
    rout((runoff + nat_inflow), rout_con[cur_cell].uh, ext_all_vars[cur_cell].routing.nat_discharge, ext_options.UH_NSTEPS);
}