#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include "vic.h"


/* ---------------------------------------------------------------------------*/
/*  function RespirationRef(float TotalAssimilation)                          */
/*  Purpose: Calculation of the crop respiration rate (kg ha-1 d-1). Note     */
/*  that the respiration rate can not exceed the total assimilation rate.     */
/* ---------------------------------------------------------------------------*/

float RespirationRef(float TotalAssimilation)
{
    float respiration; 
    float TempRef = 25.;

    respiration  = Crop->prm.RelRespiLeaves * Crop->st.leaves;
    respiration  += Crop->prm.RelRespiStorage * Crop->st.storage;
    respiration  += Crop->prm.RelRespiRoots * Crop->st.roots;	
    respiration  += Crop->prm.RelRespiStems * Crop->st.stems;
    respiration  *= Afgen(Crop->prm.FactorSenescence, &(Crop->DevelopmentStage));
    respiration  *= pow(Crop->prm.Q10, 0.1 * (Temp-TempRef));
    
    /* respiration can not exceed the assimilation */
    return (min(respiration, TotalAssimilation));
}
