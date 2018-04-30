#include <stdio.h>
#include "wofost.h"

/* ---------------------------------------------------------------------------*/
/*  function FillCropVariables(float *Variable)                               */
/*  Purpose: Fill the crop parameters that are read by GetCropData()          */
/* ---------------------------------------------------------------------------*/

void FillCropVariables(Plant *CROP, float *Variable)
{
    int i;
    CROP->prm.TempBaseEmergence        = Variable[0];
    CROP->prm.TempEffMax               = Variable[1];
    CROP->prm.TSumEmergence            = Variable[2];                      	     

    /**  Phenology  **/
    CROP->prm.IdentifyAnthesis         = (int) Variable[3]; 
    CROP->prm.OptimumDaylength         = Variable[4];	            
    CROP->prm.CriticalDaylength        = Variable[5];
    
    /** Switch if vernalization is accounted for **/
    if (CROP->prm.IdentifyAnthesis < 2)
    {
        CROP->prm.SatVernRequirement   = -99.;
        CROP->prm.BaseVernRequirement  = -99.;
    }
    else
    {
        CROP->prm.SatVernRequirement   = Variable[6];
        CROP->prm.BaseVernRequirement  = Variable[7];
    }        

    CROP->prm.TempSum1                 = Variable[8];       
    CROP->prm.TempSum2                 = Variable[9]; 
    CROP->prm.InitialDVS               = Variable[10];
    CROP->prm.DevelopStageHarvest      = Variable[11];
    
    /** Initial Values  **/
    CROP->prm.InitialDryWeight         = Variable[12];
    CROP->prm.LAIEmergence             = Variable[13];
    CROP->prm.RelIncreaseLAI           = Variable[14];

    /**  Green Area  **/
    CROP->prm.SpecificPodArea          = Variable[15];
    CROP->prm.LifeSpan                 = Variable[16];
    CROP->prm.TempBaseLeaves           = Variable[17];

    /** Conversion assimilates into biomass **/
    CROP->prm.ConversionLeaves         = Variable[18];
    CROP->prm.ConversionStorage        = Variable[19];
    CROP->prm.ConversionRoots          = Variable[20];
    CROP->prm.ConversionStems          = Variable[21];

    /** Maintenance Respiration **/
    CROP->prm.Q10                      = Variable[22];
    CROP->prm.RelRespiLeaves           = Variable[23];
    CROP->prm.RelRespiStorage          = Variable[24];
    CROP->prm.RelRespiRoots            = Variable[25];
    CROP->prm.RelRespiStems            = Variable[26];

    /** Death Rates  **/
    CROP->prm.MaxRelDeathRate          = Variable[27];

    /** Water Use  **/
    CROP->prm.CorrectionTransp         = Variable[28];
    CROP->prm.CropGroupNumber          = Variable[29];
    CROP->prm.Airducts                 = Variable[30];
    
    /** Rooting **/
    /** Convert cm to mm */
    CROP->prm.InitRootingDepth         = Variable[31];
    CROP->prm.MaxIncreaseRoot          = Variable[32];
    CROP->prm.MaxRootingDepth          = Variable[33];

    /** Nutrients  **/
    CROP->prm.DyingLeaves_NPK_Stress   = Variable[34];
    CROP->prm.DevelopmentStageNLimit   = Variable[35];
    CROP->prm.DevelopmentStageNT       = Variable[36];
    CROP->prm.FracTranslocRoots        = Variable[37];
    CROP->prm.Opt_N_Frac               = Variable[38];
    CROP->prm.Opt_P_Frac               = Variable[39];
    CROP->prm.Opt_K_Frac               = Variable[40];
    CROP->prm.N_MaxRoots               = Variable[41];
    CROP->prm.N_MaxStems               = Variable[42];
    CROP->prm.P_MaxRoots               = Variable[43];
    CROP->prm.P_MaxStems               = Variable[44];
    CROP->prm.K_MaxRoots               = Variable[45];
    CROP->prm.K_MaxStems               = Variable[46];
    CROP->prm.NitrogenStressLAI        = Variable[47];                	     
    CROP->prm.NLUE                     = Variable[48];
    CROP->prm.Max_N_storage            = Variable[49];
    CROP->prm.Max_P_storage            = Variable[50];
    CROP->prm.Max_K_storage            = Variable[51];
    CROP->prm.N_lv_partitioning        = Variable[52];
    CROP->prm.NutrientStessSLA         = Variable[53];
    CROP->prm.N_ResidualFrac_lv        = Variable[54];
    CROP->prm.N_ResidualFrac_st        = Variable[55];
    CROP->prm.N_ResidualFrac_ro        = Variable[56];
    CROP->prm.P_ResidualFrac_lv        = Variable[57];
    CROP->prm.P_ResidualFrac_st        = Variable[58];
    CROP->prm.P_ResidualFrac_ro        = Variable[59]; 
    CROP->prm.K_ResidualFrac_lv        = Variable[60];
    CROP->prm.K_ResidualFrac_st        = Variable[61];
    CROP->prm.K_ResidualFrac_ro        = Variable[62];
    CROP->prm.TCNT                     = Variable[63];
    CROP->prm.TCPT                     = Variable[64];
    CROP->prm.TCKT                     = Variable[65];
    CROP->prm.N_fixation               = Variable[66];


    for (i=0;i<=NR_VARIABLES_CRP;i++) 
    {
        Variable[i] = 0.;
    }
}

