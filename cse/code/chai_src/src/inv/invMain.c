/**CFile***********************************************************************

  FileName    [InvMain.c]

  PackageName [Inv]

  Synopsis    [Contains routines for implementing user level commands.]

  SeeAlso     [Inv.y Inv.l InvUtil.c Inv.h InvInt.h]

  Author      [Freddy Mang and Shaz Qadeer]

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

#include  "invInt.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
EXTERN FILE* InvYyin;

/**Variable********************************************************************

  Synopsis    [Pointer to the Invariant Manager]

  Description [This pointer is set to the invariant manager where formulas are
  stored.  This pointer is visible only in this Main file.]

******************************************************************************/

static Inv_Manager_t* InvManager;
static int InvFormulaCounter;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int InvariantFileRead(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int InvariantPrint(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int InvariantCheck(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Init function for tcl.]

  Description        [It creates all the new tcl commands defined in this
  package.]

  SideEffects        [None]

******************************************************************************/
int
Inv_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{

  Tcl_CreateCommand(interp, "inv_read",
                    InvariantFileRead, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "inv_print",
                    InvariantPrint, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "inv_check",
                    InvariantCheck, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  /* also initialize the appropriate data structures */
  InvInitialize();

  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Inv_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
   Inv_ManagerFree(InvManager);
   InvInitialize();

   return TCL_OK;   
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Inv_End(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
   Inv_ManagerFree(InvManager);

   return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Export the Inv Global Manager]

  Description        [This function exports the Inv Global Manager to the
  calling function.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Inv_Manager_t *
Inv_ReadInvManager () 
{
  return InvManager;
}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
InvInitialize()
{
  /* initialize the InvGlobalManager */
  InvManager = Inv_ManagerAlloc();
  InvFormulaCounter = 0;

}


/**Function********************************************************************

  Synopsis           [Returns an internally generated new invariant name.]

  Description        [This routine returns an internally generated new
  invariant name which serves as an ID to the unnamed invariant. ]

  SideEffects        [Should free the string after use.]

  SeeAlso            [optional]

******************************************************************************/
char *
InvObtainNewFormulaName()
{
  char *counterString = ALLOC(char, INDEX_STRING_LENGTH);
  char *name;

  sprintf(counterString, "%d",InvFormulaCounter);
  name = util_strcat3("inv", counterString, "");
  
  InvFormulaCounter++;
  FREE(counterString);

  return name;

}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Implements the command inv_read.]

  SideEffects        [None]

  CommandName        [inv_read] 	   

  CommandSynopsis    [Reads in a file containing the invariants.]  

  CommandArguments   [\[-h\] &lt;file_name&gt;]  

  CommandDescription [This command reads in a file containing invariants. The
  syntax of the invariants inside the file is as follows. <p>
  "spec_name1" formula1; <p>
  "spec_name2" formula2; <p>
  ...... <p>
    
  Command Options:<p>

  <dl>

  <dt> -h
  <dd> Print usage.<p>

  </dl>]

******************************************************************************/
static int
InvariantFileRead(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{

  array_t * formulaArray;
  FILE *fp;
  lsList formulaRead;
  char *fileName;
  int c;
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != EOF) {
    switch (c) {
        case 'h':
           goto usage;
           break;
        default:
           goto usage;
           break;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("Invariant file not provided.\n");
    goto usage;
  }
  else if (argc - util_optind > 1) {
     Main_MochaErrorPrint("Too many arguments.\n");
     goto usage;
  }

  fileName = argv[util_optind];
  fp = fopen (fileName, "r");

  if (fp == NIL(FILE)) {
    Main_MochaErrorPrint("Cannot read %s.\n", fileName);
    return TCL_ERROR;
  }

  formulaRead =  Inv_FileParseFormula(InvManager, fp);

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

 usage:
  Main_MochaErrorPrint("inv_read [-h] <file_name>\n");
  return TCL_ERROR;
}

/**Function********************************************************************

  Synopsis           [Implements the command inv_print.]

  SideEffects        [None]

  CommandName        [inv_print] 	   

  CommandSynopsis    [Prints all the invariants.]  

  CommandArguments   [\[-l\] \[invariant_name\]]  

  CommandDescription [The command prints invariants. If invariant_name is
  specified then only that invariant is printed else all invariants are
  printed.
  <p>

  Command options:<p>

  <dl>

  <dt> -h
  <dd> Print usage.<p>
  
  <dt> -l
  <dd> The full invariant expression is printed.<p>

  </dl>]  

******************************************************************************/
static int
InvariantPrint(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char c;
  boolean listlong = FALSE;
  array_t *nameArray;
  array_t *formulaArray;
  char *name;
  int numFormula, i;
  Inv_Invariant_t *formula;
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "hl")) != EOF) {
    switch (c) {
        case 'h':
           goto usage;
           break;
        case 'l':
           listlong = TRUE;
           break;
        default:
           goto usage;
           break;
    }
  }

  if (argc - util_optind > 1) {
     Main_MochaErrorPrint("Too many arguments.\n");
     goto usage;
  }
  
  if (argc - util_optind == 0) {
    nameArray = Inv_ManagerReadInvariantNames(InvManager);
    formulaArray = Inv_ManagerReadInvariants(InvManager);
  }
  else {
    name = argv[util_optind];
    if ((formula = Inv_ManagerReadInvariantFromName(InvManager, name))
        == NIL(Inv_Invariant_t)) {
      Main_MochaErrorPrint("%s not found.\n", name);
      return TCL_ERROR;
    }
    else {
      nameArray = array_alloc(char *, 1);
      formulaArray = array_alloc(Inv_Invariant_t *, 1);
      
      array_insert (char *, nameArray, 0, name);
      array_insert (Inv_Invariant_t *, formulaArray, 0, formula);
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
        
        formula = array_fetch(Inv_Invariant_t *, formulaArray, i);
	formulaString = InvExprConvertToString(formula->invExpr);
        Tcl_AppendResult(interp, formulaString, "\n\n", NIL(char));
      }
    }
  }
  
  array_free(formulaArray);
  array_free(nameArray);
  
  return TCL_OK;

 usage:
  Main_MochaErrorPrint("inv_print [-h] [-l] [formula_name]\n");
  return TCL_ERROR;
}

/**Function********************************************************************

  Synopsis           [Implements the command inv_check.]

  SideEffects        [None]

  CommandName        [inv_check] 	   

  CommandSynopsis    [Checks invariants on a module.]  

  CommandArguments   [\[-h\] \[-e search_method\] &lt;module_name&gt;
  \[inv_name\] \[inv_name\] ...]  

  CommandDescription [This command checks the specified invariants on the
  specified module. The invariants have to be read using the command inv_read
  before this command is invoked. If no invariants are specified on the
  command line, then all invariants that have been read are checked. If the -e
  option has been used, use enumerative search else symbolic search. If the -r
  option has been used, use symbolic breadth-first search on real-time modules.
  The -r option should be used with real-time modules only, and takes precedence
  over the -e option.<p>

  Command Options:<p>

  <dl>

  <dt> -h
  <dd> Print usage.<p>

  <dt> -e search_method
  <dd> Use the specified search method. search_method must be one of the
  following. <p>

  <code> 0 </code>: Use breadth-first search.<p>
  <code> 1 </code>: Use depth-first search.<p>
  
  
  </dl>]  

******************************************************************************/
static int
InvariantCheck(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
   Main_Manager_t *mainManager = (Main_Manager_t *) clientData;
   Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
     Main_ManagerReadModuleManager(mainManager);
  Var_TypeManager_t *typeManager = (Var_TypeManager_t *)
     Main_ManagerReadTypeManager(mainManager);
  char c;
  array_t *invNameArray;
  array_t *invariantArray;
  char *moduleName, *invName;
  int numFormula, i;
  Inv_Invariant_t *invariant;
  boolean useEnum = FALSE;
  boolean useRtm = FALSE;
  boolean error;
  int enumMethod;
  Mdl_Module_t *module;
  Atm_Expr_t *typedExpr;
  array_t *typedExprArray;
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "hre:")) != EOF) {
    switch (c) {
        case 'h':
           goto usage;
           break;
        case 'e':
	   useEnum = TRUE;
	   for (i = 0; i < strlen(util_optarg); i++) {
	      if (!isdigit(util_optarg[i])) {
		 goto usage;
	      }
	   }
	   
	   enumMethod = atoi(util_optarg);
	   if (enumMethod != 0 && enumMethod != 1)
	      goto usage;
	   break;

        case 'r':
          if (useEnum)
            goto usage;
          useRtm = TRUE;
	   break;

        default:
	   goto usage;
	   break;
    }
  }

  if (argc - util_optind == 0) {
     Main_MochaErrorPrint("No module name given\n");
     goto usage;
  }
  else {
    moduleName = argv[util_optind++];
    if ((module = Mdl_ModuleReadFromName(mdlManager, moduleName))
        == NIL(Mdl_Module_t)) {
      Main_MochaErrorPrint("Module %s not found.\n", moduleName);
      return TCL_ERROR;
    }
    else {
       if (argc - util_optind == 0) {
	  invNameArray = Inv_ManagerReadInvariantNames(InvManager);
	  invariantArray = Inv_ManagerReadInvariants(InvManager);
       }
       else {
	  invNameArray = array_alloc(char *, 0);
	  invariantArray = array_alloc(Inv_Invariant_t *, 0);
	  
	  while (util_optind < argc) {
	     invName = argv[util_optind];
	     if ((invariant = Inv_ManagerReadInvariantFromName(InvManager, invName))
		 != NIL(Inv_Invariant_t)) {
		array_insert_last(char *, invNameArray, invName);
		array_insert_last(Inv_Invariant_t *, invariantArray, invariant);
	     }
	     else {
		Main_MochaErrorPrint("Invariant %s not found\n", invName);
		array_free(invNameArray);
		array_free(invariantArray);
		goto usage;
	     }
	     util_optind++;
	  }
       }
    }
  }

  error = FALSE;
  typedExprArray = array_alloc(Atm_Expr_t *, 0);
  arrayForEachItem(Inv_Invariant_t *, invariantArray, i, invariant) {
     invName = array_fetch(char *, invNameArray, i);
     Main_MochaPrint("Typechecking invariant %s...\n", invName);
     if ((typedExpr = InvInvariantCreateTypedExpr(module, typeManager, invariant))
	 == NIL(Atm_Expr_t)) {
	Main_MochaErrorPrint("Error in typechecking invariant %s\n", invName);
	error = TRUE;
     }
     else {
	array_insert_last(Atm_Expr_t *, typedExprArray, typedExpr);
	Main_MochaPrint("Typechecking successful\n");
     }
  }

  if (error) {
     arrayForEachItem(Atm_Expr_t *, typedExprArray, i, typedExpr) {
	Atm_ExprFreeExpression(typedExpr);
     }

     array_free(invariantArray);
     array_free(invNameArray);
     array_free(typedExprArray);
     return TCL_ERROR;
  }

  if (useRtm) {
    Rtm_ModulePerformBFS(interp, mainManager, module, invNameArray, typedExprArray);
  }
  else if (useEnum) {
     if (enumMethod == 0) {
       Enum_ModulePerformBFS(interp, mainManager, module, invNameArray, typedExprArray);
     }
     else {
       Enum_ModulePerformDFS(interp, mainManager, module, invNameArray, typedExprArray);
     }
  }
  else {
       Sym_ModulePerformBFS(interp, mainManager, module, invNameArray, typedExprArray);
  }
  
  arrayForEachItem(Atm_Expr_t *, typedExprArray, i, typedExpr) {
     Atm_ExprFreeExpression(typedExpr);
  }
  
  array_free(invariantArray);
  array_free(invNameArray);
  array_free(typedExprArray);
  return TCL_OK;

 usage:
  Main_MochaErrorPrint("inv_check [-h] [-e search_method] <module_name> <formula_name> <formula_name>....\n");
  return TCL_ERROR;
}



