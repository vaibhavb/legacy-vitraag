/**CHeaderFile*****************************************************************

  FileName    [enum.h]

  PackageName [enum]

  Synopsis    [This is header file that should be included by any applicaition
  making use of the enum package. It contains declarations for all the
  exported routines and structures.]

  Description []

  SeeAlso     [enumInt.h]

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

  Revision    [$Id: enum.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _ENUM
#define _ENUM

#include  "main.h"
#include  "mdl.h"
#include  "var.h"
#include  "atm.h"
#include <limits.h> 

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct EnumStateStruct Enum_State_t;
typedef struct EnumStateManagerStruct Enum_StateManager_t;
typedef struct EnumStateTableEntryStruct Enum_StateTableEntry_t;
typedef struct EnumStateTableStruct Enum_StateTable_t;
typedef struct EnumVarInfoStruct Enum_VarInfo_t;

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
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN int Enum_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Enum_Reinit(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Enum_End(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN Enum_StateManager_t * Enum_StateManagerAlloc(void);
EXTERN void Enum_StateManagerFree(Enum_StateManager_t *stateManager);
EXTERN Enum_VarInfo_t * Enum_VarInfoAlloc(Mdl_Module_t *module);
EXTERN void Enum_VarInfoFree(Enum_VarInfo_t *varInfo);
EXTERN Tcl_HashTable * Enum_StateManagerReadStateNameTable(Enum_StateManager_t *stateManager);
EXTERN unsigned long Enum_StateManagerReadStateCounter(Enum_StateManager_t *stateManager);
EXTERN void Enum_StateManagerIncrementStateCounter(Enum_StateManager_t *stateManager);
EXTERN Var_Variable_t ** Enum_VarInfoReadHistDependVars(Enum_VarInfo_t *varInfo);
EXTERN Var_Variable_t ** Enum_VarInfoReadHistFreeVars(Enum_VarInfo_t *varInfo);
EXTERN Var_Variable_t ** Enum_VarInfoReadEventVars(Enum_VarInfo_t *varInfo);
EXTERN int Enum_VarInfoReadNumHistDependVars(Enum_VarInfo_t *varInfo);
EXTERN int Enum_VarInfoReadNumHistFreeVars(Enum_VarInfo_t *varInfo);
EXTERN int Enum_VarInfoReadNumEventVars(Enum_VarInfo_t *varInfo);
EXTERN Enum_VarInfo_t * Enum_StateManagerObtainVarInfoFromModule(Enum_StateManager_t *stateManager, Mdl_Module_t *module);
EXTERN void Enum_StateManagerAddVarInfo(Enum_StateManager_t *stateManager, Mdl_Module_t *module, Enum_VarInfo_t *varInfo);
EXTERN boolean Enum_StateIsVariableAssigned(Enum_State_t *state, Var_Variable_t *var, Enum_VarInfo_t *varInfo);
EXTERN long Enum_StateReadVariableValue(Enum_State_t *state, Var_Variable_t *var, Enum_VarInfo_t *varInfo);
EXTERN void Enum_StateSetVariableValue(Enum_State_t *state, Var_Variable_t *var, Enum_VarInfo_t *varInfo, long value);
EXTERN void Enum_StateFreeHistFreeVars(Enum_State_t *state);
EXTERN void Enum_StateFreeEventVars(Enum_State_t *state);
EXTERN lsList Enum_ModuleComputeSetOfInitialStates(Mdl_Module_t *module, Enum_VarInfo_t *varInfo);
EXTERN lsList Enum_AtomComputeSetOfPartialInitialStates(Atm_Atom_t *atom, Enum_VarInfo_t *varInfo, Enum_State_t *partialState);
EXTERN lsList Enum_ModuleComputeSetOfPostStates(Mdl_Module_t *module, Enum_VarInfo_t *varInfo, Enum_State_t *state);
EXTERN lsList Enum_AtomComputeSetOfPartialPostStates(Atm_Atom_t *atom, Enum_VarInfo_t *varInfo, Enum_State_t *state, Enum_State_t *partialState);
EXTERN Enum_State_t * Enum_StateAlloc(Enum_VarInfo_t *varInfo);
EXTERN void Enum_StateFree(Enum_State_t *state);
EXTERN Enum_State_t * Enum_StateDup(Enum_State_t *state, Enum_VarInfo_t *varInfo);
EXTERN char * Enum_PartialStatePrintToString(Enum_VarInfo_t *varInfo, Enum_State_t *partialState);
EXTERN lsList Enum_StateAssignAllVarCombinations(Enum_State_t *state, lsList varList, Enum_VarInfo_t *varInfo);
EXTERN Enum_StateTable_t * Enum_StateTableAlloc(int power, Enum_VarInfo_t *varInfo);
EXTERN void Enum_StateTableFree(Enum_StateTable_t *stateTable);
EXTERN Enum_StateTableEntry_t * Enum_StateTableEntryAlloc(Enum_State_t *state, Enum_StateTableEntry_t *next);
EXTERN void Enum_StateTableEntryFree(Enum_StateTableEntry_t *entry);
EXTERN void Enum_StateSetParentState(Enum_State_t *state, Enum_State_t *parentState);
EXTERN void Enum_ModulePerformBFS(Tcl_Interp *interp, Main_Manager_t *mainManager, Mdl_Module_t *module, array_t *invNameArray, array_t *typedExprArray);
EXTERN void Enum_ModulePerformDFS(Tcl_Interp *interp, Main_Manager_t *mainManager, Mdl_Module_t *module, array_t *invNameArray, array_t *typedExprArray);
EXTERN char * Enum_StateNameTableAddState(Enum_StateManager_t * stateManager,
                                          char * moduleName,
                                          Enum_State_t * state) ;
EXTERN char * Enum_StateNameTableAddStateWithNameSuffix( Enum_StateManager_t * stateManager,
                                                         char * moduleName,
                                                         Enum_State_t * state,
                                                         char * nameSuffix
                                                         ) ;

/**AutomaticEnd***************************************************************/

#endif /* _ENUM */
