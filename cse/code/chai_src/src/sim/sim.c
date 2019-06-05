/**CFile***********************************************************************

  FileName    [sim.c]

  PackageName [sim]

  Synopsis    [Functions for doing module simulations]

  Description [This file contains the routines for module simulations.]

  SeeAlso     [optional]

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

#include  "simInt.h"

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

static void SimListConcatenate(lsList list1, lsList list2);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Read the name of the simAtom]

  Description        [This function returns the name of the simAtom.]

  SideEffects        [User should not free the returned string.]

  SeeAlso            [optional]

******************************************************************************/
char*
Sim_SimAtomReadName(
  Sim_Atom_t* simAtom)
{
  return simAtom->name;
}

/**Function********************************************************************

  Synopsis           [Returns true if the simAtom is an user atom.]

  Description        [This function returns true if the simAtom is an user
  atom, i.e. atom controlled by the user.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Sim_SimAtomReadIsUserAtom(
  Sim_Atom_t* simAtom)
{
  return simAtom->isUserAtom;
}

/**Function********************************************************************

  Synopsis           [Returns the atom that is represented by this simAtom.]

  Description        [This function returns a pointer to the atom that is
  represented by this simAtom.]

  SideEffects        [User should not free the returned pointer.]

  SeeAlso            [optional]

******************************************************************************/
Atm_Atom_t*
Sim_SimAtomReadAtom(
  Sim_Atom_t* simAtom)
{
  return simAtom->atom;
}

/**Function********************************************************************

  Synopsis           [Returns a sorted array of the simAtoms.]

  Description        [Given a module, an array of users atoms (Atm_Atom_t
  *), and the type of the atoms (are they userAtoms?), this routine creates
  the simAtoms for the module and returns an array of topologically sorted
  simAtoms. Note that the non-user atoms will come as late as possible in the
  linearization so that the user will have the least knowledge of how the
  non-user atom behaves.]

  SideEffects        [The atomDependencyInfo will be updated. The sorted array
  will be returned. User should free the array (Sim_SimAtomArrayFree) after
  use. ]

  SeeAlso            [Sim_SimAtomArrayFree]

******************************************************************************/
array_t * 
Sim_SimAtomReturnSortedArray(
  Mdl_Module_t * module,
  array_t * atomArray, /* array of Atm_Atom_t * */
  int isUserAtom)
{
  array_t * simAtomSortedArray, * simReturnAtomArray;
  SimAtomDependencyInfo_t * simAtomDependencyInfo;
  int i;

  
  simAtomDependencyInfo = SimReadAtomDependencyInfo(simManager);
  
  SimAtomDependencyInfoUpdate(module, simAtomDependencyInfo);
  SimDependencyInfoSetIsUserAtom(simAtomDependencyInfo, atomArray,
                                 isUserAtom);
  simAtomSortedArray = SimTopologicalSort(simAtomDependencyInfo);

  /* make a copy of the array, since the content would be
     freed later */
  simReturnAtomArray = array_alloc(Sim_Atom_t*, 0);
  for(i=0; i<array_n(simAtomSortedArray); i++){
    Sim_Atom_t *simAtom =
        array_fetch(Sim_Atom_t*, simAtomSortedArray, i);
    
    array_insert_last(Sim_Atom_t *, simReturnAtomArray, SimAtomDup(simAtom));
  }

  array_free(simAtomSortedArray);

  return simReturnAtomArray;
  
}

/**Function********************************************************************

  Synopsis           [Frees an array of simAtoms.]

  Description        [This routine frees an array of simAtoms. This is
  useful to free the array returned by Sim_SimAtomReturnSortedArray.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Sim_SimAtomArrayFree(
  array_t *simAtomArray)
{

  if (simAtomArray) {
    int i;
    int n = array_n (simAtomArray);
    for (i=0; i<n; i++){
      Sim_Atom_t * simAtom = array_fetch(Sim_Atom_t*, simAtomArray, i);
      SimAtomFree(simAtom);
    }
    array_free(simAtomArray);
  }
  
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Allocates a SimAtom.]

  Description        [It allocates a SimAtom and setting its fields.]

  SideEffects        [required]

  SeeAlso            [SimAtomFree]

******************************************************************************/
Sim_Atom_t *
SimAtomAlloc(
  Atm_Atom_t * atom
  )
{

  Sim_Atom_t * simAtom = ALLOC(Sim_Atom_t, 1);
  if (atom != NIL(Atm_Atom_t)){
    simAtom -> name = util_strsav(Atm_AtomReadName(atom));
  }
  
  simAtom -> atom = atom; 
  simAtom -> successorAtomList = array_alloc(Sim_Atom_t *, 0);
  simAtom -> isUserAtom = FALSE;
  simAtom -> numAwaitingAtom = 0;  

  return simAtom;
}

