#include <stdio.h>
#include <stdlib.h>
#include "wofost/wofost.h"
#include "wofost/extern.h"

/* ----------------------------------------------------------------------*/
/*  function IntegrationNutrients()                                      */
/*  Purpose: Integration of the soil and crop nutrient rates  kg ha-1    */
/* ----------------------------------------------------------------------*/      

void IntegrationNutrients()
{   
    /* Integration of the total of soil N,P,K */
    Site->st_N_tot = max(0., Site->st_N_tot + Site->rt_N_tot);
    Site->st_P_tot = max(0., Site->st_P_tot + Site->rt_P_tot);
    Site->st_K_tot = max(0., Site->st_K_tot + Site->rt_K_tot);
    
    /* Integration of the total N,P,K soil mineralization */
    Site->st_N_mins += Site->rt_N_mins;
    Site->st_P_mins += Site->rt_P_mins;
    Site->st_K_mins += Site->rt_K_mins;
        
    /* Crops nutrients */
    Crop->N_st.leaves  += Crop->N_rt.leaves ;
    Crop->N_st.stems   += Crop->N_rt.stems  ;
    Crop->N_st.roots   += Crop->N_rt.roots  ;
    Crop->N_st.storage += Crop->N_rt.storage;
          
    Crop->P_st.leaves  += Crop->P_rt.leaves ;
    Crop->P_st.stems   += Crop->P_rt.stems  ;
    Crop->P_st.roots   += Crop->P_rt.roots  ;
    Crop->P_st.storage += Crop->P_rt.storage;
   
    Crop->K_st.leaves  += Crop->K_rt.leaves ;
    Crop->K_st.stems   += Crop->K_rt.stems  ;
    Crop->K_st.roots   += Crop->K_rt.roots  ;
    Crop->K_st.storage += Crop->K_rt.storage;
    
    /* Nutrient losses due to dying plant parts */
    Crop->N_st.death_lv += Crop->N_rt.death_lv;
    Crop->N_st.death_st += Crop->N_rt.death_st;
    Crop->N_st.death_ro += Crop->N_rt.death_ro;
  
    Crop->P_st.death_lv += Crop->P_rt.death_lv;
    Crop->P_st.death_st += Crop->P_rt.death_st;
    Crop->P_st.death_ro += Crop->P_rt.death_ro;
 
    Crop->K_st.death_lv += Crop->K_rt.death_lv;
    Crop->K_st.death_st += Crop->K_rt.death_st;
    Crop->K_st.death_ro += Crop->K_rt.death_ro;
   
}