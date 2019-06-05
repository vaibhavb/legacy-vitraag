/**CFile***********************************************************************

  FileName    [rtm.c]

  PackageName [rtm]

  Synopsis    []

  Description []

  SeeAlso     []

  Author      [Serdar Tasiran]

  Copyright   [Copyright (c) 1997 The Regents of the Univ. of California.
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

#include "rtmInt.h" 

bdd_reorder_type_t  RtmDynamicVarOrderingMethod = BDD_REORDER_NONE; 


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

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
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int CommandReadOrder(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandComputeReachSet(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandComputeInitSet(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int CommandBuildTrans(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static bdd_reorder_type_t RtmStringConvertToDynOrderType(char *string);
static char * RtmDynOrderTypeConvertToString(bdd_reorder_type_t method);
static int CommandRtmDynamicVarOrdering(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static array_t * ModuleTotalOrderVariables(Mdl_Module_t *module, array_t *varOrderArray);
static int VarOrderIsInArray(array_t *varOrderArray, VarOrder_t *varOrder);
static void ModuleAssignMddIdsToVariables(Mdl_Module_t *module, Sym_Info_t *symInfo, array_t *totalVarArray);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Initializes the rtm package]

  Description        [Registers the rtm package commands with tcl
                      Allocates memory for RtmRegionManager and deposit it in
                      the main manager.]

  SideEffects        [None]

  SeeAlso            [Rtm_End, Rtm_Reinit]

******************************************************************************/
int
Rtm_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Tcl_CreateCommand(interp, "rtm_trans", CommandBuildTrans, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "rtm_init", CommandComputeInitSet, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "rtm_search", CommandComputeReachSet, (ClientData) manager,
                    (Tcl_CmdDeleteProc *)NULL);

  Tcl_CreateCommand(interp, "rtm_dynamic_var_ordering", CommandRtmDynamicVarOrdering, (ClientData)
                    manager, (Tcl_CmdDeleteProc *)NULL);

  Tcl_CreateCommand(interp, "rtm_static_order", CommandReadOrder, (ClientData)
                    manager, (Tcl_CmdDeleteProc *)NULL);
  
  Main_ManagerSetRtmRegionManager(manager, (Main_RtmRegionManagerGeneric) Rtm_RegionManagerAlloc());
  
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Re-initializes the rtm package]

  Description        [This function is called when the user types a re-init command. 
                      Re-initialization is not yet supported by Rtm.
		              Currently, this function does nothing]]

  SideEffects        [None]

  SeeAlso            [Rtm_Init, Rtm_End]

******************************************************************************/
int
Rtm_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Closes the rtm package]

  Description        [Currently this function does nothing. During a subsequent
                      cleanup this should be made to free all memory allocated
                      by the Rtm package.]

  SideEffects        [None]

  SeeAlso            []

******************************************************************************/
int
Rtm_End(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [do invariant checking on a timed module]

  Description        [Do invariant checking. This is the entry point
                      for the rtm invariant engine to be called from
                      the inv package]

  SideEffects        [none]

  SeeAlso            [Rtm_ModuleBuildTransitionRelation]


******************************************************************************/
void
Rtm_ModulePerformBFS(
  Tcl_Interp *interp,
  Main_Manager_t *mainManager,
  Mdl_Module_t *module,
  array_t *invNameArray,
  array_t *typedExprArray
  )
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager(mainManager);
  Rtm_RegionManager_t *rtmRegionManager = (Rtm_RegionManager_t *)
      Main_ManagerReadRtmRegionManager(mainManager);
  Tcl_HashTable *rtmRegionTable = Rtm_RegionManagerReadRegionTable(rtmRegionManager);
  Rtm_Info_t *rtmInfo;
  mdd_t *reachSet;
  int flag,c;
  Tcl_HashEntry *entryPtr;
  boolean verbose = FALSE;
  boolean checkInvariants = FALSE;
  int maxNumSteps = -1;

  rtmInfo = Rtm_RegionManagerObtainRtmInfoFromModule(rtmRegionManager, module);
  
  if(!rtmInfo){
    char cmd[512];
    Main_MochaPrint( "No rtm_info.. building it(using rtm_trans)\n");
    sprintf(cmd, "rtm_trans %s", Mdl_ModuleReadName(module));
    Tcl_Eval(interp, cmd);
    rtmInfo =  Rtm_RegionManagerObtainRtmInfoFromModule(rtmRegionManager, module); 
    if(!rtmInfo){
      Main_MochaErrorPrint( "Command: %s unsuccessful\n", cmd);
      return;
    }
  }

  if(array_n(invNameArray) == 0){
    Main_MochaErrorPrint("Error: Array of invariant names has 0 elements\n");
    return;
  }

  if(array_n(invNameArray) != array_n(typedExprArray)){
    Main_MochaErrorPrint("Error: lengths of invariant name array  and expression array are not equal\n");
    return;
  }

  
  reachSet = RtmModuleSearch(module, rtmInfo, verbose, TRUE, invNameArray,
			     typedExprArray, interp);
  mdd_free(reachSet);
  return;
  
}

/**Function********************************************************************

  Synopsis           [build transition relation (as an MDD) for a timed module]

  Description        [build symbolic transition relation for a module.
                      One timed and one untimed conjunct is created for each atom
                      and timed and untimed conjuncts are stored in separate arrays
                      The timed conjunct for the atom represents the transitions
                      where one time unit elapses (all timers incremented by 1)
                      The untimed conjunct for the atom represents control transitions
                      where no time elapses.]

  SideEffects        [transition relation is built and stored in rtmInfo]

  SeeAlso            [RtmModuleSearch]


******************************************************************************/
void
Rtm_ModuleBuildTransitionRelation(
  Rtm_Info_t *rtmInfo,
  Mdl_Module_t *module)
{
  lsList atomList;
  lsGen atmGen;
  Atm_Atom_t *atom;
  mdd_t *atomMdd;
  
  /* Get a list of sorted atoms. */
  atomList = Mdl_ModuleObtainSortedAtomList(module);
  
  /* Create the mdd variable representing the time increment */
  RtmCreateTimeIncVar(rtmInfo, module); 
  
  /* Now loop through atoms and build a conjunct for each atom */
  lsForEachItem(atomList, atmGen, atom) {
    atomMdd = Rtm_AtomBuildTransitionRelation((Atm_Atom_t*)atom, rtmInfo);
    array_insert_last ( mdd_t *, Sym_SymInfoReadConjuncts(rtmInfo->symInfo), atomMdd);
    Sym_SymInfoSetConjunctForAtom(rtmInfo->symInfo, atom, atomMdd);
  }
  
}

/**Function********************************************************************

  Synopsis           [build transition relation for an atom]

  Description        [build transition relation for an atom
                      One timed and one untimed conjunct is created for each atom
                      The timed conjunct for the atom represents the transitions
                      where one time unit elapses (all timers incremented by 1)
                      The untimed conjunct for the atom represents control transitions
                      where no time elapses.
                      The first entry in the array returned is the timed conjunct,
                      the second one is the untimed one.]
                      
  SideEffects        [none]

  SeeAlso            [Rtm_ModuleBuildTransitionRelation]

******************************************************************************/
mdd_t *
Rtm_AtomBuildTransitionRelation(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo
  )
{
  array_t *results =  array_alloc(mdd_t *, 0);
  mdd_t *untimedTransitions, *timedTransitions,
      *atomMdd, *guardOrMdd, *timedGuardOrMdd, *inc0mdd,
      *temp1, *legalrestrictmdd;
  
  /* Initialize some mdd's */
  atomMdd = mdd_zero(Sym_SymInfoReadManager(rtmInfo->symInfo));
  guardOrMdd = mdd_zero(Sym_SymInfoReadManager(rtmInfo->symInfo));
  timedGuardOrMdd = mdd_zero(Sym_SymInfoReadManager(rtmInfo->symInfo));
  /* End: Initialize some mdd's */
  
  Rtm_AtomBuildUpdateCmdListMdds(atom, rtmInfo, &atomMdd, &guardOrMdd);
  Rtm_AtomBuildWaitCmdListMdds(atom, rtmInfo, &atomMdd, &timedGuardOrMdd);

  /* inc0mdd says that time increment variable is 0 */
  inc0mdd = RtmGetMddLiteral(Sym_SymInfoReadManager(rtmInfo->symInfo), rtmInfo->timeIncId, 0);

  /* legalrestrictmdd = if no wait statement is specified for current
     state, then inc is 0 */
  legalrestrictmdd = mdd_or(timedGuardOrMdd, inc0mdd, 1, 1);
  temp1 = atomMdd;
  atomMdd = mdd_and(temp1, legalrestrictmdd, 1, 1);
  
  mdd_free(temp1);
  mdd_free(legalrestrictmdd);
  mdd_free(timedGuardOrMdd);  
  mdd_free(guardOrMdd);
  
  return atomMdd;
}

