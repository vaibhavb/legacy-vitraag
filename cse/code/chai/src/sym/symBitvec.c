/**CFile*****************************************************************

  FileName    [symBitvec.c]

  PackageName [sym]

  Synopsis    [provides utilities to build transition relation for bit vector operations]

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

  Revision    [$Id: symBitvec.c,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

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

  Synopsis           [build MDD for bit vector assignment]

  Description        [This routine builds an MDD for a bit vector assignment]

  SideEffects        [none]

  SeeAlso            [SymBitVectorBuildBooleanExpression]

******************************************************************************/
mdd_t *
SymBitVectorBuildAssignmentMdd(
  Sym_Info_t *symInfo,
  Var_Variable_t *var,
  Atm_Expr_t *expr)
{
  Var_Type_t *varType  = Var_VariableReadType (var);
  array_t *lhsArray, *rhsArray, *xnorArray;
  mdd_t *andMdd;
  
  if(!Var_TypeIsBV(varType)) {
    Main_MochaErrorPrint("Error: SymBuildBitVectorAssignmentMdd:LHS is not a bitvector!\n");
    exit(1);
  }

  lhsArray  = SymBitVectorBuildMddArrayForVar(symInfo, Atm_BitwisePrimedVar_c, var);
  rhsArray  = SymBitVectorBuildMddArray(symInfo, expr);
  xnorArray = SymMddArrayXnorMddArray(symInfo, lhsArray, rhsArray);
  andMdd    = SymMddArrayAndComponents(symInfo, xnorArray);
  SymMddArrayFree(lhsArray);
  SymMddArrayFree(rhsArray);
  SymMddArrayFree(xnorArray);
  
  return(andMdd);
}


/**Function********************************************************************

  Synopsis           [build MDD for a boolean expression of bit vectors]

  Description        [Given an expression of the form e1 <relop> e2, where
                      e1 and e2 are bit-vector expressions and
		      relop is a comparision operator, this
		      routne builds an MDD for the comparison]

  SideEffects        [none]

  SeeAlso            [SymBitVetorBuildAssignmentMdd]

******************************************************************************/
mdd_t *
SymBitVectorBuildBooleanExpressionMdd(
  Sym_Info_t *symInfo,
  Atm_Expr_t *expr)
{
  mdd_t *result;
  int i, length;
  Atm_ExprType eType = Atm_ExprReadType(expr);
  Atm_Expr_t * lexpr = Atm_ExprReadLeftChild(expr);
  Atm_Expr_t * rexpr = Atm_ExprReadRightChild(expr);
  
  array_t * lhsArray = SymBitVectorBuildMddArray(symInfo,lexpr);
  array_t * rhsArray = SymBitVectorBuildMddArray(symInfo,rexpr);
  
  
  if((lhsArray != NIL(array_t)) &&
     (rhsArray != NIL(array_t)) &&
     (array_n(lhsArray) != array_n(rhsArray))){

    Main_MochaErrorPrint("SymBuildBitVectorBooleanExpression: vectors have"
                         "different sizes: %d and %d\n", array_n(lhsArray),
                         array_n(rhsArray));
    exit(1);
  }
  
  switch(eType){
      case Atm_BitwiseEqual_c:
        result = SymMddArrayCompareEqual(symInfo, lhsArray, rhsArray);
        break;

      case Atm_BitwiseNotEqual_c:
        result = SymMddArrayCompareEqual(symInfo, lhsArray, rhsArray);
        break;

      case Atm_BitwiseLess_c:
        result = SymMddArrayCompareLesser(symInfo, lhsArray, rhsArray);
        break;
        
      case Atm_BitwiseLessEqual_c:
        result = SymMddArrayCompareLesserEqual(symInfo, lhsArray, rhsArray);
        break;
        
      case Atm_BitwiseGreater_c:
        result = SymMddArrayCompareGreater(symInfo, lhsArray, rhsArray);
        break;
        
      case Atm_BitwiseGreaterEqual_c: 
        result = SymMddArrayCompareGreaterEqual(symInfo, lhsArray, rhsArray);
        break;
       
      case Atm_RedAnd_c:
        result = SymMddArrayRedAnd(symInfo, lhsArray);
        break;

      case Atm_RedOr_c:
        result = SymMddArrayRedOr(symInfo, lhsArray);
        break;

      case Atm_RedXor_c:
        result = SymMddArrayRedXor(symInfo, lhsArray);
        break;

        
      default:
        Main_MochaErrorPrint("Bitvector comparison: unsupported operator\n");
        exit(1);
  }

  /*
  SymMddArrayFree(lhsArray);
  SymMddArrayFree(rhsArray);
  */
  
  return(result);
}
  
