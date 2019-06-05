/**CFile***********************************************************************

  FileName    [enumMain.c]

  PackageName [enum]

  Synopsis    [This file contains routines for providing the Tcl command line
               interface to the commands provided by the enum package.]

  Author      [Shaz Qadeer]

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

#include  "enumInt.h"

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


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int ModuleComputeInitialSet(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int StateComputePostSet(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int StatePrintVariableValues(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int ModulePerformEnumerativeSearch(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Initializes the enum package.]
  
  Description        [The function registers all commands implemented by the
                      enum package with the TCL interpreter. This function is
                      called whenever mocha is started.]

  SideEffects        [The global TCL associative array mocha_commands is
                      modified.]

  SeeAlso            [Enum_Reinit, Enum_End]
  
******************************************************************************/
int
Enum_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  char cmd[512];

  Tcl_CreateCommand(interp, "enum_init", ModuleComputeInitialSet,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "enum_post", StateComputePostSet, (ClientData)
                    manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "state_print", StatePrintVariableValues,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "enum_search", ModulePerformEnumerativeSearch,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "enum_numVar", EnumNumVarCmd,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "enum_var", EnumVarCmd,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "enum_value", EnumValueCmd,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);


  Main_ManagerSetStateManager(manager, (Main_StateManagerGeneric) Enum_StateManagerAlloc());
  
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Reinitializes the enum package.]

  SideEffects        [The state manager of the enum package is reset. All
                      states are freed and the varInfo associated with all
                      modules freed. The stateCounter is also reset to 0.]

  SeeAlso            [Enum_Init, Enum_End]

******************************************************************************/
int
Enum_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
     Main_ManagerReadStateManager(manager);

  Enum_StateManagerFree(stateManager);
  Main_ManagerSetStateManager(manager, (Main_StateManagerGeneric) Enum_StateManagerAlloc());
  
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Ends the enum package.]

  Description        [The function terminates the enum package. It is called
                      when mocha exits.]

  SideEffects        [None]

  SeeAlso            [Enum_Init, Enum_Reinit]

******************************************************************************/
int
Enum_End(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
      Main_ManagerReadStateManager(manager);

  Enum_StateManagerFree(stateManager);

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Allocates and initializes a state manager.]

  SideEffects        [None]

  SeeAlso            [Enum_StateManagerFree, Enum_StateManagerReinitialize]
  
******************************************************************************/
Enum_StateManager_t *
Enum_StateManagerAlloc(
  void)
{
  Enum_StateManager_t *stateManager = ALLOC(Enum_StateManager_t, 1);

  stateManager->moduleToVarInfo = st_init_table(st_ptrcmp, st_ptrhash);
  stateManager->stateNameTable = ALLOC(Tcl_HashTable, 1);
  Tcl_InitHashTable(stateManager->stateNameTable, TCL_STRING_KEYS);
  stateManager->stateCounter = 0;
  return stateManager;
}


/**Function********************************************************************

  Synopsis           [Frees a state manager.]

  Description        [The function frees all memory associated with a state
                      manager, including all states and varInfo associated
                      with every module.]

  SideEffects        [None]

  SeeAlso            [Enum_StateManagerAlloc, Enum_StateManagerReinitialize]

******************************************************************************/
void
Enum_StateManagerFree(
  Enum_StateManager_t *stateManager)
{
  st_generator *stgen;
  char *name;
  Enum_VarInfo_t *varInfo;
  Tcl_HashSearch search;
  Enum_State_t *state;
  Tcl_HashEntry *entryPtr;
  Mdl_Module_t *module;
  
  st_foreach_item(stateManager->moduleToVarInfo, stgen, (char **) &module,
                  (char **) &varInfo) {
    Enum_VarInfoFree(varInfo);
  }
  st_free_table(stateManager->moduleToVarInfo);
  
  for(entryPtr = Tcl_FirstHashEntry(stateManager->stateNameTable, &search);
      entryPtr != NIL(Tcl_HashEntry); entryPtr = Tcl_NextHashEntry(&search))
    {
      state = (Enum_State_t *) Tcl_GetHashValue(entryPtr);
      Enum_StateFree(state);
    }
  Tcl_DeleteHashTable(stateManager->stateNameTable);
  FREE(stateManager->stateNameTable);
  
  FREE(stateManager);
}