/* ---------------------------------------------------------------------------*/
/*  function FillSoilVariables(float *Variable)                               */
/*  Purpose: Fill the soil parameters that are read by GetSoilData()          */
/* ---------------------------------------------------------------------------*/

void FillSoilVariables(Soil *SOIL, float *Variable) {
    int i;
    
    SOIL->ct.MoistureWP       = Variable[0];
    SOIL->ct.MoistureFC       = Variable[1];
    SOIL->ct.MoistureSAT      = Variable[2];
    SOIL->ct.CriticalSoilAirC = Variable[3];
    SOIL->ct.K0               = Variable[4];
    SOIL->ct.MaxPercolRTZ     = Variable[5];
    SOIL->ct.MaxPercolSubS    = Variable[6];
    
    
    /* No workability parameters will be used in this version */
    
    for (i=0;i<=NR_VARIABLES_SOIL_USED;i++) 
    {
        Variable[i]= 0.;
    }
}


/* ---------------------------------------------------------------------------*/
/*  function FillSiteVariables(float *Variable)                               */
/*  Purpose: Fill the site parameters that are read by GetSiteData()          */
/* ---------------------------------------------------------------------------*/

void FillSiteVariables(Field *SITE, float *Variable) {
    int i;
   
    SITE->FlagGroundWater                 = Variable[0];
    SITE->InfRainDependent                = Variable[1];
    SITE->FlagDrains                      = Variable[2];
    SITE->MaxSurfaceStorage               = Variable[3];
    SITE->InitSoilMoisture                = Variable[4];
    SITE->GroundwaterDepth                = Variable[5];
    SITE->DD                              = Variable[6];
    SITE->SoilLimRootDepth                = Variable[7];
    SITE->NotInfiltrating                 = Variable[8];
    SITE->SurfaceStorage                  = Variable[9];
    SITE->MaxInitSoilM                    = Variable[10];
    
    CO2 = Variable[11];
       
    for (i=0;i<=NR_VARIABLES_SITE;i++) 
    {
        Variable[i]= 0.;
    }
}


/* ---------------------------------------------------------------------------*/
/*  function FillManageVariables(float *Variable)                             */
/*  Purpose: Fill the management parameters that are read by GetManagement()  */
/* ---------------------------------------------------------------------------*/

void FillManageVariables(Management *MNG, float *Variable) {
    int i;
    
    MNG->N_Mins                          = Variable[0];
    MNG->NRecoveryFrac                   = Variable[1];
    MNG->P_Mins                          = Variable[2];
    MNG->PRecoveryFrac                   = Variable[3];
    MNG->K_Mins                          = Variable[4];
    MNG->KRecoveryFrac                   = Variable[5];
    
    for (i=0;i<=NR_VARIABLES_MANAGEMENT;i++) 
    {
        Variable[i]= 0.;
    }
}
