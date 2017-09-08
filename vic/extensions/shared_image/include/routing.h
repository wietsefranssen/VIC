#ifndef ROUTING_H
#define ROUTING_H

#include <ext_driver_shared_image.h>

typedef struct{
    short unsigned int direction;
    short unsigned int Nupstream;
    size_t downstream;
    size_t *upstream;
}rout_con_struct;

void routing_init(char *nc_name);

#endif

