/**CFile***********************************************************************

  FileName    [simDependency.c]

  PackageName [sim]

  Synopsis    [Routines manipulating the atom dependency table.]

  Description [In game simulation, the order of execution of the agents is of
  vital importance, since it effect the type and order of quantification of
  the variables. In this file, there is a structure, called SimAtom, for each
  atom of the module. The variable, SimAtomDependencyInfo, stores a directed
  acyclic graph whose arc goes from the SimAtom that is being awaited to the
  one that is awaiting this SimAtom.]

  SeeAlso     [simInt.h]

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
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void SimAtomDecrementNumAwaitingAtoms(array_t * successorAtomList, lsList queue);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [returns the name of module in the atom dependency info.]

  Description        [This returns the name of the module whose atom dependeny
  info is stored.]

  SideEffects        [It is an error to call this function with a NULL
  dependency info.]

  SeeAlso            [optional]

******************************************************************************/
char *
SimAtomDependencyInfoReadName(
  SimAtomDependencyInfo_t * simAtmInfo)
{
  assert (simAtmInfo);

  return simAtmInfo -> moduleName;
  
}


/**Function********************************************************************

  Synopsis           [Topological sort of the simAtoms.]

  Description        [This function employs the non-depth-first-search version
  of topological sort.  It returns an array of the simAtoms.  Any atom that
  awaits another atom will come later in the list.  Any atom that is not
  specified as agent in the formula will come as late as possible, while
  respecting the await dependency.]

  SideEffects        [Users should free the array, but not the simAtoms]

  SeeAlso            [optional]

******************************************************************************/
array_t *
SimTopologicalSort(
  SimAtomDependencyInfo_t * simAtmInfo)
{
  /* do topological sort of the atoms */

  lsList queue;     /* a queue used to determine which atom is to be taken */
  array_t * sortedArray;
  Sim_Atom_t * simAtom;
  st_table * dependencyTable = simAtmInfo->nameToSimAtom;
  st_generator * gen;
  char * key;
      
  queue = lsCreate();
  sortedArray = array_alloc(Sim_Atom_t *, 0);
  
  /* to start with, search for all the atoms with in degree 0 */
  st_foreach_item(dependencyTable, gen, &key, (char **) &simAtom) {
    if (*key == '$') { /* external variable */
      simAtom->numAwaitingAtom = 0;
    } else {
      simAtom->numAwaitingAtom =
          lsLength(Atm_AtomReadAwaitVarList(simAtom->atom));
    }
    
    if (simAtom->numAwaitingAtom == 0) {
      if (simAtom->isUserAtom) {
        lsNewBegin(queue, (lsGeneric) simAtom, LS_NH);
      } else {
        lsNewEnd(queue, (lsGeneric) simAtom, LS_NH);
      }
    }
  }
  
  /* then remove the atoms in the queue one by one, from the begining */
  while(lsDelBegin(queue, (lsGeneric *) &simAtom) != LS_NOMORE) {

    array_insert_last(Sim_Atom_t *, sortedArray, simAtom);
    SimAtomDecrementNumAwaitingAtoms(simAtom->successorAtomList, queue);
  }

  /* free the queue */
  lsDestroy(queue, 0);
  
  /* debug */
  /*
  {
    int i;
    int num = array_n(sortedArray);
    for (i=0; i<num; i++){
      Sim_Atom_t* simAtom = array_fetch(Sim_Atom_t*, sortedArray, i);

      fprintf(stderr, "name = %s\n", Sim_SimAtomReadName(simAtom));
    }
    
  }
  */
  return sortedArray;
}

