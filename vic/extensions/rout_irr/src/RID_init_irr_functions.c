/******************************************************************************
 * @section DESCRIPTION
 *  
 * Functions used in rout_init
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Set irr_cells based on the number of cells with crops and the irrigated area
 ******************************************************************************/

void set_irr(){
    extern domain_struct global_domain;
    extern veg_con_map_struct *veg_con_map;
    extern RID_struct RID;
    
    size_t i;
    size_t j;
    size_t iIrr;  
    
    /*******************************
     Find number of irrigated cells
    *******************************/
    for(i=0;i<global_domain.ncells_active;i++){
                
        for(j=0;j<RID.param.nr_crops;j++){
            
            if(veg_con_map[i].vidx[RID.param.crop_class[j]]==NODATA_VEG){
                continue;
            }

            RID.nr_irr_cells++;
            break;
        }
    }
        
    RID.irr_cells=malloc(RID.nr_irr_cells * sizeof(*RID.irr_cells));
    check_alloc_status(RID.irr_cells,"Memory allocation error.");
    
    /*******************************
     Assign irrigated cells
    *******************************/
    iIrr=0;
    for(i=0;i<global_domain.ncells_active;i++){
        
        for(j=0;j<RID.param.nr_crops;j++){
            
            if(veg_con_map[i].vidx[RID.param.crop_class[j]]==NODATA_VEG){
                continue;
            }

            RID.cells[i].irr=&RID.irr_cells[iIrr];
            RID.irr_cells[iIrr].cell=&RID.cells[i];
            iIrr++;
            break;
        }
    }
}

/******************************************************************************
 * @section brief
 *  
 * Set the crop information for each irrigated cell
 ******************************************************************************/

void set_irr_crops(){
    extern veg_con_map_struct *veg_con_map;
    extern soil_con_struct *soil_con;
    extern RID_struct RID;
    
    size_t i;
    size_t j;
    size_t iCrop;
    
    for(i=0;i<RID.nr_irr_cells;i++){
        
        RID.irr_cells[i].nr_crops=0;
        
        /*******************************
         Find number of crops
        *******************************/
        for(j=0;j<RID.param.nr_crops;j++){
            if(veg_con_map[RID.irr_cells[i].cell->id].vidx[RID.param.crop_class[j]]!=NODATA_VEG){
                RID.irr_cells[i].nr_crops++;
            }
        }
        
        RID.irr_cells[i].crop_index=malloc(RID.irr_cells[i].nr_crops * sizeof(*RID.irr_cells[i].crop_index));
        check_alloc_status(RID.irr_cells[i].crop_index,"Memory allocation error.");
        RID.irr_cells[i].veg_class=malloc(RID.irr_cells[i].nr_crops * sizeof(*RID.irr_cells[i].veg_class));
        check_alloc_status(RID.irr_cells[i].veg_class,"Memory allocation error.");
        RID.irr_cells[i].veg_index=malloc(RID.irr_cells[i].nr_crops * sizeof(*RID.irr_cells[i].veg_index));
        check_alloc_status(RID.irr_cells[i].veg_index,"Memory allocation error.");
        RID.irr_cells[i].demand=malloc(RID.irr_cells[i].nr_crops * sizeof(*RID.irr_cells[i].demand));
        check_alloc_status(RID.irr_cells[i].demand,"Memory allocation error.");
        RID.irr_cells[i].moisture=malloc(RID.irr_cells[i].nr_crops * sizeof(*RID.irr_cells[i].moisture));
        check_alloc_status(RID.irr_cells[i].moisture,"Memory allocation error.");
        RID.irr_cells[i].deficit=malloc(RID.irr_cells[i].nr_crops * sizeof(*RID.irr_cells[i].deficit));
        check_alloc_status(RID.irr_cells[i].deficit,"Memory allocation error.");
        RID.irr_cells[i].storage=malloc(RID.irr_cells[i].nr_crops * sizeof(*RID.irr_cells[i].storage));
        check_alloc_status(RID.irr_cells[i].storage,"Memory allocation error.");
        RID.irr_cells[i].normal_Ksat=malloc(RID.irr_cells[i].nr_crops * sizeof(*RID.irr_cells[i].normal_Ksat));
        check_alloc_status(RID.irr_cells[i].normal_Ksat,"Memory allocation error.");
        
        /*******************************
         Assign crop information
        *******************************/
        iCrop=0;        
        for(j=0;j<RID.param.nr_crops;j++){
            if(veg_con_map[RID.irr_cells[i].cell->id].vidx[RID.param.crop_class[j]]!=NODATA_VEG){
                RID.irr_cells[i].crop_index[iCrop]=j;
                RID.irr_cells[i].veg_class[iCrop]=RID.param.crop_class[j];
                RID.irr_cells[i].veg_index[iCrop]=veg_con_map[RID.irr_cells[i].cell->id].vidx[RID.param.crop_class[j]];
                iCrop++;
            }
        }
        
        for(j=0;j<RID.irr_cells[i].nr_crops;j++){
            RID.irr_cells[i].demand[j]=0;
            RID.irr_cells[i].moisture[j]=0;
            RID.irr_cells[i].deficit[j]=0;
            RID.irr_cells[i].storage[j]=0;
            RID.irr_cells[i].normal_Ksat[j]=soil_con[RID.irr_cells[i].cell->id].Ksat[0];
            soil_con[RID.irr_cells[i].cell->id].Ksat[0] = RID.param.crop_ksat;
        }
        
        RID.irr_cells[i].servicing_dam=NULL;
        RID.irr_cells[i].servicing_dam_index=0;
    }
}