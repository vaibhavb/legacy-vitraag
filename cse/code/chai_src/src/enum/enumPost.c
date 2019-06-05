/**CFile***********************************************************************

  FileName    [enumPost.c]

  PackageName [enum]

  Synopsis    [This file contains the functions used in implementing the
  commands enum_init and enum_post.]

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
#define INDEX_STRING_LENGTH 50

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

static boolean IsInList(lsList list, lsGeneric item);
static lsList AssignListRemoveNondetAssignment(lsList assignList, lsList nondetVarList);
static void VariablesAddRecursively(lsList varList, Var_Variable_t *var);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Returns the history dependent variables.]

  SideEffects        [None]

******************************************************************************/
Var_Variable_t **
Enum_VarInfoReadHistDependVars(
  Enum_VarInfo_t *varInfo)
{
  return varInfo->histDependVars;
}

/**Function********************************************************************

  Synopsis           [Returns the history free variables.]

  SideEffects        [None]

******************************************************************************/
Var_Variable_t **
Enum_VarInfoReadHistFreeVars(
  Enum_VarInfo_t *varInfo)
{
  return varInfo->histFreeVars;
}

/**Function********************************************************************

  Synopsis           [Returns the event variables.]

  SideEffects        [None]

******************************************************************************/
Var_Variable_t **
Enum_VarInfoReadEventVars(
  Enum_VarInfo_t *varInfo)
{
  return varInfo->eventVars;
}

/**Function********************************************************************

  Synopsis           [Returns the number of history dependent variables.]

  SideEffects        [None]

******************************************************************************/
int
Enum_VarInfoReadNumHistDependVars(
  Enum_VarInfo_t *varInfo)
{
  return varInfo->numHistDependVars;
}

/**Function********************************************************************

  Synopsis           [Returns the number of history free variables.]

  SideEffects        [None]

******************************************************************************/
int
Enum_VarInfoReadNumHistFreeVars(
  Enum_VarInfo_t *varInfo)
{
  return varInfo->numHistFreeVars;
}

/**Function********************************************************************

  Synopsis           [Returns the number of event variables.]

  SideEffects        [None]

******************************************************************************/
int
Enum_VarInfoReadNumEventVars(
  Enum_VarInfo_t *varInfo)
{
  return varInfo->numEventVars;
}

/**Function********************************************************************

  Synopsis           [Returns the varInfo of a module.]

  Description        [The function returns the varInfo of a module if it
                      exists in the state manager. Otherwise it creates the
                      Enum_VarInfo_t data structure, registers it with
                      state manager and returns the new data structure.]

  SideEffects        [moduleToVarInfo hash table inside stateManager could be
                      updated.]

******************************************************************************/
Enum_VarInfo_t *
Enum_StateManagerObtainVarInfoFromModule(
  Enum_StateManager_t *stateManager,
  Mdl_Module_t *module)
{
  Enum_VarInfo_t *varInfo;
  
  if(st_lookup(stateManager->moduleToVarInfo, (char *) module, (char **) &varInfo))
    return varInfo;
  else {
    varInfo = Enum_VarInfoAlloc(module);
    Enum_StateManagerAddVarInfo(stateManager, module, varInfo);
    return varInfo;
  }
}

/**Function********************************************************************

  Synopsis           [Adds varInfo of a module to the state manager.]

  SideEffects        [None]

******************************************************************************/
void
Enum_StateManagerAddVarInfo(
  Enum_StateManager_t *stateManager,
  Mdl_Module_t *module,
  Enum_VarInfo_t *varInfo)
{
  st_insert(stateManager->moduleToVarInfo, (char *) module, (char *) varInfo);
}

/**Function********************************************************************

  Synopsis           [Checks if a variable in a state has been assigned a
                      value.]

  Description        [The function returns TRUE if var has been assigned a
                      value, otherwise it returns FALSE.]

  SideEffects        [None]

  SeeAlso            [Enum_StateSetVariableValue, Enum_StateReadVariableValue]

******************************************************************************/
boolean
Enum_StateIsVariableAssigned(
  Enum_State_t *state,
  Var_Variable_t *var,
  Enum_VarInfo_t *varInfo)
{
  int index;

  st_lookup(varInfo->varToIndex, (char *) var, (char **) &index);
  
  if(Var_VariableIsHistoryDependent(var))
    return (state->histDependVars[index] != LONG_MAX);  
  else if(Var_VariableIsHistoryFree(var))
    return (state->histFreeVars[index] != LONG_MAX);  
  else
    return (state->eventVars[index] != INT_MAX);
}

/**Function********************************************************************

  Synopsis           [Reads the value of a variable in a state.]

  Description        [The function returns the value of var in state. Note
                      that the function does not check if var has been
                      assigned a value in state. It is the user's
                      responsibility to make sure that he reads only a
                      variable that has been assigned.]

  SideEffects        [None]

  SeeAlso            [Enum_StateSetVariableValue, Enum_StateIsVariableAssigned]

******************************************************************************/
long
Enum_StateReadVariableValue(
  Enum_State_t *state,
  Var_Variable_t *var,
  Enum_VarInfo_t *varInfo)
{
  int index ;

  st_lookup(varInfo->varToIndex, (char *) var, (char **) &index);
  
  if(Var_VariableIsHistoryDependent(var))
    return state->histDependVars[index];  
  else if(Var_VariableIsHistoryFree(var))
    return state->histFreeVars[index];  
  else
    return (long) state->eventVars[index];
  
}

/**Function********************************************************************

  Synopsis           [Sets the value of a variable.]

  Description        [The value of the variable is set to value. Value is a
                      number for boolean, integer or natural number variables.
                      For symbolically enumerated variables, value is an index
                      associated with the symbolic values of the variable.]

  SideEffects        [The old value of the variable is overwritten by the new
                      value.]

  SeeAlso            [Enum_StateReadVariableValue, Enum_StateIsVariableAssigned]

******************************************************************************/
void
Enum_StateSetVariableValue(
  Enum_State_t *state,
  Var_Variable_t *var,
  Enum_VarInfo_t *varInfo,
  long value)
{
  int index;

  st_lookup(varInfo->varToIndex, (char *) var, (char **) &index);
  
  if(Var_VariableIsHistoryDependent(var))
    state->histDependVars[index] = value;  
  else if(Var_VariableIsHistoryFree(var))
    state->histFreeVars[index] = value;  
  else
    state->eventVars[index] = (int) value;

}

/**Function********************************************************************

  Synopsis           [Frees the history free part of a state.]

  Description        [The function frees the history free variables of a
                      state and sets the field histFreeVars to NULL.]

  SideEffects        [None]

******************************************************************************/
void
Enum_StateFreeHistFreeVars(
  Enum_State_t *state)
{
  FREE(state->histFreeVars);
  state->histFreeVars = NIL(long);
}

/**Function********************************************************************

  Synopsis           [Frees the event variables of a state.]

  Description        [The function frees the event variables of a state and
                      sets the field eventVars to NULL.] 

  SideEffects        [None]

******************************************************************************/
void
Enum_StateFreeEventVars(
  Enum_State_t *state)
{
  FREE(state->eventVars);
  state->eventVars = NIL(int);
}

/**Function********************************************************************

  Synopsis           [Computes the set of initial states of a module.]

  Description        [The function returns the set of initial states of a
                      module. The 
                      module should not have any external variables that are
                      not boolean, event or enumerated type. If successful,
                      the function returns a list of initial states, otherwise
                      it returns NULL.]

  SideEffects        [None]

  SeeAlso            [Enum_ModuleComputeSetOfPostStates]

******************************************************************************/
lsList
Enum_ModuleComputeSetOfInitialStates(
  Mdl_Module_t *module,
  Enum_VarInfo_t *varInfo)
{
  lsGen atmGen, varGen;
  lsGeneric atom, var;
  lsList initialSet, currentInitialSet;
  lsList atomList;
  Enum_State_t *state, *dupState;
  lsList extVarList, extNonEventVarList;
  
  /* Currently, I allow only boolean and enumerated types as external 
     variables to a module. Otherwise, the set of initial states is infinite. */
  state = Enum_StateAlloc(varInfo);

  extVarList = Mdl_ModuleGetExternalVariableList(module);
  extNonEventVarList = lsCreate();
  lsForEachItem(extVarList, varGen, var) {
    if(Var_VariableReadDataType((Var_Variable_t *) var) != Var_Event_c)
      lsNewEnd(extNonEventVarList, var, LS_NH);
  }
  
  initialSet = Enum_StateAssignAllVarCombinations(state, extNonEventVarList, varInfo);

  Enum_StateFree(state);
  lsDestroy(extVarList, NULL);
  lsDestroy(extNonEventVarList, NULL);
  
  if(initialSet == (lsList) 0) 
    return (lsList) 0;
  
  /* Get a list of sorted atoms. */
  atomList = Mdl_ModuleObtainSortedAtomList(module);
  /* Iterate over the atoms. */
  lsForEachItem(atomList, atmGen, atom) {

    /* Obtain the initial command list. For each state in currentInitialSet,
       generate updated states from those guarded assignments for which
       guards are true and append them to initialSet. If no guards are true
       for some state then append that state itself to initialSet. This is
       the default non-blocking semantics of reactive modules. */

    lsList cmdList = Atm_AtomReadInitCommandList((Atm_Atom_t *) atom);
    lsList ctrlVarList = Atm_AtomReadControlVarList((Atm_Atom_t *) atom);

    if (lsLength(cmdList) > 0) {
      lsList assignList;
      lsList cmdGen, stateGen;
      int i;
      lsGeneric cmd, tempState;
      lsList newList;
      
      currentInitialSet = initialSet;
      initialSet = lsCreate();
      /* Execute guarded commands on each intermediate state in currentInitialSet
         one by one. */
      lsForEachItem(currentInitialSet, stateGen, tempState) {
        boolean noGuardTrue = TRUE;
        
        lsForEachItem(cmdList, cmdGen, cmd) {
          Atm_Expr_t *guard = Atm_CmdReadGuard((Atm_Cmd_t *) cmd);
          
          if(EnumExprIsTrue(guard, varInfo, NIL(Enum_State_t), (Enum_State_t *) tempState)) {
            noGuardTrue = FALSE;	     
            assignList = Atm_CmdReadAssignList((Atm_Cmd_t *) cmd);
            newList = EnumInitAssignListExecuteSetup((Atm_Atom_t *) atom, varInfo, assignList, (Enum_State_t *) tempState);
            EnumListConcatenate(initialSet, newList);
	  }
	}
        

	if (noGuardTrue) {
	   Atm_Cmd_t *defaultCmd = Atm_AtomReadDefaultInitCommand((Atm_Atom_t *) atom);

	   if (defaultCmd != NIL(Atm_Cmd_t)) {
	      assignList = Atm_CmdReadAssignList(defaultCmd);
	      newList = EnumInitAssignListExecuteSetup((Atm_Atom_t *) atom, varInfo, assignList, (Enum_State_t *) tempState);
	      EnumListConcatenate(initialSet, newList);
	   }
	   else {
	      Main_MochaErrorPrint("Error in creating the initial set. There is an assignment to the external variables such that some of the other variables cannot be initialized.\n");
	      exit(1);
	   }
	}

	lsDestroy(newList, NULL);
      }
      
      lsDestroy(currentInitialSet, Enum_StateFree);
    }
    else {
       
       lsGeneric state1, state2, var;
       lsGen stateGen1, stateGen2, varGen;
       lsList nonEventCtrlVars = lsCreate(); 
       
       currentInitialSet = initialSet;
       initialSet = lsCreate();
       
       lsForEachItem(ctrlVarList, varGen, var) {
	  if(Var_VariableReadDataType((Var_Variable_t *) var) != Var_Event_c)
	     lsNewEnd(nonEventCtrlVars, var, LS_NH);
       }
       
       lsForEachItem(currentInitialSet, stateGen1, state1) {
	  lsList temp = Enum_StateAssignAllVarCombinations((Enum_State_t *) state1,
							  nonEventCtrlVars, varInfo);
	  
	  lsForEachItem(temp, stateGen2, state2) {
	     lsNewEnd(initialSet, state2, LS_NH);
	  }
	  
	  lsDestroy(temp, (void (*)()) NULL);
       }
       
       lsDestroy(currentInitialSet, Enum_StateFree);
    }
  }

  lsDestroy(atomList, NULL);
  
  return initialSet;
  
}



