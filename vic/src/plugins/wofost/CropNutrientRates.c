#include <stdio.h>
#include <stdlib.h>
#include "wofost/wofost.h"
#include "wofost/extern.h"


/* ---------------------------------------------------------------------------*/
/*  function NutrientRates()                                                  */
/*  Purpose: To calculate nutrient changes (i.e. rates) in the plant          */
/*           organs (kg N,P,K ha-1 d-1)                                       */
/* ---------------------------------------------------------------------------*/

void CropNutrientRates()
{   
    Crop->N_rt.storage = 0;
    Crop->P_rt.storage = 0;
    Crop->K_rt.storage = 0;     
 
    Crop->N_rt.leaves  = Crop->N_rt.Uptake_lv - Crop->N_rt.Transloc_lv - Crop->N_rt.death_lv;
    Crop->N_rt.stems   = Crop->N_rt.Uptake_st - Crop->N_rt.Transloc_st - Crop->N_rt.death_st;
    Crop->N_rt.roots   = Crop->N_rt.Uptake_ro - Crop->N_rt.Transloc_ro - Crop->N_rt.death_ro;

    Crop->P_rt.leaves  = Crop->P_rt.Uptake_lv - Crop->P_rt.Transloc_lv - Crop->P_rt.death_lv;
    Crop->P_rt.stems   = Crop->P_rt.Uptake_st - Crop->P_rt.Transloc_st - Crop->P_rt.death_st;
    Crop->P_rt.roots   = Crop->P_rt.Uptake_ro - Crop->P_rt.Transloc_ro - Crop->P_rt.death_ro;

    Crop->K_rt.leaves  = Crop->K_rt.Uptake_lv - Crop->K_rt.Transloc_lv - Crop->K_rt.death_lv;
    Crop->K_rt.stems   = Crop->K_rt.Uptake_st - Crop->K_rt.Transloc_st - Crop->K_rt.death_st;
    Crop->K_rt.roots   = Crop->K_rt.Uptake_ro - Crop->K_rt.Transloc_ro - Crop->K_rt.death_ro;
   
   
    /* Rate of N,P,K uptake in storage organs (kg N,P,K ha-1 d-1) */
    if (Crop->DevelopmentStage <  Crop->prm.DevelopmentStageNT)
    {
        Crop->N_rt.storage = min(Crop->N_rt.Demand_so, Crop->N_rt.Transloc/Crop->prm.TCNT);
        Crop->P_rt.storage = min(Crop->P_rt.Demand_so, Crop->P_rt.Transloc/Crop->prm.TCPT);
        Crop->K_rt.storage = min(Crop->K_rt.Demand_so, Crop->K_rt.Transloc/Crop->prm.TCKT); 
    }
}
