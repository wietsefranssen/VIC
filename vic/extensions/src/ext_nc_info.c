#include <ext_driver_shared_image.h>

bool 
ext_set_nc_var_info(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file)
{
    extern ext_option_struct ext_options;
    
    if(ext_options.WATER_USE){
        if(wu_set_nc_var_info(varid, nc_var, nc_file)){
            return true;
        }
    }
    return false;
}

bool
ext_set_nc_var_dimids(int varid, nc_var_struct *nc_var, nc_file_struct *nc_file)
{
    extern ext_option_struct ext_options;
    
    if(ext_options.WATER_USE){
        if(wu_set_nc_var_dimids(varid, nc_var, nc_file)){
            return true;
        }
    }
    return false;
}