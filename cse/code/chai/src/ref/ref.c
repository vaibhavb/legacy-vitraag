/**CFile***********************************************************************

  FileName    [ref.c]

  PackageName [ref]

  Synopsis    [Routines to check if one module is a refinement of another]

  Description [This package has routines that check if one module refines
                another.
		1. refinement check is provided via the check_refine
		   command. It requires that all history dependent 
		   variables of the specification be witnessed in the 
		   implementation
		2. simulation check is provided via the check_simulate
		   command.
		3. compositonal refinement facility is provided by the
		   check_refine_atom command. This command helps the 
		   user carry out one proof oligation in a compositional 
		   proof. ]


  SeeAlso     [sym]

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

#include  "refInt.h"

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

  Synopsis           [Init function for tcl.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int Ref_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Tcl_CreateCommand(interp, "check_refine", RefCheckNohiddenRefinement,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "check_refine_atom",  RefCheckCompositionalRefinementOneAtom,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "check_simulation",  RefCheckSimulation,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);

  /*
  Tcl_CreateCommand(interp, "ref_test",  TestFunction,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
   */                    
  return TCL_OK;
}

 
/**Function********************************************************************

  Synopsis           [Reinit function]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Ref_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [End function]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Ref_End(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  return TCL_OK;
}


/**Function********************************************************************
  Synopsys              [do one step of the compositional refinement proof]

  Description           [Given implementation, specification and and atom atom1 in implementation,
                       construct new modules newimplementation and newspecification
                       such that
                          1. new implementation contains atom1 only
                          2. new specification contains heuristically chosen atoms
                             from specification and implementation that control variables
                             controlled by atom1, but do not include atom1 itself]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Ref_CheckAtomRefinement(
  Mdl_Manager_t *mdlManager,
  Mdl_Module_t * spec,
  Atm_Atom_t *specAtom,
  Mdl_Module_t *impl,
  lsList varNameList,
  lsList forceVarNameList,
  Tcl_Interp *interp,
  boolean checkRef,
  boolean verbose,
  boolean keepIntermediate,
  int    impBoundary,
  boolean checkAtEnd,
  char *orderFileName)
{
  char cmd[512];
  char addMsg[512];
  boolean result = FALSE;
  lsList emptyList = lsCreate();
  lsList specAtomList       = lsCreate();
  lsList implAtomList       = lsCreate();
  Mdl_Module_t *newSpecModule = 0;
  Mdl_Module_t *newImplModule = 0;
  
  lsList newSpecExtVarList, newSpecCtrlVarList;
  lsList specExtVarList, implExtVarList;
  lsList pendingImplCtrlVarList, doneImplCtrlVarList;
  lsGen varGen, gen;
  lsGeneric v;
  char *varName;
  
  specExtVarList = Mdl_ModuleGetExternalVariableList(spec);
  implExtVarList = Mdl_ModuleGetExternalVariableList(impl);

  /* create new module with just spec atom and all variables visible (null  privateVarList) */
  lsNewEnd(specAtomList, (lsGeneric)specAtom, LS_NH);
  newSpecModule = Mdl_ModuleCreateFromAtoms(specAtomList, emptyList);
  newSpecExtVarList =  Mdl_ModuleGetExternalVariableList(newSpecModule);
  newSpecCtrlVarList =  Mdl_ModuleGetInterfaceVariableList(newSpecModule);

  Main_MochaPrint("New specification module %s created\n", Mdl_ModuleReadName(newSpecModule));
  Mdl_ModuleAddToManager(newSpecModule, mdlManager);

  /* create a list of variables for which choosing newImplementation atoms is pending */
  pendingImplCtrlVarList = lsCopy(newSpecCtrlVarList, 0);

