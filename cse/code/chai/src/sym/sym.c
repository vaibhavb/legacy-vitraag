/**CFile***********************************************************************

  FileName    [sym.c]

  PackageName [sym]

  Synopsis    []

  Description []

  SeeAlso     []

  Author      [Sriram K. Rajamani]

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
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [do invariant checking]

  Description        [Do invariant checking. This is the entry point
                      for the sym invariant engine to be called from
		      the inv package]

  SideEffects        []

  SeeAlso            [Sym_ModuleBuildTransitionRelation]


******************************************************************************/
void
Sym_ModulePerformBFS(
  Tcl_Interp *interp,
  Main_Manager_t *mainManager,
  Mdl_Module_t *module,
  array_t *invNameArray,
  array_t *typedExprArray
  )
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager(mainManager);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager(mainManager); 
  Tcl_HashTable *regionTable = Sym_RegionManagerReadRegionTable(regionManager);
  Sym_Info_t *symInfo;
  mdd_t *reachSet;
  int flag,c;
  Tcl_HashEntry *entryPtr;
  boolean verbose = FALSE;
  boolean checkInvariants = FALSE;
  int maxNumSteps = -1;

  symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);
  if(!symInfo){
    char cmd[512];
    Main_MochaPrint( "No sym_info.. building it(using sym_trans)\n");
    sprintf(cmd, "sym_trans %s", Mdl_ModuleReadName(module));
    Tcl_Eval(interp, cmd);
    symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);  
    if(!symInfo){
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

  
  reachSet = SymModuleSearch(module, symInfo, verbose, TRUE, invNameArray,
			     typedExprArray, interp, maxNumSteps);
  mdd_free(reachSet);
  return;
  
}

  
  



