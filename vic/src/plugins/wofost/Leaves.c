#include <stdio.h>
#include <stdlib.h>
#include "wofost/extern.h"
#include "wofost/wofost.h"
#include "wofost/penman.h"

/* ------------------------------------------------------------------------*/
/*  function DyingLeaves()                                                 */
/*  Purpose: To compute the amount of dying leaves as a result of shading, */
/*  (i.e. high LAI), stress or age                                         */
/* ------------------------------------------------------------------------*/

float DyingLeaves()
{
    float tiny = 0.001;
    float Death, Death1, Death2, DeathStress, DeathAge;
    float CriticalLAI;
    Green *wipe;

    /* Dying rate of leaves due to water stress or high LAI */
    CriticalLAI = 3.2/Afgen(Crop->prm.KDiffuseTb, &(Crop->DevelopmentStage));
    Death1      = Crop->st.leaves *(1. - WatBal->rt.Transpiration / 
                  Evtra.MaxTranspiration) * Crop->prm.MaxRelDeathRate ;
    Death2      = Crop->st.leaves * limit(0.,0.03, 0.03*(Crop->st.LAI-CriticalLAI)/CriticalLAI);
    Death       = max(Death1, Death2);  

    /* Death rate increase due to nutrient shortage */
    Death += Crop->st.leaves * Crop->prm.DyingLeaves_NPK_Stress * (1. - Crop->NPK_Indx);

    if (Death < tiny) Death = 0;
        
    DeathStress = Death;

    if (Crop->LeaveProperties != NULL)
    {
        /* Oldest leave classes are at the beginning of the list */
        while(Death > Crop->LeaveProperties->weight)
        { 
            Death                 = Death - Crop->LeaveProperties->weight;
            wipe                  = Crop->LeaveProperties; 
            Crop->LeaveProperties = Crop->LeaveProperties->next; 
            free(wipe);
        }
    }

   
    DeathAge = 0; // Amount of death leaves due to ageing
    if (Crop->LeaveProperties != NULL)
    {
        Crop->LeaveProperties->weight = Crop->LeaveProperties->weight - Death;

        while(Crop->LeaveProperties != NULL && Crop->LeaveProperties->age > Crop->prm.LifeSpan)
        {
            wipe                   = Crop->LeaveProperties; 
            DeathAge              += Crop->LeaveProperties->weight;
            Crop->LeaveProperties  = Crop->LeaveProperties->next; 
            free(wipe);
        }
    }
    
    /* Return the amount of death leaves due to stress and due to ageing */
    return (DeathStress + DeathAge); 
}