/* add all external variables also to pending implcntrl vars */
  if(impBoundary >= 2  ){
    lsForEachItem(newSpecExtVarList, varGen, v){
      varName = Var_VariableReadName((Var_Variable_t *)v);
      /* if it is an external variable of both the new and old specifications,
	 skip it*/
      if(Ref_LookupVarList(specExtVarList,varName) &&
	 Ref_LookupVarList(implExtVarList,varName))
	continue;

      lsNewEnd(pendingImplCtrlVarList, v, LS_NH);
    }
  }

  
  doneImplCtrlVarList    = lsCreate();
  
  while(lsLength(pendingImplCtrlVarList) > 0){
    Var_Variable_t *var;
    char *varName;
    Atm_Atom_t *newAtom;
    lsList newPendingVars;
    
    lsDelBegin(pendingImplCtrlVarList, &v);
    lsNewEnd(doneImplCtrlVarList, v, LS_NH);
    
    varName = Var_VariableReadName((Var_Variable_t *)v);
    var     = Mdl_ModuleReadVariableFromName(varName, spec);
    newAtom =  (Atm_Atom_t *)Var_VariableReadControlAtom(var);
    
    if(!newAtom || Ref_IsMemberOfAtomList(specAtomList, newAtom) ||
       Ref_IsMemberOfNameList(varNameList, varName)) {
      Var_Variable_t *newVar = Mdl_ModuleReadVariableFromName(varName, impl);
      newAtom = 0;
      if(newVar)
        newAtom = (Atm_Atom_t *)Var_VariableReadControlAtom(newVar);
      if(!newAtom){
        Main_MochaErrorPrint("Cant find atom controlling variable %s in"
                             "module %s", varName, Mdl_ModuleReadName(impl));
        goto done;
      }
      else {
        sprintf(addMsg, "Adding IMPLEMENTATION atom %s\n",
                        Atm_AtomReadName(newAtom));
      }
    }
    else {
      sprintf(addMsg, "Adding SPECIFICATION  atom %s\n",
                      Atm_AtomReadName(newAtom));
    }

    /* add new atom to implementation atom list */
    if(!Ref_IsMemberOfAtomList(implAtomList, newAtom)){
      lsNewEnd(implAtomList, (lsGeneric) newAtom, LS_NH);
      Main_MochaPrint("%s\n", addMsg);
    }
    else
      continue;

    /* remove other variables controlled by this atom from
       pendingImplCntrlVarList */
    {
      lsList cleanedPendingList = lsCreate();
      lsList ctrlVars = Atm_AtomReadControlVarList(newAtom);
      
      lsForEachItem(pendingImplCtrlVarList, varGen, v){
        var = (Var_Variable_t *)v;
        varName = Var_VariableReadName(var);
        if(Ref_LookupVarList(ctrlVars, varName)){
	  #ifdef DEBUG_REF
	  Main_MochaPrint("Deleting %s from pending vars\n", varName);
	  #endif
	}
	else
	  lsNewEnd(cleanedPendingList, v, LS_NH);
      }

      lsDestroy(pendingImplCtrlVarList, 0);
      pendingImplCtrlVarList = cleanedPendingList;

      /* add new control variables to doneImplCrelVarList */
      lsForEachItem(ctrlVars, varGen, v){
        var = (Var_Variable_t *)v;
        varName = Var_VariableReadName(var);
        if(!Ref_LookupVarList(doneImplCtrlVarList, varName)){
	  #ifdef DEBUG_REF
	  Main_MochaPrint("Adding %s to done vars\n", varName);
	  #endif
	  lsNewEnd(doneImplCtrlVarList, v, LS_NH);
	}
      }
    }

    if(impBoundary >= 1){
      newPendingVars = Atm_AtomObtainReadOrAwaitVarList(newAtom);
      lsForEachItem(newPendingVars, varGen, v){
	var = (Var_Variable_t *)v;
	varName = Var_VariableReadName(var);

	/* if small implementation, sufficient to stop at new spec boundary */
	if( (impBoundary == 1) && Ref_LookupVarList(newSpecExtVarList, varName))
	  continue;

	/* if it is already in done or pending variable lists skip it */
	if(Ref_LookupVarList(doneImplCtrlVarList, varName))
	  continue;
	if(Ref_LookupVarList(pendingImplCtrlVarList, varName))
	  continue;

	/* if it is an external variable of new specification atom, skip it! 
	   if(Ref_LookupVarList(newSpecExtVarList, varName))
	   continue; */

	/* if it is an external variable of both the new and old specifications,
         skip it*/
	if(Ref_LookupVarList(specExtVarList,varName) &&
	   Ref_LookupVarList(implExtVarList,varName))
	  continue;

	/* otherwise add it to pending variables */
	lsNewEnd(pendingImplCtrlVarList, v, LS_NH);
      }
      lsDestroy(newPendingVars, 0);
    }
  }

  /* add the atoms that are forced to be present */
  lsForEachItem(forceVarNameList, gen, varName){
    Atm_Atom_t *newAtom;
    Var_Variable_t *var;
    lsList ctrlVars;

    if(Ref_LookupVarList(doneImplCtrlVarList, varName))
      continue;

    if(Ref_LookupVarList(specExtVarList,varName) &&
       Ref_LookupVarList(implExtVarList,varName))
      continue;
    
    var     = Mdl_ModuleReadVariableFromName(varName, spec);
    newAtom =  (Atm_Atom_t *)Var_VariableReadControlAtom(var);
    
    if(!newAtom || Ref_IsMemberOfAtomList(specAtomList, newAtom) ||
       Ref_IsMemberOfNameList(varNameList, varName)) {
      Var_Variable_t *newVar = Mdl_ModuleReadVariableFromName(varName, impl);
      newAtom = 0;
      if(newVar)
        newAtom = (Atm_Atom_t *)Var_VariableReadControlAtom(newVar);
      if(!newAtom){
        Main_MochaErrorPrint("Cant find atom controlling variable %s in"
                             "module %s", varName, Mdl_ModuleReadName(impl));
        goto done;
      }
      else {
        sprintf(addMsg, "FORCE: Adding IMPLEMENTATION atom %s\n",
                        Atm_AtomReadName(newAtom));
      }
    }
    else {
      sprintf(addMsg, "FORCE: Adding SPECIFICATION  atom %s\n",
                      Atm_AtomReadName(newAtom));
    }

    /* add new atom to implementation atom list */
    if(!Ref_IsMemberOfAtomList(implAtomList, newAtom)){
      lsNewEnd(implAtomList, (lsGeneric) newAtom, LS_NH);
      Main_MochaPrint("%s\n", addMsg);
      
      ctrlVars = Atm_AtomReadControlVarList(newAtom);
      /* add new control variables to doneImplCtrlVarList */
      lsForEachItem(ctrlVars, varGen, v){
        var = (Var_Variable_t *)v;
        varName = Var_VariableReadName(var);
        if(!Ref_LookupVarList(doneImplCtrlVarList, varName)){
	  #ifdef DEBUG_REF
	  Main_MochaPrint("Adding %s to done vars\n", varName);
	  #endif
	  lsNewEnd(doneImplCtrlVarList, v, LS_NH);
	}
      }
    }
    
  } /* end forcing loop */
      

  newImplModule = Mdl_ModuleCreateFromAtoms(implAtomList, emptyList);
  Main_MochaPrint("New Implementation module %s created\n", Mdl_ModuleReadName(newImplModule));
  Mdl_ModuleAddToManager(newImplModule, mdlManager);

  
  if(checkRef){
    char orderCmd[512];
    orderCmd[0] = '\0';
    if(orderFileName)
      sprintf(orderCmd, "-o %s", orderFileName );
    sprintf(cmd, "check_refine %s %s %s %s %s",
	    ( verbose ? "-v" : " "),  orderCmd,
	    (checkAtEnd ? "-e" : " "),  Mdl_ModuleReadName(newImplModule),
	    Mdl_ModuleReadName(newSpecModule));
    result = (Tcl_Eval(interp, cmd) == TCL_OK);
  }
  else
    result = TRUE;

  done:

  if(!keepIntermediate){
    Main_MochaPrint("Deleting intermediate specification and implementation\n");
    if(newSpecModule)
      Mdl_ModuleRemoveFromManager(Mdl_ModuleReadName(newSpecModule),mdlManager);
    if(newImplModule)
      Mdl_ModuleRemoveFromManager(Mdl_ModuleReadName(newImplModule),mdlManager);
  }
  else
    Main_MochaPrint("Keeping intermediate specification and implementation\n");


  lsDestroy(emptyList, 0);
  lsDestroy(specAtomList, 0);
  lsDestroy(implAtomList, 0);
  lsDestroy(pendingImplCtrlVarList, 0);
  lsDestroy(doneImplCtrlVarList, 0);

  return(result);
}

