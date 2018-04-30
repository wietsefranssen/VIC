#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "wofost/penman.h"
#include "vic.h"


/*------------------------------------------------*/
/* function InitializeWatBal                      */
/* Purpose: Initialize the water balance (WatBal) */
/*------------------------------------------------*/

void InitializeWatBal()
{ 
    float KDiffuse;
    
    /* Crop Growth has not started yet */
    WatBal->SoilMaxRootingDepth = 0.;
    
    
    /* Assume no water stress at initialization */
    WatBal->WaterStress = 1.;
    
    /* Check initial soil moisture. It cannot be larger than the              */
    /* saturated soil moisture SoilMoistureSAT or smaller than SoilMoistureWP */
    if (Site->MaxInitSoilM < WatBal->ct.MoistureWP)  
            Site->MaxInitSoilM = WatBal->ct.MoistureWP;
    if (Site->MaxInitSoilM > WatBal->ct.MoistureSAT) 
            Site->MaxInitSoilM = WatBal->ct.MoistureSAT;
    
    /* Set initial surface storage */
    WatBal->st.SurfaceStorage = Site->SurfaceStorage;
    
    /* Initial soil moisture for a rice crop */
    if (Crop->prm.Airducts) Site->MaxInitSoilM = WatBal->ct.MoistureSAT; 
    
    WatBal->st.Moisture = limit(WatBal->ct.MoistureWP, Site->MaxInitSoilM, 
            WatBal->ct.MoistureWP + Site->InitSoilMoisture/Crop->RootDepth);
    
    /* Initial moisture amount in rootable zone */
    WatBal->st.RootZoneMoisture = WatBal->st.Moisture * Crop->RootDepth;
    
    /*  Soil evaporation, days since last rain */
    WatBal->DaysSinceLastRain = 1.;
    if (WatBal->st.Moisture <= (WatBal->ct.MoistureWP + 
            0.5*(WatBal->ct.MoistureFC - WatBal->ct.MoistureWP))) 
            WatBal->DaysSinceLastRain = 5.;
    
    /* Moisture amount between rooted zone and max.rooting depth */
    WatBal->st.MoistureLOW  = limit (0., WatBal->ct.MoistureSAT
        *(Crop->prm.MaxRootingDepth - Crop->RootDepth), 
        Site->InitSoilMoisture + Crop->prm.MaxRootingDepth * WatBal->ct.MoistureWP - 
            WatBal->st.RootZoneMoisture);
    
    KDiffuse = Afgen(Crop->prm.KDiffuseTb, &(Crop->DevelopmentStage));
    WatBal->rt.EvapSoil = max(0., Penman.ES0 * exp(-0.75 * KDiffuse * Crop->st.LAI));
   
}

/*---------------------------------------------------*/
/* function RateCalulationWatBal                     */
/* Purpose: Calculate the rate of the WatBal struct  */
/*---------------------------------------------------*/