/**Function********************************************************************

  Synopsis           [Updates the partial initial state to generate all possible
                      initial states for the atom.]

  Description        [The function returns all possible extensions of the partial
  state by executing the init guarded commands of the atom. If there are no init
  guarded commands all possible evaluations of the non-event controlled variables
  are done. If some guarded command does not specify the initial value of a
  non-event variable, then all possible evaluations are generated. The reason for
  generating all possible combinations of event variables is that they would not
  affect the update of any variable in the next round. If no guard is true an empty
  list is returned.]

  SideEffects        [None]

  SeeAlso            [Enum_AtomComputeSetOfPartialPostStates]

******************************************************************************/
lsList
Enum_AtomComputeSetOfPartialInitialStates(
  Atm_Atom_t *atom,
  Enum_VarInfo_t *varInfo,
  Enum_State_t *partialState)
{
  lsGen varGen;
  lsGeneric var;
  lsList initialSet = lsCreate();
  Enum_State_t *state, *dupState;
  lsList nonEventCtrlVars; 
  lsList cmdList = Atm_AtomReadInitCommandList(atom);
  lsList ctrlVarList = Atm_AtomReadControlVarList(atom);
       
  if(lsLength(cmdList) > 0) {
     lsList assignList;
     lsList cmdGen, stateGen;
     int i;
     lsGeneric cmd;
     boolean noGuardTrue = TRUE;
     lsList newList;
     
     lsForEachItem(cmdList, cmdGen, cmd) {
       Atm_Expr_t *guard= Atm_CmdReadGuard((Atm_Cmd_t *) cmd);
        
       if (EnumExprIsTrue(guard, varInfo, NIL(Enum_State_t), partialState)) {
         noGuardTrue = FALSE;
         assignList = Atm_CmdReadAssignList((Atm_Cmd_t *) cmd);
         newList = EnumInitAssignListExecuteSetup((Atm_Atom_t *) atom, varInfo, assignList, (Enum_State_t *) partialState);
         EnumListConcatenate(initialSet, newList);
       }

     }

     if (noGuardTrue) {
	Atm_Cmd_t *defaultCmd = Atm_AtomReadDefaultInitCommand((Atm_Atom_t *) atom);
	
	if (defaultCmd != NIL(Atm_Cmd_t)) {
	   assignList = Atm_CmdReadAssignList(defaultCmd);
	   newList = EnumInitAssignListExecuteSetup((Atm_Atom_t *) atom, varInfo, assignList, (Enum_State_t *) partialState);
	   EnumListConcatenate(initialSet, newList);
	}
	else {
	   Main_MochaErrorPrint("Error in creating the initial set. There is an assignment to the external variables such that some of the other variables cannot be initialized.\n");
	   exit(1);
	}
     }

     lsDestroy(newList, NULL);
  }
  else {
     
     nonEventCtrlVars = lsCreate(); 
     lsForEachItem(ctrlVarList, varGen, var) {
	if(Var_VariableReadDataType((Var_Variable_t *) var) != Var_Event_c)
	   lsNewEnd(nonEventCtrlVars, var, LS_NH);
     }
     
     initialSet = Enum_StateAssignAllVarCombinations(partialState, nonEventCtrlVars, varInfo);   
  }
  
  return initialSet;
  
}


/**Function********************************************************************

  Synopsis           [Computes the post set of a state.]

  Description        [The function returns a list of successor states of state
                      in module. The module should not have any external
                      variables that are not boolean, event or enumerated
                      types. If successful, the function returns a list of
                      successor states, otherwise it returns NULL.]

  SideEffects        [None]

  SeeAlso            [Enum_ModuleComputeSetOfInitialStates]

******************************************************************************/
lsList
Enum_ModuleComputeSetOfPostStates(
  Mdl_Module_t *module,
  Enum_VarInfo_t *varInfo,
  Enum_State_t *state)
{
  lsGen atmGen, aGen;
  lsGeneric atom;
  lsList postSet, currentPostSet;
  lsList atomList, nondetVarList, nondetAssignList;
  Enum_State_t *tempState, *aState;
  lsList extVarList = Mdl_ModuleGetExternalVariableList(module);
  
  tempState = Enum_StateAlloc(varInfo);
  postSet = Enum_StateAssignAllVarCombinations(tempState, extVarList, varInfo);

  Enum_StateFree(tempState);
  lsDestroy(extVarList, NULL);
  
  if(postSet == (lsList) 0) /* Some external variable does not have a finite
                               domain. */
    return (lsList) 0;

  /* Get a list of sorted atoms. */
  atomList = Mdl_ModuleObtainSortedAtomList(module);
  /* Iterate over the atoms. */
  lsForEachItem(atomList, atmGen, atom) {

    /* Obtain the update command list. For each state in currentPostSet,
        generate updated states from those guarded assignments for which
        guards are true and append them to postSet (postSet is empty
        initially. If no guards are true
        for some state then append that state itself to postSet. This is
        the default non-blocking semantics of reactive modules. */
    
    lsList cmdList = Atm_AtomReadUpdateCommandList((Atm_Atom_t *) atom);
    lsList assignList, readAwaitList;
    lsGen cmdGen, stateGen, varGen;
    lsGeneric cmd, var;
    Atm_Expr_t *guard;
    Enum_State_t *dupState;
    boolean atomIsLazy = (Atm_AtomReadAtomType((Atm_Atom_t *) atom) ==
                    Atm_Lazy_c);
    lsList ctrlVarList = Atm_AtomReadControlVarList((Atm_Atom_t *) atom);
    long value;
    
    currentPostSet = postSet;
    postSet = lsCreate();
    /* Execute guarded commands on each intermediate state in currentPostSet
       one by one. */
    lsForEachItem(currentPostSet, stateGen, tempState) {
      boolean noGuardTrue = TRUE;
      boolean flag = atomIsLazy;
      lsList newList;
      
      /* If noGuardTrue remains TRUE at the end of the following loop, then */
      /* the current partial state should be just copied to the postSet */
      /* because of the non-blocking semantics of Reactive Modules. */
      
      lsForEachItem(cmdList, cmdGen, cmd) {
        guard = Atm_CmdReadGuard((Atm_Cmd_t *) cmd);

        if (EnumExprIsTrue(guard, varInfo, state, (Enum_State_t *) tempState)) {
	   noGuardTrue = FALSE;
	   assignList = Atm_CmdReadAssignList((Atm_Cmd_t *) cmd);
	   newList = EnumUpdateAssignListSetup((Atm_Atom_t *) atom, varInfo, assignList, state, (Enum_State_t *) tempState);
	   EnumListConcatenate(postSet, newList);
	   lsDestroy(newList, NULL);
	}
      }

      if (noGuardTrue) {
	 Atm_Cmd_t *defaultCmd = Atm_AtomReadDefaultUpdateCommand((Atm_Atom_t *) atom);

	 if (defaultCmd != NIL(Atm_Cmd_t)) {
	    assignList = Atm_CmdReadAssignList((Atm_Cmd_t *) defaultCmd);
	    newList = EnumUpdateAssignListSetup((Atm_Atom_t *) atom, varInfo, assignList, state, (Enum_State_t *) tempState);
	    EnumListConcatenate(postSet, newList);
	    lsDestroy(newList, NULL);
	    noGuardTrue = FALSE;
	 }
      }
      
      /* If flag is FALSE, then it is worthwhile investigating whether current */
      /* partial state can be copied over to postSet because of the event */
      /* nature of the current atom. */
      if(!flag) {
        if (Atm_AtomReadAtomType((Atm_Atom_t *) atom) == Atm_Event_c) {
          readAwaitList = Atm_AtomObtainReadAwaitVarList((Atm_Atom_t *) atom);
          if (lsLength(readAwaitList) > 0) {   
            flag = TRUE;
            lsForEachItem(readAwaitList, varGen, var) {
              if(Var_VariableReadDataType((Var_Variable_t *) var) != Var_Event_c) {
                if(Enum_StateReadVariableValue((Enum_State_t *) tempState,
                                               (Var_Variable_t *) var, varInfo) !=
                   Enum_StateReadVariableValue((Enum_State_t *) state, 
                                               (Var_Variable_t *) var, varInfo))
                  flag = FALSE;
              }
              else {
                if(Enum_StateReadVariableValue((Enum_State_t *) tempState,
                                               (Var_Variable_t *) var, varInfo))
                  flag = FALSE;
              }
            }
          }
          
          lsDestroy(readAwaitList, NULL);
        }
      }


      if(flag || noGuardTrue) {         
         dupState = Enum_StateDup((Enum_State_t *) tempState, varInfo);
         nondetVarList = lsCreate();
         lsForEachItem(ctrlVarList, varGen, var) {
            if (IsInList(Atm_AtomReadReadVarList((Atm_Atom_t *) atom), var)) {
               if(Var_VariableReadDataType((Var_Variable_t *) var) != Var_Event_c) {
                  value = Enum_StateReadVariableValue(state, (Var_Variable_t *) var,
                                                      varInfo);
                  Enum_StateSetVariableValue(dupState, (Var_Variable_t *) var,
                                             varInfo, value);
               }
               else {
                  Enum_StateSetVariableValue(dupState, (Var_Variable_t *) var,
                                             varInfo, 0);
               }
            }
            else {
               lsNewEnd(nondetVarList, var, LS_NH);
            }
         }

         if (lsLength(nondetVarList) > 0) {
            nondetAssignList = Enum_StateAssignAllVarCombinations(dupState,nondetVarList,varInfo);
            EnumListConcatenate(postSet, nondetAssignList);
            Enum_StateFree(dupState);
            lsDestroy(nondetAssignList, NULL);
         }
         else {
            lsNewEnd(postSet, (lsGeneric) dupState, LS_NH);
         }
      
         lsDestroy(nondetVarList, NULL);
         
      }
    }
    
    if(lsDestroy(currentPostSet, Enum_StateFree) != LS_OK)
      Main_MochaErrorPrint("Error in destroying a list\n");
  }

  lsDestroy(atomList, NULL);
  
  return postSet;
}

