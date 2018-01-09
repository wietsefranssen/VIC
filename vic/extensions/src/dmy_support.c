#include <vic_def.h>

bool
between_dmy(dmy_struct start, dmy_struct end, dmy_struct current)
{
    if((start.day_in_year < end.day_in_year) || 
            (start.day_in_year == end.day_in_year && start.dayseconds < end.dayseconds)){
        if((current.day_in_year > end.day_in_year) ||
                (current.day_in_year == end.day_in_year && current.dayseconds > end.dayseconds)){
            return false;
        }else if((current.day_in_year > start.day_in_year) ||
                (current.day_in_year == start.day_in_year && current.dayseconds > start.dayseconds)){
            return true;
        }else{
            return false;
        }
    }else{
        if((current.day_in_year > start.day_in_year) ||
                (current.day_in_year == start.day_in_year && current.dayseconds > start.dayseconds)){
            return true;
        }else if((current.day_in_year > end.day_in_year) ||
                (current.day_in_year == end.day_in_year && current.dayseconds > end.dayseconds)){
            return false;
        }else{
            return true;
        }
    }
}