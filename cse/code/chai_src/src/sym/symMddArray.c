/**CFile***********************************************************************

  FileName    [symMddArray.c]

  PackageName [sym]

  Synopsis    [routines to manage an array of MDDs]

  Description [optional]

  SeeAlso     [optional]

  Author      [Sriram K. Rajamani]

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

#include "symInt.h" 

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


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Obtain constant MDD array]

  Description        [Given a size n and a boolean c,  create and
                      return an MDD array of zeros or ones (as specified 
		      by c)
		      It is the caller's responsibilty to free this
		      array as well as the mdds in it]

  SideEffects        [none]

  SeeAlso            [SymMddArrayFree]

******************************************************************************/
array_t *
SymMddArrayObtainConstant(
  Sym_Info_t *symInfo,
  int n,
  boolean c)
{
  int i;
  array_t *resultArray = array_alloc(mdd_t *, 0);

  for (i =0; i < n; i++){
    mdd_t *curBit = c ? mdd_one(symInfo->manager) : mdd_zero(symInfo->manager);
    array_insert_last(mdd_t *, resultArray, curBit);
  }

  return(resultArray);
}


/**Function********************************************************************

  Synopsis           [Door "or" on all components of MDD array  with given mdd]

  Description        [Given an mdd array and an mdd, 
                      perform or on each element of the array with the given mdd,
		      and return the result as and MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayAndMddArray]

******************************************************************************/
array_t *
SymMddArrayOrMdd(
  Sym_Info_t *symInfo,
  array_t *array1,
  mdd_t *m)
{
  int len1 =  array_n(array1);
  array_t *resultArray;
  int i;

  resultArray = array_alloc( mdd_t *, len1);
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curBit;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    curBit = mdd_or(temp1, m, 1, 1);
    array_insert_last(mdd_t *, resultArray, curBit);
  }

  return(resultArray);
}


/**Function********************************************************************

  Synopsis           [Does "and" on all components of MDD array  with given mdd]

  Description        [Given an mdd array and an mdd, 
                      perform and on each element of the array with the given mdd,
		      and return the result as and MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayAndMddArray]

******************************************************************************/
array_t *
SymMddArrayAndMdd(
  Sym_Info_t *symInfo,
  array_t *array1,
  mdd_t *m)
{
  int len1 =  array_n(array1);
  array_t *resultArray;
  int i;

  resultArray = array_alloc( mdd_t *, len1);
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curBit;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    curBit = mdd_and(temp1, m, 1, 1);
    array_insert_last(mdd_t *, resultArray, curBit);
  }

  return(resultArray);
}


/**Function********************************************************************

  Synopsis           [do "implies" on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform component-wise implication and return the result
		      as and MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayAndMddArray]

******************************************************************************/
array_t *
SymMddArrayImpliesMddArray(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  array_t *resultArray;
  int i;
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  resultArray = array_alloc( mdd_t *, len1);
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curBit;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    curBit = mdd_or(temp1, temp2, 0, 1);
    array_insert_last(mdd_t *, resultArray, curBit);
  }

  return(resultArray);
}


/**Function********************************************************************

  Synopsis           [do "or" on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform component-wise or-ing and return the result
		      as and MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayAndMddArray]

******************************************************************************/
array_t *
SymMddArrayOrMddArray(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  array_t *resultArray;
  int i;
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  resultArray = array_alloc( mdd_t *, len1);
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curBit;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    curBit = mdd_or(temp1, temp2, 1, 1);
    array_insert_last(mdd_t *, resultArray, curBit);
  }

  return(resultArray);
}


/**Function********************************************************************

  Synopsis           [do "and" on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform component-wise and-ing and return the result
		      as and MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayOrMddArray]

******************************************************************************/
array_t *
SymMddArrayAndMddArray(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  array_t *resultArray;
  int i;
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  resultArray = array_alloc( mdd_t *, len1);
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curBit;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    curBit = mdd_and(temp1, temp2, 1, 1);
    array_insert_last(mdd_t *, resultArray, curBit);
  }

  return(resultArray);
}


/**Function********************************************************************

  Synopsis           [do "xnor" on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform component-wise xnor-ing and return the result
		      as and MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayOrMddArray]

******************************************************************************/
array_t *
SymMddArrayXnorMddArray(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  array_t *resultArray;
  int i;
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  resultArray = array_alloc( mdd_t *, len1);
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curBit;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    curBit = mdd_xnor(temp1, temp2);
    array_insert_last(mdd_t *, resultArray, curBit);
  }

  return(resultArray);
}