/**Function********************************************************************

  Synopsis           [Executes an atom on a partial state to generate all
  possible partial post states.]

  Description        [The function generates all possible augmentations of the
  partial state by executing the update guarded commands of the atom. If a
  guarded command does not update a variable, then its value is same as its
  value in state if it is read by the atom otherwise it is updated
  nondeterministically. If the atom is lazy or event appropriate post states
  are generated. Note that it is imperative for a lazy or event atom to read
  all its controlled variables.]

  SideEffects        [None]

  SeeAlso            [Enum_AtomComputeSetOfPartialInitialStates]

******************************************************************************/
lsList
Enum_AtomComputeSetOfPartialPostStates(
  Atm_Atom_t *atom,
  Enum_VarInfo_t *varInfo,
  Enum_State_t *state,
  Enum_State_t *partialState)
{
   lsList postSet;
   lsList assignList, readAwaitList, nondetVarList, nondetAssignList;
   lsGen cmdGen, aGen, varGen;
   lsGeneric cmd, var;
   Atm_Expr_t *guard;
   Enum_State_t *aState, *dupState;
   long value;
   lsList ctrlVarList = Atm_AtomReadControlVarList((Atm_Atom_t *) atom);
   lsList cmdList = Atm_AtomReadUpdateCommandList((Atm_Atom_t *) atom);
   boolean atomIsLazy = (Atm_AtomReadAtomType((Atm_Atom_t *) atom) ==
			 Atm_Lazy_c);
   boolean noGuardTrue = TRUE;
   boolean flag = atomIsLazy;
   lsList newList;
   
   postSet = lsCreate();

   lsForEachItem(cmdList, cmdGen, cmd) {
     guard = Atm_CmdReadGuard((Atm_Cmd_t *) cmd);

     if (EnumExprIsTrue(guard, varInfo, state, partialState)) {
	 noGuardTrue = FALSE;
	 assignList = Atm_CmdReadAssignList((Atm_Cmd_t *) cmd);
	 newList = EnumUpdateAssignListSetup((Atm_Atom_t *) atom, varInfo, assignList, state, partialState);
	 EnumListConcatenate(postSet, newList);
	 lsDestroy(newList, NULL);
      }

   }
   
   if (noGuardTrue) {
      Atm_Cmd_t *defaultCmd = Atm_AtomReadDefaultUpdateCommand((Atm_Atom_t *) atom);

      if (defaultCmd != NIL(Atm_Cmd_t)) {
	 assignList = Atm_CmdReadAssignList((Atm_Cmd_t *) defaultCmd);
	 newList = EnumUpdateAssignListSetup((Atm_Atom_t *) atom, varInfo, assignList, state, partialState);
	 EnumListConcatenate(postSet, newList);
	 lsDestroy(newList, NULL);
	 noGuardTrue = FALSE;
      }
   }

   if (!flag) {
     if(Atm_AtomReadAtomType((Atm_Atom_t *) atom) == Atm_Event_c) {
       readAwaitList = Atm_AtomObtainReadAwaitVarList((Atm_Atom_t *) atom);
       if(lsLength(readAwaitList) > 0) {         
	 flag = TRUE;
         lsForEachItem(readAwaitList, varGen, var) {
           if(Var_VariableReadDataType((Var_Variable_t *) var) != Var_Event_c) {
             if(Enum_StateReadVariableValue(partialState,
                                            (Var_Variable_t *) var, varInfo) !=
                Enum_StateReadVariableValue((Enum_State_t *) state, 
                                            (Var_Variable_t *) var, varInfo))
	       flag = FALSE;
           }
           else {
             if(Enum_StateReadVariableValue(partialState,
                                            (Var_Variable_t *) var, varInfo))
	       flag = FALSE;
           }
         }
       }

       lsDestroy(readAwaitList, NULL);   
     }
   }
   
	
   if(flag || noGuardTrue) {
      
      dupState = Enum_StateDup(partialState, varInfo);

      nondetVarList = lsCreate();
      lsForEachItem(ctrlVarList, varGen, var) {
	 if (IsInList(Atm_AtomReadReadVarList(atom), var)) {
	    if(Var_VariableReadDataType((Var_Variable_t *) var) != Var_Event_c) {
	       value = Enum_StateReadVariableValue(state, (Var_Variable_t *) var,
						   varInfo);
	       Enum_StateSetVariableValue(dupState, (Var_Variable_t *) var,
					  varInfo, value);
	    }
	    else {
	       Enum_StateSetVariableValue(dupState, (Var_Variable_t *) var,
					  varInfo, 0);
	    }
	 }
	 else {
	    lsNewEnd(nondetVarList, var, LS_NH);
	 }
      }

      if (lsLength(nondetVarList) > 0) {
	 nondetAssignList = Enum_StateAssignAllVarCombinations(dupState, nondetVarList, varInfo);
	 EnumListConcatenate(postSet, nondetAssignList);
	 Enum_StateFree(dupState);
	 lsDestroy(nondetAssignList, NULL);
      }
      else {
	 lsNewEnd(postSet, (lsGeneric) dupState, LS_NH);
      }
      
      lsDestroy(nondetVarList, NULL);
      
   }
   
   return postSet;
}


/**Function********************************************************************

  Synopsis           [Allocates a state.]

  Description        [The function allocates and returns a state. It
                      initializes all variable values in the state to {LONG,
                      INT}_MAX, which means that these variables are
                      uninitialized. The parentState field is set to NULL.]

  SideEffects        [None]

  SeeAlso            [Enum_StateFree]

******************************************************************************/
Enum_State_t *
Enum_StateAlloc(
  Enum_VarInfo_t  *varInfo)
{
  Enum_State_t *state = ALLOC(Enum_State_t, 1);
  int i;
  
  state->histDependVars = ALLOC(long, varInfo->numHistDependVars);
  for(i = 0; i < varInfo->numHistDependVars; i++)
    state->histDependVars[i] = LONG_MAX;

  state->histFreeVars = ALLOC(long, varInfo->numHistFreeVars);
  for(i = 0; i < varInfo->numHistFreeVars; i++)
    state->histFreeVars[i] = LONG_MAX;

  state->eventVars = ALLOC(int, varInfo->numEventVars);
  for(i = 0; i < varInfo->numEventVars; i++)
    state->eventVars[i] = INT_MAX;

  state->parentState = NIL(Enum_State_t);
  
  return state;
}

/**Function********************************************************************

  Synopsis           [Frees a state.]

  Description        [The function frees all the internal memory of a state
                      and the state itself.]

  SideEffects        [None]

  SeeAlso            [Enum_StateAlloc]

******************************************************************************/
void
Enum_StateFree(
  Enum_State_t *state)
{
  if(state->histDependVars != NIL(long))
    FREE(state->histDependVars);
  if(state->histFreeVars != NIL(long))
    FREE(state->histFreeVars);
  if(state->eventVars != NIL(int))
    FREE(state->eventVars);
  FREE(state);
}

/**Function********************************************************************

  Synopsis           [Returns a duplicate copy of a state.]

  Description        [The function returnss a duplicate copy of state. It is the
                      responsibility of the caller function to free the
                      returned state. The parentState field is just copied
                      from state.]

  SideEffects        [None]

******************************************************************************/
Enum_State_t *
Enum_StateDup(
  Enum_State_t *state,
  Enum_VarInfo_t *varInfo)
{
  Enum_State_t *dupState = Enum_StateAlloc(varInfo);
  int i;
  
  for(i = 0; i < varInfo->numHistDependVars; i++)
    dupState->histDependVars[i] = state->histDependVars[i];

  for(i = 0; i < varInfo->numHistFreeVars; i++)
    dupState->histFreeVars[i] = state->histFreeVars[i];

  for(i = 0; i < varInfo->numEventVars; i++)
    dupState->eventVars[i] = state->eventVars[i];

  dupState->parentState = state->parentState;
  
  return dupState;
}