/**Function********************************************************************

  Synopsis           [updates mdds by processing the update commands of the atom]

  Description        [updates mdds by processing the update commands of the atom
                      guardOrMdd and atomMdd are affected.]

  SideEffects        [guardOrMdd and atomMdd are changed]

  SeeAlso            [Rtm_AtomBuildTransitionRelation]

******************************************************************************/
void
Rtm_AtomBuildUpdateCmdListMdds(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo,
  mdd_t **atomMddPtr,
  mdd_t **guardOrMddPtr
  )
{
  lsList cmdList; 
  lsGen cmdGen;
  Atm_Cmd_t *cmd;
  Atm_Expr_t *guard;

  mdd_t *rhsMdd, *temp1, *temp2, *guardMdd, *cmdMdd;
  mdd_t *atomMdd = mdd_dup(*atomMddPtr);          /* Should be initialized to mdd_zero */
  mdd_t *guardOrMdd = mdd_dup(*guardOrMddPtr);    /* Should be initialized to mdd_zero */

  cmdList = Atm_AtomReadUpdateCommandList(atom);

  lsForEachItem(cmdList, cmdGen, cmd) {

    /* Build the mdd for the guard, OR it with guardOrMdd */
    guard = Atm_CmdReadGuard((Atm_Cmd_t *)cmd);
    guardMdd = Rtm_GuardBuildMdd(rtmInfo, guard);
    temp1 = mdd_or(guardOrMdd, guardMdd, 1, 1);
    mdd_free(guardOrMdd);
    guardOrMdd = temp1;
    /* End: Build the mdd for the guard, OR it with guardOrMdd */
    
    rhsMdd = Rtm_AssignmentsBuildMdd(atom, rtmInfo, (Atm_Cmd_t *)cmd);

    /* mdd for this cmd is obtained by
       (guard *  rhsMdd) */
    cmdMdd = mdd_and(guardMdd, rhsMdd, 1, 1);
    temp1  = mdd_or(atomMdd, cmdMdd, 1, 1);
    
    mdd_free(guardMdd);
    mdd_free(cmdMdd);
    mdd_free(atomMdd);
    mdd_free(rhsMdd);
    
    atomMdd = temp1;
  }

  cmd = Atm_AtomReadDefaultUpdateCommand(atom);
  if (cmd != NIL(Atm_Cmd_t)) {
    Rtm_AtomProcessDefaultUpdateCommand((Atm_Atom_t *)atom, rtmInfo,
                                              &atomMdd, &guardOrMdd);
  }  
  
  Rtm_AtomProcessImplicitUpdateCommand(atom, rtmInfo, &atomMdd, &guardOrMdd);

  /* For update commands, the time increment must be 0 */
  temp1 = atomMdd;
  temp2 = RtmGetMddLiteral(Sym_SymInfoReadManager(rtmInfo->symInfo), rtmInfo->timeIncId, 0);
  atomMdd = mdd_and(temp1, temp2, 1, 1);
  mdd_free(temp1);
  mdd_free(temp2);
  
  *atomMddPtr = atomMdd;
  *guardOrMddPtr = guardOrMdd;
}
/**Function********************************************************************

  Synopsis           [updates mdds by processing the wait(delay) commands of the atom]

  Description        [updates mdds by processing the wait(delay) commands of the atom
                      guardOrMdd and atomMdd are affected.]

  SideEffects        [guardOrMdd and atomMdd are changed]

  SeeAlso            [Rtm_AtomBuildTransitionRelation]

******************************************************************************/
void
Rtm_AtomBuildWaitCmdListMdds(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo,
  mdd_t **atomMddPtr,
  mdd_t **timedGuardOrMddPtr
  )
{
  mdd_t *waitMdd;
  lsList waitCmdList;
  lsGen cmdGen;
  lsGeneric cmd;
  Atm_Expr_t *guard;
  mdd_t *rhsMdd, *temp1, *temp2, *guardMdd, *cmdMdd;
  mdd_t *atomMdd = mdd_dup(*atomMddPtr);          
  mdd_t *timedGuardOrMdd = mdd_dup(*timedGuardOrMddPtr);
                                 /* Should be initialized to mdd_zero */

  waitCmdList = Atm_AtomReadWaitCommandList(atom);
  
  lsForEachItem(waitCmdList, cmdGen, cmd) {
    /* Build the mdd for the guard, OR it with guardOrMdd */
    guard = Atm_CmdReadGuard((Atm_Cmd_t *)cmd);
    guardMdd = Rtm_GuardBuildMdd(rtmInfo, guard);
    temp1 = mdd_or(timedGuardOrMdd, guardMdd, 1, 1);
    mdd_free(timedGuardOrMdd);
    timedGuardOrMdd = temp1;
    /* End: Build the mdd for the guard, OR it with guardOrMdd */
    rhsMdd = Rtm_WaitAssignmentsBuildMdd(atom, rtmInfo, (Atm_Cmd_t *)cmd);

    /* mdd for this cmd is obtained by
       (guard *  rhsMdd) */
    cmdMdd = mdd_and(guardMdd, rhsMdd, 1, 1);
    temp1  = mdd_or(atomMdd, cmdMdd, 1, 1);
    
    mdd_free(guardMdd);
    mdd_free(cmdMdd);
    mdd_free(atomMdd);
    mdd_free(rhsMdd);  
    
    atomMdd = temp1;
  }

  *atomMddPtr = atomMdd;
  *timedGuardOrMddPtr = timedGuardOrMdd;  
}

/**Function********************************************************************

  Synopsis           [builds the mdd for the guard of a command]

  Description        [builds the mdd for the guard of a command]

  SideEffects        [none]

  SeeAlso            [Rtm_AtomBuildTransitionRelation]

******************************************************************************/
mdd_t *
Rtm_GuardBuildMdd(
  Rtm_Info_t *rtmInfo,
  Atm_Expr_t *guard
  )
{
  mdd_t *guardMdd;
  
  guardMdd = Sym_ExprBuildMdd(rtmInfo->symInfo, guard );
  
  return guardMdd;
}

/**Function********************************************************************

  Synopsis           [builds the mdd for the assignments of a command]

  Description        [builds the mdd for the assignments of a command]

  SideEffects        [none]

  SeeAlso            [Rtm_AtomBuildTransitionRelation]

******************************************************************************/
mdd_t *
Rtm_AssignmentsBuildMdd(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo,
  Atm_Cmd_t *cmd
  )
{
  mdd_t *rhsMdd, *assignmentMdd, *temp1, *tempCmd;
  lsList readVarList, assignList, ctrlVarList;
  lsGen gen;
  st_table *assignedVarTable, *readVarTable;
  Var_Variable_t *var;
  Atm_Assign_t *assign;
  
  int uid, pid;

  ctrlVarList = Atm_AtomReadControlVarList(atom); 
  assignList = Atm_CmdReadAssignList(cmd);
  readVarList = Atm_AtomReadReadVarList(atom);

  /* Table of variables assigned to by this assignment
     Filled in by Sym_AssignmentBuildMdd */
  assignedVarTable = st_init_table(st_ptrcmp, st_ptrhash);

  /* Table of variables with state */
  readVarTable = st_init_table(st_ptrcmp, st_ptrhash);
  lsForEachItem(readVarList, gen, var){
    st_insert(readVarTable, (char *)var, NIL(char));
  };
 
  /* go through all the assignments */
  rhsMdd = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));

  lsForEachItem(assignList, gen, assign) {
    Atm_Assign_t *typedAssign;
    
      assignmentMdd = Sym_AssignmentBuildMdd(rtmInfo->symInfo, assign, assignedVarTable);

      temp1 = mdd_and(rhsMdd, assignmentMdd, 1, 1);
      mdd_free(assignmentMdd);
      mdd_free(rhsMdd);
      rhsMdd = temp1;
    }
      
    
    /* make sure that the variables not changed by this assignment are
       constrained to not change */
    lsForEachItem(ctrlVarList, gen, var){

      if(!st_lookup(readVarTable, (char *)var, NIL(char *)))
        continue;

      if (st_lookup(assignedVarTable, (char *) var, NIL(char *)))
        continue;
      
      if(!Sym_SymInfoLookupUnprimedVariableId(rtmInfo->symInfo, var, &uid )){
        Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
      }

      if(!Sym_SymInfoLookupPrimedVariableId(rtmInfo->symInfo, var, &pid )){
        Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
      }

      if(Var_VariableReadDataType( (Var_Variable_t *) var) == Var_Event_c){
        tempCmd = RtmGetMddLiteral(Sym_SymInfoReadManager(rtmInfo->symInfo), pid, 0);
      }
      else {
        tempCmd = mdd_eq(Sym_SymInfoReadManager(rtmInfo->symInfo), pid, uid);
      }
      
      temp1 = mdd_and(rhsMdd, tempCmd, 1, 1);
      mdd_free(tempCmd);
      mdd_free(rhsMdd);
      rhsMdd = temp1;
    }

    st_free_table(assignedVarTable);
    st_free_table(readVarTable);
    
    return rhsMdd;
}
/**Function********************************************************************

  Synopsis           [builds the mdd for the assignments of an init command]

  Description        [builds the mdd for the assignments of an init command]

  SideEffects        [none]

  SeeAlso            [Rtm_AtomBuildTransitionRelation]

******************************************************************************/
mdd_t *
Rtm_InitAssignmentsBuildMdd(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo,
  Atm_Cmd_t *cmd
  )
{
  mdd_t *rhsMdd, *assignmentMdd, *temp1;
  Var_Variable_t *var;
  Atm_Expr_t *expr;
  lsList readVarList, assignList;
  lsGen gen;
  Atm_Assign_t *assign;
  st_table *assignedVarTable;
  int uid, pid;
  
  assignList = Atm_CmdReadAssignList(cmd);
  readVarList = Atm_AtomReadReadVarList(atom);

  /* Table of variables assigned to by this assignment
     Filled in by Sym_AssignmentBuildMdd */
  assignedVarTable = st_init_table(st_ptrcmp, st_ptrhash);

  
  /* go through all the assignments */
  rhsMdd = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));

  lsForEachItem(assignList, gen, assign) {
    Atm_Assign_t *typedAssign;

    /* Checking if a timer is assigned a non-zero initial value. This is an error */
    var = Atm_AssignReadVariable((Atm_Assign_t *) assign);
    if (Var_VariableReadDataType( (Var_Variable_t *) var) == Var_Timer_c) {
      expr = Atm_AssignReadExpr((Atm_Assign_t *) assign);
      if (Atm_ExprReadType(expr) != Atm_NumConst_c)  {
        Main_MochaErrorPrint("Warning: Timer assigned to non-zero initial value. Not allowed. Timer will be initialized to zero.\n");
        continue;
      }
      else {
        if ( ((long) Atm_ExprReadLeftChild(expr)) != 0) {
          Main_MochaErrorPrint("Warning: Timer assigned to non-zero initial value. Not allowed. Timer will be initialized to zero.\n");
          continue;
        }
      }
    }
      
    assignmentMdd = Sym_AssignmentBuildMdd(rtmInfo->symInfo, assign, assignedVarTable);

    temp1 = mdd_and(rhsMdd, assignmentMdd, 1, 1);
    mdd_free(assignmentMdd);
    mdd_free(rhsMdd);
    rhsMdd = temp1;
  }
 
  st_free_table(assignedVarTable);  
  
  return rhsMdd;
}
/**Function********************************************************************

  Synopsis           [updates mdds by processing the default update command of the atom]

  Description        [updates mdds by processing the default update command of the atom
                      guardOrMdd and atomMdd are affected.]

  SideEffects        [guardOrMdd and atomMdd are changed]

  SeeAlso            [Rtm_AtomBuildUpdateCmdListMdds, Rtm_AtomBuildTransitionRelation]

******************************************************************************/
void
Rtm_AtomProcessDefaultUpdateCommand(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo,
  mdd_t **atomMddPtr,
  mdd_t **guardOrMddPtr
  )
{
  Atm_Cmd_t *defaultCmd = Atm_AtomReadDefaultUpdateCommand(atom); /* Should be non-nil */
  st_table *assignedVarTable;
  lsGen gen;
  lsList assignList, ctrlVarList, readVarList;
  st_table *readVarTable;
  mdd_t *guardMdd, *temp1, *cmdMdd, *rhsMdd, *assignmentMdd, *tempCmd;
  int uid, pid;
  Var_Variable_t *var;
  Atm_Assign_t *assign;

  mdd_t *atomMdd = mdd_dup(*atomMddPtr);
  mdd_t *guardOrMdd = mdd_dup(*guardOrMddPtr);
  
  if (!defaultCmd)
    Main_MochaErrorPrint("Rtm_AtomProcessDefaultUpdateCommand: There is no default update command for this atom \n");

  ctrlVarList = Atm_AtomReadControlVarList(atom);   
  assignList = Atm_CmdReadAssignList(defaultCmd);
  readVarList = Atm_AtomReadReadVarList(atom);
  
  /* Table of variables with state */
  readVarTable = st_init_table(st_ptrcmp, st_ptrhash);
  lsForEachItem(readVarList, gen, var){
    st_insert(readVarTable, (char *)var, NIL(char));
  };


  guardMdd = mdd_not(guardOrMdd);

  /* go through all the assignments */
  assignedVarTable = st_init_table(st_ptrcmp, st_ptrhash);
  rhsMdd = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));

  lsForEachItem(assignList, gen, assign) {
    Atm_Assign_t *typedAssign;
    
    assignmentMdd = Sym_AssignmentBuildMdd(rtmInfo->symInfo, assign, assignedVarTable);
      
    temp1 = mdd_and(rhsMdd, assignmentMdd, 1, 1);
    mdd_free(assignmentMdd);
    mdd_free(rhsMdd);
    rhsMdd = temp1;
  }
    
  /* make sure that the variables not changed by this assignment are
     constrained to not change */
  lsForEachItem(ctrlVarList, gen, var){
    if(!st_lookup(readVarTable, (char *)var, NIL(char *)))
      continue;
    
    if (st_lookup(assignedVarTable, (char *) var, NIL(char *)))
      continue;
    
    if(!Sym_SymInfoLookupUnprimedVariableId(rtmInfo->symInfo, var, &uid )){
      Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
    }

    if(!Sym_SymInfoLookupPrimedVariableId(rtmInfo->symInfo, var, &pid )){
      Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
    }
    
    if(Var_VariableReadDataType( (Var_Variable_t *) var) == Var_Event_c){
      tempCmd = RtmGetMddLiteral(Sym_SymInfoReadManager(rtmInfo->symInfo), pid, 0);
    }
    else {
      tempCmd = mdd_eq(Sym_SymInfoReadManager(rtmInfo->symInfo), pid, uid);
    }
    
    temp1 = mdd_and(rhsMdd, tempCmd, 1, 1);
    mdd_free(tempCmd);
    mdd_free(rhsMdd);
    rhsMdd = temp1;
  }

  st_free_table(assignedVarTable);

  cmdMdd = mdd_and(guardMdd, rhsMdd, 1, 1);
  temp1 = mdd_or(atomMdd, cmdMdd, 1, 1);

  mdd_free(guardMdd);
  mdd_free(cmdMdd);
  mdd_free(atomMdd);
  mdd_free(rhsMdd);
  
  atomMdd = temp1;

  /* make guardOrMdd the tautology */
  mdd_free(guardOrMdd);
  guardOrMdd = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));

    
  *atomMddPtr = atomMdd;
  *guardOrMddPtr = guardOrMdd;
}

