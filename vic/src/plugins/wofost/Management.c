#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vic.h"
#include "wofost/manage.h"
       
void GetManagement(wofost_management *MNG, char *management)
{
  wofost_afgen *Table[NR_TABLES_MANAGEMENT], *start;
  
  int i, c;
  float Variable[100], XValue, YValue;
  char x[2], xx[2],  word[100];
  FILE *fq;

 if ((fq = fopen(management, "rt")) == NULL) {
     fprintf(stderr, "Cannot open input file.\n"); 
     exit(0);
 }

 i=0;
  while ((c=fscanf(fq,"%s",word)) != EOF && i < 12 ) {
    if (!strcmp(word, ManageParam[i])) {
        while ((c=fgetc(fq)) !='=');
	fscanf(fq,"%f",  &Variable[i]);
	i++; 
       }  
  }

  if (i != NR_VARIABLES_MANAGEMENT) {
    fprintf(stderr, "Something wrong with the Site variables.\n"); 
    exit(0);
  }
 
  rewind(fq);  
  
  FillManageVariables(MNG, Variable);
 

  i=0;
  while ((c=fscanf(fq,"%s",word)) != EOF) 
  {
    if (!strcmp(word, ManageParam2[i])) {
        Table[i] = start = malloc(sizeof(wofost_afgen));
	fscanf(fq,"%s %f %s  %f", x, &Table[i]->x, xx, &Table[i]->y);
        Table[i]->next = NULL;				     
			       
	while ((c=fgetc(fq)) !='\n');
	while (fscanf(fq," %f %s  %f",  &XValue, xx, &YValue) > 0)  {
	    Table[i]->next = malloc(sizeof(wofost_afgen));
            Table[i] = Table[i]->next; 
            Table[i]->next = NULL;
	    Table[i]->x = XValue;
	    Table[i]->y = YValue;
	    
	    while ((c=fgetc(fq)) !='\n');
	    }
        /* Go back to beginning of the table */
        Table[i] = start;
	i++; 
       }      
  }

  fclose(fq);
  
  if (i!= NR_TABLES_MANAGEMENT){
    fprintf(stderr, "Something wrong with the Management tables.\n"); 
    exit(0);
  }
      
 
  MNG->N_Fert_table   = Table[0];
  MNG->P_Fert_table   = Table[1];
  MNG->K_Fert_table   = Table[2];
  MNG->N_Uptake_frac  = Table[3];
  MNG->P_Uptake_frac  = Table[4];
  MNG->K_Uptake_frac  = Table[5];
  MNG->Irrigation     = Table[6];
  
  }