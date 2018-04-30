#ifndef WOFOST_H
#define WOFOST_H

#include <stdbool.h>
#include <time.h>


#define NR_VARIABLES_CRP	67
#define NR_TABLES_CRP   	22
#define NR_VARIABLES_SITE       12
#define NR_TABLES_SITE          1
#define NR_VARIABLES_SOIL       12
#define NR_VARIABLES_SOIL_USED  6
#define NR_TABLES_SOIL          2
#define NR_VARIABLES_MANAGEMENT 6
#define NR_TABLES_MANAGEMENT    7
#define NUMBER_OF_TABLES        31
#define NR_SIMUNITS             4

typedef struct TABLE {
	float x;
	float y;
	struct TABLE *next;
	} AFGEN;

typedef struct MANAGEMENT {
        /** Tables for fertilizer application and recovery fraction **/
        AFGEN *N_Fert_table;
        AFGEN *P_Fert_table;
        AFGEN *K_Fert_table;
        AFGEN *N_Uptake_frac;
        AFGEN *P_Uptake_frac;
        AFGEN *K_Uptake_frac;
        AFGEN *Irrigation;
        
        float N_Mins;
        float NRecoveryFrac;
        float P_Mins;
        float PRecoveryFrac;
        float K_Mins; 
        float KRecoveryFrac;
        } Management;
Management *Mng;

typedef struct CONSTANTS {
        float MaxEvapWater;
        float MoistureFC;
        float MoistureWP;
        float MoistureSAT;
        float CriticalSoilAirC;
        float MaxPercolRTZ;
        float MaxPercolSubS;
        float MaxSurfaceStorge;
        float K0;
        } Constants;

typedef struct PARAMETERS {
          /** Tables for the Crop simulations **/
        AFGEN *Roots;
        AFGEN *Stems;
        AFGEN *Leaves;
        AFGEN *Storage;

        AFGEN *VernalizationRate;
        AFGEN *DeltaTempSum;
        AFGEN *SpecificLeaveArea;
        AFGEN *SpecificStemArea;
        AFGEN *KDiffuseTb;
        AFGEN *EFFTb;
        AFGEN *MaxAssimRate; 
        AFGEN *FactorAssimRateTemp;
        AFGEN *FactorGrossAssimTemp;
        AFGEN *FactorSenescence;
        AFGEN *DeathRateStems;
        AFGEN *DeathRateRoots; 
        
        /** Tables to account for the atmospheric CO2 concentration **/
        AFGEN *CO2AMAXTB;
        AFGEN *CO2EFFTB;
        AFGEN *CO2TRATB;

        /** Tables for the maximum nutrient content in leaves as a function of DVS **/
        AFGEN *N_MaxLeaves;
        AFGEN *P_MaxLeaves;
        AFGEN *K_MaxLeaves;

        /** Static Variables  **/
        /**  Emergence  **/
        float TempBaseEmergence;
        float TempEffMax;
        float TSumEmergence;                      	     

        /**  Phenology  **/
        int   IdentifyAnthesis; 
        float OptimumDaylength;	            
        float CriticalDaylength;
        float SatVernRequirement;
        float BaseVernRequirement;
        float TempSum1;       
        float TempSum2; 
        float InitialDVS;
        float DevelopStageHarvest;

        /** Initial Values  **/
        float InitialDryWeight;
        float LAIEmergence;
        float RelIncreaseLAI;

        /**  Green Area  **/
        float SpecificPodArea;
        float LifeSpan;
        float TempBaseLeaves;

        /** Conversion assimilates into biomass **/
        float ConversionLeaves;
        float ConversionStorage;
        float ConversionRoots;
        float ConversionStems;

        /** Maintenance Respiration **/
        float Q10;
        float RelRespiLeaves;
        float RelRespiStorage;
        float RelRespiRoots;
        float RelRespiStems;

        /** Death Rates  **/
        float MaxRelDeathRate;

        /** Water Use  **/
        float CorrectionTransp;
        float CropGroupNumber;
        float Airducts;

        /** Rooting **/
        float InitRootingDepth;
        float MaxIncreaseRoot;
        float MaxRootingDepth;

        /** Nutrients **/
        float DyingLeaves_NPK_Stress; 
        float DevelopmentStageNLimit; 
        float DevelopmentStageNT;
        float FracTranslocRoots;  
        float Opt_N_Frac;   
        float Opt_P_Frac;   
        float Opt_K_Frac;   
        float N_MaxRoots;   
        float N_MaxStems;   
        float P_MaxRoots;   
        float P_MaxStems;   
        float K_MaxRoots;   
        float K_MaxStems;   
        float NitrogenStressLAI;   
        float NLUE;   
        float Max_N_storage; 
        float Max_P_storage; 
        float Max_K_storage; 
        float N_lv_partitioning;  
        float NutrientStessSLA;   
        float N_ResidualFrac_lv;  
        float N_ResidualFrac_st;  
        float N_ResidualFrac_ro;  
        float P_ResidualFrac_lv;  
        float P_ResidualFrac_st;  
        float P_ResidualFrac_ro;  
        float K_ResidualFrac_lv;  
        float K_ResidualFrac_st;   
        float K_ResidualFrac_ro;   
        float TCNT;   
        float TCPT;   
        float TCKT;   
        float N_fixation; 
        } Parameters;