/**Function********************************************************************

  Synopsis           [Allocates the varInfo data structure of a module.]

  SideEffects        [None]

  SeeAlso            [Enum_VarInfoFree]

******************************************************************************/
Enum_VarInfo_t *
Enum_VarInfoAlloc(
  Mdl_Module_t *module)
{
  
  lsList histDependVarList = Mdl_ModuleObtainHistoryDependentVariableList(module);
  lsList histFreeVarList = Mdl_ModuleObtainHistoryFreeVariableList(module);
  lsList eventVarList = Mdl_ModuleObtainEventVariableList(module);
  Enum_VarInfo_t *varInfo = ALLOC(Enum_VarInfo_t, 1);
  lsGeneric var;
  lsGen gen;
  int i;

  varInfo->module = module;
  
  varInfo->varToIndex = st_init_table(st_ptrcmp,st_ptrhash);
  
  varInfo->numHistDependVars = lsLength(histDependVarList);
  varInfo->histDependVars = ALLOC(Var_Variable_t *,
                                  varInfo->numHistDependVars);
  varInfo->numHistFreeVars = lsLength(histFreeVarList);
  varInfo->histFreeVars = ALLOC(Var_Variable_t *,
                                varInfo->numHistFreeVars);
  varInfo->numEventVars = lsLength(eventVarList);
  varInfo->eventVars = ALLOC(Var_Variable_t *,
                             varInfo->numEventVars);
  
  i = 0;
  lsForEachItem(histDependVarList, gen, var) {
    st_insert(varInfo->varToIndex, (char *) var, (char *) (long) i);
    varInfo->histDependVars[i] = (Var_Variable_t *) var;
    i++;
  }

  i = 0;
  lsForEachItem(histFreeVarList, gen, var) {
    st_insert(varInfo->varToIndex, (char *) var, (char *) (long) i);
    varInfo->histFreeVars[i] = (Var_Variable_t *) var;
    i++;
  }

  i = 0;
  lsForEachItem(eventVarList, gen, var) {
    st_insert(varInfo->varToIndex, (char *) var, (char *) (long) i);
    varInfo->eventVars[i] = (Var_Variable_t *) var;
    i++;
  }

  lsDestroy(histDependVarList, NULL);
  lsDestroy(histFreeVarList, NULL);
  lsDestroy(eventVarList, NULL);
  
  return varInfo;
}

/**Function********************************************************************

  Synopsis           [Frees a varInfo data structure.]

  SideEffects        [None]

  SeeAlso            [Enum_VarInfoAlloc]

******************************************************************************/
void
Enum_VarInfoFree(
  Enum_VarInfo_t *varInfo)
{
  FREE(varInfo->histDependVars);
  FREE(varInfo->histFreeVars);
  FREE(varInfo->eventVars);
  st_free_table(varInfo->varToIndex);
  
  FREE(varInfo);
}

/**Function********************************************************************

  Synopsis           [Returns the state table.]

  Description        [This function returns the state table in the state
                      manager.]

  SideEffects        [None]

******************************************************************************/
Tcl_HashTable *
Enum_StateManagerReadStateNameTable(
  Enum_StateManager_t *stateManager)
{
  return stateManager->stateNameTable;
}

/**Function********************************************************************

  Synopsis           [Returns the state counter.]

  Description        [This function returns the state counter in the state
                      manager.]

  SideEffects        [None]

******************************************************************************/
unsigned long
Enum_StateManagerReadStateCounter(
  Enum_StateManager_t *stateManager)
{
  return stateManager->stateCounter;
}

/**Function********************************************************************

  Synopsis           [Increments the state counter.]

  Description        [This function increments the state counter in the state
                      manager.]

  SideEffects        [Increments the state counter.]

******************************************************************************/
void
Enum_StateManagerIncrementStateCounter(
  Enum_StateManager_t *stateManager)
{
  ++(stateManager->stateCounter);
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Extracts the module name from a state name.]

  Description        [This function extracts the module name from a state
                      name. A state name begins with the name of the module to
                      which it belongs followed by a ".".]

  SideEffects        [None]                    
                      
******************************************************************************/
char *
StateNameExtractModuleName(
  char *stateName)
{
  int i = 0;
  char *moduleName;
  
  while(stateName[i] != '.')
    i++;

  moduleName = ALLOC(char, i+1);
  strncpy(moduleName, stateName, i);
  moduleName[i] = '\0';
  
  return moduleName;
}

