/**CFile***********************************************************************

  FileName    [enumUtil.c]

  PackageName [enum]

  Synopsis    [Utility functions pretaining to the enum package.]

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

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [The function implementing the command enum_numVar.]

  SideEffects        [None]

  SeeAlso            []

  CommandName        [enum_numVar] 	   

  CommandSynopsis    [returns number of variables in a state.]

  CommandArguments   [\[-h\] &lt;state&gt;]  

  CommandDescription [The command returns the number of variables in a state.]

  <dl>

  <dt> -h
  <dd> Print usage.<p>

  </dl>]


******************************************************************************/
int
EnumNumVarCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
#define Usage_enum_numVar "enum_numVar <state>"

  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
      Main_ManagerReadStateManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *stateNameTable = Enum_StateManagerReadStateNameTable(stateManager);
  Mdl_Module_t *module;
  char *stateName;
  char *nextStateName;
  Enum_State_t *state;
  char *moduleName;
  Tcl_HashEntry *entryPtr;
  Enum_VarInfo_t *varInfo;
  char c;
  
  util_getopt_reset();
  while((c = util_getopt(argc, argv, "h")) != EOF) {
    switch(c) {
        case 'h' :
          Main_MochaPrint("%s", Usage_enum_numVar);
          return TCL_OK;
          break;
        default :
          Main_MochaPrint("%s", Usage_enum_numVar);
          return TCL_OK;
          break;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("%s\n", Usage_enum_numVar);
    return TCL_OK;
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

  Main_ClearResult();
  Main_AppendResult("%d",Enum_VarInfoReadNumHistDependVars(varInfo) +
                    Enum_VarInfoReadNumHistFreeVars(varInfo) +
                    Enum_VarInfoReadNumEventVars(varInfo));

  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [The function implementing the command enum_numVar.]

  SideEffects        [None]

  SeeAlso            []

  CommandName        [enum_numVar] 	   

  CommandSynopsis    [returns number of variables in a state.]

  CommandArguments   [\[-h\] &lt;state&gt;]  

  CommandDescription [The command returns the number of variables in a state.]

  <dl>

  <dt> -h
  <dd> Print usage.<p>

  </dl>]


******************************************************************************/
int
EnumVarCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
#define Usage_enum_var "enum_var <state> <num>"

  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
      Main_ManagerReadStateManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *stateNameTable = Enum_StateManagerReadStateNameTable(stateManager);
  Mdl_Module_t *module;
  char *stateName;
  char *nextStateName;
  Enum_State_t *state;
  char *moduleName;
  Tcl_HashEntry *entryPtr;
  Enum_VarInfo_t *varInfo;
  char c;
  int index;
  int t;
  Var_Variable_t ** varArray;
  Var_Variable_t * var;
  
  util_getopt_reset();
  while((c = util_getopt(argc, argv, "h")) != EOF) {
    switch(c) {
        case 'h' :
          Main_MochaPrint("%s", Usage_enum_var);
          return TCL_OK;
          break;
        default :
          Main_MochaPrint("%s", Usage_enum_var);
          return TCL_OK;
          break;
    }
  }

  if (argc - util_optind != 2) {
    Main_MochaErrorPrint("%s\n", Usage_enum_var);
    return TCL_OK;
  }

  stateName = argv[util_optind];
  index = atoi(argv[util_optind+1]);
  
  if((entryPtr = Tcl_FindHashEntry(stateNameTable, stateName)) ==
     NIL(Tcl_HashEntry)) {
    Main_MochaErrorPrint("State %s not found", stateName);
    return TCL_ERROR;
  }

  state = (Enum_State_t *) Tcl_GetHashValue(entryPtr);
  moduleName = StateNameExtractModuleName(stateName);
  module = Mdl_ModuleReadFromName(mdlManager, moduleName);
  varInfo = Enum_StateManagerObtainVarInfoFromModule(stateManager, module);

  Main_ClearResult();
  
  t = Enum_VarInfoReadNumHistDependVars(varInfo);
  if (index < t) {
    varArray = Enum_VarInfoReadHistDependVars(varInfo);
    Main_AppendResult("%s", Var_VariableReadName(*(varArray + index)));
    return TCL_OK;
  } 

  index = index - t;
  t = Enum_VarInfoReadNumHistFreeVars(varInfo);
  if (index < t) {
    varArray = Enum_VarInfoReadHistFreeVars(varInfo);
    Main_AppendResult("%s", Var_VariableReadName(*(varArray + index)));
    return TCL_OK;
  } 
  
  index = index - t;
  t= Enum_VarInfoReadNumEventVars(varInfo);
  if (index < t) {
    varArray = Enum_VarInfoReadEventVars(varInfo);
    Main_AppendResult("%s", Var_VariableReadName(*(varArray + index)));
    return TCL_OK;
  } 
  
  Main_MochaErrorPrint("Error: Index out of range.\n");

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [The function implementing the command enum_numVar.]

  SideEffects        [None]

  SeeAlso            []

  CommandName        [enum_numVar] 	   

  CommandSynopsis    [returns number of variables in a state.]

  CommandArguments   [\[-h\] &lt;state&gt;]  

  CommandDescription [The command returns the number of variables in a state.]

  <dl>

  <dt> -h
  <dd> Print usage.<p>

  </dl>]


