#include <vic.h>

void
rout_alloc(void)
{
    extern domain_struct    global_domain;
    extern domain_struct    local_domain;
    extern option_struct    options;
    extern rout_var_struct *rout_var;
    extern rout_con_struct *rout_con;
    extern size_t          *routing_order;

    size_t                  i;

    rout_var = malloc(local_domain.ncells_active * sizeof(*rout_var));
    check_alloc_status(rout_var, "Memory allocation error");

    rout_con = malloc(local_domain.ncells_active * sizeof(*rout_con));
    check_alloc_status(rout_con, "Memory allocation error");

    if (options.ROUTING_TYPE == ROUTING_BASIN) {
        routing_order =
            malloc(local_domain.ncells_active * sizeof(*routing_order));
        check_alloc_status(routing_order, "Memory allocation error");
    }
    else if (options.ROUTING_TYPE == ROUTING_RANDOM) {
        routing_order =
            malloc(global_domain.ncells_active * sizeof(*routing_order));
        check_alloc_status(routing_order, "Memory allocation error");
    }

    for (i = 0; i < local_domain.ncells_active; i++) {
        rout_con[i].river_irf =
            malloc(options.RIRF_NSTEPS * sizeof(*rout_con[i].river_irf));
        check_alloc_status(rout_con[i].river_irf, "Memory allocation error");

        rout_con[i].grid_irf =
            malloc(options.GIRF_NSTEPS * sizeof(*rout_con[i].grid_irf));
        check_alloc_status(rout_con[i].grid_irf, "Memory allocation error");

        rout_var[i].discharge =
            malloc(options.RIRF_NSTEPS * sizeof(*rout_var[i].discharge));
        check_alloc_status(rout_var[i].discharge, "Memory allocation error");

        rout_var[i].nat_discharge =
            malloc(options.RIRF_NSTEPS * sizeof(*rout_var[i].nat_discharge));
        check_alloc_status(rout_var[i].nat_discharge, "Memory allocation error");
    }
}

void
rout_finalize(void)
{
    extern domain_struct    local_domain;
    extern rout_var_struct *rout_var;
    extern rout_con_struct *rout_con;
    extern size_t          *routing_order;

    size_t                  i;

    for (i = 0; i < local_domain.ncells_active; i++) {
        free(rout_con[i].river_irf);
        free(rout_con[i].grid_irf);
        free(rout_con[i].upstream);
        free(rout_var[i].discharge);
        free(rout_var[i].nat_discharge);
    }
    free(rout_var);
    free(rout_con);
    free(routing_order);
}
