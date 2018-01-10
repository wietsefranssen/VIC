#include <ext_driver_shared_image.h>

void
dam_run(size_t cur_cell)
{
    extern dmy_struct *dmy;
    extern size_t current;
    extern ext_all_vars_struct *ext_all_vars;
    extern dam_con_map_struct *dam_con_map;
    extern dam_con_struct **dam_con;
    
    size_t years_running;
    
    size_t i;
    
    for(i = 0; i < dam_con_map[cur_cell].nd_active; i++){
        
        if(dmy[current].month != dmy[current-1].month){
            ext_all_vars[cur_cell].dams[i].months_running++;
            if(ext_all_vars[cur_cell].dams[i].months_running > 
                    DAM_HIST_YEARS * MONTHS_PER_YEAR){
                ext_all_vars[cur_cell].dams[i].months_running =
                        DAM_HIST_YEARS * MONTHS_PER_YEAR;
            }

            years_running = (size_t)(ext_all_vars[cur_cell].dams[i].months_running / 
                    MONTHS_PER_YEAR);
            if(years_running > DAM_HIST_YEARS){
                years_running = DAM_HIST_YEARS;
            }

            // Shift array
            ext_all_vars[cur_cell].dams[i].history_flow[DAM_HIST_YEARS * MONTHS_PER_YEAR-1] = 0.0;
            cshift(ext_all_vars[cur_cell].dams[i].history_flow, 1, DAM_HIST_YEARS * MONTHS_PER_YEAR, 1, -1);

            // Store monthly average
            ext_all_vars[cur_cell].dams[i].history_flow[0] =
                    ext_all_vars[cur_cell].dams[i].total_flow / 
                    ext_all_vars[cur_cell].dams[i].total_steps;
            ext_all_vars[cur_cell].dams[i].total_flow = 0.0;
            ext_all_vars[cur_cell].dams[i].total_steps = 0;        

            // Calculate volume
        }

        ext_all_vars[cur_cell].dams[i].total_flow += 
                ext_all_vars[cur_cell].routing.nat_discharge[0];
        ext_all_vars[cur_cell].dams[i].total_steps++;

        // Run dams
        if(dmy[current].year >= dam_con[cur_cell][i].year){
            ext_all_vars[cur_cell].dams[i].volume += 
                    ext_all_vars[cur_cell].routing.discharge[0];
            
            ext_all_vars[cur_cell].dams[i].area = 
                    dam_area(ext_all_vars[cur_cell].dams[i].volume,
                    dam_con[cur_cell][i].max_volume,
                    dam_con[cur_cell][i].max_area,
                    dam_con[cur_cell][i].max_height);
            ext_all_vars[cur_cell].dams[i].height = 
                    dam_height(ext_all_vars[cur_cell].dams[i].area,
                    dam_con[cur_cell][i].max_height);
        }
    }
}

double
dam_area(double volume, double max_volume, double max_area, double max_height){
    // Calculate surface area based on Kaveh et al 2013
    double N = ( 2 * max_volume ) / ( max_height * max_area );
    
    return max_area * pow(( volume / max_volume ), ((2-N)/2));
}

double
dam_height(double area, double max_height){
    // Calculate dam height based on Liebe et al 2005
    double height = sqrt(area) * (1 / 19.45);
    
    if(height > max_height){
        height = max_height;
    }
    
    return height;
}