#include <vic.h>

/******************************************************************************
 * @brief    Read double precision netCDF field from file.
 *****************************************************************************/
int
get_active_nc_field_double(nameid_struct   *nc_nameid,
                    char   *var_name,
                    size_t *start,
                    size_t *count,
                    double *var)
{
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    int status;
    double *dvar = NULL;
    
    dvar =
            malloc(global_domain.ncells_total * sizeof(*dvar));
        check_alloc_status(dvar, "Memory allocation error.");
    
    status = get_nc_field_double(nc_nameid, var_name, start, count, dvar);
    check_nc_status(status, "Error getting values %s for %s", 
            var_name, nc_nameid->nc_filename);
        
    // filter the active cells only
    map(sizeof(double), global_domain.ncells_active, filter_active_cells, NULL,
        dvar, var);
    
    return status;
}

/******************************************************************************
 * @brief    Read single precision netCDF field from file.
 *****************************************************************************/
int
get_active_nc_field_float(nameid_struct   *nc_nameid,
                   char   *var_name,
                   size_t *start,
                   size_t *count,
                   float  *var)
{
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    int status;
    float *fvar = NULL;
    
    fvar =
            malloc(global_domain.ncells_total * sizeof(*fvar));
        check_alloc_status(fvar, "Memory allocation error.");
    
    status = get_nc_field_float(nc_nameid, var_name, start, count, fvar);
    check_nc_status(status, "Error getting values %s for %s", 
            var_name, nc_nameid->nc_filename);
        
    // filter the active cells only
    map(sizeof(float), global_domain.ncells_active, filter_active_cells, NULL,
        fvar, var);
    
    return status;
}

/******************************************************************************
 * @brief    Read integer netCDF field from file.
 *****************************************************************************/
int
get_active_nc_field_int(nameid_struct   *nc_nameid,
                 char   *var_name,
                 size_t *start,
                 size_t *count,
                 int    *var)
{
    extern domain_struct global_domain;
    extern size_t *filter_active_cells;
    
    int status;
    int *ivar = NULL;
    
    ivar =
            malloc(global_domain.ncells_total * sizeof(*ivar));
        check_alloc_status(ivar, "Memory allocation error.");
    
    status = get_nc_field_int(nc_nameid, var_name, start, count, ivar);
    check_nc_status(status, "Error getting values %s for %s", 
            var_name, nc_nameid->nc_filename);
        
    // filter the active cells only
    map(sizeof(int), global_domain.ncells_active, filter_active_cells, 
            NULL,
            ivar, var);
    
    return status;
}
