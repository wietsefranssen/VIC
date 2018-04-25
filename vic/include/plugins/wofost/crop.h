/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   crop.h
 * Author: wietse
 *
 * Created on April 25, 2018, 1:39 PM
 */

#ifndef CROP_H
#define CROP_H

/* Crop */
char *CropParam[]={"TBASEM",
                   "TEFFMX",
		   "TSUMEM",
		   "IDSL",
		   "DLO",
		   "DLC",
                   "VERSAT",
                   "VBASE",
		   "TSUM1",
		   "TSUM2",
                   "DVSI",
		   "DVSEND",
		   "TDWI",
		   "LAIEM",
		   "RGRLAI",
		   "SPA",
		   "SPAN",
                   "TBASE",
		   "CVL",
		   "CVO",
		   "CVR",
		   "CVS",
		   "Q10",
		   "RML",
		   "RMO",
		   "RMR",
		   "RMS",
		   "PERDL",
		   "CFET",
		   "DEPNR",
		   "IAIRDU",
		   "RDI",
		   "RRI",
		   "RDMCR",
                   "RDRLV_NPK",  
                   "DVS_NPK_STOP", 
                   "DVS_NPK_TRANSL",  
                   "NPK_TRANSLRT_FR",  
                   "NCRIT_FR",   
                   "PCRIT_FR",   
                   "KCRIT_FR",   
                   "NMAXRT_FR",   
                   "NMAXST_FR",   
                   "PMAXRT_FR",   
                   "PMAXST_FR",   
                   "KMAXRT_FR",   
                   "KMAXST_FR",   
                   "NLAI_NPK",   
                   "NLUE_NPK",   
                   "NMAXSO", 
                   "PMAXSO", 
                   "KMAXSO", 
                   "NPART",  
                   "NSLA_NPK",   
                   "NRESIDLV",  
                   "NRESIDST",  
                   "NRESIDRT",  
                   "PRESIDLV",  
                   "PRESIDST",  
                   "PRESIDRT",  
                   "KRESIDLV",  
                   "KRESIDST",  
                   "KRESIDRT",  
                   "TCNT",   
                   "TCPT",   
                   "TCKT",   
                   "NFIX_FR",  
                   "NULL"
		   };
		   
char *CropParam2[]={
                    "VERNRT",
                    "DTSMTB",
                    "SLATB",
                    "SSATB",
                    "KDIFTB",
		    "EFFTB",
                    "AMAXTB",
		    "TMPFTB",
		    "TMNFTB",
                    "CO2AMAXTB",
                    "CO2EFFTB",
                    "CO2TRATB",
                    "RFSETB",
		    "FRTB",
                    "FLTB",
                    "FSTB",
		    "FOTB",
		    "RDRRTB",
		    "RDRSTB",
                    "NMAXLV_TB",
                    "PMAXLV_TB",
                    "KMAXLV_TB",
                    "NULL"
		    };

extern void FillCropVariables();
extern int FillAfgenTables();

#endif /* CROP_H */