/**Function********************************************************************

  Synopsis           [build MDD array for bit vector expression]

  Description        [This routine builds an MDD array for a bit vector
  expression. Valid bit vector expressions are
  0. c, a bit vector constant
  1. v,a bit vector variable
  2. a[index], a is an array of bitvectors and index is a suitable index expression.]
  3. ~e, where e is bit vector expression
  4. (e1+e2), (e1-e2), (e1 & e2), (e1 | e2), where e1 and e2 are bit vector expressions

  SideEffects        [none]

  SeeAlso            [SymBitVectorBuildBooleanExpressionMdd]

******************************************************************************/
array_t *
SymBitVectorBuildMddArray(
  Sym_Info_t *symInfo,
  Atm_Expr_t *expr
  )
{
  array_t *resultArray;
  int bLen;
  Atm_ExprType exprType;
  Var_Variable_t *opa;
  Var_Type_t *opaType;
  Var_DataType opaDataType;
  int i;

  if (expr == NIL(Atm_Expr_t))
    return NIL(array_t);
  
  exprType = Atm_ExprReadType(expr);  
  
  switch(exprType){

      case Atm_BitwiseNumConst_c:
      {
        char *bitArray;
        int numBits;
        int i;
        resultArray = array_alloc(mdd_t *, 0);
        numBits = (int)(long)Atm_ExprReadRightChild(expr);
        bitArray = (char *)Atm_ExprReadLeftChild(expr);

        for ( i = 0; i < numBits; i++){
          mdd_t *bitMdd = bitArray[i] ? mdd_one(symInfo->manager) :
              mdd_zero(symInfo->manager);
          array_insert_last( mdd_t *, resultArray, bitMdd);
        }
        break;
      }
      
      case Atm_BitwisePrimedVar_c:
      case Atm_BitwiseUnPrimedVar_c:
      {
        Var_Variable_t *opa = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        Var_DataType opaDataType = Var_VariableReadDataType(opa);
        
        if( !Var_TypeIsBV(Var_VariableReadType(opa))) {
          Main_MochaErrorPrint("Error:SymBitVectorBuildMddArray:LHS is not a bitvector!\n");
          exit(1);
        }
        resultArray = SymBitVectorBuildMddArrayForVar(symInfo, exprType, opa);
        break;
      }

      case Atm_BitwiseIndex_c:
      {
        /* the array has to be an array of bitvectors */
        int i;
        Atm_Expr_t *lexpr = Atm_ExprReadLeftChild(expr);
        Var_Variable_t *aVar   = (Var_Variable_t *)Atm_ExprReadLeftChild(lexpr);
        Atm_ExprType aVarType = Atm_ExprReadType(lexpr);
        Atm_Expr_t *indexExpr =  Atm_ExprReadRightChild(expr);
        array_t *temp1, *temp2, *temp3;
        Var_Variable_t *elemVar;
        
        if(SymIsNumEnumRangeConstant(indexExpr)){
          elemVar  = SymGetArrayElementVar(symInfo, aVar, SymGetConstant(indexExpr));
          resultArray = SymBitVectorBuildMddArrayForVar(symInfo, aVarType, elemVar);
        }
        else {
          int nVals = SymGetArrayNvals(aVar);
          mdd_t *exprMdd;
          int bitvSize;

          elemVar = SymGetArrayElementVar( symInfo, aVar, 0);
          bitvSize   = Var_VarTypeArrayOrBitvectorReadSize( Var_VariableReadType(elemVar));
          resultArray = SymMddArrayObtainConstant(symInfo, bitvSize , 0);
          for ( i = 0; i < nVals; i++){
            exprMdd = SymExprBuildExprConstMdd(symInfo, indexExpr, i, nVals);
            elemVar = SymGetArrayElementVar( symInfo, aVar, i);
            temp1  = SymBitVectorBuildMddArrayForVar(symInfo, aVarType,
                                                         elemVar);
            temp2  = SymMddArrayAndMdd(symInfo, temp1, exprMdd);
            temp3  = SymMddArrayOrMddArray(symInfo, resultArray, temp2);

            SymMddArrayFree(temp1);
            SymMddArrayFree(temp2);
            SymMddArrayFree(resultArray);
            resultArray = temp3;
          }
        }
        break;
      }
      

      case Atm_BitwiseNot_c:
      {
        Atm_Expr_t *lexp;
        array_t *temp;
        lexp = Atm_ExprReadLeftChild(expr);
        temp = SymBitVectorBuildMddArray(symInfo, lexp);
        resultArray = SymMddArrayNot(symInfo, temp);
        SymMddArrayFree(temp);
        break;
      }
      
      case Atm_BitwiseImplies_c:
      case Atm_BitwiseEquiv_c:
      case Atm_BitwiseNotEquiv_c:
      case Atm_BitwiseAnd_c:
      case Atm_BitwiseOr_c:
      case Atm_BitwiseXor_c:
      case Atm_BitwisePlus_c:
      case Atm_BitwiseMinus_c:
      {
        Atm_Expr_t *lexp, *rexp;
        array_t *temp1, *temp2;
        lexp = Atm_ExprReadLeftChild(expr);
        rexp = Atm_ExprReadRightChild(expr);
        temp1 = SymBitVectorBuildMddArray(symInfo, lexp);
        temp2 = SymBitVectorBuildMddArray(symInfo, rexp);

        switch(exprType){
            case Atm_BitwiseImplies_c:
	    {
              resultArray = SymMddArrayImpliesMddArray(symInfo, temp1, temp2);
              break;	       
	    }
	    
            case Atm_BitwiseEquiv_c:
	    {
              resultArray = SymMddArrayXnorMddArray(symInfo, temp1, temp2);
              break;
            }

            case Atm_BitwiseNotEquiv_c:
            case Atm_BitwiseXor_c:
	    {
              resultArray = SymMddArrayXorMddArray(symInfo, temp1, temp2);
              break;
            }

            case Atm_BitwiseAnd_c:
            {
              resultArray = SymMddArrayAndMddArray(symInfo, temp1, temp2);
              break;
            }

            case Atm_BitwiseOr_c:
            {
              resultArray = SymMddArrayOrMddArray(symInfo, temp1, temp2);
              break;
            }

            case Atm_BitwisePlus_c:
            {
              resultArray = SymMddArrayPlusMddArray(symInfo, temp1, temp2);
              break;
            }

            case Atm_BitwiseMinus_c:
            {
              resultArray = SymMddArrayMinusMddArray(symInfo, temp1, temp2);
              break;
            }
            
            default:
              Main_MochaPrint("Error: SymBitVectorBuildMddArray: bad expression type\n");
              exit(1);
        }

        SymMddArrayFree(temp1);
        SymMddArrayFree(temp2);
        break;
      }

      case Atm_BitwiseIfThenElseFi_c:
      {
        mdd_t *ifExpr = Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        mdd_t *elseExpr = mdd_not(ifExpr);
        array_t *ifArray = SymBitVectorBuildMddArray(symInfo,
                                                       Atm_ExprReadLeftChild(Atm_ExprReadRightChild(expr)));
        array_t *elseArray = SymBitVectorBuildMddArray(symInfo,
                                                       Atm_ExprReadRightChild(Atm_ExprReadRightChild(expr)));
        array_t *temp1 = SymMddArrayAndMdd(symInfo, ifArray, ifExpr);
        array_t *temp2 = SymMddArrayAndMdd(symInfo, elseArray, elseExpr);
        resultArray = SymMddArrayOrMddArray(symInfo, temp1, temp2);

        mdd_free(ifExpr);
        mdd_free(elseExpr);
        SymMddArrayFree(ifArray);
        SymMddArrayFree(elseArray);
        SymMddArrayFree(temp1);
        SymMddArrayFree(temp2);
        break;
      }
      
      default:      
        Main_MochaPrint("Error: SymBitVectorBuildMddArray: unsupported expression type\n");
        exit(1);
  }

  return(resultArray);
}

