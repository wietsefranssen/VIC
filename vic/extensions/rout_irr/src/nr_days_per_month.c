/******************************************************************************
 * @section DESCRIPTION
 *  
 * Calculate number of day in a specific month and year
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Calculate number of day in a specific month and year
 ******************************************************************************/
int nr_days_in_month(int month, int year){
    if(month<=0){
        return 31;
    }
    if(month == 2){
        return 28 + is_leap_year(year);
    }
    return 31 - (month-1) % 7 % 2;
}

