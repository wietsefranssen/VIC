#include <vic.h>

void
wu_alloc(void)
{
    extern domain_struct local_domain;
    extern wu_var_struct **wu_var;
    extern wu_con_struct **wu_con;
    
    size_t i;
    
    wu_var = malloc(local_domain.ncells_active * sizeof(*wu_var));
    check_alloc_status(wu_var,"Memory allocation error");
    
    wu_con = malloc(local_domain.ncells_active * sizeof(*wu_con));
    check_alloc_status(wu_con,"Memory allocation error");
        
    for(i=0; i<local_domain.ncells_active; i++){        
        wu_con[i] = malloc(WU_NSECTORS * sizeof(*wu_con[i]));
        check_alloc_status(wu_con[i],"Memory allocation error");
        
        wu_var[i] = malloc(WU_NSECTORS * sizeof(*wu_var[i]));
        check_alloc_status(wu_var[i],"Memory allocation error");
    }   
}

void
wu_finalize(void)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern filenames_struct filenames;
    extern wu_var_struct **wu_var;
    extern wu_con_struct **wu_con;
    
    int status;
    
    size_t i;
                
    if(options.WU_NINPUT_FROM_FILE > 0){
        // close previous forcing file
        status = nc_close(filenames.water_use.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.water_use.nc_filename);
    }
            
    for(i=0; i<local_domain.ncells_active; i++){
        free(wu_con[i]);
        free(wu_var[i]);
    }
    free(wu_con);
    free(wu_var);
}