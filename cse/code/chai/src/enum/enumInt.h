/**CHeaderFile*****************************************************************

  FileName    [enumInt.h]

  PackageName [enum]

  Synopsis    [This is the internal header file for the enum package. It
  contains the definitions for all the structures used in the package and
  declarations of all the functions used internally in the package.]

  Description []

  SeeAlso     [Enum.h]

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

  Revision    [$Id: enumInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _ENUMINT
#define _ENUMINT

#include  "enum.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct EnumBitwiseStruct EnumBitwise_t;


/**Enum************************************************************************

  Synopsis    [The two types of searches that can be performed.]

******************************************************************************/
typedef enum {
  Enum_DFS_c,
  Enum_BFS_c
} Enum_AlgorithmType;


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
/**Struct**********************************************************************

  Synopsis    [The structure of the explicit representation of a state.]

  Description [This data structure contains the values of the variables
               comprising the state. histDependVars, histFreeVars and
               eventVars are arrays of long, long and int respectively. These
               arrays store values of variables. The symbolic values of
               enumerated type variables are assigned values from the set of
               natural numbers. The event variables are handled
               differently. eventVars[i]=1 if the corresponding event 
               happened, otherwise it remains 0. The information about the
               correspondence between the values and the variables is present
               in EnumVarInfoStruct data structure.]

  SeeAlso     [EnumVarInfoStruct]

******************************************************************************/
struct EnumStateStruct {
  long *histDependVars;        /* values of non-event variables read by some
                                  atom */ 
  long *histFreeVars;          /* values of non-event variables not read by
                                  any atom */ 
  int *eventVars;              /* values of event variables */
  Enum_State_t *parentState;   /* state from which this state was reached for
                                  the first time; used in search */ 
};

/**Struct**********************************************************************

  Synopsis    [This structure provides the mapping between the values of
               variables in a state and the variables themselves.]

  Description [This structure provides the mapping between the values of
               variables in a state and the variables themselves. The three
               arrays - histDependVars, histFreeVars and eventVars, store
               variables in the same order as the values are stored in a
               state. The hash table varToIndex yields the index of a variable
               in the relevant array (depending on whether it history
               dependent, history free or event) from a pointer to the
               variable.]

  SeeAlso     [EnumStateStruct]

******************************************************************************/
struct EnumVarInfoStruct {
  Mdl_Module_t *module;
  Var_Variable_t **histDependVars;  /* non-event variables read by some atom */
  int numHistDependVars;         
  Var_Variable_t **histFreeVars; /* non-event variables not read by any atom */
  int numHistFreeVars;
  Var_Variable_t **eventVars;    /* event variables */
  int numEventVars;
  st_table *varToIndex;          /* hash table from Var_Variable_t * to int */
};

/**Struct**********************************************************************

  Synopsis    [The structure for the state manager.]

  Description [The state manager keeps a record of the states created through
               the commands enum_init and enum_post in stateNameTable which is
               a hash table from names given to states to the states
               themselves. A name is of the form
               <Module>.s<stateCounter>. Every time a new state is created,
               stateCounter is incremented. The names can be used to refer to
               states from the TCL command line. moduleToVarInfo is a hash
               table from module name to the vaInfo data structure
               corresponding to the module. The varInfo data structure gives
               the indexing of the state variables used to store their values.]

  SeeAlso     [EnumVarInfoStruct]

******************************************************************************/
struct EnumStateManagerStruct {
  st_table *moduleToVarInfo;
  Tcl_HashTable *stateNameTable;
  unsigned long stateCounter;
};

/**Struct**********************************************************************

  Synopsis    [The hash table in which states are stored during enumerative
               search.]

  Description [Each bucket in the hash table is a pointer to a linked list of
               states. numBuckets (always a power of 2) is the number of
               buckets in the hash table. size is the number of states in the
               hash table. numStoredVars is the number of variables that
               comprise the state stored in the hash table.]

  See Also     [EnumStateTableEntryStruct]
  
******************************************************************************/
struct EnumStateTableStruct {
  Enum_StateTableEntry_t **table;
  unsigned long numBuckets;
  unsigned long size;
  int power; /* numBuckets = 2^power */
  int numStoredVars;
};

/**Struct**********************************************************************

  Synopsis    [The data structure for storing a state in the hash table.]

  Description [States hashing into the same bucket in the hash table are
               stored in the form of a linked list. This structure is the unit
               element for storing a state in a linked list.]

******************************************************************************/
struct EnumStateTableEntryStruct {
  Enum_State_t *state;
  Enum_StateTableEntry_t *next;
};

/**Struct**********************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct EnumBitwiseStruct {
  char *bitArray;
  int size;
};


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
static Var_Variable_t *EnumIndexVar;
static long EnumIndexVarValue;


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN char * StateNameExtractModuleName(char *stateName);
EXTERN void EnumListConcatenate(lsList list1, lsList list2);
EXTERN boolean EnumExprIsTrue(Atm_Expr_t *expr, Enum_VarInfo_t *varInfo, Enum_State_t *prevState, Enum_State_t *state);
EXTERN long EnumExprEvaluate(Atm_Expr_t *expr, Enum_VarInfo_t *varInfo, Enum_State_t *prevState, Enum_State_t *state);
EXTERN EnumBitwise_t * EnumBitvectorExprEvaluate(Atm_Expr_t *expr, Enum_VarInfo_t *varInfo, Enum_State_t *prevState, Enum_State_t *state);
EXTERN lsList EnumInitAssignListExecuteSetup(Atm_Atom_t *atom, Enum_VarInfo_t *varInfo, lsList assignList, Enum_State_t *state);
EXTERN lsList EnumUpdateAssignListSetup(Atm_Atom_t *atom, Enum_VarInfo_t *varInfo, lsList assignList, Enum_State_t *state, Enum_State_t *partialState);
EXTERN void EnumAssignListExecuteOnState(lsList assignList, Enum_VarInfo_t *varInfo, Enum_State_t *prevState, Enum_State_t *state);
EXTERN EnumBitwise_t * EnumBitwiseAlloc(int size);
EXTERN void EnumBitwiseFree(EnumBitwise_t *a);
EXTERN void EnumCounterExamplePrint(Tcl_Interp *interp, Enum_StateManager_t *stateManager, Mdl_Module_t *module, char *invName, Enum_State_t *badState);
EXTERN boolean EnumStateAddToTable(Enum_StateTable_t *stateTable, Enum_State_t *state, Enum_VarInfo_t *varInfo);
EXTERN void EnumStateTablePrint(Enum_StateTable_t *stateTable, Enum_VarInfo_t *varInfo);
EXTERN void EnumStatePrint(Enum_State_t *state, Enum_VarInfo_t *varInfo);
EXTERN boolean StateCompare(Enum_State_t *state1, Enum_State_t *state2, Enum_VarInfo_t *varInfo);
EXTERN void StateTableResize(Enum_StateTable_t *stateTable);
EXTERN unsigned long StateHashFunction(Enum_StateTable_t *stateTable, Enum_State_t *state);
EXTERN unsigned long StateCompactToNumber(Enum_State_t *state, int numStoredVars);
EXTERN int EnumNumVarCmd(ClientData clientData, Tcl_Interp *interp, int argc,
                         char **argv);
EXTERN int EnumVarCmd(ClientData clientData, Tcl_Interp *interp, int argc,
                      char **argv);
EXTERN char *EnumPrintValueToString(   Var_Variable_t * var,  long value);
EXTERN int EnumValueCmd(ClientData clientData, Tcl_Interp *interp, int argc,
                        char **argv);


/**AutomaticEnd***************************************************************/

#endif /* _ENUMINT */






