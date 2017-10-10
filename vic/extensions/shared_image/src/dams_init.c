#include <ext_driver_shared_image.h>

void
dams_init(){
    extern ext_filenames_struct ext_filenames;
    extern ext_option_struct ext_options;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern dam_con_struct **dam_con;
    extern dam_con_map_struct *dam_con_map;
    
    double *dvar = NULL;
    int *ivar = NULL;
    
    size_t i;
    size_t j;
    size_t  d3count[3];
    size_t  d3start[3];
    
    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1; 
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx; 
        
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");    
    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");    
            
    for(j=0;j<ext_options.ndams;j++){
        d3start[0]=j;        
        get_scatter_nc_field_double(&ext_filenames.dams, ext_filenames.info.dam_lat_var, d3start, d3count,
                         dvar);
        for(i=0;i<local_domain.ncells_active;i++){
            if(dam_con_map[i].Ndams>j){
                dam_con[i][j].lat=dvar[i];
            }
        }
    }
    
    for(j=0;j<ext_options.ndams;j++){
        d3start[0]=j;        
        get_scatter_nc_field_double(&ext_filenames.dams, ext_filenames.info.dam_lon_var, d3start, d3count,
                         dvar);
        for(i=0;i<local_domain.ncells_active;i++){
            if(dam_con_map[i].Ndams>j){
                dam_con[i][j].lon=dvar[i];
            }
        }
    }
    
    for(j=0;j<ext_options.ndams;j++){
        d3start[0]=j;        
        get_scatter_nc_field_double(&ext_filenames.dams, ext_filenames.info.dam_volume_var, d3start, d3count,
                         dvar);
        for(i=0;i<local_domain.ncells_active;i++){
            if(dam_con_map[i].Ndams>j){
                dam_con[i][j].max_volume=dvar[i] * pow(M_PER_KM, 2);
            }
        }
    }
    
    for(j=0;j<ext_options.ndams;j++){
        d3start[0]=j;        
        get_scatter_nc_field_double(&ext_filenames.dams, ext_filenames.info.dam_area_var, d3start, d3count,
                         dvar);
        for(i=0;i<local_domain.ncells_active;i++){
            if(dam_con_map[i].Ndams>j){
                dam_con[i][j].max_area=dvar[i] * pow(M_PER_KM, 2);
            }
        }
    }
    
    for(j=0;j<ext_options.ndams;j++){
        d3start[0]=j;        
        get_scatter_nc_field_double(&ext_filenames.dams, ext_filenames.info.dam_height_var, d3start, d3count,
                         dvar);
        for(i=0;i<local_domain.ncells_active;i++){
            if(dam_con_map[i].Ndams>j){
                dam_con[i][j].max_height=dvar[i];
            }
        }
    }
    
    for(j=0;j<ext_options.ndams;j++){
        d3start[0]=j;        
        get_scatter_nc_field_int(&ext_filenames.dams, ext_filenames.info.dam_year_var, d3start, d3count,
                         ivar);
        for(i=0;i<local_domain.ncells_active;i++){
            if(dam_con_map[i].Ndams>j){
                dam_con[i][j].year=ivar[i];
            }
        }
    }
    
    
    free(dvar);
    free(ivar);
}