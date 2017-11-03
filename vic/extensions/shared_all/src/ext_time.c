#include <ext_driver_shared_image.h>

double
no_leap_day_in_year_from_dmy(dmy_struct dmy){
    int N1, N2;
    
    N1 = floor(275 * dmy.month / 9);
    
    // 0 if month is before February, else 1
    N2 = floor((dmy.month + 9) / 12);
    
    return N1 - (N2 * 2) - 30 + fractional_day_from_dmy(&dmy);
}

double
day_in_year_from_dmy(dmy_struct dmy){
    int N1, N2, N3;
    
    N1 = floor(275 * dmy.month / 9);
    
    // 0 if month is before February, else 1
    N2 = floor((dmy.month + 9) / 12);
    
    // 2 if not a leap year, else 1
    N3 = (1 + floor((dmy.year - 4 * floor(dmy.year / 4) + 2) / 3));
    
    return N1 - (N2 * N3) - 30 + fractional_day_from_dmy(&dmy);
}

void
dmy_from_day_in_year(double day_in_year, int year, dmy_struct *dmy){
    
    if(day_in_year < 1){
        log_err("Day in year can not be smaller than 1");
    }    
    day_in_year = fmod(day_in_year, DAYS_PER_YEAR);
    day_in_year--;
    
    dmy_no_leap_day(day_in_year, dmy);
    
    dmy->year = year;
    dmy->dayseconds = fmod(day_in_year, 1) * SEC_PER_DAY;
    dmy->day_in_year = day_in_year;
}