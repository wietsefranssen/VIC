#include <stdio.h>
#include "wofost/extern.h"
#include "wofost.h"

/* ----------------------------------------------------------------------------*/
/*  function GetDevelopmentStage()                                             */
/*  Purpose: Calculation of the development stage as function of the effective */
/*  daily temperature, day length and the vernalization                        */
/*-----------------------------------------------------------------------------*/

float GetDevelopmentStage(void)
{
    float DevelopmentRate;
    float VernalizationFactor;

    if (Crop->DevelopmentStage  < 1.)
    {
        DevelopmentRate = Afgen(Crop->prm.DeltaTempSum, &Temp)/Crop->prm.TempSum1;
        if (Crop->prm.IdentifyAnthesis == 1 || Crop->prm.IdentifyAnthesis == 2) DevelopmentRate = DevelopmentRate *
                limit(0., 1., (PARDaylength - Crop->prm.CriticalDaylength)/(Crop->prm.OptimumDaylength-Crop->prm.CriticalDaylength));
        
        /* Vernalization takes place */
        if (Crop->prm.IdentifyAnthesis == 2)
        {
            VernalizationFactor = limit(0., 1., 
                    (Crop->st.vernalization - Crop->prm.BaseVernRequirement)/
                    (Crop->prm.SatVernRequirement - Crop->prm.BaseVernRequirement));
            
	    DevelopmentRate = DevelopmentRate * insw(Crop->DevelopmentStage - 0.3, VernalizationFactor, 1.);
        }
        
        /* Adjustment of the development stage */
        if (DevelopmentRate + Crop->DevelopmentStage >= 1. ) return 1.;
    }
    else 
         DevelopmentRate = Afgen(Crop->prm.DeltaTempSum, &Temp)/Crop->prm.TempSum2;  

    return (Crop->DevelopmentStage + DevelopmentRate);
}