/**Function********************************************************************

  Synopsis           [Process the implicit command for the update command list of an atom]

  Description        [Process the implicit command for the update command list of an atom] 

  SideEffects        [none]

  SeeAlso            [Sym_AtomBuildTransitionRelation]

******************************************************************************/
void
Rtm_AtomProcessImplicitUpdateCommand(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo,
  mdd_t **atomMddPtr,
  mdd_t **guardOrMddPtr
  )
{
  mdd_t *tempCmd, *temp1, *temp2;
  mdd_t *icmdMdd = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));
  lsGen gen;  
  lsList ctrlVarList, readVarList, rdAwtVarList;
  st_table *readVarTable;
  Var_Variable_t *var;
  mdd_t *atomMdd = mdd_dup(*atomMddPtr);
  mdd_t *guardOrMdd = mdd_dup(*guardOrMddPtr);
  int uid, pid;

  rdAwtVarList = Atm_AtomObtainReadAwaitVarList(atom); 
  ctrlVarList = Atm_AtomReadControlVarList(atom);
  readVarList = Atm_AtomReadReadVarList(atom);
    /* Table of variables with state */
  readVarTable = st_init_table(st_ptrcmp, st_ptrhash);
  lsForEachItem(readVarList, gen, var){
    st_insert(readVarTable, (char *)var, NIL(char));
  };
  
  /* compute implicit command -- none of the read & controlled variables change! */ 
  icmdMdd = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));
  lsForEachItem(ctrlVarList, gen, var){

    if(!st_lookup(readVarTable, (char *)var, NIL(char *)))
      continue;

    if(!Sym_SymInfoLookupUnprimedVariableId(rtmInfo->symInfo, var, &uid )){
      Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
    }

    if(!Sym_SymInfoLookupPrimedVariableId(rtmInfo->symInfo, var, &pid )){
      Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
    }

    if(Var_VariableReadDataType( (Var_Variable_t *) var) == Var_Event_c){
      tempCmd = RtmGetMddLiteral(Sym_SymInfoReadManager(rtmInfo->symInfo), pid, 0);
    }
    else {
      tempCmd = mdd_eq(Sym_SymInfoReadManager(rtmInfo->symInfo), pid, uid);
    }
      
    temp1 = mdd_and(icmdMdd, tempCmd, 1, 1);
    mdd_free(icmdMdd);
    mdd_free(tempCmd);
    icmdMdd = temp1;
  }

  /* now the implicit guard */
  /* if lazy atom change guardOrMdd to zero */
  if(Atm_AtomReadAtomType(atom) == Atm_Lazy_c){
    temp1 = mdd_dup(icmdMdd);
    /* Main_MochaPrint( "\nlazy atom %s..adding cmd:\n",
       Atm_AtomReadName(atom)); */
  }
  else{
    temp1 = mdd_and(guardOrMdd, icmdMdd, 0, 1);
    /* Main_MochaPrint( "\nNON lazy %s..adding cmd with implicit guard:\n",
       Atm_AtomReadName(atom)); */
  }

  temp2 = mdd_or(atomMdd, temp1, 1,1);
  mdd_free(temp1);
  mdd_free(guardOrMdd);
  mdd_free(atomMdd);
  atomMdd = temp2;
  
  /* in the case of event atom come up with new implicit guard */
  if((Atm_AtomReadAtomType(atom) == Atm_Event_c) &&
     (lsLength(rdAwtVarList) > 0)) {
    mdd_t *iguardMdd = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));
    lsForEachItem(rdAwtVarList, gen, var){
      if(Var_VariableReadDataType( (Var_Variable_t *) var) != Var_Event_c)
        continue;
      
      if(!Sym_SymInfoLookupPrimedVariableId(rtmInfo->symInfo, var, &pid )){
          Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
        }
      temp1 = RtmGetMddLiteral(Sym_SymInfoReadManager(rtmInfo->symInfo), pid, 0);
      temp2 = mdd_and(iguardMdd, temp1, 1, 1);
      mdd_free(iguardMdd);
      iguardMdd = temp2;
    }
    temp1 = mdd_and(iguardMdd, icmdMdd, 1, 1);
    temp2 = mdd_or(atomMdd, temp1, 1,1);
    mdd_free(temp1);
    mdd_free(iguardMdd);
    mdd_free(atomMdd);
    atomMdd = temp2;
  }
  
  /*in any case - event atom  or not, free icmdMdd */
  mdd_free(icmdMdd);
  st_free_table(readVarTable);

  *atomMddPtr = atomMdd;
  *guardOrMddPtr = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));
}
/**Function********************************************************************

  Synopsis           [builds the mdd for all of the assignments of a wait command]

  Description        [builds the mdd for all of the assignments of a wait command]

  SideEffects        [none]

  SeeAlso            [Rtm_AtomBuildTransitionRelation]

******************************************************************************/
mdd_t *
Rtm_WaitAssignmentsBuildMdd(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo,
  Atm_Cmd_t *cmd
  )
{
  mdd_t *rhsMdd, *assignmentMdd, *temp1, *tempCmd;
  Var_Variable_t *var;
  Atm_Expr_t *expr;
  Atm_Assign_t *assign;
  lsList readVarList, assignList, ctrlVarList;
  lsGen gen;
  st_table *assignedVarTable, *readVarTable;
  int uid, pid;
  

  ctrlVarList = Atm_AtomReadControlVarList(atom);
  assignList = Atm_CmdReadAssignList(cmd);
  readVarList = Atm_AtomReadReadVarList(atom);

  /* Table of variables with state */
  readVarTable = st_init_table(st_ptrcmp, st_ptrhash);
  lsForEachItem(readVarList, gen, var){
    st_insert(readVarTable, (char *)var, NIL(char));
  };

  rhsMdd = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));

  lsForEachItem(assignList, gen, assign) {
    
    var = Atm_AssignReadVariable((Atm_Assign_t *) assign);
    expr = Atm_AssignReadExpr((Atm_Assign_t *) assign);

    if (Var_VariableReadDataType( (Var_Variable_t *) var) != Var_Timer_c) {
        Main_MochaErrorPrint(" Error: Rtm_AtomBuildTransitionRelation: Assignment to non-timer variable inside a wait statement \n");
      }
      
      if(!Sym_SymInfoLookupPrimedVariableId(rtmInfo->symInfo, var, &pid )){
        Main_MochaErrorPrint( "Error: Rtm_AtomBuildTransitionRelation, cant find mdd id for var\n");
      }

      if(!Sym_SymInfoLookupUnprimedVariableId(rtmInfo->symInfo, var, &uid )){
        Main_MochaErrorPrint( "Error: Rtm_AtomBuildTransitionRelation, cant find mdd id for var\n");
      }

      assignmentMdd = Rtm_BuildOneWaitAssignmentMdd(expr,rtmInfo,pid,uid);
      temp1 = mdd_and(rhsMdd, assignmentMdd, 1, 1);

      mdd_free(assignmentMdd);
      mdd_free(rhsMdd);
      rhsMdd = temp1;      
  }

  /* make sure that the timers not mentioned in this statement are incremented by inc */
  lsForEachItem(ctrlVarList, gen, var){
    if(!Sym_SymInfoLookupUnprimedVariableId(rtmInfo->symInfo, var, &uid )){
      Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
    }
    if(!Sym_SymInfoLookupPrimedVariableId(rtmInfo->symInfo, var, &pid )){
      Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
    }

    /* If variable has no history, no restrictions on its next value */
    if(!st_lookup(readVarTable, (char *)var, NIL(char *)))
      continue;

    /* For wait statements, all non-timer variables need to remain unchanged */
    if (RtmMddInTrueSupport(rhsMdd, pid, rtmInfo)) {
      continue;
    }
    else if (Var_VariableReadDataType( (Var_Variable_t *) var) == Var_Event_c){
      tempCmd = RtmGetMddLiteral(Sym_SymInfoReadManager(rtmInfo->symInfo), pid, 0);
    }
    else if (Var_VariableReadDataType( (Var_Variable_t *) var) == Var_Timer_c){
      tempCmd = Rtm_BuildUnconstrainedTimerIncMdd(pid, uid, var, rtmInfo);
    }
    else {
      tempCmd = mdd_eq(Sym_SymInfoReadManager(rtmInfo->symInfo), pid, uid);
    }
    
    temp1 = mdd_and(rhsMdd, tempCmd, 1, 1);
    mdd_free(tempCmd);
    mdd_free(rhsMdd);
    rhsMdd = temp1;
  }
  
  return rhsMdd;
}

