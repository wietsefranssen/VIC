#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include "vic.h"

#include <time.h>

void
wofost_run() {
    extern domain_struct       local_domain;
    extern wofost_simUnit            **wofost_var;
    size_t                     i;
   
    for (i = 0; i < local_domain.ncells_active; i++) {
        wofost_var[i] = wofost_cell(wofost_var[i]);
    }    

}

wofost_simUnit*
wofost_cell(wofost_simUnit *Grid) {

    int Emergence;
    int Start;
    int CycleLength   = 300;

    wofost_simUnit *initial = NULL;

    initial = Grid; 

//    /* Get the meteodata */
//    GetMeteoData(path, dateString, place);
//
//    Temp = 0.5 * (Tmax[Day] + Tmin[Day]);
//    DayTemp = 0.5 * (Tmax[Day] + Temp);
//
//    Astro();
//    CalcPenman();

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

//                        fprintf(output[Grid->file],"%4d-%02d-%02d,%4d,%7.0f,%7.0f,%7.0f,%7.2f,%7.2f,%7.2f,%7.3f,%7.2f,%7.1f\n",
//                            simTime.tm_year + 1900, simTime.tm_mon +1, simTime.tm_mday,
//                            Day,Crop->st.stems,Crop->st.leaves,Crop->st.storage,
//                            Crop->st.LAI,Crop->DevelopmentStage,WatBal->WaterStress,
//                            WatBal->st.Moisture,WatBal->rt.Infiltration,Rain[Day]);

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

    /* Set Grid back to the initial address */
    Grid = initial; 
    /* Update time */
//    simTime.tm_mday++;
//    mktime(&simTime);
    
    
//    /* Allocate memory for the file pointers */
//    output = malloc(sizeof(**output) * --count);
//    
//    /* Open the output files */
//    while (Grid)
//    {   /* Make valgrind happy  */
//        memset(name,0,100);
//        
//        memcpy(name, Grid->name, strlen(Grid->name)-4);
//        output[Grid->file] = fopen(name, "w");
//        Grid = Grid->next;
//    }
//    
//    
//    for (Day = 1; Day < 762; Day++)
//    {        
//        /* Go back to the beginning of the list */
//        Grid = initial;
//        
//        Temp = 0.5 * (Tmax[Day] + Tmin[Day]);
//        DayTemp = 0.5 * (Tmax[Day] + Temp);
//        
//        Astro();
//        CalcPenman();
//        
//        while (Grid)
//        {
//            /* Get data, states and rates from the Grid structure and */
//            /* put them in the place holders */
//            Crop      = Grid->crp;
//            WatBal    = Grid->soil;
//            Mng       = Grid->mng;
//            Site      = Grid->ste;
//            Start     = Grid->start;
//            Emergence = Grid->emergence;
//            
//            if (Day >= Start && Crop->Emergence == 0)
//            {
//                if (EmergenceCrop(Emergence))
//                {                 
//                    /* Initialize */
//                    InitializeCrop();
//                    InitializeWatBal();
//                    InitializeNutrients(); 
//                }
//            }
//            
//            if (Day >= Start && Crop->Emergence == 1)
//            {   
//                if (Crop->DevelopmentStage <= Crop->prm.DevelopStageHarvest && Crop->GrowthDay < CycleLength) 
//                {
//                    /* Rate calculations */
//                    EvapTra();
//                    RateCalulationWatBal();
//                    RateCalcultionNutrients();
//                    RateCalculationCrop();
//                    
//                    /* Calculate LAI and DVS */
//                    Crop->st.LAI = LeaveAreaIndex();
//                    Crop->DevelopmentStage = GetDevelopmentStage();
//                                       
//                    fprintf(output[Grid->file],"%4d-%02d-%02d,%4d,%7.0f,%7.0f,%7.0f,%7.2f,%7.2f,%7.2f,%7.3f,%7.2f,%7.1f\n",
//                        simTime.tm_year + 1900, simTime.tm_mon +1, simTime.tm_mday,
//                        Day,Crop->st.stems,Crop->st.leaves,Crop->st.storage,
//                        Crop->st.LAI,Crop->DevelopmentStage,WatBal->WaterStress,
//                        WatBal->st.Moisture,WatBal->rt.Infiltration,Rain[Day]);
//                    
//                    /* State calculations */
//                    IntegrationWatBal();
//                    IntegrationNutrients();
//                    IntegrationCrop();
//                    
//                    
//                    
//                    /* Update the number of days that the crop has grown*/
//                    Crop->GrowthDay++;
//                }
//            }
//
//            /* Store the daily calculations in the Grid structure */
//            Grid->crp  = Crop;
//            Grid->soil = WatBal;
//            Grid->mng  = Mng;
//            Grid->ste  = Site;
//            Grid = Grid->next;
//        }
//    
//    /* Update time */
//    simTime.tm_mday++;
//    mktime(&simTime);
//    }
//     
//    /* Return to the beginning of the list */
//    Grid = initial;
//
//    /* Close the output files and free the allocated memory */
//    while(Grid)
//    {
//        fclose(output[Grid->file]);
//        Grid = Grid->next;
//    }
//    free(output);
//
//    /* Go back to the beginning of the list */
//    Grid = initial;
//    Clean(Grid);
    return(Grid);
}


