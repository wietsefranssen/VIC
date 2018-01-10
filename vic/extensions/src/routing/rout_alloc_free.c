#include <ext_driver_shared_image.h>

void
rout_alloc(void)
{
    extern domain_struct local_domain;
    extern ext_option_struct ext_options;
    extern ext_all_vars_struct *ext_all_vars;
    extern rout_con_struct *rout_con;
    extern size_t *routing_order;
    
    size_t i;
    
    rout_con = malloc(local_domain.ncells_active * sizeof(*rout_con));
    check_alloc_status(rout_con,"Memory allocation error");
    
    routing_order = malloc(local_domain.ncells_active * sizeof(*routing_order));
    check_alloc_status(routing_order,"Memory allocation error");
        
    for(i=0; i<local_domain.ncells_active; i++){        
        rout_con[i].river_irf = malloc(ext_options.RIRF_NSTEPS * sizeof(*rout_con[i].river_irf));
        check_alloc_status(rout_con[i].river_irf,"Memory allocation error");
        
        rout_con[i].grid_irf = malloc(ext_options.GIRF_NSTEPS * sizeof(*rout_con[i].grid_irf));
        check_alloc_status(rout_con[i].grid_irf,"Memory allocation error");
        
        ext_all_vars[i].routing.discharge = malloc(ext_options.RIRF_NSTEPS * sizeof(*ext_all_vars[i].routing.discharge));
        check_alloc_status(ext_all_vars[i].routing.discharge,"Memory allocation error");
        
        ext_all_vars[i].routing.nat_discharge = malloc(ext_options.RIRF_NSTEPS * sizeof(*ext_all_vars[i].routing.nat_discharge));
        check_alloc_status(ext_all_vars[i].routing.nat_discharge,"Memory allocation error");
    }   
}

void
rout_finalize(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern rout_con_struct *rout_con;
    extern size_t *routing_order;
    
    size_t i;    
    
    for(i=0; i<local_domain.ncells_active; i++){
        free(rout_con[i].river_irf);
        free(rout_con[i].grid_irf);
        free(rout_con[i].upstream);
        free(ext_all_vars[i].routing.discharge);
        free(ext_all_vars[i].routing.nat_discharge);
    }
    free(rout_con);
    free(routing_order);
}