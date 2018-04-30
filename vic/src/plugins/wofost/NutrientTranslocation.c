#include <stdio.h>
#include <stdlib.h>
#include "wofost.h"
#include "wofost/extern.h"

/* ---------------------------------------------------------------------------*/
/*  function Translocation()                                                  */ 
/*  Purpose: To compute the amount of nutrients in the organs that can        */
/*  be translocated kg ha-1                                                   */
/* ---------------------------------------------------------------------------*/
void NutrientTranslocation()                                                                                                       
{
     float Avail_N_lv;
     float Avail_P_lv; 
     float Avail_K_lv;
     
     float Avail_N_st;
     float Avail_P_st; 
     float Avail_K_st;      
     
     float Avail_N_rt; 
     float Avail_P_rt; 
     float Avail_K_rt; 
                                                                                                                      
    /* N amount available for translocation */                                                                                                              
    Avail_N_lv = max(0.,Crop->N_st.leaves - Crop->st.leaves * Crop->prm.N_ResidualFrac_lv);
    Avail_N_st = max(0.,Crop->N_st.stems  - Crop->st.stems  * Crop->prm.N_ResidualFrac_st);
    Avail_N_rt = max(0.,min((Crop->N_rt.Transloc_lv + Crop->N_rt.Transloc_st) * Crop->prm.FracTranslocRoots, 
            Crop->N_st.roots - Crop->st.roots * Crop->prm.N_ResidualFrac_ro));
   
    /* P amount available for translocation */ 
    Avail_P_lv = max (0.,Crop->P_st.leaves - Crop->st.leaves * Crop->prm.P_ResidualFrac_lv);
    Avail_P_st = max (0.,Crop->P_st.stems  - Crop->st.stems  * Crop->prm.P_ResidualFrac_st);
    Avail_P_rt = max (0.,min((Crop->P_rt.Transloc_lv + Crop->P_rt.Transloc_st) * Crop->prm.FracTranslocRoots, 
            Crop->P_st.roots - Crop->st.roots * Crop->prm.P_ResidualFrac_ro));
    
    /* K amount available for translocation */ 
    Avail_K_lv = max(0.,Crop->K_st.leaves - Crop->st.leaves * Crop->prm.K_ResidualFrac_lv);
    Avail_K_st = max(0.,Crop->K_st.stems  - Crop->st.stems  * Crop->prm.K_ResidualFrac_st);
    Avail_K_rt = max(0.,min((Crop->K_rt.Transloc_lv + Crop->K_rt.Transloc_st) * Crop->prm.FracTranslocRoots, 
            Crop->K_st.roots - Crop->st.roots * Crop->prm.P_ResidualFrac_ro));

    /* Total available nutrient amount for translocation */
    Crop->N_rt.Transloc = Avail_N_lv + Avail_N_st + Avail_N_rt;
    Crop->P_rt.Transloc = Avail_P_lv + Avail_P_st + Avail_P_rt;
    Crop->K_rt.Transloc = Avail_K_lv + Avail_K_st + Avail_K_rt;
    
    /* Actual N translocation rate */ 
    if (Crop->N_rt.Transloc > 0.001)
    {
        Crop->N_rt.Transloc_lv = Crop->N_rt.storage * Avail_N_lv / Crop->N_rt.Transloc;
        Crop->N_rt.Transloc_st = Crop->N_rt.storage * Avail_N_st / Crop->N_rt.Transloc;
        Crop->K_rt.Transloc_ro = Crop->N_rt.storage * Avail_N_rt / Crop->N_rt.Transloc;  
    }
    else
    {
        Crop->N_rt.Transloc_lv = 0.;       
        Crop->N_rt.Transloc_st = 0.;
        Crop->K_rt.Transloc_ro = 0.;
    }
    
    /* Actual P translocation rate */
    if (Crop->P_rt.Transloc > 0.001)
    {
        Crop->P_rt.Transloc_lv = Crop->P_rt.storage * Avail_P_lv / Crop->P_rt.Transloc;
        Crop->P_rt.Transloc_st = Crop->P_rt.storage * Avail_P_st / Crop->P_rt.Transloc;
        Crop->P_rt.Transloc_ro = Crop->P_rt.storage * Avail_P_rt / Crop->P_rt.Transloc;  
    }
    else
    {
        Crop->P_rt.Transloc_lv = 0.;       
        Crop->P_rt.Transloc_st = 0.;
        Crop->P_rt.Transloc_ro = 0.;
    }
    
    /* Actual K translocation rate */
    if (Crop->K_rt.Transloc > 0.001)
    {
        Crop->K_rt.Transloc_lv = Crop->K_rt.storage * Avail_K_lv / Crop->K_rt.Transloc;
        Crop->K_rt.Transloc_st = Crop->K_rt.storage * Avail_K_st / Crop->K_rt.Transloc;
        Crop->K_rt.Transloc_ro = Crop->K_rt.storage * Avail_K_rt / Crop->K_rt.Transloc;  
    }
    else
    {
        Crop->K_rt.Transloc_lv = 0.;       
        Crop->K_rt.Transloc_st = 0.;
        Crop->K_rt.Transloc_ro = 0.;
    }
}   