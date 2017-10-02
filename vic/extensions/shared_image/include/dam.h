#include <ext_driver_shared_image.h>

#ifndef DAM_H
#define DAM_H

typedef struct{
    
}dam_var_struct;

typedef struct{
    char name[MAXSTRING];
    unsigned short int year;
    
}dam_con_struct;

typedef struct{
    unsigned short int ndams;
    unsigned short int *didx;
}dam_con_map_struct;

#endif /* DAM_H */

