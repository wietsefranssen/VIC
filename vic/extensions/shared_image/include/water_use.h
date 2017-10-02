#ifndef WATER_USE_H
#define WATER_USE_H

#include <ext_driver_shared_image.h>

enum{
    WU_IRRIGATION,
    WU_DOMESTIC,
    WU_INDUSTRIAL,
    WU_NSECTORS
};

enum{
    WU_RETURN_GROUNDWATER,
    WU_RETURN_SURFACEWATER
};

typedef struct{
    double demand;
    double consumption_factor;
    unsigned short int return_delay;
    unsigned short int compenstation_delay;
    unsigned short int return_location;
}wu_con_struct;

typedef struct{
    double *demand;
    double *consumption_factor;
}wu_hist_struct;

typedef struct{
    double withdrawn;
    double consumed;
    double compensated;
    double returned;
    
    double to_return;
    double to_compensate;
    double *return_flow;
    double *compensation;
}wu_var_struct;

void get_water_use_type(char *cmdstr);

void water_use_init();


#endif