/**Function********************************************************************

  Synopsis           [Check if implModule is a refinement of specModule
                      with the assumption that all history dependent
                      variables of specModule are also present in implModule]

  Description        [Currently all variables of specModule need to be present in implModule
  
  The algorithm for doing this is as follows:
  -- First check if the initial states of specModule are contained in implModule
     If not FAIL
  
   -- do reachability of implModule
       During each stage of reachability 
             1. restrict trans relation of implModule to states in the frontier (ie build (frontier & implTrans))
             2. If restricted implementation trans does NOT imply specification trans, then FAIL and print error trace

   -- If reachability completes, PASS
   ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Ref_CheckNohiddenRefinement(
  Mdl_Module_t * implModule,
  Sym_Info_t *implSymInfo,
  Mdl_Module_t *specModule,
  Sym_Info_t *specSymInfo,
  boolean verbose,
  boolean checkAtEnd)
{

  mdd_manager *manager = Sym_SymInfoReadManager(implSymInfo);
  boolean result = FALSE;

  mdd_t *specFlatTrans = 0;  

  array_t *specIdArr = array_alloc(int, 0);
  array_t *implIdArr = array_alloc(int, 0);
  array_t *implEventIdArr = array_alloc(int, 0);
  array_t *implHistFreeIdArr = array_alloc(int, 0);
  array_t *implHistDependIdArr = array_alloc(int, 0);
  array_t *implTransQuantifyIdArr = array_alloc(int, 0);

  lsList  specAllVars = Mdl_ModuleObtainVariableList(specModule);
  lsList  implAllVars = Mdl_ModuleObtainVariableList(implModule);
  lsGen   gen;
  lsGeneric v;
  mdd_t *initSet1, *initSet2, *temp;


  if(!Ref_CheckImplCompatibility(specModule, implModule)){
    Main_MochaPrint("check_refine: Failed syntactic check\n");
    goto done;
  }

  specFlatTrans = RefGetFlatSpecTransRelation(specModule, specSymInfo, implModule, implSymInfo,
					  specIdArr, implIdArr);
  if(verbose){
    Main_MochaPrint("Transition relation of specModule after substitution");
    Sym_MddPrintCubes(implSymInfo, specFlatTrans, 0);
  }


  /* gather variables to be quantified during transition  invariant check*/
  lsForEachItem(implAllVars, gen, v) {
    int uVarid;
    int pVarid;

    if(!Sym_SymInfoLookupUnprimedVariableId(implSymInfo, (Var_Variable_t *)v, &uVarid )){
      Main_MochaErrorPrint( "Error: ref: cant find mdd id for var\n");
    }
    
    if(!Sym_SymInfoLookupPrimedVariableId(implSymInfo, (Var_Variable_t *)v, &pVarid )){
      Main_MochaErrorPrint( "Error: ref: cant find mdd id for var\n");
    }
    
    if(Var_VariableIsEvent((Var_Variable_t *) v)){
      array_insert_last(int, implEventIdArr, uVarid);
    }
    else if(Var_VariableIsHistoryFree((Var_Variable_t *) v)){
      array_insert_last(int, implHistFreeIdArr, uVarid);
    }
    else if(Var_VariableIsHistoryDependent((Var_Variable_t *) v)){
      array_insert_last(int, implHistDependIdArr, uVarid);
    }
    else {
      Main_MochaErrorPrint( "Error: bad variable type : %s\n",
                            Var_VariableReadName((Var_Variable_t *)v));
      exit(1);
    }

    if(!Ref_LookupVarList(specAllVars, Var_VariableReadName((Var_Variable_t *)v))){
      array_insert_last(int, implTransQuantifyIdArr, uVarid);
      array_insert_last(int, implTransQuantifyIdArr, pVarid);
    }
  }


  /* check if there is inclusion of initial states */
  initSet1 = Sym_ModuleBuildInitialRegion(implSymInfo,implModule);
  Main_MochaPrint( "Initial Region Computed For %s\n",  Mdl_ModuleReadName(implModule));  

  initSet2 = Sym_ModuleBuildInitialRegion(specSymInfo,specModule);
  Main_MochaPrint( "Initial Region Computed For %s\n",  Mdl_ModuleReadName(specModule));
  
    /* replace all variables of initSet2 in terms of variables in implModule */
  temp= mdd_substitute(manager, initSet2, specIdArr, implIdArr);
  mdd_free(initSet2);
  initSet2 = temp;

  temp = mdd_and(initSet1, initSet2, 1, 0);

  if(!mdd_is_tautology(temp, 0)){
    array_t *errorMddArr = array_alloc(mdd_t *, 0);
    array_insert_last(mdd_t *, errorMddArr, temp);
    
    Main_MochaPrint("Initial states dont match up\n");
    Sym_DebugTracePrint(
        implSymInfo,
	errorMddArr,
        array_n(errorMddArr)-1,
	mdd_one(manager), /*pseudo invariant */
        implEventIdArr,
        implHistDependIdArr,
        "Refinement Check",
        Mdl_ModuleReadName(implModule));

    array_free(errorMddArr);
    result = FALSE;
    goto done;
  }

  result = RefCheckTransitionInvariant(implModule, implSymInfo, specFlatTrans, initSet1,
                                       implEventIdArr, implHistFreeIdArr,
                                       implHistDependIdArr,
                                       implTransQuantifyIdArr, verbose, checkAtEnd);
  mdd_free(initSet1);
  mdd_free(initSet2);
  
  done:
  if(specFlatTrans)
    mdd_free(specFlatTrans);
  array_free(specIdArr);
  array_free(implIdArr);
  array_free(implEventIdArr);
  array_free(implHistFreeIdArr);
  array_free(implHistDependIdArr);
  array_free(implTransQuantifyIdArr);

  return(result);
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/



