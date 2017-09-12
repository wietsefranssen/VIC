#include <ext_driver_shared_image.h>

void
routing_alloc()
{    
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern rout_con_struct *rout_con;
    extern ext_all_vars_struct *ext_all_vars;
    extern size_t *cell_order;
    extern int mpi_rank;
    extern int mpi_decomposition;
       
    size_t i;
    size_t j;
    
    if(mpi_decomposition==BASIN_DECOMPOSITION){
        rout_con = malloc(local_domain.ncells_active * sizeof(*rout_con));
        check_alloc_status(rout_con, "Memory allocation error");
        
        for(i=0;i<local_domain.ncells_active;i++){
            rout_con[i].uh = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*rout_con[i].uh));
            check_alloc_status(rout_con[i].uh, "Memory allocation error");
            ext_all_vars[i].rout_var.discharge = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*ext_all_vars[i].rout_var.discharge));
            check_alloc_status(ext_all_vars[i].rout_var.discharge, "Memory allocation error");
        }
        
        cell_order = malloc(local_domain.ncells_active * sizeof(*cell_order));
        check_alloc_status(cell_order, "Memory allocation error");
        
    } else if(mpi_decomposition==RANDOM_DECOMPOSITION && 
            mpi_rank==VIC_MPI_ROOT){
        rout_con = malloc(global_domain.ncells_active * sizeof(*rout_con));
        check_alloc_status(rout_con, "Memory allocation error");
                
        for(i=0;i<global_domain.ncells_active;i++){
            rout_con[i].uh = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*rout_con[i].uh));
            check_alloc_status(rout_con[i].uh, "Memory allocation error");
            ext_all_vars[i].rout_var.discharge = malloc(global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH * sizeof(*ext_all_vars[i].rout_var.discharge));
            check_alloc_status(ext_all_vars[i].rout_var.discharge, "Memory allocation error");
        }
        
        cell_order = malloc(global_domain.ncells_active * sizeof(*cell_order));
        check_alloc_status(cell_order, "Memory allocation error");
    }
    
    if(mpi_decomposition==BASIN_DECOMPOSITION){
        
        for(i=0;i<local_domain.ncells_active;i++){
            rout_con[i].Nupstream=0;
            rout_con[i].rank=0;
            rout_con[i].direction=NODATA_DIRECTION;
            rout_con[i].downstream=MISSING_USI;
            
            for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
                rout_con[i].uh[j]=0.0;
                ext_all_vars[i].rout_var.discharge[j] = 0.0;                
            }
            
            cell_order[i] = MISSING_USI;
        }
    } else if(mpi_decomposition==RANDOM_DECOMPOSITION && 
            mpi_rank==VIC_MPI_ROOT){
        
        for(i=0;i<global_domain.ncells_active;i++){
            rout_con[i].Nupstream=0;
            rout_con[i].rank=0;
            rout_con[i].direction=NODATA_DIRECTION;
            rout_con[i].downstream=MISSING_USI;
            
            for(j=0;j<global_param.model_steps_per_day * ext_param.UH_MAX_LENGTH;j++){
                rout_con[i].uh[j]=0.0;
                ext_all_vars[i].rout_var.discharge[j] = 0.0;                
            }
            
            cell_order[i] = MISSING_USI;
        }
    }
}

void
mpi_alloc()
{
    extern basin_struct basins;
    
    basins.Nbasin = 0;
}

void
ext_alloc(void)
{    
    extern ext_all_vars_struct *ext_all_vars;
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern int mpi_rank;
    extern int mpi_decomposition;
        
    if(mpi_decomposition==BASIN_DECOMPOSITION){
        ext_all_vars = malloc(local_domain.ncells_active * sizeof(*ext_all_vars));
        check_alloc_status(ext_all_vars, "Memory allocation error");        
    }else if(mpi_decomposition==RANDOM_DECOMPOSITION && 
            mpi_rank==VIC_MPI_ROOT){
        ext_all_vars = malloc(global_domain.ncells_active * sizeof(*ext_all_vars));
        check_alloc_status(ext_all_vars, "Memory allocation error");        
    }
    
    mpi_alloc();
    routing_alloc();
}