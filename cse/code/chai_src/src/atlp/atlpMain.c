/**CFile***********************************************************************

  FileName    [atlpMain.c]

  PackageName [atlp]

  Synopsis    [Contains routines to interface with cmd.]

  Description [optional]

  SeeAlso     [atlp.y atlp.l atlpUtil.c atlp.h atlpInt.h]

  Author      [Freddy Mang]

  Copyright   [Copyright (c) 1994-1996 The Regents of the Univ. of California.
  All rights reserved.

  Permission is hereby granted, without written agreement and without license
  or royalty fees, to use, copy, modify, and distribute this software and its
  documentation for any purpose, provided that the above copyright notice and
  the following two paragraphs appear in all copies of this software.

  IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
  CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN
  "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.]

******************************************************************************/

#include  "atlpInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define Usage_atl_read  "atl_read <filename>"
#define Usage_atl_print "atl_print [-l] <formula_name>"
#define Usage_atl_convertToDag "_atl_convertToDag <fname> <fname> ..."

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
EXTERN FILE* AtlpYyin;

/**Variable********************************************************************

  Synopsis    [Pointer to the ATL Manager]

  Description [This pointer is set to the atl manager where formulas are
  stored.  This pointer is visible only in this Main file.]

******************************************************************************/

static Atlp_Manager_t* atlpManager;
static int atlpFormulaCounter;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int AtlpReadCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int AtlpPrintCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int AtlpConvertToDagCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static void AtlpInitialize();
static void FormulaPrintFromArray(array_t * formulaArray);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Init function for tcl.]

  Description        [It creates all the new tcl commands defined in this
  package. Also, it initialize the data structures used in this package.]

  SideEffects        [AtlpInitialize is called, and some global structures are
  allocated.]

  SeeAlso            [AtlpInitialize]

******************************************************************************/
int
Atlp_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{

  Tcl_CreateCommand(interp, "atl_read",
                    AtlpReadCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "atl_print",
                    AtlpPrintCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "_atl_convertToDag",
                    AtlpConvertToDagCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);


  /* also initialize the appropriate data structures */
  AtlpInitialize();

  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Export the Atlp Global Manager]

  Description        [This function exports the Atlp Global Manager to the
  calling function. Casual user should not free the manager.]

  SideEffects        [none]

  SeeAlso            [optional]

******************************************************************************/
Atlp_Manager_t *
Atlp_ReadAtlpManager () 
{
  return atlpManager;
}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Returns an internally generated new name.]

  Description        [This routine returns an internally generated new name
  which serves as an ID to the unnamed ATL formula.]

  SideEffects        [Should free the name after use.]

  SeeAlso            [optional]

******************************************************************************/
char *
AtlpObtainNewFormulaName()
{
  char *counterString = ALLOC(char, INDEX_STRING_LENGTH);
  char *name;

  sprintf(counterString, "%d",atlpFormulaCounter);
  name = util_strcat3("atl", counterString, "");
  
  atlpFormulaCounter++;
  FREE(counterString);

  return name;

}



/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Executes the command atlp_read]

  SideEffects        []

  CommandName        [atl_read] 	   

  CommandSynopsis    [Reads in a formula containing the ATL formulas]  

  CommandArguments   [\[-h\] &lt;file_name&gt;]  

  CommandDescription [The command reads in a file of ATL specifications. The
  specifications are type-checked and stored.  Each specification is
  associated with an identifier. For unnamed specification, an internally
  generated new name is used to identify it.  To view all read specifications,
  use atl_print

  Command Options:<p>

  <dl>

  <dt> -h
  <dd> print usage <p>

  </dl>]

******************************************************************************/
static int
AtlpReadCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{

  array_t * formulaArray;
  FILE *fp;
  lsList formulaRead;
  char c;

  util_getopt_reset();

  while((c = util_getopt(argc, argv, "h")) != EOF) {
    switch(c) {
        case 'h' :
          Main_MochaErrorPrint("%s\n", Usage_atl_read);
          return TCL_OK;
        default :
          Main_MochaErrorPrint("%s\n", Usage_atl_read);
          return TCL_ERROR;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("%s\n", Usage_atl_read);
    return TCL_ERROR;
  }


  fp = fopen (argv[util_optind],"r");

  if (fp == NIL(FILE)) {
    Main_MochaErrorPrint("Cannot read %s.\n", argv[argc-1]);
    return TCL_ERROR;
  }

  formulaRead =  Atlp_FileParseFormula(atlpManager, fp);

  {
    /* set up the tcl interpreter */
    /* values are printed to the tcl interpreter */
    char * Tcl_Empty, * name;
    lsGen gen;

    Tcl_Empty = ALLOC(char, 1);
    *Tcl_Empty = '\0';
    Tcl_SetResult(interp, Tcl_Empty, (Tcl_FreeProc *) free);
    lsForEachItem(formulaRead, gen, (lsGeneric) name) {
      Tcl_AppendResult(interp, name, "\n" , NIL(char));
    }
  }
  
  lsDestroy (formulaRead, free);
  fclose(fp);

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Executes the command atl_print]

  SideEffects        [required]

  CommandName        [atl_print]

  CommandSynopsis    [Prints an ATL formula.]

  CommandArguments   [\[-lh\] \[<formula>\]]  

  CommandDescription [This command prints the names of all the ATL formulas that
  have been read.

  Command Options:<p>

  <dl>

  <dt> -l
  <dd> Prints the name as well as the formula itself.

  <dt> -h
  <dd> Prints Usage.

  <dt> <formula>
  <dd> Prints only the specified formula.

  
  </dl>]
]  