/**Function********************************************************************

  Synopsis           [Updates the AtomDependencyInfo to the current module.]

  Description        [This function updates the current Atom Dependency Info
  to reflect the await dependency of atoms in the module. For each external
  variables, it creates an "external atom" that controls it.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimAtomDependencyInfoUpdate(
  Mdl_Module_t * module,
  SimAtomDependencyInfo_t * simAtomDependencyInfo)
{
  st_table * newDependencyTable;
  st_table * dependencyTable = simAtomDependencyInfo->nameToSimAtom;
  st_generator *gen;
  char * key;
  Sim_Atom_t * simAtom;
  Atm_Atom_t *atom;

  if (Mdl_ModuleReadCreationTime(module) !=
      simAtomDependencyInfo->moduleCreationTime){
    Main_MochaErrorPrint("SIM: building atom dependency info\n");
    /* give it a new name */
    FREE(simAtomDependencyInfo -> moduleName);
    simAtomDependencyInfo->moduleName = util_strsav(Mdl_ModuleReadName(module));

    /* copy the creation time of the module */
    simAtomDependencyInfo->moduleCreationTime =
        Mdl_ModuleReadCreationTime(module);
    
    /* clean up the table in simAtomDependencyInfo */
    /* build another one, with atoms from the module */
    
    st_foreach_item(dependencyTable, gen, &key, (char **) &simAtom) {
      SimAtomFree(simAtom);
    }
    
    st_free_table(dependencyTable);
    
    /* create a new dependency table */
    newDependencyTable = st_init_table(strcmp, st_strhash);
    simAtomDependencyInfo->nameToSimAtom = newDependencyTable;
    
    /* create a simAtom for each of the atoms */
    {
      lsList atomList = Mdl_ModuleObtainSortedAtomList(module);
      lsGen lsgen;
      
      lsForEachItem(atomList, lsgen, atom) {
        Sim_Atom_t * simAtom = SimAtomAlloc(atom);
        st_insert(newDependencyTable, Sim_SimAtomReadName(simAtom),
                  (char *) simAtom);
      }
      
      lsDestroy(atomList, 0);
    }
    
    /* for each external variable, imagine there is a corresponding
       "external atom" that controls it */
    /* here, let the atom has the name formed by prefixing an dollar sign
       before the variable name */
    {
      Var_Variable_t * var;
      lsGen lsgen;
      lsList extVarList = Mdl_ModuleGetExternalVariableList(module);
      
      lsForEachItem(extVarList, lsgen, var) {
        Sim_Atom_t * simAtom = SimAtomAlloc(NIL(Atm_Atom_t));
        char * name;
        
        name = util_strcat3("$", Var_VariableReadName(var),"");
        simAtom -> name = name;
        simAtom -> atom = (Atm_Atom_t *) var;
        st_insert(newDependencyTable, name, (char *) simAtom);
      }
      
      lsDestroy(extVarList, 0);
    }
    
    /* establish the dependencies among the simAtoms */
    st_foreach_item(newDependencyTable, gen, &key, (char **) &simAtom) {
      if (*(Sim_SimAtomReadName(simAtom)) == '$'){
        /* atom controlling an external variable */
        simAtom->numAwaitingAtom = 0;
      } else {
        Var_Variable_t * var;  
        Atm_Atom_t * atom = simAtom -> atom;
        lsList awaitVarList = Atm_AtomReadAwaitVarList(atom);
        lsGen lsgen;
        
        simAtom->numAwaitingAtom = lsLength(awaitVarList);
        
        lsForEachItem(awaitVarList, lsgen, var){
          Sim_Atom_t * simAwaitingAtom;
          char * name;
          
          if (!Var_VariableIsExternal(var)){
            name = util_strsav(Atm_AtomReadName(
              (Atm_Atom_t *) Var_VariableReadControlAtom(var)));
          } else {
            name = util_strcat3("$", Var_VariableReadName(var), "");          
          }
          
          st_lookup(newDependencyTable, name, (char **)
                    &simAwaitingAtom);
          
          array_insert_last (Sim_Atom_t *,
                             simAwaitingAtom -> successorAtomList,
                             simAtom);
          FREE(name);
        }
      }
    }
  }
}


/**Function********************************************************************

  Synopsis           [Clears all the isUserAtom flag in the dependencyInfo]

  Description        [This function clears all the isUsrAtom flag in the
  atom table of the supplied SimAtomDependencyInfo.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SimDependencyInfoSetIsUserAtom(
  SimAtomDependencyInfo_t *simAtomDependencyInfo,
  array_t *atomArray,
  boolean isUserAtom)
{
  int i, numSimAtoms;
  Sim_Atom_t * simAtom;
  st_table * atmTable = simAtomDependencyInfo -> nameToSimAtom;
  st_generator * gen;
  char *key;
  
  st_foreach_item(atmTable, gen, &key, (char **) &simAtom) {
    simAtom->isUserAtom = !isUserAtom;
  }

  numSimAtoms = array_n (atomArray);
  for(i=0; i<numSimAtoms; i++){
    Atm_Atom_t * atom;
    atom = array_fetch(Atm_Atom_t *, atomArray, i);
    
    st_lookup(atmTable, Atm_AtomReadName(atom), (char **) &simAtom);

    assert (simAtom);
    simAtom->isUserAtom = isUserAtom;
  }
  
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Decrements the numAwaitingAtom field.]

  Description        [This function decrements the numAwaitingAtom field in
  the array of SimAtoms.  If the field reaches zero, then the SimAtom will be
  added to the queue.  If the simAtom is an agent, it will be added to the
  front of the queue.  If it is not an agent, then it will be added to
  the end of the queue.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void 
SimAtomDecrementNumAwaitingAtoms(
  array_t *  successorAtomList,
  lsList queue)
{
  lsGen lsgen;
  Sim_Atom_t * simAtom;
  int i;
  int num = array_n (successorAtomList);

  for (i=0; i<num; i++) {
    simAtom = array_fetch (Sim_Atom_t *, successorAtomList, i);

    assert (simAtom->numAwaitingAtom > 0);
    
    if ((-- simAtom->numAwaitingAtom) == 0) {
      if (simAtom->isUserAtom){
        lsNewBegin(queue, (lsGeneric) simAtom, LS_NH);
      } else {
        lsNewBegin(queue, (lsGeneric) simAtom, LS_NH);
      }
    }
  }
}