/**Function********************************************************************

  Synopsis           [builds the mdd for a single assignment of a wait command]

  Description        [builds the mdd for a single assignment of a wait command]

  SideEffects        [none]

  SeeAlso            [Rtm_AtomBuildTransitionRelation]

******************************************************************************/
mdd_t *
Rtm_BuildOneWaitAssignmentMdd(
  Atm_Expr_t *expr,
  Rtm_Info_t *rtmInfo,
  int pid,
  int uid
  )
{
  mdd_manager *manager =  Sym_SymInfoReadManager(rtmInfo->symInfo);
  mdd_t *result = mdd_zero(manager);
  mdd_t *incMdd, *newTimerMdd, *curTimerMdd, *incTimerMdd, *temp;
  mdd_t *temp1, *temp2, *innerLoopResult, *outerLoopResult, *thisIterResult;
  long incUpperBound, invUpperBound, inc, curTimerVal;  
  Atm_ExprType exprType;
  
  
  exprType = Atm_ExprReadType(expr);
  switch(exprType){
      case Atm_TimerUpperBound_c:
      {
        invUpperBound = (long) Atm_ExprReadLeftChild(expr);

        outerLoopResult = mdd_zero(manager);
        for (curTimerVal = 0; curTimerVal <= invUpperBound; curTimerVal++) {

          incUpperBound = invUpperBound - curTimerVal;

           /* This if statement is here to restrict the time increment to a
              maximum of 1. Experiments show that traversal is more efficient
              with this restriction. */          
          if (incUpperBound > 0)
            incUpperBound = 1;
            
          innerLoopResult = mdd_zero(manager);
          for (inc = 0; inc <= incUpperBound; inc++) {
            incMdd = RtmGetMddLiteral(manager, rtmInfo->timeIncId, inc);
            newTimerMdd = RtmGetMddLiteral(manager, pid, curTimerVal+inc);
            incTimerMdd = mdd_and(incMdd, newTimerMdd, 1, 1);
            mdd_free(incMdd);
            mdd_free(newTimerMdd);
            
            temp = innerLoopResult;
            innerLoopResult = mdd_or(temp, incTimerMdd, 1, 1);
            mdd_free(temp);
            mdd_free(incTimerMdd);
          }
          
          curTimerMdd = RtmGetMddLiteral(manager, uid, curTimerVal);          
          thisIterResult = mdd_and(curTimerMdd, innerLoopResult, 1, 1);
          mdd_free(curTimerMdd);
          mdd_free(innerLoopResult);


          temp = outerLoopResult;
          outerLoopResult = mdd_or(temp, thisIterResult, 1, 1);
          mdd_free(temp);
          mdd_free(thisIterResult);
        }
        
        break;
      }
      default:
      {
        Main_MochaErrorPrint("RtmBuildOneWaitAssignmentMdd: Disallowed assignment in wait statement\n");
      }     
  }

  temp1 = mdd_leq_c(manager, rtmInfo->timeIncId, invUpperBound);
  result = mdd_and(temp1, outerLoopResult, 1, 1);
  
  mdd_free(temp1);
  mdd_free(outerLoopResult);

  return (result);
}

/**Function********************************************************************

  Synopsis           [Builds the mdd for timers not constrained by a wait command]

  Description        [Timers not constrained by a wait command all increase by
                      the amount "inc" as represented by the rtmInfo->timeIncId
                      variable]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
mdd_t *
Rtm_BuildUnconstrainedTimerIncMdd(
  int pid,
  int uid,
  Var_Variable_t *var,
  Rtm_Info_t *rtmInfo
  )
{
  mdd_manager *manager = Sym_SymInfoReadManager(rtmInfo->symInfo);
  mdd_t *incMdd, *newTimerMdd, *curTimerMdd, *incTimerMdd, *temp;
  mdd_t *temp1, *temp2, *temp3, *innerLoopResult, *outerLoopResult, *thisIterResult;
  long inc, curTimerVal, curTimerUpperBound;

  curTimerUpperBound = Var_VariableTypeReadNumVals(Var_VariableReadType(var))-1;

  outerLoopResult = mdd_zero(manager);

  for (curTimerVal = 0; curTimerVal <= curTimerUpperBound; curTimerVal++) {
    
    innerLoopResult = mdd_zero(manager);
    for (inc = 0; (inc + curTimerVal <= curTimerUpperBound) && (inc <= 1); inc++) {
      incMdd = RtmGetMddLiteral(manager, rtmInfo->timeIncId, inc);
      newTimerMdd = RtmGetMddLiteral(manager, pid, curTimerVal+inc);
      incTimerMdd = mdd_and(incMdd, newTimerMdd, 1, 1);
      mdd_free(incMdd);
      mdd_free(newTimerMdd);
            
      temp = innerLoopResult;
      innerLoopResult = mdd_or(temp, incTimerMdd, 1, 1);
      mdd_free(temp);
      mdd_free(incTimerMdd);
    }

   /* If a timer has reached its upper bound, its value should remain the same
      for a time increment of 1 */
    if (curTimerUpperBound == curTimerVal) {
      temp1 =  mdd_eq_c(manager, rtmInfo->timeIncId, 1);
      temp2 =  mdd_eq_c(manager, pid, curTimerUpperBound);
      temp3 =  mdd_and(temp1, temp2, 1, 1);
      mdd_free(temp1);
      mdd_free(temp2);
    }
    else {
      temp3 =  mdd_zero(manager);
    }

    
    thisIterResult = mdd_or(temp3, innerLoopResult, 1, 1);
    mdd_free(temp3);
    mdd_free(innerLoopResult);
    
    temp1 = thisIterResult;
    curTimerMdd = RtmGetMddLiteral(manager, uid, curTimerVal);
    thisIterResult = mdd_and(curTimerMdd, temp1, 1, 1);
    mdd_free(curTimerMdd);
    mdd_free(temp1);

    
    temp = outerLoopResult;
    outerLoopResult = mdd_or(temp, thisIterResult, 1, 1);
    mdd_free(temp);
    mdd_free(thisIterResult);
  }
  
  return (outerLoopResult);
}  



 
/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Read variable order for module from a file]

  Description        [Write variable order for module from a file]

  SideEffects        [none]
  
  SeeAlso            [SymWriteOrder]

