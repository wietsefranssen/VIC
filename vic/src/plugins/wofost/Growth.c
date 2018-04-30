#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "wofost.h"
#include "wofost/extern.h"

/* ---------------------------------------------------------------------------*/
/*  function Growth(float NewPlantMaterial)                                   */
/*  Purpose: Establish growth rates of the plant organs (kg ha-1 d-1) and     */
/*  rooting depth (cm)                                                        */
/* ---------------------------------------------------------------------------*/

void Growth(float NewPlantMaterial)
{
    float shoots;
    float factor;
    float flv;
    
    float Fraction_ro;
    float Fraction_lv;
    float Fraction_st;
    float Fraction_so;
    
    float RootGrowth;
        
    /* Water stress is more severe as compared to Nitrogen stress and */
    /* partitioning will follow the original assumptions of LINTUL2   */     
        
    if (WatBal->WaterStress < Crop->N_st.Indx)
    {
        factor = max(1., 1./(WatBal->WaterStress + 0.5));
        Fraction_ro = min(0.6, Afgen(Crop->prm.Roots, &(Crop->DevelopmentStage)) * factor);
        Fraction_lv = Afgen(Crop->prm.Leaves, &(Crop->DevelopmentStage));
        Fraction_st = Afgen(Crop->prm.Stems, &(Crop->DevelopmentStage));
        Fraction_so = Afgen(Crop->prm.Storage, &(Crop->DevelopmentStage));
    }
    else
    {
        flv = Afgen(Crop->prm.Leaves, &(Crop->DevelopmentStage));
        factor = exp(-Crop->prm.N_lv_partitioning * ( 1. - Crop->N_st.Indx));
        
        Fraction_lv = flv * factor;
        Fraction_ro = Afgen(Crop->prm.Roots, &(Crop->DevelopmentStage));
        Fraction_st = Afgen(Crop->prm.Stems, &(Crop->DevelopmentStage)) + flv - Fraction_lv;
        Fraction_so = Afgen(Crop->prm.Storage, &(Crop->DevelopmentStage));
    }
                
    Crop->drt.roots = Crop->st.roots * Afgen(Crop->prm.DeathRateRoots, &(Crop->DevelopmentStage));
    Crop->rt.roots  = NewPlantMaterial * Fraction_ro - Crop->drt.roots;
	
    shoots         = NewPlantMaterial * (1-Fraction_ro);
	    
    Crop->drt.stems = Crop->st.stems * Afgen(Crop->prm.DeathRateStems, &(Crop->DevelopmentStage));	
    Crop->rt.stems  = shoots * Fraction_st - Crop->drt.stems;
	
    Crop->rt.storage = shoots * Fraction_so;
	
    Crop->drt.leaves = DyingLeaves(); 
    Crop->rt.leaves  = shoots * Fraction_lv;
    Crop->rt.LAIExp  = LeaveGrowth(Crop->st.LAIExp, Crop->rt.leaves);	
    Crop->rt.leaves  = Crop->rt.leaves -  Crop->drt.leaves;
	
    Crop->RootDepth_prev = Crop->RootDepth;
    
    /* No Root growth if no assimilates are partitioned to the roots or if */
    /* the crop has no airducts and the roots are close to the groundwater */
    if (Fraction_ro <= 0.0 || (!Crop->prm.Airducts && Site->GroundwaterDepth - Crop->RootDepth < 10.))
        RootGrowth = 0.;
    else
        RootGrowth = min(Crop->prm.MaxRootingDepth - Crop->RootDepth,
                Crop->prm.MaxIncreaseRoot*Step);
    
     Crop->RootDepth += RootGrowth;
}	
