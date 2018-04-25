#include <stdio.h>
#include <stdlib.h>
#include "wofost/wofost.h"
#include "wofost/extern.h"

/* -----------------------------------------------------------------------*/
/*  function LeaveAreaIndex()                                             */
/*  Purpose: Calculation of the LAI of the LeaveProperties ha ha-1 struct */
/* -----------------------------------------------------------------------*/
float LeaveAreaIndex()
{
    float LAISum = 0.;
    Green *LeaveProperties;

    /* Store the initial address */
    LeaveProperties = Crop->LeaveProperties;

    /* Loop until the last element in the list */
    while (Crop->LeaveProperties) 
    {
        LAISum += Crop->LeaveProperties->weight * Crop->LeaveProperties->area;
        Crop->LeaveProperties = Crop->LeaveProperties->next;
    }

    /* Return to the initial address */
    Crop->LeaveProperties = LeaveProperties; 
 
    /* Return Green Area Index which will be used as LAI */     
    return (LAISum + Crop->st.stems * Afgen(Crop->prm.SpecificStemArea, &(Crop->DevelopmentStage)) +
	     Crop->st.storage * Crop->prm.SpecificPodArea) ;
}
