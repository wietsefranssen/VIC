#include <ext_driver_shared_image.h>

void
dam_set_nd_active(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern ext_filenames_struct ext_filenames;
    extern dam_con_map_struct *dam_con_map;
        
    int *ivar;
    
    size_t i;
    
    size_t  d2count[2];
    size_t  d2start[2];
    
    // Get active dams  
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx; 
    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error."); 
        
    get_scatter_nc_field_int(&(ext_filenames.dams), 
            ext_filenames.info.ndam, d2start, d2count, ivar);
    for (i = 0; i < local_domain.ncells_active; i++) {
        dam_con_map[i].nd_active = ivar[i];
    }
    
    free(ivar);
}

void
dam_alloc(void)
{
    extern domain_struct local_domain;
    extern ext_filenames_struct ext_filenames;
    extern ext_all_vars_struct *ext_all_vars;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    extern int mpi_rank;
    
    int status;
    
    size_t i;
    size_t j;
    
    // open parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_open(ext_filenames.dams.nc_filename, NC_NOWRITE,
                         &(ext_filenames.dams.nc_id));
        check_nc_status(status, "Error opening %s",
                        ext_filenames.dams.nc_filename);
    }
            
    // Allocate cells
    dam_con_map = malloc(local_domain.ncells_active * sizeof(*dam_con_map));
    check_alloc_status(dam_con_map,"Memory allocation error");
    dam_con = malloc(local_domain.ncells_active * sizeof(*dam_con));
    check_alloc_status(dam_con,"Memory allocation error");
    
    dam_set_nd_active();
     
    for(i=0; i<local_domain.ncells_active; i++){
        dam_con[i] = malloc(dam_con_map[i].nd_active * sizeof(*dam_con[i]));
        check_alloc_status(dam_con[i],"Memory allocation error");
        ext_all_vars[i].dams = malloc(dam_con_map[i].nd_active * sizeof(*ext_all_vars[i].dams));
        check_alloc_status(ext_all_vars[i].dams,"Memory allocation error"); 
        for(j=0;j<dam_con_map[i].nd_active;j++){
//            ext_all_vars[i].dams[j].history_flow = malloc(DAM_HIST_YEARS * MONTHS_PER_YEAR * sizeof(*ext_all_vars[i].dams[j].history_flow));
//            check_alloc_status(ext_all_vars[i].dams[j].history_flow,"Memory allocation error");
        }   
    }   

    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
        status = nc_close(ext_filenames.dams.nc_id);
        check_nc_status(status, "Error closing %s",
                        ext_filenames.dams.nc_filename);
    }
}

void
dam_finalize(void)
{
    extern domain_struct local_domain;
    extern ext_all_vars_struct *ext_all_vars;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    size_t i;    
    size_t j;
            
    for(i=0; i<local_domain.ncells_active; i++){        
        for(j=0;j<dam_con_map[i].nd_active;j++){
//            free(ext_all_vars[i].dams[j].history_flow);
        }
        free(ext_all_vars[i].dams);
        free(dam_con[i]);
    }
    free(dam_con_map);
    free(dam_con);
}