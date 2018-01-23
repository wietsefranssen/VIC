#include <vic.h>

/******************************************************************************
 * @brief    Read double precision netCDF field from file.
 *****************************************************************************/
void
get_active_nc_field_double(nameid_struct   *nc_nameid,
                    char   *var_name,
                    size_t *start,
                    size_t *count,
                    double *var)
{
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    double *dvar = NULL;
    
    dvar =
            malloc(global_domain.ncells_total * sizeof(*dvar));
        check_alloc_status(dvar, "Memory allocation error.");
    
    get_nc_field_double(nc_nameid, var_name, start, count, dvar);
        
    // filter the active cells only
    map(sizeof(double), global_domain.ncells_active, filter_active_cells, NULL,
        dvar, var);
}

/******************************************************************************
 * @brief    Read single precision netCDF field from file.
 *****************************************************************************/
void
get_active_nc_field_float(nameid_struct   *nc_nameid,
                   char   *var_name,
                   size_t *start,
                   size_t *count,
                   float  *var)
{
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    float *fvar = NULL;
    
    fvar =
            malloc(global_domain.ncells_total * sizeof(*fvar));
        check_alloc_status(fvar, "Memory allocation error.");
    
    get_nc_field_float(nc_nameid, var_name, start, count, fvar);
        
    // filter the active cells only
    map(sizeof(float), global_domain.ncells_active, filter_active_cells, NULL,
        fvar, var);
}

/******************************************************************************
 * @brief    Read integer netCDF field from file.
 *****************************************************************************/
void
get_active_nc_field_int(nameid_struct   *nc_nameid,
                 char   *var_name,
                 size_t *start,
                 size_t *count,
                 int    *var)
{
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    int *ivar = NULL;
    
    ivar =
            malloc(global_domain.ncells_total * sizeof(*ivar));
        check_alloc_status(ivar, "Memory allocation error.");
    
    get_nc_field_int(nc_nameid, var_name, start, count, ivar);
        
    // filter the active cells only
    map(sizeof(int), global_domain.ncells_active, filter_active_cells, 
            NULL,
            ivar, var);
}