/**Function********************************************************************

  Synopsis           [Frees an SimAtom.]

  Description        [Frees an SimAtom.]

  SideEffects        [required]

  SeeAlso            [SimAtomAlloc]

******************************************************************************/
void
SimAtomFree(
  Sim_Atom_t * simAtom) 
{
  FREE(simAtom->name);
  array_free(simAtom->successorAtomList);
  FREE(simAtom);
}


/**Function********************************************************************

  Synopsis           [Start Simulation for a module]

  Description        [This function starts the simulation for a given module.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
SimSimulationStart(
  Mdl_Module_t * module,
  array_t *atomArray,
  boolean isUserAtom)
{
  Sim_Info_t * simInfo;
  array_t *simAtomOrder;
  int i;
  
  if (!module)
    return FALSE;
  
  /* now, allow only one simulation per module */
  simInfo = SimManagerReadSimInfo(Mdl_ModuleReadName(module));
  if (simInfo) {
    Main_MochaErrorPrint(
      "SIM: another simulation of the same module is running.\n");
    Main_MochaErrorPrint(
      "SIM: please exit the other one and restart simulation again.\n");
    return FALSE;
  }

  simAtomOrder = Sim_SimAtomReturnSortedArray(module, atomArray, isUserAtom);

  if(array_n(simAtomOrder) == 0) {
    /* this is an empty module */
    Main_MochaErrorPrint(
      "SIM: simulation cannot be initiated for an empty module.\n");
    return FALSE;
  }
  
  simInfo = SimSimInfoAlloc(module);
  SimSimInfoAddToManager(simInfo);

  SimSimInfoSetSimAtomOrder(simInfo, simAtomOrder);
  SimComputeChoice(simInfo);
  
  return TRUE;
}

/**Function********************************************************************

  Synopsis           [Initialize the global sim_manager]

  Description        [This routine initializes the global simulation
  manager. One point to note is that the simulation manager is only visible
  within the sim package.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimManagerInit()
{
  simManager = ALLOC(Sim_Manager_t,1);
  simManager -> nameToSimInfo = st_init_table (strcmp, st_strhash);

  simManager -> simAtomDependencyInfo = ALLOC(SimAtomDependencyInfo_t, 1);  
  simManager -> simAtomDependencyInfo -> moduleName = util_strsav("");
  simManager -> simAtomDependencyInfo -> nameToSimAtom =
      st_init_table(strcmp, st_strhash);
  simManager -> simAtomDependencyInfo -> moduleCreationTime = -1;

}

/**Function********************************************************************

  Synopsis           [Returns the simInfo of the module.]

  Description        [This routine returns the simInfo of the given module. If
  the module has no simInfo, ie. not simulated before, this routine returns
  NIL. ]

  SideEffects        [Should not free the pointer.]

  SeeAlso            [optional]

******************************************************************************/
Sim_Info_t*
SimManagerReadSimInfo(
  char* moduleName)
{
  Sim_Info_t* simInfo;
  st_table * nameToSimInfo = simManager->nameToSimInfo;

  if (!moduleName)
    return NIL(Sim_Info_t);
  
  if(st_lookup(nameToSimInfo, moduleName, (char **) &simInfo))
    return simInfo;
  else
    return NIL(Sim_Info_t);
}

/**Function********************************************************************

  Synopsis           [Delete the simInfo of the module.]

  Description        [This routine deletes the simInfo of the module from the
  simulation manager. It returns a pointer to the simInfo if it is present,
  and NIL otherwise. ]

  SideEffects        [The user should free the returned pointer after use.]

  SeeAlso            [optional]

******************************************************************************/
Sim_Info_t*
SimManagerDeleteSimInfo(
  char* moduleName)
{
  Sim_Info_t* simInfo;
  st_table * nameToSimInfo = simManager->nameToSimInfo;

  if (!moduleName)
    return NIL(Sim_Info_t);
  
  if(st_delete(nameToSimInfo, (char **) &moduleName, (char **) &simInfo))
    return simInfo;
  else
    return NIL(Sim_Info_t);
}