/**Function********************************************************************

  Synopsis           [Check refinement using transition invariant check on the
  implementation]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
RefCheckTransitionInvariant(
  Mdl_Module_t *module,
  Sym_Info_t *symInfo,
  mdd_t *transInv,
  mdd_t *initSet,
  array_t *eventIdArray,
  array_t *histFreeIdArray,
  array_t *histDependIdArray,
  array_t *transQuanIdArr,
  boolean verbose,
  boolean checkAtEnd)
{
  st_table *varToUnprimedId = Sym_SymInfoReadVarToUnprimedId(symInfo);
  array_t *unprimedIdArray = Sym_SymInfoReadUnprimedIdArray(symInfo);
  array_t *primedIdArray   = Sym_SymInfoReadPrimedIdArray(symInfo);
  Img_ImageInfo_t *imgInfo = Sym_SymInfoReadImgInfo(symInfo);
  boolean result = FALSE; 
  mdd_manager *manager = Sym_SymInfoReadManager(symInfo);
  mdd_t * lowerBound = mdd_dup(initSet);
  mdd_t * upperBound = mdd_dup(initSet);
  mdd_t *toCareSet  = mdd_not(initSet);
  mdd_t *reachSet   = mdd_dup(initSet);
  int step       = 0;
  mdd_t *onionRing = mdd_dup(initSet);
  array_t *savedOnions = array_alloc(mdd_t *, 0);
  mdd_t *image, *temp1, *temp2;
  array_t *conjuncts = Sym_SymInfoReadConjuncts(symInfo);
  
  while (1) {

    if(!checkAtEnd){
      Main_MochaPrint("Doing transition invariant check..\n");
      result = RefCheckTransitionInvariantAtGivenSet(
                       symInfo, onionRing, transQuanIdArr,
		       conjuncts, savedOnions, eventIdArray,
		       histDependIdArray, transInv, module);
      Main_MochaPrint("Done..\n");
      if(!result) 
	goto done;
    }
    /*save the onion ring */
    array_insert_last(mdd_t *, savedOnions, onionRing);
    
    image = 
        Img_ImageInfoComputeFwdWithDomainVars(
          imgInfo,
          lowerBound,
          upperBound,
          toCareSet);

    if(verbose){
      Main_MochaPrint( "\n ***** Printing image..\n");
      Sym_MddPrintCubes(symInfo, image, 0);
    }

    onionRing = mdd_and(image, reachSet, 1, 0);

    if(verbose){
      Main_MochaPrint( "***** Printing onion ring..\n");
      Sym_MddPrintCubes(symInfo, onionRing, 0);
    }
    step ++;
    mdd_free(lowerBound);
    mdd_free(upperBound);
    mdd_free(toCareSet);

    Main_MochaPrint( "Step %d: image mdd size = %10d\t |states| = %8g\t",
            step, mdd_size(image),  mdd_count_onset(manager, image,
                                                    unprimedIdArray));
    mdd_free(image);

    if(mdd_is_tautology(onionRing, 0)){
      Main_MochaPrint( "\nDone reached set computation...\n");
      Main_MochaPrint( "reached set mdd size = %10d\t number of states = %8g\n",
              mdd_size(reachSet),
              mdd_count_onset(manager, reachSet,histDependIdArray));

      if(checkAtEnd){
        Main_MochaPrint("Doing transition invariant check..\n");
	result = RefCheckTransitionInvariantAtGivenSet(
                       symInfo, reachSet, transQuanIdArr,
		       conjuncts, savedOnions, eventIdArray,
		       histDependIdArray, transInv, module);
	Main_MochaPrint("Done..\n");
      }
     else
       result = TRUE;
      break;
    }

    lowerBound = mdd_dup(onionRing);
    temp1  = mdd_or(reachSet, onionRing, 1, 1);
    mdd_free(reachSet);
    reachSet= temp1;

    if(verbose){
      Main_MochaPrint( "\n ***** Printing reachSet..\n");
      Sym_MddPrintCubes(symInfo, reachSet, 0);
    }

    temp1 = mdd_smooth(manager, reachSet, histFreeIdArray);
    mdd_free(reachSet);
    
    reachSet = mdd_smooth(manager, temp1, eventIdArray);
    mdd_free(temp1);

    if(verbose){
      Main_MochaPrint( "\n ***** Printing reachSet after quantifying event and hist free vars..\n");
      Sym_MddPrintCubes(symInfo, reachSet, 0);
    }
    
    Main_MochaPrint( "reached set mdd size = %10d\t |states| = %8g\n",
            mdd_size(reachSet),
            mdd_count_onset(manager, reachSet,histDependIdArray));
    
    upperBound = mdd_dup(reachSet);
    toCareSet  = mdd_not(reachSet);
  }

  
  done:
  {
    int i;
    for (i = 0; i < array_n(savedOnions); i++){
      mdd_free((mdd_t *) array_fetch(mdd_t *, savedOnions, i));
    }
    array_free(savedOnions);
    mdd_free(reachSet);
    return(result);
  }
  
}


