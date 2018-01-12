#include <ext_driver_shared_image.h>

bool 
dam_set_nc_var_info(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file)
{
    
    switch (varid) {        
    case OUT_DAM_VOLUME: 
    case OUT_DAM_DISCHARGE:   
    case OUT_DAM_AREA:   
    case OUT_DAM_HEIGHT:   
    case OUT_DAM_OP_DISCHARGE:  
    case OUT_DAM_OP_VOLUME:   
    case OUT_DAM_OP_MONTH:  
        nc_var->nc_dims = 4;
        nc_var->nc_counts[1] = nc_file->dam_size;
        nc_var->nc_counts[2] = nc_file->nj_size;
        nc_var->nc_counts[3] = nc_file->ni_size;
        return true;
        break;
    }
    
    return false;
}

bool 
dam_set_nc_var_dimids(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file)
{
    
    switch (varid) {        
    case OUT_DAM_VOLUME:  
    case OUT_DAM_DISCHARGE:  
    case OUT_DAM_AREA:   
    case OUT_DAM_HEIGHT:   
    case OUT_DAM_OP_DISCHARGE: 
    case OUT_DAM_OP_VOLUME:  
    case OUT_DAM_OP_MONTH:  
        nc_var->nc_dimids[0] = nc_file->time_dimid;
        nc_var->nc_dimids[1] = nc_file->dam_dimid;
        nc_var->nc_dimids[2] = nc_file->nj_dimid;
        nc_var->nc_dimids[3] = nc_file->ni_dimid;
        return true;
        break;
    }
    
    return false;
}