/**Function********************************************************************

  Synopsis           [do "xor" on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform component-wise xor-ing and return the result
		      as and MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayOrMddArray]

******************************************************************************/
array_t *
SymMddArrayXorMddArray(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  array_t *resultArray;
  int i;
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  resultArray = array_alloc( mdd_t *, len1);
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curBit;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    curBit = mdd_xor(temp1, temp2);
    array_insert_last(mdd_t *, resultArray, curBit);
  }

  return(resultArray);
}




/**Function********************************************************************

  Synopsis           [do "add" on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform component-wise addition (propogating 
		      carry from the 0th bit to the last bit)
		      as and MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayOrMddArray]

******************************************************************************/
array_t *
SymMddArrayPlusMddArray(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  array_t *resultArray;
  int i;
  mdd_t *cin = mdd_zero(symInfo->manager);
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  resultArray = array_alloc( mdd_t *, len1);
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *temp3, *curBit, *p1, *p2, *p3;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    temp3 = mdd_xor(temp1, temp2);
    curBit = mdd_xor(temp3, cin);
    mdd_free(temp3);

    p1 = mdd_and(temp1, temp2, 1, 1);
    p2 = mdd_and(temp2,   cin, 1, 1);
    p3 = mdd_and(cin,   temp1, 1, 1);
    temp3 = mdd_or(p1, p2, 1, 1);
    mdd_free(cin);
    cin =  mdd_or(temp3, p3, 1, 1);

    mdd_free(p1);
    mdd_free(p2);
    mdd_free(p3);
    mdd_free(temp3);
    
    array_insert_last(mdd_t *, resultArray, curBit);
  }
  mdd_free(cin);
  return(resultArray);
}


/**Function********************************************************************

  Synopsis           [do "subtract" on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform component-wise subtraction(propogating 
		      borrow from the 0th bit to the last bit)
		      as and MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayAddMddArray]

******************************************************************************/
array_t *
SymMddArrayMinusMddArray(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  array_t *resultArray;
  int i;
  mdd_t *cin = mdd_zero(symInfo->manager);
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  resultArray = array_alloc( mdd_t *, len1);
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *temp3, *curBit, *p1, *p2, *p3;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    temp3 = mdd_xor(temp1, temp2);
    curBit = mdd_xor(temp3, cin);
    mdd_free(temp3);

    p1 = mdd_and(temp1, temp2, 0, 1);
    p2 = mdd_and(temp2,   cin, 1, 1);
    p3 = mdd_and(cin,   temp1, 1, 0);
    temp3 = mdd_or(p1, p2, 1, 1);
    mdd_free(cin);
    cin =  mdd_or(temp3, p3, 1, 1);

    mdd_free(p1);
    mdd_free(p2);
    mdd_free(p3);
    mdd_free(temp3);
    
    array_insert_last(mdd_t *, resultArray, curBit);
  }
  mdd_free(cin);
  return(resultArray);
}



/**Function********************************************************************

  Synopsis           [do "not" on MDD array]

  Description        [Given an mdd array perform component-wise 
                      negation and return the result
		      as and MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayNot]

******************************************************************************/
array_t *
SymMddArrayNot(
  Sym_Info_t *symInfo,
  array_t *mddArray)
{
  int len = array_n(mddArray);
  int i;
  array_t *resultArray = array_alloc(mdd_t *, 0);

  for ( i = 0; i < len; i++){
    mdd_t *curBit = (mdd_t *)array_fetch(mdd_t *, mddArray, i);
    mdd_t *notCurBit = mdd_not(curBit);
    array_insert_last(mdd_t *, resultArray, notCurBit);
  }

  return(resultArray);
}


/**Function********************************************************************

  Synopsis           [do "and" of all MDD array components]

  Description        [Given an mdd array, perform and-ing of all
                      components and return the result  as an MDD]

  SideEffects        [none]

  SeeAlso            [SymMddArrayAndComponents]

******************************************************************************/
mdd_t *
SymMddArrayAndComponents(
  Sym_Info_t *symInfo,
  array_t *mddArray)
{
  int len = array_n(mddArray);
  mdd_t *result = mdd_one(symInfo->manager);
  int i;

  for ( i = 0; i < len; i++){
    mdd_t *curBit = (mdd_t *)array_fetch(mdd_t *, mddArray, i);
    mdd_t *temp   = mdd_and(curBit, result, 1, 1);
    mdd_free(result);
    result = temp;
  }

  return(result);
}