/**Function********************************************************************

  Synopsis           [The function implementing the command enum_init.]

  SideEffects        [None]

  SeeAlso            [StateComputePostSet]

  CommandName        [enum_init] 	   

  CommandSynopsis    [Computes the set of initial states of a module.]  

  CommandArguments   [\[-h\] &lt;module_name&gt;]  

  CommandDescription [The command computes the set of initial states of a
  module. It takes as input a module name and returns a string of names of
  states. These state names can be used as arguments to future enum_post
  commands. 

  <dl>

  <dt> -h
  <dd> Print usage.<p>

  </dl>]


******************************************************************************/
static int
ModuleComputeInitialSet(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
      Main_ManagerReadStateManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *stateNameTable = Enum_StateManagerReadStateNameTable(stateManager);
  Mdl_Module_t *module;
  lsList initialSet, extVarList;
  char *initialSetString;
  Tcl_HashEntry *entryPtr;
  lsGen gen, varGen;
  lsGeneric state, var;
  char *stateName;
  int flag;
  Enum_VarInfo_t *varInfo;
  char *moduleName;
  Var_DataType dataType;
  int c;
  
  util_getopt_reset();
  while((c = util_getopt(argc, argv, "h")) != EOF) {
    switch(c) {
        case 'h':
        default :
          goto usage;
    }
  }
  
  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("Module not provided.\n");
    goto usage;
  }
  else if (argc - util_optind > 1) {
    Main_MochaErrorPrint("Too many arguments.\n");
    goto usage;
  }

  moduleName = argv[util_optind];
  
  if((module = Mdl_ModuleReadFromName(mdlManager, moduleName)) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s not found", moduleName);
    return TCL_ERROR;
  }
  
  varInfo = Enum_StateManagerObtainVarInfoFromModule(stateManager, module);
  
  extVarList = Mdl_ModuleGetExternalVariableList(module);
  flag = FALSE;
  lsForEachItem(extVarList, varGen, var) {
    dataType = Var_VariableReadDataType((Var_Variable_t *) var);
    if(dataType == Var_Integer_c || dataType == Var_Natural_c)
      flag = TRUE;
  }
  lsDestroy(extVarList, NULL);
  
  if(flag) {
    Main_MochaErrorPrint("Infinite domain external variables not allowed.\n");
    return TCL_ERROR;
  }
    
  initialSet = Enum_ModuleComputeSetOfInitialStates(module, varInfo);

  lsForEachItem(initialSet, gen, state) {
    unsigned long num = Enum_StateManagerReadStateCounter(stateManager);
    long length = 1;
    char *id;
    char *temp;

    if(num > 9)
      length = (long) floor(log10(num)) + 1;
    id = ALLOC(char, length + 1);
    sprintf(id, "%d", num);
    Enum_StateManagerIncrementStateCounter(stateManager);
    stateName = util_strcat3(moduleName, ".s", id);
    FREE(id);
    entryPtr = Tcl_CreateHashEntry(stateNameTable, stateName, &flag);
    Tcl_SetHashValue(entryPtr, (ClientData) state);
    Tcl_AppendElement(interp, stateName);
    FREE(stateName);
  }

  lsDestroy(initialSet, NULL);
  
  return TCL_OK;

  usage:
  Main_MochaErrorPrint("Usage: enum_init [-h] <module_name>");
  return TCL_ERROR;
}

