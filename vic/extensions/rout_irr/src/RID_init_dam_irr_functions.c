/******************************************************************************
 * @section DESCRIPTION
 *  
 * Functions used in rout_init
 ******************************************************************************/

#include <rout.h>
 
/******************************************************************************
 * @section brief
 * 
 * Combine dams with irrigated areas to facilitate redistribution of dam water
 * to irrigated areas.
 ******************************************************************************/

void set_dam_irr_service(){
    extern RID_struct RID;
    extern soil_con_struct *soil_con;
    extern domain_struct local_domain;
    extern veg_con_struct **veg_con;
        
    dam_unit *assigned_dams[RID.nr_irr_cells];
    
    size_t i;
    size_t j;
    size_t k;

    for(j=0;j<RID.nr_irr_cells;j++){
        assigned_dams[j]=NULL;
    }
            
    /*******************************
     Find servicing dams
    *******************************/ 
    for(i=0;i<RID.nr_dams;i++){        
        if(RID.dams[i].function!=DAM_IRR_FUNCTION){
            continue;
        }          
        for(j=0;j<RID.nr_irr_cells;j++){
            if(assigned_dams[j]!=NULL){
                continue;
            }
            
            if(soil_con[RID.irr_cells[j].cell->id].elevation >= soil_con[RID.dams[i].cell->id].elevation){
                continue;
            }

            if(distance(RID.irr_cells[j].cell->x,RID.irr_cells[j].cell->y,
                    RID.dams[i].cell->x,RID.dams[i].cell->y)
                    >RID.param.dam_irr_distance){
                continue;
            }
            
            assigned_dams[j]=&RID.dams[i];
            
            for(k=0;k<RID.irr_cells[j].nr_crops;k++){
                RID.dams[i].irrigated_area += local_domain.locations[RID.irr_cells[j].cell->id].area * 
                        veg_con[RID.irr_cells[j].cell->id][RID.irr_cells[j].veg_index[k]].Cv;
            }
        }
    }
    
    /*******************************
     Handle cases of multiple servicing dams
    *******************************/
    for(i=0;i<RID.nr_dams;i++){        
        if(RID.dams[i].function!=DAM_IRR_FUNCTION){
            continue;
        }
        
        for(j=0;j<RID.nr_irr_cells;j++){            
            if(assigned_dams[j]==&RID.dams[i] || assigned_dams[j]==NULL){
                continue;
            }
            
            if(soil_con[RID.irr_cells[j].cell->id].elevation >= soil_con[RID.dams[i].cell->id].elevation){
                continue;
            }

            if(distance(RID.irr_cells[j].cell->x,RID.irr_cells[j].cell->y,
                    RID.dams[i].cell->x,RID.dams[i].cell->y)
                    >RID.param.dam_irr_distance){
                continue;
            }
            
            if((assigned_dams[j]->capacity/assigned_dams[j]->irrigated_area) >
                    (RID.dams[i].capacity/RID.dams[i].irrigated_area)){
                continue;
            }
            
            for(k=0;k<RID.irr_cells[j].nr_crops;k++){
                assigned_dams[j]->irrigated_area -= local_domain.locations[RID.irr_cells[j].cell->id].area * 
                        veg_con[RID.irr_cells[j].cell->id][RID.irr_cells[j].veg_index[k]].Cv;
                RID.dams[i].irrigated_area += local_domain.locations[RID.irr_cells[j].cell->id].area * 
                        veg_con[RID.irr_cells[j].cell->id][RID.irr_cells[j].veg_index[k]].Cv;
            }
            
            assigned_dams[j]=&RID.dams[i];
        }
    }
    
    for(i=0;i<RID.nr_dams;i++){        
        if(RID.dams[i].function!=DAM_IRR_FUNCTION){
            continue;
        }          
        
        for(j=0;j<RID.nr_irr_cells;j++){
            if(assigned_dams[j]!=&RID.dams[i]){
                continue;
            }
            
            RID.dams[i].nr_serviced_cells++;
        }
        
        RID.dams[i].serviced_cells=malloc(RID.dams[i].nr_serviced_cells * sizeof(*RID.dams[i].serviced_cells));
        check_alloc_status(RID.dams[i].serviced_cells,"Memory allocation error.");
        
        /*******************************
         Assign serviced cells
        *******************************/ 
        k=0;
        for(j=0;j<RID.nr_irr_cells;j++){
            if(assigned_dams[j]!=&RID.dams[i]){
                continue;
            }
            
            RID.dams[i].serviced_cells[k].cell=RID.irr_cells[j].cell;
            
            RID.dams[i].serviced_cells[k].demand_crop=malloc(RID.irr_cells[j].nr_crops * sizeof(*RID.dams[i].serviced_cells[k].demand_crop));
            check_alloc_status(RID.dams[i].serviced_cells[k].demand_crop,"Memory allocation error.");
            RID.dams[i].serviced_cells[k].moisture_content=malloc(RID.irr_cells[j].nr_crops * sizeof(*RID.dams[i].serviced_cells[k].moisture_content));
            check_alloc_status(RID.dams[i].serviced_cells[k].moisture_content,"Memory allocation error.");
            RID.dams[i].serviced_cells[k].deficit=malloc(RID.irr_cells[j].nr_crops * sizeof(*RID.dams[i].serviced_cells[k].deficit));
            check_alloc_status(RID.dams[i].serviced_cells[k].deficit,"Memory allocation error.");
            
            RID.irr_cells[j].servicing_dam=&RID.dams[i];
            RID.irr_cells[j].servicing_dam_cell_index=k;
            k++;
        }
        
        for(j=0;j<RID.dams[i].nr_serviced_cells;j++){
            for(k=0;k<RID.dams[i].serviced_cells[j].cell->irr->nr_crops;k++){
                RID.dams[i].serviced_cells[j].deficit[k]=0;
                RID.dams[i].serviced_cells[j].moisture_content[k]=0;
                RID.dams[i].serviced_cells[j].demand_crop[k]=0;
            }
        }
    }
}
