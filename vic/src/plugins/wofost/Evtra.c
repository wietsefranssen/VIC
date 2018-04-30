/* original: Penman.for from I.G.A.M. Noy and C.A. van Diepen, */
/* van Kraalingen, and Allard de Wit, Sep 2011                 */

#include <stdio.h>
#include <math.h>
#include "wofost/penman.h"
#include "wofost.h"
#include "wofost/extern.h"

/* ---------------------------------------------------------------*/
/*  function sweaf()                                              */
/*  Purpose: Calculates the Soil Water Easily Available Fraction  */
/*                                                                */
/* Chapter 20 in documentation WOFOST Version 4.1 (1988)          */
/*                                                                */
/*    The fraction of easily available soil water between         */
/*    field capacity and wilting point is a function of the       */
/*    potential evapotranspiration rate (for a closed canopy)     */
/*    in cm/day, ET0, and the crop group number, CGNR (from       */
/*    1 (=drought-sensitive) to 5 (=drought-resistent)). The      */
/*    function SWEAF describes this relationship given in tabular */
/*    form by Doorenbos & Kassam (1979) and by Van Keulen & Wolf  */
/*    (1986; p.108, table 20).                                    */
/*    Original fortran version: D.M. Jansen and C.A. van Diepen,  */
/*    October 1986.                                               */
/* ---------------------------------------------------------------*/ 
float sweaf(){
    float sweaf; 
    sweaf = 1./(0.76 + 1.5 * Penman.ET0) - (5.-Crop->prm.CropGroupNumber ) * 0.10;
    if  (Crop->prm.CropGroupNumber < 3.) {
        sweaf = sweaf + (Penman.ET0 - 0.6)/(Crop->prm.CropGroupNumber *
                (Crop->prm.CropGroupNumber + 3.));
    }
    return limit(0.10, 0.95, sweaf);

}
    
/* -----------------------------------------------------------------*/
/*  function EvapTra()                                              */
/*  Purpose: Calculates the water stress and the transpiration rate */
/* -----------------------------------------------------------------*/     
void EvapTra() {   
    float CriticalSoilMoisture;
    float KDiffuse;
    float MaxReductionOxygenStress;
    float MoistureStress;
    float OxygenStress;
    float SoilMoistureAeration;
    float SoilWatDepletion;
    
    /* crop specific correction on potential evapotranspiration rate */
    Penman.ET0 = Penman.ET0 * Crop->prm.CorrectionTransp;
    
    KDiffuse = Afgen(Crop->prm.KDiffuseTb, &(Crop->DevelopmentStage));      
    Evtra.MaxEvapWater = Penman.E0 * exp(-0.75 * KDiffuse * Crop->st.LAI);
    Evtra.MaxEvapSoil  = max(0., Penman.ES0 * exp(-0.75 * KDiffuse * Crop->st.LAI));
    Evtra.MaxTranspiration = max(0.0001,  
                             Penman.ET0 * Afgen(Crop->prm.CO2TRATB, &CO2) *
                             (1.-exp(-0.75 * KDiffuse * Crop->st.LAI)));
       
    SoilWatDepletion = sweaf();
    CriticalSoilMoisture = (1. - SoilWatDepletion)*
            (WatBal->ct.MoistureFC - WatBal->ct.MoistureWP) + WatBal->ct.MoistureWP;
    
    /* Transpiration reduction in case of water shortage */
    MoistureStress = limit(0.,1.,(WatBal->st.Moisture - WatBal->ct.MoistureWP)/
            (CriticalSoilMoisture - WatBal->ct.MoistureWP));
    
    if (!Crop->prm.Airducts) 
    {
        /* Critical soil moisture content for aeration */
        SoilMoistureAeration = WatBal->ct.MoistureSAT - WatBal->ct.CriticalSoilAirC;
        
        /* Count days since start oxygen shortage (up to 4 days) */
        if (WatBal->st.Moisture >= SoilMoistureAeration) {
            Crop->DaysOxygenStress = min(Crop->DaysOxygenStress++, 4.);
        }
        else 
        {
            Crop->DaysOxygenStress = 0.;
        }
        
        /* Maximum reduction reached after 4 days */
        MaxReductionOxygenStress = limit (0.,1.,(WatBal->ct.MoistureSAT - WatBal->st.Moisture)/
                (WatBal->ct.MoistureSAT - SoilMoistureAeration));
        
        OxygenStress   = MaxReductionOxygenStress + 
                (1.-Crop->DaysOxygenStress/4.)*(1.-MaxReductionOxygenStress);        
    }
    else 
    {
        OxygenStress = 1.;
    }
    
    WatBal->WaterStress = MoistureStress * OxygenStress;
    
     
    WatBal->rt.Transpiration = WatBal->WaterStress * Evtra.MaxTranspiration;
    }