boolean RefCheckTransitionInvariantAtGivenSet(
  Sym_Info_t *symInfo,
  mdd_t *setOfStates,
  array_t *transQuanIdArr,
  array_t *conjuncts,
  array_t *savedOnions,
  array_t *eventIdArray,
  array_t *histDependIdArray,
  mdd_t * transInv,
  Mdl_Module_t *module)
{
    array_t *unprimedIdArray = Sym_SymInfoReadUnprimedIdArray(symInfo);
    array_t *primedIdArray   = Sym_SymInfoReadPrimedIdArray(symInfo);
    Img_ImageInfo_t *imgInfo = Sym_SymInfoReadImgInfo(symInfo);
    /* check if states in onion ring satisfy the refinement condition */
    array_t *andSmoothArray = array_alloc(mdd_t *, 0);
    array_t *nullArray = array_alloc(mdd_t *, 0);
    int i;
    mdd_t *temp1, *temp2, *image;
    boolean result;
    mdd_manager *manager = Sym_SymInfoReadManager(symInfo);
    
    array_insert_last(mdd_t *, andSmoothArray, setOfStates);
    for (i = 0; i < array_n(conjuncts); i++){
      temp1 =  (mdd_t *) array_fetch(mdd_t *, conjuncts, i);
      array_insert_last(mdd_t *, andSmoothArray, temp1);
    }
    
    temp1 = Img_MultiwayLinearAndSmooth(manager,
                                        andSmoothArray,
                                        transQuanIdArr,
                                        nullArray);
    temp2 = mdd_and(temp1, transInv , 1, 0);
    mdd_free(temp1);

    array_free(andSmoothArray);

    #ifdef DUMB_TRANSITION_INVARIANT
    int i;
    temp1 = mdd_dup(setOfStates);
    for ( i = 0; i < array_n(conjuncts); i++){
      temp2 = mdd_and(temp1, (mdd_t *) array_fetch(mdd_t *, conjuncts, i), 1,
                      1);
      mdd_free(temp1);
      temp1 = temp2;
    }
    
    temp2 = mdd_and(temp1, transInv , 1, 0);
    mdd_free(temp1);
    #endif
    
    if(!mdd_is_tautology(temp2, 0)){
      mdd_t *errorTrans, *ps, *ns;
      Main_MochaPrint("Transition invariant is not satisfied\n");
      errorTrans = Sym_MddPickMinterm(symInfo, temp2);
      ps = mdd_smooth(manager, errorTrans, primedIdArray);
      ns = mdd_smooth(manager, errorTrans, unprimedIdArray);
     
      /*intersect ps with the nth onion ring */
      temp1 = mdd_and(ps, setOfStates, 1, 1);
      mdd_free(ps);
      ps = temp1;

      /* replace last onion ring with present state - ps */
      array_insert_last(mdd_t *, savedOnions, ps);

      /* get ns in terms of unprimedIdVariables */
      temp1 = mdd_substitute(manager, ns, primedIdArray, unprimedIdArray);
      mdd_free(ns);
      ns = temp1;

      /* to ge sensible valued of next state implementation vars do anothe
         rimage computation */
      image = 
        Img_ImageInfoComputeFwdWithDomainVars(
          imgInfo,
          ps,  ps, ns);

      temp1 = mdd_and(image, ns, 1, 1);
      array_insert_last(mdd_t *, savedOnions, temp1);
      
      result = FALSE;
      
      Sym_DebugTracePrint(
        symInfo,
        savedOnions,
        array_n(savedOnions)-1,
        ns, /* pseudo invariant! */
        eventIdArray,
        histDependIdArray,
        "Refinement Check",
        Mdl_ModuleReadName(module));

      mdd_free(ns);

      return(FALSE);
    }
    return(TRUE);
}


