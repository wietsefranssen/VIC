/******************************************************************************
 * @section DESCRIPTION
 *  
 * Functions used in rout_run
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Return whether a crop is in its irrigation season.
 ******************************************************************************/
bool in_irrigation_season(size_t crop_index, size_t current_julian_day){
    extern RID_struct RID;
    
    if(RID.param.start_irr[crop_index] < RID.param.end_irr[crop_index]){
        if(current_julian_day>=RID.param.start_irr[crop_index] && current_julian_day < RID.param.end_irr[crop_index]){
            return true;
        }else{
            return false;
        }
    }else{
        if(current_julian_day>=RID.param.start_irr[crop_index] && current_julian_day < RID.param.end_irr[crop_index]){
            return false;
        }else{
            return true;
        }
    }
}

/******************************************************************************
 * @section brief
 *  
 * Get the moisture content over all snow and frost bands. Ice is not included
 ******************************************************************************/
void get_moisture_content(size_t cell_id, size_t veg_index, double *moisture_content){
    extern option_struct options;
    extern all_vars_struct *all_vars;
    extern soil_con_struct *soil_con;
    
    double ice_content = 0.0;       //mm
    double liquid_content = 0.0;    //mm
    
    size_t i;
    size_t j;
    
    for(i=0;i<options.SNOW_BAND;i++){
        liquid_content +=  all_vars[cell_id].cell[veg_index][i].layer[0].moist 
                * soil_con[cell_id].AreaFract[i];
        
        for (j = 0; j < options.Nfrost; j++) {
            ice_content += all_vars[cell_id].cell[veg_index][i].layer[0].ice[j] 
                    * soil_con[cell_id].AreaFract[i] * soil_con[cell_id].frost_fract[j];
        }
    }
            
    *moisture_content = liquid_content - ice_content;
    
    if(*moisture_content<0){
        log_err("Negative moisture content?");
    }
}

void get_storage_infiltration(size_t cell_id, double *storage, double *infiltration, double moisture_content){
    extern soil_con_struct *soil_con;
    
    double infiltration_need;
    infiltration_need = soil_con[cell_id].max_moist[0] - moisture_content;
    
    if(infiltration_need>0){
        if(*storage>infiltration_need){
            *infiltration = infiltration_need;
            *storage-=infiltration_need;
        }else{
            *infiltration=*storage;
            *storage=0;
        }
    }
}

/******************************************************************************
 * @section brief
 *  
 * Get the irrigation demand for each crop and the total cell. Irrigation
 * demand is defined as the difference between the current moisture content
 * and the field capacity (Wcr/ 0.7). There is only demand when the soil 
 * moisture is below the critical point (Wcr).
 ******************************************************************************/
void get_demand(irr_cell *cur_irr, size_t veg_index, double *demand, double storage){
    extern veg_con_struct **veg_con;
    extern domain_struct local_domain;
    
    if(storage <= MIN_POND_FRAC * MAX_POND_STORAGE){                  
        *demand = (MAX_POND_STORAGE - storage)
                / MM_PER_M * (local_domain.locations[cur_irr->cell->id].area * 
                veg_con[cur_irr->cell->id][veg_index].Cv);
    }
    if(*demand<0){
        *demand=0;
    }
    
    if(*demand<0){
        log_err("Negative demand?");
    }
}

/******************************************************************************
 * @section brief
 *  
 * Get crop irrigation based on available water, distribute irrigation
 * equally across crops.
 ******************************************************************************/
void get_irrigation(double *irrigation_crop, double demand_cell, double demand_crop, double available_water){ 
    
    if(demand_cell<available_water){        
        *irrigation_crop = demand_crop;
    }else{
        *irrigation_crop = demand_crop * (available_water / demand_cell);
    }
    
    if(*irrigation_crop<0){
        log_err("Negative crop irrigation?");
    }
}

/******************************************************************************
 * @section brief
 *  
 * Update values used in irrigation, mostly for output.
 ******************************************************************************/
void update_demand_and_irrigation(double irrigation_crop, double *demand_cell, double *demand_crop, double *available_water){
    
    *demand_crop -= irrigation_crop;
    *demand_cell -= irrigation_crop;
    *available_water -= irrigation_crop;
}

/******************************************************************************
 * @section brief
 *  
 * Add irrigation water to moisture content across snow bands
 ******************************************************************************/
void increase_moisture_content(size_t cell_id, size_t veg_index, double *moisture_content, double increase){
    extern all_vars_struct *all_vars;
    extern option_struct options;
        
    size_t i;
      
    *moisture_content += increase;

    for(i=0;i<options.SNOW_BAND;i++){ 
        all_vars[cell_id].cell[veg_index][i].layer[0].moist = *moisture_content;
    }
}

void increase_storage_content(size_t cell_id, size_t veg_index, double *storage, double increase){
    extern domain_struct local_domain;
    extern veg_con_struct **veg_con;
    
    *storage+=increase 
                / (local_domain.locations[cell_id].area * 
                veg_con[cell_id][veg_index].Cv) * MM_PER_M;
}