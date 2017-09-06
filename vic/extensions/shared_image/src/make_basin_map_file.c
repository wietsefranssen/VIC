#include <ext_shared_image.h>

void make_basin_map_file(basin_struct *basins){
    extern domain_struct global_domain;
        
    size_t i;
    
    FILE *file;
    char full_path [MAXSTRING];
    strcpy(full_path, "debug_output/basins.txt");
    
    if((file = fopen(full_path, "w"))!=NULL){
        for(i = 0;i < global_domain.ncells_total; i++){
            if(i%global_domain.n_nx==0){
                fprintf(file,"\n");
            }
            
            if(basins->basin_map[i]==NODATA_BASIN){
                fprintf(file," ;");
            }else{
                fprintf(file,"%zu;",basins->basin_map[i]);
            }            
        }
    }
    
    fclose(file);
}