#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "vic.h"

#define  ANGLE  -4.0
#define  PI     3.1415926
#define  RAD	0.0174533

/* ---------------------------------------------------------------------*/
/*  function Astro()                                                    */
/*  Purpose: Calculation of the astronomical parameters used in Wofost  */
/*                                                                      */
/*  Originally written in Fortran by:                                   */
/*         Daniel van Kraalingen, April 1991                            */
/*         revised Allard de Wit, January 2011                          */
/* ---------------------------------------------------------------------*/

int Astro()
{
    float Declination;
    float SolarConstant;
    float AOB;
    float DSinB;
    float FractionDiffuseRad;
    float AngotRadiation;
    
    if (fabsf(Latitude) > 90.) return 0;  

    /* We start at Day= 1, we do not use Day = 0 */
    Declination    = -asin(sin(23.45*RAD)*cos(2.*PI*(float)(Day+10.)/365.));
    SolarConstant  = 1370.*(1.+0.033*cos(2.*PI*(float)(Day)/365.));
  
    SinLD = sin(RAD*Latitude)*sin(Declination);
    CosLD = cos(RAD*Latitude)*cos(Declination);
    AOB   = SinLD/CosLD;
    
   /* Astronomical day length */
    Daylength = max(0,min(24.,12.0*(1.+2.*asin(AOB)/PI)));
    
    /* Photoactive day length */
    PARDaylength = max(0,min(24.,12.0*(1.+2.*asin((-sin(ANGLE*RAD)+SinLD)/CosLD)/PI)));
    
    /* Integrals of sine of solar height */
    if (AOB <= 1.0)
    {   
        DSinB  = 3600.*(Daylength*SinLD+(24./PI)*CosLD*sqrt(1.-AOB*AOB));
        DSinBE = 3600.*(Daylength*(SinLD+0.4*(SinLD*SinLD + CosLD*CosLD*0.5))+
            12.*CosLD*(2.+3.*0.4*SinLD)*sqrt(1.-AOB*AOB)/PI);
    }  
    else
    {
        DSinB  = 3600.*(Daylength*SinLD);
        DSinBE = 3600.*(Daylength*(SinLD+0.4*(SinLD*SinLD + CosLD*CosLD*0.5)));
    }
    
    /*  Extraterrestrial radiation and atmospheric transmission */
    AngotRadiation  = SolarConstant*DSinB;
    AtmosphTransm   = Radiation[Day]/AngotRadiation;

    if (AtmosphTransm > 0.75)
       FractionDiffuseRad = 0.23;
  
    if (AtmosphTransm <= 0.75 && AtmosphTransm > 0.35)
       FractionDiffuseRad = 1.33-1.46 * AtmosphTransm;
  
    if (AtmosphTransm <= 0.35 && AtmosphTransm > 0.07) 
       FractionDiffuseRad = 1.-2.3*pow((AtmosphTransm-0.07), 2.);
  
    if (AtmosphTransm < 0.07)  
       FractionDiffuseRad = 1.0;
    
    DiffRadPP = 0.5 * FractionDiffuseRad * AtmosphTransm * SolarConstant;

    return 1;
}
