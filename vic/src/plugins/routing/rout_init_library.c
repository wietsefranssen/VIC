#include <vic.h>

void
initialize_rout_var(rout_var_struct *rout_var)
{
    extern option_struct options;

    size_t               i;

    rout_var->storage = 0.0;
    for (i = 0; i < options.RIRF_NSTEPS; i++) {
        rout_var->discharge[i] = 0.0;
        rout_var->nat_discharge[i] = 0.0;
    }
}

void
initialize_rout_con(rout_con_struct *rout_con)
{
    extern option_struct options;

    size_t               i;

    rout_con->Nupstream = 0;
    rout_con->direction = -1;
    rout_con->downstream = MISSING_USI;

    rout_con->upstream = NULL;

    for (i = 0; i < options.RIRF_NSTEPS; i++) {
        rout_con->river_irf[i] = 0.0;
    }
    for (i = 0; i < options.GIRF_NSTEPS; i++) {
        rout_con->grid_irf[i] = 0.0;
    }
}

void
initialize_rout_local_structures(void)
{
    extern domain_struct    local_domain;
    extern rout_var_struct *rout_var;
    extern rout_con_struct *rout_con;

    size_t                  i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        initialize_rout_con(&rout_con[i]);
        initialize_rout_var(&(rout_var[i]));
    }
}
