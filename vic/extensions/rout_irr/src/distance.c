/******************************************************************************
 * @section DESCRIPTION
 *  
 * Calculate euclidian distance between two points
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Calculate euclidian distance between two points
 ******************************************************************************/
double distance(size_t from_x, size_t from_y, size_t to_x, size_t to_y){
    double a = (double)(from_x - to_x);
    double b = (double)(from_y - to_y);
    return(sqrt(pow(a,2)+pow(b,2)));
}

/******************************************************************************
 * @section brief
 *  
 * returns 1 if year is a leap year, else zero
 ******************************************************************************/
int is_leap_year(int year){
    if((year % 4 == 0) || ((year % 100 == 0) && (year % 400 == 0))){
        return 1;
    }
    return 0;
}