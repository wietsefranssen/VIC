#include <vic_def.h>

bool
between_dmy(dmy_struct start, dmy_struct end, dmy_struct current)
{
    if((start.month < end.month) || 
            (start.month == end.month && start.day < end.day) ||
            (start.month == end.month && start.day == end.day && start.dayseconds < end.dayseconds)){
        if((current.month > end.month) ||
                (current.month == end.month && current.day > end.day) ||
                (current.month == end.month && current.day == end.day && current.dayseconds > end.dayseconds)){
            return false;
        }else if((current.month > start.month) ||
                (current.month == start.month && current.day > start.day) ||
                (current.month == start.month && current.day == start.day && current.dayseconds > start.dayseconds)){
            return true;
        }else{
            return false;
        }
    }else{
        if((current.month > start.month) ||
                (current.month == start.month && current.day > start.day) ||
                (current.month == start.month && current.day == start.day && current.dayseconds > start.dayseconds)){
            return true;
        }else if((current.month > end.month) ||
                (current.month == end.month && current.day > end.day) ||
                (current.month == end.month && current.day == end.day && current.dayseconds > end.dayseconds)){
            return false;
        }else{
            return true;
        }
    }
}

dmy_struct 
dmy_from_diy(int diy)
{
    
}