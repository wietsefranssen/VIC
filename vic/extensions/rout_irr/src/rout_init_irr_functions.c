#include <rout.h>

void set_irr(){
    //Set irr_cell for a cell if the cell contains crops
    extern domain_struct global_domain;
    extern veg_con_map_struct *veg_con_map;
    extern module_struct rout;
    
    size_t i;
    size_t j;
    size_t iIrr;
    
    for(i=0;i<global_domain.ncells_active;i++){
        for(j=0;j<rout.param.nr_crop_classes;j++){
            if(veg_con_map[i].vidx[rout.param.crop_class[j]]!=NODATA_VEG){
                //Crop is in cell
                rout.nr_irr_cells++;
                break;
            }
        }
    }
    
    //Allocate memory based on number of irrigated cells
    rout.irr_cells=malloc(rout.nr_irr_cells * sizeof(*rout.irr_cells));
    check_alloc_status(rout.irr_cells,"Memory allocation error.");
    
    iIrr=0;
    for(i=0;i<global_domain.ncells_active;i++){
        for(j=0;j<rout.param.nr_crop_classes;j++){
            if(veg_con_map[i].vidx[rout.param.crop_class[j]]!=NODATA_VEG){
                //Connect module_cell with irr_cell
                rout.cells[i].irr=&rout.irr_cells[iIrr];
                rout.irr_cells[iIrr].cell=&rout.cells[i];
                iIrr++;
                break;
            }
        }
    }
}

void set_irr_crops(){
    //Set the actual crop information if a cell has crops
    extern veg_con_map_struct *veg_con_map;
    extern module_struct rout;
    
    size_t i;
    size_t j;
    size_t iCrop;
    
    for(i=0;i<rout.nr_irr_cells;i++){
        
        rout.irr_cells[i].nr_crops=0;
        rout.irr_cells[i].nr_servicing_dams=0;
        
        for(j=0;j<rout.param.nr_crop_classes;j++){
            if(veg_con_map[rout.irr_cells[i].cell->id].vidx[rout.param.crop_class[j]]!=NODATA_VEG){
                //Crop is in cell
                rout.irr_cells[i].nr_crops++;
            }
        }
        
        //Allocate memory based on number of crops
        rout.irr_cells[i].crop_index=malloc(rout.irr_cells[i].nr_crops * sizeof(*rout.irr_cells[i].crop_index));
        check_alloc_status(rout.irr_cells[i].crop_index,"Memory allocation error.");
        rout.irr_cells[i].veg_class=malloc(rout.irr_cells[i].nr_crops * sizeof(*rout.irr_cells[i].veg_class));
        check_alloc_status(rout.irr_cells[i].veg_class,"Memory allocation error.");
        rout.irr_cells[i].veg_index=malloc(rout.irr_cells[i].nr_crops * sizeof(*rout.irr_cells[i].veg_index));
        check_alloc_status(rout.irr_cells[i].veg_index,"Memory allocation error.");
        rout.irr_cells[i].max_cv=malloc(rout.irr_cells[i].nr_crops * sizeof(*rout.irr_cells[i].max_cv));
        check_alloc_status(rout.irr_cells[i].max_cv,"Memory allocation error.");
        
        iCrop=0;
        
        for(j=0;j<rout.param.nr_crop_classes;j++){
            if(veg_con_map[rout.irr_cells[i].cell->id].vidx[rout.param.crop_class[j]]!=NODATA_VEG){
                //Set crop information
                rout.irr_cells[i].crop_index[iCrop]=j;
                rout.irr_cells[i].veg_class[iCrop]=rout.param.crop_class[j];
                rout.irr_cells[i].veg_index[iCrop]=veg_con_map[rout.irr_cells[i].cell->id].vidx[rout.param.crop_class[j]];
                rout.irr_cells[i].max_cv[iCrop]=veg_con_map[rout.irr_cells[i].cell->id].Cv[rout.param.crop_class[j]];
                iCrop++;
            }
        }
    }
}