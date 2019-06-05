/**CHeaderFile*****************************************************************

  FileName    [simInt.h]

  PackageName [sim]

  Synopsis    [structure definitions used in the sim package]

  Description [optional]

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

  Revision    [$Id: simInt.h,v 1.2 2003/05/24 07:06:44 vaibhav Exp $]

******************************************************************************/

#ifndef _SIMINT
#define _SIMINT

#include  "sim.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define MAXNUMCHOICE 20

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct SimAtomDependencyInfoStruct SimAtomDependencyInfo_t;

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
/**Struct**********************************************************************

  Synopsis    [Simulation Manager]

  Description [This is essentially a hash table that maps the name of modules
  being simulated to the simInfo.]

  SeeAlso     [optional]

******************************************************************************/
struct SimManagerStruct {
  st_table * nameToSimInfo;
  SimAtomDependencyInfo_t* simAtomDependencyInfo;
};


/**Struct**********************************************************************

  Synopsis    [Simulation Info]

  Description [For each module being simulated, there is an associated simInfo
  which stores information about the module.  This simInfo will be updated
  every time a move of simulation is made.]

  SeeAlso     [optional]

******************************************************************************/
struct SimInfoStruct {
  char *name;              /* name of the module */
  int creationTime;        /* creation time of the module being simulated */
  array_t *simAtomOrder;   /* an array of simAtoms which stores the order of 
                              execution of simAtoms */

  /* data for doing game simulation */
  Enum_VarInfo_t *varInfo;
  Enum_State_t * partialState;
  Enum_State_t * prevState;
  lsList       choice;
  int index;               /* which atom to execute next */
};



/**Struct**********************************************************************

  Synopsis    [Structure for storing the atom dependency table]

  Description [This is a structure that stores the dependency table, and the
  creationTime of a module. Currently, this structure will be stored in the
  simManager.]

  SeeAlso     [optional]

******************************************************************************/
struct SimAtomDependencyInfoStruct {
  char * moduleName;
  st_table * nameToSimAtom;
  int moduleCreationTime;
};

/**Struct**********************************************************************

  Synopsis    [An structure used in this file to represent an atom.]

  Description [This is the counterpart of the Atm_Atom_t used in the
  atm package. The use of this is mainly for dependency computation.
  A non-dfs topological sort is employed.]

  SeeAlso     [optional]

******************************************************************************/
struct SimAtomStruct {
  char * name;                 /* name of the atom */
  Atm_Atom_t *atom;            /* pointer to the corresponding atom in module*/
  boolean isUserAtom;          /* isUserAtom */
  array_t * successorAtomList; /* the list of SimAtoms awaiting this atom */
  int numAwaitingAtom;         /* the number of atoms this atom is awaiting */
};

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

static Sim_Manager_t *simManager;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN Sim_Atom_t * SimAtomAlloc(Atm_Atom_t * atom);
EXTERN void SimAtomFree(Sim_Atom_t * simAtom);
EXTERN boolean SimSimulationStart(Mdl_Module_t * module, array_t *atomArray, boolean isUserAtom);
EXTERN void SimManagerInit();
EXTERN Sim_Info_t* SimManagerReadSimInfo(char* moduleName);
EXTERN Sim_Info_t* SimManagerDeleteSimInfo(char* moduleName);
EXTERN Sim_Info_t* SimSimInfoAlloc(Mdl_Module_t * module);
EXTERN void SimSimInfoFree(Sim_Info_t* simInfo);
EXTERN void SimSimInfoAddToManager(Sim_Info_t* simInfo);
EXTERN void SimSimInfoSetIndex(Sim_Info_t * simInfo, int index);
EXTERN void SimSimInfoSetChoice(Sim_Info_t * simInfo, lsList choice);
EXTERN void SimSimInfoSetSimAtomOrder(Sim_Info_t * simInfo, array_t* simAtomOrder);
EXTERN void SimSimInfoSetPrevState(Sim_Info_t* simInfo, Enum_State_t* prevState);
EXTERN void SimSimInfoSetPartialState(Sim_Info_t * simInfo, Enum_State_t * partialState);
EXTERN void SimSimInfoSetVarInfo(Sim_Info_t * simInfo, Enum_VarInfo_t * varInfo);
EXTERN void SimComputeChoice(Sim_Info_t * simInfo);
EXTERN char * SimStateUpdateWithChoice(Enum_StateManager_t *stateManager, Sim_Info_t *simInfo, int index);
EXTERN char * SimSimInfoReadName(Sim_Info_t * simInfo);
EXTERN int SimSimInfoReadIndex(Sim_Info_t * simInfo);
EXTERN Enum_State_t * SimSimInfoReadPrevState(Sim_Info_t * simInfo);
EXTERN array_t * SimSimInfoReadSimAtomOrder(Sim_Info_t * simInfo);
EXTERN Enum_State_t * SimSimInfoReadPartialState(Sim_Info_t * simInfo);
EXTERN lsList SimSimInfoReadChoice(Sim_Info_t * simInfo);
EXTERN Enum_VarInfo_t * SimSimInfoReadVarInfo(Sim_Info_t * simInfo);
EXTERN SimAtomDependencyInfo_t * SimReadAtomDependencyInfo(Sim_Manager_t *simManager);
EXTERN Sim_Atom_t * SimAtomDup(Sim_Atom_t * simAtom);
EXTERN boolean SimSimAtomIsExternalVariable(Sim_Atom_t *simAtom);
EXTERN char * SimAtomDependencyInfoReadName(SimAtomDependencyInfo_t * simAtmInfo);
EXTERN array_t * SimTopologicalSort(SimAtomDependencyInfo_t * simAtmInfo);
EXTERN void SimAtomDependencyInfoUpdate(Mdl_Module_t * module, SimAtomDependencyInfo_t * simAtomDependencyInfo);
EXTERN void SimDependencyInfoSetIsUserAtom(SimAtomDependencyInfo_t *simAtomDependencyInfo, array_t *atomArray, boolean isUserAtom);

/**AutomaticEnd***************************************************************/

#endif /* _SIMINT */
