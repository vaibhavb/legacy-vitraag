/**CHeaderFile*****************************************************************

  FileName    [atm.h]

  PackageName [atm]

  Synopsis    [External header file for the package Atm.]

  Description [Data structure  and routines to manipulate atoms.]

  SeeAlso     [atmInt.h]

  Author      [Shaz Qadeer, Sriram Rajamani]



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

  Revision    [$Id: atm.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _ATM
#define _ATM
#include  "main.h"
#include  "var.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct AtmCmdStruct  Atm_Cmd_t;
typedef struct AtmAtomStruct  Atm_Atom_t;
typedef struct AtmExprStruct Atm_Expr_t;
typedef struct AtmAssignStruct Atm_Assign_t;
typedef char* AtmModuleGeneric;

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
/**Enum************************************************************************

  Synopsis    [Types of expressions.]

  Description [This is an enumeration of the different types of expressions
               that are possible in guards and right hand sides of
               assignments.]

  SeeAlso     [Atm_Expr_t]

******************************************************************************/
typedef enum {
  Atm_And_c,
  Atm_BoolConst_c,                        /* boolean const */
  Atm_Equal_c,
  Atm_NotEqual_c,
  Atm_EnumConst_c,                        /* enumerative const */
  Atm_Equiv_c,
  Atm_NotEquiv_c,
  Atm_EventQuery_c,
  Atm_Greater_c,
  Atm_GreaterEqual_c,
  Atm_IfThenElseFi_c,
  Atm_Implies_c,
  Atm_Index_c,                            /* a[expr] */
  Atm_Less_c,
  Atm_LessEqual_c,
  Atm_Minus_c, 
  Atm_Not_c,    
  Atm_NumConst_c,                         /* number const (int, nat, range)*/
  Atm_Or_c,
  Atm_Xor_c,
  Atm_Plus_c,
  Atm_PrimedVar_c,
  Atm_RangeConst_c,
  Atm_UnaryMinus_c,
  Atm_UnPrimedVar_c,
  Atm_BitwiseAnd_c,
  Atm_BitwiseEqual_c,
  Atm_BitwiseNotEqual_c,
  Atm_BitwiseEquiv_c,
  Atm_BitwiseNotEquiv_c,
  Atm_BitwiseGreater_c,
  Atm_BitwiseGreaterEqual_c,
  Atm_BitwiseIfThenElseFi_c,
  Atm_BitwiseImplies_c,
  Atm_BitwiseIndex_c,                            /* a[expr] */
  Atm_BitwiseLess_c,
  Atm_BitwiseLessEqual_c,
  Atm_BitwiseMinus_c, 
  Atm_BitwiseNot_c,    
  Atm_BitwiseNumConst_c,                /* number const (int, nat, range)*/
  Atm_BitwiseOr_c,
  Atm_BitwiseXor_c,
  Atm_BitwisePlus_c,
  Atm_BitwisePrimedVar_c,
  Atm_BitwiseUnPrimedVar_c,
  Atm_Nondet_c,
  Atm_TimerUpperBound_c,                 /* The upper bound for a timer at a given location */
  Atm_RedAnd_c,
  Atm_RedOr_c,
  Atm_RedXor_c
} Atm_ExprType;


/**Enum************************************************************************

  Synopsis    [Types of atoms]

  Description []

  SeeAlso     [optional]

******************************************************************************/
typedef enum {
  Atm_Event_c,
  Atm_Lazy_c,
  Atm_Normal_c
} Atm_AtomType;

/**Enum************************************************************************

  Synopsis    [Types of assignments.]

  Description [This is an enumeration of the different types of assignments.]

  SeeAlso     []

******************************************************************************/
typedef enum {
  Atm_AssignEvent_c,
  Atm_AssignForall_c,
  Atm_AssignIndex_c,
  Atm_AssignStd_c
} Atm_AssignType;


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

