#include <vic.h>

void
wu_alloc(void)
{
    extern domain_struct local_domain;
    extern option_struct options;
    extern wu_var_struct **wu_var;
    extern wu_con_struct **wu_con;
    
    size_t i;
    size_t j;
    
    wu_var = malloc(local_domain.ncells_active * sizeof(*wu_var));
    check_alloc_status(wu_var,"Memory allocation error");
    
    wu_con = malloc(local_domain.ncells_active * sizeof(*wu_con));
    check_alloc_status(wu_con,"Memory allocation error");
        
    for(i=0; i<local_domain.ncells_active; i++){        
        wu_con[i] = malloc(WU_NSECTORS * sizeof(*wu_con[i]));
        check_alloc_status(wu_con[i],"Memory allocation error");
        
        wu_var[i] = malloc(WU_NSECTORS * sizeof(*wu_var[i]));
        check_alloc_status(wu_var[i],"Memory allocation error");
        
        for(j = 0; j < WU_NSECTORS; j++){        
            wu_var[i][j].compensation = malloc(options.WU_COMPENSATION_TIME[j] * sizeof(*wu_var[i][j].compensation));
            check_alloc_status(wu_var[i][j].compensation,"Memory allocation error");
        }
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
    size_t j;
                
    if(options.WU_NINPUT_FROM_FILE > 0){
        // close previous forcing file
        status = nc_close(filenames.water_use.nc_id);
        check_nc_status(status, "Error closing %s",
                        filenames.water_use.nc_filename);
    }
            
    for(i=0; i<local_domain.ncells_active; i++){
        for(j = 0; j < WU_NSECTORS; j++){
            free(wu_var[i][j].compensation);
        }
        free(wu_con[i]);
        free(wu_var[i]);
    }
    free(wu_con);
    free(wu_var);
}