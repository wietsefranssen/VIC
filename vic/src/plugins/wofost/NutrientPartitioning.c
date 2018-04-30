#include <stdio.h>
#include <stdlib.h>
#include "vic.h"

#include "wofost/penman.h"

/* -------------------------------------------------------------------------*/
/*  function NutrientPartioning()                                           */
/*  Purpose: To compute the partitioning of the total N/P/K uptake rate     */
/*           (N,P,K UPTR) over leaves, stem, and roots kg  ha-1 d-1         */
/* -------------------------------------------------------------------------*/
void NutrientPartioning()
{     
    float Total_N_demand;
    float Total_P_demand;
    float Total_K_demand;
    
    float NutrientLimit;
    float N_Fix_rt;
    
    float tiny = 0.001;
    
    Crop->N_rt.Uptake_lv = 0.;
    Crop->N_rt.Uptake_st = 0.;
    Crop->N_rt.Uptake_ro = 0.;
    
    Crop->P_rt.Uptake_lv = 0.;      
    Crop->P_rt.Uptake_st = 0.;      
    Crop->P_rt.Uptake_ro = 0.;  
    
    Crop->K_rt.Uptake_lv = 0.;
    Crop->K_rt.Uptake_st = 0.;
    Crop->K_rt.Uptake_ro = 0.;         
    
    Total_N_demand = Crop->N_rt.Demand_lv + Crop->N_rt.Demand_st + Crop->N_rt.Demand_ro;
    Total_P_demand = Crop->P_rt.Demand_lv + Crop->P_rt.Demand_st + Crop->P_rt.Demand_ro;
    Total_K_demand = Crop->K_rt.Demand_lv + Crop->K_rt.Demand_st + Crop->K_rt.Demand_ro;
    
    NutrientLimit = insw(Crop->DevelopmentStage - Crop->prm.DevelopmentStageNLimit , 
            insw(WatBal->rt.Transpiration/Evtra.MaxTranspiration -0.01,0.,1.) , 0.0);
    
    /* Nutrient uptake cannot be larger than the availability and is larger or equal to zero */
    Crop->N_rt.Uptake = min((1.-Crop->prm.N_fixation)*Total_N_demand, Site->st_N_tot)* NutrientLimit/Step;
    Crop->P_rt.Uptake = min(Total_P_demand, Site->st_P_mins) * NutrientLimit/Step;
    Crop->K_rt.Uptake = min(Total_K_demand, Site->st_K_mins) * NutrientLimit/Step;
    
    N_Fix_rt= max(0.,Crop->N_rt.Uptake * Crop->prm.N_fixation / max(0.02, 1.-Crop->prm.N_fixation));
   
    /* N uptake per crop organ kg ha-1 d-1*/
    if (Total_N_demand > tiny)
    {
        Crop->N_rt.Uptake_lv = (Crop->N_rt.Demand_lv / Total_N_demand) * (Crop->N_rt.Uptake + N_Fix_rt);
        Crop->N_rt.Uptake_st = (Crop->N_rt.Demand_st / Total_N_demand) * (Crop->N_rt.Uptake + N_Fix_rt);
        Crop->N_rt.Uptake_ro = (Crop->N_rt.Demand_ro / Total_N_demand) * (Crop->N_rt.Uptake + N_Fix_rt);
    }
    
    /* P uptake per crop organ kg ha-1 d-1 */
    if (Total_P_demand > tiny)
    {
        Crop->P_rt.Uptake_lv = (Crop->P_rt.Demand_lv / Total_P_demand) * Crop->P_rt.Uptake;
        Crop->P_rt.Uptake_st = (Crop->P_rt.Demand_st / Total_P_demand) * Crop->P_rt.Uptake;
        Crop->P_rt.Uptake_ro = (Crop->P_rt.Demand_ro / Total_P_demand) * Crop->P_rt.Uptake; 
    }

    
   /* K uptake per crop organ kg ha-1 d-1*/
    if (Total_K_demand > tiny)
    {
        Crop->K_rt.Uptake_lv = (Crop->K_rt.Demand_lv / Total_K_demand) * Crop->K_rt.Uptake;
        Crop->K_rt.Uptake_st = (Crop->K_rt.Demand_st / Total_K_demand) * Crop->K_rt.Uptake;
        Crop->K_rt.Uptake_ro = (Crop->K_rt.Demand_ro / Total_K_demand) * Crop->K_rt.Uptake;   
    }
}    