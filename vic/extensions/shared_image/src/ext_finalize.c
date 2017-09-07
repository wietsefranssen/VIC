#include <ext_driver_shared_image.h>

void ext_finalize()
{
    extern basin_struct basins;
    
    size_t i;
    
    for(i=0;i<basins.Nbasin;i++){
        free(basins.catchment);
    }
    free(basins.basin_map);
    free(basins.Ncells);
    free(basins.sorted_basins);
    free(basins.catchment);
}