******************************************************************************/
static int
AtlpPrintCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char c;
  boolean listlong=FALSE;
  array_t *nameArray;
  array_t *formulaArray;
  char *name;
  int numFormula, i;
  Atlp_Formula_t *formula;
  
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "lh")) != -1) {
    switch (c) {
        case 'l':
          listlong = TRUE;
          break;
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_atl_print);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_atl_print);
          return TCL_ERROR;
          break;
    }
  }

  if (argc - util_optind == 0) {
    nameArray = Atlp_ManagerReadFormulaNames(atlpManager);
    formulaArray = Atlp_ManagerReadFormulas (atlpManager);
  } else {
    name = argv[util_optind];
    if ((formula = Atlp_FormulaReadByName(name, atlpManager))
        == NIL(Atlp_Formula_t)) {
      Main_MochaErrorPrint("%s not found.\n", name);
      return TCL_ERROR;
    } else {
      nameArray = array_alloc(char *, 1);
      formulaArray = array_alloc(Atlp_Formula_t *, 1);
      
      array_insert (char *, nameArray, 0, name);
      array_insert (Atlp_Formula_t *, formulaArray, 0, formula);
    }
  }

  {
    /* set up the tcl interpreter */
    /* values are printed to the tcl interpreter */
    char* Tcl_Empty;

    Tcl_Empty = ALLOC(char, 1);
    *Tcl_Empty = '\0';
    Tcl_SetResult(interp, Tcl_Empty, (Tcl_FreeProc *) free);

    numFormula = array_n(nameArray);
    
    for(i=0; i<numFormula; i++) {
      name = array_fetch(char *, nameArray, i);
      
      Tcl_AppendResult(interp, name, "\n", NIL(char));
      if (listlong) {
        char *formulaString;
        
        formula = array_fetch(Atlp_Formula_t *, formulaArray, i);
        formulaString = Atlp_FormulaConvertToString(formula);
        Tcl_AppendResult(interp, formulaString, "\n\n", NIL(char));

        FREE(formulaString);
      }
    }
  }
  
  array_free(formulaArray);
  array_free(nameArray);
  
  return TCL_OK;
}

  
/**Function********************************************************************

  Synopsis           [Convert an array of formula to a DAG.]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [_atlp_convertToDag] 	   

  CommandSynopsis    [Command that converts a list of formula into a DAG.]  

  CommandArguments   [optional]  

  CommandDescription [Temporary command for debugging only.]  

******************************************************************************/
static int
AtlpConvertToDagCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char c;
  int i;
  array_t *formulaArray, *convertedFormulaArray;
  boolean opt_e = FALSE; /* convert to existential dag? */
  

  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "e")) != -1) {
    switch (c) {
        case 'e':
          opt_e = TRUE;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_atl_convertToDag);
          return TCL_ERROR;
          break;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("%s\n", Usage_atl_convertToDag);
    return TCL_ERROR;
  }


  formulaArray = array_alloc(Atlp_Formula_t *, 0);
  
  for (i=util_optind; i < argc; i++) {
    char * name;
    Atlp_Formula_t * formula;

    name = argv[i];
    if ((formula = Atlp_FormulaReadByName(name, atlpManager))
        == NIL(Atlp_Formula_t)) 
      Main_MochaErrorPrint("%s : no such formula.\n", name);
    else
      array_insert_last(Atlp_Formula_t *, formulaArray,
                        Atlp_FormulaDup(formula));
  }

  convertedFormulaArray = Atlp_FormulaArrayConvertToDAG(formulaArray);

  if (opt_e) {
    array_t * tmpArray;
    
    tmpArray = Atlp_FormulaDAGConvertToExistentialFormDAG(
      convertedFormulaArray);

    Atlp_FormulaArrayFree(convertedFormulaArray);
    convertedFormulaArray = tmpArray;
  }
   

  FormulaPrintFromArray(convertedFormulaArray);
  
  Atlp_FormulaArrayFree(convertedFormulaArray);
  
  array_free(formulaArray);
  
  return TCL_OK;
  
}


/**Function********************************************************************

  Synopsis           [Initialize the global atlp manager.]

  Description        [This function allocates the globla atlp manager. The
  manager is used to manage the formula.]

  SideEffects        [The manager is global, and there should only be one such
  manager in one mocha session.]

  SeeAlso            [Atlp_ManagerAlloc]

******************************************************************************/
static void
AtlpInitialize()
{
  /* initialize the AtlpGlobalManager */
  atlpManager = Atlp_ManagerAlloc();
  atlpFormulaCounter = 0;
}

/**Function********************************************************************

  Synopsis           [Prints each of the formula in an array.]

  Description        [This routine prints the formulas in the given
  formulaArray. This function is called by AtlpPrintCmd.]

  SideEffects        [required]

  SeeAlso            [AtlpPrintCmd]

******************************************************************************/

static void
FormulaPrintFromArray(
  array_t * formulaArray
  )
{

  if (formulaArray != NIL(array_t)) {
    int i;
    int numFormula = array_n(formulaArray);
    char * formulaString; 
    Atlp_Formula_t *formula;
    
    for (i=0; i<numFormula; i++) {
      formula = array_fetch(Atlp_Formula_t *, formulaArray, i);
      formulaString = Atlp_FormulaConvertToString(formula);
      Main_MochaPrint("%s\n", formulaString);
      FREE(formulaString);
    }
  }
  
}