******************************************************************************/
int
EnumValueCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
#define Usage_enum_value "enum_value <state> <num>"

  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
      Main_ManagerReadStateManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *stateNameTable = Enum_StateManagerReadStateNameTable(stateManager);
  Mdl_Module_t *module;
  char *stateName;
  char *nextStateName;
  Enum_State_t *state;
  char *moduleName;
  Tcl_HashEntry *entryPtr;
  Enum_VarInfo_t *varInfo;
  char c;
  int index;
  int t;
  Var_Variable_t ** varArray;
  Var_Variable_t * var;
  
  util_getopt_reset();
  while((c = util_getopt(argc, argv, "h")) != EOF) {
    switch(c) {
        case 'h' :
          Main_MochaPrint("%s", Usage_enum_value);
          return TCL_OK;
          break;
        default :
          Main_MochaPrint("%s", Usage_enum_value);
          return TCL_OK;
          break;
    }
  }

  if (argc - util_optind != 2) {
    Main_MochaErrorPrint("%s\n", Usage_enum_value);
    return TCL_OK;
  }
  
  stateName = argv[util_optind];
  index = atoi(argv[util_optind+1]);
  
  if((entryPtr = Tcl_FindHashEntry(stateNameTable, stateName)) ==
     NIL(Tcl_HashEntry)) {
    Main_MochaErrorPrint("State %s not found", stateName);
    return TCL_ERROR;
  }

  state = (Enum_State_t *) Tcl_GetHashValue(entryPtr);
  moduleName = StateNameExtractModuleName(stateName);
  module = Mdl_ModuleReadFromName(mdlManager, moduleName);
  varInfo = Enum_StateManagerObtainVarInfoFromModule(stateManager, module);

  Main_ClearResult();
  
  t = Enum_VarInfoReadNumHistDependVars(varInfo);
  if (index < t) {
    char * s;
    varArray = Enum_VarInfoReadHistDependVars(varInfo);
    s = EnumPrintValueToString(varArray[index],
                               state->histDependVars[index]);
    Main_AppendResult("%s", s);
    FREE(s);
    return TCL_OK;
  } 

  index = index - t;
  t = Enum_VarInfoReadNumHistFreeVars(varInfo);
  if (index < t) {
    char * s;
    varArray = Enum_VarInfoReadHistFreeVars(varInfo);
    s = EnumPrintValueToString(varArray[index],
                               state->histDependVars[index]);
    Main_AppendResult("%s", s);
    FREE(s);
    return TCL_OK;
  } 
  
  index = index - t;
  t= Enum_VarInfoReadNumEventVars(varInfo);
  if (index < t) {
    char * s;
    varArray = Enum_VarInfoReadEventVars(varInfo);
    s = EnumPrintValueToString(varArray[index],
                               state->histDependVars[index]);
    Main_AppendResult("%s", s);
    FREE(s);
    return TCL_OK;
  } 
  
  Main_MochaErrorPrint("Error: Index out of range.\n");

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Returns a string indicating the value of a variable.]

  Description        [This function, given a variable and a numeric value of
                      that variable, returns a string indicating the symbolic
                      value of that variable.]

  SideEffects        [User has to free the returned string.]

  SeeAlso            []


******************************************************************************/
char *
EnumPrintValueToString(
  Var_Variable_t * var,
  long value)
{
  char *valueString;
  Var_DataType dataType = Var_VariableReadDataType(var);

  if (value == LONG_MAX)
    return util_strsav(" ");
  
  if(dataType == Var_Enumerated_c) {
    valueString = util_strsav(Var_EnumElementReadName(
      Var_VariableObtainEnumElementFromIndex(
        var, value)));
  } else if(dataType == Var_Boolean_c) {
    if((long) value == 0)
      valueString = util_strsav("false");
    if((long) value == 1)
      valueString = util_strsav("true");
  } else if (dataType == Var_Event_c) {
    if((long) value == 1)
      valueString = util_strsav("x");
    else 
      valueString = util_strsav(" ");
  } else {
    valueString = ALLOC(char, sizeof(long));
    sprintf(valueString, "%d", (long) value);
  }
  
  return valueString;
}