/**Function********************************************************************

  Synopsis           [Prints the partial state to a string.]

  Description        [This function returns a string which contains the
  valuation of the variables in the given partial state.]

  SideEffects        [User should free the string after use.]

  SeeAlso            [optional]

******************************************************************************/
char *
Enum_PartialStatePrintToString(
  Enum_VarInfo_t *varInfo,
  Enum_State_t *partialState)
{
  Enum_State_t *state = partialState;
  int i, n;
  Var_Variable_t **varArray;
  char * result = util_strsav("");
  char * tmpResult1, *tmpResult2;
  
  n = Enum_VarInfoReadNumHistDependVars(varInfo);
  varArray = Enum_VarInfoReadHistDependVars(varInfo);
  for(i = 0; i < n; i++) {
    char *name = Var_VariableReadName(varArray[i]);
    long value = state->histDependVars[i];
    char * valueString = EnumPrintValueToString(varArray[i], value);
    
    if (valueString) {
      tmpResult1 = util_strcat3(name, "=", valueString);
      tmpResult2 = util_strcat3(result, tmpResult1, " ");
      FREE(tmpResult1);
      FREE(result);
      result = tmpResult2;
      
      FREE(valueString);
    }
  }

  /* Second, print history free variables. */
  n = Enum_VarInfoReadNumHistFreeVars(varInfo);
  varArray = Enum_VarInfoReadHistFreeVars(varInfo);
  for(i = 0; i < n; i++) {
    char *name = Var_VariableReadName(varArray[i]);
    long value = state->histFreeVars[i];
    char *valueString = EnumPrintValueToString(varArray[i], value);
    
    if (valueString) {
      tmpResult1 = util_strcat3(name, "=", valueString);
      tmpResult2 = util_strcat3(result, tmpResult1, " ");
      FREE(tmpResult1);
      FREE(result);
      result = tmpResult2;
      
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

    if(value == 1) {
      tmpResult1 = util_strcat3(result, name, " ");
      FREE(result);
      result = tmpResult1;
    }
  }

  return result;
    
}

/**Function********************************************************************

  Synopsis           [Produces a list of states in which all combinations of
                      values to variables in varList have been assigned.]

  Description        [The function takes as arguments a partial state and a
                      variable list. The variables in varList should not have
                      been assigned a value in state. All variables
                      in varList should be of one of three types -
                      Var_Boolean_c, Var_Enumerated_c or Var_Event_c. If
                      successful, the function returns a list of states
                      (possibly partial) that have the same value as state for
                      all variables assigned to in state and all combinations
                      of the values to the variables in varList.]

  SideEffects        [None]

******************************************************************************/
lsList Enum_StateAssignAllVarCombinations(
  Enum_State_t *state,
  lsList varList,
  Enum_VarInfo_t *varInfo)
{
  
  lsList stateSet, currentStateSet;
  lsGen varGen, stateGen;
  lsGeneric var, tempState;
  boolean flag = FALSE;
  Enum_State_t *dupState;
  Var_DataType dataType;

  lsForEachItem(varList, varGen, var) {
    dataType = Var_VariableReadDataType((Var_Variable_t *) var);
    if(!(dataType == Var_Boolean_c || dataType == Var_Enumerated_c ||
         dataType== Var_Event_c || dataType == Var_Range_c)) 
      flag = TRUE;
    if(Enum_StateIsVariableAssigned(state, (Var_Variable_t *) var, varInfo))
      flag = TRUE;
  }
  if(flag) {
    return (lsList) 0;
  }
  
  /* Initialize a list of possible states generated by assigning different
     combination of external variables. */
  stateSet = lsCreate();
  if(lsNewEnd(stateSet, (lsGeneric) Enum_StateDup(state, varInfo), LS_NH) != LS_OK) {
    Main_MochaErrorPrint("Error in appending to a list\n");
  }
  
  lsForEachItem(varList, varGen, var) {
    dataType = Var_VariableReadDataType((Var_Variable_t *) var);
    
    currentStateSet = stateSet;
    stateSet = lsCreate();
    if(dataType == Var_Boolean_c || dataType == Var_Event_c) {
      lsForEachItem(currentStateSet, stateGen, tempState) {
        dupState = Enum_StateDup((Enum_State_t *) tempState, varInfo);
        Enum_StateSetVariableValue(dupState, (Var_Variable_t *) var, varInfo, 0);
        if(lsNewEnd(stateSet, (lsGeneric) dupState, LS_NH) != LS_OK) {
          Main_MochaErrorPrint("Error in appending to a list\n");
        }
      }
      lsForEachItem(currentStateSet, stateGen, tempState) {
        dupState = Enum_StateDup((Enum_State_t *) tempState, varInfo);
        Enum_StateSetVariableValue(dupState, (Var_Variable_t *) var, varInfo, 1);
        if(lsNewEnd(stateSet, (lsGeneric) dupState, LS_NH) != LS_OK) {
          Main_MochaErrorPrint("Error in appending to a list\n");
        }
      }
    }
    else {
      int i;
      int numValues =
          Var_VarTypeReadDomainSize(Var_VariableReadType((Var_Variable_t *) var)); 
      
      for(i = 0; i < numValues; i++) {
        lsForEachItem(currentStateSet, stateGen, tempState) {
          dupState = Enum_StateDup((Enum_State_t *) tempState, varInfo);
          Enum_StateSetVariableValue(dupState, (Var_Variable_t *) var,
                                     varInfo, (long) i);
          if(lsNewEnd(stateSet, (lsGeneric) dupState, LS_NH) != LS_OK) {
            Main_MochaErrorPrint("Error in appending to a list\n");
          }
        }
      }
    }
    
    
    if(lsDestroy(currentStateSet, Enum_StateFree) != LS_OK) {
      Main_MochaErrorPrint("Error in destroying a list\n");
    }
  }
  return stateSet;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************
  
  Synopsis           [Appends list2 to list1.]
  
  Description        [Appends the elements of list2 to the end of list1. The
                      elements themselves are not copied.]
  
  SideEffects        [list1 is modified.]
  
  ******************************************************************************/
void
EnumListConcatenate(
  lsList list1,
  lsList list2)
{
  lsGen gen;
  lsGeneric data;
  
  lsForEachItem(list2, gen, data) {
    if(lsNewEnd(list1, data, LS_NH) != LS_OK)
      Main_MochaErrorPrint("Error in appending to a list\n");
  }
}

/**Function********************************************************************

  Synopsis           [Checks whether an expression is true.]

  Description        [The function returns TRUE if the expresion expr is true,
                      otherwise it returns FALSE. The primed variables in expr
                      refer to values in state and the unprimed variables
                      refer to values in prevState.] 

  SideEffects        [None]

  SeeAlso            [EnumExprEvaluate, EnumBitvectorExprEvaluate]

******************************************************************************/
boolean
EnumExprIsTrue(
  Atm_Expr_t *expr,
  Enum_VarInfo_t *varInfo,
  Enum_State_t *prevState,
  Enum_State_t *state)
{
  /* If expression is of boolean type (Atm_And_c, Atm_Or_c, Atm_Not_c or
     Atm_EventQuery_c) evaluate its children as boolean expressions.
     If expression is of arithmetic type (Atm_Plus_c or Atm_Minus_c),
     Atm_Equal_c, Atm_NotEqual_c, Atm_Greater_c, Atm_GreeaterEqual_c, Atm_Less_c or
     Atm_LessEqual_c, evaluate its children as arithmetic expressions.
     If expression is of type Atm_Var_c or Atm_Const_c it is a leaf node.
     */
  EnumBitwise_t *result1, *result2;
  int i;
  Atm_ExprType exprType = Atm_ExprReadType(expr);
  
  switch (exprType) {
      case Atm_UnPrimedVar_c:
      {
        if(Enum_StateReadVariableValue(prevState, (Var_Variable_t *)
                                       Atm_ExprReadLeftChild(expr), varInfo))
          return TRUE;
        else
          return FALSE;
      }
      case Atm_PrimedVar_c:
      {
        if(Enum_StateReadVariableValue(state, (Var_Variable_t *)
                                       Atm_ExprReadLeftChild(expr), varInfo))        
          return TRUE;
        else
          return FALSE;
      }
      case Atm_BoolConst_c:
      {
        if(Atm_ExprReadLeftChild(expr))
          return TRUE;
        else
          return FALSE;
      }
      
      case Atm_Index_c :
      {
        Atm_Expr_t *varExpr = Atm_ExprReadLeftChild(expr);
        Atm_Expr_t *indexExpr = Atm_ExprReadRightChild(expr);
        Atm_ExprType varExprType = Atm_ExprReadType(varExpr);
        Var_Variable_t *var = (Var_Variable_t *)
            Atm_ExprReadLeftChild(varExpr);
        Var_Variable_t *actualVar;
        char *varName, *actualVarName;
        Var_Type_t *varType, *indexType;
        int value;
        int indexIsEnum = 0;
        char *tmpString;
        
        assert(varExprType == Atm_UnPrimedVar_c || varExprType ==
               Atm_PrimedVar_c);

        varName = Var_VariableReadName(var);
        varType = Var_VariableReadType(var);

        value = EnumExprEvaluate(indexExpr, varInfo, prevState, state);
        
        if (Var_VarTypeReadDataType(varType) == Var_Array_c) {
          indexType = Var_VarTypeReadIndexType(varType);
          if (Var_VarTypeReadDataType(indexType) == Var_Enumerated_c)
            indexIsEnum = 1;
        }
        
        if (indexIsEnum)
          actualVarName = util_strcat4(varName, "[", Var_EnumElementReadName(
            Var_VarTypeReadSymbolicValueFromIndex(indexType, value)), "]");
        else {
          tmpString = ALLOC(char, INDEX_STRING_LENGTH);
          sprintf(tmpString, "%d", value);
          actualVarName = util_strcat4(varName, "[", tmpString, "]");
          FREE(tmpString);
        }
        
        actualVar = Mdl_ModuleReadVariableFromName(actualVarName, varInfo->module);
        FREE(actualVarName);

        assert(Var_VariableReadDataType(actualVar) == Var_Boolean_c);
        
        if (varExprType == Atm_UnPrimedVar_c)
          return Enum_StateReadVariableValue(prevState, actualVar, varInfo);
        else
          return Enum_StateReadVariableValue(state, actualVar, varInfo);
      }

      case Atm_Implies_c:
      {
        if(!EnumExprIsTrue(Atm_ExprReadLeftChild(expr), varInfo, prevState,
			   state))
          return TRUE;
        else
          return EnumExprIsTrue(Atm_ExprReadRightChild(expr), varInfo, prevState,
                                       state);
      }
	 
      case Atm_Or_c: 
      {
        if(EnumExprIsTrue(Atm_ExprReadLeftChild(expr), varInfo, prevState,
                                 state))
          return TRUE;
        else
          return EnumExprIsTrue(Atm_ExprReadRightChild(expr), varInfo, prevState,
                                       state);
      }
      
      case Atm_And_c:
      {
        if(!EnumExprIsTrue(Atm_ExprReadLeftChild(expr), varInfo, prevState,
                                 state))
          return FALSE;
        else
          return EnumExprIsTrue(Atm_ExprReadRightChild(expr), varInfo, prevState,
                                       state);
      }
      
      case Atm_Not_c:
      {
        if(EnumExprIsTrue(Atm_ExprReadLeftChild(expr), varInfo, prevState,
                                 state))
          return FALSE;
        else
          return TRUE;
      }
      
      case Atm_Equiv_c:
      {
        if(EnumExprEvaluate(Atm_ExprReadLeftChild(expr), varInfo, prevState, state) ==
           EnumExprEvaluate(Atm_ExprReadRightChild(expr), varInfo, prevState, state))
          return TRUE;
        else
          return FALSE;
      }

      case Atm_NotEquiv_c:
      case Atm_Xor_c:
      {
        if(EnumExprEvaluate(Atm_ExprReadLeftChild(expr), varInfo, prevState, state) !=
           EnumExprEvaluate(Atm_ExprReadRightChild(expr), varInfo, prevState, state))
          return TRUE;
        else
          return FALSE;
      }

      case Atm_Equal_c:
      {
        if(EnumExprEvaluate(Atm_ExprReadLeftChild(expr), varInfo, prevState, state) ==
           EnumExprEvaluate(Atm_ExprReadRightChild(expr), varInfo, prevState, state))
          return TRUE;
        else
          return FALSE;
      }

      case Atm_NotEqual_c:
      {
        if(EnumExprEvaluate(Atm_ExprReadLeftChild(expr), varInfo, prevState, state) !=
           EnumExprEvaluate(Atm_ExprReadRightChild(expr), varInfo, prevState, state))
          return TRUE;
        else
          return FALSE;
      }

      case Atm_Greater_c:
      {
        if(EnumExprEvaluate(Atm_ExprReadLeftChild(expr), varInfo, prevState, state) >
           EnumExprEvaluate(Atm_ExprReadRightChild(expr), varInfo, prevState, state))
          return TRUE;
        else
          return FALSE;
      }
      
      case Atm_GreaterEqual_c:
      {
        if(EnumExprEvaluate(Atm_ExprReadLeftChild(expr), varInfo, prevState, state) >=
           EnumExprEvaluate(Atm_ExprReadRightChild(expr), varInfo, prevState, state))
          return TRUE;
        else
          return FALSE;
      }
      
      case Atm_Less_c:
      {
        if(EnumExprEvaluate(Atm_ExprReadLeftChild(expr), varInfo, prevState, state) <
           EnumExprEvaluate(Atm_ExprReadRightChild(expr), varInfo, prevState, state))
          return TRUE;
        else
          return FALSE;
      }
      
      case Atm_LessEqual_c:
      {
        if(EnumExprEvaluate(Atm_ExprReadLeftChild(expr), varInfo, prevState, state) <=
           EnumExprEvaluate(Atm_ExprReadRightChild(expr), varInfo, prevState, state))
          return TRUE;
        else
          return FALSE;
      }
      
      case Atm_EventQuery_c:
      {
        if(Enum_StateReadVariableValue(state, (Var_Variable_t *)
                                       Atm_ExprReadLeftChild(Atm_ExprReadLeftChild(expr)), varInfo))
          return TRUE;
        else
          return FALSE;
      }
  
      case Atm_BitwiseEqual_c : 
      {
        boolean flag = TRUE;
        
        result1 = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                            varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for(i = 0; i < result1->size; i++)
          if (result1->bitArray[i] != result2->bitArray[i]) {
            flag = FALSE;
            break;
          }
        
        EnumBitwiseFree(result1);
        EnumBitwiseFree(result2);
        return flag;
      }

      case Atm_BitwiseNotEqual_c : 
      {
        boolean flag = FALSE;
        
        result1 = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                            varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for(i = 0; i < result1->size; i++)
          if (result1->bitArray[i] != result2->bitArray[i]) {
            flag = TRUE;
            break;
          }
        
        EnumBitwiseFree(result1);
        EnumBitwiseFree(result2);
        return flag;
      }

      case Atm_BitwiseLess_c :
      {
        boolean flag = FALSE;
        
        result1 = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                            varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for(i = result1->size-1; i >= 0 ; i--) {
          if (result1->bitArray[i] < result2->bitArray[i]) {
            flag = TRUE;
            break;
          }
          else if (result1->bitArray[i] > result2->bitArray[i]) {
            flag = FALSE;
            break;
          }
        }

        EnumBitwiseFree(result1);
        EnumBitwiseFree(result2);
        return flag;
      }
      
      case Atm_BitwiseLessEqual_c :
      {
        boolean flag = TRUE;
        
        result1 = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                            varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for(i = result1->size-1; i >= 0 ; i--) {
          if (result1->bitArray[i] < result2->bitArray[i]) {
            flag = TRUE;
            break;
          }
          else if (result1->bitArray[i] > result2->bitArray[i]) {
            flag = FALSE;
            break;
          }
        }

        EnumBitwiseFree(result1);
        EnumBitwiseFree(result2);
        return flag;
      }
      
      case Atm_BitwiseGreater_c :
      {
        boolean flag = FALSE;
        
        result1 = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                            varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for(i = result1->size-1; i >= 0 ; i--) {
          if (result1->bitArray[i] < result2->bitArray[i]) {
            flag = FALSE;
            break;
          }
          else if (result1->bitArray[i] > result2->bitArray[i]) {
            flag = TRUE;
            break;
          }
        }

        EnumBitwiseFree(result1);
        EnumBitwiseFree(result2);
        return flag;
      }
      
      case Atm_BitwiseGreaterEqual_c :
      {
        boolean flag = TRUE;
        
        result1 = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                            varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for(i = result1->size-1; i >= 0 ; i--) {
          if (result1->bitArray[i] < result2->bitArray[i]) {
            flag = FALSE;
            break;
          }
          else if (result1->bitArray[i] > result2->bitArray[i]) {
            flag = TRUE;
            break;
          }
        }

        EnumBitwiseFree(result1);
        EnumBitwiseFree(result2);
        return flag;
      }

      case Atm_RedAnd_c:
      {
        result1 = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                            varInfo, prevState, state);

        for(i = result1->size-1; i >= 0 ; i--) {
          if (!(result1->bitArray[i])) {
            EnumBitwiseFree(result1);
            return FALSE;
          }
        }

        EnumBitwiseFree(result1);
        return TRUE;
        
      }

      case Atm_RedOr_c:
      {
        result1 = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                            varInfo, prevState, state);

        for(i = result1->size-1; i >= 0 ; i--) {
          if ((result1->bitArray[i])) {
            EnumBitwiseFree(result1);
            return TRUE;
          }
        }

        EnumBitwiseFree(result1);
        return FALSE;
        
      }

      case Atm_RedXor_c:
      {
        boolean flag = FALSE;
        
        result1 = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                            varInfo, prevState, state);

        for(i = result1->size-1; i >= 0 ; i--) {
          flag = flag ^ (result1 -> bitArray[i]);
        }

        EnumBitwiseFree(result1);
        return flag;
        
      }

      default:
        fprintf(stderr, "Operation not implemented\n");
        exit(1);
        
  }
}