/**Function********************************************************************

  Synopsis           [Allocate a simInfo for a module]

  Description        [Allocates and initialize the simInfo for the given
  module. ]

  SideEffects        [Should free the returned pointer after use.]

  SeeAlso            [SimSimInfoFree]

******************************************************************************/
Sim_Info_t*
SimSimInfoAlloc(
  Mdl_Module_t * module)
{
  Sim_Info_t *simInfo;
  
  if (!module)
    return NIL(Sim_Info_t);

  simInfo = ALLOC(Sim_Info_t,1);
  simInfo -> name = util_strsav(Mdl_ModuleReadName(module));
  simInfo -> creationTime = Mdl_ModuleReadCreationTime(module);
  simInfo -> simAtomOrder = NIL(array_t);

  simInfo -> varInfo = Enum_VarInfoAlloc(module);
  simInfo -> prevState = NIL(Enum_State_t);
  simInfo -> partialState = Enum_StateAlloc(simInfo -> varInfo);
  simInfo -> choice = (lsList) 0;
  simInfo -> index = 0;

  return simInfo;
}

/**Function********************************************************************

  Synopsis           [Frees a simInfo]

  Description        [It frees up the simInfo.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimSimInfoFree(
  Sim_Info_t* simInfo)
{
  if (simInfo) {
    FREE(simInfo -> name);

    /* frees the simAtomOrder */
    if (simInfo -> simAtomOrder) {
      Sim_SimAtomArrayFree(simInfo->simAtomOrder);
    }
    
    /* frees the state information */
    if (simInfo->prevState)
      Enum_StateFree(simInfo -> prevState);
    if (simInfo->partialState)
      Enum_StateFree(simInfo -> partialState);
    if (simInfo -> choice)
      lsDestroy(simInfo -> choice, Enum_StateFree);

    Enum_VarInfoFree(simInfo -> varInfo);
    
    FREE(simInfo);
  }
  
}

/**Function********************************************************************

  Synopsis           [Add the simInfo to the Manager]

  Description        [It adds the simInfo to the simulation manager.]

  SideEffects        [required]

  SeeAlso            [SimManagerDeleteSimInfo]

******************************************************************************/
void
SimSimInfoAddToManager(
  Sim_Info_t* simInfo)
{
  if (simInfo) {
    st_insert(simManager->nameToSimInfo,
              SimSimInfoReadName(simInfo),
              (char *) simInfo);
  }
  
}

