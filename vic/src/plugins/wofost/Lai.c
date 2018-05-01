#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "vic.h"

/* ---------------------------------------------------------------------------*/
/*  function LeaveGrowth(float LAIExp, float NewLeaves)                       */
/*  Purpose: Calculation of the daily leaves growth rate, the results are     */
/*           stored in the Crop->properties linked list                       */
/* ---------------------------------------------------------------------------*/

float LeaveGrowth(float LAIExp, float NewLeaves)
{
    float GrowthExpLAI;
    float GrowthSourceLimited;
    float SpecLeafArea; 
    float Stress;

    wofost_green *New = NULL;
    wofost_green *LeaveProperties = NULL;


    /* Specific Leaf area(m2/g), as dependent on NPK stress */
    SpecLeafArea = Afgen(Crop->prm.SpecificLeaveArea, &(Crop->DevelopmentStage)) * 
        exp(-Crop->prm.NutrientStessSLA * (1.-Crop->NPK_Indx));

    /* Leave area not to exceed exponential growth */
    if (LAIExp < 6 && NewLeaves > 0.) 
    {
        /* Growth during juvenile stage */
        GrowthExpLAI = LAIExp * Crop->prm.RelIncreaseLAI * max(0.,Temp - Crop->prm.TempBaseLeaves);
        if (Crop->DevelopmentStage < 0.2 && Crop->st.LAI < 0.75)
        {
          Stress = WatBal->WaterStress * exp(-Crop->prm.NitrogenStressLAI * (1. - Crop->N_st.Indx));
        }
        else
        {
            Stress = 1.;
        }

        /* Correction for nutrient stress */
        GrowthExpLAI = GrowthExpLAI * Stress;

        /* Source limited leaf area increase */
        GrowthSourceLimited = NewLeaves* Afgen(Crop->prm.SpecificLeaveArea, &(Crop->DevelopmentStage));

        /* Sink-limited leaf area increase */
        SpecLeafArea = min(GrowthExpLAI, GrowthSourceLimited)/NewLeaves;
    }
    else
    {
        GrowthExpLAI = 0.;
    }
    
        
    New         = malloc(sizeof(wofost_green));
    New->weight = NewLeaves;
    New->age    = 0.;
    New->area   = SpecLeafArea;
    New->next   = NULL;

    /* Store the start address */
    LeaveProperties = Crop->LeaveProperties;

    /* Loop until the last element in the list */
    while (Crop->LeaveProperties->next != NULL)
    {
        Crop->LeaveProperties = Crop->LeaveProperties->next; 
    }

    /* Add new element to the list */
    Crop->LeaveProperties->next = New;

    /* Restore the start address */
    Crop->LeaveProperties = LeaveProperties;

    return GrowthExpLAI;
}