/**Function********************************************************************

  Synopsis           [Evaluates an expression.]

  Description        [The function evaluates expr at the previous state
                      prevState and the partially updated state. Every
                      expression is evaluated as a long. The enumerated values
                      of a symbolic variable are coded up as natural numbers
                      also.]

  SideEffects        [None]

  SeeAlso            [EnumExprIsTrue, EnumBitvectorExprEvaluate]

******************************************************************************/
long
EnumExprEvaluate(
  Atm_Expr_t *expr,
  Enum_VarInfo_t *varInfo,
  Enum_State_t *prevState,
  Enum_State_t *state)
{
  Atm_ExprType exprType = Atm_ExprReadType(expr);

  /* I should check for overflow/underflow whenever + or - is performed in
     integers and naturals. I am not doing that right now. I need to figure
     out how to do that. Moreover, by casting char * to long, I am assuming
     that long will never be smaller than char *. */

  switch(exprType){
      case Atm_IfThenElseFi_c:
      {
        if(EnumExprIsTrue(Atm_ExprReadLeftChild(expr), varInfo, prevState, state))
          return
              EnumExprEvaluate(Atm_ExprReadLeftChild(Atm_ExprReadRightChild(expr)), varInfo, prevState, state);
        else
          return
              EnumExprEvaluate(Atm_ExprReadRightChild(Atm_ExprReadRightChild(expr)), varInfo, prevState, state);
      }
      
      case Atm_UnaryMinus_c:
      {
        return (-1 * EnumExprEvaluate(Atm_ExprReadLeftChild(expr), varInfo,
                                         prevState, state));
      }
      
      case Atm_Plus_c:
      {
        boolean rangePlus = FALSE;
        Atm_Expr_t *leftExpr, *rightExpr;
        Atm_ExprType leftExprType, rightExprType;
        Var_Variable_t *var;
        int range, value;
        
        /* Check if the addition is on range types */

        leftExpr = Atm_ExprReadLeftChild(expr);
        leftExprType = Atm_ExprReadType(leftExpr);
        rightExpr = Atm_ExprReadRightChild(expr);
        rightExprType = Atm_ExprReadType(rightExpr);

        if(leftExprType == Atm_PrimedVar_c || leftExprType ==
           Atm_UnPrimedVar_c) {
          var = (Var_Variable_t *) Atm_ExprReadLeftChild(leftExpr);
          if(Var_VariableReadDataType(var) == Var_Range_c) {
            rangePlus = TRUE;
            range = Var_VarTypeReadDomainSize(Var_VariableReadType(var));
          }
        }
        else {
          if(rightExprType == Atm_PrimedVar_c || rightExprType ==
             Atm_UnPrimedVar_c) {
            var = (Var_Variable_t *) Atm_ExprReadRightChild(leftExpr);
            if(Var_VariableReadDataType(var) == Var_Range_c) {
              rangePlus = TRUE;
              range = Var_VarTypeReadDomainSize(Var_VariableReadType(var));
            }
          }
        }

        value = EnumExprEvaluate(leftExpr, varInfo, prevState, state) +
            EnumExprEvaluate(rightExpr, varInfo, prevState, state); 

        if(rangePlus) {
          if(value >= 0)
            return value % range;
          else
            return range - (-value) % range;
        }
        else 
          return value;
      }
      
      case Atm_Minus_c:
      {
        boolean rangeMinus = FALSE;
        Atm_Expr_t *leftExpr, *rightExpr;
        Atm_ExprType leftExprType, rightExprType;
        Var_Variable_t *var;
        int range, value;
        
        /* Check if the addition is on range types */

        leftExpr = Atm_ExprReadLeftChild(expr);
        leftExprType = Atm_ExprReadType(leftExpr);
        rightExpr = Atm_ExprReadRightChild(expr);
        rightExprType = Atm_ExprReadType(rightExpr);

        if(leftExprType == Atm_PrimedVar_c || leftExprType ==
           Atm_UnPrimedVar_c) {
          var = (Var_Variable_t *) Atm_ExprReadLeftChild(leftExpr);
          if(Var_VariableReadDataType(var) == Var_Range_c) {
            rangeMinus = TRUE;
            range = Var_VarTypeReadDomainSize(Var_VariableReadType(var));
          }
        }
        else {
          if(rightExprType == Atm_PrimedVar_c || rightExprType ==
             Atm_UnPrimedVar_c) {
            var = (Var_Variable_t *) Atm_ExprReadRightChild(rightExpr);
            if(Var_VariableReadDataType(var) == Var_Range_c) {
              rangeMinus = TRUE;
              range = Var_VarTypeReadDomainSize(Var_VariableReadType(var));
            }
          }
        }

        value = EnumExprEvaluate(leftExpr, varInfo, prevState, state) -
            EnumExprEvaluate(rightExpr, varInfo, prevState, state); 

        if(rangeMinus) {
          if(value >= 0)
            return value % range;
          else
            return range - (-value) % range;
        }
        else 
          return value;
      }

      case Atm_Index_c :
      {
        Atm_Expr_t *varExpr = Atm_ExprReadLeftChild(expr);
        Atm_Expr_t *indexExpr = Atm_ExprReadRightChild(expr);
        Atm_ExprType varExprType = Atm_ExprReadType(varExpr);
        Var_Variable_t *var = (Var_Variable_t *)
            Atm_ExprReadLeftChild(varExpr);
        Var_Variable_t *actualVar;
        char *varName, *actualVarName;
        Var_Type_t *varType, *indexType;
        int value;
        int indexIsEnum = 0;
        char *tmpString;
        
        assert(varExprType == Atm_UnPrimedVar_c || varExprType ==
               Atm_PrimedVar_c);

        varName = Var_VariableReadName(var);
        varType = Var_VariableReadType(var);

        value = EnumExprEvaluate(indexExpr, varInfo, prevState, state);
        
        if (Var_VarTypeReadDataType(varType) == Var_Array_c) {
          indexType = Var_VarTypeReadIndexType(varType);
          if (Var_VarTypeReadDataType(indexType) == Var_Enumerated_c)
            indexIsEnum = 1;
        }
        
        if (indexIsEnum)
          actualVarName = util_strcat4(varName, "[", Var_EnumElementReadName(
            Var_VarTypeReadSymbolicValueFromIndex(indexType, value)), "]");
        else {
          tmpString = ALLOC(char, INDEX_STRING_LENGTH);
          sprintf(tmpString, "%d", value);
          actualVarName = util_strcat4(varName, "[", tmpString, "]");
          FREE(tmpString);
        }
        
        actualVar = Mdl_ModuleReadVariableFromName(actualVarName, varInfo->module);
        FREE(actualVarName);
        
        if (varExprType == Atm_UnPrimedVar_c)
          return Enum_StateReadVariableValue(prevState, actualVar, varInfo);
        else
          return Enum_StateReadVariableValue(state, actualVar, varInfo);
      }
      
      case Atm_UnPrimedVar_c:
      {
        Var_Variable_t *var = (Var_Variable_t *) Atm_ExprReadLeftChild(expr);

        if (var == EnumIndexVar)
          return EnumIndexVarValue;
        else
          return Enum_StateReadVariableValue(prevState, var, varInfo);
      }
      
      case Atm_PrimedVar_c:
      {
        return Enum_StateReadVariableValue(state, (Var_Variable_t *)
                                           Atm_ExprReadLeftChild(expr), varInfo);
      }
      
      case Atm_BoolConst_c:
      case Atm_NumConst_c:
      case Atm_RangeConst_c:
      {
        return (long) Atm_ExprReadLeftChild(expr);
      }

      case Atm_EnumConst_c:
      {
        return (long) Var_EnumElementObtainIndex((Var_EnumElement_t *)
                                                    Atm_ExprReadLeftChild(expr));
      }

      default:
      {
        if(EnumExprIsTrue(expr, varInfo, prevState, state))
          return (long) 1;
        else
          return (long) 0;
      }
      
  }
}