/**Function********************************************************************

  Synopsis           [return and mdd array for a bitvector variable]

  Description        [Given a bitvector variable and its variable type (primed
  or unprimed), return an array of MDDs corresponding to its variables]

  SideEffects        [none]

  SeeAlso            [SymBitVectorBuildMddArray]

******************************************************************************/
array_t *
SymBitVectorBuildMddArrayForVar(
  Sym_Info_t *symInfo,
  Atm_ExprType  varType,
  Var_Variable_t *var)
{
  int length = Var_VarTypeArrayOrBitvectorReadSize(Var_VariableReadType(var));
  array_t *resultArray = array_alloc(mdd_t *, 0);
  mdd_t *curBit;
  int   varid;
  int  i;
  Var_Type_t * type;
  
  type = Var_VariableReadType(var);
  length = Var_VarTypeArrayOrBitvectorReadSize( Var_VariableReadType(var));

  if (Var_TypeIsBitvector(type)) {
    for (i = 0; i < length; i++){
      varid = SymGetArrayId(symInfo, var, varType, i);
      curBit = SymGetMddLiteral(symInfo->manager, varid, 1);
      array_insert_last(mdd_t *, resultArray, curBit);
    }
  } else {
    for (i = 0; i < length; i++){
      varid = SymGetArrayId(symInfo, var, varType, length - 1 - i);
      curBit = SymGetMddLiteral(symInfo->manager, varid, 1);
      array_insert_last(mdd_t *, resultArray, curBit);
    }
    
  }
  
  return(resultArray);
}