******************************************************************************/
array_t *
RtmReadOrder(
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

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
RtmCreateTimeIncVar(
  Rtm_Info_t *rtmInfo,
  Mdl_Module_t *module
  )
{
  int unprimedMddId, primedMddId;
  char *varName;

  /* Time increments of 0 and 1 are allowed only. Therefore, the range of this variable is 2 */
  
  Var_Type_t *incType = Var_VarTimeIncrementTypeAlloc(2);
  Var_Variable_t *incVar = Var_VariableAlloc((VarModuleGeneric)module, "timeIncrement", incType);

  /* Create the primed and unprimed variables using the mdd manager */  
  array_t *mvarValues = array_alloc(unsigned int, 2);      
  array_insert(unsigned int, mvarValues, 0, 2);
  array_insert(unsigned int, mvarValues, 1, 2);
  unprimedMddId = mdd_create_variables(Sym_SymInfoReadManager(rtmInfo->symInfo),
                                       mvarValues,
                                       NIL(array_t),
                                       NIL(array_t));
  primedMddId = unprimedMddId + 1;   
  array_free(mvarValues);

  /* Register variable with the Mdl package */
  varName = util_strsav(Var_VariableReadName((Var_Variable_t *)incVar));
  Mdl_ModuleAddVariable(module, util_strsav(varName), incVar);

  /* Register the mdd id of the time increment variable with the rtmInfo data structure */
  rtmInfo->timeIncId = ((int)unprimedMddId);

  /* Register the unprimed variable with rtmInfo->symInfo */
  st_insert(Sym_SymInfoReadVarToUnprimedId(rtmInfo->symInfo),
            (char *)(incVar),
            (char*)(long)unprimedMddId
            );
  st_insert(Sym_SymInfoReadIdToVar(rtmInfo->symInfo), (char *)(long) unprimedMddId, (char *) incVar);
  array_insert_last(int, Sym_SymInfoReadUnprimedIdArray(rtmInfo->symInfo), unprimedMddId);

  /* Register the primed variable with rtmInfo->symInfo
     This variable is never used, but is created so that each variable has
     one primed and one unprimed mdd id. Other packages may be relying on this */  
  st_insert(Sym_SymInfoReadVarToPrimedId(rtmInfo->symInfo),
            (char *)(incVar),
            (char*)(long)primedMddId
            );
  st_insert(Sym_SymInfoReadIdToVar(rtmInfo->symInfo), (char *)(long) primedMddId, (char *) incVar);
  array_insert_last(int, Sym_SymInfoReadPrimedIdArray(rtmInfo->symInfo), primedMddId);

}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Tcl_HashTable *
Rtm_RegionManagerReadRegionTable(
  Rtm_RegionManager_t *rtmRegionManager
  ) 
{
  return(rtmRegionManager->rtmRegionTable);
}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Rtm_Info_t *
Rtm_RegionManagerObtainRtmInfoFromModule(
  Rtm_RegionManager_t *rtmRegionManager,
  Mdl_Module_t *module)
{
  Rtm_Info_t *rtmInfo;
  st_table *moduleToRtmInfo =
    rtmRegionManager->moduleToRtmInfo;
  
  if(st_lookup(moduleToRtmInfo, (char *) module, (char **) &rtmInfo))
    return rtmInfo;
  else
    return NIL(Rtm_Info_t);
}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Rtm_RegionManager_t *
Rtm_RegionManagerAlloc(
)
{
  Rtm_RegionManager_t *rtmRegionManager = ALLOC(Rtm_RegionManager_t, 1);

  rtmRegionManager->moduleToRtmInfo = st_init_table(st_ptrcmp, st_ptrhash);
  rtmRegionManager->rtmRegionTable = ALLOC(Tcl_HashTable, 1);
  Tcl_InitHashTable(rtmRegionManager->rtmRegionTable, TCL_STRING_KEYS);
  rtmRegionManager->regionCounter = 0;

  return rtmRegionManager; 
}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Rtm_RegionManagerAddModuleToRtmInfo(
  Rtm_RegionManager_t *rtmRegionManager,
  Mdl_Module_t *module,
  Rtm_Info_t *rtmInfo)
{
  st_insert(rtmRegionManager->moduleToRtmInfo, (char *) module, (char *) rtmInfo);  
}
/**Function********************************************************************

  Synopsis           [build mdd for initial region of module]

  Description        [build mdd for initial region of module]

  SideEffects        [none]

  SeeAlso            [Rtm_ModuleBuildTransitionRelation]

******************************************************************************/
mdd_t *
Rtm_ModuleBuildInitialRegion(
  Rtm_Info_t *rtmInfo,
  Mdl_Module_t *module)
{
  mdd_t *initialState = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));
  lsList atomList;
  lsGen  atmGen;
  lsGeneric atom;
  int i = 0;
  int varid;
  mdd_t *initStateUnprimed;
  
  
  /* Get a list of sorted atoms. */
  atomList = Mdl_ModuleObtainSortedAtomList(module);

  /* Now loop through atoms and build initial states for each atom */
  lsForEachItem(atomList, atmGen, atom) {
    mdd_t *atomInitState = Rtm_AtomBuildInitialRegion((Atm_Atom_t*)atom, rtmInfo);
    mdd_t *temp = mdd_and (atomInitState, initialState, 1, 1);
    mdd_free(initialState);
    mdd_free(atomInitState);
    initialState = temp;
  }
  /* make sure that initial state is independent of unprimed vars    */
  for ( i = 0 ; i < array_n(Sym_SymInfoReadUnprimedIdArray(rtmInfo->symInfo)); i++){
    varid = array_fetch(int, Sym_SymInfoReadUnprimedIdArray(rtmInfo->symInfo), i);

    if(RtmMddInTrueSupport(initialState, varid, rtmInfo)){
      Main_MochaErrorPrint(
              "Error: Initial region depends on unprimed var: %d \n",
              varid);
      exit(1);
    }

  }

  if(array_n(Sym_SymInfoReadPrimedIdArray(rtmInfo->symInfo)) !=
     array_n(Sym_SymInfoReadUnprimedIdArray(rtmInfo->symInfo))){
    Main_MochaErrorPrint(
            "Error: Unprimed and Primed MDD  id arrays have unequal sizes\n");
    exit(1);
  }
  
  /* replace primed variables in initial state by unprimed  ones */
  initStateUnprimed =  mdd_substitute( Sym_SymInfoReadManager(rtmInfo->symInfo),
                                       initialState,
                                       Sym_SymInfoReadPrimedIdArray(rtmInfo->symInfo),
                                       Sym_SymInfoReadUnprimedIdArray(rtmInfo->symInfo));
  mdd_free(initialState);
  return(initStateUnprimed);

}
/**Function********************************************************************

  Synopsis           [build initial region for an atom in a timed module]

  Description        [build initial region for an atom in a timed module]

  SideEffects        [none]

  SeeAlso            [Rtm_AtomBuildTransitionRelation]

******************************************************************************/
mdd_t *
Rtm_AtomBuildInitialRegion(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo
  )
{
  mdd_t *atomMdd, *guardOrMdd;

  /* Initialize some mdd's */
  atomMdd = mdd_zero(Sym_SymInfoReadManager(rtmInfo->symInfo));
  guardOrMdd = mdd_zero(Sym_SymInfoReadManager(rtmInfo->symInfo));
  /* End: Initialize some mdd's */

  Rtm_AtomBuildInitCmdListMdds(atom, rtmInfo, &atomMdd, &guardOrMdd);
  
  return atomMdd;
}

/**Function********************************************************************

  Synopsis           [updates mdds by processing the init commands of the atom]

  Description        [updates mdds by processing the init commands of the atom
                      guardOrMdd and atomMdd are affected.]

  SideEffects        [guardOrMdd and atomMdd are changed]

  SeeAlso            [Rtm_AtomBuildInitialRegion]

******************************************************************************/
void
Rtm_AtomBuildInitCmdListMdds(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo,
  mdd_t **atomMddPtr,
  mdd_t **guardOrMddPtr
  )
{
  lsList cmdList, ctrlVarList; 
  lsGen cmdGen, varGen;
  Atm_Cmd_t *cmd;
  Atm_Expr_t *guard;
  Var_Variable_t *var;
  int pid;
  
  mdd_t *rhsMdd, *temp1, *temp2, *timerResetMdd, *guardMdd, *cmdMdd;
  mdd_t *atomMdd = mdd_dup(*atomMddPtr);          /* Should be initialized to mdd_zero */
  mdd_t *guardOrMdd = mdd_dup(*guardOrMddPtr);    /* Should be initialized to mdd_zero */

  cmdList = Atm_AtomReadInitCommandList(atom);

  lsForEachItem(cmdList, cmdGen, cmd) {

    /* Build the mdd for the guard, OR it with guardOrMdd */
    guard = Atm_CmdReadGuard((Atm_Cmd_t *)cmd);
    guardMdd = Rtm_GuardBuildMdd(rtmInfo, guard);
    temp1 = mdd_or(guardOrMdd, guardMdd, 1, 1);
    mdd_free(guardOrMdd);
    guardOrMdd = temp1;
    /* End: Build the mdd for the guard, OR it with guardOrMdd */
    
    rhsMdd = Rtm_InitAssignmentsBuildMdd(atom, rtmInfo, (Atm_Cmd_t *)cmd);

    /* mdd for this cmd is obtained by
       (guard *  rhsMdd) */
    cmdMdd = mdd_and(guardMdd, rhsMdd, 1, 1);
    temp1  = mdd_or(atomMdd, cmdMdd, 1, 1);
    
    mdd_free(guardMdd);
    mdd_free(cmdMdd);
    mdd_free(atomMdd);
    mdd_free(rhsMdd);
    
    atomMdd = temp1;
  }

  cmd = Atm_AtomReadDefaultInitCommand(atom);
  if (cmd != NIL(Atm_Cmd_t)) {
    Rtm_AtomProcessDefaultInitCommand((Atm_Atom_t *)atom, rtmInfo,
                                              &atomMdd, &guardOrMdd);
  }
  else {
    /* add implicit guard to atomMdd
       i.e. if none of the guards hold, then initial state can be anything.
       Timers, however, must be all 0 at the initial state. This is taken care of below.
    */
    temp1 = mdd_or(guardOrMdd, atomMdd, 0, 1);
    mdd_free(guardOrMdd);
    mdd_free(atomMdd);
    atomMdd = temp1;
  }
  
  /* Timers must be 0 in initial state. Initial state mdd's are constructed using
     primed id's. Then substitution happens at the end of Rtm_ModuleBuildInitialRegion
     when constructing initStateUnprimed */
  ctrlVarList = Atm_AtomReadControlVarList((Atm_Atom_t *)atom);
  lsForEachItem(ctrlVarList, varGen, var){
    if (Var_VariableReadDataType( (Var_Variable_t *) var) == Var_Timer_c) {
      Sym_SymInfoLookupPrimedVariableId(rtmInfo->symInfo, var, &pid );
      timerResetMdd = RtmGetMddLiteral(Sym_SymInfoReadManager(rtmInfo->symInfo), pid, 0);
      temp1 = atomMdd;
      atomMdd = mdd_and(temp1, timerResetMdd, 1, 1);
      mdd_free(temp1);
      mdd_free(timerResetMdd);
    }
  }
  
  *atomMddPtr = atomMdd;
  *guardOrMddPtr = guardOrMdd;
}
/**Function********************************************************************

  Synopsis           [updates mdds by processing the default init command of the atom]

  Description        [updates mdds by processing the default init command of the atom
                      guardOrMdd and atomMdd are affected.]

  SideEffects        [guardOrMdd and atomMdd are changed]

  SeeAlso            [Rtm_AtomBuildUpdateCmdListMdds, Rtm_AtomBuildTransitionRelation]

******************************************************************************/
void
Rtm_AtomProcessDefaultInitCommand(
  Atm_Atom_t *atom,
  Rtm_Info_t *rtmInfo,
  mdd_t **atomMddPtr,
  mdd_t **guardOrMddPtr
  )
{
  Atm_Cmd_t *defaultCmd = Atm_AtomReadDefaultInitCommand(atom); /* Should be non-nil */
  st_table *assignedVarTable;
  lsList assignList;
  lsGen gen;
  mdd_t *guardMdd, *temp1, *cmdMdd, *rhsMdd, *assignmentMdd;
  Var_Variable_t *var;
  Atm_Expr_t *expr;
  Atm_Assign_t *assign;
  
  mdd_t *atomMdd = mdd_dup(*atomMddPtr);
  mdd_t *guardOrMdd = mdd_dup(*guardOrMddPtr);
  
  if (!defaultCmd)
    Main_MochaErrorPrint("Rtm_AtomProcessDefaultInitCommand: There is no default init command for this atom \n");

  guardMdd = mdd_not(guardOrMdd);
  assignList = Atm_CmdReadAssignList(defaultCmd);
    
  /* go through all the assignments */
  assignedVarTable = st_init_table(st_ptrcmp, st_ptrhash);
  rhsMdd = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));

  lsForEachItem(assignList, gen, assign) {

    Atm_Assign_t *typedAssign;
    /* Checking if a timer is assigned a non-zero initial value. This is an error */
    var = Atm_AssignReadVariable((Atm_Assign_t *) assign);
    if (Var_VariableReadDataType( (Var_Variable_t *) var) != Var_Timer_c) {
      expr = Atm_AssignReadExpr((Atm_Assign_t *) assign);
      if (Atm_ExprReadType(expr) != Atm_NumConst_c)  {
        Main_MochaErrorPrint("Warning: Timer assigned to non-zero initial value. Not allowed. Timer will be initialized to zero.\n");
        continue;
      }
      else {
        if ( ((long) Atm_ExprReadLeftChild(expr)) != 0) {
          Main_MochaErrorPrint("Warning: Timer assigned to non-zero initial value. Not allowed. Timer will be initialized to zero.\n");
          continue;
        }
      }
    }
    
    assignmentMdd = Sym_AssignmentBuildMdd(rtmInfo->symInfo, assign, assignedVarTable);
      
    temp1 = mdd_and(rhsMdd, assignmentMdd, 1, 1);
    mdd_free(assignmentMdd);
    mdd_free(rhsMdd);
    rhsMdd = temp1;
  }
  st_free_table(assignedVarTable);

  /* mdd for this cmd is obtained by (guard *  rhsmdd) */  
  cmdMdd = mdd_and(guardMdd, rhsMdd, 1, 1);
  temp1  = mdd_or(atomMdd, cmdMdd, 1, 1);

  mdd_free(guardMdd);
  mdd_free(cmdMdd);
  mdd_free(atomMdd);
  atomMdd = temp1;

  mdd_free(guardOrMdd);
  guardOrMdd = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));
  
  *atomMddPtr = atomMdd;
  *guardOrMddPtr = guardOrMdd;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Rtm_Region_t *