/**Function********************************************************************

  Synopsis           [Evaluates a bitvector expression.]

  Description        [The function evaluates an expression whose value is a
                      bitvector at the previous state
                      prevState and the partially updated state. Every
                      expression is evaluated as the structure EnumBitwise_t.]

  SideEffects        [None]

  SeeAlso            [EnumExprIsTrue, EnumExprEvaluate]

******************************************************************************/
EnumBitwise_t *
EnumBitvectorExprEvaluate(
  Atm_Expr_t *expr,
  Enum_VarInfo_t *varInfo,
  Enum_State_t *prevState,
  Enum_State_t *state)
{
  Atm_ExprType exprType = Atm_ExprReadType(expr);
  Mdl_Module_t *module = varInfo->module;
  int i;
  EnumBitwise_t *result, *result2;
  char *tmpString;
  int numBits;
  
  switch (exprType) {
      case Atm_BitwiseNumConst_c : 
      {
        char *bitArray = (char *) Atm_ExprReadLeftChild(expr);

        numBits = (int) (long) Atm_ExprReadRightChild(expr);
        result = EnumBitwiseAlloc(numBits);
        
        for (i = 0; i < numBits; i++) 
          result->bitArray[i] = bitArray[i];

        return result;
      }
      
      case Atm_BitwisePrimedVar_c :
      case Atm_BitwiseUnPrimedVar_c :
      {
        char *actualVarName, *tmpString;
        Var_Variable_t *actualVar;
        Var_Variable_t *var = (Var_Variable_t *) Atm_ExprReadLeftChild(expr);
        char *varName = Var_VariableReadName(var);
        char value;
        boolean isBitvector;

        numBits =
            Var_VarTypeArrayOrBitvectorReadSize(Var_VariableReadType(var));
        
        result = EnumBitwiseAlloc(numBits);
        
        isBitvector = Var_TypeIsBitvector(Var_VariableReadType(var));

        for (i = 0; i < numBits; i++) {
          tmpString = ALLOC(char, INDEX_STRING_LENGTH);
          sprintf(tmpString, "%d", i);
          actualVarName = util_strcat4(varName, "[", tmpString, "]");
          FREE(tmpString);
          actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);

          value = (char) (exprType == Atm_BitwiseUnPrimedVar_c)? 
              Enum_StateReadVariableValue(state, actualVar, varInfo):
              Enum_StateReadVariableValue(prevState, actualVar, varInfo);
          
          if (isBitvector)
            result->bitArray[i] = value;
          else
            result->bitArray[numBits - 1 - i] = value;
          FREE(actualVarName);
        }
        
        return result;
      }
      
      case Atm_BitwiseIndex_c :
      {
        Atm_Expr_t *varExpr = Atm_ExprReadLeftChild(expr);
        Atm_Expr_t *indexExpr = Atm_ExprReadRightChild(expr);
        Atm_ExprType varExprType = Atm_ExprReadType(varExpr);
        Var_Variable_t *var = (Var_Variable_t *)
            Atm_ExprReadLeftChild(varExpr);
        Var_Variable_t *actualVar, *realVar;
        char *varName, *actualVarName, *realVarName;
        Var_Type_t *varType, *indexType;
        int value;
          
        assert(varExprType == Atm_UnPrimedVar_c || varExprType == Atm_PrimedVar_c);

        varName = Var_VariableReadName(var);
        varType = Var_VariableReadType(var);
        
        value = EnumExprEvaluate(indexExpr, varInfo, prevState, state);
        
        assert(Var_VarTypeReadDataType(varType) == Var_Array_c);
        
        indexType = Var_VarTypeReadIndexType(varType);
        numBits =
            Var_VarTypeArrayOrBitvectorReadSize(Var_VarTypeReadEntryType(varType));
        
        if (Var_VarTypeReadDataType(indexType) == Var_Enumerated_c) {
          actualVarName = util_strcat4(varName, "[", Var_EnumElementReadName(
            Var_VarTypeReadSymbolicValueFromIndex(indexType, value)), "]");
        } else {
          tmpString = ALLOC(char, INDEX_STRING_LENGTH);
          sprintf(tmpString, "%d", value);
          actualVarName = util_strcat4(varName, "[", tmpString, "]");
          FREE(tmpString);
        }
        
        actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
        
        result = EnumBitwiseAlloc(numBits);
        
        for (i = 0; i < numBits; i++) {
          tmpString = ALLOC(char, INDEX_STRING_LENGTH);
          sprintf(tmpString, "%d", i);
          realVarName = util_strcat4(actualVarName, "[", tmpString, "]");
          FREE(tmpString);
          realVar = Mdl_ModuleReadVariableFromName(realVarName, module);
          if (varExprType == Atm_UnPrimedVar_c)
            result->bitArray[i] = (char) Enum_StateReadVariableValue(prevState, realVar, varInfo);
          else
            result->bitArray[i] = (char) Enum_StateReadVariableValue(state, realVar, varInfo);
          FREE(realVarName);
        }

        FREE(actualVarName);
        return result;
      }
      
      case Atm_BitwiseNot_c :
      {
        result = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                           varInfo, prevState, state);
        for (i = 0; i < result->size; i++)
          if (result->bitArray[i])
            result->bitArray[i] = 0;
          else
            result->bitArray[i] = 1;
        
        return result;
      }
  

      case Atm_BitwiseAnd_c :
      {
        result = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                           varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for (i = 0; i < result->size; i++)
          if (!result2->bitArray[i])
            result->bitArray[i] = 0;

        EnumBitwiseFree(result2);
        return result;
      }
      
      case Atm_BitwiseOr_c :
      {
        result = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                           varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for (i = 0; i < result->size; i++)
          if (result2->bitArray[i])
            result->bitArray[i] = 1;

        EnumBitwiseFree(result2);
        return result;
      }
      
      case Atm_BitwiseEquiv_c : 
      {
        result = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                           varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for (i = 0; i < result->size; i++)
          if (result->bitArray[i] == result2->bitArray[i])
            result->bitArray[i] = 1;
          else
            result->bitArray[i] = 0;

        EnumBitwiseFree(result2);
        return result;
      }

      case Atm_BitwiseNotEquiv_c :
      case Atm_BitwiseXor_c : 
      {
        result = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                           varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for (i = 0; i < result->size; i++)
          if (result->bitArray[i] != result2->bitArray[i]) 
            result->bitArray[i] = 1;
          else
            result->bitArray[i] = 0;

        EnumBitwiseFree(result2);
        return result;
      }

      case Atm_BitwiseImplies_c :
      {
        result = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                           varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for (i = 0; i < result->size; i++)
          result -> bitArray[i] = !((result  -> bitArray[i]) ||
                                    (result2 -> bitArray[i]));
          
        EnumBitwiseFree(result2);
        return result;
      }
      
      case Atm_BitwiseIfThenElseFi_c : 
      {
        if (EnumExprIsTrue(Atm_ExprReadLeftChild(expr), varInfo, prevState, state))
          return EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(Atm_ExprReadRightChild(expr)),
                                           varInfo, prevState, state);
        else
          return EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(Atm_ExprReadRightChild(expr)),
                                           varInfo, prevState, state);
      }
      
      case Atm_BitwisePlus_c :
      {
        int carry = 0;
        int nextResult;
        int nextCarry;
        
        result = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                           varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for (i = 0; i < result->size; i++) {
          nextCarry = (carry & (result->bitArray[i] | result2->bitArray[i])) | (result->bitArray[i] & result2->bitArray[i]);
          nextResult = (carry & ~result->bitArray[i] & ~result2->bitArray[i]) | (~carry & result->bitArray[i] & ~result2->bitArray[i]) |
              (~carry & ~result->bitArray[i] & result2->bitArray[i]) | (carry & result->bitArray[i] & result2->bitArray[i]);

          carry = nextCarry;
          result->bitArray[i] = nextResult;
        }
                
        EnumBitwiseFree(result2);
        return result;
      }

      case Atm_BitwiseMinus_c :
      {
        int carry = 1;
        int nextResult;
        int nextCarry;
        
        result = EnumBitvectorExprEvaluate(Atm_ExprReadLeftChild(expr),
                                           varInfo, prevState, state);
        result2 = EnumBitvectorExprEvaluate(Atm_ExprReadRightChild(expr),
                                            varInfo, prevState, state);
        for (i = 0; i < result->size; i++)
          if (result2->bitArray[i])
            result2->bitArray[i] = 0;
          else
            result2->bitArray[i] = 1;
        

        for (i = 0; i < result->size; i++) {
          nextCarry = (carry & (result->bitArray[i] | result2->bitArray[i])) | (result->bitArray[i] & result2->bitArray[i]);
          nextResult = (carry & ~result->bitArray[i] & ~result2->bitArray[i]) | (~carry & result->bitArray[i] & ~result2->bitArray[i]) |
              (~carry & ~result->bitArray[i] & result2->bitArray[i]) | (carry & result->bitArray[i] & result2->bitArray[i]);

          carry = nextCarry;
          result->bitArray[i] = nextResult;
        }
                
        EnumBitwiseFree(result2);
        return result;
      }
  }
}


