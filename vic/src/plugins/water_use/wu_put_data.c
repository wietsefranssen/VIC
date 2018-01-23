#include <vic.h>

void
wu_put_data(void)
{
    extern domain_struct local_domain;
    extern wu_var_struct **wu_var;
    extern double ***out_data;
    extern node            *outvar_types;
    
    size_t i;    
    size_t j;
    
    int OUT_WU_DEMAND = list_search_id(outvar_types, "OUT_WU_DEMAND");
    int OUT_WU_COMPENSATION = list_search_id(outvar_types, "OUT_WU_COMPENSATION");
    int OUT_WU_COMPENSATED = list_search_id(outvar_types, "OUT_WU_COMPENSATED");
    int OUT_WU_WITHDRAWN = list_search_id(outvar_types, "OUT_WU_WITHDRAWN");
    int OUT_WU_CONSUMED = list_search_id(outvar_types, "OUT_WU_CONSUMED");
    int OUT_WU_RETURNED = list_search_id(outvar_types, "OUT_WU_RETURNED");
    int OUT_WU_SHORTAGE = list_search_id(outvar_types, "OUT_WU_SHORTAGE");
    
    for(i = 0; i < local_domain.ncells_active; i++){ 
        for(j = 0; j < WU_NSECTORS; j++){
            out_data[i][OUT_WU_DEMAND][j] = wu_var[i][j].demand;
            out_data[i][OUT_WU_COMPENSATION][j] = wu_var[i][j].compensation_total;
            out_data[i][OUT_WU_COMPENSATED][j] = wu_var[i][j].compensated;
            out_data[i][OUT_WU_WITHDRAWN][j] = wu_var[i][j].withdrawn;
            out_data[i][OUT_WU_CONSUMED][j] = wu_var[i][j].consumed;
            out_data[i][OUT_WU_RETURNED][j] = wu_var[i][j].returned;
            out_data[i][OUT_WU_SHORTAGE][j] = wu_var[i][j].compensation[0];
        }
    }    
}