/**Function********************************************************************

  Synopsis           [do equality check on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform component-wise equality checking
		      and return the result as an MDD.
		      One could interpret the result as a collection of
		      conditions on the variables that make the MDD arrays
		      equal]

  SideEffects        [none]

  SeeAlso            [SymMddArrayCompareLesser]

******************************************************************************/

mdd_t *
SymMddArrayCompareEqual(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  mdd_t *result;
  int i;
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  result =  mdd_one(symInfo->manager);
  
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curBit, *temp;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    curBit = mdd_xnor(temp1, temp2);
    temp   = mdd_and(curBit, result, 1, 1);
    mdd_free(curBit);
    mdd_free(result);
    result = temp;
  }

  return(result);
}


/**Function********************************************************************

  Synopsis           [do Reduction And of an Array]

  Description        []

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/

mdd_t *
SymMddArrayRedAnd(
  Sym_Info_t *symInfo,
  array_t *array1)
{
  int len =  array_n(array1);
  mdd_t *result;
  int i;

  result =  mdd_one(symInfo->manager);
  
  for (i = 0; i < len; i++){
    mdd_t * temp1, *curBit, *temp;

    curBit = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp   = mdd_and(curBit, result, 1, 1);
    mdd_free(result);
    result = temp;
  }

  return(result);
}

/**Function********************************************************************

  Synopsis           [do Reduction Or of an Array]

  Description        []

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/

mdd_t *
SymMddArrayRedOr(
  Sym_Info_t *symInfo,
  array_t *array1)
{
  int len =  array_n(array1);
  mdd_t *result;
  int i;

  result =  mdd_zero(symInfo->manager);
  
  for (i = 0; i < len; i++){
    mdd_t * temp1, *curBit, *temp;

    curBit = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp   = mdd_or(curBit, result, 1, 1);
    mdd_free(result);
    result = temp;
  }

  return(result);
}

/**Function********************************************************************

  Synopsis           [do Reduction Or of an Array]

  Description        []

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/

mdd_t *
SymMddArrayRedXor(
  Sym_Info_t *symInfo,
  array_t *array1)
{
  int len =  array_n(array1);
  mdd_t *result;
  int i;

  result =  mdd_zero(symInfo->manager);
  
  for (i = 0; i < len; i++){
    mdd_t * temp1, *curBit, *temp;

    curBit = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp   = mdd_xor(curBit, result);
    mdd_free(result);
    result = temp;
  }

  return(result);
}



/**Function********************************************************************

  Synopsis           [do greater than check on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform greater than checking 
		      and return the result as an MDD.
                      (The lsb is at bit zero.)
		      One could interpret the result as a collection of
		      conditions on the variables that make the first
		      MDD array greater than the second]

  SideEffects        [none]

  SeeAlso            [SymMddArrayCompareLesser]

******************************************************************************/
mdd_t *
SymMddArrayCompareGreater(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  mdd_t *result;
  int i;
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  result =  mdd_zero(symInfo->manager);
  
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curGt, *temp, *curEq;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    
    curGt = mdd_and(temp1, temp2, 1, 0);
    curEq  = mdd_xnor(temp1, temp2);
    temp   = mdd_and(curEq, result, 1, 1);
    mdd_free(result);

    result = mdd_or(curGt, temp, 1, 1);
    mdd_free(curGt);
    mdd_free(curEq);
    mdd_free(temp);
  }
  return(result);
}


/**Function********************************************************************

  Synopsis           [do lesser than check on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform lesser than checking
		      and return the result as an MDD. (The lsb is at bit zero.)
		      One could interpret the result as a collection of
		      conditions on the variables that make the first
		      MDD array lesser than the second]

  SideEffects        [none]

  SeeAlso            [SymMddArrayCompareGreater]

******************************************************************************/