/**Function********************************************************************

  Synopsis           [print debug trace for an invariant]

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
		      running]

  SideEffects        [none]

  SeeAlso            [SymModuleSearch]

******************************************************************************/
void
Sym_DebugTracePrint(
  Sym_Info_t *symInfo,
  array_t *savedOnions,
  int step,
  mdd_t *notInv,
  array_t *eventIdArray,
  array_t *histDependIdArray,
  char *invName,
  char *moduleName
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

    minterm =  Sym_MddPickMinterm(symInfo, intersect);
    /* if minterm is free of an event variable, set it to 0*/
    for ( i = 0 ; i < array_n(eventIdArray); i++){
      varid =  array_fetch(int,
                          eventIdArray, i);
      if(!SymMddInTrueSupport(minterm, varid, symInfo)){
        var  = SymGetMddLiteral(symInfo->manager,varid, 0);
        temp = mdd_and(minterm, var, 1, 1);
        mdd_free(minterm);
        minterm = temp;
      }
    }
    

    /* if minterm is free of a history dependent variable, set it to 0  
    for ( i = 0 ; i < array_n(histDependIdArray); i++){
      varid =  array_fetch(int,
                          histDependIdArray, i);
      if(!SymMddInTrueSupport(minterm, varid, symInfo)){
        var  = SymGetMddLiteral(symInfo->manager,varid, 0);
        temp = mdd_and(minterm, var, 1, 1);
        mdd_free(minterm);
        minterm = temp;
      }
    }
    */

    curErrString = ALLOC(char,1);
    *curErrString = '\0';
    
    Sym_MddPrintStateForErrorTrace(symInfo, minterm, &curErrString);
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
      symInfo->imgInfo,
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

  Synopsis           [build symbolic transtion relation for a module]

  Description        [build symbloic transition relation for a module.
                      A conjunct is created for each atom and all conjuncts
		      are stored in an array]

  SideEffects        [transtion relation is build and stored inside symInfo]

  SeeAlso            [SymSearchModule]

******************************************************************************/
void
Sym_ModuleBuildTransitionRelation(
  Sym_Info_t *symInfo,
  Mdl_Module_t *module)
{
  lsList      atomList;
  lsGen atmGen;
  lsGeneric atom;
  mdd_t *atomMdd;

  atomList = Mdl_ModuleObtainSortedAtomList(module);
  lsForEachItem(atomList, atmGen, atom) {
    atomMdd = Sym_AtomBuildTransitionRelation(symInfo, (Atm_Atom_t *) atom);
    array_insert_last( mdd_t *, symInfo->transRelation->conjuncts, atomMdd);
    st_insert(symInfo->transRelation->atmToConjunct, (char *) atom, (char *) atomMdd);
  }

  lsDestroy(atomList, NULL);
}


  
/**Function********************************************************************

  Synopsis           [build mdd for initial region of module]

  Description        [build mdd for initial region of module]

  SideEffects        [none]

  SeeAlso            [Sym_ModuleBuildTransitionRelation]

******************************************************************************/
mdd_t *
Sym_ModuleBuildInitialRegion(
  Sym_Info_t *symInfo,
  Mdl_Module_t *module)
{

  mdd_t *initialState = mdd_one(symInfo->manager);
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
    mdd_t *atomInitState = Sym_AtomBuildInitialRegion(symInfo,(Atm_Atom_t*)atom);
    mdd_t *temp = mdd_and (atomInitState , initialState, 1, 1);
    mdd_free(initialState);
    mdd_free(atomInitState);
    initialState = temp;
  }

  /* make sure that initial state is independent of unprimed vars    */
  for ( i = 0 ; i < array_n(symInfo->unprimedIdArray); i++){
    varid = array_fetch(int, symInfo->unprimedIdArray, i);

    if(SymMddInTrueSupport(initialState, varid, symInfo)){
      Main_MochaErrorPrint(
              "Error: Initial region depends on unprimed var: %d \n",
              varid);
      exit(1);
    }

  }

  if(array_n(symInfo->primedIdArray) != array_n(symInfo->unprimedIdArray)){
    Main_MochaErrorPrint(
            "Error: Unprimed and Primed MDD  id arrays have unequal sizes\n");
    exit(1);
  }
  
  /* replace primed variables in initial state by unprimed  ones */
  initStateUnprimed =  mdd_substitute( symInfo->manager,
                                       initialState,
                                       symInfo->primedIdArray,
                                       symInfo->unprimedIdArray);
  mdd_free(initialState);
  lsDestroy(atomList, NULL);
  return(initStateUnprimed);
  
}

