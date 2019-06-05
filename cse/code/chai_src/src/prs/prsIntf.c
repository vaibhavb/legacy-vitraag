#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "prsIntf.h"

char* splitIntf(char* inFile)
{ 
  FILE* input, *fi, *fo;
  char inputFile[50], inputI[50], inputO[50], module_name[256]; 
  char  parseString[100];
  int i, state = NOSTATE, var;
  strcpy(inputFile, inFile);
  input = fopen(inputFile, "r");
  sprintf(inputI, "%s.I",inputFile);
  sprintf(inputO, "%s.O",inputFile);
  fi = fopen(inputI, "w");
  fo = fopen(inputO, "w");

  while ((i = parseInput(input, parseString)) != -1)
    switch(i)
      {
      case INTERFACE: 
	writeIO(fi, "module");
	writeIO(fo, "module");
	state = INTERFACE;	
	break;

      case INPUT:	 
	var = INPUT;
	break;

      case OUTPUT:
	var = OUTPUT;
	break;	
	
      case VARS:
	if (var == INPUT)
	  {
	    writeIO(fi,"external");
	    writeIO(fo,"interface");
	  }
	else if(var == OUTPUT)
	  {
	   writeIO(fo,"external");
	   writeIO(fi,"interface");
	  }
	break;

      case ATOM:
	if (var == INPUT)
	  {
	    writeIO(fi,"atom");
	    state = INPUTATOM;
	  }
	else if(var == OUTPUT)
	  {
	    writeIO(fo,"atom");
	    state = OUTPUTATOM;	  
	  }
	break;

      case ENDATOM:
	if (state == INPUTATOM)
	  writeIO(fi, parseString);
	else if (state == OUTPUTATOM)
	  writeIO(fo, parseString);
	state = NOSTATE;
	break;

      case ENDINTERFACE:
	writeIO(fi, "endmodule");
	writeIO(fo, "endmodule");
	break;

      case SPACE:	
	if (state == INPUTATOM)
	  writeIO(fi, parseString);
	else if (state == OUTPUTATOM)
	  writeIO(fo, parseString);
	else{
	  writeIO(fi, parseString);
	  writeIO(fo, parseString);
	}
	break;

      case NOMATCH:
	if (state == INPUTATOM)
	  writeIO(fi, parseString);
	else if (state == OUTPUTATOM)
	  writeIO(fo, parseString);
	else{
	  if(state==INTERFACE){
	    strcpy(module_name, parseString);
	    writeIO(fi, strcat(strdup(parseString), "I"));
	    writeIO(fo, strcat(strdup(parseString), "O"));
	    state = NULL;
	  }
	  else{
	    writeIO(fi, parseString);
	    writeIO(fo, parseString);
	  }
	}
	break;
	
      default:
       	perror("Error!\n");
      }		
  printf("Done..\n");
  fclose(fi);
  fclose(fo);
  return module_name;
}


int parseInput(FILE* fp, char* s)
{
  if (myfscanf(fp,s) == EOF) return -1;
  printf("%s", s);
  if (strcmp(s, "interface")==0)
     return INTERFACE;
  else if(strcmp(s, "input")==0)
    return INPUT;
  else if(strcmp(s, "output")==0)
    return OUTPUT;
  else if(strcmp(s, "vars")==0 || strcmp(s, "vars:")==0)
    return VARS;
  else if(strcmp(s, "atom")==0)
    return ATOM;
  else if(strcmp(s, "endatom")==0)
    return ENDATOM;
  else if (strcmp(s, "endinterface")==0)
    return ENDINTERFACE;
  else if (strcmp(s, " ")==0)
    return SPACE;
  else if (strcmp(s, "\t")==0)
    return SPACE;
  else if (strcmp(s, "\n")==0)
    return SPACE;
  else 
    return NOMATCH;
}


void writeIO(FILE* fi, const char* s)
{
  fprintf(fi, "%s", s);
  return;
}


int myfscanf(FILE* fp, char* s)
{
  int i;
  char c;

  if (fscanf(fp, "%c", &c)==EOF) 
    return EOF;

  if (isspace((int) c))
    {
      s[0] = c;
      s[1] = '\0';
      return 1;
    }
  else
    s[0] = c;
      
  for (i=1; i<99; i++)
    {
      if (fscanf(fp, "%c", &c)==EOF) return EOF;
      if (isspace((int) c))
	{
	  ungetc((int)c, fp);
	  s[i] = '\0';
	  return 1;
	}
      else
	s[i] = c;
    }	

  return 0;
}















