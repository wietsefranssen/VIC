#include <rout.h>

double distance(size_t from_x, size_t from_y, size_t to_x, size_t to_y){
    //Calculate the euclidian distance between coordinates
    double a = (double)(from_x - to_x);
    double b = (double)(from_y - to_y);
    return(sqrt(pow(a,2)+pow(b,2)));
}