/**Function********************************************************************

  Synopsis           [build transition relation for an atom]

  Description        [build transition relation for an atom]

  SideEffects        [none]

  SeeAlso            [Sym_ModuleBuildTransitionRelation]

******************************************************************************/
mdd_t *
Sym_AtomBuildTransitionRelation(
  Sym_Info_t *symInfo,
  Atm_Atom_t *atom
  )
{
  lsList cmdList,assignList;
  Atm_Expr_t *guard;
  lsGen  cmdGen,gen;
  Atm_Cmd_t *cmd;
  mdd_t  *atomMdd, *guardMdd,  *assignmentMdd, *temp1, *temp2, *rhsMdd,
      *cmdMdd, *guardOrMdd, *icmdMdd, *tempCmd;
  Var_Variable_t *var;
  Atm_Assign_t *assign;
  int uid, pid;
  mdd_manager *manager = symInfo->manager;
  lsList ctrlVarList = Atm_AtomReadControlVarList(atom);
  lsList rdAwtVarList = Atm_AtomObtainReadAwaitVarList(atom);
  lsList readVarList = Atm_AtomReadReadVarList(atom);
  st_table *assignedVarTable, *readVarTable;


  readVarTable = st_init_table(st_ptrcmp, st_ptrhash);
  lsForEachItem(readVarList, gen, var){
    st_insert(readVarTable, (char *)var, NIL(char));
  };

  
  cmdList = Atm_AtomReadUpdateCommandList(atom);
  atomMdd = mdd_zero(manager);
  guardOrMdd = mdd_zero(manager);

  
  lsForEachItem(cmdList, cmdGen, cmd) {
    guard = Atm_CmdReadGuard(cmd);
    
    guardMdd = Sym_ExprBuildMdd(symInfo, guard );
   
    temp1 = mdd_or(guardOrMdd, guardMdd, 1, 1);
    mdd_free(guardOrMdd);
    guardOrMdd = temp1;

    assignList = Atm_CmdReadAssignList(cmd);
    assignedVarTable = st_init_table(st_ptrcmp, st_ptrhash);
    
    /* go through all the assignments */
    rhsMdd = mdd_one(manager);
    lsForEachItem(assignList, gen, assign) {
      Atm_Assign_t *typedAssign;

      assignmentMdd = Sym_AssignmentBuildMdd(symInfo, assign, assignedVarTable);

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
      
      if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, var, &uid )){
        Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
      }

      if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &pid )){
        Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
      }

      if(Var_VariableReadDataType( (Var_Variable_t *) var) == Var_Event_c){
        tempCmd = SymGetMddLiteral(symInfo->manager, pid, 0);
      }
      else {
        tempCmd = mdd_eq(manager, pid, uid);
      }
      
      temp1 = mdd_and(rhsMdd, tempCmd, 1, 1);
      mdd_free(tempCmd);
      mdd_free(rhsMdd);
      rhsMdd = temp1;
    }

    st_free_table(assignedVarTable);
    
    /* mdd for this cmd is obtained by
       (guard *  rhsmdd) */
    cmdMdd = mdd_and(guardMdd, rhsMdd, 1, 1);
    temp1  = mdd_or(atomMdd, cmdMdd, 1, 1);

    mdd_free(rhsMdd);
    mdd_free(guardMdd);
    mdd_free(cmdMdd);
    mdd_free(atomMdd);
    atomMdd = temp1;
  }

  cmd = Atm_AtomReadDefaultUpdateCommand(atom);
  if (cmd != NIL(Atm_Cmd_t)) {
    guardMdd = mdd_not(guardOrMdd);
   
    assignList = Atm_CmdReadAssignList(cmd);
    assignedVarTable = st_init_table(st_ptrcmp, st_ptrhash);
    
    /* go through all the assignments */
    rhsMdd = mdd_one(manager);
    lsForEachItem(assignList, gen, assign) {
      Atm_Assign_t *typedAssign;

      assignmentMdd = Sym_AssignmentBuildMdd(symInfo, assign, assignedVarTable);

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
      
      if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, var, &uid )){
        Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
      }

      if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &pid )){
        Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
      }

      if(Var_VariableReadDataType( (Var_Variable_t *) var) == Var_Event_c){
        tempCmd = SymGetMddLiteral(symInfo->manager, pid, 0);
      }
      else {
        tempCmd = mdd_eq(manager, pid, uid);
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
    atomMdd = temp1;

    /* make guardOrMdd the tautology */
    mdd_free(guardOrMdd);
    guardOrMdd = mdd_one(manager);
  }
  


  /* compute implicit command -- none of the read & controlled variables change! */ 
  icmdMdd = mdd_one(manager);
  lsForEachItem(ctrlVarList, gen, var){

    if(!st_lookup(readVarTable, (char *)var, NIL(char *)))
      continue;

    if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, var, &uid )){
      Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
    }

    if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &pid )){
      Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
    }

    if(Var_VariableReadDataType( (Var_Variable_t *) var) == Var_Event_c){
      tempCmd = SymGetMddLiteral(symInfo->manager, pid, 0);
    }
    else {
      tempCmd = mdd_eq(manager, pid, uid);
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
    mdd_t *iguardMdd = mdd_one(manager);
    lsForEachItem(rdAwtVarList, gen, var){
      if(Var_VariableReadDataType( (Var_Variable_t *) var) != Var_Event_c)
        continue;
      
      if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &pid )){
          Main_MochaErrorPrint( "Error: cant find mdd id for var\n");
        }
      temp1 = SymGetMddLiteral(symInfo->manager, pid, 0);
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
  lsDestroy(rdAwtVarList, NULL);
  return(atomMdd);
}

