#include <ext_driver_shared_image.h>

void
ext_set_nc_output_file_info(nc_file_struct *nc_output_file)
{
    extern ext_option_struct ext_options;
    
    if(ext_options.WATER_USE){
        wu_set_nc_output_file_info(nc_output_file);
    }
}

void
ext_write_def_dim(nc_file_struct *nc_output_file, stream_struct *stream)
{
    extern ext_option_struct ext_options;
    
    if(ext_options.WATER_USE){
        wu_write_def_dim(nc_output_file, stream);
    }    
}

void
ext_write_def_dimvar(nc_file_struct *nc_output_file, stream_struct *stream)
{
    extern ext_option_struct ext_options;
    
    if(ext_options.WATER_USE){
        wu_write_def_dimvar(nc_output_file);
    }    
}

void
ext_write_put_dimvar(nc_file_struct *nc_output_file, stream_struct *stream)
{
    extern ext_option_struct ext_options;
    
    if(ext_options.WATER_USE){
        wu_write_put_dimvar(nc_output_file);
    }    
}