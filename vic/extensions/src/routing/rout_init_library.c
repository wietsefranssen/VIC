#include <ext_driver_shared_image.h>

void
initialize_rout_var(rout_var_struct *rout_var)
{
    extern ext_option_struct ext_options;
    
    size_t i;
    
    for(i=0; i<ext_options.UH_NSTEPS; i++){
        rout_var->discharge[i] = 0.0;
        rout_var->nat_discharge[i] = 0.0;
    }
}

void
initialize_rout_con(rout_con_struct *rout_con)
{
    size_t i;
    
    rout_con->Nupstream = 0;
    rout_con->direction = -1;
    rout_con->downstream = MISSING_USI;
    
    rout_con->upstream = NULL;
    
    for(i=0; i<ext_options.UH_NSTEPS; i++){
        rout_con->uh[i] = 0.0;
    }
}

void
initialize_rout_local_structures(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern rout_con_struct *rout_con;
    
    size_t i;
    
    for(i=0; i < local_domain.ncells_active; i++){
        initialize_rout_con(&rout_con[i]);
        initialize_rout_var(&(ext_all_vars[i].routing));
    }
}