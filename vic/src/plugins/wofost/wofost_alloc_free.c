#include <vic.h>

void
wofost_alloc(void)
{
//    extern domain_struct local_domain;
//    extern option_struct options;
//    extern option_struct options;
//    extern filenames_struct filenames;
//    extern irr_var_struct ***irr_var;
//    extern irr_con_map_struct *irr_con_map;
//    extern irr_con_struct **irr_con;
    extern int mpi_rank;
    
//    int status;
//    
//    size_t i;
//    size_t j;
    
//    // open parameter file
//    if(mpi_rank == VIC_MPI_ROOT){
//        status = nc_open(filenames.irrigation.nc_filename, NC_NOWRITE,
//                         &(filenames.irrigation.nc_id));
//        check_nc_status(status, "Error opening %s",
//                        filenames.irrigation.nc_filename);
//    }
//            
//    // Allocate cells
//    irr_con_map = malloc(local_domain.ncells_active * sizeof(*irr_con_map));
//    check_alloc_status(irr_con_map,"Memory allocation error");
//    irr_con = malloc(local_domain.ncells_active * sizeof(*irr_con));
//    check_alloc_status(irr_con,"Memory allocation error");

    // close parameter file
    if(mpi_rank == VIC_MPI_ROOT){
//        status = nc_close(filenames.irrigation.nc_id);
//        check_nc_status(status, "Error closing %s",
//                        filenames.irrigation.nc_filename);
    }
}

void
wofost_finalize(void)
{
//    extern domain_struct local_domain;
//    extern irr_var_struct ***irr_var;
//    extern irr_con_map_struct *irr_con_map;
//    extern irr_con_struct **irr_con;
//    
//    size_t i;    
//    size_t j;
//            
//    for(i=0; i<local_domain.ncells_active; i++){        
//        for(j=0;j<irr_con_map[i].ni_active;j++){
//            free(irr_var[i][j]);
//            free(irr_con[i][j].season_start);
//            free(irr_con[i][j].season_end);
//        }
//        free(irr_var[i]);
//        free(irr_con_map[i].vidx);
//        free(irr_con_map[i].iidx);
//        free(irr_con[i]);
//    }
//    free(irr_con_map);
//    free(irr_con);
//    free(irr_var);
}