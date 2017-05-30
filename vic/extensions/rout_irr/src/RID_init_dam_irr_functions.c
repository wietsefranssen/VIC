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
    extern domain_struct local_domain;
    extern veg_con_struct **veg_con;
        
    RID_cell *cur_cell;
    dam_unit *assigned_dams[local_domain.ncells_active];
    
    size_t i;
    size_t j;
    size_t k;

    for(j=0;j<local_domain.ncells_active;j++){
        assigned_dams[j]=NULL;
    }
            
    /*******************************
     Find servicing dams
    *******************************/ 
    for(i=0;i<RID.nr_dams;i++){        
        if(RID.dams[i].function!=DAM_IRR_FUNCTION){
            continue;
        }          
        
        cur_cell = RID.dams[i].cell;
        
        for(j=0;j<RID.param.dam_irr_distance;j++){  
            if(cur_cell->irr==NULL){
                continue;
            }
            
            if(assigned_dams[cur_cell->id]!=NULL){
                continue;
            }
            
            for(k=0;k<cur_cell->irr->nr_crops;k++){
                RID.dams[i].irrigated_area += local_domain.locations[cur_cell->id].area * 
                        veg_con[cur_cell->id][cur_cell->irr->veg_index[k]].Cv;
            }
            
            assigned_dams[cur_cell->id]=&RID.dams[i];            
            cur_cell = cur_cell->rout->downstream->cell;
        }
    }
    
    /*******************************
     Handle cases of multiple servicing dams
    *******************************/
    for(i=0;i<RID.nr_dams;i++){        
        if(RID.dams[i].function!=DAM_IRR_FUNCTION){
            continue;
        }  
        
        cur_cell = RID.dams[i].cell->rout->downstream->cell;
        
        for(j=0;j<RID.param.dam_irr_distance;j++){  
            if(cur_cell==NULL){
                continue;
            }
            
            if(assigned_dams[cur_cell->id]==&RID.dams[i] || assigned_dams[cur_cell->id]==NULL){
                continue;
            }
            
            if((assigned_dams[cur_cell->id]->capacity/assigned_dams[cur_cell->id]->irrigated_area) >
                    (RID.dams[i].capacity/RID.dams[i].irrigated_area)){
                continue;
            }
            
            for(k=0;k<cur_cell->irr->nr_crops;k++){
                assigned_dams[cur_cell->id]->irrigated_area -= local_domain.locations[cur_cell->id].area * 
                        veg_con[cur_cell->id][cur_cell->irr->veg_index[k]].Cv;
                RID.dams[i].irrigated_area += local_domain.locations[cur_cell->id].area * 
                        veg_con[cur_cell->id][cur_cell->irr->veg_index[k]].Cv;
            }
            
            assigned_dams[cur_cell->id]=&RID.dams[i];
            
            cur_cell = cur_cell->rout->downstream->cell;
        }
    }
    
    for(i=0;i<RID.nr_dams;i++){        
        if(RID.dams[i].function!=DAM_IRR_FUNCTION){
            continue;
        }          
        
        for(j=0;j<local_domain.ncells_active;j++){
            if(assigned_dams[j]!=&RID.dams[i]){
                continue;
            }
            
            RID.dams[i].nr_serviced_cells++;
        }
            
        RID.dams[i].serviced_cells=malloc(RID.dams[i].nr_serviced_cells * sizeof(*RID.dams[i].serviced_cells));
        check_alloc_status(RID.dams[i].serviced_cells,"Memory allocation error");
        
        /*******************************
         Assign serviced cells
        *******************************/ 
        k=0;
        for(j=0;j<local_domain.ncells_active;j++){
            if(assigned_dams[j]!=&RID.dams[i]){
                continue;
            }
            
            RID.dams[i].serviced_cells[k]=RID.cells[j].irr;
            RID.cells[j].irr->servicing_dam=&RID.dams[i];
            RID.cells[j].irr->servicing_dam_index = k;
            k++;
        }
    }
}
