#include <ext_driver_shared_image.h>

void
gw_set_aquifer(void)
{
    extern domain_struct       local_domain;
    extern domain_struct       global_domain;
    extern ext_filenames_struct ext_filenames;
    extern gw_con_struct      *gw_con;
    
    double                    *dvar = NULL;
    
    size_t                     d2count[2];
    size_t                     d2start[2];
    
    size_t i;
    
    // allocate memory for variables to be read
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;
    
    get_scatter_nc_field_double(&(ext_filenames.groundwater),
            ext_filenames.info.Ka_expt, d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        gw_con[i].Ka_expt = dvar[i];
    }
    
    get_scatter_nc_field_double(&(ext_filenames.groundwater),
            ext_filenames.info.Sy, d2start, d2count, dvar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        gw_con[i].Sy = dvar[i];
    }
        
    free(dvar);  
}

void
gw_set_maximum_baseflow(void)
{
    extern domain_struct       local_domain;
    extern soil_con_struct    *soil_con;
    extern gw_con_struct      *gw_con;
    extern option_struct        options;
    
    size_t i;
    size_t j;
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < options.Nlayer; j++){
            if(gw_con[i].Qb_max < soil_con[i].Ksat[j]){
                gw_con[i].Qb_max = soil_con[i].Ksat[j];
            }
        }
    }
    
}

void
gw_init(void)
{
    extern ext_filenames_struct ext_filenames;
    extern int mpi_rank;
    
    int status;
    
    // open parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_open(ext_filenames.groundwater.nc_filename, NC_NOWRITE,
                         &(ext_filenames.groundwater.nc_id));
        check_nc_status(status, "Error opening %s",
                        ext_filenames.groundwater.nc_filename);
    }
    
    gw_set_aquifer();
    gw_set_maximum_baseflow();    
    
    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(ext_filenames.groundwater.nc_id);
        check_nc_status(status, "Error closing %s",
                        ext_filenames.groundwater.nc_filename);
    } 
}