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

void alloc_init_set_irrigation(){
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern veg_con_map_struct *veg_con_map;
    extern RID_struct RID;
    extern int mpi_rank;
    
    int **ivar_global_veg = NULL;
    int *ivar_local_veg = NULL;
    
    size_t i;
    size_t j;
    size_t iIrr; 
    size_t iCrop; 
    
    /*******************************
     Gather vegetation from local nodes to the master node
    *******************************/    
    ivar_global_veg = malloc(RID.param.nr_crops * sizeof (*ivar_global_veg));
    check_alloc_status(*ivar_global_veg, "Memory allocation error.");
    
    for(j=0;j<RID.param.nr_crops;j++){
        ivar_global_veg[j] = malloc(global_domain.ncells_total * sizeof (*ivar_global_veg[j]));
        check_alloc_status(ivar_global_veg[j], "Memory allocation error.");
    }
    
    ivar_local_veg = malloc(local_domain.ncells_active * sizeof (*ivar_local_veg));
    check_alloc_status(ivar_local_veg, "Memory allocation error.");
                    
    for(j=0;j<RID.param.nr_crops;j++){
        for (i = 0; i < local_domain.ncells_active; i++) {
            ivar_local_veg[i] = veg_con_map[i].vidx[RID.param.crop_class[j]];
        }
            
        gather_put_var_int(ivar_global_veg[j], ivar_local_veg);
    }
        
    free(ivar_local_veg);
    
    if (mpi_rank == VIC_MPI_ROOT) {    
        /*******************************
         Find number of irrigated cells
        *******************************/
        for(i=0;i<global_domain.ncells_active;i++){            
            for(j=0;j<RID.param.nr_crops;j++){
                if(ivar_global_veg[j][i]!=NODATA_VEG){
                    RID.nr_irr_cells++;
                    break;
                }
            }
        }
        
        printf("nr_irr_cells %zu",RID.nr_irr_cells);
        
        /*******************************
         Allocate and initialize irrigated cells
        *******************************/
        RID.irr_cells=malloc(RID.nr_irr_cells * sizeof(*RID.irr_cells));
        check_alloc_status(RID.irr_cells,"Memory allocation error.");
        
        RID.irr_cells[i].servicing_dam=NULL;
        RID.irr_cells[i].servicing_dam_index=0;    
        for(i=0;i<RID.nr_irr_cells;i++){
            RID.irr_cells[i].nr_crops=0;
        }
        
        /*******************************
         Assign irrigated cells and count number of crops
        *******************************/
        iIrr=0;
        for(i=0;i<global_domain.ncells_active;i++){
            for(j=0;j<RID.param.nr_crops;j++){
                if(ivar_global_veg[j][i]!=NODATA_VEG){
                    if(RID.cells[i].irr==NULL){
                        RID.cells[i].irr=&RID.irr_cells[iIrr];
                        RID.irr_cells[iIrr].cell=&RID.cells[i];  
                        iIrr++;
                    }
                    RID.cells[i].irr->nr_crops++;                    
                }
            }        
            printf("cell %zu: nr_crops %zu",i, RID.cells[i].irr->nr_crops);
        }
        
        /*******************************
         Allocate and initialize crop variables
        *******************************/        
        for(i=0;i<RID.nr_irr_cells;i++){
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
            
            for(j=0;j<RID.irr_cells[i].nr_crops;j++){
                RID.irr_cells[i].crop_index[j]=0;
                RID.irr_cells[i].veg_class[j]=0;
                RID.irr_cells[i].veg_index[j]=0;
                RID.irr_cells[i].demand[j]=0;
                RID.irr_cells[i].moisture[j]=0;
                RID.irr_cells[i].deficit[j]=0;
                RID.irr_cells[i].storage[j]=0;
                RID.irr_cells[i].normal_Ksat[j]=0;
            }
        }
        
        /*******************************
         Assign crop variables
        *******************************/  
        iCrop=0;        
        for(i=0;i<global_domain.ncells_active;i++){
            for(j=0;j<RID.param.nr_crops;j++){
                if(ivar_global_veg[j][i]!=NODATA_VEG){
                    RID.irr_cells[i].crop_index[iCrop]=j;
                    RID.irr_cells[i].veg_class[iCrop]=RID.param.crop_class[j];
                    RID.irr_cells[i].veg_index[iCrop]=ivar_global_veg[j][i];
                    iCrop++;
                }
            }
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