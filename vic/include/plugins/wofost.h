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
	} wofost_afgen;

typedef struct MANAGEMENT {
        /** Tables for fertilizer application and recovery fraction **/
        wofost_afgen *N_Fert_table;
        wofost_afgen *P_Fert_table;
        wofost_afgen *K_Fert_table;
        wofost_afgen *N_Uptake_frac;
        wofost_afgen *P_Uptake_frac;
        wofost_afgen *K_Uptake_frac;
        wofost_afgen *Irrigation;
        
        float N_Mins;
        float NRecoveryFrac;
        float P_Mins;
        float PRecoveryFrac;
        float K_Mins; 
        float KRecoveryFrac;
        } wofost_management;
wofost_management *Mng;

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
        wofost_afgen *Roots;
        wofost_afgen *Stems;
        wofost_afgen *Leaves;
        wofost_afgen *Storage;

        wofost_afgen *VernalizationRate;
        wofost_afgen *DeltaTempSum;
        wofost_afgen *SpecificLeaveArea;
        wofost_afgen *SpecificStemArea;
        wofost_afgen *KDiffuseTb;
        wofost_afgen *EFFTb;
        wofost_afgen *MaxAssimRate; 
        wofost_afgen *FactorAssimRateTemp;
        wofost_afgen *FactorGrossAssimTemp;
        wofost_afgen *FactorSenescence;
        wofost_afgen *DeathRateStems;
        wofost_afgen *DeathRateRoots; 
        
        /** Tables to account for the atmospheric CO2 concentration **/
        wofost_afgen *CO2AMAXTB;
        wofost_afgen *CO2EFFTB;
        wofost_afgen *CO2TRATB;

        /** Tables for the maximum nutrient content in leaves as a function of DVS **/
        wofost_afgen *N_MaxLeaves;
        wofost_afgen *P_MaxLeaves;
        wofost_afgen *K_MaxLeaves;

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
        } wofost_parameters;


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
        } wofost_states;
        

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
        } wofost_rates;
        
 
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
        } wofost_nutrient_rates;
        

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
        } wofost_nutrient_states;
        

typedef struct GROWTH_RATES {
        float roots;
        float stems;
        float leaves;
        float LAIExp;
        float storage;
        float vernalization;
} wofost_growth_rates;

typedef struct GROWTH_STATES {
        float roots;
        float stems;
        float leaves;
        float LAI;
        float LAIExp;
        float storage;
        float vernalization;
        } wofost_growth_states;

        
typedef struct DYING_RATES {
        float roots;
        float stems;
        float leaves;
        } wofost_dying_rates; 


typedef struct GREEN {
	float weight;
	float age;
	float area;
	struct GREEN *next;
	} wofost_green;
        

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
        
        wofost_parameters prm;
        
        wofost_growth_rates  rt;
        wofost_growth_states st;
        wofost_dying_rates   drt;
        
        wofost_nutrient_states N_st;
	wofost_nutrient_states P_st;
        wofost_nutrient_states K_st;
        
        wofost_nutrient_rates N_rt;
	wofost_nutrient_rates P_rt;
        wofost_nutrient_rates K_rt;            
        
        wofost_green *LeaveProperties;
	} wofost_plant;    
wofost_plant *Crop; /* Place holder for the current crop simulations */


typedef struct SOIL {
        float DaysSinceLastRain;
        float SoilMaxRootingDepth;
        float WaterStress;
        
        /* Tables for Soil */
        wofost_afgen *VolumetricSoilMoisture;
        wofost_afgen *HydraulicConductivity; /* currently not used */
        
        Constants ct;
        wofost_states st;
        wofost_rates rt;
        } wofost_soil;
wofost_soil *WatBal; /* Place holder for the current water balance simulations */


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
        wofost_afgen *NotInfTB;
        } wofost_field;
wofost_field *Site; /* Place holder for the current site simulations */


/* Simulation time */
struct tm simTime;


/* Place holder for a simulation unit */
typedef struct SIMUNIT {
        wofost_plant *crp;
        wofost_field *ste;
        wofost_management *mng;
        wofost_soil  *soil;
        int start;
        int emergence;
        int file;
        char name[100]; 
        struct SIMUNIT *next;
        } wofost_simUnit;

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
extern void GetSoilData();
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
void initialize_wofost_local_structures(void);
void wofost_alloc(void);
void wofost_init(void);
void wofost_generate_default_state(void);
bool wofost_history(int, unsigned int *);
void wofost_put_data(void);
void wofost_finalize(void);
void wofost_add_types(void);
void wofost_run(void);
wofost_simUnit *wofost_cell(wofost_simUnit *Grid);
wofost_simUnit *wofost_read_data(size_t);

wofost_simUnit **wofost_var;

#endif /* WOFOST_H */
