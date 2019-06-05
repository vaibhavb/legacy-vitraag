/**CFile***********************************************************************

  FileName    [mcMain.c]

  PackageName [mc]

  Synopsis    [Initialization of the ATL model checking package.]

  Description [This file contains all the initialization routine of the
  ATL model-checking package, user command definition and documentation.
  This package works hand-in-hand with the ATL parser, atlp.]

  SeeAlso     [atlp]

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
#include  "mcInt.h"
#include <signal.h> 
#include <setjmp.h> 

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct McOptionsStruct McOptions_t;

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************

  Synopsis    [A struct for keeping the command line options.]

******************************************************************************/
struct McOptionsStruct {
  Mc_VerbosityLevel verbosityLevel;
  McDbgLevel dbgLevel;
  int timeOutPeriod;
  array_t * formulaArray;
  array_t * formulaNameArray;
  Mdl_Module_t * module;
};


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
static jmp_buf timeOutEnv;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static  McModelCheck(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static McOptions_t * McOptionsParse(int argc, char ** argv, Mdl_Manager_t * mdlManager);
static McOptions_t * McOptionsAlloc();
static void McOptionsFree(McOptions_t * options);
static void McNameArrayFree(array_t * nameArray);
static void TimeOutHandle();

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Tcl Initialization of the package]

  Description [Called by TclAppInit(TkAppInit) for initialization of
  package. It also calls Atlp_Init to initialize the formula parsing
  package. Moreover, it creates the command mc for model-checking.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int
Mc_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{

  Tcl_CreateCommand(interp, "atl_check",
                    McModelCheck, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  /* also initialize the Atlp package */
  return (Atlp_Init(interp, manager));

}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [ATL Model-checking Command]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [McOptionsParse]

  CommandName        [atl_check] 	   

  CommandSynopsis    [Model check ATL formula]  

  CommandArguments   [\[-h\] &lt;module&gt; &lt;formula&gt; \[&lt;formula&gt ...\]]

  CommandDescription [Model check ATL formula.

  Command Options:<p>

  <dl>
  
  <dt> -h
  <dd> Prints the usage of the command.

  </dl>]

******************************************************************************/
static
McModelCheck(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  McOptions_t * options;
  Mc_VerbosityLevel verbosityLevel;
  McDbgLevel dbgLevel;
  array_t *mcFormulaArray, *mcFormulaNameArray;
  Mdl_Module_t * module;
  boolean flag;
  int numFormulas, i;
  int timeOutPeriod = 0;
  mdd_t * initialStates;
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Var_TypeManager_t *typeManager = (Var_TypeManager_t *)
      Main_ManagerReadTypeManager((Main_Manager_t *) clientData);
  Sym_Info_t *symInfo;
    
  /* parsing the options */
  if ((options = McOptionsParse(argc, argv, mdlManager)) == NIL(McOptions_t)) {
    return TCL_ERROR;
  }
  
  /* retrieve the option data */
  verbosityLevel	= options -> verbosityLevel;
  dbgLevel		= options -> dbgLevel;
  timeOutPeriod		= options -> timeOutPeriod; 
  mcFormulaArray        = options -> formulaArray; 
  mcFormulaNameArray    = options -> formulaNameArray;
  module		= options -> module;
  McOptionsFree(options);
  
  /* check if the module is update */
  /* if not, then print a warning message */
  if (!Mdl_ModuleIsUpdate(module, mdlManager))
    Main_MochaErrorPrint("Warning: Module %s is not update!\n",
                         Mdl_ModuleReadName(module));

  /* Foreach of the formula */
  /* perform semantic check on the formula against the module */
  /* if all fail type checking, return TCL_ERROR */

  numFormulas = array_n (mcFormulaArray);
  flag = TRUE;
  
  /* Convert to an existential dag */
  Main_MochaErrorPrint(
    "Converting formula to existential normal form...\n");

  {
    array_t *temp = Atlp_FormulaArrayConvertToDAG( mcFormulaArray );
    array_free( mcFormulaArray );
    mcFormulaArray = temp;

    temp = Atlp_FormulaDAGConvertToExistentialFormDAG( mcFormulaArray );
    Atlp_FormulaArrayFree( mcFormulaArray );

    mcFormulaArray = temp;
  }
    

  Main_MochaErrorPrint("Performing semantic check on the formulas...\n");
  for (i=0; i<numFormulas; i++) {
    Atlp_Formula_t * formula;
    char * formulaName;

    formula	= array_fetch(Atlp_Formula_t *, mcFormulaArray, i);
    formulaName = array_fetch(char *, mcFormulaNameArray, i);
    
    if (!Mc_FormulaSemanticCheck(formula, module, typeManager)) {
      /* failing the semantic check */
      flag = FALSE;
    }

  }


  if (!flag) {
    Atlp_FormulaArrayFree(mcFormulaArray);
    McNameArrayFree(mcFormulaNameArray);
    return TCL_ERROR;
  }

  /* Formulas Pass the semantic check, so start model-checking */
  /* should create a child process to do the model-checking */
  /* mode of model-checking is based on the options */

  
  /* set the timeOutPeriod */
  if (timeOutPeriod > 0){
    (void) signal(SIGALRM, TimeOutHandle);
    (void) alarm(timeOutPeriod);
    if (setjmp(timeOutEnv) > 0) {
      Main_MochaErrorPrint("Timeout occured after %d seconds.\n",
                           timeOutPeriod);
      alarm(0);
      Atlp_FormulaArrayFree(mcFormulaArray);
      McNameArrayFree(mcFormulaNameArray);
      return TCL_OK;
    }
  }

  /* initialization for model-checking */
  Main_MochaErrorPrint("Start model checking...\n");
  {
    Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
        Main_ManagerReadRegionManager((Main_Manager_t *) clientData); 
    
    symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);
    if (!symInfo) {
      /* there is transition relation associated with this module */
      /* build it first, by calling the TCL command sym_trans <module> */
      Main_MochaErrorPrint(
        "Building transition relations for module...\n");
      Tcl_VarEval(interp, "sym_trans ", Mdl_ModuleReadName(module), NIL(char));
      symInfo = Sym_RegionManagerReadSymInfo(
        regionManager, module);
    }

    /* build the initial region of the module */
    Main_MochaErrorPrint("Building the initial region of the module...\n");
    initialStates = Sym_ModuleBuildInitialRegion(symInfo,module);
  }
     
  for (i=0; i<numFormulas; i++) {
    Atlp_Formula_t * formula;
    char * name;
    mdd_t * atlFormulaStates;
    
    formula = array_fetch(Atlp_Formula_t *, mcFormulaArray, i);
    name    = array_fetch(char *, mcFormulaNameArray, i);
    
    Main_MochaErrorPrint("Model-checking formula \"%s\"\n", name);

    /* debug */
    /*
    {
      char * formulaString = Atlp_FormulaConvertToString(formula);
      fprintf(stderr, "%s\n", formulaString);
      FREE(formulaString);
    }
    */
    /* */
    
    atlFormulaStates = Mc_FormulaEvaluate(symInfo, formula, verbosityLevel);

    /* debug 
    Sym_MddPrintCubes(symInfo, atlFormulaStates,  0);
    */
    
    if (mdd_lequal(initialStates , atlFormulaStates, 1, 1)) {
      Main_MochaPrint("ATL_CHECK: formula \"%s\" passed\n", name);
    } else {
      Main_MochaPrint("ATL_CHECK: formula \"%s\" failed\n", name);
      /*
        should display error trace at this point, based on the debugLevel
        */        
    }

    mdd_free(atlFormulaStates);
  }

  mdd_free(initialStates);

  Atlp_FormulaArrayFree(mcFormulaArray);
  McNameArrayFree(mcFormulaNameArray);

  return TCL_OK;

}


/**Function********************************************************************

  Synopsis           [Parse the model check options]

  Description        [This function concerns parsing of the options for mc.]

  SideEffects        []

  SeeAlso            [McModelCheck]

******************************************************************************/
static McOptions_t *
McOptionsParse (
  int argc,
  char ** argv,
  Mdl_Manager_t * mdlManager)
{
  McOptions_t * options;  
  char c;
  int i;
  array_t * formulaArray,  *formulaNameArray;
  Atlp_Manager_t * atlpManager;
  Mdl_Module_t * module;
  boolean flag = TRUE;
  int dbgLevel=0;
  int timeOutPeriod=0;
  int verbosityLevel=0;
  
  options = McOptionsAlloc();
  
  util_getopt_reset();
  /* temporary disabled, because they are not implemented yet */
  /* while ((c=util_getopt(argc, argv, "hv:d:t:")) != EOF) {*/
  while ((c=util_getopt(argc, argv, "h")) != EOF) {
    switch (c) {
        case 'h':
          goto usage;
          break;
        case 'v':
          for (i=0; i<strlen(util_optarg); i++) {
            if (!isdigit(util_optarg[i])) {
              goto usage;
            }
          }
          verbosityLevel = atoi (util_optarg);
          break;
        case 'd':
          for (i=0; i<strlen(util_optarg); i++) {
            if (!isdigit(util_optarg[i])) {
              goto usage;
            }
          }
          dbgLevel = atoi (util_optarg);
          break;
        case 't':
          timeOutPeriod = atoi (util_optarg);
          break;
        default:
          goto usage;
          break;
    }
  }
  
  /* module and formulas? */
  if (argc - util_optind < 2)
    goto usage;
  
  /* check if the module exists? */
  if ((module = Mdl_ModuleReadFromName(mdlManager, argv[util_optind]))
      == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("module %s not found.\n", argv[util_optind]);
    return NIL(McOptions_t);
  }
  
  /* check if the formulas are defined */
  /* if so, put the formulas in a formula array */
  atlpManager = Atlp_ReadAtlpManager();
  formulaArray = array_alloc(Atlp_Formula_t *, 0);
  formulaNameArray = array_alloc(char *, 0);

  /* setting the options */
  options -> module = module;
  options -> timeOutPeriod = timeOutPeriod;
  options -> formulaArray = formulaArray;
  options -> formulaNameArray = formulaNameArray;

  for (i=util_optind+1; i < argc; i++) {
    char * name;
    Atlp_Formula_t * formula;
    
    name = argv[i];
    if ((formula = Atlp_FormulaReadByName(name, atlpManager))
        == NIL(Atlp_Formula_t)) {
      Main_MochaErrorPrint("%s : no such formula.\n", name);
      flag = FALSE;
    }
    
    else {
      /* stop duplicating the formulas if flag == FALSE */
      if (flag) {
        array_insert_last(Atlp_Formula_t *, formulaArray,
                          Atlp_FormulaDup(formula));
        array_insert_last(char *, formulaNameArray, util_strsav(name));
      }
    }
  }
  
  
  if (!flag) goto usage; /* any formula failed */
  
  switch (verbosityLevel) {
      case 0:
        options -> verbosityLevel = McVerbosityNone_c;
        break;
      case 1:
        options -> verbosityLevel = McVerbositySome_c;
        break;
      case 2:
        options -> verbosityLevel = McVerbosityMax_c;
        break;
      default:
        goto usage;
  }
  
  switch (dbgLevel) {
      case 0:
        options -> dbgLevel = McDbgLevelNone_c;
        break;
      case 1:
        options -> dbgLevel = McDbgLevelMin_c;
        break;
      case 2:
        options -> dbgLevel = McDbgLevelMax_c;
        break;
      case 3:
        options -> dbgLevel = McDbgLevelInteractive_c;
        break;
      default:
        goto usage;
  }
      

  return options;
  
  usage:
  Main_MochaErrorPrint(
    "usage: atl_check [-h] <module> <formula> [<formula> ...]\n");

  Atlp_FormulaArrayFree(options->formulaArray);
  McNameArrayFree(options->formulaNameArray);
  McOptionsFree(options);
  return NIL(McOptions_t);

}

/**Function********************************************************************

  Synopsis           [Allocation of Options.]

  Description        [It allocates an McOptions_t for parsing.]

  SideEffects        []

  SeeAlso            [optional]
  
******************************************************************************/
static McOptions_t *
McOptionsAlloc()
{
  McOptions_t * options;
  
  options = ALLOC(McOptions_t, 1);
  options -> module = NIL(Mdl_Module_t);
  options -> timeOutPeriod = -1;
  options -> formulaArray = NIL(array_t);
  options -> formulaNameArray = NIL(array_t);

  return options;
}

/**Function********************************************************************

  Synopsis           [Freeing of Options.]

  Description        [optional]

  SideEffects        []

  SeeAlso            [the options is freed.]
  
******************************************************************************/
static void
McOptionsFree(
  McOptions_t * options
  )
{
  if (options != NIL(McOptions_t)) {
    FREE(options);
  }
}

/**Function********************************************************************

  Synopsis           [Free an array of names.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
McNameArrayFree(
  array_t * nameArray)
{

  if (nameArray != NIL(array_t)) {
    int i; 
    int numNames = array_n(nameArray);
    
    for (i = 0; i < numNames; i++) {
      char *name = array_fetch(char *, nameArray, i);
      FREE(name);
    }
    
    array_free(nameArray);
  }
}


/**Function********************************************************************

  Synopsis    [Handle function for timeout.]

  Description [This function is called when the time out occurs.]

  SideEffects []

******************************************************************************/
static void
TimeOutHandle()
{
  longjmp(timeOutEnv, 1);
}