typedef struct STATES {
        float EvapWater;
        float EvapSoil;
        float Infiltration;
        float Irrigation;
        float Loss;
        float Moisture;
        float MoistureLOW;
        float Percolation;
        float Rain;
        float RootZoneMoisture;
        float Runoff;
        float SurfaceStorage;
        float Transpiration;
        float WaterRootExt;
        } States;
        

typedef struct RATES {
        float EvapWater;
        float EvapSoil;
        float Infiltration;
        float Irrigation;
        float Loss;
        float Moisture;
        float MoistureLOW;
        float Percolation;
        float RootZoneMoisture;
        float Runoff;
        float SurfaceStorage;
        float Transpiration;
        float WaterRootExt;
        } Rates;
        
 
typedef struct NUTRIENT_RATES {
        float roots;
        float stems;
        float leaves;
        float storage;
        float Demand_lv;
        float Demand_st;
        float Demand_ro;
        float Demand_so;
        float Transloc;
        float Transloc_lv;
        float Transloc_st;
        float Transloc_ro;
        float Uptake;
        float Uptake_lv;
        float Uptake_st;
        float Uptake_ro;
        float death_lv;
        float death_st;
        float death_ro;
        } nutrient_rates;
        

typedef struct NUTRIENT_STATES {
        float roots;
        float stems;
        float leaves;
        float storage;
        float Max_lv;
        float Max_st;
        float Max_ro;
        float Max_so;
        float Optimum_lv;
        float Optimum_st;
        float Indx;
        float Uptake;
        float Uptake_lv;
        float Uptake_st;
        float Uptake_ro;
        float death_lv;
        float death_st;
        float death_ro;
        } nutrient_states;
        

typedef struct GROWTH_RATES {
        float roots;
        float stems;
        float leaves;
        float LAIExp;
        float storage;
        float vernalization;
} growth_rates;

typedef struct GROWTH_STATES {
        float roots;
        float stems;
        float leaves;
        float LAI;
        float LAIExp;
        float storage;
        float vernalization;
        } growth_states;

        
typedef struct DYING_RATES {
        float roots;
        float stems;
        float leaves;
        } dying_rates; 


typedef struct GREEN {
	float weight;
	float age;
	float area;
	struct GREEN *next;
	} Green;
        

typedef struct PLANT {
        int Emergence;
        int GrowthDay;
        float DevelopmentStage;
        float RootDepth;
        float RootDepth_prev;
        float NPK_Indx;
        float NutrientStress;
        float DaysOxygenStress;
        float TSumEmergence;
        
        Parameters prm;
        
        growth_rates  rt;
        growth_states st;
        dying_rates   drt;
        
        nutrient_states N_st;
	nutrient_states P_st;
        nutrient_states K_st;
        
        nutrient_rates N_rt;
	nutrient_rates P_rt;
        nutrient_rates K_rt;            
        
        Green *LeaveProperties;
	} Plant;    
Plant *Crop; /* Place holder for the current crop simulations */


typedef struct SOIL {
        float DaysSinceLastRain;
        float SoilMaxRootingDepth;
        float WaterStress;
        
        /* Tables for Soil */
        AFGEN *VolumetricSoilMoisture;
        AFGEN *HydraulicConductivity; /* currently not used */
        
        Constants ct;
        States st;
        Rates rt;
        } Soil;
Soil *WatBal; /* Place holder for the current water balance simulations */


