/**CFile***********************************************************************

  FileName    [atmParse.c]

  PackageName [atm]

  Synopsis    [The file provides typechecking functions for expressions.]

  Description [Provides functions required in typechecking and for modifying 
  expressions so that the MDDs can be recursively built.]

  SeeAlso     [Atm.h, AtmInt.h]

  Author      [Shaz Qadeer, Freddy Mang]

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

#include  "atmInt.h"


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


/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Converts leaves of type Atm_NumConst_c of the expr to
                      Atm_BitwiseNumConst_c.]

  Description        [The function expects the argument expression to have all
  its children of type Atm_IfThenElseFi_c or Atm_NumConst_c. It modifies the
  leaf Atm_NumConst_c expression to a bitvector with numBits which when
  interpreted as an integer has value equal to the value of the numerical
  constant in the old expression.]

  SideEffects        [expr is modified.]

  SeeAlso            [Atm_NumConstExprConvertToRangeConstExpr]

******************************************************************************/
void
Atm_NumConstExprConvertToBitvectorConstExpr(
  Atm_Expr_t *expr,
  int numBits)
{
  int i;

  long numConst = (long) expr->leftExpr;
  long one = 1;
  boolean negative = (numConst < 0);
  char *bitArray = ALLOC(char, numBits);
  
  if (negative)
    numConst = -1 * numConst;
  
  for (i = 0; i < numBits; i++) {
    bitArray[i] = (numConst & one);
    numConst >>= 1;
  }
  
  if (negative) {
    /* Subtract the bit array obtained from 2^numBits 
     * First flip the bits and then add 1. I am doing the 
     * two operations simultaneously. */
    int carry = 1;
    
    for (i = 0; i < numBits; i++) {
      if (carry == 0) {
        bitArray[i] = !(bitArray[i]);
      } else {
        if (bitArray[i] == 1)
          carry = 0;
      }
    }
  }
  
  expr->type = Atm_BitwiseNumConst_c;
  expr->leftExpr = (Atm_Expr_t *) bitArray;
  expr->rightExpr = (Atm_Expr_t *) (long) numBits;
  
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           []

  Description        [Returns a pointer to the left child pointer of expr.
                      Allows for in-place modification of the expression tree.]

  SideEffects        [None]

******************************************************************************/
Atm_Expr_t **
AtmExprReadLeftChildAddress(
  Atm_Expr_t * expr) 
{
  return &(expr -> leftExpr);

}

/**Function********************************************************************

  Synopsis           []

  Description        [Returns a pointer to the right child pointer of expr.
                      Allows for in-place modification of the expression tree.]

  SideEffects        [None]

******************************************************************************/
Atm_Expr_t **
AtmExprReadRightChildAddress(
  Atm_Expr_t * expr) 
{
  return &(expr -> rightExpr);

}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