mdd_t *
SymMddArrayCompareLesser(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  mdd_t *result, *cin;
  int i;
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  result =  mdd_zero(symInfo->manager);
  
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curLt, *temp, *curEq;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    
    curLt = mdd_and(temp1, temp2, 0, 1);
    curEq  = mdd_xnor(temp1, temp2);
    temp   = mdd_and(curEq, result, 1, 1);
    mdd_free(result);

    result = mdd_or(curLt, temp, 1, 1);
    mdd_free(curLt);
    mdd_free(curEq);
    mdd_free(temp);
  }
  return(result);
}



/**Function********************************************************************

  Synopsis           [do greater than or equal check on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform "greater than or equal" checking
		      and return the result as an MDD. (The lsb is at bit zero.)
		      One could interpret the result as a collection of
		      conditions on the variables that make the first
		      MDD array greater than or equal to the second]

  SideEffects        [none]

  SeeAlso            [SymMddArrayCompareGreater]

******************************************************************************/
mdd_t *
SymMddArrayCompareGreaterEqual(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  mdd_t *result, *cin;
  int i;
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  result =  mdd_one(symInfo->manager);
  
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curGt, *temp, *curEq;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    
    curGt = mdd_and(temp1, temp2, 1, 0);
    curEq  = mdd_xnor(temp1, temp2);
    temp   = mdd_and(curEq, result, 1, 1);
    mdd_free(result);

    result = mdd_or(curGt, temp, 1, 1);
    mdd_free(curGt);
    mdd_free(curEq);
    mdd_free(temp);
  }
  return(result);
}


/**Function********************************************************************

  Synopsis           [do lesser than or equal check on two MDD arrays]

  Description        [Given two mdd arrays of equal length, 
                      perform "lesser than or equal to" checking
		      and return the result as an MDD. (The lsb is the at bit zero).
		      One could interpret the result as a collection of
		      conditions on the variables that make the first
		      MDD array lesser than or equal to the second]

  SideEffects        [none]

  SeeAlso            [SymMddArrayCompareGreaterEqual]

******************************************************************************/
mdd_t *
SymMddArrayCompareLesserEqual(
  Sym_Info_t *symInfo,
  array_t *array1,
  array_t *array2)
{
  int len1 =  array_n(array1);
  int len2 =  array_n(array2);
  mdd_t *result, *cin;
  int i;
  
  if(len1 != len2){
    Main_MochaErrorPrint("SymMddArray:operands have unequal length: %d and %d\n", 
                         len1, len2);
    assert(0);
  }

  result =  mdd_one(symInfo->manager);
  
  for (i = 0; i < len1; i++){
    mdd_t * temp1, *temp2, *curLt, *temp, *curEq;
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    temp2 = (mdd_t *)array_fetch(mdd_t *, array2, i);
    
    curLt = mdd_and(temp1, temp2, 0, 1);
    curEq  = mdd_xnor(temp1, temp2);
    temp   = mdd_and(curEq, result, 1, 1);
    mdd_free(result);

    result = mdd_or(curLt, temp, 1, 1);
    mdd_free(curLt);
    mdd_free(curEq);
    mdd_free(temp);
  }
  return(result);
}


/**Function********************************************************************

  Synopsis           [free MDD array]

  Description        [Free the MDD array along with all MDDs contained in it]

  SideEffects        [none]

  SeeAlso            [SymMddArrayObtainConstant]

******************************************************************************/
void
SymMddArrayFree(
  array_t *mddArray)
{
  int len = array_n(mddArray);
  int i;

  for ( i = 0; i < len; i++){
    mdd_t *curBit = (mdd_t *)array_fetch(mdd_t *, mddArray, i);
    mdd_free(curBit);
  }
  array_free(mddArray);
}

/**Function********************************************************************

  Synopsis           [print MDD array]

  Description        [print all MDDs in the MDD array]

  SideEffects        [none]

  SeeAlso            [SymMddArrayPrint]

******************************************************************************/
void
SymMddArrayPrint(
  Sym_Info_t *symInfo,
  array_t *array1)
{
  int len1 =  array_n(array1);
  int i;
  Main_MochaPrint("size = %d\n", len1);
  
  for (i = 0; i < len1; i++){
    mdd_t * temp1;
    Main_MochaPrint(" element : %d : ", i);
    temp1 = (mdd_t *)array_fetch(mdd_t *, array1, i);
    Sym_MddPrintCubes(symInfo, temp1 , 0);
  }
}

  
/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/



