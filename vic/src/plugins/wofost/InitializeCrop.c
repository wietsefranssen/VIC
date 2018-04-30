#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "vic.h"



/*---------------------------------------------------*/
/* function EmergenceCrop                            */
/* Purpose: determine if crop emergence has occurred */
/*---------------------------------------------------*/

int EmergenceCrop(int Emergence)
{
    float DeltaTempSum;
     
    /*  Emergence has not taken place yet*/
    if (!Emergence)
	{
            DeltaTempSum = limit(0, Crop->prm.TempEffMax - Crop->prm.TempBaseEmergence, 
                Temp-Crop->prm.TempBaseEmergence);
	    Crop->TSumEmergence += DeltaTempSum;
	    if (Crop->TSumEmergence >= Crop->prm.TSumEmergence)
            {
                Emergence = 1;
	    }
	}
    return Emergence;
}
    

/* ----------------------------------------------------------*/
/*  function InitializeCrop                                  */
/*  Purpose: Set the initial crop state and leave variables  */
/*  ---------------------------------------------------------*/ 

void InitializeCrop()
{ 
    float FractionRoots;
    float FractionShoots; 
    float InitialShootWeight;
   
    /* Initialize the crop states */
    Crop->DevelopmentStage = Crop->prm.InitialDVS;

    FractionRoots      = Afgen(Crop->prm.Roots, &(Crop->DevelopmentStage));
    FractionShoots     = 1 - FractionRoots;
    InitialShootWeight = Crop->prm.InitialDryWeight * FractionShoots;

    Crop->st.roots     = Crop->prm.InitialDryWeight * FractionRoots;
    Crop->RootDepth    = Crop->prm.InitRootingDepth;
    Crop->st.stems     = InitialShootWeight * Afgen(Crop->prm.Stems, &(Crop->DevelopmentStage));                   
    Crop->st.leaves    = InitialShootWeight * Afgen(Crop->prm.Leaves, &(Crop->DevelopmentStage));
    Crop->st.storage   = InitialShootWeight * Afgen(Crop->prm.Storage, &(Crop->DevelopmentStage));

    /* Adapt the maximum rooting depth */
    Crop->prm.MaxRootingDepth = max(Crop->prm.InitRootingDepth, min(Crop->prm.MaxRootingDepth,
         Site->SoilLimRootDepth));

    Crop->prm.LAIEmergence  = Crop->st.leaves * Afgen(Crop->prm.SpecificLeaveArea, &(Crop->DevelopmentStage)); 

    Crop->st.LAIExp = Crop->prm.LAIEmergence;

    Crop->st.LAI = Crop->prm.LAIEmergence + Crop->st.stems * 
           Afgen(Crop->prm.SpecificStemArea, &(Crop->DevelopmentStage)) +
           Crop->st.storage*Crop->prm.SpecificPodArea;
    
    /* Initialize the leaves */
    Crop->LeaveProperties         = malloc(sizeof (Green));
    Crop->LeaveProperties->age    = 0.;
    Crop->LeaveProperties->weight = Crop->st.leaves;
    Crop->LeaveProperties->area   = Afgen(Crop->prm.SpecificLeaveArea, &(Crop->DevelopmentStage));
    Crop->LeaveProperties->next   = NULL;
    
    /* Crop death rates set to zero */
    Crop->drt.leaves = 0.;
    Crop->drt.roots  = 0.;
    Crop->drt.stems  = 0.;
    
    /* Emergence true */
    Crop->Emergence = 1;
    Crop->GrowthDay = 1;
            
}  