Rtm_AllocateRegionStruct(
  Mdl_Module_t *module,
  mdd_t *regionSet
  )
{
  Rtm_Region_t *retRegion = ALLOC(Rtm_Region_t, 1);

  retRegion->module = module;
  retRegion->mdd = regionSet;

  return (retRegion);
}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

unsigned long 
Rtm_RtmRegionManagerReadCounter(
  Rtm_RegionManager_t *rtmRegionManager
  ) 
{
  return(rtmRegionManager->regionCounter);
}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
unsigned long 
Rtm_RtmRegionManagerIncrementCounter(
  Rtm_RegionManager_t *rtmRegionManager
  ) 
{
  (rtmRegionManager->regionCounter)++;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

void
Rtm_MddPrintCubes(
  mdd_t *mdd,
  Rtm_Info_t *rtmInfo,
  int level
 )
{
  Sym_MddPrintCubes(rtmInfo->symInfo, mdd, level);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
RtmGetNvals(
  Rtm_Info_t *rtmInfo,
  int id
  )
{
  mvar_type x;
  array_t *mvar_list = mdd_ret_mvar_list(Sym_SymInfoReadManager(rtmInfo->symInfo));
  x = array_fetch(mvar_type, mvar_list, id);
  return(x.values);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
RtmGetTopMddId(
  mdd_t *m,
  Rtm_Info_t *rtmInfo)
{
  array_t * varidArray;
  int firstId;
  
  varidArray = mdd_get_support(Sym_SymInfoReadManager(rtmInfo->symInfo), m);
  if (array_n(varidArray) == 0){
    Main_MochaErrorPrint("Error: RtmGetTopMddId : mdd has empty support\n");
    exit(1);
  }

  firstId = (int)array_fetch( int, varidArray, 0);
  return(firstId);
}

/**Function********************************************************************

  Synopsis           [do reachability + invariant checking on timed modules]

  Description        [reachability + optionally invariant checking on timed modules]

  SideEffects        [state of invariants may be changed]

  SeeAlso            [Rtm_ModuleBuildTransitionRelation]


******************************************************************************/
mdd_t *
RtmModuleSearch(
  Mdl_Module_t *module,
  Rtm_Info_t *rtmInfo,
  boolean verbose,
  boolean checkInvariants,
  array_t *invNameArray,
  array_t *typedExprArray,
  Tcl_Interp *interp
  )
{
  mdd_t *initSet, *lowerBound, *upperBound, *reachSet, *toCareSet, *temp1,
      *temp2, *image, *onionRing;
  int step,i;
  int varid;
  mdd_t *var;
  array_t *histFreeIdArray    = array_alloc(int, 0);
  array_t *histDependIdArray  = array_alloc(int, 0);
  array_t *eventIdArray    = array_alloc(int, 0);
  array_t *invMddArray      = array_alloc(mdd_t *, 0);
  
  int *invStatusArray = 0;
  char *invName;
  st_generator *stgen;
  Atm_Expr_t *expr;
  mdd_t *exprMdd, *notInv;
  array_t *savedOnions = array_alloc(mdd_t *, 0);
  lsList varList;
  lsGen  gen;
  lsGeneric v;


  /* first create arrays of event, history free and history dependent
     variables: Note that any variable has to belong to one of these types.
     These arrays are useful when we want to existentially quantify out
     event and history free variables */ 

  varList = Mdl_ModuleObtainVariableList(module);
    lsForEachItem(varList, gen, v) {
      if(!Sym_SymInfoLookupUnprimedVariableId(rtmInfo->symInfo, (Var_Variable_t *)v, &varid )){
        Main_MochaErrorPrint( "Error: RtmSearchModule, cant find mdd id for var\n");
      }

      if(Var_VariableIsEvent((Var_Variable_t *) v)){
        array_insert_last(int, eventIdArray, varid);
      }
      else if(Var_VariableIsHistoryFree((Var_Variable_t *) v)){
        array_insert_last(int, histFreeIdArray, varid);
      }
      else if(Var_VariableIsHistoryDependent((Var_Variable_t *) v)){
        array_insert_last(int, histDependIdArray, varid);
      }
      else {
        Main_MochaErrorPrint( "Error: bad variable type : %s\n",
                Var_VariableReadName((Var_Variable_t *)v));
        exit(1);
      }
    }
    
  /* if invariants need to be checked, we will build an mdd for each */
  /* invariant. This mdd will represent the set of states that violate the */
  /* invariant. */
    
  if(checkInvariants) {
    for (i = 0; i < array_n(typedExprArray); i++){
      Atm_Expr_t *expr;
      expr = (Atm_Expr_t *) array_fetch(mdd_t *, typedExprArray, i);
      exprMdd = Sym_ExprBuildMdd(rtmInfo->symInfo, expr);
      notInv = mdd_not(exprMdd);
      array_insert_last(mdd_t *, invMddArray, notInv);
      mdd_free(exprMdd);
    }
    

    if(array_n(typedExprArray)){
      invStatusArray = ALLOC ( int , array_n(invNameArray));
      for (i = 0; i < array_n(invNameArray); i++){
        invStatusArray[i] = -1;
      }
    }
    
  }
  
  initSet = Rtm_ModuleBuildInitialRegion(rtmInfo,module);

  temp1 = mdd_smooth(Sym_SymInfoReadManager(rtmInfo->symInfo), initSet, histFreeIdArray);
  mdd_free(initSet);
   
  initSet = mdd_smooth(Sym_SymInfoReadManager(rtmInfo->symInfo), temp1, eventIdArray);
  mdd_free(temp1);


  Main_MochaPrint( "Initial Region Computed...\n");

  if(verbose)
    Rtm_MddPrintCubes(initSet, rtmInfo, 0);

  
  lowerBound = initSet;
  upperBound = mdd_dup(initSet);
  toCareSet  = mdd_not(initSet);
  reachSet   = mdd_dup(initSet);
  step       = 0;
  onionRing = mdd_dup(initSet);
  
  while (1) {

    /* check if any invariant has been violated */
    for (i = 0; i < array_n(invMddArray); i++){
      mdd_t *intersect;
      
      /* check if bug has already been found */
      if(invStatusArray[i] >= 0) continue;
      
      intersect = mdd_and ( onionRing,
                                   (mdd_t *) array_fetch(mdd_t *, invMddArray, i),
                                   1, 1);
      if(!mdd_is_tautology(intersect, 0)){
        Main_MochaPrint( "Invariant %s has been violated\n",
                (char *) array_fetch(char *, invNameArray, i));
        invStatusArray[i] = step;
      }
      mdd_free(intersect);
    }

    if(array_n(invMddArray)) {
      array_insert_last(mdd_t *, savedOnions, onionRing);
    }
    else{
      mdd_free(onionRing);
    }
    
    image = 
        Img_ImageInfoComputeFwdWithDomainVars(
          Sym_SymInfoReadImgInfo(rtmInfo->symInfo),
          lowerBound,
          upperBound,
          toCareSet);

    if(verbose){
      Main_MochaPrint( "\n ***** Printing image..\n");
      Rtm_MddPrintCubes(image, rtmInfo, 0);
    }

    onionRing = mdd_and(image, reachSet, 1, 0);

    if(verbose){
      Main_MochaPrint( "***** Printing onion ring..\n");
      Rtm_MddPrintCubes(onionRing , rtmInfo, 0);
    }
    step ++;
    mdd_free(lowerBound);
    mdd_free(upperBound);
    mdd_free(toCareSet);

    Main_MochaPrint( "Step %d: image mdd size = %10d\t |states| = %8g\t",
            step, mdd_size(image),  mdd_count_onset(Sym_SymInfoReadManager(rtmInfo->symInfo),
                                                    image,
                                                    Sym_SymInfoReadUnprimedIdArray(rtmInfo->symInfo)));
    mdd_free(image);

    if(mdd_is_tautology(onionRing, 0)){

      if(mdd_is_tautology(onionRing, 0))
         Main_MochaPrint( "\nDone reached set computation...\n");
      else
        Main_MochaPrint("\nStopping reachability after %d steps\n", step);
      
      Main_MochaPrint( "reached set mdd size = %10d\t number of states = %8g\n",
              mdd_size(reachSet),
              mdd_count_onset(Sym_SymInfoReadManager(rtmInfo->symInfo), reachSet,histDependIdArray));
      mdd_free(onionRing);
      break;
    }
    
    lowerBound = mdd_dup(onionRing);
    temp1  = mdd_or(reachSet, onionRing, 1, 1);
    mdd_free(reachSet);
    reachSet= temp1;

    if(verbose){
      Main_MochaPrint( "\n ***** Printing reachSet..\n");
      Rtm_MddPrintCubes(reachSet, rtmInfo, 0);
    }
    
    temp1 = mdd_smooth(Sym_SymInfoReadManager(rtmInfo->symInfo), reachSet, histFreeIdArray);
    mdd_free(reachSet);
    
    reachSet = mdd_smooth(Sym_SymInfoReadManager(rtmInfo->symInfo), temp1, eventIdArray);
    mdd_free(temp1);
    
    if(verbose){
      Main_MochaPrint( "\n ***** Printing reachSet after quantifying event and hist free vars..\n");
      Rtm_MddPrintCubes(reachSet, rtmInfo , 0);
    }
    
    Main_MochaPrint( "reached set mdd size = %10d\t |states| = %8g\n",
                     mdd_size(reachSet),
                     mdd_count_onset(Sym_SymInfoReadManager(rtmInfo->symInfo), reachSet,histDependIdArray));
    
    upperBound = mdd_dup(reachSet);
    toCareSet  = mdd_not(reachSet);
  }
  
  for (i = 0; i < array_n(invMddArray); i++){
    if(invStatusArray[i] < 0){
      Main_MochaPrint( "Invariant %s passed\n",
                       (char *) array_fetch(char *, invNameArray, i));
    }
    else{
      Main_MochaPrint( "Invariant %s failed in step %d\n",
                       (char *) array_fetch(char *, invNameArray, i), invStatusArray[i]);
      Rtm_DebugTracePrint(rtmInfo, savedOnions, invStatusArray[i],
                          (mdd_t *)array_fetch(mdd_t *, invMddArray, i),
                          eventIdArray, histDependIdArray,
                          (char *) array_fetch(char *, invNameArray, i),
                          Mdl_ModuleReadName(module), interp );
    }
  }
  
  array_free(histFreeIdArray);
  array_free(histDependIdArray);
  array_free(eventIdArray);
  
  for (i = 0; i < array_n(invMddArray); i++){
    mdd_free((mdd_t *) array_fetch(mdd_t *, invMddArray, i));
  }
  array_free(invMddArray);
  if(invStatusArray)
    FREE(invStatusArray);
  
  for (i = 0; i < array_n(savedOnions); i++){
    mdd_free((mdd_t *) array_fetch(mdd_t *, savedOnions, i));
  }
  array_free(savedOnions);
  
  return(reachSet);
}

/**Function********************************************************************

  Synopsis           [print debug trace for an invariant for timed modules]

  Description        [Given an array of MDDs (representing the onion ring),
                      the step at which the error occured,
		      the negation of the invariant that got violated,
		      arrays of event and history free ids of the module,
		      name of the invariant and module, this function
		      prints the debug trace.
		      It starts with the onion ring at the specified step and 
		      intersects it with the negation of the invariant. Then
		      it successivey does pre-image computations and finds 
		      a path to an initial state. Then it reverses the path and
		      and prints an error trace.
		     ]

  SideEffects        [none]

  SeeAlso            [RtmModuleSearch]

******************************************************************************/
void
Rtm_DebugTracePrint(
  Rtm_Info_t *rtmInfo,
  array_t *savedOnions,
  int step,
  mdd_t *notInv,
  array_t *eventIdArray,
  array_t *histDependIdArray,
  char *invName,
  char *moduleName,
  Tcl_Interp *interp
  )
{
  int curStep    = step;
  mdd_t *curOnion, *intersect;
  mdd_t *minterm, *backImage, *upperBound, *var, *temp ;
  mdd_t *oneMdd ;
  int varid;
  int i;
  char *curErrString;
  array_t *errorTrace = array_alloc(char *, 1);
  
  curOnion   = (mdd_t *) array_fetch(mdd_t *, savedOnions, step);
  intersect  =  mdd_and(curOnion, notInv, 1, 1);

  while(1){

    if(mdd_is_tautology(intersect, 0)){
    Main_MochaErrorPrint( "Error in printing debug trace\n");
    return;
    }

    minterm =  Rtm_MddPickMinterm(intersect, rtmInfo);
    /* if minterm is free of an event variable, set it to 0*/
    for ( i = 0 ; i < array_n(eventIdArray); i++){
      varid =  array_fetch(int,
                          eventIdArray, i);
      if(!RtmMddInTrueSupport(minterm, varid, rtmInfo)){
        var  = RtmGetMddLiteral(Sym_SymInfoReadManager(rtmInfo->symInfo),varid, 0);
        temp = mdd_and(minterm, var, 1, 1);
        mdd_free(minterm);
        minterm = temp;
      }
    }
    
    curErrString = ALLOC(char,1);
    *curErrString = '\0';
    
    Sym_MddPrintStateForErrorTrace(rtmInfo->symInfo, minterm, &curErrString);
    array_insert_last(char *, errorTrace, curErrString);
    
    mdd_free(intersect);
    if(curStep == 0) {
          mdd_free(minterm);
          break;
    }
    
  
    upperBound = mdd_dup(minterm);
    curStep--;
    curOnion   = (mdd_t *) array_fetch(mdd_t *, savedOnions, curStep);
  
    backImage = Img_ImageInfoComputeBwdWithDomainVars(
      Sym_SymInfoReadImgInfo(rtmInfo->symInfo),
      minterm,
      upperBound,
      curOnion);

    mdd_free(upperBound);
    mdd_free(minterm);
    intersect = mdd_and(backImage, curOnion, 1, 1);
    mdd_free(backImage);
  }

  fprintf(stdout, "Counterexample for invariant %s\n", invName);
  /* now print the error trace */
  for ( i = array_n(errorTrace) -1; i >= 0; i--){
    curErrString = (char *)array_fetch(char *, errorTrace, i);
    Main_MochaPrint("%s\n", curErrString);
    FREE(curErrString);
  }
  
  array_free(errorTrace);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
mdd_t  *
Rtm_MddPickMinterm(
  mdd_t *mdd,
  Rtm_Info_t *rtmInfo
  )
{
  mdd_t *one = mdd_one(Sym_SymInfoReadManager(rtmInfo->symInfo));
  int id, nvals, i;
  mdd_t *literal;
  mdd_t *rest, *result, *temp;
  
  if(mdd_is_tautology(mdd, 1)){
    return(one);
  }

  if(mdd_is_tautology(mdd, 0)){
    Main_MochaErrorPrint( "Error in Rtm_MddPickMinterm\n");
  }

  id = RtmGetTopMddId(mdd, rtmInfo);
  nvals = RtmGetNvals(rtmInfo, id); 

  for ( i =0; i < nvals; i++){
      literal = RtmGetMddLiteral(Sym_SymInfoReadManager(rtmInfo->symInfo),id,i);
      temp = mdd_cofactor(Sym_SymInfoReadManager(rtmInfo->symInfo), mdd, literal);
      if(!mdd_is_tautology(temp, 0)){
        rest = Rtm_MddPickMinterm(temp, rtmInfo);
        result = mdd_and(literal, rest, 1, 1);
        return(result);
      }
  }
  
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Order the MDD variables of a module as specified by a file]

  Description        [Read an ordering of MDD variables from a file, and order
                      the MDD variables of a module accordingly]
                      
  SideEffects        [none]

  SeeAlso            [CommandWriteOrder]

  CommandName        [rtm_static_order] 	   

  CommandSynopsis    [Orders mdd variables of a module as specified by a file]  

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
  Rtm_RegionManager_t *rtmRegionManager = (Rtm_RegionManager_t *)
      Main_ManagerReadRtmRegionManager((Main_Manager_t *) clientData); 
  mdd_manager *manager = (mdd_manager *)
      Main_ManagerReadMddManager((Main_Manager_t *) clientData);
  Mdl_Module_t *module;
  Rtm_Info_t *rtmInfo;
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
    if ((totalVarArray = RtmReadOrder(fp, module)) == NIL(array_t)) {
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
  
  rtmInfo = Rtm_RtmInfoAlloc(interp, module, manager);
  Rtm_RegionManagerAddModuleToRtmInfo( rtmRegionManager, module, rtmInfo);
  
  ModuleAssignMddIdsToVariables(module, rtmInfo->symInfo, totalVarArray);
  arrayForEachItem(VarOrder_t *, totalVarArray, i, varOrder) {
    FREE(varOrder);
  }
  array_free(totalVarArray);
  
  return TCL_OK;
  
  usage:
  Main_MochaErrorPrint( "Usage: rtm_static_order [-h] [-f] <file> <module>");
  return (TCL_ERROR);
}

/**Function********************************************************************

  Synopsis           [Computes the reached state set of a timed module]

  Description        [Compute reached set for a timed module using MDDs as
                      the representation. The transition relation must be built
                      using the command "rtm_trans" before this function is
                      called]

  SideEffects        [The MDD representing the set of reached states is added
                      to the rtmRegionTable associated with the module]

  SeeAlso            []

  CommandName        [rtm_search] 	   

  CommandSynopsis    [Performs symbolic (mdd based) state space search on a timed
                      module ]  


  CommandArguments   [\[-v\] \[-i\] \[-h\] &lt;module_name&gt;]  

  CommandDescription [The command performs MDD based symbolic  search on the state
  space of a timed module starting from its initial region. The rtm_trans command
  must be executed prior to  executing this command.
  

  Command Options:<p>

  <dl>

  <dt> -v
  <dd> verbose mode - prints MDDs associated with each intermediate step<p>

 
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
  Rtm_RegionManager_t *rtmRegionManager = (Rtm_RegionManager_t *)
      Main_ManagerReadRtmRegionManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *rtmRegionTable = Rtm_RegionManagerReadRegionTable(rtmRegionManager);
  Mdl_Module_t *module;
  Rtm_Info_t *rtmInfo;
  char *reachRegionName;
  mdd_t *reachSet;
  int flag, c;
  Tcl_HashEntry *entryPtr;
  
  boolean verbose = FALSE;
  boolean checkInvariants = FALSE;
  
  util_getopt_reset();

  while((c = util_getopt(argc, argv, "ivh")) != EOF) {
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

  rtmInfo = Rtm_RegionManagerObtainRtmInfoFromModule(rtmRegionManager, module);

  if(!rtmInfo){
    Main_MochaErrorPrint( "Please build transition relation first (use rtm_trans)\n");
    return(TCL_ERROR);
  }
  
  reachSet = RtmModuleSearch(module, rtmInfo, verbose, FALSE, 0, 0, interp);

  {
  Rtm_Region_t *reach = Rtm_AllocateRegionStruct(module, reachSet);
  unsigned long num = Rtm_RtmRegionManagerReadCounter(rtmRegionManager);
  long length = 1;
  char *id;
  char *temp;

  if(num > 9)
    length = (long) floor(log10(num)) + 1;
  id = ALLOC(char, length + 1);
  sprintf(id, "%d", num);
  Rtm_RtmRegionManagerIncrementCounter(rtmRegionManager);
  reachRegionName = util_strcat3(argv[util_optind], ".r", id);
  FREE(id);

  entryPtr = Tcl_CreateHashEntry(rtmRegionTable, reachRegionName, &flag);
  Tcl_SetHashValue(entryPtr, (ClientData) reach);
  Tcl_AppendElement(interp, reachRegionName);
  }

  return TCL_OK;
  
usage:
  Main_MochaErrorPrint( "Usage: rtm_search [-v] [-i] [-h] <module>");
  return TCL_ERROR;
}

/**Function********************************************************************

  Synopsis           [driver for building initial state of a timed module]

  Description        [Compute the mdd representing initial states of a module]

  SideEffects        [The initial region computed is added to the rtm region
                      manager's region table]

  SeeAlso            [CommandComputeReachedSet]

  CommandName        [rtm_init] 	   

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
  Rtm_RegionManager_t *rtmRegionManager = (Rtm_RegionManager_t *)
      Main_ManagerReadRtmRegionManager((Main_Manager_t *) clientData);
  Tcl_HashTable *rtmRegionTable = Rtm_RegionManagerReadRegionTable(rtmRegionManager);
  Mdl_Module_t *module;
  Rtm_Info_t *rtmInfo;
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
  
  rtmInfo = Rtm_RegionManagerObtainRtmInfoFromModule(rtmRegionManager, module);
  if(!rtmInfo){
    Main_MochaErrorPrint( "Please build transition relation first (use rtm_trans)\n");
    return(TCL_ERROR);
  }  

  initSet = Rtm_ModuleBuildInitialRegion(rtmInfo,module);
  Main_MochaPrint( "Initial Region Computed...\n");

  if(verbose) 
       Rtm_MddPrintCubes(initSet, rtmInfo, 0);
  
  {
  Rtm_Region_t *init = Rtm_AllocateRegionStruct(module, initSet);
  unsigned long num = Rtm_RtmRegionManagerReadCounter(rtmRegionManager);
  long length = 1;
  char *id;
  char *temp;

  if(num > 9)
    length = (long) floor(log10(num)) + 1;
  id = ALLOC(char, length + 1);
  sprintf(id, "%d", num);
  Rtm_RtmRegionManagerIncrementCounter(rtmRegionManager);
  initRegionName = util_strcat3(moduleName, ".r", id);
  FREE(id);

  entryPtr = Tcl_CreateHashEntry(rtmRegionTable, initRegionName, &flag);
  Tcl_SetHashValue(entryPtr, (ClientData) init);
  Tcl_AppendElement(interp, initRegionName);
  }
  
  return TCL_OK;

  usage:

  Main_MochaErrorPrint( "Usage rtm_init [-v] [-h] <module>");
  return (TCL_ERROR);
}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
CommandBuildTrans(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Module_t *module;
  Rtm_Info_t *rtmInfo;
  int i,c;

  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Rtm_RegionManager_t *rtmRegionManager = (Rtm_RegionManager_t *)
      Main_ManagerReadRtmRegionManager((Main_Manager_t *) clientData);
  mdd_manager *manager = (mdd_manager *)
      Main_ManagerReadMddManager((Main_Manager_t *) clientData);
  boolean verbose = FALSE;
  /* DIAG */
  lsList atomList;
  lsGen atmGen;
  Atm_Atom_t *atom;
  /* DIAG */

  
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

  /*
  atomList = Mdl_ModuleObtainSortedAtomList(module);

  lsForEachItem(atomList, atmGen, atom) {
    Main_MochaErrorPrint("Number of wait commands = %d \n",lsLength(Atm_AtomReadWaitCommandList((Atm_Atom_t *)atom)));
  }
  */

  rtmInfo = Rtm_RegionManagerObtainRtmInfoFromModule(rtmRegionManager, module);
  if (!rtmInfo) {
    char cmd[512];
    Main_MochaPrint("Ordering variables using rtm_static_order\n");
    sprintf(cmd, "rtm_static_order %s", Mdl_ModuleReadName(module));
    Tcl_Eval(interp, cmd);
    rtmInfo = Rtm_RegionManagerObtainRtmInfoFromModule(rtmRegionManager, module);
    if(!rtmInfo){
      Main_MochaErrorPrint("internal error: Cannot obtain rtmInfo for %s\n",
                           Mdl_ModuleReadName(module));
      return TCL_ERROR;
    }
  }

  if(array_n(Sym_SymInfoReadConjuncts(rtmInfo->symInfo)) > 0){
    Main_MochaErrorPrint("Transition relation for %s already exists\n",
                         Mdl_ModuleReadName(module));
    return TCL_ERROR;
  }
    
  Rtm_ModuleBuildTransitionRelation(rtmInfo, module);

  Main_MochaPrint( "Transition relation computed : %d conjuncts\n",
                   array_n(Sym_SymInfoReadConjuncts(rtmInfo->symInfo)));

  if(verbose){
    for ( i = 0; i < array_n(Sym_SymInfoReadConjuncts(rtmInfo->symInfo)); i++){
      Main_MochaPrint( "Printing Conjunct :  %d..\n", i);
      Rtm_MddPrintCubes( (mdd_t *) (array_fetch( mdd_t *,
                                                Sym_SymInfoReadConjuncts(rtmInfo->symInfo),
                                                i)),
                         rtmInfo,
                         0);
    }
  }

  Sym_SymInfoSetImgInfo(rtmInfo->symInfo,
                        Img_ImageInfoInitialize( 0,
                                                 Sym_SymInfoReadManager(rtmInfo->symInfo),
                                                 Sym_SymInfoReadUnprimedIdArray(rtmInfo->symInfo), 
                                                 Sym_SymInfoReadPrimedIdArray(rtmInfo->symInfo),  
                                                 Sym_SymInfoReadQuantifyIdArray(rtmInfo->symInfo),
                                                 Img_Iwls95_c,
                                                 Sym_SymInfoReadConjuncts(rtmInfo->symInfo),
                                                 Img_Both_c
                                                 )
                        );
  
  Main_MochaPrint( "Done initializing image info...\n");
  
  return TCL_OK;
  
usage:
   Main_MochaErrorPrint( "Usage: rtm_trans [-v] [-h] <module>");
   return TCL_ERROR;
}

/**Function********************************************************************

  Synopsis    [Converts a string to a dynamic ordering method type.]

  Description [Converts a string to a dynamic ordering method type. If string
  is not "sift" or "window", then returns BDD_REORDER_NONE.]

  SideEffects []

******************************************************************************/
static bdd_reorder_type_t
RtmStringConvertToDynOrderType(
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
RtmDynOrderTypeConvertToString(
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

  Synopsis    [Implements the rtm_dynamic_var_ordering command for the rtm
               package .]

  SideEffects []

  CommandName [rtm_dynamic_var_ordering]

  CommandSynopsis [control the application of dynamic variable ordering]

  CommandArguments [ \[-d\] \[-e &lt;method&gt;\] \[-f &lt;method&gt;\]
  \[-h\]]

  CommandDescription [Control the application of dynamic variable ordering. Dynamic ordering is a 
  technique to reorder the MDD variables to reduce the size of the existing MDDs.  When no options
  are specified, the current status of dynamic ordering is displayed.  At most one
  of the options -e, -f, and -d should be specified. 

  Dynamic ordering may be time consuming, but can often reduce the size of the
  MDDs dramatically.
  
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
  
  <b>The following methods are only available if MOCHA has been linked with the
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
CommandRtmDynamicVarOrdering(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
/* This command is copied directly from VIS code */
  
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
        dynOrderingMethod = RtmStringConvertToDynOrderType(util_optarg);
        if (dynOrderingMethod == BDD_REORDER_NONE) {
          Main_MochaErrorPrint("unknown method: %s\n", util_optarg);
          goto usage;
        }
        break;
      case 'e':
        enableFlag = TRUE;
        dynOrderingMethod = RtmStringConvertToDynOrderType(util_optarg);
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
  currentMethod = RtmDynamicVarOrderingMethod;

  /* If no options are given, then just display current status. */
  if (!(disableFlag || enableFlag || forceFlag)) {
    if (currentMethod == BDD_REORDER_NONE) {
      Main_MochaPrint("Dynamic variable ordering is disabled.\n");
    }
    else {
      Main_MochaPrint("Dynamic variable ordering is enabled ");
      Main_MochaPrint("with method %s.\n",
                     RtmDynOrderTypeConvertToString(currentMethod));
    }
  }

  if (disableFlag) {
    if (currentMethod == BDD_REORDER_NONE) {
      Main_MochaPrint("Dynamic variable ordering is already disabled.\n");
    }
    else {
      Main_MochaPrint("Dynamic variable ordering is disabled.\n");
      bdd_dynamic_reordering(mddManager, BDD_REORDER_NONE, BDD_REORDER_VERBOSITY_DEFAULT);
      RtmDynamicVarOrderingMethod = BDD_REORDER_NONE;
    }
  }

  /*
   * Set the dynamic ordering method of the network.  
   */
  if (enableFlag) {
    bdd_dynamic_reordering(mddManager, dynOrderingMethod, BDD_REORDER_VERBOSITY_DEFAULT);
    RtmDynamicVarOrderingMethod = dynOrderingMethod;
    Main_MochaPrint("Dynamic variable ordering is enabled ");
    Main_MochaPrint("with method %s.\n",
                   RtmDynOrderTypeConvertToString(dynOrderingMethod));
  }

  /*
   * Force a reordering.  Note that the mddManager has to have the method set
   * before calling bdd_reorder.  The value
   * of the ordering method is restored afterwards.
   */
  if (forceFlag) {
    Main_MochaPrint("Dynamic variable ordering forced ");
    Main_MochaPrint("with method %s....\n",
                   RtmDynOrderTypeConvertToString(dynOrderingMethod));
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

  Synopsis           [Convert an ordering of a subset of variables to a total 
                      ordering]

  Description        [Given a module and an array of variables (the order in which the 
                      variables are present in the array), pad remaining variables to the
		              end of the array]

  SideEffects        [Remaining variables are added to the end of the array]

  SeeAlso            [RtmReadOrder]

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
  mdd_manager *mddManager = Sym_SymInfoReadManager(symInfo);
  int i, mddId;
  long unprimedMddId, primedMddId;
  VarOrder_t *varOrder;
  Var_Variable_t *var;
  Var_Type_t *varType;
  Var_DataType dataType;
  st_generator *stgen;
  
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
      st_insert(Sym_SymInfoReadVarToUnprimedId(symInfo), (char *) var,
                (char*)(long) mddId);
      array_insert_last(int, Sym_SymInfoReadUnprimedIdArray(symInfo), mddId);
      st_insert(Sym_SymInfoReadIdToVar(symInfo), (char *)(long) mddId, (char *) var);
    }
    else {
      st_insert(Sym_SymInfoReadVarToPrimedId(symInfo), (char *)var,
                (char *) (long) mddId);
      array_insert_last(int, Sym_SymInfoReadPrimedIdArray(symInfo), mddId);
      st_insert(Sym_SymInfoReadIdToVar(symInfo), (char *)(long) mddId, (char *)var);
    }

    mdd_create_variables(mddManager, mvarValues, mvarNames, NIL(array_t));
    array_free(mvarValues);
    array_free(mvarNames);
  }

  st_foreach_item(Sym_SymInfoReadVarToUnprimedId(symInfo), stgen, (char **) &var, (char **)
                  &unprimedMddId) {
    st_lookup(Sym_SymInfoReadVarToPrimedId(symInfo), (char *) var, (char **) &primedMddId);
    
    if (unprimedMddId == primedMddId + 1)
      MddGroupVariables(mddManager, primedMddId, 2);
    else if (primedMddId == unprimedMddId + 1)
      MddGroupVariables(mddManager, unprimedMddId, 2);  
  }
}