/**Function********************************************************************
   
  Synopsis           [Sets the Index field in the simInfo]

  Description        [Sets the Index field in the simInfo. The index field
  indicates which atom to execute next. If the module is empty, the index
  field has value -1]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimSimInfoSetIndex(
  Sim_Info_t * simInfo,
  int index)
{
  simInfo -> index = index;
}

/**Function********************************************************************
   
  Synopsis           [Sets the Choice field in the simInfo]

  Description        [Sets the Choice field in the simInfo. The choice field
  is a lsList of Enum_States, which are the possible extension of the current
  partial state. Partial state is stored in the partialState field of the
  SimInfo. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimSimInfoSetChoice(
  Sim_Info_t * simInfo,
  lsList choice)
{
  simInfo -> choice = choice;
}


/**Function********************************************************************

  Synopsis           [Sets the SimAtomOrder field in the simInfo.]

  Description        [The SimAtomOrder is an array of Sim_Atoms, which are
  ordered according to the sequence of execution of the atoms. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimSimInfoSetSimAtomOrder(
  Sim_Info_t * simInfo,
  array_t* simAtomOrder)
{
  simInfo -> simAtomOrder = simAtomOrder;
}

/**Function********************************************************************

  Synopsis           [Sets the PrevState field.]

  Description        [The PrevState field is a Enum_State that stores the
  previous state.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimSimInfoSetPrevState(
  Sim_Info_t* simInfo,
  Enum_State_t* prevState)
{
  simInfo -> prevState = prevState;
}

/**Function********************************************************************

  Synopsis           [Sets the PartialState field.]

  Description        [The PartialState is a Enum_State that stores the
  partialState.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimSimInfoSetPartialState(
  Sim_Info_t * simInfo,
  Enum_State_t * partialState)
{
  simInfo-> partialState = partialState;

}

/**Function********************************************************************

  Synopsis           [Sets the varInfo field.]

  Description        [The varInfo is a Enum_VarInfo which stores the
  module infomation needed by the enum package for executing the module.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimSimInfoSetVarInfo(
  Sim_Info_t * simInfo,
  Enum_VarInfo_t * varInfo)
{
  simInfo-> varInfo = varInfo;

}


/**Function********************************************************************

  Synopsis           [Compute the choice, from the partialState and prevState.]

  Description        [This computes the choice from the partialState and
  prevState in the simInfo. The choice is a list of (partial) Enum_States, and
  it is then stored in the Choice field of the simInfo.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimComputeChoice(
  Sim_Info_t * simInfo)
{

  Enum_State_t * prevState = SimSimInfoReadPrevState(simInfo);
  Enum_State_t * partialState = SimSimInfoReadPartialState(simInfo);
  Enum_VarInfo_t * varInfo = SimSimInfoReadVarInfo(simInfo);
  Sim_Atom_t* simAtom;
  lsList allChoice, choice = SimSimInfoReadChoice(simInfo);
  int isUserAtom, index, numSimAtoms;
  boolean done;
  array_t *simAtomArray;
  char *name;
  lsGen gen;
    
  index = SimSimInfoReadIndex(simInfo);

  if (index == -1) {
    /* this is an empty module */
    return;
  }

  simAtomArray = SimSimInfoReadSimAtomOrder(simInfo);
  numSimAtoms = array_n(simAtomArray);

  simAtom = array_fetch(Sim_Atom_t*, simAtomArray, index);
  isUserAtom = Sim_SimAtomReadIsUserAtom(simAtom);
  name = Sim_SimAtomReadName(simAtom);

  if (choice) {
    lsDestroy(choice, Enum_StateFree); 
  }
  
  allChoice = lsCreate();
  lsNewEnd(allChoice, (lsGeneric) Enum_StateDup(partialState, varInfo), LS_NH);

  done = FALSE;
  while (!done) {
    lsList tmpChoice;
    Enum_State_t * state;
    
    choice = lsCreate();
    
   /*  Main_MochaErrorPrint("executing atom: %s\n", name); */
    lsForEachItem(allChoice, gen, state) {
      if (SimSimAtomIsExternalVariable(simAtom)) { /* external atom */
        lsList varList = lsCreate();
        
        lsNewEnd(varList, (lsGeneric) Sim_SimAtomReadAtom(simAtom), LS_NH);
        
        tmpChoice =
            Enum_StateAssignAllVarCombinations(
              state, varList, varInfo);
        lsDestroy(varList, 0);
      } else {
        if (!prevState) {
          tmpChoice = 
              Enum_AtomComputeSetOfPartialInitialStates(
                Sim_SimAtomReadAtom(simAtom), varInfo, state);
        } else {
          tmpChoice = 
              Enum_AtomComputeSetOfPartialPostStates(
                Sim_SimAtomReadAtom(simAtom), varInfo, prevState, state);
        }
        
      }
      
      if (lsLength(allChoice) >= MAXNUMCHOICE) {
        /* if hit the limit, then just generate one out of random */
        int idex = random() % lsLength(tmpChoice);
        lsGen gen;
        lsGeneric data;
        
        lsForEachItem(tmpChoice, gen, data){
          if (idex==0){
            lsNewEnd(choice, data, LS_NH);
          }
          idex --;
        }
      } else {
        SimListConcatenate(choice, tmpChoice);
      }
      lsDestroy(tmpChoice, 0);
    }
    
    lsDestroy(allChoice, Enum_StateFree);
    allChoice = choice;

    /* update the index */
    index ++;
    if (index == numSimAtoms){
      done = 1;
      index = 0;
    }

    SimSimInfoSetIndex(simInfo, index);
    
    simAtom = array_fetch(Sim_Atom_t*, simAtomArray, index);
    name = Sim_SimAtomReadName(simAtom);
    if (isUserAtom != Sim_SimAtomReadIsUserAtom(simAtom))
      done = 1;
    
  }
  
  SimSimInfoSetChoice(simInfo, allChoice);
  
}


/**Function********************************************************************

  Synopsis           [Update the simInfo when a choice is made.]

  Description        [This routine takes an array of variable pointers, as
  well as an array of values (in long), and update the partial states. It then
  calls the SimComputeChoice to update the choices available.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
char *
SimStateUpdateWithChoice(
  Enum_StateManager_t *stateManager,
  Sim_Info_t *simInfo,
  int index /* which choice? */
  )
{
  Enum_State_t * state, *newPartialState;
  Enum_State_t * partialState = SimSimInfoReadPartialState(simInfo);
  Enum_State_t * prevState = SimSimInfoReadPrevState(simInfo);
  Enum_VarInfo_t *varInfo = SimSimInfoReadVarInfo(simInfo);
  lsList choice = SimSimInfoReadChoice(simInfo);
  lsGen gen;
  char * stateName;
  
  assert (partialState);
  assert (varInfo);
  assert (choice);

  /* free the previous partial state */
  Enum_StateFree(partialState);

  /* get the choice */
  lsForEachItem(choice, gen, state){
    if (index==0){
      newPartialState = Enum_StateDup(state, varInfo);
    }
    index --;
  }

  /* check if it is the end of the current round */
  if (SimSimInfoReadIndex(simInfo) == 0) {
    stateName = Enum_StateNameTableAddState(
      stateManager, SimSimInfoReadName(simInfo), newPartialState);
    
    SimSimInfoSetPrevState(simInfo, newPartialState);
    SimSimInfoSetPartialState(simInfo, Enum_StateAlloc(varInfo));
  } else {
    SimSimInfoSetPartialState(simInfo, newPartialState);
    stateName = NIL(char);
  }
  
  /* compute the choice again */
  SimComputeChoice(simInfo);

  return stateName;

}