/**Function********************************************************************

  Synopsis           [driver for checking refinement with no hidden variables
  in the specification]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [check_refine] 	   

  CommandSynopsis    [Performs symbolic (mdd based) refinement check. Assumes
  that all history dependent variables of specModule are present in implModule]

  CommandArguments   [ \[-h\] [\-o &lt;fname&gt;\] \[-v\]  &lt;module_name1&gt; &lt;module_name2&gt;]    

  CommandDescription [
  

  Command Options:<p>

  <dt> -h
  <dd> print usage <p>

  <dt> -o <fname>
  <dd> write out the mdd variable ordering in file specified by <fname> <p>

  <dt> -v
  <dd> verbose mode - prints MDDs associated with each intermediate step<p>


  </dl>]
******************************************************************************/
int
RefCheckNohiddenRefinement(
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
  Mdl_Module_t *implModule, *specModule;
  Sym_Info_t *implSymInfo, *specSymInfo;
  boolean result;
  int retVal;
  boolean verbose = FALSE;
  boolean checkAtEnd = FALSE;
  char *fileName = NIL(char);
  int c;
  
  util_getopt_reset();

  while((c = util_getopt(argc, argv, "o:vh")) != EOF) {
    switch(c) {
        case 'v' :
          verbose = TRUE;
          break;
       case 'o':
	 fileName = util_optarg;
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
  
  if((implModule = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }

  util_optind++;
  if (argc - util_optind == 0) {
    goto usage;
  }
  if((specModule = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }
  


  implSymInfo = Sym_RegionManagerReadSymInfo(regionManager, implModule);
  if(!implSymInfo){
      char cmd[512];
	
      if(fileName){
	Main_MochaPrint("Ordering variables from %s\n", fileName);
	sprintf(cmd, "sym_static_order -f %s %s", fileName, Mdl_ModuleReadName(implModule));
	Tcl_Eval(interp, cmd);
      }

      Main_MochaPrint("Building transition relation for module %s\n",
                      Mdl_ModuleReadName(implModule));
      sprintf(cmd, "sym_trans %s", Mdl_ModuleReadName(implModule));
      Tcl_Eval(interp, cmd);
      implSymInfo = Sym_RegionManagerReadSymInfo(regionManager, implModule);  
      if(!implSymInfo){
	Main_MochaErrorPrint( "Command: %s unsuccessful\n", cmd);
	return(TCL_ERROR);
      }

      Main_MochaPrint("Writing order into imporder.dat\n");
      sprintf(cmd, "sym_write_order -f /tmp/imporder.dat %s", Mdl_ModuleReadName(implModule));
      Tcl_Eval(interp, cmd);
  }

  specSymInfo = Sym_RegionManagerReadSymInfo(regionManager, specModule);
  if(!specSymInfo){
      char cmd[512];
      Main_MochaPrint("Building transition relation for module %s\n", Mdl_ModuleReadName(specModule));
      sprintf(cmd, "sym_trans %s", Mdl_ModuleReadName(specModule));
      Tcl_Eval(interp, cmd);
      specSymInfo = Sym_RegionManagerReadSymInfo(regionManager, specModule);  
      if(!specSymInfo){
	Main_MochaErrorPrint( "Command: %s unsuccessful\n", cmd);
	return(TCL_ERROR);
      }
  }

  {
    /* first turn on dynamic variable ordering */
    char cmd[512];
    sprintf(cmd, "sym_dynamic_var_ordering -e sift");
    Tcl_Eval(interp, cmd);
  }

  result = Ref_CheckNohiddenRefinement(implModule, implSymInfo, specModule,
                                       specSymInfo, verbose, checkAtEnd);


  if(result){
    Main_MochaPrint("%s refines %s\n",
                    Mdl_ModuleReadName(implModule), Mdl_ModuleReadName(specModule));
    retVal = TCL_OK;
  }
  else{
    Main_MochaPrint("%s does not refine %s\n",
                    Mdl_ModuleReadName(implModule), Mdl_ModuleReadName(specModule));
    retVal = TCL_ERROR;
  }


  Main_MochaPrint("Freeing syminfo for both specification and implementation\n");
  Sym_RegionManagerDeleteModuleToSymInfo(regionManager, implModule);
  Sym_RegionManagerDeleteModuleToSymInfo(regionManager, specModule);
  Sym_SymInfoFree(implSymInfo);
  Sym_SymInfoFree(specSymInfo);

  return(retVal);

usage:
  Main_MochaErrorPrint( "Usage: check_refine [-h] [-o <filename>] [-v]  <implModule> <specModule>\n");
  return TCL_ERROR;
}



/**Function********************************************************************


  Synopsis           [driver for checking one step in the compositional refinement proof]

  Description        [optional]

  SideEffects        [trnasition relations for new modules will be built if -r option is used]

  SeeAlso            [optional]

  CommandName        [check_refine_atom] 	   

  CommandSynopsis   [Do one step in the compositional refinement proof of "implementation refines
  specification"]


  CommandArguments   [\[-e\] \[-f &lt;size or varlist&gt; \] \[-h\] \[-i &lt;varlist&gt; \] \[-k\] \[-o &lt;fname&gt;\] \[-r\] \[-v\] &lt;impl&gt; &lt;spec&gt; &lt;spec_interface_varname&gt;]    

  CommandDescription [  Do one step in the compositional refinement proof of "implementation refines
  specification". The step done corresponds to the atom that controls variable named spec_interface_varname
  in the specification (let us call this atom atom1). The command does the following: <p>

  Given an implementation, specification modules and and an interface variable in 
  the specification that controls and atom (say atom1),
  construct new modules "new specification" and "new implementation"
  such that
  1. new specification contains atom1 only.
  2. new implementation contains heuristically chosen atoms
  from specification and implementation that control variables
  controlled by atom1, but do not include atom1 itself <p>
			     
  A refinement check between the new specification and the new implementation is performed 
  First, new specification and implementation modules are created. The new 
  specification contains just  atom1. The new implementation contains chosen atoms
  from specification and implementation that control variables
  controlled by atom1, but do not include atom1 itself. At this point the 
  new implementation contains only "essential atoms" to do a refinement check. <p>

  Further atoms may need to be added to the new implementaion to
  constrain its environment.  There are two ways to do this:<p>

     1. AUTOMATIC:  If the "-f 0" (size) option is chosen, then the
     implementation module is not grown any further. For the "-f 1" and
     "-f 2" options, progressively larger "constraining" environments are
     chosen for the new implementation module. (The default if no explicit
     "-f" option is used is "-f 2") <p>

     2. MANUAL: The user can also FORCE specific variables to be controlled in the
     new implementation by using simply supplying a list of variables
     in the -f option (example -f {var1 var2}). <p>

  Preference is always given to choose atoms from the specification for the 
  new implementation whenever possible. However, the "-i" option can be 
  used to choose specific atoms from the implelementation. <p>

  NOTE 1 : Since atom names are mangled by MOCHA (to uniquify atom names during
  parallel composition), we identify an atom by any variable that is controlled
  by the atom.<p>

  NOTE  2 : List of variables are specified by enclosing them in curly braces
  Example {var1 var2}<p>

  NOTE 3: You also have to use curly braces if your variable name has array indices
  Example {foo2\[0\]\[1\]}<p>

  EXAMPLES:<p>
  
  Suppose the specification module is "Spec" and implementation module is "Impl".
  To do the sub-proof corresponding to a specific atom in the Spec that controls
  variable "myVar":
     check_refine_atom Impl Spec myVar<p>
     
  To force variables foo1 and foo2\[0\] to be controlled use:
     check_refine_atom -f {foo1 foo2\[0\]} Impl Spec myVar<p>

  In addition, to force foo2\[0\] to be constrained by an atom from Impl, use:
     check_refine_atom -f {foo1 foo2\[0\]} -i {foo2\[0\]} Impl Spec myVar <p>


  Command Options:<p>

  <dl>
  <dt> -e
  <dd> do transition invariant check only at the end (after completing reachability)   <p>

  <dt> -f 0|1|2|var_name_list
  <dd> 0 --> only the "essential atoms" are chosen
  <dd> 1 --> chooses a bigger set of atoms than size=0. Guaranteed to be comparable
  <dd>            with the new specification 
  <dd> 2 --> heuristically chooses a bigger set of atoms than size=1 <p>
  <dd> var_name_list -> forces atoms controlling variables in  var_name_list to be chosen in the new implementation <p>

  <dt> -h
  <dd> print usage <p>

  <dt> -i var_name_list
  <dd> if the atom controlling variable in var_name_list is chosen, choose it  from the implementation <p>

  <dt> -k
  <dd> keep the new modules (for debugging purposes: default behavior if you do not use this option is to delete them) <p>

  <dt> -o fname
  <dd> write out the mdd variable ordering in file specified by <fname> <p>
  
  <dt> -r
  <dd> Do not do refinement check on the new modules (for debugging purposes) <p>

  <dd> verbose mode <p>

  </dl>]
******************************************************************************/
int
RefCheckCompositionalRefinementOneAtom(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager((Main_Manager_t *) clientData);
  mdd_manager *oldManager = (mdd_manager *)
      Main_ManagerReadMddManager((Main_Manager_t *) clientData);

  Mdl_Module_t *impl, *spec;
  Atm_Atom_t  *specAtom;
  Var_Variable_t *var;
  int result;
  boolean verbose = FALSE;
  boolean doRefine = TRUE;
  boolean keepIntermediate = FALSE;
  boolean checkAtEnd    = FALSE;
  int  impBoundary = 2;
  int c, i;
  int listArgc;
  char **listArgv;
  lsList varNameList = lsCreate();
  lsList forceVarNameList = lsCreate();
  char *name;
  lsGen lsgen;
  mdd_manager *newManager;
  char *fileName = NIL(char);
  
  
  util_getopt_reset();

  while((c = util_getopt(argc, argv, "i:f:o:krveh")) != EOF) {
    switch(c) {
        case 'i':
        {
          if (Tcl_SplitList(interp, util_optarg, &listArgc, &listArgv) != TCL_OK)
            return TCL_ERROR;
          for (i = 0; i < listArgc; i++)
            lsNewEnd(varNameList, util_strsav(listArgv[i]), LS_NH);
          Tcl_Free((char *) listArgv);
          break;
        }


        case 'f':
        {
	  if((util_optarg[0] >= '0') && (util_optarg[0] <= '2'))
	    impBoundary = atoi(util_optarg);
	  else{
	    /* assume that you have a list */
	    impBoundary = 0;
	    if (Tcl_SplitList(interp, util_optarg, &listArgc, &listArgv) != TCL_OK)
	      return TCL_ERROR;
	    for (i = 0; i < listArgc; i++)
	      lsNewEnd(forceVarNameList, util_strsav(listArgv[i]), LS_NH);
	    Tcl_Free((char *) listArgv);
	  }
	  break;
	}

       case 'o':
	 fileName = util_optarg;
	 break;

        case 'k':
        {
          keepIntermediate = TRUE;
          break;
        }
        
        case 'r':
        {
	  doRefine = FALSE;
	  break;
        }
        
        case 'v' :
        {
          verbose = TRUE;
          break;
        }

        case 'e':
        {
	  checkAtEnd = TRUE;
          break;
        }

        case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }

  if (argc - util_optind == 0) {
    goto usage;
  }
 
  if((impl = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }

  util_optind++;
  if (argc - util_optind == 0) {
    goto usage;
  }
  
  
  if((spec = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }

  util_optind++;
  if (argc - util_optind == 0) {
    goto usage;
  }
  
  var = Mdl_ModuleReadVariableFromName(argv[util_optind], spec);
  if(var)
    specAtom = (Atm_Atom_t *)Ref_VariableReadControlAtom(var);
  
  if(!specAtom){
    Main_MochaErrorPrint("Atom controlling variable %s not found in specification\n", argv[util_optind]);
    return(TCL_ERROR);
  }


  /* set up new mdd manager for refinement */
  newManager = mdd_init_empty();
  Main_ManagerSetMddManager((Main_Manager_t *) clientData,
                            (Main_MddManagerGeneric) newManager);
  
  result = Ref_CheckAtomRefinement(mdlManager, spec, specAtom, impl,
                                   varNameList, forceVarNameList, interp, doRefine, verbose,
                                   keepIntermediate, impBoundary, checkAtEnd, fileName);

  /* revert back to the old manager   */
  mdd_quit(newManager);
  Main_ManagerSetMddManager((Main_Manager_t *) clientData,
                            (Main_MddManagerGeneric) oldManager);
  
  lsForEachItem(varNameList, lsgen, name) {
    FREE(name);
  }
  lsDestroy(varNameList, NULL);


  if(doRefine){
    if(result){
      Main_MochaPrint("Compositional refinement step successful\n");
    }
    else{
      Main_MochaPrint("Compositional refinement step failed\n");
    }
  }

  return(result ? TCL_OK : TCL_ERROR);
  
usage:
  
  Main_MochaErrorPrint( "Usage: check_refine_atom"
			"[-e]  [-f 0|1|2|<var_name_list>] [-h] [-i <var_name_list>] [-k]  [-o <ord_file_name> ]"
			"[-r] [-v] <impl_module> <spec_module> <spec_var_name>\n");
  return TCL_ERROR;
}


/**Function********************************************************************

  Synopsis           [dump and array of mdd Ids for debugging]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void RefDumpIds(
  array_t *a)
{
  int i;
  int id;
  
  for ( i = 0; i < array_n(a); i++){
    id = array_fetch(int, a, i);
    printf( " %d: (%d) ", i, id);
  }
  printf("\n");
}


/**Function********************************************************************

  Synopsis           [get flattened transition relation for specification in terms
                      of variables of implementation]

  Description        [optional]

  SideEffects        [specIdArray and implId array will be filled with matching mddIds
                      from specification and implementation]

  SeeAlso            [optional]

******************************************************************************/
mdd_t *
RefGetFlatSpecTransRelation(
  Mdl_Module_t *specModule,
  Sym_Info_t *specSymInfo,
  Mdl_Module_t * implModule,
  Sym_Info_t *implSymInfo,
  array_t *specIdArray,
  array_t *implIdArray)
{
  lsList  specAllVars = Mdl_ModuleObtainVariableList(specModule);
  lsList  implAllVars = Mdl_ModuleObtainVariableList(implModule);
  lsGen gen;
  lsGeneric v;
  array_t *quantifyIdArray = array_alloc(int, 0);
  array_t *nullIdArray = array_alloc(int, 0);
  mdd_t *flatTrans, *temp;
  

  lsForEachItem(specAllVars, gen, v) {
    int suVarid;
    int spVarid;
    Var_Variable_t *var = (Var_Variable_t *)v;
    Var_Variable_t *ivar;

    if(!Sym_SymInfoLookupUnprimedVariableId(specSymInfo, var, &suVarid )){
      Main_MochaErrorPrint( "Error: ref: cant find mdd id for var\n");
      goto done;
    }
    
    if(!Sym_SymInfoLookupPrimedVariableId(specSymInfo, var, &spVarid )){
      Main_MochaErrorPrint( "Error: ref: cant find mdd id for var\n");
      goto done;
    }

    ivar = Ref_LookupVarList( implAllVars, Var_VariableReadName(var));
    if(!ivar){
      if(Var_VariableIsEvent(var) ||  Var_VariableIsHistoryFree(var)){
	array_insert_last(int, quantifyIdArray, spVarid);
      }
      else{
	Main_MochaErrorPrint("History Dependent specification variable %s"
			     " is not found in implementation\n",
                             Var_VariableReadName(var));
        goto done;
      }
    }
    else{
      int  iuVarid, ipVarid;
      if(!Sym_SymInfoLookupUnprimedVariableId(implSymInfo, ivar, &iuVarid )){
	Main_MochaErrorPrint( "Error: ref: cant find mdd id for var\n");
        goto done;
      }
    
      if(!Sym_SymInfoLookupPrimedVariableId(implSymInfo, ivar, &ipVarid )){
	Main_MochaErrorPrint( "Error: ref: cant find mdd id for var\n");
        goto done;
      }

      array_insert_last(int, specIdArray, suVarid);
      array_insert_last(int, implIdArray, iuVarid);
      
      array_insert_last(int, specIdArray, spVarid);
      array_insert_last(int, implIdArray, ipVarid);
    }
  }
  
  flatTrans = Img_MultiwayLinearAndSmooth(Sym_SymInfoReadManager(specSymInfo),
					  Sym_SymInfoReadConjuncts(specSymInfo),
					  quantifyIdArray,
					  nullIdArray);
  temp = mdd_substitute(Sym_SymInfoReadManager(specSymInfo), flatTrans, specIdArray, implIdArray);
  mdd_free(flatTrans);
  flatTrans = temp;

  done:
  lsDestroy(specAllVars, 0);
  lsDestroy(implAllVars, 0);
  array_free(quantifyIdArray);
  array_free(nullIdArray);
  return(flatTrans);
}

/**Function********************************************************************

  Synopsis           [Ref_VariableReadControlAtom]

  Description        [Given a variable, get its controlling atom.
                      This differs from Var_VariableReadControllingAtom
		      in that it also works for array and bit-vector variables.
		      In the case of arrays and bit-vectors (or arrays of bit vectors)
		      the atom controlling the first element is returned.]

  SideEffects        [ ]

  SeeAlso            [Var_VariableReadControlAtom]

******************************************************************************/
Atm_Atom_t *
Ref_VariableReadControlAtom(
Var_Variable_t *var)
{
  Mdl_Module_t *module = (Mdl_Module_t *) Var_VariableReadModule(var);
  Var_Type_t *varType = Var_VariableReadType(var);
  char *varName = Var_VariableReadName(var);
  Var_DataType varDataType = Var_VarTypeReadDataType(varType);
  char *actualVarName;
  Var_Variable_t *actualVar;

  if (Var_TypeIsArray(varType)) {
    Var_Type_t *indexType = Var_VarTypeReadIndexType(varType);
    int indexIsEnum = 0;
    
    indexIsEnum = Var_TypeIsEnum(indexType);
    
    if (!indexIsEnum)
      actualVarName = ALLOC(char, strlen(varName) + 1 +
                            INDEX_STRING_LENGTH + 1 + 1);
                
    if (indexIsEnum)
      actualVarName = util_strcat4(varName, "[", Var_EnumElementReadName(
          Var_VarTypeReadSymbolicValueFromIndex(indexType, 0)), "]");
    else
      sprintf(actualVarName, "%s[0]", varName);

    actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
    FREE(actualVarName);

  }
  
  else if (Var_TypeIsBV(varType)) {
    actualVarName = ALLOC(char, strlen(varName) + 1 +
                          INDEX_STRING_LENGTH + 1 + 1);
                
    sprintf(actualVarName, "%s[0]", varName);
    actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
    FREE(actualVarName);
  }
  else {
    return((Atm_Atom_t *)Var_VariableReadControlAtom(var));
  }

  if(!actualVar) 
    return(0);
  else
    {
      Atm_Atom_t *atm = Ref_VariableReadControlAtom(actualVar);
      return(atm);
    }
}
      




/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
static int
TestFunction(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  int listArgc;
  char **listArgv;
  int i;
  
  if (Tcl_SplitList(interp, argv[1], &listArgc, &listArgv) != TCL_OK) {
    printf("bug!!\n");
    return TCL_ERROR;
  }

  for (i = 0; i < listArgc; i++)
    printf("%s\n", listArgv[i]);

  free((char *) listArgv);

  return TCL_OK;
}



