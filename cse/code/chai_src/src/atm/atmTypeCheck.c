/**CFile***********************************************************************

  FileName    [atmTypeCheck.c]

  PackageName [atm]

  Synopsis    [Contains routine for type checking.]

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

******************************************************************************/

#include "atmInt.h" 

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/* number of sets below */

#define TC_NumOfSets             18

/* common sets */
#define TC_emptySet              0
#define TC_univeralSet           1
#define TC_naturalSet            2
#define TC_integerSet            3
#define TC_eventSet              4
#define TC_boolSet               5
#define TC_allTimerSet           6

/* then for each operator there is a sotOperator */
#define TC_allNumSet             7 
#define TC_allArraySet           8 
#define TC_allBVSet              9 
#define TC_allRangeSet           10
#define TC_allEnumSet            11

/* this one is used by Atm_Nondet_c */
#define TC_nondetSet             12

/* this one is used by And, NotEquiv,  Equiv, Or, Implies, Not */
#define TC_boolConnectiveSet     13

/* this one is used by Equal, NotEqual */
#define TC_equalSet              14

/* this one is used by Plus, Minus, UnaryMinus */
#define TC_arithmeticSet         15

/* this one is used by index of an array */
#define TC_indexSet              16


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
/* defines a SetOfType as an array of integers */

typedef unsigned long long TypeVector_t;

typedef struct {

  int num;
  TypeVector_t * typeVector; /* an array of type vectors */

} AtmSetOfType_t;

