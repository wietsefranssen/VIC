#include <vic.h>

void
efr_alloc(void)
{
    extern domain_struct   local_domain;
    extern efr_var_struct *efr_var;

    efr_var = malloc(local_domain.ncells_active * sizeof(*efr_var));
    check_alloc_status(efr_var, "Memory allocation error");
}

void
efr_finalize(void)
{
    extern efr_var_struct *efr_var;

    free(efr_var);
}
