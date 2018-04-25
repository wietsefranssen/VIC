#include <vic.h>

//void
//initialize_wofost_var(wofost_var_struct *wofost_var)
//{    
////    wofost_var->pond_storage = 0.0;
////    wofost_var->leftover = 0.0;
////    wofost_var->prev_req = 0.0;
////    wofost_var->prev_store = 0.0;
////    wofost_var->prev_short = 0.0;
////    wofost_var->requirement = 0.0;
////    wofost_var->need = 0.0;
////    wofost_var->deficit = 0.0;
////    wofost_var->shortage = 0.0;
//}

//void
//initialize_wofost_con(wofost_con_struct *wofost_con)
//{
////    size_t i;
////    
////    wofost_con->veg_index = NODATA_VEG;
////    wofost_con->ponding = false;
////    wofost_con->pond_capacity = 0.0;
////    wofost_con->season_offset = 0.0;
////    for(i = 0; i < wofost_con->nseasons; i++){
////        wofost_con->season_start[i] = 0.0;
////        wofost_con->season_end[i] = 0.0;
////    }
//}

void
initialize_wofost_local_structures(void)
{
//    extern domain_struct local_domain;
//    extern option_struct options;
//    extern wofost_var_struct ***wofost_var;
//    extern wofost_con_map_struct *wofost_con_map;
//    extern wofost_con_struct **wofost_con;
//    
//    size_t i;
//    size_t j;
//    size_t k;
//    
//    for(i=0; i < local_domain.ncells_active; i++){        
//        for(j = 0; j < wofost_con_map[i].ni_active; j++){
//            initialize_wofost_con(&wofost_con[i][j]);
//        
//            for(k=0; k < options.SNOW_BAND; k++){
//                initialize_wofost_var(&wofost_var[i][j][k]);
//            }
//        }
//    }
}