/**Function********************************************************************

  Synopsis           [Returns the module name represented by the simInfo.]

  Description        [This returns the name of the module which is simulated
  by the simInfo.]

  SideEffects        [Should not free the returned string.]

  SeeAlso            [optional]

******************************************************************************/
char *
SimSimInfoReadName(
  Sim_Info_t * simInfo)
{
  return (simInfo->name);
}

/**Function********************************************************************

  Synopsis           [Returns the index in the simInfo.]

  Description        [Returns the index in the simInfo.]

  SideEffects        [required]

  SeeAlso            [SimSimInfoIndexSet]

******************************************************************************/
int
SimSimInfoReadIndex(
  Sim_Info_t * simInfo)
{
  return (simInfo->index);
}


/**Function********************************************************************

  Synopsis           [Returns the prevState.]

  Description        [Returns the prevState from the simInfo.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Enum_State_t *
SimSimInfoReadPrevState(
  Sim_Info_t * simInfo)
{
  return (simInfo->prevState);
}

/**Function********************************************************************

  Synopsis           [Returns the array of simAtomOrder.]

  Description        [Returns the array of simAtomOrder in the simInfo.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
array_t *
SimSimInfoReadSimAtomOrder(
  Sim_Info_t * simInfo)
{
  return (simInfo->simAtomOrder);
}

/**Function********************************************************************

  Synopsis           [Returns the partialState.]

  Description        [Returns the partialState.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Enum_State_t *
SimSimInfoReadPartialState(
  Sim_Info_t * simInfo)
{
  return (simInfo->partialState);
}

/**Function********************************************************************

  Synopsis           [retuns the list of choices.]

  Description        [returns the list of choices.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
lsList
SimSimInfoReadChoice(
  Sim_Info_t * simInfo)
{
  return (simInfo->choice);
}

/**Function********************************************************************

  Synopsis           [retuns the varInfo.]

  Description        [returns the varInfo.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Enum_VarInfo_t *
SimSimInfoReadVarInfo(
  Sim_Info_t * simInfo)
{
  return (simInfo->varInfo);
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
SimAtomDependencyInfo_t *
SimReadAtomDependencyInfo(
  Sim_Manager_t *simManager)
{
  return simManager->simAtomDependencyInfo;
}


/**Function********************************************************************

  Synopsis           [Partially duplicates a SimAtom.]

  Description        [It duplicates a simAtom. It doesn't duplicate all
  the fields because of lack of information.  For example, the successorAtom
  field is not duplicated, because very likely the successorAtoms will also
  be duplicated.]

  SideEffects        [The atom should be freed.]

  SeeAlso            [SimAtomFree]

******************************************************************************/
Sim_Atom_t *
SimAtomDup(
  Sim_Atom_t * simAtom)
{

  Sim_Atom_t * newSimAtom = ALLOC(Sim_Atom_t, 1);
  
  newSimAtom -> name = util_strsav(simAtom->name);
  
  newSimAtom -> atom = simAtom->atom;
  newSimAtom -> isUserAtom = simAtom->isUserAtom;
  newSimAtom -> numAwaitingAtom = simAtom->numAwaitingAtom;

  newSimAtom -> successorAtomList = NIL(array_t);

  return newSimAtom;
}


/**Function********************************************************************

  Synopsis           [Checks if the simAtom represents an external variable.]

  Description        [Checks if the given simAtom is an external
  variable. Returns TRUE if so, FALSE otherwise.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
SimSimAtomIsExternalVariable(
  Sim_Atom_t *simAtom)
{
  char* name = Sim_SimAtomReadName(simAtom);
  if (*name == '$') {
    return TRUE;
  } else {
    return FALSE;
  }
  
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************
   
   Synopsis           [Appends list2 to list1.]
   
   Description        [Appends the elements of list2 to the end of list1. The
   elements themselves are not copied. If list2 is not needed anymore, then it
   should be freed (do not free the content). ]
   
   SideEffects        [list1 is modified.]
   
********************************************************************************/
static void
SimListConcatenate(
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














