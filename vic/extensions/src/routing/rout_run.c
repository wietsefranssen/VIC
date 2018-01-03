#include <ext_driver_shared_image.h>

void
rout_run(void)
{
    extern domain_struct local_domain;
    extern ext_option_struct ext_options;
    extern ext_all_vars_struct *ext_all_vars;
    extern rout_con_struct *rout_con;
    extern size_t *rout_order;
    extern double ***out_data;
    
    double inflow;
    double nat_inflow;
    double runoff;
    
    size_t cur_cell;
    
    size_t i;
    size_t j;
    
    for(i = 0; i < local_domain.ncells_active; i++){
        cur_cell = rout_order[i];
        
        inflow = 0;
        for(j = 0; j < rout_con[cur_cell].Nupstream; j++){
            inflow += ext_all_vars[rout_con[cur_cell].upstream[i]].routing.discharge[0];
        }  
        
        nat_inflow = 0;
        for(j = 0; j < rout_con[cur_cell].Nupstream; j++){
            nat_inflow += ext_all_vars[rout_con[cur_cell].upstream[i]].routing.nat_discharge[0];
        }        
        
        runoff = 0;
        runoff += out_data[cur_cell][OUT_RUNOFF][0] + out_data[cur_cell][OUT_BASEFLOW][0];
        
        rout((runoff + inflow), rout_con[cur_cell].uh, ext_all_vars[cur_cell].routing.discharge, ext_options.UH_NSTEPS);  
        rout((runoff + nat_inflow), rout_con[cur_cell].uh, ext_all_vars[cur_cell].routing.nat_discharge, ext_options.UH_NSTEPS);   
    }    
}