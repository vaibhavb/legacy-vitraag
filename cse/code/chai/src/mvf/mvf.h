/**CHeaderFile*****************************************************************

  FileName    [mvf.h]

  PackageName [mvf]

  Synopsis    [Creation and manipulation of MDD-based multi-valued functions.]

  Description [This package is used to create and manipulate single output
  functions that take multiple values, and are defined over multi-valued
  variables.  Mathematically, such a function is described as, f: Y1 x Y2 x
  ... x Yn --> Yn+1.  Each Yi is a finite, ordered set; if Yi is of
  cardinality k, then the elements of Yi are {0, 1, ..., k-1}. We use yi do
  denote a variable over Yi. If<p>

  A single MDD over variables y1,...,yn cannot be used to represent f, because
  an MDD can only represent binary-valued functions, not multi-valued
  functions.  Instead, to represent f, we use an array of MDDs, of length
  equal to the cardinality of Yn+1.  Each MDD of this array is defined over
  y1,...,yn.  Furthermore, the minterms for which the ith MDD, fi, evaluates
  to one, are exactly those minterms for which f evaluates to the ith member
  of Yn+1.  If f is deterministic, then the intersection of fi and fj, for i
  not equal to j, is empty.  If f is completely specified, then the union of
  the fi's is the tautology.  The union of the fi's is referred to as the
  "domain" of the function.]

  SeeAlso     [mdd]

  Author      [Tom Shiple]

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

  Revision    [$Id: mvf.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _MVF
#define _MVF

/*---------------------------------------------------------------------------*/
/* Nested includes                                                           */
/*---------------------------------------------------------------------------*/
#include  "main.h"

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef array_t Mvf_Function_t;


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**Macro***********************************************************************

  Synopsis     [Iterates over the components of a multi-valued function.]

  Description [This macro iterates over the components of a multi-valued
  function.]

  SideEffects  [This macro instantiates macros from the array package.  Hence
  it is advisable not to nest this macro within array macros.]

  SeeAlso      [Mvf_FunctionAlloc]

******************************************************************************/
#define Mvf_FunctionForEachComponent(                                       \
  /* Mvf_Function_t * */ function  /* function to iterate components */,    \
  /* int */              i         /* local variable for iterator */,       \
  /* mdd_t * */          component /* component of function */              \
)                                                                           \
  arrayForEachItem(mdd_t *, (array_t *) function, i, component)


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN void Mvf_Init();
EXTERN void Mvf_End();
EXTERN Mvf_Function_t * Mvf_FunctionAlloc(mdd_manager *mddManager, int n);
EXTERN void Mvf_FunctionAddMintermsToComponent(Mvf_Function_t *function, int i, mdd_t *g);
EXTERN mdd_t * Mvf_FunctionBuildRelationWithVariable(Mvf_Function_t *function, int mddId);
EXTERN int Mvf_FunctionReadNumComponents(Mvf_Function_t *function);
EXTERN mdd_manager * Mvf_FunctionReadMddManager(Mvf_Function_t *function);
EXTERN Mvf_Function_t * Mvf_FunctionDuplicate(Mvf_Function_t *function);
EXTERN void Mvf_FunctionFree(Mvf_Function_t *function);
EXTERN void Mvf_FunctionArrayFree(array_t *functionArray);
EXTERN mdd_t * Mvf_FunctionObtainComponent(Mvf_Function_t *function, int i);
EXTERN mdd_t * Mvf_FunctionReadComponent(Mvf_Function_t *function, int i);
EXTERN Mvf_Function_t * Mvf_FunctionCreateFromVariable(mdd_manager *mddManager, int mddId);
EXTERN Mvf_Function_t * Mvf_FunctionComposeWithFunctionArray(Mvf_Function_t *f, array_t *mddIdArray, array_t *functionArray);
EXTERN Mvf_Function_t * Mvf_FunctionComposeWithFunction(Mvf_Function_t *f, int mddId, Mvf_Function_t *g);
EXTERN mdd_t * Mvf_MddComposeWithFunction(mdd_t *f, int mddId, Mvf_Function_t *g);
EXTERN boolean Mvf_FunctionTestIsDeterministic(Mvf_Function_t *function);
EXTERN boolean Mvf_FunctionTestIsCompletelySpecified(Mvf_Function_t *function);
EXTERN boolean Mvf_FunctionTestIsConstant(Mvf_Function_t *function, int *constantValue);
EXTERN boolean Mvf_FunctionTestIsNonDeterministicConstant(Mvf_Function_t *function);
EXTERN mdd_t * Mvf_FunctionComputeDomain(Mvf_Function_t *function);
EXTERN boolean Mvf_FunctionTestIsWellFormed(Mvf_Function_t *function);
EXTERN boolean Mvf_FunctionTestIsEqualToFunction(Mvf_Function_t *function1, Mvf_Function_t *function2);
EXTERN mdd_t * Mvf_FunctionsComputeEquivalentSet(Mvf_Function_t *function1, Mvf_Function_t *function2);
EXTERN Mvf_Function_t * Mvf_FunctionCofactor(Mvf_Function_t * function, mdd_t * wrtMdd);
EXTERN Mvf_Function_t * Mvf_FunctionMinimize(Mvf_Function_t *f, mdd_t *c);
EXTERN long Mvf_FunctionArrayComputeNumBddNodes(array_t * functionArray);
EXTERN long Mvf_FunctionComputeNumBddNodes(Mvf_Function_t * function);
EXTERN int Mvf_FunctionFindFirstTrueComponent(Mvf_Function_t * function);
EXTERN int Mvf_FunctionComputeHashValue(Mvf_Function_t * function);
EXTERN array_t * Mvf_FunctionComputeSupport(Mvf_Function_t *function, mdd_manager *mddMgr, int *value);
EXTERN Mvf_Function_t * Mvf_FunctionComputeAddition(Mvf_Function_t *mvf1, Mvf_Function_t *mvf2);
EXTERN Mvf_Function_t * Mvf_FunctionComputeSubtraction(Mvf_Function_t *mvf1, Mvf_Function_t *mvf2);
EXTERN Mvf_Function_t * Mvf_FunctionComputeITE(mdd_t *ifMdd, Mvf_Function_t *thenMvf, Mvf_Function_t *elseMvf);

/**AutomaticEnd***************************************************************/


#endif /* _MVF */