void RateCalulationWatBal() {
   
    float Available;
    float CMaxSoilEvap;
    float Perc1, Perc2;
    float WaterEq;
    float WELOW;
    
    /* If surface storage > 1 cm */
    if (WatBal->st.SurfaceStorage > 1.) 
    {
        WatBal->rt.EvapWater = Evtra.MaxEvapWater;
    }
    else 
    {
        if (WatBal->rt.Infiltration >= 1.) 
        {
            WatBal->rt.EvapSoil = Evtra.MaxEvapSoil;
            WatBal->DaysSinceLastRain = 1.;
        }
        else 
        {
            WatBal->DaysSinceLastRain++;
            CMaxSoilEvap = Evtra.MaxEvapSoil*(sqrt(WatBal->DaysSinceLastRain) - 
                    sqrt(WatBal->DaysSinceLastRain - 1));
            WatBal->rt.EvapSoil = min(Evtra.MaxEvapSoil, CMaxSoilEvap + 
                    WatBal->rt.Infiltration);
        }
    }
    
    /* Preliminary infiltration rate */
    if (WatBal->st.SurfaceStorage <= 0.1) 
    {
        /* Without surface storage */
        if (Site->InfRainDependent) WatBal->rt.Infiltration = 
               (1.-Site->NotInfiltrating * Afgen(Site->NotInfTB,Rain[Day])) * 
               Rain[Day] + WatBal->rt.Irrigation + WatBal->st.SurfaceStorage/Step;
        else
            WatBal->rt.Infiltration = (1.-Site->NotInfiltrating) * Rain[Day] + 
                WatBal->rt.Irrigation + WatBal->st.SurfaceStorage / Step;
    }
    else 
    {
        /* Surface storage, infiltration limited by maximum percolation */
        /* rate root zone */
        Available = WatBal->st.SurfaceStorage + (Rain[Day] * 
                (1.-Site->NotInfiltrating) + WatBal->rt.Irrigation 
                 - WatBal->rt.EvapSoil) * Step;
        WatBal->rt.Infiltration = min(WatBal->ct.MaxPercolRTZ * Step, 
                Available) / Step;
    }
    
    /* Equilibrium amount of soil moisture in rooted zone*/
    WaterEq = WatBal->ct.MoistureFC * Crop->RootDepth;
    
    /* Percolation from rooted zone to subsoil equals amount of   */
    /* Excess moisture in rooted zone (not to exceed conductivity)*/
    Perc1 = limit(0., WatBal->ct.MaxPercolRTZ, 
            (WatBal->st.RootZoneMoisture - WaterEq) / Step - 
            WatBal->rt.Transpiration - WatBal->rt.EvapSoil);
    
    /* Loss of water at the lower end of the maximum root zone */
    /* Equilibrium amount of soil moisture below rooted zone   */
    WELOW = WatBal->ct.MoistureFC * (Crop->prm.MaxRootingDepth - Crop->RootDepth);
    WatBal->rt.Loss  = limit (0., WatBal->ct.MaxPercolSubS, 
            (WatBal->st.MoistureLOW - WELOW)/Step + Perc1);
    
    /* For rice water losses are limited to K0/20 */
    if (Crop->prm.Airducts) 
        WatBal->rt.Loss = min(WatBal->rt.Loss, 0.05*WatBal->ct.K0);
    
    /* Percolation not to exceed uptake capacity of subsoil */
    Perc2 = ((Crop->prm.MaxRootingDepth - Crop->RootDepth) * WatBal->ct.MoistureSAT - 
            WatBal->st.MoistureLOW) / Step + WatBal->rt.Loss;
    WatBal->rt.Percolation = min(Perc1, Perc2);
    
    /* Adjustment of the infiltration rate */
    WatBal->rt.Infiltration = min(WatBal->rt.Infiltration,
          (WatBal->ct.MoistureSAT - WatBal->st.Moisture) * Crop->RootDepth/Step + 
          WatBal->rt.Transpiration + WatBal->rt.EvapSoil + WatBal->rt.Percolation);
            
    /* Rates of change in amounts of moisture W and WLOW */
    WatBal->rt.RootZoneMoisture = -WatBal->rt.Transpiration - WatBal->rt.EvapSoil -  
            WatBal->rt.Percolation + WatBal->rt.Infiltration;
    WatBal->rt.MoistureLOW = WatBal->rt.Percolation - WatBal->rt.Loss;
                  
}


/*-----------------------------------------------------*/
/* function IntegrationWatBal                          */
/* Purpose: integrate the waterbalance rates over time */
/* and calculate the water content in the rooted zone  */
/*-----------------------------------------------------*/

void IntegrationWatBal() 
{
    float PreSurfaceStorage;
    float WaterRootExt;
    
    WatBal->st.Transpiration += WatBal->rt.Transpiration;
    WatBal->st.EvapWater     += WatBal->rt.EvapWater;
    WatBal->st.EvapSoil      += WatBal->rt.EvapSoil;
    
    WatBal->st.Rain += Rain[Day];
    WatBal->st.Infiltration += WatBal->rt.Infiltration;
    WatBal->st.Irrigation   += WatBal->rt.Irrigation;
    
    /* Surface storage and runoff */
    PreSurfaceStorage = WatBal->st.SurfaceStorage + (Rain[Day] + 
            WatBal->rt.Irrigation - WatBal->rt.EvapWater - 
            WatBal->rt.Infiltration) * Step;
    WatBal->st.SurfaceStorage = min(PreSurfaceStorage, 
            Site->MaxSurfaceStorage);
    WatBal->st.Runoff += PreSurfaceStorage - WatBal->st.SurfaceStorage;
    
    /* Water amount in the rooted zone */
    WatBal->st.RootZoneMoisture += WatBal->rt.RootZoneMoisture * Step;
    if (WatBal->st.RootZoneMoisture < 0.) 
    {
       WatBal->st.EvapSoil += WatBal->st.RootZoneMoisture;
       WatBal->st.RootZoneMoisture = 0.;
    }
    
    /* Total percolation and loss of water by deep leaching */
    WatBal->st.Percolation += WatBal->rt.Percolation * Step;
    WatBal->st.Loss        += WatBal->rt.Loss * Step;     
    
    WatBal->st.MoistureLOW += WatBal->rt.MoistureLOW;
   
    /* Change of root zone subsystem boundary                  */
    /* Calculation of amount of soil moisture in new root zone */
    if (Crop->RootDepth - Crop->RootDepth_prev > 0.001) 
    {
        
        /* water added to root zone by root growth, in cm   */
        WaterRootExt = WatBal->st.MoistureLOW *
                (Crop->RootDepth - Crop->RootDepth_prev) / 
                (Crop->prm.MaxRootingDepth - Crop->RootDepth_prev);
        WatBal->st.MoistureLOW -= WaterRootExt;

        /* Total water addition to root zone by root growth  */
        WatBal->st.WaterRootExt += WaterRootExt;

        /* Amount of soil moisture in extended root zone */
        WatBal->st.RootZoneMoisture += WaterRootExt;
    }

    /* Mean soil moisture content in rooted zone */
    WatBal->st.Moisture = WatBal->st.RootZoneMoisture/Crop->RootDepth;
  
}