/**Function********************************************************************

  Synopsis           [The function implementing the command enum_post.]

  SideEffects        [None]

  SeeAlso            [StateComputeInitialSet]

  CommandName        [enum_post] 	   

  CommandSynopsis    [Computes the set of post (successor) states of a state.]  

  CommandArguments   [\[-h\] &lt;state_name&gt;]  

  CommandDescription [The command computes the set of all successor states of a
  state. It takes as input the name of a state that is generated by the
  commands enum_init or enum_post. It returns a string of names of
  states. These names can be used as arguments to future enum_post commands.

  <dl>

  <dt> -h
  <dd> Print usage.<p>

  </dl>]

******************************************************************************/
static int
StateComputePostSet(
ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
      Main_ManagerReadStateManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *stateNameTable = Enum_StateManagerReadStateNameTable(stateManager);
  Mdl_Module_t *module;
  char *stateName;
  char *nextStateName;
  Enum_State_t *state;
  lsGeneric nextState, var;
  char *postSetString;
  lsList postSet, extVarList;
  lsGen gen, varGen;
  int flag;
  char *moduleName;
  Tcl_HashEntry *entryPtr;
  Enum_VarInfo_t *varInfo;
  Var_DataType dataType;
  int c;
      
  util_getopt_reset();
  while((c = util_getopt(argc, argv, "h")) != EOF) {
    switch(c) {
        case 'h' :
        default :
          goto usage;
    }
  }
  
  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("State not provided.\n");
    goto usage;
  }
  else if (argc - util_optind > 1) {
    Main_MochaErrorPrint("Too many arguments.\n");
    goto usage;
  }

  stateName = argv[util_optind];
  if((entryPtr = Tcl_FindHashEntry(stateNameTable, stateName)) ==
     NIL(Tcl_HashEntry)) {
    Main_MochaErrorPrint("State %s not found", stateName);
    return TCL_ERROR;
  }
  
  state = (Enum_State_t *) Tcl_GetHashValue(entryPtr);
  moduleName = StateNameExtractModuleName(stateName);
  module = Mdl_ModuleReadFromName(mdlManager, moduleName);
  varInfo = Enum_StateManagerObtainVarInfoFromModule(stateManager, module);

  extVarList = Mdl_ModuleGetExternalVariableList(module);
  flag = FALSE;
  lsForEachItem(extVarList, varGen, var) {
    dataType = Var_VariableReadDataType((Var_Variable_t *) var);
    if(dataType == Var_Integer_c || dataType == Var_Natural_c)
      flag = TRUE;
  }
  lsDestroy(extVarList, NULL);
  
  if(flag) {
    Main_MochaErrorPrint("Infinite domain external variables not allowed.\n");
    return TCL_ERROR;
  }

  postSet = Enum_ModuleComputeSetOfPostStates(module, varInfo, state);

  lsForEachItem(postSet, gen, nextState) {
    unsigned long num = Enum_StateManagerReadStateCounter(stateManager);
    long length = 1;
    char *id;
    char *temp;

    if(num > 9)
      length = (long) floor(log10(num)) + 1;
    id = ALLOC(char, length + 1);
    sprintf(id, "%d", num);
    Enum_StateManagerIncrementStateCounter(stateManager);
    nextStateName = util_strcat3(moduleName, ".s", id);
    FREE(id);
    entryPtr = Tcl_CreateHashEntry(stateNameTable, nextStateName, &flag);
    Tcl_SetHashValue(entryPtr, (ClientData) nextState);
    Tcl_AppendElement(interp, nextStateName);
    FREE(nextStateName); 
      }

  lsDestroy(postSet, NULL);
  FREE(moduleName);
  return TCL_OK;

  usage:
  Main_MochaErrorPrint("Usage: enum_post [-h] <state_name>");
  return TCL_ERROR;
}
    
