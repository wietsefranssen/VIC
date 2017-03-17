/******************************************************************************
 * @section DESCRIPTION
 *  
 * Functions used in rout_run
 ******************************************************************************/

#include <rout.h>

/******************************************************************************
 * @section brief
 *  
 * Check whether a crop is in its growing season or 2 weeks before the
 * growing season.
 ******************************************************************************/

bool in_irrigation_season(size_t crop_index, size_t current_julian_day){
    extern RID_struct RID;
    
    short int start;
    start=RID.param.crop_sow[crop_index] - (2 * DAYS_PER_WEEK);
    if(start<1){
        start+=DAYS_PER_YEAR;
    }
    
    if(start < RID.param.crop_harvest[crop_index]){
        if(current_julian_day>=(size_t)start && current_julian_day < RID.param.crop_harvest[crop_index]){
            return true;
        }else{
            return false;
        }
    }else{
        if(current_julian_day>=(size_t)start && current_julian_day < RID.param.crop_harvest[crop_index]){
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
            
    *moisture_content=liquid_content - ice_content;
}

/******************************************************************************
 * @section brief
 *  
 * Get the irrigation demand for each crop and the total cell. Irrigation
 * demand is defined as the difference between the current moisture content
 * and the field capacity (Wcr/ 0.7). There is only demand when the soil 
 * moisture is below the critical point (Wcr).
 ******************************************************************************/

void get_irrigation_demand(size_t cell_id, size_t veg_index, double moisture_content, double *demand_crop){
    extern soil_con_struct *soil_con;
    extern veg_con_struct **veg_con;
    extern domain_struct local_domain;
    
    if(moisture_content <= soil_con[cell_id].Wcr[0]){                  
        *demand_crop = ((soil_con[cell_id].Wcr[0] / 0.7) - moisture_content)
                / MM_PER_M * (local_domain.locations[cell_id].area * 
                veg_con[cell_id][veg_index].Cv);
    }else{
        *demand_crop=0;
    }
}

/******************************************************************************
 * @section brief
 *  
 * If there is a local source of water, do irrigation. If the local source
 * is not enough to supply the full irrigation demands then spread irrigation
 * equally.
 ******************************************************************************/

void do_irrigation(RID_cell *cur_cell, double demand_crop[], double *demand_cell, double moisture_content[]){
    extern double ***out_data;
    extern global_param_struct global_param;
    extern all_vars_struct *all_vars;
    extern domain_struct local_domain;
    extern veg_con_struct **veg_con;
    extern option_struct options;
    extern RID_struct RID;
    
    size_t i;
    size_t j;
    double added_water_cell;
    double added_water_crop[cur_cell->irr->nr_crops];
    
    double available_water = cur_cell->rout->outflow[0] * global_param.dt;
    
    if(available_water<=0){
        return;
    }    
    
    for(i=0;i<cur_cell->irr->nr_crops;i++){
        if(*demand_cell<available_water){        
            added_water_crop[i] = demand_crop[i];
        }else{
            added_water_crop[i] = available_water * (demand_crop[i] / *demand_cell);
        }
    }
    
    *demand_cell = 0;
    added_water_cell=0;
    for(i=0;i<cur_cell->irr->nr_crops;i++){        
        available_water -= added_water_crop[i];
        demand_crop[i] -= added_water_crop[i];
        added_water_cell += added_water_crop[i];
        *demand_cell += demand_crop[i];
        
        if(RID.param.fpot_irrigation){
            added_water_crop[i]+=demand_crop[i];
            demand_crop[i]=0;
            added_water_cell += added_water_crop[i];
            *demand_cell=0;
        }
    }        
    
    out_data[cur_cell->id][OUT_LOCAL_IRR][0] = added_water_cell / M3_PER_HM3;
    
    if(added_water_cell<0){
        log_err("Adding a negative amount of water?");
    }
    
    for(i=0;i<cur_cell->irr->nr_crops;i++){        
        moisture_content[i] += added_water_crop[i] 
                    / (local_domain.locations[cur_cell->id].area * 
                    veg_con[cur_cell->id][cur_cell->irr->veg_index[i]].Cv) * MM_PER_M;
        
        for(j=0;j<options.SNOW_BAND;j++){ 
            all_vars[cur_cell->id].cell[cur_cell->irr->veg_index[i]][j].layer[0].moist = moisture_content[i];
        }
    }
    
    cur_cell->rout->outflow[0]= available_water / global_param.dt;
    out_data[cur_cell->id][OUT_DISCHARGE][0] = cur_cell->rout->outflow[0];
}

/******************************************************************************
 * @section brief
 *  
 * Distribute demand and moisture content values over the servicing dam
 ******************************************************************************/

void update_servicing_dam_values(RID_cell *cur_cell, double demand_crop[], double moisture_content[]){    
    extern double ***out_data;
    size_t i;
    
    if(cur_cell->irr->servicing_dam==NULL){
        return;
    }
    
    for(i=0;i<cur_cell->irr->nr_crops;i++){
        cur_cell->irr->servicing_dam->serviced_cells[cur_cell->irr->servicing_dam_cell_index].demand_crop[i]=demand_crop[i];
        cur_cell->irr->servicing_dam->serviced_cells[cur_cell->irr->servicing_dam_cell_index].moisture_content[i]=moisture_content[i];
        
        out_data[cur_cell->id][OUT_DEMAND_DAM][0] += cur_cell->irr->servicing_dam->serviced_cells[cur_cell->irr->servicing_dam_cell_index].demand_crop[i] / M3_PER_HM3;
    }
}