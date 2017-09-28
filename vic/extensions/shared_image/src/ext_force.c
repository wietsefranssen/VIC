#include <ext_driver_shared_image.h>

void
ext_force(){
    extern size_t              current;
    extern dmy_struct         *dmy;
    extern global_param_struct global_param;
    extern ext_parameters_struct ext_param;
    extern ext_option_struct ext_options;
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern ext_filenames_struct ext_filenames;
    extern wu_hist_struct **wu_hist;
    
    double                    *dvar = NULL;
    
    size_t                     d3count[3];
    size_t                     d3start[3];
    
    size_t i;
    size_t j;
    
    if(ext_options.WATER_USE){
        if (current == 0 || (dmy[current].year != dmy[current - 1].year)) {
            d3start[0] = ext_param.forceoffset;
            d3start[1] = 0;
            d3start[2] = 0;
            d3count[0] = 1;
            d3count[1] = global_domain.n_ny;
            d3count[2] = global_domain.n_nx;

            dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
            check_alloc_status(dvar, "Memory allocation error.");
            
            //TODO: currently water demand is distributed equally, which should not be the case
            
            // irrigation demand
            get_scatter_nc_field_double(&ext_filenames.water_use,ext_filenames.info.irr_demand_var,
                    d3start, d3count, dvar);
            for(i=0;i<local_domain.ncells_active;i++){
                if(dvar[i]==NODATA_WU){
                    dvar[i]=0;
                }
                if(leap_year(dmy[current].year, CALENDAR_STANDARD)){
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_LYEAR;j++){
                        wu_hist[i][WU_IRRIGATION].demand[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_LYEAR);
                    }
                }else{
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_YEAR;j++){
                        wu_hist[i][WU_IRRIGATION].demand[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_YEAR);
                    }
                }
            }

            // domestic demand
            get_scatter_nc_field_double(&ext_filenames.water_use,ext_filenames.info.dom_demand_var,
                    d3start, d3count, dvar);
            for(i=0;i<local_domain.ncells_active;i++){
                if(dvar[i]==NODATA_WU){
                    dvar[i]=0;
                }
                if(leap_year(dmy[current].year, CALENDAR_STANDARD)){
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_LYEAR;j++){
                        wu_hist[i][WU_DOMESTIC].demand[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_LYEAR);
                    }
                }else{
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_YEAR;j++){
                        wu_hist[i][WU_DOMESTIC].demand[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_YEAR);
                    }
                }
            }

            // industrial demand
            get_scatter_nc_field_double(&ext_filenames.water_use,ext_filenames.info.ind_demand_var,
                    d3start, d3count, dvar);
            for(i=0;i<local_domain.ncells_active;i++){
                if(dvar[i]==NODATA_WU){
                    dvar[i]=0;
                }
                if(leap_year(dmy[current].year, CALENDAR_STANDARD)){
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_LYEAR;j++){
                        wu_hist[i][WU_INDUSTRIAL].demand[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_LYEAR);
                    }
                }else{
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_YEAR;j++){
                        wu_hist[i][WU_INDUSTRIAL].demand[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_YEAR);
                    }
                }
            }

            // irrigation consumption factor
            get_scatter_nc_field_double(&ext_filenames.water_use,ext_filenames.info.irr_cons_var,
                    d3start, d3count, dvar);
            for(i=0;i<local_domain.ncells_active;i++){
                if(dvar[i]==NODATA_WU){
                    dvar[i]=0;
                }
                if(leap_year(dmy[current].year, CALENDAR_STANDARD)){
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_LYEAR;j++){
                        wu_hist[i][WU_IRRIGATION].consumption_factor[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_LYEAR);
                    }
                }else{
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_YEAR;j++){
                        wu_hist[i][WU_IRRIGATION].consumption_factor[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_YEAR);
                    }
                }
            }

            // domestic consumption factor
            get_scatter_nc_field_double(&ext_filenames.water_use,ext_filenames.info.dom_cons_var,
                    d3start, d3count, dvar);
            for(i=0;i<local_domain.ncells_active;i++){
                if(dvar[i]==NODATA_WU){
                    dvar[i]=0;
                }
                if(leap_year(dmy[current].year, CALENDAR_STANDARD)){
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_LYEAR;j++){
                        wu_hist[i][WU_DOMESTIC].consumption_factor[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_LYEAR);
                    }
                }else{
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_YEAR;j++){
                        wu_hist[i][WU_DOMESTIC].consumption_factor[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_YEAR);
                    }
                }
            }

            // industrial consumption factor
            get_scatter_nc_field_double(&ext_filenames.water_use,ext_filenames.info.ind_cons_var,
                    d3start, d3count, dvar);
            for(i=0;i<local_domain.ncells_active;i++){
                if(dvar[i]==NODATA_WU){
                    dvar[i]=0;
                }
                if(leap_year(dmy[current].year, CALENDAR_STANDARD)){
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_LYEAR;j++){
                        wu_hist[i][WU_INDUSTRIAL].consumption_factor[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_LYEAR);
                    }
                }else{
                    for(j=0;j<global_param.model_steps_per_day * DAYS_PER_YEAR;j++){
                        wu_hist[i][WU_INDUSTRIAL].consumption_factor[j] = dvar[i] / (global_param.model_steps_per_day * DAYS_PER_YEAR);
                    }
                }
            }   
            
            ext_param.forceoffset++;
            ext_param.wu_hist_offset=0;
        }
    }
}