typedef struct FIELD {
        /* Water related parameters */
        float FlagGroundWater;
        float InfRainDependent;
        float FlagDrains;
        float MaxSurfaceStorage;     
        float InitSoilMoisture;
        float GroundwaterDepth;
        float DD;
        float SoilLimRootDepth;
        float NotInfiltrating;
        float SurfaceStorage;
        float MaxInitSoilM;
        
        /* Mineral states and rates */
        float st_N_tot;
        float st_P_tot;
        float st_K_tot;

        float st_N_mins;
        float st_P_mins;
        float st_K_mins;

        float rt_N_tot;
        float rt_P_tot;
        float rt_K_tot;

        float rt_N_mins;
        float rt_P_mins;
        float rt_K_mins;
        
        /** Table for the fraction of precipitation that does not infiltrate **/
        AFGEN *NotInfTB;
        } Field;
Field *Site; /* Place holder for the current site simulations */


/* Simulation time */
struct tm simTime;


/* Place holder for a simulation unit */
typedef struct SIMUNIT {
        Plant *crp;
        Field *ste;
        Management *mng;
        Soil  *soil;
        int start;
        int emergence;
        int file;
        char name[100]; 
        struct SIMUNIT *next;
        } SimUnit;

float CO2;

/** Meteorological Variables  **/
int Station, Year;
float AngstA;
float AngstB;
float Longitude, Latitude, Altitude;
float Tmin[732], Tmax[732], Radiation[732], Rain[732];
float Windspeed[732], Vapour[732];

/* Time step */
float Step;

/* Astro */
float AtmosphTransm;
float Daylength;
float PARDaylength;
float SinLD;
float CosLD; 
float DiffRadPP;
float DSinBE;

extern float limit(float a, float b, float c);
extern float Afgen();

int Day;
float Temp;
float DayTemp;

/* General help functions */
extern float Afgen();
extern float limit(float a, float b, float c);
extern float notnul(float x);
extern float insw(float x1, float x2, float x3);
//extern int leap_year(int year);


/* Crop growth */
extern int Astro();
extern void CalcPenman();

extern void Clean();
extern void RateCalculationCrop();
extern void Growth(float NewPlantMaterial);
extern void IntegrationCrop();
extern void InitializeCrop();
extern int EmergenceCrop(int Emergence);

extern float GetDevelopmentStage();
extern float DailyTotalAssimilation();
extern float DyingLeaves();
extern float InstantAssimilation(float KDiffuse, float EFF, float AssimMax, float SinB, float PARDiffuse, float PARDirect);
extern float LeaveGrowth(float LAIExp, float Newleaves);
extern float LeaveAreaIndex();
extern float Correct(float GrossAssimilation);
extern float RespirationRef(float TotalAssimilation);
extern float Conversion(float NetAssimilation);


/* Nutrients */
extern void CropNutrientRates();
extern void InitializeNutrients();
extern void IntegrationNutrients();
extern void NutritionINDX();
extern void NutrientLoss();
extern void NutrientMax();
extern void NutrientPartioning();
extern void NutrientRates();
extern void NutrientOptimum();
extern void NutrientDemand();
extern void SoilNutrientRates();
extern void NutrientTranslocation();
extern void RateCalcultionNutrients();


/* Read data */
extern int GetAfgenTables();
extern int GetMeteoData();
extern int FillAfgenTables();

extern void GetCropData();
extern void FillCropVariables();
extern void GetSiteData();
extern void FillSiteVariables();
extern void  GetSoilData();
extern void FillSoilVariables();
extern void GetManagement();
extern void FillManageVariables();


/* Water balance */
extern void InitializeWatBal();
extern void RateCalulationWatBal();
extern void IntegrationWatBal();
extern void EvapTra();


// VIC Functions
bool wofost_get_global_parameters(char *cmdstr);
void wofost_validate_global_parameters(void);
void wofost_start(void);
void wofost_set_output_meta_data_info(void);
void wofost_set_state_meta_data_info(void);
void wofost_alloc(void);
void initialize_wofost_local_structures(void);
void wofost_init(void);
void wofost_generate_default_state(void);
bool wofost_history(int, unsigned int *);
void wofost_put_data(void);
void wofost_finalize(void);
void wofost_add_types(void);
//void wofost_run(size_t cur_cell);
void wofost_run(void);

SimUnit *Grid;
SimUnit *initial;

#endif /* WOFOST_H */
