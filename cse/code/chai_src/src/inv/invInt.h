/**CHeaderFile*****************************************************************

  FileName    [InvInt.h]

  PackageName [Inv]

  Synopsis    [Declarations for internal use.]

  Author      [Freddy Mang and Shaz Qadeer]

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

  Revision    [$Id: invInt.h,v 1.2 2003/02/09 21:29:58 ashwini Exp $]

******************************************************************************/

#ifndef _INVINT
#define _INVINT

#define MAX_LENGTH_OF_VAR_NAME 512 

#include  "inv.h"

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct InvExprStruct InvExpr_t;


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
/**Enum************************************************************************

  Synopsis    [Types of expressions.]

  Description [This is an enumeration of the different types of expressions
               that are possible in guards and right hand sides of
               assignments.]

******************************************************************************/
typedef enum {
  InvAnd_c,
  InvBoolConst_c,                        /* boolean const */
  InvEqual_c,
  InvEquiv_c,
  InvGreater_c,
  InvGreaterEqual_c,
  InvImplies_c,
  InvIndex_c,                            /* a[expr] */
  InvLess_c,
  InvLessEqual_c,
  InvMinus_c, 
  InvNot_c,    
  InvNumConst_c,                         /* number const (int, nat, range)*/
  InvOr_c,
  InvPlus_c,
  InvUnaryMinus_c,
  InvId_c,
  InvXor_c				/* Ashwini */
} InvExprType;


/**Struct**********************************************************************

  Synopsis    [Structure for an invariant.]

  SeeAlso     [InvFormulaType]
  
******************************************************************************/
struct InvInvariantStruct {
  char *name;
  InvExpr_t *invExpr;
  Atm_Expr_t *typedExpr;
};

/**Struct**********************************************************************

  Synopsis    [Inv Formula Manager]

  Description [This is essentially a hash table that stores the name
  and the pointer to a Inv_formula.]

  SeeAlso     [optional]

******************************************************************************/
struct InvManagerStruct {
  st_table *nameToFormula;
};


/**Struct**********************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct InvExprStruct {
  InvExprType type;
  InvExpr_t *left;
  InvExpr_t *right;
};


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/




/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN void InvInitialize();
EXTERN char * InvObtainNewFormulaName();
EXTERN Inv_Invariant_t * InvInvariantAlloc(char *name, InvExpr_t *expr);
EXTERN void InvInvariantFree(Inv_Invariant_t *invariant);
EXTERN InvExpr_t * InvExprAlloc(InvExprType exprType, InvExpr_t *left, InvExpr_t *right);
EXTERN void InvExprFree(InvExpr_t *invExpr);
EXTERN void InvFormulaAddToManager(Inv_Manager_t *manager, char * name, Inv_Invariant_t * formula);
EXTERN boolean InvExprIsWellFormed(InvExpr_t * expr);
EXTERN char * InvExprConvertToString(InvExpr_t * expr);
EXTERN Atm_Expr_t * InvInvariantCreateTypedExpr(Mdl_Module_t *module, Var_TypeManager_t *typeManager, Inv_Invariant_t *invariant);

/**AutomaticEnd***************************************************************/
EXTERN lsList InvParse(Inv_Manager_t* manager);

#endif /* _INVINT */

