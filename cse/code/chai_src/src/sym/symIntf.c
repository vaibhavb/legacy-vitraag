/**CFile***********************************************************************

  FileName    [symIntf.c]

  PackageName [sym]

  Synopsis    [Tcl/Tk interface to  image computation ]

  Description []

  Author      [Sriram Rajamani]

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
#include "symInt.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
bdd_reorder_type_t  DynamicVarOrderingMethod = BDD_REORDER_SIFT; 

/*
 * States of the state machine used to parse the input node list file.
 */
#define STATE_TEST 0 /* next char is in first column */
#define STATE_WAIT 1 /* wait until end of line '\n' is reached */
#define STATE_IN   2 /* parsing a node name */

/*
 * Maximum permissible length of a node name in the input node list file.
 */
#define MAX_NAME_LENGTH 200


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct VarOrderStruct VarOrder_t;

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
struct VarOrderStruct {
  Var_Variable_t *var;
  int prime;
  array_t *bddIdArray;
};


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/



/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int CommandComputeReachSet(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandComputeInitSet(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandBuildTrans(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandComputePostSet(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandComputeUnion(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandPrintRegion(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandDynamicVarOrdering(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandWriteOrder(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandReadOrder(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static array_t * VarBuildBddIdArray(Sym_Info_t *symInfo, Var_Variable_t *var, boolean primed);
static int VarOrdersCompareBddIdArray(lsGeneric node1, lsGeneric node2);
static int IntegersCompare(char ** obj1, char ** obj2);
static bdd_reorder_type_t StringConvertToDynOrderType(char *string);
static char * DynOrderTypeConvertToString(bdd_reorder_type_t method);
static array_t * ModuleTotalOrderVariables(Mdl_Module_t *module, array_t *varOrderArray);
static int VarOrderIsInArray(array_t *varOrderArray, VarOrder_t *varOrder);
static void ModuleAssignMddIdsToVariables(Mdl_Module_t *module, Sym_Info_t *symInfo, array_t *totalVarArray);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Initilization function for sym package]

  Description        [This function is called when Mocha comes up and does the following:
                      1. Create hooks to sym package's functions for the
		      supported commands
		      2. Allocate memory for SymRegionManager and deposit it in the
		      main manager]

  SideEffects        [none]

  SeeAlso            [Sym_End, Sym_Reinit]

******************************************************************************/
int Sym_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Tcl_CreateCommand(interp, "sym_trans", CommandBuildTrans, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sym_init", CommandComputeInitSet, (ClientData)
                    manager, (Tcl_CmdDeleteProc *)NULL);

  Tcl_CreateCommand(interp, "sym_search", CommandComputeReachSet, (ClientData)
                    manager, (Tcl_CmdDeleteProc *)NULL);
  
  Tcl_CreateCommand(interp, "sym_post", CommandComputePostSet, (ClientData)
                    manager, (Tcl_CmdDeleteProc *)NULL);

  Tcl_CreateCommand(interp, "sym_union", CommandComputeUnion, (ClientData)
                    manager, (Tcl_CmdDeleteProc *)NULL);

  Tcl_CreateCommand(interp, "sym_print", CommandPrintRegion, (ClientData)
                    manager, (Tcl_CmdDeleteProc *)NULL);

  Tcl_CreateCommand(interp, "sym_dynamic_var_ordering", CommandDynamicVarOrdering, (ClientData)
                    manager, (Tcl_CmdDeleteProc *)NULL);

  Tcl_CreateCommand(interp, "sym_write_order", CommandWriteOrder, (ClientData)
                    manager, (Tcl_CmdDeleteProc *)NULL);

  Tcl_CreateCommand(interp, "sym_static_order", CommandReadOrder, (ClientData)
                    manager, (Tcl_CmdDeleteProc *)NULL);

  Main_ManagerSetRegionManager(manager, (Main_RegionManagerGeneric) Sym_RegionManagerAlloc());
  
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Re-initialization function for sym package]

  Description        [This function is called when the user types a re-init command. 
                      Re-initialization is not yet supported by sym.
		      Currently, this function does nothing]

  SideEffects        [none]

  SeeAlso            [Sym_Init, Sym_End]

******************************************************************************/
int
Sym_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager(manager);

   Sym_RegionManagerFree(regionManager);
   Main_ManagerSetRegionManager(manager, (Main_RegionManagerGeneric) Sym_RegionManagerAlloc());
   
   return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Termination function for sym package]

  Description        [Currently this function does nothing. During a subsequent
                      cleanup this should be made to free all memeory allocated by sym]

  SideEffects        [none]

  SeeAlso            [Sym_Init, Sym_Reinit]

******************************************************************************/
int
Sym_End(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
       Main_ManagerReadRegionManager(manager);

   Sym_RegionManagerFree(regionManager);
   
   return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Read region manager's region table]

  Description        [Given a pointer to the region manager, return the
                      region table (a Tcl hash table) associated with it.
		      The region table hashes region names to SymRegionStruct
		      vlaues. The user should NOT free this table]

  SideEffects        [none]

  SeeAlso            [Sym_RegionManagerReadCounter]

******************************************************************************/
Tcl_HashTable *
Sym_RegionManagerReadRegionTable(
  Sym_RegionManager_t *regionManager
  ) 
{
  return(regionManager->regionTable);
}



/**Function********************************************************************

  Synopsis           [Read region manager's region counter]

  Description        [Given a pointer to the region manager, return
                      the value of the region counter inside the manager]

  SideEffects        [none]

  SeeAlso            [Sym_RegionManagerReadRegionTable, Sym_RegionManagerIncrementCounter]

******************************************************************************/

unsigned long 
Sym_RegionManagerReadCounter(
  Sym_RegionManager_t *regionManager
  ) 
{
  return(regionManager->regionCounter);
}




/**Function********************************************************************

  Synopsis           [Increment region manager's region counter]

  Description        [Given a pointer to the region manager, increment
                      the value of the region counter]

  SideEffects        [none]

  SeeAlso            [Sym_RegionManagerReadCounter]

******************************************************************************/
void
Sym_RegionManagerIncrementCounter(
  Sym_RegionManager_t *regionManager
  ) 
{
  (regionManager->regionCounter)++;
}


/**Function********************************************************************

  Synopsis           [Allocate a region struct]

  Description        [Allocate a region struct and set its module
                      and image pointers to the arguments passed]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Sym_Region_t *
Sym_RegionAlloc(
  Mdl_Module_t *module,
  mdd_t *regionMdd) 
{
  Sym_Region_t *region = ALLOC(Sym_Region_t, 1);

  region -> module = module;
  region -> mdd    = regionMdd;

  return region; 
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Sym_RegionFree(
  Sym_Region_t *region)
{
   if (region->mdd != NIL(mdd_t))
      mdd_free(region->mdd);

   FREE(region);
}

  
/**Function********************************************************************

  Synopsis           [Allocate a region manager]


  Description        [A region manager manintains:
                     1. mapping from modules to their symInfos and 
		     2. region tables mapping region names to regions
		     3. region counter used to generate unique region names

		     This routine allocates and returns a 
		     pointer to a region manager]

  SideEffects        [required]

******************************************************************************/
Sym_RegionManager_t *
Sym_RegionManagerAlloc(
  )
{
  Sym_RegionManager_t *regionManager = ALLOC(Sym_RegionManager_t, 1);

  regionManager->moduleToSymInfo = st_init_table(st_ptrcmp, st_ptrhash);
  regionManager->regionTable = ALLOC(Tcl_HashTable, 1);
  Tcl_InitHashTable(regionManager->regionTable, TCL_STRING_KEYS);
  regionManager->regionCounter = 0;

  return regionManager;
}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Sym_RegionManagerFree(
  Sym_RegionManager_t *regionManager)
{
   Mdl_Module_t *module;
   Sym_Info_t *symInfo;
   st_generator *stgen;
   Tcl_HashSearch search;
   Tcl_HashEntry *entryPtr;
   char *name;
   Sym_Region_t *region;
   
   st_foreach_item(regionManager->moduleToSymInfo, stgen, (char **) &module, (char **) &symInfo) {
      Sym_SymInfoFree(symInfo);
   }
   st_free_table(regionManager->moduleToSymInfo);

   for(entryPtr = Tcl_FirstHashEntry(regionManager->regionTable, &search);
       entryPtr != NIL(Tcl_HashEntry); entryPtr = Tcl_NextHashEntry(&search))
   {
      region = (Sym_Region_t *) Tcl_GetHashValue(entryPtr);
      Sym_RegionFree(region);
   }

   Tcl_DeleteHashTable(regionManager->regionTable);
   FREE(regionManager->regionTable);

   FREE(regionManager);
}


/**Function********************************************************************

  Synopsis           [Find the symInfo for the given module]

  Description        [Given a region manager and a module pointer,
                      this routine returns a pointer to the module's
		      symInfo it it exists, and 0 otherwise.
		      The caller should NOT free this pointer]

  SideEffects        [SymRegionManagerAddModuleToSymInfo]

******************************************************************************/
Sym_Info_t *
Sym_RegionManagerReadSymInfo(
  Sym_RegionManager_t *regionManager,
  Mdl_Module_t *module)
{
  Sym_Info_t *symInfo;
  st_table *moduleToSymInfo =
    regionManager->moduleToSymInfo;
  
  if(st_lookup(moduleToSymInfo, (char *) module, (char **) &symInfo))
    return symInfo;
  else
    return NIL(Sym_Info_t);
}

/**Function********************************************************************

  Synopsis           [Add a module and symInfo pair to the moduleToSymInfo table]

  Description        [Given a module and its symInfo, add it to the region manager's
                      moduleToSymInfo hash table]

  SideEffects        [none]

  SeeAlso            [Sym_RegionManagerReadSymInfo, Sym_RegionManagerDeleteModuleToSymInfo]

******************************************************************************/
void
Sym_RegionManagerAddModuleToSymInfo(
  Sym_RegionManager_t *regionManager,
  Mdl_Module_t *module,
  Sym_Info_t *symInfo)
{
  Sym_Info_t *oldSymInfo;
  
  if (st_lookup(regionManager->moduleToSymInfo, (char *) module, (char **)
                &oldSymInfo))
    Sym_SymInfoFree(oldSymInfo);
  
  st_insert(regionManager->moduleToSymInfo, (char *) module, (char *) symInfo);
}


/**Function********************************************************************

  Synopsis           [Delete a module-syminfo pair from moduleToSymInfo table]

  Description        [Given a module, delete it (and it's symInfo) from the
                      region manager's moduleToSymInfo table. If the module is
		      not in the table, this is a no-op]

  SideEffects        [none]

  SeeAlso           [Sym_RegionManagerAddModuleToSymInfo]

******************************************************************************/
void
Sym_RegionManagerDeleteModuleToSymInfo(
  Sym_RegionManager_t *regionManager,
  Mdl_Module_t *module)
{
  Sym_Info_t *symInfo;
  st_delete(regionManager->moduleToSymInfo, (char **) &module, (char **) &symInfo);
}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Write variable order for symInfo into a file]

  Description        [Write variable order for symInfo into a file]

  SideEffects        [none]
  
  SeeAlso            [SymReadOrder]

******************************************************************************/
void
SymWriteOrder(
  FILE *fp,
  Sym_Info_t *symInfo)
{
  Mdl_Module_t *module = symInfo->transRelation->module;
  VarOrder_t *varOrder;
  lsList varOrderList = lsCreate();
  int nameLength, maxNameLength;
  st_generator *stgen;
  char *name;
  Var_Variable_t *var;
  lsGen lsgen;
  
  Mdl_ModuleForEachVariable(module, stgen, name, var) {
    Var_Type_t * type = Var_VariableReadType(var);

    if (!Var_VariableIsDummy(var) &&
        !Var_TypeIsArray(type) &&
        !Var_TypeIsBV(type)) {
      varOrder = ALLOC(VarOrder_t, 1);
      varOrder->var = var;
      varOrder->prime = 0;
      varOrder->bddIdArray = VarBuildBddIdArray(symInfo, var, 0);
      lsNewEnd(varOrderList, (lsGeneric) varOrder, LS_NH);
      
      varOrder = ALLOC(VarOrder_t, 1);
      varOrder->var = var;
      varOrder->prime = 1;
      varOrder->bddIdArray = VarBuildBddIdArray(symInfo, var, 1);
      lsNewEnd(varOrderList, (lsGeneric) varOrder, LS_NH);
    }
  }

  lsSort(varOrderList, VarOrdersCompareBddIdArray);

  /*
   * Compute the maximum length of a node name, for pretty printing.
   */
  maxNameLength = 0;
  lsForEachItem(varOrderList, lsgen, varOrder) {
    int nameLength = strlen(Var_VariableReadName(varOrder->var));
    maxNameLength = (maxNameLength < nameLength) ? nameLength : maxNameLength;
  }

  /* Increase by one to account for the " ' " at the end. */
  
  maxNameLength++;
  
  lsForEachItem(varOrderList, lsgen, varOrder) {
    int i;
    array_t *bddIdArray = varOrder->bddIdArray;
    Var_Variable_t *var = varOrder->var;
    char *varTypeName = Var_VarTypeReadName(Var_VariableReadType(var));
    int mddId;

    if (varOrder->prime) {
      char *primedName = util_strcat3(Var_VariableReadName(var), "'", "");
      Sym_SymInfoLookupPrimedVariableId(symInfo, var, &mddId);
      (void) fprintf(fp, "%-*s  %-16s %5d ",
                     maxNameLength,
                     primedName,
                     varTypeName,
                     mddId);
      FREE(primedName);
    }
    else {
      Sym_SymInfoLookupUnprimedVariableId(symInfo, var, &mddId);
      (void) fprintf(fp, "%-*s  %-16s %5d ",
                     maxNameLength,
                     Var_VariableReadName(var),
                     varTypeName,
                     mddId);
    }
        
    (void) fprintf(fp, "(");
    for (i = 0; i < array_n(bddIdArray); i++) {
      int level = array_fetch(int, bddIdArray, i);
      
      if (i == 0) {
        (void) fprintf(fp, "%d", level);
      }
      else {
        (void) fprintf(fp, ", %d", level);
      }
    }
    (void) fprintf(fp, ")\n");
    array_free(bddIdArray);
    FREE(varOrder);
  }
}

/**Function********************************************************************

  Synopsis           [Read variable order for module from a file]

  Description        [Write variable order for module from a file]

  SideEffects        [none]
  
  SeeAlso            [SymWriteOrder]

******************************************************************************/
array_t *
SymReadOrder(
  FILE *fp,
  Mdl_Module_t *module)
{
  int     c, i, prime;
  int     state;
  int     curPosition = 0;
  boolean status;
  char    nameString[MAX_NAME_LENGTH];
  boolean flag = TRUE;
  array_t *varOrderArray = array_alloc(VarOrder_t *, 0);
  VarOrder_t *varOrder;
  Var_Variable_t *var;
  
  state = STATE_TEST;
  while ((c = fgetc(fp)) != EOF) {

    switch (state) {
        case STATE_TEST:
          /* At start of a new line. */
          if (c == '#') {
            /* Line starting with comment character; wait for newline */
            state = STATE_WAIT;
          }
          else if ((c == ' ') || (c == '\t')) {
            /* Line starting with white space; wait for newline */
            state = STATE_WAIT;
          }
          else if (c == '\n') {
            /* Line starting newline; go to next line */
            state = STATE_TEST;
          }
          else {
            /* Assume starting a node name. */
            curPosition = 0;
            nameString[curPosition++] = c;
            state = STATE_IN;
          }
          break;
        case STATE_WAIT:
          /*
           * Waiting for the newline character.
           */
          state = (c == '\n') ? STATE_TEST : STATE_WAIT;
          break;
        case STATE_IN:
          /*
           * Parsing a node name.  If white space reached, then terminate the
           * node name and process it.  Else, continue parsing.
           */
          if ((c == ' ') || (c == '\n') || (c == '\t')) {
            nameString[curPosition] = '\0';

            /* Check if the last character in the string is " ' " (the prime
             * symbol denoting that the variable is the next state variable.
             */

            if (nameString[curPosition-1] == '\'') {
              prime = 1;
              nameString[curPosition-1] = '\0';
            }
            else
              prime = 0;
            
            var = Mdl_ModuleReadVariableFromName(nameString, module);
            
            if (var == NIL(Var_Variable_t)) {
              Main_MochaErrorPrint("Node not found for name: %s\n", nameString);
              /* flag = FALSE; */
            }
            else {
              varOrder = ALLOC(VarOrder_t, 1);
              varOrder->var = var;
              varOrder->prime = prime;
              varOrder->bddIdArray = NIL(array_t);
              array_insert_last(VarOrder_t *, varOrderArray,  varOrder);  
            }
            
            state = (c == '\n') ? STATE_TEST : STATE_WAIT;
          }
          else {
            nameString[curPosition++] = c;
            if (curPosition >= MAX_NAME_LENGTH) {
              error_append("maximum node name length exceeded");
              flag = FALSE;
            }
            state = STATE_IN; /* redundant, but be explicit */
          }
          break;
        default:
          fail("unrecognized state");
    }
  }

  /*
   * Handle case where EOF terminates a name.
   */
  if (state == STATE_IN) {
    nameString[curPosition] = '\0';
    if (nameString[curPosition-1] == '\'') {
      prime = 1;
      nameString[curPosition-1] = '\0';
    }
    else
      prime = 0;
    
    var = Mdl_ModuleReadVariableFromName(nameString, module);
    
    if (var == NIL(Var_Variable_t)) {
      Main_MochaErrorPrint("Node not found for name: %s\n", nameString);
      /* flag = FALSE; */
    }
    else {
      varOrder = ALLOC(VarOrder_t, 1);
      varOrder->var = var;
      varOrder->prime = prime;
      varOrder->bddIdArray = NIL(array_t);
      array_insert_last(VarOrder_t *, varOrderArray,  varOrder);  
    }
  }

  if (flag) {
    /* Check for duplicate occurrences of variables. If it occurs, print a
     *  warning and remove the second occurrence.
     */
    array_t *tempArray = array_alloc(VarOrder_t *, 0);
    arrayForEachItem(VarOrder_t *, varOrderArray, i, varOrder) {
      if (VarOrderIsInArray(tempArray, varOrder) == -1)
        array_insert_last(VarOrder_t *, tempArray, varOrder);
      else {
        Main_MochaErrorPrint("Warning: duplicate names in the input file\n");
        FREE(varOrder);
      }
    }
    array_free(varOrderArray);
    return tempArray;
  }
  else {
    arrayForEachItem(VarOrder_t *, varOrderArray, i, varOrder) {
      FREE(varOrder);
    }
    array_free(varOrderArray);
    return NIL(array_t);
  }
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [driver for building reached set]

  Description        [Do symbolic reachability for a module. A transition 
                      relation must exist before thie function is called ]

  SideEffects        [1. The command could modify the status of the invariants
                      associated with the module on which it is executed.
		      2. The mdd represnting all reached states is added to the
		      region table (of the region manager)]

  SeeAlso            [CommandComputeInitSet]

  CommandName        [sym_search] 	   

  CommandSynopsis    [Performs symbolic (mdd based) state space search on a module ]  


  CommandArguments   [\[-v\]  \[-h\] &lt;module_name&gt;]  

  CommandDescription [The command performs MDD based symbolic  search on the state
  space of a module starting from its initial region. The build_trans command
  must be executed prior to  executing this command.
  

  Command Options:<p>

  <dl>

  <dt> -v
  <dd> verbose mode - prints MDDs associated with each intermediate step<p>

  <dt> -n
  <dd> stop reachability after prescribed numbe of steps

  <dt> -h
  <dd> print usage <p>

  </dl>]
******************************************************************************/
static int
CommandComputeReachSet(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *regionTable = Sym_RegionManagerReadRegionTable(regionManager);
  Mdl_Module_t *module;
  Sym_Info_t *symInfo;
  char *reachRegionName;
  mdd_t *reachSet;
  int flag,c;
  Tcl_HashEntry *entryPtr;
  boolean verbose = FALSE;
  boolean checkInvariants = FALSE;
  int maxNumSteps = -1;
  
  util_getopt_reset();

  while((c = util_getopt(argc, argv, "n:vh")) != EOF) {
    switch(c) {
        case 'v' :
          verbose = TRUE;
          break;
        case 'n':
          maxNumSteps = atoi(util_optarg);
          Main_MochaPrint("limiting number of reachability steps to %d\n",maxNumSteps);
          break;
        case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }

  if (argc - util_optind == 0) {
    goto usage;
  }


  if((module = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }

  symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);
  if(!symInfo){
    char cmd[512];
    Main_MochaPrint( "No sym_info.. building it(using sym_trans)\n");
    sprintf(cmd, "sym_trans %s", Mdl_ModuleReadName(module));
    Tcl_Eval(interp, cmd);
    symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);  
    if(!symInfo){
      Main_MochaErrorPrint( "Command: %s unsuccessful\n", cmd);
      return(TCL_ERROR);
    }
  }
  
  reachSet = SymModuleSearch(module, symInfo, verbose, FALSE, 0, 0, interp, maxNumSteps);
  Main_MochaPrint( "NOTE: Regions are represented only in terms of history dependent variables...\n");

  {
  Sym_Region_t *reach = Sym_RegionAlloc(module, reachSet);
  unsigned long num = Sym_RegionManagerReadCounter(regionManager);
  long length = 1;
  char *id;
  char *temp;

  if(num > 9)
    length = (long) floor(log10(num)) + 1;
  id = ALLOC(char, length + 1);
  sprintf(id, "%d", num);
  Sym_RegionManagerIncrementCounter(regionManager);
  reachRegionName = util_strcat3(argv[util_optind], ".r", id);
  FREE(id);

  entryPtr = Tcl_CreateHashEntry(regionTable, reachRegionName, &flag);
  Tcl_SetHashValue(entryPtr, (ClientData) reach);
  Tcl_AppendElement(interp, reachRegionName);
  FREE(reachRegionName);
  }

  return(TCL_OK);
usage:
  Main_MochaErrorPrint( "Usage: sym_search [-v] [-h] <module>\n");
  return TCL_ERROR;

  
}



/**Function********************************************************************

  Synopsis           [driver for building initial state]

  Description        [Compute the mdd representing initial states of a module]

  SideEffects        [The initial region computed is added to the region manager's
                      region table]

  SeeAlso            [CommandComputeReachedSet]

  CommandName        [sym_init] 	   

  CommandSynopsis    [compute initial set of states of a module (represented
  as MDD). The build_trans command must be executed prior to executing this
  command]

  CommandArguments   [\[-v\]  \[-h\] &lt;module_name&gt;]  

  CommandDescription [The command computes the initial region of a module.

  Command Options:<p>

  <dl>

  <dt> -v
  <dd> verbose mode <p>

  <dt> -h
  <dd> print usage <p>

  </dl>]
******************************************************************************/
static int
CommandComputeInitSet(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *regionTable = Sym_RegionManagerReadRegionTable(regionManager);
  Mdl_Module_t *module;
  Sym_Info_t *symInfo;
  char *initRegionName;
  int flag;
  Tcl_HashEntry *entryPtr;
  char *moduleName;
  int i, c;
  mdd_t *initSet;



  boolean verbose = FALSE;
  util_getopt_reset();

  while((c = util_getopt(argc, argv, "vh")) != EOF) {
    switch(c) {
        case 'v' :
          verbose = TRUE;
          break;
        case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }


  if (argc - util_optind == 0) {
    goto usage;
  }

  if((module = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }
  moduleName = argv[util_optind];
  
  symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);
  if (!symInfo) {
    char cmd[512];
    Main_MochaPrint("Ordering variables using sym_static_order\n");
    sprintf(cmd, "sym_static_order %s", Mdl_ModuleReadName(module));
    Tcl_Eval(interp, cmd);
    symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);
    if(!symInfo){
      Main_MochaErrorPrint("internal error: Cannot obtain symInfo for %s\n",
                           Mdl_ModuleReadName(module));
      return TCL_ERROR;
    }
  }

  initSet = Sym_ModuleBuildInitialRegion(symInfo,module);

  {
    /* quantify out history free and event variables */
  array_t *histFreeIdArray    = array_alloc(int, 0);
  array_t *eventIdArray    = array_alloc(int, 0);
  lsList varList;
  lsGen  gen;
  lsGeneric v;
  mdd_t *temp1;
  int varid;

  varList = Mdl_ModuleObtainVariableList(module);
  lsForEachItem(varList, gen, v) {
     if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, (Var_Variable_t *)v, &varid )){
        Main_MochaErrorPrint( "Error: SymSearchModule, cant find mdd id for var\n");
     }
     
     if(Var_VariableIsEvent((Var_Variable_t *) v)){
        array_insert_last(int, eventIdArray, varid);
     }
     else if(Var_VariableIsHistoryFree((Var_Variable_t *) v)){
        array_insert_last(int, histFreeIdArray, varid);
     }
  }
  lsDestroy(varList, NULL);

  temp1 = mdd_smooth(symInfo->manager, initSet, histFreeIdArray);
  mdd_free(initSet);
   
  initSet = mdd_smooth(symInfo->manager, temp1, eventIdArray);
  mdd_free(temp1);

  array_free(histFreeIdArray);
  array_free(eventIdArray);
  }

  Main_MochaPrint( "Intitial Region Computed...\n");
  Main_MochaPrint( "NOTE: Regions are represented only in terms of history dependent variables...\n");

  if(verbose) 
       Sym_MddPrintCubes(symInfo, initSet , 0);
  
  {
  Sym_Region_t *init = Sym_RegionAlloc(module, initSet);
  unsigned long num = Sym_RegionManagerReadCounter(regionManager);
  long length = 1;
  char *id;
  char *temp;

  if(num > 9)
    length = (long) floor(log10(num)) + 1;
  id = ALLOC(char, length + 1);
  sprintf(id, "%d", num);
  Sym_RegionManagerIncrementCounter(regionManager);
  initRegionName = util_strcat3(moduleName, ".r", id);
  FREE(id);

  entryPtr = Tcl_CreateHashEntry(regionTable, initRegionName, &flag);
  Tcl_SetHashValue(entryPtr, (ClientData) init);
  Tcl_AppendElement(interp, initRegionName);
  FREE(initRegionName);
  }
  
  return(TCL_OK);
usage:
  Main_MochaErrorPrint( "Usage sym_init [-v] [-h] <module>\n");
  return (TCL_ERROR);
}


/**Function********************************************************************

  Synopsis           [driver for building transition relation]

  Description        [Build transition relation for the given module. The transtion
                      relation is conjunctively decomposed - one conjunct per
		      atom and stored as part of the symInfo for the module]

  SideEffects        [1. transition relation is stored as part of modules's SymInfo
                      2. If there is no symInfo, one is creared by 
		        executing the sym_static_order command.]

  SeeAlso            [CommandComputeInitSet CommandComputeReachSet]

  CommandName        [sym_trans] 	   

  CommandSynopsis    [compute transition realtion MDD for the module]

  CommandArguments   [\[-v\]  \[-h\] &lt;module_name&gt;]  

  CommandDescription [The command build the transtion relation MDD for a module.

  Command Options:<p>

  <dl>

  <dt> -v
  <dd> verbose mode <p>

  <dt> -h
  <dd> print usage <p>

  </dl>]
******************************************************************************/
static int
CommandBuildTrans(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager((Main_Manager_t *) clientData);
  Mdl_Module_t *module;
  Sym_Info_t *symInfo;
  int i,c;
  boolean verbose = FALSE;
  boolean noImageInfo = FALSE;

  util_getopt_reset();

  while((c = util_getopt(argc, argv, "nvh")) != EOF) {
    switch(c) {
        case 'n':
	  noImageInfo = TRUE;
	  break;
        case 'v' :
          verbose = TRUE;
          break;
        case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }

  if (argc - util_optind == 0) {
    goto usage;
  }

  
  if((module = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }

  symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);
  if (!symInfo) {
    char cmd[512];
    Main_MochaPrint("Ordering variables using sym_static_order\n");
    sprintf(cmd, "sym_static_order %s", Mdl_ModuleReadName(module));
    Tcl_Eval(interp, cmd);
    symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);
    if(!symInfo){
      Main_MochaErrorPrint("internal error: Cannot obtain symInfo for %s\n",
                           Mdl_ModuleReadName(module));
      return TCL_ERROR;
    }
  }

  if(array_n(symInfo->transRelation->conjuncts) > 0){
    Main_MochaErrorPrint("Transition relation for %s already exists\n",
                         Mdl_ModuleReadName(module));
    return TCL_ERROR;
  }
    
  Sym_ModuleBuildTransitionRelation(symInfo, module);

  Main_MochaPrint( "Transition relation computed : %d conjuncts\n",
                   array_n(symInfo->transRelation->conjuncts));

  if(verbose){
    for ( i = 0; i < array_n(symInfo->transRelation->conjuncts); i++){
      Main_MochaPrint( "Printing conjunct :  %d..\n", i);
      Sym_MddPrintCubes(symInfo,  
			(mdd_t *) array_fetch( mdd_t *,symInfo->transRelation->conjuncts, i), 0);
     }
  }

  if(!noImageInfo){
    char cmd[512];
    /* it seems there is a effect if i call bdd ordering at this point */
    sprintf(cmd, "sym_dynamic_var_ordering");
    Tcl_Eval(interp, cmd);

     bdd_dynamic_reordering(symInfo->manager,
                            DynamicVarOrderingMethod,
                            BDD_REORDER_VERBOSITY_DEFAULT);

     /* bdd_dynamic_reordering(symInfo->manager,
                               BDD_REORDER_SIFT,
                               BDD_REORDER_VERBOSITY_DEFAULT);
                               */

    
    symInfo->imgInfo = Img_ImageInfoInitialize( 0,
                                              symInfo->manager,
                                              symInfo->unprimedIdArray, /* domainVars*/
                                              symInfo->primedIdArray,  /* rangeVars*/
                                              symInfo->quantifyIdArray,
                                              Img_Iwls95_c,
                                              symInfo->transRelation->conjuncts,
                                              Img_Both_c
                                              );
    
    Main_MochaPrint( "Done initializing image info...\n");
  }

  return TCL_OK;

usage:
   Main_MochaErrorPrint( "Usage: sym_trans [-v] [-h] <module>\n");
   return TCL_ERROR;
}


/**Function********************************************************************

  Synopsis           [compute the post set of a given set]

  Description        [driver for one step image computation]

  SideEffects        [the image is stored in the region manager's
                      region table]

  SeeAlso            [CommandBuildTrans]

  CommandName        [sym_post] 	   

  CommandSynopsis    [compute the image of a region with respect to the
                      module's transition relation ]

  CommandArguments [\[-v\] \[-h\] &lt;region_name&gt; ]

  CommandDescription [The command computes the image of a given region with
  respect to the module's transition relation. The region is specified by
  a name assigned to it previously by mocha. 

  Command Options:<p>

  <dl>

  <dt> -v
  <dd> verbose mode <p>

  <dt> -h
  <dd> print usage <p>

  </dl>]
******************************************************************************/
static int
CommandComputePostSet(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *regionTable = Sym_RegionManagerReadRegionTable(regionManager);
  char *regionName;
  char *postRegionName;
  Sym_Region_t * region;
  Sym_Info_t *symInfo;
  int flag, c;
  char *moduleName;
  Tcl_HashEntry *entryPtr;

  boolean verbose = FALSE;
  util_getopt_reset();

  while((c = util_getopt(argc, argv, "vh")) != EOF) {
    switch(c) {
        case 'v' :
          verbose = TRUE;
          break;
        case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }

  if (argc - util_optind == 0) {
    goto usage;
  }
  
  regionName = argv[util_optind];
  
  if((entryPtr = Tcl_FindHashEntry(regionTable, regionName)) ==
     NIL(Tcl_HashEntry)) {
    Main_MochaErrorPrint( "Region %s not found", regionName);
    return TCL_ERROR;
  }
    
  region = (Sym_Region_t *) Tcl_GetHashValue(entryPtr);
  moduleName = Mdl_ModuleReadName(region->module);
  symInfo = Sym_RegionManagerReadSymInfo(regionManager,
                                                        region->module);
  if(!symInfo){
    Main_MochaErrorPrint( "ERROR: symInfo is null!!\n");
    return(TCL_ERROR);
  }


  {
    /* compute the image */
    mdd_t *lowerBound = region->mdd;
    mdd_t *upperBound = mdd_dup(region->mdd);
    mdd_t *toCareSet = mdd_not(region->mdd);
    mdd_t *image = 
        Img_ImageInfoComputeFwdWithDomainVars(
          symInfo->imgInfo,
          lowerBound,
          upperBound,
          toCareSet);
    
  
    {
      /* quantify out history free and event variables */
      array_t *histFreeIdArray    = array_alloc(int, 0);
      array_t *eventIdArray    = array_alloc(int, 0);
      lsList varList;
      lsGen  gen;
      lsGeneric v;
      mdd_t *temp1;
      int varid;
      
      varList = Mdl_ModuleObtainVariableList(region->module);
      lsForEachItem(varList, gen, v) {
        if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, (Var_Variable_t *)v, &varid )){
          Main_MochaErrorPrint( "Error: SymSearchModule, cant find mdd id for var\n");
        }
        
        if(Var_VariableIsEvent((Var_Variable_t *) v)){
          array_insert_last(int, eventIdArray, varid);
        }
        else if(Var_VariableIsHistoryFree((Var_Variable_t *) v)){
          array_insert_last(int, histFreeIdArray, varid);
        }
      }
      lsDestroy(varList, NULL);
      
      temp1 = mdd_smooth(symInfo->manager, image, histFreeIdArray);
      mdd_free(image);
      
      image = mdd_smooth(symInfo->manager, temp1, eventIdArray);
      mdd_free(temp1);
      
      array_free(histFreeIdArray);
      array_free(eventIdArray);
    }
    
    {
      Sym_Region_t *post = Sym_RegionAlloc(region->module, image);
      unsigned long num = Sym_RegionManagerReadCounter(regionManager);
      long length = 1;
      char *id;
      char *temp;
      
      Main_MochaPrint( "Image computed\n");
      Main_MochaPrint( "NOTE: Regions are represented only in terms of history dependent variables...\n");
      if( verbose)
        Sym_MddPrintCubes(symInfo, image , 0);
      
      if(num > 9)
        length = (long) floor(log10(num)) + 1;
      id = ALLOC(char, length + 1);
      sprintf(id, "%d", num);
      Sym_RegionManagerIncrementCounter(regionManager);
      postRegionName = util_strcat3(moduleName, ".r", id);
      FREE(id);
      entryPtr = Tcl_CreateHashEntry(regionTable, postRegionName, &flag);
      Tcl_SetHashValue(entryPtr, (ClientData) post);
      Tcl_AppendElement(interp, postRegionName);
      
      mdd_free(upperBound);
      mdd_free(toCareSet);
      
      FREE(postRegionName); 
    }
  }
  return TCL_OK;

usage:
  Main_MochaErrorPrint( "Usage sym_post [-v] [-h] <region>\n");
  return (TCL_ERROR);
  
}



/**Function********************************************************************

  Synopsis           [compute the union of two regions]

  Description        [typical usage is to union an image with an existing region]

  SideEffects        [The result of the union is added to the region manager's
                       region table]

  SeeAlso            [CommandComputePostSet]

  CommandName        [sym_union] 	   

  CommandSynopsis    [Computes the union of two regions] 

  CommandArguments   [\[-v\]  \[-h\]  &lt;region_name1&gt;  &lt;region_name2&gt;]  

  CommandDescription [The command computes the union of two regions

  Command Options:<p>
  <dl>

  <dt> -v
  <dd> verbose mode <p>

  <dt> -h
  <dd> print usage <p>

  </dl>]
******************************************************************************/
static int
CommandComputeUnion(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *regionTable = Sym_RegionManagerReadRegionTable(regionManager);
  char *regionName1 = argv[1];
  char *regionName2 = argv[2];
  char *unionRegionName;
  Sym_Region_t * region1;
  Sym_Region_t * region2;
  int flag, c;
  char *moduleName;
  Tcl_HashEntry *entryPtr;
  

  boolean verbose = FALSE;
  util_getopt_reset();

  while((c = util_getopt(argc, argv, "vh")) != EOF) {
    switch(c) {
        case 'v' :
          verbose = TRUE;
          break;
        case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }

  if (argc - util_optind == 0) {
    goto usage;
  }
  regionName1 = argv[util_optind++];
  

  if (argc - util_optind == 0) {
    goto usage;
  }
  regionName2 = argv[util_optind];


  if((entryPtr = Tcl_FindHashEntry(regionTable, regionName1)) ==
     NIL(Tcl_HashEntry)) {
    Main_MochaErrorPrint( "Region %s not found", regionName1);
    return TCL_ERROR;
  }
    
  region1 = (Sym_Region_t *) Tcl_GetHashValue(entryPtr);
  moduleName = Mdl_ModuleReadName(region1->module);


  if((entryPtr = Tcl_FindHashEntry(regionTable, regionName2)) ==
     NIL(Tcl_HashEntry)) {
    Main_MochaErrorPrint( "Region %s not found", regionName2);
    return TCL_ERROR;
  }
    
  region2 = (Sym_Region_t *) Tcl_GetHashValue(entryPtr);
  if(strcmp (moduleName,  Mdl_ModuleReadName(region2->module)) != 0){
    Main_MochaPrint( "Cant union regions of two different modules\n");
    return(-1);
  }
  
  
  {
    /* compute the image */
  mdd_t *mdd1 = region1->mdd;
  mdd_t *mdd2 = region2->mdd;
  mdd_t *result = mdd_or(mdd1, mdd2, 1, 1);
  Sym_Region_t *symUnion = Sym_RegionAlloc(region1->module, result);
  unsigned long num = Sym_RegionManagerReadCounter(regionManager);
  long length = 1;
  char *id;
  char *temp;
  Sym_Info_t * symInfo = Sym_RegionManagerReadSymInfo(regionManager,
                                                        region1->module);
  if(!symInfo){
    Main_MochaErrorPrint("symInfo is NULL, cant proceed\n");
    return(TCL_ERROR);
  }
  Main_MochaPrint( "Union computed\n");
  if(verbose)
     Sym_MddPrintCubes(symInfo, result , 0);
  
  if(num > 9)
    length = (long) floor(log10(num)) + 1;
  id = ALLOC(char, length + 1);
  sprintf(id, "%d", num);
  Sym_RegionManagerIncrementCounter(regionManager);
  unionRegionName = util_strcat3(moduleName, ".r", id);
  FREE(id);
  entryPtr = Tcl_CreateHashEntry(regionTable, unionRegionName, &flag);
  Tcl_SetHashValue(entryPtr, (ClientData) symUnion);
  Tcl_AppendElement(interp, unionRegionName);

  FREE(unionRegionName); 
  }
  return TCL_OK;

  usage:
    Main_MochaErrorPrint( "Usage: sym_union <region1> <region2>\n");
    return TCL_ERROR;
}


/**Function********************************************************************

  Synopsis           [Print the given region]

  Description        [Print the given region]

  SideEffects        [none]

  SeeAlso            [CommandComputePostSet]

  CommandName        [sym_print] 	   

  CommandSynopsis    [print the given region]

  CommandArguments [ \[-h\] &lt;region_name&gt; ]

  CommandDescription [The command prints the given region. The region is specified by
  a name assigned to it previously by mocha. 

  Command Options:<p>

  <dl>
  <dt> -h
  <dd> print usage <p>

  </dl>]
******************************************************************************/
static int
CommandPrintRegion(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *regionTable = Sym_RegionManagerReadRegionTable(regionManager);
  char *regionName;
  Sym_Region_t * region;
  int flag, c;
  char *moduleName;
  Tcl_HashEntry *entryPtr;

  util_getopt_reset();

  while((c = util_getopt(argc, argv, "h")) != EOF) {
    switch(c) {
        case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }

  if (argc - util_optind == 0) {
    goto usage;
  }
  regionName = argv[util_optind];
  
  if((entryPtr = Tcl_FindHashEntry(regionTable, regionName)) ==
     NIL(Tcl_HashEntry)) {
    Main_MochaErrorPrint( "Region %s not found", regionName);
    return TCL_ERROR;
  }
    
  region = (Sym_Region_t *) Tcl_GetHashValue(entryPtr);

  {
  Sym_Info_t * symInfo = Sym_RegionManagerReadSymInfo(regionManager,
                                                        region->module);
  Sym_MddPrintCubes(symInfo, region->mdd , 0);
  return TCL_OK;
  }

usage:
  Main_MochaErrorPrint( "Usage sym_print [-h] <region>\n");
  return (TCL_ERROR);
  
}

/**Function********************************************************************

  Synopsis    [Implements the dynamic_var_ordering command.]

  SideEffects []

  CommandName [sym_dynamic_var_ordering]

  CommandSynopsis [control the application of dynamic variable ordering]

  CommandArguments [ \[-d\] \[-e &lt;method&gt;\] \[-f &lt;method&gt;\]
  \[-h\]]

  CommandDescription [Control the application of dynamic variable ordering to the
  flattened network. Dynamic ordering is a technique to reorder the MDD
  variables to reduce the size of the existing MDDs.  When no options are
  specified, the current status of dynamic ordering is displayed.  At most one
  of the options -e, -f, and -d should be specified.  The commands
  <tt>flatten_hierarchy</tt> and <tt>static_order</tt> must be invoked before
  this command.<p>

  Dynamic ordering may be time consuming, but can often reduce the size of the
  MDDs dramatically.  The good points to invoke dynamic ordering explicitly
  (using the -f option) are after the commands <tt>build_partition_mdds</tt>
  and <tt>print_img_info</tt>.  If dynamic ordering finds a good ordering,
  then you may wish to save this ordering (using <tt>write_order</tt>) and
  reuse it (using <tt>static_order -s</tt>) in the future. A common sequence
  used to get a good ordering is the following:<p>

  <pre>
  init_verify
  print_img_info
  dynamic_var_ordering -f sift
  write_order <file>
  flatten_hierarchy
  static_order -s input_and_latch -f <file>
  build_partition_mdds
  print_img_info
  dynamic_var_ordering -f sift
  </pre>  
  
  Command options:<p>  

  <dl>

  <dt> -d
  <dd> Disable dynamic ordering from triggering automatically.<p>
  
  <dt> -e &lt;method&gt;
  <dd> Enable dynamic ordering to trigger automatically whenever a certain
  threshold on the overall MDD size is reached. "Method" must be one of the following:<p>

  <b>window:</b> Permutes the variables within windows of three adjacent
  variables so as to minimize the overall MDD size.  This process is repeated
  until no more reduction in size occurs.<p>

  <b>sift:</b> Moves each variable throughout the order to find an optimal
  position for that variable (assuming all other variables are fixed).  This
  generally achieves greater size reductions than the window method, but is
  slower.<p>
  
  <b>The following methods are only available if VIS has been linked with the
  Bdd package from the University of Colorado (cuBdd).</b><p>

  <b>random:</b> Pairs of variables are randomly chosen, and swapped in the
  order. The swap is performed by a series of swaps of adjacent variables. The
  best order among those obtained by the series of swaps is retained. The
  number of pairs chosen for swapping equals the number of variables in the
  diagram.<p>

  <b>random_pivot:</b> Same as <b>random<\b>, but the two variables are chosen
  so that the first is above the variable with the largest number of nodes, and
  the second is below that variable.  In case there are several variables tied
  for the maximum number of nodes, the one closest to the root is used.<p>

  <b>sift_converge:</b> The <b>sift<\b> method is iterated until no further
  improvement is obtained.<p>

  <b>symmetry_sift:</b> This method is an implementation of symmetric
  sifting. It is similar to sifting, with one addition: Variables that become
  adjacent during sifting are tested for symmetry. If they are symmetric, they
  are linked in a group. Sifting then continues with a group being moved,
  instead of a single variable.<p>

  <b>symmetry_sift_converge:</b> The <b>symmetry_sift<\b> method is iterated
  until no further improvement is obtained.<p>

  <b>window{2,3,4}:</b> Permutes the variables within windows of n adjacent
  variables, where "n" can be either 2, 3 or 4, so as to minimize the overall
  MDD size.<p>

  <b>window{2,3,4}_converge:</b> The <b>window{2,3,4}<\b> method is iterated
  until no further improvement is obtained.<p>

  <b>group_sift:</b> This method is similar to <b>symmetry_sift</b>,
  but uses more general criteria to create groups.

  <b>group_sift_converge:</b> The <b>group_sift<\b> method is iterated until no
  further improvement is obtained.<p>

  <b>annealing:</b> This method is an implementation of simulated annealing for
  variable ordering. This method is potentially very slow.<p>

  <b>genetic:</b> This method is an implementation of a genetic algorithm for
  variable ordering. This method is potentially very slow.<p>

  <dt> -f &lt;method&gt;
  <dd> Force dynamic ordering to be invoked immediately.  The values for
  method are the same as in option -e.<p>

  <dt> -h
  <dd> Print the command usage.

  </dl>
  ]
  
******************************************************************************/
static int
CommandDynamicVarOrdering(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  mdd_manager *mddManager = (mdd_manager *)
      Main_ManagerReadMddManager((Main_Manager_t *) clientData);
  int                 c;
  bdd_reorder_type_t  currentMethod;
  bdd_reorder_type_t  dynOrderingMethod = BDD_REORDER_NONE; /* for lint */
  boolean             disableFlag       = FALSE;
  boolean             enableFlag        = FALSE;
  boolean             forceFlag         = FALSE;


  if(!mddManager){
    mddManager = mdd_init_empty();
    Main_ManagerSetMddManager((Main_Manager_t *) clientData,
                              (Main_MddManagerGeneric) mddManager);
  }

  /*
   * Parse the command line.
   */
  util_getopt_reset();
  while ((c = util_getopt(argc, argv, "df:e:h")) != EOF) {
    switch (c) {
      case 'h':
        goto usage;
        break;
      case 'f':
        forceFlag = TRUE;
        dynOrderingMethod = StringConvertToDynOrderType(util_optarg);
        if (dynOrderingMethod == BDD_REORDER_NONE) {
          Main_MochaErrorPrint("unknown method: %s\n", util_optarg);
          goto usage;
        }
        break;
      case 'e':
        enableFlag = TRUE;
        dynOrderingMethod = StringConvertToDynOrderType(util_optarg);
        if (dynOrderingMethod == BDD_REORDER_NONE) {
          Main_MochaErrorPrint("unknown method: %s\n", util_optarg);
          goto usage;
        }
        break;
      case 'd':
        disableFlag = TRUE;
        break;
      default:
        goto usage;
    }
  }

  /* At most one option is allowed. */
  if ((disableFlag && enableFlag) || (disableFlag && forceFlag)
      || (enableFlag && forceFlag)) {
    Main_MochaErrorPrint("Only one of -d, -f, -e is allowed.\n");
    return 1;
  }

  /*
   * Get the current method for reading and to save in case temporarily
   * overwritten.
   */
  currentMethod = DynamicVarOrderingMethod;

  /* If no options are given, then just display current status. */
  if (!(disableFlag || enableFlag || forceFlag)) {
    if (currentMethod == BDD_REORDER_NONE) {
      Main_MochaPrint("Dynamic variable ordering is disabled.\n");
    }
    else {
      Main_MochaPrint("Dynamic variable ordering is enabled ");
      Main_MochaPrint("with method %s.\n",
                     DynOrderTypeConvertToString(currentMethod));
    }
  }

  if (disableFlag) {
    if (currentMethod == BDD_REORDER_NONE) {
      Main_MochaPrint("Dynamic variable ordering is already disabled.\n");
    }
    else {
      Main_MochaPrint("Dynamic variable ordering is disabled.\n");
      bdd_dynamic_reordering(mddManager, BDD_REORDER_NONE, BDD_REORDER_VERBOSITY_DEFAULT);
      DynamicVarOrderingMethod = BDD_REORDER_NONE;
    }
  }

  /*
   * Set the dynamic ordering method of the network.  
   */
  if (enableFlag) {
    bdd_dynamic_reordering(mddManager, dynOrderingMethod, BDD_REORDER_VERBOSITY_DEFAULT);
    DynamicVarOrderingMethod = dynOrderingMethod;
    Main_MochaPrint("Dynamic variable ordering is enabled ");
    Main_MochaPrint("with method %s.\n",
                   DynOrderTypeConvertToString(dynOrderingMethod));
  }

  /*
   * Force a reordering.  Note that the mddManager has to have the method set
   * before calling bdd_reorder.  The value
   * of the ordering method is restored afterwards.
   */
  if (forceFlag) {
    Main_MochaPrint("Dynamic variable ordering forced ");
    Main_MochaPrint("with method %s....\n",
                   DynOrderTypeConvertToString(dynOrderingMethod));
    bdd_dynamic_reordering(mddManager, dynOrderingMethod, BDD_REORDER_VERBOSITY_DEFAULT);
    bdd_reorder(mddManager);
  }

  return 0;  /* Everything okay */

usage:
  Main_MochaErrorPrint("usage: dynamic_var_ordering [-d] [-e method] [-f method] [-h]\n");
  Main_MochaErrorPrint("   -d        disable dynamic ordering\n");
  Main_MochaErrorPrint("   -e method enable dynamic ordering with method (window, sift)\n");
  Main_MochaErrorPrint("   -f method force dynamic ordering with method (window, sift)\n");
  Main_MochaErrorPrint("   -h        print the command usage\n");

  return 1;
}


/**Function********************************************************************

  Synopsis           [Write the current ordering of the module into a file]

  Description        [Write the current ordering of MDD variables into a file]
                      
  SideEffects        [none]

  SeeAlso            [CommandReadOrder]

  CommandName        [sym_write_order] 	   

  CommandSynopsis    [write the current ordering into a file]  

  CommandArguments   [\[-h\] -f &lt;file_name&gt; &lt;module_name;&gt; ]  

  CommandDescription [This command writes the current ordering of MDD variables
                      into a file, specified by the given file name]  

  Command Options:<p>

  <dl>
  <dt> -h
  <dd> print usage <p>

  </dl>]
		     

******************************************************************************/
static int
CommandWriteOrder(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager((Main_Manager_t *) clientData); 
  Mdl_Module_t *module;
  Sym_Info_t *symInfo;
  int c;
  char *fileName;
  boolean toFile = FALSE;
  FILE *fp;
  
  util_getopt_reset();
  while((c = util_getopt(argc, argv, "hf:")) != EOF) {
    switch (c) {
        case 'f' :
          fileName = util_optarg;
          toFile = TRUE;
          break;
        case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }

  if (argc - util_optind != 1)
    goto usage;

  if((module = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }

  symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);
  if(!symInfo){
    Main_MochaErrorPrint( "Please build transition relation first (use sym_trans)\n");
    return(TCL_ERROR);
  }

  if (toFile) {
    fp = fopen(fileName, "w");
    if(!fp){
      Main_MochaErrorPrint("sym_write_order: Cant open file for writing variable order..skipping \n");
    }
    else{
    SymWriteOrder(fp, symInfo);
    fclose(fp);
    }
    return TCL_OK;
  }
  
  usage:
  Main_MochaErrorPrint( "Usage: sym_write_order [-h] [-f] <file> <module>\n");
  return (TCL_ERROR);
}


/**Function********************************************************************

  Synopsis           [Order the MDD variables of a module as specified by a file]

  Description        [Read an ordering of MDD variables into a file, and order
                      the MDD variables of a module accordingly]
                      
  SideEffects        [none]

  SeeAlso            [CommandWriteOrder]

  CommandName        [sym_static_order] 	   

  CommandSynopsis    [write the current ordering into a file]  

  CommandArguments   [\[-h\] -f &lt;file_name&gt; &lt;module_name;&gt; ]  

  CommandDescription [This command reads the current ordering of MDD variables
                      from a file, specified by the given file name]  

  Command Options:<p>

  <dl>
  <dt> -h
  <dd> print usage <p>

  </dl>]
		     

******************************************************************************/
static int
CommandReadOrder(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager((Main_Manager_t *) clientData);
  mdd_manager *manager = (mdd_manager *)
      Main_ManagerReadMddManager((Main_Manager_t *) clientData);
  Mdl_Module_t *module;
  Sym_Info_t *symInfo;
  int c, i;
  char *fileName;
  boolean fromFile = FALSE;
  FILE *fp;
  array_t *totalVarArray;
  VarOrder_t *varOrder;
  
  if(!manager){
    manager = mdd_init_empty();
    Main_ManagerSetMddManager((Main_Manager_t *) clientData,
                              (Main_MddManagerGeneric) manager);
  }
  
  
  util_getopt_reset();
  while((c = util_getopt(argc, argv, "hf:")) != EOF) {
    switch (c) {
        case 'f' :
          fileName = util_optarg;
          fromFile = TRUE;
          break;
        case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }

  if (argc - util_optind != 1)
    goto usage;

  if((module = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }


  if (fromFile) {
    fp = fopen(fileName, "r");
    if ((totalVarArray = SymReadOrder(fp, module)) == NIL(array_t)) {
      Main_MochaErrorPrint("Error in reading the input file.\n");
      fclose(fp);
      return TCL_ERROR;
    }
    fclose(fp);
    ModuleTotalOrderVariables(module, totalVarArray);
  }
  else {
    totalVarArray = array_alloc(VarOrder_t *, 0);
    ModuleTotalOrderVariables(module, totalVarArray);
  }


  symInfo = Sym_SymInfoAlloc(interp, module, manager);
  
  /* This call frees the old sym info. */
  
  Sym_RegionManagerAddModuleToSymInfo(regionManager, module, symInfo);
  
  ModuleAssignMddIdsToVariables(module, symInfo, totalVarArray);
  arrayForEachItem(VarOrder_t *, totalVarArray, i, varOrder) {
    FREE(varOrder);
  }
  array_free(totalVarArray);
  
  return TCL_OK;
  
  usage:
  Main_MochaErrorPrint( "Usage: sym_static_order [-h] [-f] <file> <module>\n");
  return (TCL_ERROR);
}

/**Function********************************************************************

  Synopsis    [Gets the levels of the BDD variables corresponding to the MDD
  variable of a node.]

  Description [Returns an array (of int's) of the levels of the BDD variables
  corresponding to the MDD variable of a node.  The level of a BDD variable is
  it place in the current variable ordering of the BDD manager.  The returned
  array is sorted in ascending order.  It is the responsibility of the caller
  to free this array.]

  SideEffects []

******************************************************************************/
static array_t *
VarBuildBddIdArray(
  Sym_Info_t *symInfo,
  Var_Variable_t *var,
  boolean primed)
{
  int i;
  mdd_manager *mddManager = symInfo->manager;
  array_t *mvarArray = mdd_ret_mvar_list(mddManager);
  int mddId;
  mvar_type mv;
  array_t *bddIdArray;
  
  if (primed)
    Sym_SymInfoLookupPrimedVariableId(symInfo, var, &mddId);
  else
    Sym_SymInfoLookupUnprimedVariableId(symInfo, var, &mddId);

  mv = array_fetch(mvar_type, mvarArray, mddId);
  bddIdArray = array_alloc(int, mv.encode_length);

  for (i = 0; i < mv.encode_length; i++) {
    int    bddId    = mdd_ret_bvar_id(&mv, i);
    bdd_t *varBdd   = bdd_get_variable((bdd_manager *) mddManager, bddId);
    int    varLevel = (int) bdd_top_var_level((bdd_manager *) mddManager, varBdd);

    bdd_free(varBdd);
    array_insert_last(int, bddIdArray, varLevel);
  }
  array_sort(bddIdArray, IntegersCompare);

  return (bddIdArray);
}

/**Function********************************************************************

  Synopsis    [Used to sort an array of nodes in ascending order of lowest BDD
  level.]

  SideEffects []

******************************************************************************/
static int
VarOrdersCompareBddIdArray(
  lsGeneric node1,
  lsGeneric node2)
{
  array_t *bddIdArray1 = ((VarOrder_t *) node1)->bddIdArray;
  array_t *bddIdArray2 = ((VarOrder_t *) node2)->bddIdArray;
  int      firstLevel1    = array_fetch(int, bddIdArray1, 0);
  int      firstLevel2    = array_fetch(int, bddIdArray2, 0);
  
  if (firstLevel1 < firstLevel2) {
    return -1;
  }
  else if (firstLevel1 == firstLevel2) {
    return 0;
  }
  else {
    return 1;
  }
}

/**Function********************************************************************

  Synopsis    [Used to sort an array of integers in ascending order.]

  SideEffects []

******************************************************************************/
static int
IntegersCompare(
  char ** obj1,
  char ** obj2)
{
  int int1 = *(int *) obj1;
  int int2 = *(int *) obj2;
  
  if (int1 < int2) {
    return -1;
  }
  else if (int1 == int2) {
    return 0;
  }
  else {
    return 1;
  }
}

/**Function********************************************************************

  Synopsis    [Converts a string to a dynamic ordering method type.]

  Description [Converts a string to a dynamic ordering method type. If string
  is not "sift" or "window", then returns BDD_REORDER_NONE.]

  SideEffects []

******************************************************************************/
static bdd_reorder_type_t
StringConvertToDynOrderType(
  char *string)
{
  if (strcmp("sift", string) == 0) { 
    return BDD_REORDER_SIFT;
  }
  else if (strcmp("window", string) == 0) { 
    return BDD_REORDER_WINDOW;
  }
  else if (strcmp("random", string) == 0) { 
    return BDD_REORDER_RANDOM;
  }
  else if (strcmp("random_pivot", string) == 0) { 
    return  BDD_REORDER_RANDOM_PIVOT;
  }
  else if (strcmp("sift_converge", string) == 0) { 
    return  BDD_REORDER_SIFT_CONVERGE;
  }
  else if (strcmp("symmetry_sift", string) == 0) { 
    return  BDD_REORDER_SYMM_SIFT;
  }
  else if (strcmp("symmetry_sift_converge", string) == 0) { 
    return  BDD_REORDER_SYMM_SIFT_CONV;
  }
  else if (strcmp("window2", string) == 0) { 
    return  BDD_REORDER_WINDOW2;
  }
  else if (strcmp("window3", string) == 0) { 
    return  BDD_REORDER_WINDOW3;
  }
  else if (strcmp("window4", string) == 0) { 
    return  BDD_REORDER_WINDOW4;
  }
  else if (strcmp("window2_converge", string) == 0) { 
    return  BDD_REORDER_WINDOW2_CONV;
  }
  else if (strcmp("window3_converge", string) == 0) { 
    return  BDD_REORDER_WINDOW3_CONV;
  }
  else if (strcmp("window4_converge", string) == 0) { 
    return  BDD_REORDER_WINDOW4_CONV;
  }
  else if (strcmp("group_sift", string) == 0) { 
    return  BDD_REORDER_GROUP_SIFT;
  }
  else if (strcmp("group_sift_converge", string) == 0) { 
    return  BDD_REORDER_GROUP_SIFT_CONV;
  }
  else if (strcmp("annealing", string) == 0) { 
    return  BDD_REORDER_ANNEALING;
  }
  else if (strcmp("genetic", string) == 0) { 
    return  BDD_REORDER_GENETIC;
  }
  else if (strcmp("exact", string) == 0) { 
    return  BDD_REORDER_EXACT;
  }
  else {
    return BDD_REORDER_NONE;
  }
}


/**Function********************************************************************

  Synopsis    [Converts a dynamic ordering method type to a string.]

  Description [Converts a dynamic ordering method type to a string.  This
  string must NOT be freed by the caller.]

  SideEffects []

******************************************************************************/
static char *
DynOrderTypeConvertToString(
  bdd_reorder_type_t method)
{
  if ((method == BDD_REORDER_SIFT)|| (method == BDD_REORDER_GROUP_SIFT)) {
    return "sift"; 
  }
  else if (method == BDD_REORDER_WINDOW) {
    return "window"; 
  }
  else if (method == BDD_REORDER_NONE) {
    return "none"; 
  }
  else if (method == BDD_REORDER_RANDOM) { 
    return "random";
  }
  else if (method == BDD_REORDER_RANDOM_PIVOT) { 
    return "random_pivot";
  }
  else if (method == BDD_REORDER_SIFT_CONVERGE) { 
    return "sift_converge";
  }
  else if (method == BDD_REORDER_SYMM_SIFT) { 
    return "symmetry_sift";
  }
  else if (method == BDD_REORDER_SYMM_SIFT_CONV) { 
    return "symmetry_sift_converge";
  }
  else if (method == BDD_REORDER_WINDOW2) { 
    return "window2";
  }
  else if (method == BDD_REORDER_WINDOW3) { 
    return "window3";
  }
  else if (method == BDD_REORDER_WINDOW4) { 
    return "window4";
  }
  else if (method == BDD_REORDER_WINDOW2_CONV) { 
    return "window2_converge";
  }
  else if (method == BDD_REORDER_WINDOW3_CONV) { 
    return "window3_converge";
  }
  else if (method == BDD_REORDER_WINDOW4_CONV) { 
    return "window4_converge";
  }
  else if (method == BDD_REORDER_GROUP_SIFT_CONV) { 
    return "group_sift_converge";
  }
  else if (method == BDD_REORDER_ANNEALING) { 
    return "annealing";
  }
  else if (method == BDD_REORDER_GENETIC) { 
    return "genetic";
  }
  else {
    fail("unrecognized method");
  }
}

/**Function********************************************************************

  Synopsis           [Convert an ordering of a subset of variables to a total 
                      ordering]

  Description        [Given a module and an array of variables (the order in which the 
                      variables are present in the array), pad remaining variables to the
		      end of the array]

  SideEffects        [Remaining variables are added to the end of the array]

  SeeAlso            [SymReadOrder]

******************************************************************************/
static array_t *
ModuleTotalOrderVariables(
  Mdl_Module_t *module,
  array_t *varOrderArray)
{
  lsList extVarList, atomList, ctrlVarList;
  lsGen atmGen, varGen;
  VarOrder_t *varOrder;
  int position = 0;
  int i;
  Var_Variable_t *var;
  Atm_Atom_t *atom;
  
  /* first create variables for all external variables of the module */
  extVarList = Mdl_ModuleGetExternalVariableList(module);
  lsForEachItem(extVarList, varGen, var){
    varOrder = ALLOC(VarOrder_t, 1);
    varOrder->var = var;
    varOrder->prime = 0;
    varOrder->bddIdArray = NIL(array_t);
    if ((i = VarOrderIsInArray(varOrderArray, varOrder)) == -1) {
      array_insert(VarOrder_t *, varOrderArray, position, varOrder);
      position++;
    }
    else {
      position = i+1;
      FREE(varOrder);
    }
    
    varOrder = ALLOC(VarOrder_t, 1);
    varOrder->var = var;
    varOrder->prime = 1;
    varOrder->bddIdArray = NIL(array_t);
    if ((i = VarOrderIsInArray(varOrderArray, varOrder)) == -1) {
      array_insert(VarOrder_t *, varOrderArray, position, varOrder);
      position++;
    }
    else {
      position = i+1;
      FREE(varOrder);
    }
  }
  lsDestroy(extVarList, NULL);
  
  
  /* Get a list of sorted atoms. */
  atomList = Mdl_ModuleObtainSortedAtomList(module);
  /* Iterate over the atoms. */
  lsForEachItem(atomList, atmGen, atom) {
    ctrlVarList = Atm_AtomReadControlVarList(atom);
    lsForEachItem(ctrlVarList, varGen, var){
      varOrder = ALLOC(VarOrder_t, 1);
      varOrder->var = var;
      varOrder->prime = 0;
      varOrder->bddIdArray = NIL(array_t);
      if ((i = VarOrderIsInArray(varOrderArray, varOrder)) == -1) {
        array_insert(VarOrder_t *, varOrderArray, position, varOrder);
        position++;
      }
      else {
        position = i+1;
        FREE(varOrder);
      }
            
      varOrder = ALLOC(VarOrder_t, 1);
      varOrder->var = var;
      varOrder->prime = 1;
      varOrder->bddIdArray = NIL(array_t);
      if ((i = VarOrderIsInArray(varOrderArray, varOrder)) == -1) {
        array_insert(VarOrder_t *, varOrderArray, position, varOrder);
        position++;
      }
      else {
        position = i+1;
        FREE(varOrder);
      }
    }
  }
  
  lsDestroy(atomList, NULL);
  return varOrderArray;
}


/**Function********************************************************************

  Synopsis           [check if a variable is in a varOrderArray]

  Description        [check if a variable is in a varOrderArray]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
static int
VarOrderIsInArray(
  array_t *varOrderArray,
  VarOrder_t *varOrder)
{
  int i;
  VarOrder_t *temp;
  
  arrayForEachItem(VarOrder_t *, varOrderArray, i, temp) {
    if (temp->var == varOrder->var && temp->prime == varOrder->prime)
      return i;
  }

  return -1;
}

/**Function********************************************************************

  Synopsis           [Create MDD variables according to the specified total ordering]

  Description        [Create MDD variables according to the specified total ordering]

  SideEffects        [MDD variables are created for each of the module variables,
                      as specified by the total ordering

  SeeAlso            []

******************************************************************************/
static void
ModuleAssignMddIdsToVariables(
  Mdl_Module_t *module,
  Sym_Info_t *symInfo,
  array_t *totalVarArray)
{
  array_t *mvarValues;
  array_t *mvarNames;
  array_t *mvarArray;
  mdd_manager *mddManager = symInfo->manager;
  int i, mddId;
  long unprimedMddId, primedMddId;
  VarOrder_t *varOrder;
  Var_Variable_t *var;
  Var_Type_t *varType;
  Var_DataType dataType;
  st_generator *stgen;
  
  /* first create variables for all external variables of the module */

  arrayForEachItem(VarOrder_t *, totalVarArray, i, varOrder){
    mvarValues = array_alloc(int, 0);
    mvarNames = array_alloc(char *, 0);
    
    var = varOrder->var;
    varType = Var_VariableReadType (var);
    dataType = Var_VarTypeReadDataType(varType);
    array_insert_last(char *, mvarNames, Var_VariableReadName(var));

    switch (dataType){
        case Var_Boolean_c:
        case Var_Event_c:
          array_insert_last(int, mvarValues, 2);
          break;
          
        case Var_Enumerated_c:
        case Var_Range_c:
        case Var_Timer_c:
          array_insert_last(int, mvarValues, Var_VarTypeReadDomainSize(varType));
          break;
          
        default:
          Main_MochaErrorPrint(
            "Cannot create MDD variable for nat/int/unkonwn data types %s\n",
            Var_VariableReadName(var));
          exit(1);
    }

    mvarArray = mdd_ret_mvar_list(mddManager);
    mddId = array_n(mvarArray);
  
    if (varOrder->prime == 0) {
      st_insert(symInfo->varToUnprimedId, (char *) var,
                (char*)(long) mddId);
      array_insert_last(int, symInfo->unprimedIdArray, mddId);
      st_insert(symInfo->idToVar, (char *)(long) mddId, (char *) var);
    }
    else {
      st_insert(symInfo->varToPrimedId, (char *)var,
                (char *) (long) mddId);
      array_insert_last(int, symInfo->primedIdArray, mddId);
      st_insert(symInfo->idToVar, (char *)(long) mddId, (char *)var);
    }

    mdd_create_variables(mddManager, mvarValues, mvarNames, NIL(array_t));
    array_free(mvarValues);
    array_free(mvarNames);
  }

  st_foreach_item(symInfo->varToUnprimedId, stgen, (char **) &var, (char **)
                  &unprimedMddId) {
    st_lookup(symInfo->varToPrimedId, (char *) var, (char **) &primedMddId);
    
    if (unprimedMddId == primedMddId + 1)
      MddGroupVariables(mddManager, primedMddId, 2);
    else if (primedMddId == unprimedMddId + 1)
      MddGroupVariables(mddManager, unprimedMddId, 2);  
  }
}