/**Function********************************************************************

  Synopsis           [build initial region for an atom]

  Description        [build intial region for atom]

  SideEffects        [none]

  SeeAlso            [Sym_AtomBuildTransitionRelation]

******************************************************************************/
mdd_t *
Sym_AtomBuildInitialRegion(
  Sym_Info_t *symInfo,			   
  Atm_Atom_t *atom)
{
  lsList cmdList,assignList;
  Atm_Expr_t *guard, *expr;
  lsGen  cmdGen,gen;
  Atm_Cmd_t *cmd;
  mdd_t *atomMdd, *guardMdd,  *assignmentMdd, *temp1, *temp2, *rhsMdd,
      *cmdMdd, *guardOrMdd;
  Var_Variable_t *var;
  Var_DataType varType;
  Atm_Assign_t *assign;
  int uid, pid;
  mdd_manager *manager = symInfo->manager;
  st_table *assignedVarTable;
  
  cmdList = Atm_AtomReadInitCommandList(atom);
  atomMdd = mdd_zero(manager);
  guardOrMdd = mdd_zero(manager);
  
  lsForEachItem(cmdList, cmdGen, cmd) {
    guard = Atm_CmdReadGuard(cmd);

    guardMdd = Sym_ExprBuildMdd(symInfo, guard );

    assignedVarTable = st_init_table(st_ptrcmp, st_ptrhash);
    
    temp1 = mdd_or(guardOrMdd, guardMdd, 1, 1);
    mdd_free(guardOrMdd);
    guardOrMdd = temp1;

    assignList = Atm_CmdReadAssignList(cmd);
    /* go through all the assignments */
    rhsMdd = mdd_one(manager);
    lsForEachItem(assignList, gen, assign) {
      Atm_Assign_t *typedAssign;

      assignmentMdd = Sym_AssignmentBuildMdd(symInfo, assign,
                                             assignedVarTable);
      
      temp1 = mdd_and(rhsMdd, assignmentMdd, 1, 1);
      mdd_free(assignmentMdd);
      mdd_free(rhsMdd);
      rhsMdd = temp1;
    }

    st_free_table(assignedVarTable);
    
    /* mdd for this cmd is obtained by (guard *  rhsmdd) */
    
    cmdMdd = mdd_and(guardMdd, rhsMdd, 1, 1);
    temp1  = mdd_or(atomMdd, cmdMdd, 1, 1);

    mdd_free(rhsMdd);
    mdd_free(guardMdd);
    mdd_free(cmdMdd);
    mdd_free(atomMdd);
    atomMdd = temp1;
  }

  cmd = Atm_AtomReadDefaultInitCommand(atom);
  if (cmd != NIL(Atm_Cmd_t)) {
    assignList = Atm_CmdReadAssignList(cmd);
    guardMdd = mdd_not(guardOrMdd);
    assignedVarTable = st_init_table(st_ptrcmp, st_ptrhash);
    
    rhsMdd = mdd_one(manager);
    lsForEachItem(assignList, gen, assign) {
      Atm_Assign_t *typedAssign;

      assignmentMdd = Sym_AssignmentBuildMdd(symInfo, assign,
                                             assignedVarTable);
      
      temp1 = mdd_and(rhsMdd, assignmentMdd, 1, 1);
      mdd_free(assignmentMdd);
      mdd_free(rhsMdd);
      rhsMdd = temp1;
    }

    st_free_table(assignedVarTable);
    
    /* mdd for this cmd is obtained by (guard *  rhsmdd) */
    
    cmdMdd = mdd_and(guardMdd, rhsMdd, 1, 1);
    temp1  = mdd_or(atomMdd, cmdMdd, 1, 1);

    mdd_free(rhsMdd);
    mdd_free(guardMdd);
    mdd_free(cmdMdd);
    mdd_free(atomMdd);
    atomMdd = temp1;
  }
  else {
    /* add implicit guard to atomMdd */
    temp1 = mdd_or(guardOrMdd, atomMdd, 0, 1);
    mdd_free(guardOrMdd);
    mdd_free(atomMdd);
    atomMdd = temp1;
  }
  
  return(atomMdd);
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [do reachability + invariant checking]

  Description        [reachability + optionally invariant checking]

  SideEffects        [state of invariants may be changed]

  SeeAlso            [Sym_ModuleBuildTransitionRelation]


******************************************************************************/
mdd_t *
SymModuleSearch(
  Mdl_Module_t *module,
  Sym_Info_t *symInfo,
  boolean verbose,
  boolean checkInvariants,
  array_t *invNameArray,
  array_t *typedExprArray,
  Tcl_Interp *interp,
  int maxNumSteps
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
     if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, (Var_Variable_t *)v, &varid )){
        Main_MochaErrorPrint( "Error: SymSearchModule, cant find mdd id for var\n");
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

  lsDestroy(varList, NULL);
  
  /* if invariants need to be checked, we will build an mdd for each */
  /* invariant. This mdd will represent the set of states that violate the */
  /* invariant. */
    
  if(checkInvariants) {
    for (i = 0; i < array_n(typedExprArray); i++){
      Atm_Expr_t *expr;
      expr = (Atm_Expr_t *) array_fetch(mdd_t *, typedExprArray, i);
      exprMdd = Sym_ExprBuildMdd(symInfo, expr);
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
  
  initSet = Sym_ModuleBuildInitialRegion(symInfo,module);
  onionRing = mdd_dup(initSet);

  temp1 = mdd_smooth(symInfo->manager, initSet, histFreeIdArray);
  mdd_free(initSet);
   
  initSet = mdd_smooth(symInfo->manager, temp1, eventIdArray);
  mdd_free(temp1);


  Main_MochaPrint( "Initial Region Computed...\n");

  if(verbose)
    Sym_MddPrintCubes(symInfo, initSet , 0);

  
  lowerBound = initSet;
  upperBound = mdd_dup(initSet);
  toCareSet  = mdd_not(initSet);
  reachSet   = mdd_dup(initSet);
  step       = 0;
  
  while (1) {
    boolean violation = FALSE;
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
	violation = TRUE;
      }
      mdd_free(intersect);
    }

    if(array_n(invMddArray)) {
      array_insert_last(mdd_t *, savedOnions, onionRing);
    }
    else{
      mdd_free(onionRing);
    }

    if(violation){
      mdd_free(lowerBound);
      mdd_free(upperBound);
      mdd_free(toCareSet);
      break;
    }
    
    image = 
        Img_ImageInfoComputeFwdWithDomainVars(
          symInfo->imgInfo,
          lowerBound,
          upperBound,
          toCareSet);

    if(verbose){
      Main_MochaPrint( "\n ***** Printing image..\n");
      Sym_MddPrintCubes(symInfo, image , 0);
    }

    onionRing = mdd_and(image, reachSet, 1, 0);

    if(verbose){
      Main_MochaPrint( "***** Printing onion ring..\n");
      Sym_MddPrintCubes(symInfo, onionRing , 0);
    }
    step ++;
    mdd_free(lowerBound);
    mdd_free(upperBound);
    mdd_free(toCareSet);

    Main_MochaPrint( "Step %d: image mdd size = %10d\t |states| = %8g\t",
            step, mdd_size(image),  mdd_count_onset(symInfo->manager, image, symInfo->unprimedIdArray));
    mdd_free(image);

    if(((maxNumSteps > 0) && (step >= maxNumSteps)) ||
       mdd_is_tautology(onionRing, 0)){

      if(mdd_is_tautology(onionRing, 0))
         Main_MochaPrint( "\nDone reached set computation...\n");
      else
        Main_MochaPrint("\nStopping reachability after %d steps\n", step);
      
      Main_MochaPrint( "reached set mdd size = %10d\t number of states = %8g\n",
              mdd_size(reachSet),
              mdd_count_onset(symInfo->manager, reachSet,histDependIdArray));
      mdd_free(onionRing);
      break;
    }
    
    lowerBound = mdd_dup(onionRing);
    temp1  = mdd_or(reachSet, onionRing, 1, 1);
    mdd_free(reachSet);
    reachSet= temp1;

    if(verbose){
      Main_MochaPrint( "\n ***** Printing reachSet..\n");
      Sym_MddPrintCubes(symInfo, reachSet , 0);
    }
    
    temp1 = mdd_smooth(symInfo->manager, reachSet, histFreeIdArray);
    mdd_free(reachSet);
    
    reachSet = mdd_smooth(symInfo->manager, temp1, eventIdArray);
    mdd_free(temp1);
    
    if(verbose){
      Main_MochaPrint( "\n ***** Printing reachSet after quantifying event and hist free vars..\n");
      Sym_MddPrintCubes(symInfo, reachSet , 0);
    }
    
    Main_MochaPrint( "reached set mdd size = %10d\t |states| = %8g\n",
                     mdd_size(reachSet),
                     mdd_count_onset(symInfo->manager, reachSet,histDependIdArray));
    
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
      Sym_DebugTracePrint(symInfo, savedOnions, invStatusArray[i],
                          (mdd_t *)array_fetch(mdd_t *, invMddArray, i),
                          eventIdArray, histDependIdArray,
                          (char *) array_fetch(char *, invNameArray, i),
                          Mdl_ModuleReadName(module) );
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


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis    [Group all bdd vars corresponding to mdd vars initMddId to
  initMddId + (blockSize-1) in a block which will not be split in reordering.]

  Description [Group all bdd vars corresponding to mdd vars initMddId to
  initMddId + (blockSize-1) in a block which will not be reordered internally.
  Ths bdd's corresponding to these mdd's should be contiguous; if not the
  function will fail.]

  SideEffects []

******************************************************************************/
void
MddGroupVariables(
  mdd_manager *mddMgr,
  int initMddId,
  int blockSize )
{
  int i, j;
  int length;
  int aIndex;
  int startingBddIndex;
  int sanityCheck;
  mvar_type initMv, anMv;
  bvar_type initBvar, aBvar;
  array_t *mvar_list, *bvar_list;
  bdd_t *bdd;
  
  mvar_list = mdd_ret_mvar_list(mddMgr);
  bvar_list = mdd_ret_bvar_list(mddMgr);

  /*
   * mvar for initMddId 
   */
  initMv = array_fetch(mvar_type, mvar_list, initMddId);

  /*
   * bvar for first bdd for initMddId 
   */
  initBvar = mdd_ret_bvar(&initMv, 0, bvar_list);

  /*
   * number of bdd variables to group 
   */
  length = 0;

  /*
   * startingBddIndex is the level of the first bdd variable 
   */
  startingBddIndex = bdd_top_var_level( mddMgr, initBvar.node );
  sanityCheck = startingBddIndex;

  /*
   * in this loop we are simply ensuring that the bdd variables
   * corresponding to the mdd vars are infact contiguous. If this
   * is not the case we fail. length is the total number of bdd variables
   * to be grouped.
   */
  for( i = 0; i < blockSize; i++) {
    anMv = array_fetch(mvar_type, mvar_list, ( initMddId + i ) );
    for ( j = 0 ; j < anMv.encode_length; j++ ) {

      aBvar = mdd_ret_bvar( & anMv, j, bvar_list );
      aIndex = bdd_top_var_level( mddMgr,  aBvar.node );

      if ( sanityCheck != aIndex ) {
        /* bdd vars are not contiguous!! */
        fprintf(stderr, "Badly formed block - bdd vars for %s (mvar_id = %d) are not contiguous!!\n",
                             anMv.name, anMv.mvar_id );
        fail("Wont go on\n");
      }
      else {
        /* number of variables to group increased by one */
        sanityCheck++;
        length++;
      }
    }
  }

  bdd = bdd_var_with_index(mddMgr, startingBddIndex);
  (void) bdd_new_var_block(bdd, length);
  bdd_free(bdd);
}

