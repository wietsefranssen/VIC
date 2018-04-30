/* original: Penman.for from I.G.A.M. Noy and C.A. van Diepen, 
   van Kraalingen, and Allard de Wit, Sep 2011
*/

#include <stdio.h>
#include <math.h>
#include "wofost/extern.h"
#include "wofost/penman.h"
#include "wofost.h"

#define PSYCON  0.67    // psychrometric instrument constant (mbar/Celsius-1)
#define REFCFW  0.05    // albedo for a water surface                        
#define REFCFS  0.15    // albedo for a soil surface                         
#define REFCFC  0.25    // albedo for a  canopy                              
#define LHVAP   2.45e6  // latent heat of evaporation of water (J/kg=J/mm)  
#define STBC    4.9e-3  // Stefan Boltzmann constant (J/m2/d/K4) */

/* ---------------------------------------------------------------------*/
/*  function CalcPenman()                                               */
/*  Purpose: Calculation of ETO evapotranspiration     mm d-1           */
/*                          ESO soil evaporation       mm d-1           */
/*                          EO  open water evaporation mm d-1           */
/*                                                                      */
/*     Originally written in Fortran by:                                */
/*         I.G.A.M. Noy and C.A. van Diepen, September 1986             */
/*         revised van Kraalingen, April, van Diepen, October 1991      */
/*         revised van Kraalingen, and Allard de Wit, Sep 2011          */
/* ---------------------------------------------------------------------*/

void CalcPenman()
{
    float RelSunShineDuration;
    float Tmpa;
    float Tdif;
    float BU;
    float Pbar;
    float Gamma;
    float Ea;
    float Eac;
    float delta;
    float RB;
    float Rnw; 
    float Rns; 
    float Rnc;
    float VapourP; 
    float SaturatedVap;
            
    /* Preparatory calculations: mean daily temperature, temperature difference */
    /* (Celsius) and the Bu coefficient Bu of the wind function (depends  on    */ 
    /* temperature difference)                                                  */
    
    Tmpa  = (Tmin[Day] + Tmax[Day])/2.;
    Tdif  = Tmax[Day] - Tmin[Day];
    BU    = 0.54 + 0.35 * limit(0.,1.,(Tdif-12.)/4.);

    /* Barometric pressure (mbar)             */
    /* Psychrometric constant (mbar/Celsius)  */
    Pbar  = 1013.*exp (-0.034*Altitude/(Tmpa + 273.));
    Gamma = PSYCON * Pbar/1013.;


    /* Saturated vapour pressure according to equation of Goudriaan     */
    /* (1977) derivative of SVAP with respect to temperature, i.e.      */
    /* slope of the SVAP-temperature curve (mbar/Celsius).              */
            
    /* Measured vapour pressure not to exceed saturated vapour pressure */

    SaturatedVap  = 6.10588 * exp(17.32491 * Tmpa/(Tmpa+238.102));
    delta         = 238.102 * 17.32491 * SaturatedVap/pow((Tmpa +238.102),2);
    VapourP       = min(Vapour[Day],SaturatedVap);

    /* The expression n/N (RelLSSD) from the Penman formula is estimated   */
    /* from the Angstrom formula: RI=RA(A+B.n/N) -> n/N=(RI/RA-A)/B,       */
    /* where RI/RA is the atmospheric transmission obtained by a CALL      */
    /* to ASTRO: */
              
    RelSunShineDuration = limit(0.,1.,(AtmosphTransm-AngstA)/AngstB);

    /* Terms in Penman formula, for water, soil and canopy            */
    /* Net outgoing long-wave radiation (J/m2/d) acc. to Brunt (1932) */
    RB  = STBC * pow((Tmpa+273.),4) * (0.56-0.079 * sqrt(VapourP)) *
              (0.1 + 0.9 * RelSunShineDuration);

    /* Net absorbed radiation, expressed in mm/d */
    Rnw = (Radiation[Day] * (1.-REFCFW)-RB)/LHVAP;
    Rns = (Radiation[Day] * (1.-REFCFS)-RB)/LHVAP;
    Rnc = (Radiation[Day] * (1.-REFCFC)-RB)/LHVAP;

    /* Evaporative demand of the atmosphere (mm/d)  */
    Ea  = 0.26 * max (0.,(SaturatedVap-VapourP)) * (0.5+BU * Windspeed[Day]);
    Eac = 0.26 * max (0.,(SaturatedVap-VapourP)) * (1.0+BU * Windspeed[Day]);
   
    /* Penman formula (1948)                */
    /* Ensure reference evaporation >= 0.   */
    /* Convert to cm/day                    */
    Penman.E0  = max(0., 0.1 * (delta*Rnw + Gamma*Ea)/(delta + Gamma));
    Penman.ES0 = max(0., 0.1 * (delta*Rns + Gamma*Ea)/(delta + Gamma));
    Penman.ET0 = max(0., 0.1 * (delta*Rnc + Gamma*Eac)/(delta + Gamma));
    
    //printf("%4d %6.2f %6.2f %6.2f\n", Day, Penman.E0, Penman.ES0, Penman.ET0 );
    
}