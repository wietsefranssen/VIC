#include <ext_driver_shared_image.h>

bool
ext_get_global_param(char *cmdstr)
{
    if(gw_get_global_param(cmdstr)){
        return true;
    }else{
        return false;
    }
}

void
ext_check_global_param(void)
{
    gw_check_global_param();
}