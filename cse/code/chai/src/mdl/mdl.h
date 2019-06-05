/**CHeaderFile*****************************************************************

  FileName    [mdl.h]

  PackageName [mdl]

  Synopsis    [Building Modules and Performing Various Operations.]

  Description [This package contains structures and routines related to
  modules. It serves two main purposes: 1. providing module structures for
  storing atoms, variables and invariants; 2. providing routines for various
  module level operations, such as parallel composition, variable hiding and
  renaming.]

  SeeAlso     [var atm]

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

  Revision    [$Id: mdl.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _MDL
#define _MDL

#include <stdio.h> 
#include <string.h>
#include  "main.h"
#include  "atm.h"
#include  "var.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct  MdlManagerStruct Mdl_Manager_t;
typedef struct  MdlModuleStruct Mdl_Module_t;
typedef struct  MdlInvariantStruct Mdl_Invariant_t;
typedef struct  MdlExpressionStruct Mdl_Expr_t;

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
/**Enum************************************************************************

  Synopsis    [The status of the invariant]

  Description [An invariant can been checked and passed, or failed, or
  it has not been checked yet.]

  SeeAlso     [optional]

******************************************************************************/
typedef enum {
  Mdl_InvPassed_c,
  Mdl_InvFailed_c,
  Mdl_InvUnchecked_c
} Mdl_InvStatus_t;


/**Enum************************************************************************

  Synopsis    [The type of a module expression node.]

  Description [A module expression node can one of the different types.]

  SeeAlso     [MdlExpressionStruct]

******************************************************************************/
typedef enum {
  Mdl_ExprRename_c,      /* variable renaming */
  Mdl_ExprHide_c,        /* variable hiding */
  Mdl_ExprCompose_c,     /* parallel composition of modules */
  Mdl_ExprModuleDup_c,   /* Duplication of an existing module */
  Mdl_ExprModuleDef_c    /* Explicit definition of the module */
} Mdl_ExprType;

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
/**Macro***********************************************************************

  Synopsis     [A control structure for looping through all the invariants]

  Description  [This is a control structure for looping through all the
  invariants of a give module.]

  SideEffects  [none]

******************************************************************************/
#define                                                                    \
Mdl_ModuleForEachStateInvariant(                                           \
module,      /* the module */                                              \
gen,         /* local variable of type st_generator* */                    \
name,        /* char *, name of the invariant */                           \
inv         /* returned data of type Mdl_Invariant_t* */                   \
)                                                                          \
  st_foreach_item(Mdl_ModuleReadStateInvTable(module), (gen), &name, (char **) &(inv))
  
/**Macro***********************************************************************

  Synopsis     [A control structure for looping through all the invariants]

  Description  [This is a control structure for looping through all the
  invariants of a give module.]

  SideEffects  [none]

******************************************************************************/
#define                                                                    \
Mdl_ModuleForEachTransitionInvariant(                                           \
module,      /* the module */                                              \
gen,         /* local variable of type st_generator* */                    \
name,        /* char *, name of the invariant */                           \
inv         /* returned data of type Mdl_Invariant_t* */                   \
)                                                                          \
  st_foreach_item(Mdl_ModuleReadTranInvTable(module), (gen), &name, (char **) &(inv))
  
/**Macro***********************************************************************

  Synopsis     [required]

  Description  [optional]

  SideEffects  [required]

  SeeAlso      [optional]

******************************************************************************/
#define                                                                    \
Mdl_ModuleForEachVariable(                                                 \
module,      /* pointer to the module */                                   \
gen,         /* local variable of type st_generator* */                    \
name,        /* char *, name of the variable */                            \
var          /* returned data of type Var_Variable_t* */                   \
)                                                                          \
  st_foreach_item(Mdl_ModuleReadVariableTable(module), (gen), &(name), (char **) &(var))                                 

