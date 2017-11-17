#include <ext_driver_shared_image.h>

void
gw_init(void)
{
    extern domain_struct       local_domain;
    extern domain_struct       global_domain;
    extern gw_con_struct      *gw_con;
    extern ext_filenames_struct ext_filenames;
    
    double                    *dvar = NULL;
    int                       *ivar = NULL;
    
    size_t                     d2count[2];
    size_t                     d2start[2];
    size_t                     d3count[3];
    size_t                     d3start[3];
    size_t                     d4count[4];
    size_t                     d4start[4];
    
    size_t i;
    
    // allocate memory for variables to be read
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx;
    
    
//    get_scatter_nc_field_double(&(ext_filenames.groundwater), "Qb_max",
//                             d3start, d3count, dvar);
//    for (i = 0; i < local_domain.ncells_active; i++) {
//        gw_con[i].Qb_max = dvar[i];
//    }
//    
//    get_scatter_nc_field_double(&(ext_filenames.groundwater), "Qb_expt",
//                             d3start, d3count, dvar);
//    for (i = 0; i < local_domain.ncells_active; i++) {
//        gw_con[i].Qb_expt = dvar[i];
//    }
//    
//    get_scatter_nc_field_double(&(ext_filenames.groundwater), "Ka_expt",
//                             d3start, d3count, dvar);
//    for (i = 0; i < local_domain.ncells_active; i++) {
//        gw_con[i].Ka_expt = dvar[i];
//    }
//    
//    get_scatter_nc_field_double(&(ext_filenames.groundwater), "Fp_expt",
//                             d3start, d3count, dvar);
//    for (i = 0; i < local_domain.ncells_active; i++) {
//        gw_con[i].Fp_expt = dvar[i];
//    }
//    
//    get_scatter_nc_field_double(&(ext_filenames.groundwater), "Sy",
//                             d3start, d3count, dvar);
//    for (i = 0; i < local_domain.ncells_active; i++) {
//        gw_con[i].Sy = dvar[i];
//    }
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        gw_con[i].Fp_expt = 3.0;
        gw_con[i].Ka_expt = 3.0;
        gw_con[i].Qb_expt = 0.3;
        gw_con[i].Qb_max = 5.0;
        gw_con[i].Sy = 0.5;
    }
    
    free(dvar);
    free(ivar);    
}