EXTERN int Atm_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Atm_Reinit(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Atm_End(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN void Atm_ExprPrintExpression(Atm_Expr_t* expr);
EXTERN char* Atm_ExprTypeConvertToString(Atm_ExprType type);
EXTERN void Atm_NumConstExprConvertToBitvectorConstExpr(Atm_Expr_t *expr, int numBits);
EXTERN boolean Atm_TypeCheckNew(Var_Type_t * type, Atm_Expr_t ** expr);
EXTERN void Atm_ReinitTypeCheck(Var_TypeManager_t* typeManager);
EXTERN Atm_Atom_t* Atm_AtomAlloc(char * name);
EXTERN void Atm_AtomFree(Atm_Atom_t *atom);
EXTERN Atm_Cmd_t* Atm_CmdAlloc(Atm_Expr_t* guard, lsList assignList);
EXTERN void Atm_CmdFree(Atm_Cmd_t* cmd);
EXTERN Atm_Assign_t* Atm_AssignStdAlloc(Var_Variable_t* var, Atm_Expr_t* expr);
EXTERN Atm_Assign_t* Atm_AssignEventAlloc(Var_Variable_t* var);
EXTERN Atm_Assign_t* Atm_AssignIndexAlloc(Var_Variable_t* var, Atm_Expr_t *indexExpr, Atm_Expr_t* expr);
EXTERN Atm_Assign_t* Atm_AssignForallAlloc(Var_Variable_t* var, Var_Variable_t *indexVar, Atm_Expr_t* expr);
EXTERN void Atm_AssignFree(Atm_Assign_t* assign);
EXTERN void Atm_AtomAddInitCommandList(Atm_Atom_t *atom, lsList cmdlist);
EXTERN lsList Atm_AtomReadInitCommandList(Atm_Atom_t *atom);
EXTERN Atm_Cmd_t * Atm_AtomReadDefaultInitCommand(Atm_Atom_t *atom);
EXTERN void Atm_AtomAddUpdateCommandList(Atm_Atom_t *atom, lsList cmdList);
EXTERN lsList Atm_AtomReadUpdateCommandList(Atm_Atom_t *atom);
EXTERN Atm_Cmd_t * Atm_AtomReadDefaultUpdateCommand(Atm_Atom_t *atom);
EXTERN void Atm_AtomAddWaitCommandList(Atm_Atom_t *atom, lsList cmdList);
EXTERN lsList Atm_AtomReadWaitCommandList(Atm_Atom_t *atom);
EXTERN void Atm_AtomAddControlVar(Atm_Atom_t *atom, Var_Variable_t* var);
EXTERN lsList Atm_AtomReadControlVarList(Atm_Atom_t *atom);
EXTERN void Atm_AtomAddReadVar(Atm_Atom_t *atom, Var_Variable_t* var);
EXTERN lsList Atm_AtomReadReadVarList(Atm_Atom_t *atom);
EXTERN void Atm_AtomAddAwaitVar(Atm_Atom_t *atom, Var_Variable_t* var);
EXTERN lsList Atm_AtomReadAwaitVarList(Atm_Atom_t *atom);
EXTERN lsList Atm_AtomObtainReadAwaitVarList(Atm_Atom_t *atom);
EXTERN lsList Atm_AtomObtainReadOrAwaitVarList(Atm_Atom_t *atom);
EXTERN Atm_AtomType Atm_AtomReadAtomType(Atm_Atom_t* atom);
EXTERN void Atm_AtomSetAtomType(Atm_Atom_t* atom, Atm_AtomType t);
EXTERN void Atm_AtomSetDefaultInitCommand(Atm_Atom_t *atom, Atm_Cmd_t *cmd);
EXTERN void Atm_AtomSetDefaultUpdateCommand(Atm_Atom_t *atom, Atm_Cmd_t *cmd);
EXTERN char* Atm_AtomReadName(Atm_Atom_t* atom);
EXTERN char* Atm_AtomNewName();
EXTERN void Atm_AtomCounterReset();
EXTERN Atm_Expr_t * Atm_CmdReadGuard(Atm_Cmd_t *command);
EXTERN lsList Atm_CmdReadAssignList(Atm_Cmd_t *command);
EXTERN Atm_ExprType Atm_ExprReadType(Atm_Expr_t *expr);
EXTERN Atm_Expr_t * Atm_ExprReadLeftChild(Atm_Expr_t *expr);
EXTERN Atm_Expr_t * Atm_ExprReadRightChild(Atm_Expr_t *expr);
EXTERN Atm_AssignType Atm_AssignReadType(Atm_Assign_t *assign);
EXTERN Var_Variable_t* Atm_AssignReadVariable(Atm_Assign_t *assign);
EXTERN Atm_Expr_t * Atm_AssignReadExpr(Atm_Assign_t *assign);
EXTERN Atm_Expr_t ** Atm_AssignReadExprAddress(Atm_Assign_t *assign);
EXTERN Var_Variable_t* Atm_AssignReadIndexVar(Atm_Assign_t *assign);
EXTERN Atm_Expr_t* Atm_AssignReadIndexExpr(Atm_Assign_t *assign);
EXTERN Atm_Expr_t* Atm_ExprAlloc(Atm_ExprType exprtype, Atm_Expr_t* leftchild, Atm_Expr_t* rightchild);
EXTERN void Atm_ExprFreeExpression(Atm_Expr_t* expr);
EXTERN int Atm_AtomReadPreorder(Atm_Atom_t* atom);
EXTERN int Atm_AtomReadPostorder(Atm_Atom_t* atom);
EXTERN void Atm_AtomSetPreorder(Atm_Atom_t* atom, int preorder);
EXTERN void Atm_AtomSetPostorder(Atm_Atom_t* atom, int postorder);
EXTERN void Atm_AtomChangeName(Atm_Atom_t * atom, char * newname);
EXTERN Atm_Atom_t * Atm_AtomDup(Atm_Atom_t * atom, char * newAtmName, st_table *variableMappingTable);
EXTERN Atm_Expr_t * Atm_ExprSubstituteConstantForDummyVar(Atm_Expr_t *e, Var_Variable_t *var, int c, boolean isEnum);

/**AutomaticEnd***************************************************************/

#endif /* _ATM */
