/**CHeaderFile*****************************************************************

  FileName    [atmInt.h]

  PackageName [atm]

  Synopsis    [Internal header file for the Atm package.]

  Description [This file contains data structures and routines to manipulate 
               atoms.]

  SeeAlso     [atm.h]

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

  THE UNIVERSITY OF CALIFORNIA
  atm.h
  SPECIFICALLY DISCLAIMS ANY WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN
  "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.]

  Revision    [$Id: atmInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _ATMINT
#define _ATMINT

#include  "atm.h"
/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
/**Struct**********************************************************************

  Synopsis    [AtmAtomStruct]

  Description []

  SeeAlso     []

******************************************************************************/
struct AtmAtomStruct {
  char *name;
  lsList initCmdList;
  lsList updateCmdList;
  lsList waitCmdList;

  Atm_Cmd_t *defaultInitCmd;
  Atm_Cmd_t *defaultUpdateCmd;
  
  lsList ctrlVarList;
  lsList readVarList;
  lsList awaitVarList;

  Atm_AtomType atmType;
  
  /* these two are used in the acyclicity test in giving
     a topological sort of the atoms */
  int preorder;
  int postorder;  

};

/**Struct**********************************************************************

  Synopsis    [Structure for the guarded command.]

  Description [This is the data structure for a guarded command.]

  SeeAlso     [Atm_Expr_t, Atm_Assign_t]

******************************************************************************/
struct AtmCmdStruct {
  Atm_Expr_t *guard;
  lsList assignList;
};

/**Struct**********************************************************************

  Synopsis    [Structure for a boolean or arithmetic expression.]

  Description [This is the data structure for boolean and arithmetic
               expressions to represent guards and right hand sides of
               assignments. Expressions are in the form of trees, the leaves
               of which are expressions of type AtmPrimedVar_c, AtmUnPrimed_c,
	       and AtmConst_c. For
               leaf expressions both leftExpr and rightExpr should be NULL. If
               an expression type has only one child rightExpr should be NULL.
               This happens in types AtmEventSend_c and AtmEventQuery_c.

	       The member constVal is used to store the boolean constants (true/
	       false), integer constants (...-2,-1,0,1,2...), range and
	       natural numbers.  They are not supposed to be changed once
	       set.]

  SeeAlso     [Atm_ExprType_t]

******************************************************************************/
struct AtmExprStruct {
  Atm_ExprType type;
  Atm_Expr_t *leftExpr;
  Atm_Expr_t *rightExpr;
};


/**Struct**********************************************************************

  Synopsis    [Structure for an assignment.]

  Description [This is the data structure for an assignment to a variable.]

  SeeAlso     [Var_Variable_t*, Atm_Expr_t]

******************************************************************************/
struct AtmAssignStruct {
  Atm_AssignType type;
  Var_Variable_t *var;
  Atm_Expr_t *expr;               /* expr for the right side of assignment */
  union {
    Atm_Expr_t *indexExpr;        /* assignType == Atm_AssignIndex_c:
                                     ptr to expression of type enum const or a
                                     num const */ 
    Var_Variable_t *indexVar;     /* assignType == Atm_AssignForall_c:
                                     the dummy index variable */
  } index;
};

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

EXTERN Atm_Expr_t ** AtmExprReadLeftChildAddress(Atm_Expr_t * expr);
EXTERN Atm_Expr_t ** AtmExprReadRightChildAddress(Atm_Expr_t * expr);
EXTERN void ExprResolveTypes(Atm_Expr_t * expr, Var_Type_t * type);
EXTERN Atm_ExprType ScalarDataTypeConvertToBitvectorDataType(Atm_ExprType exprType);
EXTERN void AtmControlVarsDup(Atm_Atom_t* newatom, Atm_Atom_t* oldatom, st_table* variableMappingTable);
EXTERN void AtmReadVarsDup(Atm_Atom_t* newatom, Atm_Atom_t* oldatom, st_table * variableMappingTable);
EXTERN void AtmAwaitVarsDup(Atm_Atom_t* newatom, Atm_Atom_t* oldatom, st_table * variableMappingTable);
EXTERN void AtmGuardedCommandsDup(Atm_Atom_t* newatom, Atm_Atom_t* oldatom, st_table* variableMappingTable);
EXTERN void AtmSelectedCommandsDup(Atm_Atom_t* newatom, Atm_Atom_t* oldatom, st_table* variableMappingTable, int which);
EXTERN lsList AtmAssignListDup(lsList oldassignlist, st_table *variableMappingTable);
EXTERN Atm_Expr_t* AtmExprDup(Atm_Expr_t* expr, st_table* variableMappingTable);

/**AutomaticEnd***************************************************************/

#endif /* _ATMINT */









