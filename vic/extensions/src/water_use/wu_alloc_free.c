#include <ext_driver_shared_image.h>

void
wu_alloc(void)
{
    extern domain_struct local_domain;
    extern ext_option_struct ext_options;
    extern global_param_struct global_param;
    extern ext_all_vars_struct *ext_all_vars;
    extern wu_con_struct **wu_con;
    
    size_t i;
    size_t j;
    
    wu_con = malloc(local_domain.ncells_active * sizeof(*wu_con));
    check_alloc_status(wu_con,"Memory allocation error");
        
    for(i=0; i<local_domain.ncells_active; i++){        
        wu_con[i] = malloc(WU_NSECTORS * sizeof(*wu_con[i]));
        check_alloc_status(wu_con[i],"Memory allocation error");
        
        ext_all_vars[i].water_use = malloc(WU_NSECTORS * sizeof(*ext_all_vars[i].water_use));
        check_alloc_status(ext_all_vars[i].water_use,"Memory allocation error");
        
        for(j = 0; j < WU_NSECTORS; j++){        
            ext_all_vars[i].water_use[j].compensation = malloc(ext_options.WU_COMPENSATION_TIME[j] * global_param.model_steps_per_day * sizeof(*ext_all_vars[i].water_use[j].compensation));
            check_alloc_status(ext_all_vars[i].water_use[j].compensation,"Memory allocation error");
        }
    }   
}

void
wu_finalize(void)
{
    extern domain_struct local_domain;
    extern ext_filenames_struct ext_filenames;
    extern ext_all_vars_struct *ext_all_vars;
    extern wu_con_struct **wu_con;
    
    int status;
    
    size_t i;    
    size_t j;
                
    // close previous forcing file
    status = nc_close(ext_filenames.water_use.nc_id);
    check_nc_status(status, "Error closing %s",
                    ext_filenames.water_use.nc_filename);
            
    for(i=0; i<local_domain.ncells_active; i++){
        for(j = 0; j < WU_NSECTORS; j++){
            free(ext_all_vars[i].water_use[j].compensation);
        }
        free(wu_con[i]);
        free(ext_all_vars[i].water_use);
    }
    free(wu_con);
}