/**Function********************************************************************

  Synopsis           [The function sets up the execution of a list of
                      assignments from an init guarded command of an atom on a
		      partial state.]

  Description        [The function takes an atom, a list of assignments, and a
                      partial state and produces a list of states generated
		      from the state by executing the assignments in the list.
		      First, the nondet assignments are removed from assignList
		      and the rest of the assignments are executed. The remaining
		      controlled variables including the ones that had nondet
		      assigned to them are then chosen nondeterministically.]

  SideEffects        [None]

  SeeAlso            [EnumUpdateAssignListExecuteSetup]

******************************************************************************/
lsList
EnumInitAssignListExecuteSetup(
  Atm_Atom_t *atom,
  Enum_VarInfo_t *varInfo,
  lsList assignList,			  
  Enum_State_t *state) {
   
   
   lsList unassignedVars = lsCreate();
   lsList newAssignList = AssignListRemoveNondetAssignment(assignList, unassignedVars);
   lsList newList;
   Enum_State_t *dupState = Enum_StateDup(state, varInfo);
   lsGen varGen;
   lsGeneric var;
   lsList ctrlVarList = Atm_AtomReadControlVarList(atom);

   /* The following is used for processing index variable in a forall assignment. */
   EnumIndexVar = NIL(Var_Variable_t);
   EnumIndexVarValue = -1;

   EnumAssignListExecuteOnState(newAssignList, varInfo, NIL(Enum_State_t), dupState);
   
   /* Find the subset of controlled non-event variables of this atom */
   /* that have not been assigned a value by the execution of guarded */
   /* commands. We need not consider event variables because we are */
   /* considering only initial states here. */ 
   
   lsForEachItem(ctrlVarList, varGen, var) {
      if (Var_VariableReadDataType((Var_Variable_t *) var) != Var_Event_c &&
	  !Enum_StateIsVariableAssigned(dupState, (Var_Variable_t *) var, varInfo) &&
	  !IsInList(unassignedVars, var)) {
	 lsNewEnd(unassignedVars, var, LS_NH);
      }
   }
   
   if (lsLength(unassignedVars) > 0) {
      newList = Enum_StateAssignAllVarCombinations(dupState, unassignedVars, varInfo);
      Enum_StateFree(dupState);
   }
   else {
      newList = lsCreate();
      lsNewEnd(newList, (lsGeneric) dupState, LS_NH);
   }

   lsDestroy(newAssignList, NULL);
   lsDestroy(unassignedVars, NULL);
   
   return newList;      
}

/**Function********************************************************************

  Synopsis           [The function sets up the execution of a list of
                      assignments from an update guarded command of an atom on a
		      partial state.]

  Description        [The function takes an atom, a list of assignments, a state
                      and a partially evaluated state, and produces a list of
		      states generated from the state by executing the assignments
		      in the list. First, the nondet assignments are removed from
		      assignList and the rest of the assignments are executed. Of
		      the remaining variables, those that have nondet assigned to
		      them or are unassigned and are not read by the atom are
		      updated nondeterministically and the others have the value
		      from the previous state copied.]

  SideEffects        [None]

  SeeAlso            [EnumInitAssignListExecuteSetup]

******************************************************************************/
lsList
EnumUpdateAssignListSetup(
  Atm_Atom_t *atom,
  Enum_VarInfo_t *varInfo,
  lsList assignList,
  Enum_State_t *state,
  Enum_State_t *partialState)
{
   lsList nondetVarList = lsCreate();
   lsList newAssignList = AssignListRemoveNondetAssignment(assignList, nondetVarList);
   Enum_State_t *dupState = Enum_StateDup(partialState, varInfo);
   lsList ctrlVarList = Atm_AtomReadControlVarList(atom);
   lsGen varGen;
   lsGeneric var;
   lsList newList;
   int value;

   /* The following is used for processing index variable in a forall assignment. */
   EnumIndexVar = NIL(Var_Variable_t);
   EnumIndexVarValue = -1;

   EnumAssignListExecuteOnState(newAssignList, varInfo, state, dupState);
      
   lsForEachItem(ctrlVarList, varGen, var) {
      if(!Enum_StateIsVariableAssigned(dupState, (Var_Variable_t *) var, varInfo) &&
	 !IsInList(nondetVarList, var)) { 
	 if (IsInList(Atm_AtomReadReadVarList(atom), var)) {
	    if(Var_VariableReadDataType((Var_Variable_t *) var) !=
	       Var_Event_c) {
	       value = Enum_StateReadVariableValue(state, (Var_Variable_t *)
						   var, varInfo);
	       Enum_StateSetVariableValue(dupState, (Var_Variable_t *) var,
					  varInfo, value);
	    }
	    else {
	       Enum_StateSetVariableValue(dupState, (Var_Variable_t *) var,
					  varInfo, 0);
	    }
	 }
	 else {
	    lsNewEnd(nondetVarList, var, LS_NH);
	 }
      }
   }
   
   if (lsLength(nondetVarList) > 0) {
      newList = Enum_StateAssignAllVarCombinations(dupState, nondetVarList, varInfo);
      Enum_StateFree(dupState);
   }
   else {
      newList = lsCreate();
      lsNewEnd(newList, (lsGeneric) dupState, LS_NH);
   }
   
   lsDestroy(nondetVarList, NULL);
   lsDestroy(newAssignList, NULL);

   return newList;
}


