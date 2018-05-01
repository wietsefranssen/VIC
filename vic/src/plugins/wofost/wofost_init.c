#include <vic.h>

void
wofost_init(void)
{    
    extern domain_struct       local_domain;
    extern wofost_simUnit            **wofost_var;
    size_t                     i;
   
    for (i = 0; i < local_domain.ncells_active; i++) {
        wofost_var[i] = wofost_read_data(i);
    }    
}

wofost_simUnit*
wofost_read_data(size_t cur_cell)
{    
    FILE *ifp;
    
    wofost_simUnit *Grid;
    wofost_simUnit *initial;
       
    int Emergence;
    int Start;
    int count;
    
    char path[100];
    char cropfile[100];
    char soilfile[100];
    char sitefile[100];
    char management[100];
    char dateString [100];
    char place[15];
    
    char cf[100], sf[100], mf[100], site[100];

    Grid = initial = NULL;

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
            Grid = initial =  malloc(sizeof(wofost_simUnit));
            GetCropData(Grid->crp   = malloc(sizeof(wofost_plant)), cropfile); 
            GetSiteData(Grid->ste   = malloc(sizeof(wofost_field)), sitefile);
            GetManagement(Grid->mng = malloc(sizeof(wofost_management)), management);
            GetSoilData(Grid->soil  = malloc(sizeof(wofost_soil)), soilfile);
            
            Grid->start = Start;
            Grid->file = count++;
            strcpy(Grid->name,cf);
            Grid->emergence = Emergence;
            Grid->next = NULL; 
        }
        else 
        {
            Grid->next = malloc(sizeof(wofost_simUnit));
            Grid = Grid->next;
            GetCropData(Grid->crp   = malloc(sizeof(wofost_plant)), cropfile); 
            GetSiteData(Grid->ste   = malloc(sizeof(wofost_field)), sitefile);
            GetManagement(Grid->mng = malloc(sizeof(wofost_management)), management);
            GetSoilData(Grid->soil  = malloc(sizeof(wofost_soil)), soilfile);
            
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
    
    return(Grid);
}