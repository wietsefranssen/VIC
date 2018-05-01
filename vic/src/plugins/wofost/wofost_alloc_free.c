#include <vic.h>

void
wofost_alloc(void)
{
    extern domain_struct       local_domain;
    extern wofost_simUnit                  **wofost_var;
   
    // Allocate cells
    wofost_var = malloc(local_domain.ncells_active * sizeof(*wofost_var));
    check_alloc_status(wofost_var, "Memory allocation error");    
}

void
wofost_finalize(void)
{
}