/**Function********************************************************************

  Synopsis           [Executes a sub-round in an update round.]

  Description        [The function updates state in according to the
                      assignments in assignList.] 

  SideEffects        [state is updated.]

******************************************************************************/
void
EnumAssignListExecuteOnState(
  lsList assignList,
  Enum_VarInfo_t *varInfo,
  Enum_State_t *prevState,
  Enum_State_t *state)
{
  lsGen gen ;
  lsGeneric assign;
  Var_Variable_t *var, *actualVar, *realVar;
  char *varName, *actualVarName, *realVarName;
  Atm_Expr_t *expr;
  long value;
  Atm_AssignType assignType;
  EnumBitwise_t *result;
  int i;
  Mdl_Module_t *module = varInfo->module;
  Var_Type_t *varType;
  char *tmpString;
  
  lsForEachItem(assignList, gen,  assign) {
    var = Atm_AssignReadVariable((Atm_Assign_t *) assign);
    varType = Var_VariableReadType(var);
    varName = Var_VariableReadName(var);
    assignType = Atm_AssignReadType((Atm_Assign_t *) assign);
    expr = Atm_AssignReadExpr((Atm_Assign_t *) assign);
    
    switch (assignType) {

        case Atm_AssignStd_c : 
        {
          assert(!Var_TypeIsArray(varType));
          
          if (Var_TypeIsBV(varType)) {
            boolean isBitvector = Var_TypeIsBitvector(varType);

            result = EnumBitvectorExprEvaluate(expr, varInfo, prevState,
                                               state);
            for (i = 0; i < result->size; i++) {
              tmpString = ALLOC(char, INDEX_STRING_LENGTH);
              sprintf(tmpString, "%d", i);
              actualVarName = util_strcat4(varName, "[", tmpString, "]");
              FREE(tmpString);
              actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
              FREE(actualVarName);

              if (isBitvector) 
                Enum_StateSetVariableValue(state, actualVar, varInfo, (long)
                                           result->bitArray[i]);
              else
                Enum_StateSetVariableValue(state, actualVar, varInfo, (long)
                                           result->bitArray[result->size - 1 - i]);
            }
            EnumBitwiseFree(result);
          } else {
            value = EnumExprEvaluate(expr, varInfo, prevState, state);
            Enum_StateSetVariableValue(state, var, varInfo, value);
          }
          
          break;
        }
        
        case Atm_AssignEvent_c : 
        {
          Enum_StateSetVariableValue(state, var, varInfo, 1);
          break;
        }
        
        case Atm_AssignForall_c :
        {
          int size, i;
          Var_EnumElement_t *indexValue;
          Var_Type_t *indexType, *entryType;
          
          EnumIndexVar = Atm_AssignReadIndexVar((Atm_Assign_t *) assign);
          indexType = Var_VariableReadType(EnumIndexVar);
          entryType = Var_VarTypeReadEntryType(varType);
          size = Var_VarTypeReadDomainSize(Var_VariableReadType(EnumIndexVar));

          assert(Var_TypeIsEnum(indexType) || Var_TypeIsRange(indexType));
          
          if (Var_TypeIsEnum(indexType)) {
            Var_VariableForEachSymbolicValue(EnumIndexVar, i, indexValue) {
              EnumIndexVarValue = i;
              
              actualVarName = util_strcat4(varName, "[", Var_EnumElementReadName(
                Var_VarTypeReadSymbolicValueFromIndex(indexType, i)), "]"); 
              actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
              
              if (Var_TypeIsBV(entryType)) {
                boolean isBitvector = Var_TypeIsBitvector(entryType);
                
                result = EnumBitvectorExprEvaluate(expr, varInfo, prevState,
                                                   state);
                for (i = 0; i < result->size; i++) {
                  tmpString = ALLOC(char, INDEX_STRING_LENGTH);
                  sprintf(tmpString, "%d", i);
                  realVarName = util_strcat4(actualVarName, "[", tmpString, "]");
                  FREE(tmpString);
                  realVar = Mdl_ModuleReadVariableFromName(realVarName, module);
                  FREE(realVarName);
                  if (isBitvector) 
                    Enum_StateSetVariableValue(state, realVar, varInfo, (long)
                                               result->bitArray[i]);
                  else
                    Enum_StateSetVariableValue(state, realVar, varInfo, (long)
                                               result->bitArray[result->size -
                                                               1 - i]);
                    
                }
                EnumBitwiseFree(result);
              }
              else {
                value = EnumExprEvaluate(expr, varInfo, prevState, state);
                Enum_StateSetVariableValue(state, actualVar, varInfo, value);
              }
              
              FREE(actualVarName);
            }
          } else {
            for (i = 0; i < size; i++) {
              EnumIndexVarValue = i;
              
              tmpString = ALLOC(char, INDEX_STRING_LENGTH);
              sprintf(tmpString, "%d", i);
              actualVarName = util_strcat4(varName, "[", tmpString, "]");
              FREE(tmpString);
              
              actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);

              if (Var_TypeIsBV(entryType)) {
                int j;
                boolean isBitvector = Var_TypeIsBitvector(entryType);

                result = EnumBitvectorExprEvaluate(expr, varInfo, prevState,
                                                   state);
                for (j = 0; j < result->size; j++) {
                  tmpString = ALLOC(char, INDEX_STRING_LENGTH);
                  sprintf(tmpString, "%d", j);
                  realVarName = util_strcat4(actualVarName, "[", tmpString, "]");
                  FREE(tmpString);
                  realVar = Mdl_ModuleReadVariableFromName(realVarName, module);
                  FREE(realVarName);
                  if (isBitvector) 
                    Enum_StateSetVariableValue(state, realVar, varInfo, (long)
                                               result->bitArray[j]);
                  else
                    Enum_StateSetVariableValue(state, realVar, varInfo, (long)
                                               result->bitArray[result->size -
                                                               1 - j]);

                }
                EnumBitwiseFree(result);
              }
              else {
                value = EnumExprEvaluate(expr, varInfo, prevState, state);
                Enum_StateSetVariableValue(state, actualVar, varInfo, value);
              }
              
              FREE(actualVarName);                         
            }            
          }
          
          EnumIndexVar = NIL(Var_Variable_t);
          EnumIndexVarValue = -1;
          break;
        }

        case Atm_AssignIndex_c : 
        default:
	   Main_MochaErrorPrint("Unexpected assignment type.\n");
	   exit(1);
    }

  }
}


/**Function********************************************************************

  Synopsis           [Allocates an EnumBitwise_t structure.]

  SideEffects        [None]

  SeeAlso            [EnumBitwiseFree]

******************************************************************************/
EnumBitwise_t *
EnumBitwiseAlloc(
  int size)
{
  EnumBitwise_t *a = ALLOC(EnumBitwise_t, 1);

  a->bitArray = ALLOC(char, size);
  a->size = size;

  return a;
}

/**Function********************************************************************

  Synopsis           [Frees an EnumBitwise_t structure.]

  SideEffects        [None]

  SeeAlso            [EnumBitwiseAlloc]

******************************************************************************/
void
EnumBitwiseFree(
  EnumBitwise_t *a)
{
  FREE(a->bitArray);
  FREE(a);
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Check if an item is in a list.]

  Description        [The function returns TRUE if item is found in list,
                      otherwise it returns FALSE.]

  SideEffects        [None]

******************************************************************************/
static boolean
IsInList (
  lsList list,
  lsGeneric item)
{
  lsGen lsgen;
  lsGeneric item1;
  
  lsForEachItem(list, lsgen, item1){
    if (item1 == item) {
      lsFinish(lsgen);
      return TRUE;
    }
  }
  return FALSE;    
}

/**Function********************************************************************

  Synopsis           [Removes all nondet assignments from assignList.]

  Description        [The function those assignments from assignList that do
                      not involve nondet. It adds all variables to which nondet
		      assignment was done to the list nondetVarList.]

  SideEffects        [None]

******************************************************************************/
static lsList
AssignListRemoveNondetAssignment(
  lsList assignList,
  lsList nondetVarList) 
{
  lsGeneric assign;
  lsGen gen;
  lsList newAssignList = lsCreate();
  
  lsForEachItem(assignList, gen, assign) {
    Atm_AssignType assignType = Atm_AssignReadType((Atm_Assign_t *) assign);
    
    if (assignType == Atm_AssignEvent_c) {
      lsNewEnd(newAssignList, assign, LS_NH);
    }
    else {
      Atm_Expr_t *expr = Atm_AssignReadExpr((Atm_Assign_t *) assign);

      if (Atm_ExprReadType(expr) == Atm_Nondet_c) {
        switch (assignType) {
            case Atm_AssignStd_c :
            case Atm_AssignForall_c :
              VariablesAddRecursively(nondetVarList,
                                      Atm_AssignReadVariable((Atm_Assign_t *) assign));
              break;
              
            default:
              /* Atm_AssignIndex_c is not used currently and nondet assignment */
              /* does not make sense for Atm_AssignEvent_c. */
              Main_MochaErrorPrint("Error in enum: Unexpected assingment type\n");
              exit(1);
        }   
      }
      else {
        lsNewEnd(newAssignList, assign, LS_NH);
      }
    }
  }

  return newAssignList;
}


/**Function********************************************************************

  Synopsis           [Adds atomic variables in the variable var to varList.]

  Description        [The function flattens compound variables like bitvectors
                      and arrays into the component variables and adds these
		      variables to varList. The function is invoked by
		      AssignListRemoveNondetAssignment.]

  SideEffects        [None]

  SeeAlso            [AssignListRemoveNondetAssignment]

******************************************************************************/
static void
VariablesAddRecursively(
  lsList varList,
  Var_Variable_t *var) 
{
  Mdl_Module_t *module = (Mdl_Module_t *) Var_VariableReadModule(var);
  Var_Type_t *varType = Var_VariableReadType(var);
  char *varName = Var_VariableReadName(var);
  int i, size;
  char *actualVarName;
  Var_Variable_t *actualVar;

  
  if (Var_TypeIsArray(varType)) {
    Var_Type_t *indexType = Var_VarTypeReadIndexType(varType);
    int indexIsEnum = 0;
    
    size = Var_VarTypeArrayOrBitvectorReadSize(varType);
    if (Var_TypeIsEnum(indexType)) 
      indexIsEnum = 1;
    
    if (!indexIsEnum)
      actualVarName = ALLOC(char, strlen(varName) + 1 +
                            INDEX_STRING_LENGTH + 1 + 1);
                
    for (i = 0; i < size; i++) {
      if (indexIsEnum)
        actualVarName = util_strcat4(varName, "[", Var_EnumElementReadName(
          Var_VarTypeReadSymbolicValueFromIndex(indexType, i)), "]");
      else
        sprintf(actualVarName, "%s[%d]", varName, i);

      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
      VariablesAddRecursively(varList, actualVar);
      if (indexIsEnum)
        FREE(actualVarName);
    }
    
    if (!indexIsEnum)
      FREE(actualVarName);
    return;
  }

  if (Var_TypeIsBV(varType)) {
    size = Var_VarTypeArrayOrBitvectorReadSize(varType);
    actualVarName = ALLOC(char, strlen(varName) + 1 +
                          INDEX_STRING_LENGTH + 1 + 1);
                
    for (i = 0; i < size; i++) {
      sprintf(actualVarName, "%s[%d]", varName, i);
      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
      VariablesAddRecursively(varList, actualVar);
    }

    FREE(actualVarName);
    return;
  }

  lsNewEnd(varList, (lsGeneric) var, LS_NH);
  return;
}