typedef struct {
  int numTypes;
  Var_TypeManager_t * typeManager;
  array_t * typePointerArray;
  array_t * SOTArray;

} AtmTypeCheckManager_t;


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
static AtmTypeCheckManager_t * globalTypeCheckManager = NIL(AtmTypeCheckManager_t);

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static AtmSetOfType_t * SOTGet(int whichSet);
static AtmSetOfType_t * SOTCreate(Var_Type_t * type);
static boolean SOTIsEmpty(AtmSetOfType_t* sot);
static AtmSetOfType_t * SOTUnion(int numArgs, ...);
static AtmSetOfType_t * SOTIntersect(int numArgs, ...);
static boolean SOTSubset(AtmSetOfType_t * x, AtmSetOfType_t * y);
static boolean SOTEqual(AtmSetOfType_t * x, AtmSetOfType_t * y);
static AtmSetOfType_t * SOTMinus(AtmSetOfType_t * x, AtmSetOfType_t * y);
static AtmSetOfType_t * SOTDup(AtmSetOfType_t * x);
static AtmSetOfType_t * SOTAlloc();
static void SOTFree(AtmSetOfType_t * sot);
static AtmSetOfType_t * SOTObtainFromVar(Var_Variable_t * var);
static AtmSetOfType_t * SOTObtainFromExpr(Atm_Expr_t * expr);
static array_t * TypeArrayObtainFromSOT(AtmSetOfType_t * sot);
static void PrintSOT(AtmSetOfType_t * sot);
static void TCManagerFree(AtmTypeCheckManager_t * TCManager);
static Var_Type_t * PickAType(AtmSetOfType_t * sot);
static void ExprSimplify2(Atm_Expr_t ** expr);
static int BitArrayToNum(char * bitArray, int numBits);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Typecheck an expression.]

  Description        [Given a type "type" and an atmExpr_t * "expr", this function
                      checks if "type" is contained in the allowable types of "expr".
		      In general an expression may allow multiple types. For example,
		      the number constant "1" may be interpreted as a range constant,
		      an integer, a nat or the boolean constant "true".

		      Upon success typechecking, the routine does the following 
		      two things in order: 1. It resolve the types of expression.
		      In the previous example, it may turn the node "1" into a 
		      boolean constant node "true". 2. It then simplifies the
		      expression by evaluating the sub-expression containing
		      only constants. For example, the sub-expression "true | false", 
		      will be evaluted to "true".]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean Atm_TypeCheckNew(
  Var_Type_t * type,
  Atm_Expr_t ** expr) 
{
  AtmSetOfType_t * sotLeft, *sotRight, *sotInter;
  boolean result = TRUE;
  
  sotLeft  = SOTCreate(type);
  sotRight = SOTObtainFromExpr(*expr);
  
  sotInter = SOTIntersect(2, sotLeft, sotRight);

  if (SOTIsEmpty(sotInter)) {
    
    result = FALSE;
  }
  
  SOTFree(sotLeft);
  SOTFree(sotRight);
  SOTFree(sotInter);
  
  if (result) {

    ExprResolveTypes (*expr, type);
    ExprSimplify2 (expr);
    return TRUE;
    
  }
  
  return result;
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Atm_ReinitTypeCheck(
  Var_TypeManager_t* typeManager
  )
{
  st_generator * gen;
  Var_Type_t * type;
  int i, vectorCount;
  TypeVector_t mask;
  AtmSetOfType_t *tmpSOT;
  AtmSetOfType_t *allArraySet;
  AtmSetOfType_t *allBVSet;
  AtmSetOfType_t *allRangeSet;
  AtmSetOfType_t *allEnumSet;
  AtmSetOfType_t *allTimerSet;
  AtmSetOfType_t *naturalSet;
  AtmSetOfType_t *integerSet;
  AtmSetOfType_t *emptySet;
  AtmSetOfType_t *universalSet;
  AtmSetOfType_t *eventSet;
  AtmSetOfType_t *boolSet;
  array_t * sotArray;


  if (globalTypeCheckManager) {
    TCManagerFree(globalTypeCheckManager);
  }
  
  globalTypeCheckManager = ALLOC(AtmTypeCheckManager_t, 1);

  globalTypeCheckManager -> typeManager = typeManager;

  /* initializing globalTypeCheckManager -> numTypes */
  /* this has to be the first field to be initialized in order
     for the rest of the initialization to be correct */

  i = 0;
  Var_TypeManagerForEachVarType(typeManager, gen, type) {
    i ++;
  }

  globalTypeCheckManager -> numTypes = i;

  /* initializing globalTypeCheckManager -> typePointerArray */
  globalTypeCheckManager -> typePointerArray = array_alloc(Var_Type_t *, 0);

  Var_TypeManagerForEachVarType(typeManager, gen, type) {
    array_insert_last(Var_Type_t *,
                 globalTypeCheckManager -> typePointerArray,
                 type);
  }

  sotArray = array_alloc(AtmSetOfType_t *, TC_NumOfSets);
  globalTypeCheckManager -> SOTArray = sotArray;

  /* initializing globalTypeCheckManager -> emptySet etc. */
  emptySet        = SOTCreate(NIL(Var_Type_t));
  naturalSet      = SOTCreate(NatType);
  integerSet      = SOTCreate(IntType);
  eventSet        = SOTCreate(EventType);
  boolSet         = SOTCreate(BoolType);

  
  /* initializing globalTypeCheckManager -> allArraySet etc. */
  universalSet    = SOTAlloc();
  allArraySet     = SOTAlloc();
  allBVSet        = SOTAlloc();
  allRangeSet     = SOTAlloc();
  allEnumSet      = SOTAlloc();
  allTimerSet     = SOTAlloc();
  
  vectorCount = 0;
  mask = 1;
  arrayForEachItem (Var_Type_t *,
                    globalTypeCheckManager -> typePointerArray,
                    i,
                    type) {

    universalSet -> typeVector[vectorCount] |= mask;
    
    if (Var_TypeIsArray(type)) 
      allArraySet -> typeVector[vectorCount] |=  mask;

    if (Var_TypeIsBV(type)) 
      allBVSet -> typeVector[vectorCount] |=  mask;

    if (Var_TypeIsRange(type)) 
      allRangeSet -> typeVector[vectorCount] |=  mask;

    if (Var_TypeIsEnum(type)) 
      allEnumSet -> typeVector[vectorCount] |=  mask;

    if (Var_TypeIsTimer(type))
      allTimerSet -> typeVector[vectorCount] |=  mask;
    
    mask <<= 1;

    if (mask == 0) {
      mask = 1;
      vectorCount ++;
    }
  }

  array_insert(AtmSetOfType_t *, sotArray, TC_emptySet,   emptySet);
  array_insert(AtmSetOfType_t *, sotArray, TC_naturalSet, naturalSet);
  array_insert(AtmSetOfType_t *, sotArray, TC_integerSet, integerSet);
  array_insert(AtmSetOfType_t *, sotArray, TC_eventSet,   eventSet);
  array_insert(AtmSetOfType_t *, sotArray, TC_boolSet,    boolSet);
  array_insert(AtmSetOfType_t *, sotArray, TC_univeralSet,     universalSet);
  array_insert(AtmSetOfType_t *, sotArray, TC_allArraySet,     allArraySet);
  array_insert(AtmSetOfType_t *, sotArray, TC_allBVSet, allBVSet);
  array_insert(AtmSetOfType_t *, sotArray, TC_allRangeSet,     allRangeSet);
  array_insert(AtmSetOfType_t *, sotArray, TC_allEnumSet,      allEnumSet);
  array_insert(AtmSetOfType_t *, sotArray, TC_allTimerSet,      allTimerSet);

  /* initializing globalTypeCheckManager -> allNumSet */
  array_insert(AtmSetOfType_t *, sotArray, TC_allNumSet,      
               SOTUnion(5,
                        integerSet,
                        naturalSet,
                        allTimerSet,
                        allRangeSet,
                        allBVSet
                        ));

  array_insert(AtmSetOfType_t *, sotArray, TC_indexSet,      
               SOTUnion(2,
                        allRangeSet,
                        allEnumSet
                        ));

  /* initializing nondetSet, boolConnectiveSet, equalSet and arithmeticSet */
  array_insert(AtmSetOfType_t *, sotArray, TC_nondetSet,
               SOTMinus(universalSet, eventSet));

  /* this one is used by And, NotEquiv,  Equiv, Or, Implies, Not */
  array_insert(AtmSetOfType_t *, sotArray, TC_boolConnectiveSet,
               SOTUnion(2, boolSet, allBVSet));

  /* this one is used by Equal, NotEqual */
  tmpSOT = SOTUnion(2, eventSet, allArraySet);
  array_insert(AtmSetOfType_t *, sotArray, TC_equalSet,
               SOTMinus(universalSet, tmpSOT));
  SOTFree(tmpSOT);
  
  /* this one is used by Plus, Minus, UnaryMinus */
  array_insert(AtmSetOfType_t *, sotArray, TC_arithmeticSet,
               SOTUnion(4,
                        allRangeSet, allBVSet,
                        naturalSet,  integerSet));
                             
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*                                                                           */
/* The followings are operations on set of types (AtmSetOfType_t)            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static AtmSetOfType_t *
SOTGet(
  int whichSet
  ) 
{
  if ((0 <= whichSet) && (whichSet < TC_NumOfSets)) {
    return array_fetch(AtmSetOfType_t *,
                       globalTypeCheckManager -> SOTArray,
                       whichSet);
  } else {
    fprintf(stderr, "Error: unknown type of set.\n");
    assert(0);
  }
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static AtmSetOfType_t *
SOTCreate(
  Var_Type_t * type) 
{
  
  AtmSetOfType_t * resultSOT;
  Var_Type_t * type2;
  int i, vectorCount;
  TypeVector_t mask = 1;

  resultSOT = SOTAlloc();

  vectorCount=0;
  arrayForEachItem(Var_Type_t *,
                   globalTypeCheckManager -> typePointerArray,
                   i,
                   type2) {

    if (type2 == type) {
      resultSOT -> typeVector[vectorCount] = (resultSOT ->
                                              typeVector[vectorCount])  | mask;
    }

    mask <<= 1;

    if (mask == 0) {
      mask = 1;
      vectorCount ++;
    }
  }


  return resultSOT;
}


/**Function********************************************************************

  Synopsis           [Check if the given SOT is empty.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
SOTIsEmpty(
  AtmSetOfType_t* sot
  ) 
{
  int i;

  for (i=0; i < sot -> num; i++) {
    if (sot -> typeVector[i] != 0)
      return FALSE;
  }

  return TRUE;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static AtmSetOfType_t *
SOTUnion(
  int numArgs,
  ...
  ) 
{
  va_list args;
  int i, j;
  AtmSetOfType_t * sot, *resultSOT;
  
  va_start(args, numArgs);
  
  resultSOT = SOTAlloc();

  for (i=0; i<numArgs; i++) {
    sot = va_arg(args, AtmSetOfType_t *);    

    for (j=0; j < (sot -> num); j++) {
      resultSOT -> typeVector[j] |=  sot -> typeVector[j];
    }
  }

  va_end(args);

  return resultSOT;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static AtmSetOfType_t *
SOTIntersect(
  int numArgs,
  ...
  ) 
{
  va_list args;
  int i, j;
  AtmSetOfType_t * sot, *resultSOT;
  
  va_start(args, numArgs);
  
  resultSOT = SOTAlloc();

  for (i=0; i<numArgs; i++) {
    sot = va_arg(args, AtmSetOfType_t *);    

    if (i == 0) {
      for (j=0; j < (sot -> num); j++) {
        resultSOT -> typeVector [j] = sot -> typeVector [j];
      }
    } else {
      for (j=0; j < (sot -> num); j++) {
        resultSOT -> typeVector [j] &= sot -> typeVector [j];
      }
      
    }
  }
  
  va_end(args);
  
  return resultSOT;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
SOTSubset(
  AtmSetOfType_t * x,
  AtmSetOfType_t * y
  ) 
{
  int i;

  for (i=0; i < (x -> num); i++) {
    if ((x -> typeVector[i]) & ~(y -> typeVector[i]))
      return FALSE;
  }
  
  return TRUE;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
SOTEqual(
  AtmSetOfType_t * x,
  AtmSetOfType_t * y
  ) 
{
  int i;

  for (i=0; i < (x -> num); i++) {
    if ((x -> typeVector[i]) != (y -> typeVector[i]))
      return FALSE;
  }
  
  return TRUE;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static AtmSetOfType_t *
SOTMinus(
  AtmSetOfType_t * x,
  AtmSetOfType_t * y
  ) 
{
  int i;
  AtmSetOfType_t *resultSOT;

  resultSOT = SOTAlloc();

  for (i=0; i < (x -> num); i++) {
    resultSOT -> typeVector [i] =
        ((x -> typeVector[i]) & ~(y -> typeVector[i]));
  }

  return resultSOT;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static AtmSetOfType_t *
SOTDup(
  AtmSetOfType_t * x
  ) 
{
  int i;
  AtmSetOfType_t *resultSOT;

  resultSOT = SOTAlloc();

  for (i=0; i < (x -> num); i++) {
    resultSOT -> typeVector [i] = x -> typeVector [i];
  }

  return resultSOT;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static AtmSetOfType_t *
SOTAlloc()
{
  static int size = sizeof(TypeVector_t);

  AtmSetOfType_t * resultSOT = ALLOC(AtmSetOfType_t, 1);
  int numTypes = globalTypeCheckManager -> numTypes;
  int i, num;

  num = numTypes / size;

  if ((num * size) != numTypes)
    num ++;

  resultSOT -> typeVector = ALLOC(TypeVector_t, num);

  for (i=0; i<num; i++) {
    resultSOT -> typeVector[i] = 0;
  }

  resultSOT -> num = num;

  return resultSOT;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
SOTFree(
  AtmSetOfType_t * sot) 
{
  FREE(sot -> typeVector);
  FREE(sot);
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*                                                                           */
/* The followings are routines for type checking. They make use of the       */
/*   operations on SetOfTypes                                                */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static AtmSetOfType_t *
SOTObtainFromVar(
  Var_Variable_t * var) 
{
  Var_Type_t * type;
  
  if (var == NIL(Var_Variable_t)) {
    type = NIL(Var_Type_t);
  } else {
    type = Var_VariableReadType(var);
  }
  
  return SOTCreate(type);

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [Returns a SOT. User has to free it after use.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static AtmSetOfType_t *
SOTObtainFromExpr(
  Atm_Expr_t * expr)
{
  Atm_ExprType etype;
  AtmSetOfType_t * resultSOT, *tmpSOT;
  
      
  if (expr == NIL(Atm_Expr_t)) {
    return SOTCreate(NIL(Var_Type_t));
  }
  
  etype = Atm_ExprReadType(expr);

  switch (etype) {
      case Atm_Nondet_c:
        resultSOT =  SOTDup(SOTGet(TC_nondetSet));
        break;

      case Atm_PrimedVar_c:
      case Atm_UnPrimedVar_c:
      {
        Var_Type_t *varType = Var_VariableReadType((Var_Variable_t*)
                                                   Atm_ExprReadLeftChild(expr));
        resultSOT =  SOTCreate(varType);
        break;
      }

      case Atm_BoolConst_c:
      case Atm_EventQuery_c:
        resultSOT = SOTDup(SOTGet(TC_boolSet));
        break;

      case Atm_NumConst_c:
      {
        long v = (long) expr -> leftExpr;

        if ((v == 0) || (v == 1)) {
          resultSOT = SOTUnion(2, SOTGet(TC_allNumSet),
                               SOTGet(TC_boolSet));
        } else {
          resultSOT =  SOTDup(SOTGet(TC_allNumSet));          
        }
        
        return resultSOT;
        break;
      }
      
      case Atm_TimerUpperBound_c:
      {
        return SOTDup(SOTGet(TC_allTimerSet));
        break;
      }
      
      case Atm_RangeConst_c :
      {
        /* should not appear during type checking */
        return SOTDup(SOTGet(TC_allRangeSet));
        break;
      }
        
      case Atm_EnumConst_c:
        return SOTCreate(Var_EnumElementReadType((Var_EnumElement_t*) expr->leftExpr));
        break;

      case Atm_Index_c: 
      {
        int flag = TRUE;
        AtmSetOfType_t * leftSOT, *rightSOT, *indexSOT, *entrySOT;
        AtmSetOfType_t * tmpSOT, *returnSOT;
        array_t * typeArray;
        Var_Type_t * type;

        leftSOT  = SOTObtainFromExpr(Atm_ExprReadLeftChild(expr));
        rightSOT = SOTObtainFromExpr(Atm_ExprReadRightChild(expr));

        /* there should only be one type in the typeArray */
        type = PickAType(leftSOT);

        if (SOTSubset(leftSOT, SOTGet(TC_allArraySet))) {
          indexSOT = SOTCreate(Var_VarTypeReadIndexType(type));
          entrySOT = SOTCreate(Var_VarTypeReadEntryType(type));
          
        } else if (SOTSubset(leftSOT, SOTGet(TC_allBVSet))) {

          indexSOT = SOTCreate(
            Var_RangeTypeReadFromRange(globalTypeCheckManager -> typeManager,
                                       Var_VarTypeArrayOrBitvectorReadSize(type))
            );

          entrySOT = SOTDup(SOTGet(TC_boolSet));

        } else {
          flag = FALSE;
        }

        if (flag) {
          tmpSOT = SOTIntersect(2,
                                rightSOT,
                                indexSOT);
          
          if (SOTIsEmpty(tmpSOT)) {
            flag = FALSE;
          }
          SOTFree(tmpSOT);
        }
        
        SOTFree(leftSOT);
        SOTFree(rightSOT);
        SOTFree(indexSOT);

        if (!flag) {
          SOTFree(entrySOT);
          return SOTDup(SOTGet(TC_emptySet));
        } else {
          return entrySOT;
        }

        break;
      }

      case Atm_IfThenElseFi_c:
      {
        AtmSetOfType_t * ifSOT, *thenSOT, *elseSOT;
        
        ifSOT = SOTObtainFromExpr(expr -> leftExpr);
        
        if (!SOTEqual(ifSOT, SOTGet(TC_boolSet))) {

          resultSOT = SOTDup(SOTGet(TC_emptySet));
        } else {

          thenSOT = SOTObtainFromExpr(expr->rightExpr->leftExpr); 
          elseSOT = SOTObtainFromExpr(expr->rightExpr->rightExpr);
          
          tmpSOT  = SOTIntersect(2, thenSOT, elseSOT);
          
          resultSOT = SOTMinus(tmpSOT, SOTGet(TC_eventSet));
          SOTFree(tmpSOT);
          SOTFree(thenSOT);
          SOTFree(elseSOT);
        }

        SOTFree(ifSOT);

        return resultSOT;
        break;
      }
      
      case Atm_Equal_c: 
      case Atm_NotEqual_c:
      {
        AtmSetOfType_t * leftSOT = SOTObtainFromExpr(expr -> leftExpr);
        AtmSetOfType_t * rightSOT= SOTObtainFromExpr(expr -> rightExpr);
        AtmSetOfType_t * tmpSOT  = SOTIntersect(3, leftSOT, rightSOT, SOTGet(TC_equalSet));

        if (!SOTIsEmpty(tmpSOT)) {
          resultSOT = SOTDup(SOTGet(TC_boolSet));
        } else {
          resultSOT = SOTDup(SOTGet(TC_emptySet));
        }

        SOTFree(tmpSOT);
        SOTFree(leftSOT);
        SOTFree(rightSOT);

        return resultSOT;
        break;
      }

      case Atm_And_c:
      case Atm_Or_c:
      case Atm_Xor_c:
      case Atm_Implies_c:
      case Atm_Equiv_c:
      case Atm_NotEquiv_c:
      {
        AtmSetOfType_t * leftSOT = SOTObtainFromExpr(expr -> leftExpr);
        AtmSetOfType_t * rightSOT= SOTObtainFromExpr(expr -> rightExpr);

        resultSOT = SOTIntersect(3, leftSOT, rightSOT,
                                 SOTGet(TC_boolConnectiveSet));

        SOTFree(leftSOT);
        SOTFree(rightSOT);
        break;
      }

      case Atm_Not_c: 
      {
        AtmSetOfType_t * leftSOT = SOTObtainFromExpr(expr -> leftExpr);

        resultSOT = SOTIntersect(2, leftSOT, 
                                 SOTGet(TC_boolConnectiveSet));

        SOTFree(leftSOT);
        break;
      }

      case Atm_Greater_c:
      case Atm_Less_c:
      case Atm_GreaterEqual_c:
      case Atm_LessEqual_c: 
      {
        AtmSetOfType_t * leftSOT = SOTObtainFromExpr(expr -> leftExpr);
        AtmSetOfType_t * rightSOT= SOTObtainFromExpr(expr -> rightExpr);
        AtmSetOfType_t * tmpSOT;
        
        tmpSOT = SOTIntersect(3, leftSOT, rightSOT,
                                 SOTGet(TC_arithmeticSet));


        if (!SOTIsEmpty(tmpSOT)) {
          resultSOT = SOTDup(SOTGet(TC_boolSet));
        } else {
          resultSOT = SOTDup(SOTGet(TC_emptySet));
        }

        SOTFree(leftSOT);
        SOTFree(rightSOT);
        SOTFree(tmpSOT);

        break;
      }

      case Atm_Plus_c: 
      case Atm_Minus_c: 
      {
        AtmSetOfType_t * leftSOT = SOTObtainFromExpr(expr -> leftExpr);
        AtmSetOfType_t * rightSOT = SOTObtainFromExpr(expr -> rightExpr);
        
        resultSOT = SOTIntersect(3, leftSOT, rightSOT,
                                 SOTGet(TC_arithmeticSet));

        SOTFree(leftSOT);
        SOTFree(rightSOT);
        
        break;
      }

      case Atm_UnaryMinus_c:
      {
        AtmSetOfType_t * leftSOT = SOTObtainFromExpr(expr -> leftExpr);
        
        resultSOT = SOTIntersect(2, leftSOT,
                                 SOTGet(TC_arithmeticSet));


        SOTFree(leftSOT);

        break;
      }

      case Atm_RedAnd_c:
      case Atm_RedOr_c:
      case Atm_RedXor_c:
      {
        AtmSetOfType_t * leftSOT = SOTObtainFromExpr(expr -> leftExpr);
        AtmSetOfType_t * tmpSOT;
        
        tmpSOT = SOTIntersect(2, leftSOT,
                                 SOTGet(TC_allBVSet));

        if (!SOTIsEmpty(tmpSOT)) {
          resultSOT = SOTDup(SOTGet(TC_boolSet));
        } else {
          resultSOT = SOTDup(SOTGet(TC_emptySet));
        }

        SOTFree(leftSOT);
        SOTFree(tmpSOT);

        break;
      }

      default:
        Main_MochaErrorPrint("Unknown type encountered in SOTObtainFromExpr()\n");
        resultSOT = SOTDup(SOTGet(TC_emptySet));
        break;
  }

  return resultSOT;
      

}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]


******************************************************************************/
static array_t *
TypeArrayObtainFromSOT (
  AtmSetOfType_t * sot
  )
{
  Var_Type_t * type;
  int i, vectorCount;
  TypeVector_t mask = 1;
  array_t * returnArray = array_alloc(Var_Type_t *, 0);

  if (!sot)
    return returnArray;

  vectorCount=0;
  arrayForEachItem(Var_Type_t *,
                   globalTypeCheckManager -> typePointerArray,
                   i,
                   type) {
    if (sot -> typeVector[vectorCount] & mask) {
      array_insert_last(Var_Type_t *, returnArray, type);
    }

    mask <<= 1;

    if (mask == 0) {
      mask = 1;
      vectorCount ++;
    }
  }

  return returnArray;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [For debugging only.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
PrintSOT (
  AtmSetOfType_t * sot) 
{
  int i, c;
  TypeVector_t mask = 1;
  
  printf("%d: ", sot -> num);

  c=0;
  for (i=0; i< sot ->num; i++) {
    mask = 1;
    while (mask != 0) {
      printf("%d", ((sot -> typeVector[c] & mask) != 0)); 
      mask <<= 1;
    }
    c ++;
    printf(" ");
    
  }
  
  printf("\n");
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
TCManagerFree(
  AtmTypeCheckManager_t * TCManager) 
{

  int i;
  AtmSetOfType_t * sot;

  arrayForEachItem(AtmSetOfType_t*,  TCManager -> SOTArray, i, sot) {
    SOTFree(sot);
  }

  array_free(TCManager -> SOTArray);
  array_free(TCManager -> typePointerArray);

  FREE(TCManager);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
ExprResolveTypes (
  Atm_Expr_t * expr,
  Var_Type_t * type) 
{
  
  Atm_ExprType etype;
  AtmSetOfType_t * resultSOT, *tmpSOT;
      
  if (expr == NIL(Atm_Expr_t)) {
    return;
  }
  
  etype = Atm_ExprReadType(expr);

  switch (etype) {
      case Atm_Nondet_c:
      case Atm_BoolConst_c:
      case Atm_EnumConst_c:
      case Atm_EventQuery_c:
      case Atm_TimerUpperBound_c:
        break;
        
      case Atm_NumConst_c: 
      {
        long v = (long) Atm_ExprReadLeftChild(expr);
        if (Var_TypeIsBoolean(type)) {
          
          assert((v==0) || (v==1));

          expr -> type = Atm_BoolConst_c;
        } else if (Var_TypeIsRange(type)) {
          int range = Var_VarTypeReadDomainSize(type);
          v = v % range;
          if (v < 0)
            v += range;

          expr -> type = Atm_RangeConst_c;
          expr -> leftExpr = (Atm_Expr_t *) v;
          expr -> rightExpr = (Atm_Expr_t *) (long) range;
        } else if (Var_TypeIsBV(type)) {
          Atm_NumConstExprConvertToBitvectorConstExpr(expr,
                                                      Var_VarTypeArrayOrBitvectorReadSize(type));
        } else {
          /* do nothing */
        }
                   
        break;
      }
      
      case Atm_PrimedVar_c:
      case Atm_UnPrimedVar_c:
      {
        Var_Type_t * varType;

        varType = Var_VariableReadType((Var_Variable_t *) Atm_ExprReadLeftChild(expr));
        
        if (Var_TypeIsBV(varType)) {
          expr -> type = ScalarDataTypeConvertToBitvectorDataType(etype);
        }
        break;
      }

      case Atm_Index_c: 
      {
        Var_Type_t *varType, *indexType, *entryType;
        
        varType =
            Var_VariableReadType((Var_Variable_t *)
                                 Atm_ExprReadLeftChild(Atm_ExprReadLeftChild(expr)));

        if (Var_TypeIsArray(varType)) {
          indexType = Var_VarTypeReadIndexType(varType);
          entryType = Var_VarTypeReadEntryType(varType);
        } else {
          indexType = 
              Var_RangeTypeReadFromRange(globalTypeCheckManager -> typeManager,
                                         Var_VarTypeArrayOrBitvectorReadSize(varType) );
          
          entryType = BoolType;
        }

        if (Var_TypeIsBV(entryType)) {
          expr -> type = ScalarDataTypeConvertToBitvectorDataType(etype);
        }

        ExprResolveTypes(Atm_ExprReadLeftChild(expr), varType);
        ExprResolveTypes(Atm_ExprReadRightChild(expr), indexType);
        break;
      }

        
      case Atm_IfThenElseFi_c:
      {
        Atm_Expr_t * rightChild;

        if (Var_TypeIsBV(type)) {
          expr -> type = ScalarDataTypeConvertToBitvectorDataType(etype);
        }

        rightChild = Atm_ExprReadRightChild(expr);
        
        ExprResolveTypes(Atm_ExprReadLeftChild(expr), BoolType);
        ExprResolveTypes(Atm_ExprReadLeftChild(rightChild), type);
        ExprResolveTypes(Atm_ExprReadRightChild(rightChild), type);

        break;
      }
      
      case Atm_Equal_c: 
      case Atm_NotEqual_c:
      {
        AtmSetOfType_t * leftSOT = SOTObtainFromExpr(Atm_ExprReadLeftChild(expr));
        AtmSetOfType_t * rightSOT= SOTObtainFromExpr(Atm_ExprReadRightChild(expr));
        AtmSetOfType_t * tmpSOT = SOTIntersect(3, leftSOT, rightSOT, SOTGet(TC_equalSet));
        Var_Type_t * varType;

        /* tmpSOT should be nonempty */
        /* just pick any one type in tmpSOT */
        /* Note: prefers type of finite domain over types of infinite domain */
        
        varType = PickAType(tmpSOT);

        if (Var_TypeIsBoolean(varType))
          expr -> type = (etype == Atm_Equal_c)? Atm_Equiv_c: Atm_NotEquiv_c;
        
        if (Var_TypeIsBV(varType))
          expr -> type = ScalarDataTypeConvertToBitvectorDataType(etype);
        
        ExprResolveTypes(Atm_ExprReadLeftChild(expr), varType);
        ExprResolveTypes(Atm_ExprReadRightChild(expr), varType);
        
        SOTFree(leftSOT);
        SOTFree(rightSOT);
        SOTFree(tmpSOT);
        
        break;
      }

      case Atm_And_c:
      case Atm_Or_c:
      case Atm_Xor_c:
      case Atm_Implies_c:
      case Atm_Equiv_c:
      case Atm_NotEquiv_c:
      case Atm_Not_c:
      case Atm_Plus_c: 
      case Atm_Minus_c:
      case Atm_UnaryMinus_c:
      {
        if (Var_TypeIsBV(type)) {
          expr -> type = ScalarDataTypeConvertToBitvectorDataType(etype);  
        }
        
        ExprResolveTypes(Atm_ExprReadLeftChild(expr), type);
        ExprResolveTypes(Atm_ExprReadRightChild(expr), type);

        break;
      }

      case Atm_Greater_c:
      case Atm_Less_c:
      case Atm_GreaterEqual_c:
      case Atm_LessEqual_c: 
      {
        AtmSetOfType_t * leftSOT = SOTObtainFromExpr(expr -> leftExpr);
        AtmSetOfType_t * rightSOT= SOTObtainFromExpr(expr -> rightExpr);
        AtmSetOfType_t * tmpSOT;
        Var_Type_t * varType;

        tmpSOT = SOTIntersect(3, leftSOT, rightSOT,
                                 SOTGet(TC_arithmeticSet));

        /* tmpSOT should be nonempty */
        /* just pick any one type in tmpSOT */
        varType = PickAType(tmpSOT);

        if (Var_TypeIsBV(varType)) {
          expr -> type = ScalarDataTypeConvertToBitvectorDataType(etype);
        }
        
        ExprResolveTypes(Atm_ExprReadLeftChild(expr), varType);
        ExprResolveTypes(Atm_ExprReadRightChild(expr), varType);
        
        SOTFree(leftSOT);
        SOTFree(rightSOT);
        SOTFree(tmpSOT);

        break;
      }

      case Atm_RedAnd_c:
      case Atm_RedOr_c:
      case Atm_RedXor_c:
      {
        AtmSetOfType_t * leftSOT = SOTObtainFromExpr(expr -> leftExpr);
        AtmSetOfType_t * tmpSOT;
        Var_Type_t * varType;
        
        tmpSOT = SOTIntersect(2, leftSOT,
                                 SOTGet(TC_allBVSet));

        /* tmpSOT should not be empty */
        /* just pick one */
        varType = PickAType(tmpSOT);

        ExprResolveTypes(Atm_ExprReadLeftChild(expr), varType);
        
        SOTFree(leftSOT);
        SOTFree(tmpSOT);
        break;
      }

      default:
        Main_MochaErrorPrint("Unknown type encountered in ExprResolveTypes()\n");
        assert(0);
        break;
  }

}

/**Function********************************************************************

  Synopsis           [converts an Atm datatype into its bitwise counterpart.]

  Description        []

  SideEffects        [None]

******************************************************************************/
Atm_ExprType
ScalarDataTypeConvertToBitvectorDataType(
  Atm_ExprType exprType) 
{
  switch (exprType) {
      case Atm_Plus_c:
        return Atm_BitwisePlus_c;

      case Atm_And_c:
        return Atm_BitwiseAnd_c;

      case Atm_Equal_c:
        return Atm_BitwiseEqual_c;

      case Atm_NotEqual_c:
        return Atm_BitwiseNotEqual_c;
        
      case Atm_Equiv_c:
        return Atm_BitwiseEquiv_c;

      case Atm_NotEquiv_c:
        return Atm_BitwiseNotEquiv_c;
        
      case Atm_Greater_c:
        return Atm_BitwiseGreater_c;

      case Atm_GreaterEqual_c:
        return Atm_BitwiseGreaterEqual_c;
        
      case Atm_IfThenElseFi_c:
        return Atm_BitwiseIfThenElseFi_c;

      case Atm_Implies_c:
        return Atm_BitwiseImplies_c;
        
      case Atm_Index_c:
        return Atm_BitwiseIndex_c;
        
      case Atm_Less_c:
        return Atm_BitwiseLess_c;
        
      case Atm_LessEqual_c:
        return Atm_BitwiseLessEqual_c;

      case Atm_Minus_c: 
        return Atm_BitwiseMinus_c; 
        
      case Atm_Not_c:    
        return Atm_BitwiseNot_c;    

      case Atm_NumConst_c:   
        return Atm_BitwiseNumConst_c;                

      case Atm_Or_c:
        return Atm_BitwiseOr_c;        

      case Atm_Xor_c:
        return Atm_BitwiseXor_c;        
        
      case Atm_PrimedVar_c:
        return Atm_BitwisePrimedVar_c;
        
      case Atm_UnPrimedVar_c:
        return Atm_BitwiseUnPrimedVar_c;
        
      /* all other cases, including
         Atm_BoolConst_c,
         Atm_EnumConst_c,   
         Atm_EventQuery_c,
         Atm_RangeConst_c,
         Atm_UnaryMinus_c,
         Atm_Nondet_c, 
         Atm_TimerUpperBound_c, 
         Atm_RedAnd_c, and
         Atm_RedOr_c, and
         Atm_RedXor_c, and
      */
        
      case Atm_BoolConst_c:
      case Atm_EnumConst_c:
      case Atm_EventQuery_c:
      case Atm_RangeConst_c:
      case Atm_UnaryMinus_c:
      case Atm_Nondet_c:
      case Atm_TimerUpperBound_c:
      case Atm_RedAnd_c:
      case Atm_RedOr_c:
      case Atm_RedXor_c:
        Main_MochaErrorPrint("Warning: Type do not have corresponding bitvector type.\n");
        return exprType;
        
      default:
        fprintf(stderr, "unknown type in ScalarDataTypeConvertToBitvectorDataType()\n");
        exit (1);
  }

}

/**Function********************************************************************

  Synopsis           [Just pick a type.]

  Description        [prefers types of finite domain over types of infinite
                      domain.  As a slight optimization, prefers boolType over
                      all other types.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static Var_Type_t *
PickAType(
  AtmSetOfType_t * sot
  ) 
{
  array_t * typeArray = TypeArrayObtainFromSOT ( sot );
  Var_Type_t * type, *returnType = NIL(Var_Type_t);
  int i;

  arrayForEachItem(Var_Type_t *, typeArray, i, type) {

    if (Var_TypeIsFinite(type) && !(returnType && Var_TypeIsBoolean(returnType)))
      returnType = type;
  }
  
  return returnType;

}


/**Function********************************************************************

  Synopsis           [Returns a simplified expression.]

  Description        [This function returns a simplified version of expr. All
  simplifications are made with respect to numerical constants in the expression.
  If an arithmetic operation is performed on two numbers, then in the simplified
  expression, the sub-tree is replaced with the result, e.g., the expression
  (5+2)-3 is replaced by the expression 4. Also, expressions like 2=3 is
  simplified to FALSE.] 

  SideEffects        [None]

  SeeAlso            [Atm_ExprObtainDataType, Atm_ExprCreateTypedExpression]

******************************************************************************/
static void
ExprSimplify2(
  Atm_Expr_t ** expr)
{
   Atm_ExprType exprType;
   Atm_Expr_t *leftChild, * rightChild;
   long lvalue, rvalue, value;
   
   if ((*expr) == NIL(Atm_Expr_t))
     return;

   exprType = Atm_ExprReadType(*expr);
   
   switch (exprType) {
       case Atm_TimerUpperBound_c:
       case Atm_Nondet_c:
       case Atm_PrimedVar_c:
       case Atm_UnPrimedVar_c:
       case Atm_NumConst_c:
       case Atm_RangeConst_c:
       case Atm_EnumConst_c:
       case Atm_BoolConst_c:
         break;
         
       case Atm_EventQuery_c:
       case Atm_Index_c:
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         ExprSimplify2(AtmExprReadRightChildAddress(*expr));
         break;
         
       case Atm_And_c:
       case Atm_Or_c:
       case Atm_Xor_c:
       case Atm_Implies_c:
       case Atm_Equiv_c:
       case Atm_NotEquiv_c:
       {
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         ExprSimplify2(AtmExprReadRightChildAddress(*expr));

         leftChild  = Atm_ExprReadLeftChild(*expr);
         rightChild = Atm_ExprReadRightChild(*expr);
         
         if ((Atm_ExprReadType(leftChild)  == Atm_BoolConst_c) &&
             (Atm_ExprReadType(rightChild) == Atm_BoolConst_c)) {
           
           lvalue = (long) Atm_ExprReadLeftChild(leftChild);
           rvalue = (long) Atm_ExprReadLeftChild(rightChild);
         
           if (exprType == Atm_And_c) {
             value = lvalue && rvalue;
           } else if (exprType == Atm_Or_c) {
             value = lvalue || rvalue;
           } else if (exprType == Atm_Xor_c) {
             value = lvalue ^ rvalue;
           } else if (exprType == Atm_Implies_c) {
             value = (!lvalue) || rvalue;
           } else if (exprType == Atm_Equiv_c) {
             value = (lvalue == rvalue);
           } else if (exprType == Atm_Not_c) {
             value = (lvalue != rvalue);
           } else {
             fprintf(stderr, "Error: unexpected type.\n");
             assert(0);
           }
           
           Atm_ExprFreeExpression(*expr);
           (*expr) = Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) value, NIL(Atm_Expr_t));
         }
         
         break;
       }
       
       case Atm_Not_c:
       {
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         
         leftChild  = Atm_ExprReadLeftChild(*expr);
         
         if ((Atm_ExprReadType(leftChild)  == Atm_BoolConst_c)) {
           lvalue = (long) Atm_ExprReadLeftChild(leftChild);

           Atm_ExprFreeExpression(*expr);
           *expr = Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) (!value),
                                 NIL(Atm_Expr_t));
         }

         break;
       }

       case Atm_RedAnd_c:
       {
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         leftChild  = Atm_ExprReadLeftChild(*expr);
         
         if (Atm_ExprReadType(leftChild) == Atm_BitwiseNumConst_c) {
           char * lBitArray = (char *) Atm_ExprReadLeftChild(leftChild);
           long numBits = (long) Atm_ExprReadRightChild(leftChild);
           int i, flag = TRUE;

           for (i=0; i<numBits; i++) {
             flag = flag && (lBitArray[i] == 1);
           }
           
           Atm_ExprFreeExpression(*expr);
           *expr = Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) (long) flag,
                                 NIL(Atm_Expr_t));
         }
         
         break;
       }

       case Atm_RedOr_c:
       {
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         leftChild  = Atm_ExprReadLeftChild(*expr);

         if (Atm_ExprReadType(leftChild) == Atm_BitwiseNumConst_c) {
           char * lBitArray = (char *) Atm_ExprReadLeftChild(leftChild);
           long numBits = (long) Atm_ExprReadRightChild(leftChild);
           int v = BitArrayToNum(lBitArray, numBits);
           
           Atm_ExprFreeExpression(*expr);
           if (v != 0)
             *expr = Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) (long) 1,
                                   NIL(Atm_Expr_t));
           else
             *expr = Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) (long) 0,
                                   NIL(Atm_Expr_t));
         }

         break;
       }


       case Atm_RedXor_c:
       {
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         leftChild  = Atm_ExprReadLeftChild(*expr);
         
         if (Atm_ExprReadType(leftChild) == Atm_BitwiseNumConst_c) {
           char * lBitArray = (char *) Atm_ExprReadLeftChild(leftChild);
           long numBits = (long) Atm_ExprReadRightChild(leftChild);
           int i, flag = FALSE;

           for (i=0; i<numBits; i++) {
             flag = flag ^ (lBitArray[i] == 1);
           }
           
           Atm_ExprFreeExpression(*expr);
           *expr = Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) (long) flag,
                                 NIL(Atm_Expr_t));
         }
         
         break;
       }

       case Atm_Plus_c:
       case Atm_Minus_c:
       {
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         ExprSimplify2(AtmExprReadRightChildAddress(*expr));

         leftChild= Atm_ExprReadLeftChild(*expr);
         rightChild = Atm_ExprReadRightChild(*expr);
         
         if ((Atm_ExprReadType(leftChild)  == Atm_NumConst_c) &&
             (Atm_ExprReadType(rightChild) == Atm_NumConst_c)) {
           lvalue = (long) Atm_ExprReadLeftChild(leftChild);
           rvalue = (long) Atm_ExprReadLeftChild(rightChild);
         
           value = (exprType == Atm_Plus_c)? (lvalue + rvalue): (lvalue - rvalue);
           
           Atm_ExprFreeExpression(*expr);
           *expr = Atm_ExprAlloc(Atm_NumConst_c, (Atm_Expr_t *) value, NIL(Atm_Expr_t));
         }

         break;
       }
       
       case Atm_UnaryMinus_c:
       {
         printf(" I am here\n");
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
            
         leftChild  = Atm_ExprReadLeftChild(*expr);
            
         if (Atm_ExprReadType(leftChild)  == Atm_NumConst_c) {
           lvalue = (long) Atm_ExprReadLeftChild(leftChild);
           
           Atm_ExprFreeExpression(*expr);
           *expr = Atm_ExprAlloc(Atm_NumConst_c, (Atm_Expr_t *) (-1 * value), NIL(Atm_Expr_t));
         }

         break;
       }

       case Atm_Equal_c:
       case Atm_NotEqual_c:
       case Atm_Greater_c:
       case Atm_GreaterEqual_c:
       case Atm_Less_c:
       case Atm_LessEqual_c:
       {
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         ExprSimplify2(AtmExprReadRightChildAddress(*expr));
         

         leftChild  = Atm_ExprReadLeftChild(*expr);
         rightChild = Atm_ExprReadRightChild(*expr);

         if ((Atm_ExprReadType(leftChild)  == Atm_NumConst_c) &&
             (Atm_ExprReadType(rightChild) == Atm_NumConst_c)) {

           lvalue = (long) Atm_ExprReadLeftChild(leftChild);
           rvalue = (long) Atm_ExprReadLeftChild(rightChild);
           
           if (exprType == Atm_Equal_c)
             value = (lvalue == rvalue) ? 1 : 0;
           else if (exprType == Atm_NotEqual_c)
             value = (lvalue != rvalue) ? 1 : 0;
           else if (exprType == Atm_Greater_c)
             value = (lvalue > rvalue) ? 1 : 0;
           else if (exprType == Atm_GreaterEqual_c)
             value = (lvalue >= rvalue) ? 1 : 0;
           else if (exprType == Atm_Less_c)
             value = (lvalue < rvalue) ? 1 : 0;
           else
             value = (lvalue <= rvalue) ? 1 : 0;
           
           Atm_ExprFreeExpression(*expr);           
           *expr = Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) value, NIL(Atm_Expr_t));
         }
         break;
       }
       
       case Atm_IfThenElseFi_c:
       {
         Atm_Expr_t *rightChild;

         rightChild = Atm_ExprReadRightChild(*expr);
         
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         ExprSimplify2(AtmExprReadLeftChildAddress(rightChild));             
         ExprSimplify2(AtmExprReadRightChildAddress(rightChild));

         break;
       }  


       case Atm_BitwisePrimedVar_c:
       case Atm_BitwiseUnPrimedVar_c:
       case Atm_BitwiseNumConst_c:
         break;
       
       case Atm_BitwiseAnd_c:
       case Atm_BitwiseOr_c:
       case Atm_BitwiseXor_c:
       case Atm_BitwiseImplies_c:
       case Atm_BitwiseEquiv_c:
       case Atm_BitwiseNotEquiv_c:
       case Atm_BitwiseMinus_c: 
       case Atm_BitwisePlus_c:
       {
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         ExprSimplify2(AtmExprReadRightChildAddress(*expr));
         
         leftChild  = Atm_ExprReadLeftChild(*expr);
         rightChild = Atm_ExprReadRightChild(*expr);
         
         if ((Atm_ExprReadType(leftChild)  == Atm_BitwiseNumConst_c) &&
             (Atm_ExprReadType(rightChild) == Atm_BitwiseNumConst_c)) {
           char * lBitArray = (char *) Atm_ExprReadLeftChild(leftChild);
           char * rBitArray = (char *) Atm_ExprReadLeftChild(rightChild);
           int lv, rv;
           int numBits, v;

           /* should be the same length */
           assert(Atm_ExprReadRightChild(leftChild) ==
                  Atm_ExprReadRightChild(rightChild));

           numBits = (int) (long) Atm_ExprReadRightChild(leftChild);
           lv = BitArrayToNum(lBitArray, numBits);
           rv = BitArrayToNum(rBitArray, numBits);

           switch (exprType) {
               case Atm_BitwiseAnd_c:
                 v = lv & rv;
                 break;
                 
               case Atm_BitwiseOr_c:
                 v = lv | rv;
                 break;
                 
               case Atm_BitwiseImplies_c:
                 v = (~lv) | rv;
                 break;
                 
               case Atm_BitwiseEquiv_c:
                 v = ~(lv ^ rv); /* ~xor */
                 break;
                 
               case Atm_BitwiseNotEquiv_c:
               case Atm_BitwiseXor_c:
                 v = lv ^ rv; /* xor */
                 break;

               case Atm_BitwiseMinus_c: 
                 v = lv - rv;
                 break;
                 
               case Atm_BitwisePlus_c:
                 v = lv + rv;
                 break;
                 
               default:
                 fprintf(stderr, "Error: unexpected type.\n");
                 assert(0);
                 break;
           }
           
           Atm_ExprFreeExpression(*expr);
           *expr = Atm_ExprAlloc(Atm_NumConst_c,
                                 (Atm_Expr_t *) (long) v,
                                 NIL(Atm_Expr_t));
           Atm_NumConstExprConvertToBitvectorConstExpr(*expr, numBits);
         }
         
         break;
       }

       case Atm_BitwiseNot_c:
       {

         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         
         leftChild  = Atm_ExprReadLeftChild(*expr);
         
         if ((Atm_ExprReadType(leftChild)  == Atm_BitwiseNumConst_c)) {
           char * lBitArray = (char *) Atm_ExprReadLeftChild(leftChild);
           int lv;
           int numBits, v;

           numBits = (int) (long) Atm_ExprReadRightChild(leftChild);
           lv = BitArrayToNum(lBitArray, numBits);
           v  = ~(lv);
           
           Atm_ExprFreeExpression(*expr);
           *expr = Atm_ExprAlloc(Atm_NumConst_c,
                                 (Atm_Expr_t *) (long) v,
                                 NIL(Atm_Expr_t));
           Atm_NumConstExprConvertToBitvectorConstExpr(*expr, numBits);
         }
         
         break;
       }

       case Atm_BitwiseEqual_c:
       case Atm_BitwiseNotEqual_c:
       case Atm_BitwiseGreater_c:
       case Atm_BitwiseGreaterEqual_c:
       case Atm_BitwiseLess_c:
       case Atm_BitwiseLessEqual_c:
       {
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         ExprSimplify2(AtmExprReadRightChildAddress(*expr));

         leftChild  = Atm_ExprReadLeftChild(*expr);
         rightChild = Atm_ExprReadRightChild(*expr);

         if ((Atm_ExprReadType(leftChild)  == Atm_BitwiseNumConst_c) &&
             (Atm_ExprReadType(rightChild) == Atm_BitwiseNumConst_c)) {
           char * lBitArray = (char *) Atm_ExprReadLeftChild(leftChild);
           char * rBitArray = (char *) Atm_ExprReadLeftChild(rightChild);
           int lv, rv;
           int numBits, flag;

           /* should be the same length */
           assert(Atm_ExprReadRightChild(leftChild) ==
                  Atm_ExprReadRightChild(rightChild));

           numBits = (int) (long) Atm_ExprReadRightChild(leftChild);
           lv = BitArrayToNum(lBitArray, numBits);
           rv = BitArrayToNum(rBitArray, numBits);

           switch (exprType) {
               case Atm_BitwiseEqual_c:
                 flag = (lv = rv);
                 break;
                                  
               case Atm_BitwiseNotEqual_c:
                 flag = (lv != rv);
                 break;
                 
               case Atm_BitwiseGreater_c:
                 flag = lv > rv;
                 break;

               case Atm_BitwiseGreaterEqual_c:
                 flag = lv >= rv;
                 break;
                 
               case Atm_BitwiseLess_c:
                 flag = lv < rv;
                 break;

               case Atm_BitwiseLessEqual_c:
                 flag = lv <= rv;
                 break;

               default:
                 fprintf(stderr, "Error: unexpected type.\n");
                 assert(0);
                 break;
           }
           
           Atm_ExprFreeExpression(*expr);           
           *expr = Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) value, NIL(Atm_Expr_t));
         }
         break;
       }

         
       case Atm_BitwiseIfThenElseFi_c:
       {
         Atm_Expr_t *rightExpr = Atm_ExprReadRightChild(*expr);
         
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         ExprSimplify2(AtmExprReadLeftChildAddress(rightExpr));
         ExprSimplify2(AtmExprReadRightChildAddress(rightExpr));
         break;
       }  

       case Atm_BitwiseIndex_c:                            /* a[expr] */
         ExprSimplify2(AtmExprReadLeftChildAddress(*expr));
         ExprSimplify2(AtmExprReadRightChildAddress(*expr));
         break;
         
         
       default:
         printf("here2, %d\n", exprType);
         break;

   }


}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
BitArrayToNum(
  char * bitArray,
  int numBits) 
{
  int i;
  int v = 0;

  for (i=numBits - 1; i >=0 ; i--) {
    v = v << 1 + (bitArray[i] == 0)? 0 : 1;
  }
  
  return v;
  
}