/**Function********************************************************************

  Synopsis           [The function implementing the command state_print.]

  SideEffects        [None]

  CommandName        [state_print] 	   

  CommandSynopsis    [Prints the values of all variables in a state.]  

  CommandArguments   [\[-h\] &lt;state_name&gt;]  

  CommandDescription [The command prints the values of all variables in a
  state. The command takes as argument a state name generated by the commands
  enum_init or enum_post.

  Command Options:<p>

  <dl>

  <dt> -h
  <dd> Print usage.<p>

  </dl>]  

******************************************************************************/
static int
StatePrintVariableValues(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
      Main_ManagerReadStateManager((Main_Manager_t *) clientData);
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Tcl_HashTable *stateNameTable = Enum_StateManagerReadStateNameTable(stateManager);
  char *stateName;
  Enum_State_t *state;
  st_generator *gen;
  Var_Variable_t *var;
  char *value;
  Tcl_HashEntry *entryPtr;
  Enum_VarInfo_t *varInfo;
  int i, n;
  Var_Variable_t **varArray;
  char *moduleName;
  Mdl_Module_t *module;
  int c;
  
  util_getopt_reset();
  while((c = util_getopt(argc, argv, "h")) != EOF) {
    switch(c) {
        case 'h' :
        default :
          goto usage;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("State not provided.\n");
    goto usage;
  }
  else if (argc - util_optind > 1) {
    Main_MochaErrorPrint("Too many arguments.\n");
    goto usage;
  }

  stateName = argv[util_optind];
  
  if((entryPtr = Tcl_FindHashEntry(stateNameTable, stateName)) ==
     NIL(Tcl_HashEntry)) {
    Main_MochaErrorPrint("State %s not found", stateName);
    return TCL_ERROR;
  }

  moduleName = StateNameExtractModuleName(stateName);
  module = Mdl_ModuleReadFromName(mdlManager, moduleName);
  varInfo = Enum_StateManagerObtainVarInfoFromModule(stateManager, module);  
  state = (Enum_State_t *) Tcl_GetHashValue(entryPtr);
  FREE(moduleName);

  /* First, print the history dependent variables. */
  n = Enum_VarInfoReadNumHistDependVars(varInfo);
  varArray = Enum_VarInfoReadHistDependVars(varInfo);
  for(i = 0; i < n; i++) {
    char *name = Var_VariableReadName(varArray[i]);
    long value = state->histDependVars[i];
    Var_DataType dataType = Var_VariableReadDataType(varArray[i]);
    char *valueString;
    
    if(dataType == Var_Enumerated_c)
      Tcl_AppendResult(interp, name, "=",
                       Var_EnumElementReadName(Var_VariableObtainEnumElementFromIndex(varArray[i], value)), " ", NIL(char));    
    else
      if(dataType == Var_Boolean_c) {
        if((long) value == 0)
          Tcl_AppendResult(interp, name, "=", "0", " ", NIL(char));
        if((long) value == 1)
          Tcl_AppendResult(interp, name, "=", "1", " ", NIL(char));
      }
      else {
        valueString = ALLOC(char, sizeof(long));
        sprintf(valueString, "%d", (long) value);
        Tcl_AppendResult(interp, name, "=", valueString, " ", NIL(char));
        FREE(valueString);
      }
  }

  /* Second, print history free variables. */
  n = Enum_VarInfoReadNumHistFreeVars(varInfo);
  varArray = Enum_VarInfoReadHistFreeVars(varInfo);
  for(i = 0; i < n; i++) {
    char *name = Var_VariableReadName(varArray[i]);
    long value = state->histFreeVars[i];
    Var_DataType dataType = Var_VariableReadDataType(varArray[i]);
    char *valueString;
    
    if(dataType == Var_Enumerated_c)
      Tcl_AppendResult(interp, name, "=",
                       Var_EnumElementReadName(Var_VariableObtainEnumElementFromIndex(varArray[i], value)), " ", NIL(char));    
    else
      if(dataType == Var_Boolean_c) {
        if((long) value == 0)
          Tcl_AppendResult(interp, name, "=", "0", " ", NIL(char));
        if((long) value == 1)
          Tcl_AppendResult(interp, name, "=", "1", " ", NIL(char));
      }
      else {
        valueString = ALLOC(char, sizeof(long));
        sprintf(valueString, "%d", (long) value);
        Tcl_AppendResult(interp, name, "=", valueString, " ", NIL(char));
        FREE(valueString);
      }
  }

  /* Third, print event variables. The convention is that only those event */
  /* variable names are present that actually happened. */
  n = Enum_VarInfoReadNumEventVars(varInfo);
  varArray = Enum_VarInfoReadEventVars(varInfo);
  for(i = 0; i < n; i++) {
    char *name = Var_VariableReadName(varArray[i]);
    long value = state->eventVars[i];

    if(value == 1)
      Tcl_AppendResult(interp, name, " ", NIL(char)); 
  }

  return TCL_OK;

  usage:
  Main_MochaErrorPrint("Usage: state_print [-h] <state_name>");
  return TCL_ERROR;
}

/**Function********************************************************************

  Synopsis           [The function implementing the command enum_search.]

  SideEffects        [The command could modify the status of the invariants
                      associated with the module on which it is executed.]

  SeeAlso            [Enum_ModulePerformBFS, Enum_ModulePerformDFS]

  CommandName        [enum_search] 	   

  CommandSynopsis    [Performs enumerative state space search on a module.]  

  CommandArguments   [\[-d\] \[-h\] &lt;module_name&gt;]  

  CommandDescription [The command performs enumerative search on the state
  space of a module starting from its initial states. The default algorithm
  for the search is breadth-first.

  Command Options:<p>

  <dl>

  <dt> -h
  <dd> Print usage.<p>

  <dt> -d
  <dd> Perform a depth-first search.<p>

  </dl>]

******************************************************************************/
static int
ModulePerformEnumerativeSearch(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
   Main_Manager_t *mainManager = (Main_Manager_t *) clientData;
   Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager(mainManager);
  Mdl_Module_t *module;
  Enum_AlgorithmType algo = Enum_BFS_c;
  Enum_StateTable_t *stateTable;
  int c;
  char *moduleName;
  lsList extVarList;
  lsGen varGen;
  lsGeneric var;
  Var_DataType dataType;
  boolean flag;
  
  util_getopt_reset();
  while((c = util_getopt(argc, argv, "dh")) != EOF) {
    switch(c) {
        case 'd' :
          algo = Enum_DFS_c;
          break;
        case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("Module not provided.\n");
    goto usage;
  }
  else if (argc - util_optind > 1) {
    Main_MochaErrorPrint("Too many arguments.\n");
    goto usage;
  }

  moduleName = argv[util_optind];
  
  if((module = Mdl_ModuleReadFromName(mdlManager, moduleName)) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s not found", moduleName);
    return TCL_ERROR;
  }

  extVarList = Mdl_ModuleGetExternalVariableList(module);
  flag = FALSE;
  lsForEachItem(extVarList, varGen, var) {
    dataType = Var_VariableReadDataType((Var_Variable_t *) var);
    if(dataType == Var_Integer_c || dataType == Var_Natural_c)
      flag = TRUE;
  }
  lsDestroy(extVarList, NULL);
  
  if(flag) {
    Main_MochaErrorPrint("Infinite domain external variables not allowed.\n");
    return TCL_ERROR;
  }

  if(algo==Enum_BFS_c)
     Enum_ModulePerformBFS(interp, mainManager, module, NIL(array_t), NIL(array_t));
  else
     Enum_ModulePerformDFS(interp, mainManager, module, NIL(array_t), NIL(array_t));

  return TCL_OK;

  usage:
  Main_MochaErrorPrint("Usage: enum_search [-h] [-d] <module_name>");
  return TCL_ERROR;
  
}

/**Function********************************************************************

  Synopsis           [Add a state to the TclHashTable.]

  Description        []

  SideEffects        [return string has to be freed by user.]

  SeeAlso            []

******************************************************************************/
char *
Enum_StateNameTableAddState(
  Enum_StateManager_t * stateManager,
  char * moduleName,
  Enum_State_t * state
  ) 
{
  Tcl_HashTable *stateNameTable = Enum_StateManagerReadStateNameTable(stateManager);
  
  unsigned long num = Enum_StateManagerReadStateCounter(stateManager);
  long length = 1;
  Tcl_HashEntry *entryPtr;
  char * stateNameSuffix,  *stateName;
  int flag;

  if(num > 9)
    length = (long) floor(log10(num)) + 1;
  stateNameSuffix = ALLOC(char, length + 2);
  sprintf(stateNameSuffix, "s%d", num);
  Enum_StateManagerIncrementStateCounter(stateManager);

  stateName = Enum_StateNameTableAddStateWithNameSuffix(
    stateManager, moduleName, state, stateNameSuffix);

  FREE(stateNameSuffix);
  return stateName;
}

/**Function********************************************************************

  Synopsis           [Add a state to the TclHashTable.]

  Description        []

  SideEffects        [return string has to be freed by user.]

  SeeAlso            []

******************************************************************************/
char *
Enum_StateNameTableAddStateWithNameSuffix(
  Enum_StateManager_t * stateManager,
  char * moduleName,
  Enum_State_t * state,
  char * nameSuffix
  ) 
{
  Tcl_HashTable *stateNameTable = Enum_StateManagerReadStateNameTable(stateManager);
  Tcl_HashEntry *entryPtr;
  char * stateName;
  int flag;

  stateName = util_strcat3(moduleName, ".", nameSuffix);

  if ((entryPtr = Tcl_FindHashEntry(stateNameTable, stateName)) != NULL) {
    /* Enum_StateFree((Enum_State_t *) Tcl_GetHashValue(entryPtr)); */
    Tcl_DeleteHashEntry(entryPtr);
    /* do I need to free the hashkey? */
  } 

  entryPtr = Tcl_CreateHashEntry(stateNameTable, stateName, &flag);
  Tcl_SetHashValue(entryPtr, (ClientData) state);
  return (stateName);
}


