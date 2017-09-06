
#include <vic_driver_shared_image.h>
#include <ext_shared_image.h>

void sizet_sort(size_t *array, size_t *cost, size_t Nelements, bool acending){
    size_t i,j;
    
    if(acending){
        for(i=0;i<Nelements-1;i++){
            for(j=0;j<Nelements-i-1;j++){
                if(cost[j]>cost[j+1]){
                    sizet_swap(j,j+1,array);
                }
            }
        }
    }else{
        for(i=0;i<Nelements-1;i++){
            for(j=0;j<Nelements-i-1;j++){
                if(cost[j]<cost[j+1]){
                    sizet_swap(j,j+1,array);
                }
            }
        }
    }
}

void sizet_sort2(size_t *array, int *cost, size_t Nelements, bool acending){
    size_t i,j;
    
    if(acending){
        for(i=0;i<Nelements-1;i++){
            for(j=0;j<Nelements-i-1;j++){
                if(cost[j]>cost[j+1]){
                    sizet_swap(j,j+1,array);
                }
            }
        }
    }else{
        for(i=0;i<Nelements-1;i++){
            for(j=0;j<Nelements-i-1;j++){
                if(cost[j]<cost[j+1]){
                    sizet_swap(j,j+1,array);
                }
            }
        }
    }
}

void sizet_swap(size_t i, size_t j, size_t *array){
    size_t hold;
    
    hold = array[i];
    array[i]=array[j];
    array[j]=hold;
}