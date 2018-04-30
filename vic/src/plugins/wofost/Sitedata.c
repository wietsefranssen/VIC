#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vic.h"
#include "wofost/site.h"


void GetSiteData(Field *SITE, char *sitefile)
{
  AFGEN *Table[NR_TABLES_SITE], *start;
  
  int i, c;
  float Variable[NR_VARIABLES_SITE], XValue, YValue;
  char x[2], xx[2],  word[NR_VARIABLES_SITE];
  FILE *fq;

 if ((fq = fopen(sitefile, "rt")) == NULL) {
     fprintf(stderr, "Cannot open input file.\n"); 
     exit(0);
 }

 i=0;
 while ((c=fscanf(fq,"%s",word)) != EOF) {
    if (!strcmp(word, SiteParam[i])) {
        while ((c=fgetc(fq)) !='=');
        
	fscanf(fq,"%f",  &Variable[i]);

	i++; 
      }  
 }

  if (i != NR_VARIABLES_SITE) {
      fprintf(stderr, "Something wrong with the Site variables.\n");
      exit(0);
  }
  rewind(fq);  

  FillSiteVariables(SITE, Variable);
 

  i=0;
  while ((c=fscanf(fq,"%s",word)) != EOF) {
    if (!strcmp(word, SiteParam2[i])) {
        Table[i]= start= malloc(sizeof(AFGEN));
	fscanf(fq,"%s %f %s  %f", x, &Table[i]->x, xx, &Table[i]->y);
        Table[i]->next = NULL;				     
			       
	while ((c=fgetc(fq)) !='\n');
	while (fscanf(fq," %f %s  %f",  &XValue, xx, &YValue) > 0) {
 	    Table[i]->next = malloc(sizeof(AFGEN));
            Table[i] = Table[i]->next; 
            Table[i]->next = NULL;
	    Table[i]->x = XValue;
	    Table[i]->y = YValue;
	    
	    while ((c=fgetc(fq)) !='\n' || (c=fgetc(fq)) != EOF );
	    }
        /* Go back to beginning of the table */
        Table[i] = start;        
	i++; 
       }      
  }
  
  fclose(fq);

  if (i != NR_TABLES_SITE)
  {
    fprintf(stderr, "Something wrong with the Site tables.\n"); 
    exit(0);
  } 
   
  SITE->NotInfTB = Table[0];
 
}

