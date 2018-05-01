#include <vic.h>

void
wofost_put_data(void) {
    extern domain_struct local_domain;
    extern wofost_simUnit **wofost_var;

    extern double ***out_data;
    extern node *outvar_types;

    size_t i;


    int OUT_WOFOST_STEMS = list_search_id(
            outvar_types, "OUT_WOFOST_STEMS");

    for (i = 0; i < local_domain.ncells_active; i++) {
        out_data[i][OUT_WOFOST_STEMS][0] +=
                wofost_var[i]->soil->VolumetricSoilMoisture->y;
    }
}
