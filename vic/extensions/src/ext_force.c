#include <ext_driver_shared_image.h>

void
ext_force()
{
    extern ext_option_struct ext_options;
    
    if(ext_options.WATER_USE){
        wu_force();
    }
}