/**Macro***********************************************************************

  Synopsis     [required]

  Description  [optional]

  SideEffects  [required]

  SeeAlso      [optional]

******************************************************************************/
#define                                                                    \
Mdl_ModuleForEachAtom(                                                     \
module,      /* pointer to the module */                                   \
gen,         /* local variable of type st_generator* */                    \
name,        /* char *, name of the atom */                                \
atom         /* returned data of type Atm_Atom_t* */                       \
)                                                                          \
  st_foreach_item(Mdl_ModuleReadAtomTable(module), (gen), &(name), (char **) &(atom))                                 
  
  
/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN Mdl_Module_t* Mdl_ModuleAlloc(char* name);
EXTERN int Mdl_ModuleAddValueInfo(Mdl_Module_t* module, st_table* atmTable, st_table* varTable);
EXTERN void Mdl_ModuleFree(Mdl_Module_t* module);
EXTERN void Mdl_ModuleManagerFree(Mdl_Manager_t* mdlmanager);
EXTERN int Mdl_ModuleAddToManager(Mdl_Module_t* module, Mdl_Manager_t* mdlmanager);
EXTERN Mdl_Module_t* Mdl_ModuleReadFromName(Mdl_Manager_t* mdlmanager, char* name);
EXTERN char* Mdl_ModuleReadName(Mdl_Module_t* module);
EXTERN st_table* Mdl_ModuleReadVariableTable(Mdl_Module_t* module);
EXTERN st_table* Mdl_ModuleReadAtomTable(Mdl_Module_t* module);
EXTERN int Mdl_ModuleReadCreationTime(Mdl_Module_t * module);
EXTERN Atm_Atom_t * Mdl_ModuleReadAtomFromName(char * name, Mdl_Module_t * module);
EXTERN lsList Mdl_ModuleManagerObtainModuleList(Mdl_Manager_t* mdlmanager);
EXTERN lsList Mdl_ModuleObtainAtomList(Mdl_Module_t* module);
EXTERN Mdl_Module_t* Mdl_ModuleDup(char* name, Mdl_Module_t* module, st_table **atomMappingTable, st_table **variableMappingTable);
EXTERN void Mdl_ModuleSetModuleExpr(Mdl_Module_t * module, Mdl_Expr_t * moduleExpr);
EXTERN Mdl_Expr_t * Mdl_ModuleReadModuleExpr(Mdl_Module_t * module);
EXTERN char* Mdl_ModuleObtainNewName();
EXTERN char* Mdl_NamePrefixWithString(char* prefix, char* name);
EXTERN int Mdl_ModuleDeleteVariable(Mdl_Module_t* module, char* varname);
EXTERN int Mdl_ModuleAddVariable(Mdl_Module_t* module, char* varname, Var_Variable_t* var);
EXTERN int Mdl_ModuleAddAtom(Mdl_Module_t* module, char* atmname, Atm_Atom_t* atm);
EXTERN boolean Mdl_ModuleRemoveFromManager(char* name, Mdl_Manager_t* mdlmanager);
EXTERN Var_Variable_t* Mdl_ModuleReadVariableFromName(char* varname, Mdl_Module_t* module);
EXTERN lsList Mdl_ModuleObtainVariableList(Mdl_Module_t* module);
EXTERN lsList Mdl_ModuleObtainHistoryDependentVariableList(Mdl_Module_t* module);
EXTERN lsList Mdl_ModuleObtainHistoryFreeVariableList(Mdl_Module_t* module);
EXTERN lsList Mdl_ModuleObtainEventVariableList(Mdl_Module_t* module);
EXTERN Mdl_Invariant_t * Mdl_InvariantAlloc(char *name, Mdl_Module_t *module);
EXTERN void Mdl_InvariantFree(Mdl_Invariant_t *invariant);
EXTERN void Mdl_InvariantSetExpression(Mdl_Invariant_t *invariant, Atm_Expr_t *exptree);
EXTERN void Mdl_InvariantSetStatus(Mdl_Invariant_t *invariant, Mdl_InvStatus_t status);
EXTERN Mdl_InvStatus_t Mdl_InvariantReadStatus(Mdl_Invariant_t *invariant);
EXTERN Atm_Expr_t * Mdl_InvariantReadExpression(Mdl_Invariant_t *invariant);
EXTERN boolean Mdl_StateInvAddToModule(Mdl_Module_t *module, Mdl_Invariant_t *invariant);
EXTERN boolean Mdl_TranInvAddToModule(Mdl_Module_t *module, Mdl_Invariant_t *invariant);
EXTERN char * Mdl_InvariantReadName(Mdl_Invariant_t * invariant);
EXTERN st_table * Mdl_ModuleReadStateInvTable(Mdl_Module_t *module);
EXTERN st_table * Mdl_ModuleReadTranInvTable(Mdl_Module_t *module);
EXTERN Mdl_Expr_t * Mdl_ExprComposeAlloc(Mdl_Expr_t *module1, Mdl_Expr_t *module2);
EXTERN Mdl_Expr_t * Mdl_ExprRenameAlloc(Mdl_Expr_t *module, lsList oldVariableNameList, lsList newVariableNameList);
EXTERN Mdl_Expr_t * Mdl_ExprHideAlloc(Mdl_Expr_t *module, lsList variableNameList);
EXTERN Mdl_Expr_t * Mdl_ExprModuleDupAlloc(char * name);
EXTERN Mdl_Expr_t * Mdl_ExprModuleDefAlloc();
EXTERN void Mdl_ExprFree(Mdl_Expr_t * mexpr);
EXTERN Mdl_ExprType Mdl_ExprReadModuleExprType(Mdl_Expr_t *mexpr);
EXTERN Mdl_Expr_t * Mdl_ExprReadLeftChild(Mdl_Expr_t *mexpr);
EXTERN Mdl_Expr_t * Mdl_ExprReadRightChild(Mdl_Expr_t *mexpr);
EXTERN lsList Mdl_ExprReadList1(Mdl_Expr_t *mexpr);
EXTERN lsList Mdl_ExprReadList2(Mdl_Expr_t *mexpr);
EXTERN Mdl_Module_t* Mdl_ModuleExprEvaluate(char *name, Mdl_Expr_t *mexpr, Mdl_Manager_t *moduleManager);
EXTERN array_t * Mdl_ExprTreeReadComponents(Mdl_Expr_t * mexpr);
EXTERN Mdl_Module_t * Mdl_ModuleUpdate(Mdl_Module_t *module, Mdl_Manager_t *manager);
EXTERN boolean Mdl_ModuleIsUpdate(Mdl_Module_t * module, Mdl_Manager_t *manager);
EXTERN Mdl_Expr_t * Mdl_ExprDup(Mdl_Expr_t *mexpr);
EXTERN array_t * Mdl_ModuleObtainComponentAtomArray(Mdl_Module_t * module, array_t * nameArray);
EXTERN array_t * Mdl_ModuleSearchForComponent(char *name, Mdl_Module_t * module);
EXTERN array_t* Mdl_ExprReturnAtomArray(Mdl_Expr_t* mexpr);
EXTERN boolean Mdl_ModuleSetModuleExprForTypeModuleDef(Mdl_Module_t *module);
EXTERN int Mdl_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Mdl_Reinit(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Mdl_End(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN Mdl_Manager_t * Mdl_ModuleManagerAlloc(void);
EXTERN int Mdl_VariableRename(st_table* vartable, char* oldname, char* newname);
EXTERN int Mdl_ModuleTestForAcyclicity(Mdl_Module_t* module);
EXTERN lsList Mdl_ModuleObtainSortedAtomList(Mdl_Module_t* module);
EXTERN lsList Mdl_ModuleGetExternalVariableList(Mdl_Module_t* module);
EXTERN lsList Mdl_ModuleGetInterfaceVariableList(Mdl_Module_t* module);
EXTERN Mdl_Module_t * Mdl_ModuleCreateFromAtoms(lsList atomList, lsList privateVarNameList);

/**AutomaticEnd***************************************************************/

#endif /* _MDL */
