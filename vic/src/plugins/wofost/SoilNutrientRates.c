#include <stdio.h>
#include <stdlib.h>
#include "wofost/wofost.h"
#include "wofost/extern.h"

/* ---------------------------------------------------------------*/
/*  function SoilNutrientRates()                                  */
/*  Purpose: Calculation of the soil nutrient rates kg ha-1 d-1   */
/* ---------------------------------------------------------------*/     

void SoilNutrientRates()
{
    float N_fert;
    float P_fert;
    float K_fert;
    
    float day_fl;
    
    Site->rt_N_mins = 0.;
    Site->rt_P_mins = 0.;
    Site->rt_K_mins = 0.;
    
    if (Crop->DevelopmentStage > 0. && Crop->DevelopmentStage <= Crop->prm.DevelopmentStageNLimit)
    {   /* NPK rates that come available through mineralization, cannot exceed */
        /* the available NPK in the soil                                       */
        Site->rt_N_mins = min(Mng->N_Mins * Mng->NRecoveryFrac, Site->st_N_tot); 
        Site->rt_P_mins = min(Mng->P_Mins * Mng->PRecoveryFrac, Site->st_P_tot); 
        Site->rt_K_mins = min(Mng->K_Mins * Mng->KRecoveryFrac, Site->st_K_tot); 
    }
    
    day_fl = (float)Day;
    
    /* NPK amount that comes available for the crop at day_fl through fertilizer applications */
    N_fert = Afgen(Mng->N_Fert_table, &day_fl) * Afgen(Mng->N_Uptake_frac, &day_fl);
    P_fert = Afgen(Mng->P_Fert_table, &day_fl) * Afgen(Mng->P_Uptake_frac, &day_fl);
    K_fert = Afgen(Mng->K_Fert_table, &day_fl) * Afgen(Mng->K_Uptake_frac, &day_fl);
    
    Site->rt_N_tot = (N_fert / Step) - Crop->N_rt.Uptake  + Site->rt_N_mins;
    Site->rt_P_tot = (P_fert / Step) - Crop->P_rt.Uptake  + Site->rt_P_mins;
    Site->rt_K_tot = (K_fert / Step) - Crop->K_rt.Uptake  + Site->rt_K_mins;
    //printf("  Site->rt_N_mins: %5.1f Site->rt_P_mins: %5.1f Site->rt_K_mins: %5.1f", Site->rt_N_mins, Site->rt_P_mins, Site->rt_K_mins);
}
