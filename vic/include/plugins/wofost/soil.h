#ifndef SOIL_H
#define SOIL_H

char *SoilParam[]={"SMW",
                   "SMFCF",
		   "SM0",
		   "CRAIRC",
		   "K0",
		   "SOPE",
		   "KSUB",
		   "SPADS",
                   "SPODS",
		   "SPASS",
		   "SPOSS",
		   "DEFLIM",
		   "NULL"
		   };
		   
char *SoilParam2[]={
                    "SMTAB",
                    "CONTAB",
                    "NULL"
		    };

extern int FillSoilVariables();
extern int FillAfgenTables();

#endif //SOIL_H