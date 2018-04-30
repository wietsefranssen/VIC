#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include "wofost.h"
#include "wofost/extern.h"
#include <time.h>

int wofost() {
    FILE *ifp;
    FILE **output;
    
    SimUnit *Grid = NULL;
    SimUnit *initial = NULL;
       
    int Emergence;
    int Start;
    int CycleLength   = 300;
    int count;
    
    char path[100];
    char cropfile[100];
    char soilfile[100];
    char sitefile[100];
    char management[100];
    char dateString [100];
    char place[15];
    char name[100];
    
    char cf[100], sf[100], mf[100], site[100];
  
    Step = 1.;    
    
    ifp = fopen("list.txt", "r");

    if (ifp == NULL) 
    {
        fprintf(stderr, "Can't open input list.txt\n");
        exit(1);
    }
    
    count = 0;
    while (fscanf(ifp,"%7s %10s %7s %12s %10s %10s %2s %d %d" ,
            path, cf, sf, mf, site, dateString, place, &Start, &Emergence)
            != EOF) 
    {    
        strncpy(cropfile, path, 98);
        strncat(cropfile, cf, 98);

        strncpy(soilfile, path, 98);
        strncat(soilfile, sf, 98);

        strncpy(management, path, 98);
        strncat(management, mf, 98);

        strncpy(sitefile, path, 98);
        strncat(sitefile, site, 98);
        
        /* count the number of output files */
        /* number is the index number of the list of file pointers */
        if (initial == NULL) 
        {
            Grid = initial =  malloc(sizeof(SimUnit));
            GetCropData(Grid->crp   = malloc(sizeof(Plant)), cropfile); 
            GetSiteData(Grid->ste   = malloc(sizeof(Field)), sitefile);
            GetManagement(Grid->mng = malloc(sizeof(Management)), management);
            GetSoilData(Grid->soil  = malloc(sizeof(Soil)), soilfile);
            
            Grid->start = Start;
            Grid->file = count++;
            strcpy(Grid->name,cf);
            Grid->emergence = Emergence;
            Grid->next = NULL; 
        }
        else 
        {
            Grid->next = malloc(sizeof(SimUnit));
            Grid = Grid->next;
            GetCropData(Grid->crp   = malloc(sizeof(Plant)), cropfile); 
            GetSiteData(Grid->ste   = malloc(sizeof(Field)), sitefile);
            GetManagement(Grid->mng = malloc(sizeof(Management)), management);
            GetSoilData(Grid->soil  = malloc(sizeof(Soil)), soilfile);
            
            Grid->start = Start;            // Start day (=day number)
            Grid->file  = count++;          // number of elements in Grid carousel
            strcpy(Grid->name,cf);          // Crop file name
            Grid->emergence = Emergence;    // Start the simulations at emergence (1) or at sowing (0)
            Grid->next = NULL;
        }
    }
    
    /* Close the input file */
    fclose(ifp);
    
    /* Set Grid back to the initial address */
    Grid = initial;   
    
    /* Get the meteodata */
    GetMeteoData(path, dateString, place);
    
    /* Allocate memory for the file pointers */
    output = malloc(sizeof(**output) * --count);
    
    /* Open the output files */
    while (Grid)
    {   /* Make valgrind happy  */
        memset(name,0,100);
        
        memcpy(name, Grid->name, strlen(Grid->name)-4);
        output[Grid->file] = fopen(name, "w");
        Grid = Grid->next;
    }
    
    
    for (Day = 1; Day < 762; Day++)
    {        
        /* Go back to the beginning of the list */
        Grid = initial;
        
        Temp = 0.5 * (Tmax[Day] + Tmin[Day]);
        DayTemp = 0.5 * (Tmax[Day] + Temp);
        
        Astro();
        CalcPenman();
        
        while (Grid)
        {
            /* Get data, states and rates from the Grid structure and */
            /* put them in the place holders */
            Crop      = Grid->crp;
            WatBal    = Grid->soil;
            Mng       = Grid->mng;
            Site      = Grid->ste;
            Start     = Grid->start;
            Emergence = Grid->emergence;
            
            if (Day >= Start && Crop->Emergence == 0)
            {
                if (EmergenceCrop(Emergence))
                {                 
                    /* Initialize */
                    InitializeCrop();
                    InitializeWatBal();
                    InitializeNutrients(); 
                }
            }
            
            if (Day >= Start && Crop->Emergence == 1)
            {   
                if (Crop->DevelopmentStage <= Crop->prm.DevelopStageHarvest && Crop->GrowthDay < CycleLength) 
                {
                    /* Rate calculations */
                    EvapTra();
                    RateCalulationWatBal();
                    RateCalcultionNutrients();
                    RateCalculationCrop();
                    
                    /* Calculate LAI and DVS */
                    Crop->st.LAI = LeaveAreaIndex();
                    Crop->DevelopmentStage = GetDevelopmentStage();
                                       
                    fprintf(output[Grid->file],"%4d-%02d-%02d,%4d,%7.0f,%7.0f,%7.0f,%7.2f,%7.2f,%7.2f,%7.3f,%7.2f,%7.1f\n",
                        simTime.tm_year + 1900, simTime.tm_mon +1, simTime.tm_mday,
                        Day,Crop->st.stems,Crop->st.leaves,Crop->st.storage,
                        Crop->st.LAI,Crop->DevelopmentStage,WatBal->WaterStress,
                        WatBal->st.Moisture,WatBal->rt.Infiltration,Rain[Day]);
                    
                    /* State calculations */
                    IntegrationWatBal();
                    IntegrationNutrients();
                    IntegrationCrop();
                    
                    
                    
                    /* Update the number of days that the crop has grown*/
                    Crop->GrowthDay++;
                }
            }

            /* Store the daily calculations in the Grid structure */
            Grid->crp  = Crop;
            Grid->soil = WatBal;
            Grid->mng  = Mng;
            Grid->ste  = Site;
            Grid = Grid->next;
        }
    
    /* Update time */
    simTime.tm_mday++;
    mktime(&simTime);
    }
     
    /* Return to the beginning of the list */
    Grid = initial;

    /* Close the output files and free the allocated memory */
    while(Grid)
    {
        fclose(output[Grid->file]);
        Grid = Grid->next;
    }
    free(output);

    /* Go back to the beginning of the list */
    Grid = initial;
    Clean(Grid);

    return 0;
}


