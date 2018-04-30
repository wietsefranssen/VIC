#include <stdio.h>
#include <math.h>

#include "vic.h"

#define  ANGLE  -4.0
#define  PI     3.1415926
#define  RAD	0.0174533

float ScatCoef =0.2;
float XGauss[] ={0.1127017, 0.5000000, 0.8872983};
float WGauss[] ={0.2777778, 0.4444444, 0.2777778};


/* ----------------------------------------------------------------------------*/
/*  function InstantAssimilation(float KDiffuse, float EFF, float AssimMax,    */
/*                            float SinB, float PARDiffuse, float PARDirect)   */ 
/*  Purpose: Calculation of the instant Assimilation rate as a function of     */
/*  radiation using the three point Gaussian integration method.               */
/*-----------------------------------------------------------------------------*/

float InstantAssimilation(float KDiffuse, float EFF, float AssimMax, float SinB, 
        float PARDiffuse, float PARDirect)
{
    int i;
    float AbsorbedRadiationDiffuse, AbsorbedRadiationTotal, AbsorbedRadiationDirect;
    float AbsorbedShadedLeaves, AbsorbedDirectLeaves;
    float AssimShadedLeaves, AssimSunlitLeaves, AssimTotal;
    float Reflection, KDirectBl, KDirectTl;
    float GrossCO2, FractionSunlitLeaves, LAIC ;

    /* Extinction coefficients KDIF,KDIRBL,KDIRT */
    Reflection  = (1.-sqrt(1.-ScatCoef))/(1.+sqrt(1.-ScatCoef))*(2/(1+1.6*SinB));
    KDirectBl   = (0.5/SinB)*KDiffuse/(0.8*sqrt(1.-ScatCoef));
    KDirectTl   = KDirectBl*sqrt(1.-ScatCoef);

    /* Three-point Gaussian integration over LAI */
    GrossCO2  = 0.;
    for (i=0;i<3;i++)
    {
       LAIC   = Crop->st.LAI*XGauss[i];
        
       /* Absorbed radiation */
       AbsorbedRadiationDiffuse = (1.-Reflection)*PARDiffuse*KDiffuse * exp(-KDiffuse * LAIC);
       AbsorbedRadiationTotal   = (1.-Reflection)*PARDirect*KDirectTl * exp(-KDirectTl * LAIC);
       AbsorbedRadiationDirect  = (1.-ScatCoef)  *PARDirect*KDirectBl * exp(-KDirectBl * LAIC);

       /* Absorbed flux in W/m2 for shaded leaves and assimilation */
       AbsorbedShadedLeaves = AbsorbedRadiationDiffuse  + AbsorbedRadiationTotal - AbsorbedRadiationDirect;
       AssimShadedLeaves    = AssimMax*(1.-exp (-AbsorbedShadedLeaves*EFF/max(2.0,AssimMax)));

       /* Direct light absorbed by leaves perpendicular on direct */
       /* beam and assimilation of sunlit leaf area               */
       AbsorbedDirectLeaves=(1 - ScatCoef)*PARDirect/SinB;
       if (AbsorbedDirectLeaves <= 0) AssimSunlitLeaves = AssimShadedLeaves;
       else AssimSunlitLeaves = AssimMax*(1. - (AssimMax - AssimShadedLeaves)*
              (1 - exp( -AbsorbedDirectLeaves*EFF/max(2.0,AssimMax)))/(EFF*AbsorbedDirectLeaves));

        /*  Fraction of sunlit leaf area and local assimilation rate  */ 
        FractionSunlitLeaves  = exp(-KDirectBl*LAIC);
        AssimTotal = FractionSunlitLeaves*AssimSunlitLeaves + (1. - FractionSunlitLeaves)*AssimShadedLeaves;

        /*  Integration */
        GrossCO2 += AssimTotal * WGauss[i];
    }
    
    return (GrossCO2 * Crop->st.LAI);     
}


/* ----------------------------------------------------------------------------*/
/*  function DailyTotalAssimilation()                                          */ 
/*  Purpose: Calculation of the daily assimilation rate using the three point  */
/*  Gaussian integration method.                                               */
/*-----------------------------------------------------------------------------*/
float DailyTotalAssimilation()
{
    int i;
    float KDiffuse, EFF, Factor;
    float Hour, SinB, PAR, PARDiffuse, PARDirect, AssimMax; 
    float DailyTotalAssimilation = 0.;

    KDiffuse = Afgen(Crop->prm.KDiffuseTb, &(Crop->DevelopmentStage));

    EFF      = Afgen(Crop->prm.EFFTb, &DayTemp);
    Factor   = Afgen(Crop->prm.CO2EFFTB, &CO2);

    /* Correction for the atmospheric CO2 concentration */
    EFF      = EFF * Factor ;

    AssimMax = Afgen(Crop->prm.FactorAssimRateTemp, &DayTemp) * 
               Afgen(Crop->prm.MaxAssimRate, &(Crop->DevelopmentStage)) * 
               Afgen(Crop->prm.CO2AMAXTB, &CO2);

    if (AssimMax > 0. && Crop->st.LAI > 0.)
    {
        for (i=0;i<3;i++)
        {
            Hour       = 12.0+0.5*Daylength*XGauss[i];
            SinB       = max (0.,SinLD+CosLD*cos(2.*PI*(Hour+12.)/24.));
            PAR        = 0.5*Radiation[Day]*SinB*(1.+0.4*SinB)/DSinBE;
            PARDiffuse = min (PAR,SinB*DiffRadPP);
            PARDirect  = PAR-PARDiffuse;
            DailyTotalAssimilation = DailyTotalAssimilation + 
                InstantAssimilation(KDiffuse,EFF,AssimMax,SinB,PARDiffuse,PARDirect) * WGauss[i];
        }  
    }
    return(DailyTotalAssimilation*Daylength);
}


/* ----------------------------------------------------------------------------*/
/*  function Correct()                                                         */ 
/*  Purpose: Correct the daily assimilation rate for low temperatures          */
/*-----------------------------------------------------------------------------*/
float Correct(float Assimilation)
{
    int PreviousDay, Counter;
    float TminLowAvg = 0.;

    Counter = 0;
    PreviousDay = Day;
    while (PreviousDay >= 0 && Counter < 7)
    {
      TminLowAvg += Tmin[PreviousDay--]; 
      Counter++;
    }

    TminLowAvg = TminLowAvg/Counter;
    return (Assimilation*Afgen(Crop->prm.FactorGrossAssimTemp, &TminLowAvg)*30./44.);

}
