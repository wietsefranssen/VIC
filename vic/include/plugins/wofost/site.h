#ifndef SITE_H
#define SITE_H

char *SiteParam[]={ "IZT",
                   "IFUNRN",
		   "IDRAIN",
		   "SSMAX",
		   "WAV",
		   "ZTI",
		   "DD",
		   "RDMSOL",
                   "NOTINF",
                   "SSI",
                   "SMLIM",
                   "CO2",
                   "NULL"
		   };

char *SiteParam2[]={
                    "NINFTB",
                    "NULL"
		    };

extern void FillSiteVariables();
extern int FillAfgenTables();

#endif	// SITE_H

