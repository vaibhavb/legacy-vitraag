/**CFile***********************************************************************

  FileName    [symExprMdd.c]

  PackageName [sym]

  Synopsis    [routines to build mdd for boolean expressions]

  Description [This file containes routies that are used to build MDDs
               corresponding to boolean expressions. Note that the
	       boolean expressions can have multi valued variables
	       e.g: (x < 5) & (y > x + 5) ]

  SeeAlso     [SymAssignMdd.c]

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

  Synopsis           [build MDD for an expression]

  Description        [This routined builds an MDD for an expression. Note that
                      the expression *must* evaluate to a boolean value in
                      order to be able to build an MDD. Otherwise
                      an error message is printed, and the routine aborts ]

  SideEffects        [none]

  SeeAlso            [optional]

******************************************************************************/
mdd_t *
Sym_ExprBuildMdd(
  Sym_Info_t *symInfo,
  Atm_Expr_t *expr
  )
{
  mdd_t *result = (mdd_t*) NULL;
  mdd_t *temp1, *temp2, *temp3;
  mdd_t *exprMdd, *litMdd;
  mdd_manager *manager = symInfo->manager;
  int id, arrayId;
  Var_Variable_t *var, *aVar;
  Atm_ExprType aVarType;
  int nVals;
  Atm_Expr_t *indexExpr;
  
  Atm_ExprType exprType = Atm_ExprReadType(expr);
  switch(exprType){
      case Atm_IfThenElseFi_c:
      {
        temp1 = Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =
            Sym_ExprBuildMdd(symInfo,
			     Atm_ExprReadLeftChild(Atm_ExprReadRightChild(expr)));
        temp3 =
            Sym_ExprBuildMdd(symInfo,
			     Atm_ExprReadRightChild(Atm_ExprReadRightChild(expr)));
        result = mdd_ite( temp1, temp2, temp3, 1, 1, 1);
        mdd_free(temp1);
        mdd_free(temp2);
        mdd_free(temp3);
        break;
      } 
      
      case Atm_UnPrimedVar_c:
      {

        var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        
        if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, var, &id )){
          Main_MochaErrorPrint( "Error: Sym_ExprBuildMdd, cant find mdd id for var\n");
          exit(1);
        }

        /* we have reached a variable as a leaf. Should be boolean!!  */
        if(!(Var_VariableIsBoolean(var) || Var_VariableIsEvent(var))){
          Main_MochaErrorPrint(
            "Error: Sym_ExprBuildMdd, encountered non-boolean leaf\n");
          exit(1);
        }
        
        result = SymGetMddLiteral(symInfo->manager, id, 1);
        break;
      }
      
      case Atm_PrimedVar_c:
      {
        var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        
        if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &id )){
          Main_MochaErrorPrint( "Error: Sym_ExprBuildMdd, cant find mddid for var\n");
          exit(1);
          
        }
        /* we have reached a variable as a leaf. Should be boolean!!  */
        if(!(Var_VariableIsBoolean(var) || Var_VariableIsEvent(var))){
          Main_MochaErrorPrint(
            "Error: Sym_ExprBuildMdd, encountered non-boolean leaf\n");
          exit(1);
        }
        
        result = SymGetMddLiteral(symInfo->manager, id, 1);
        break;
      }
      
      
      case Atm_BoolConst_c:
      {
        if (Atm_ExprReadLeftChild(expr)) 
          result = mdd_one(manager);
        else
          result = mdd_zero(manager);
        break;
      }

      case Atm_Or_c:
      {
        temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadRightChild(expr));
        result = mdd_or(temp1, temp2, 1, 1);
        mdd_free(temp1);
        mdd_free(temp2);
        break;
      }
      
      case Atm_And_c:
      {
        temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadRightChild(expr));
        result = mdd_and(temp1, temp2, 1, 1);
        mdd_free(temp1);
        mdd_free(temp2);
        break;
      }
      
      case Atm_Implies_c:
      {
	temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadRightChild(expr));
        result = mdd_or(temp1, temp2, 0, 1);
        mdd_free(temp1);
        mdd_free(temp2);
        break;
      }
      
      case Atm_Equiv_c:
      {
        temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadRightChild(expr));
        result = mdd_xnor(temp1, temp2);
        mdd_free(temp1);
        mdd_free(temp2);
        break;
      }

      case Atm_NotEquiv_c:
      case Atm_Xor_c:
      {
        temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadRightChild(expr));
        result = mdd_xor(temp1, temp2);
        mdd_free(temp1);
        mdd_free(temp2);
        break;
      }

      case Atm_Not_c:
      {
        temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        result = mdd_not(temp1);
        mdd_free(temp1);
        break;
      }
      

      case Atm_Equal_c:
      case Atm_NotEqual_c:
      case Atm_Greater_c:
      case Atm_GreaterEqual_c:
      case Atm_Less_c:
      case Atm_LessEqual_c:
      {
        result = SymMVBuildBooleanExpressionMdd(symInfo, expr);
        break;
      }

      case Atm_RedAnd_c:
      case Atm_RedOr_c:
      case Atm_RedXor_c:
      case Atm_BitwiseEqual_c:
      case Atm_BitwiseNotEqual_c:
      case Atm_BitwiseGreater_c:
      case Atm_BitwiseGreaterEqual_c:
      case Atm_BitwiseLess_c:
      case Atm_BitwiseLessEqual_c:
      {
        result = SymBitVectorBuildBooleanExpressionMdd(symInfo, expr);
        break;
      }
      
      case Atm_EventQuery_c:
      {
	Var_Variable_t *var = (Var_Variable_t *)
            Atm_ExprReadLeftChild(Atm_ExprReadLeftChild(expr));

        if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &id )){
          Main_MochaErrorPrint( "Error: Sym_ExprBuildMdd, cant find mdd id for var\n");
          exit(1);
        }         

        result = SymGetMddLiteral(symInfo->manager, id, 1);
        break;
      }

      case Atm_Index_c:
      {
        int i;
        Atm_Expr_t *lexpr = Atm_ExprReadLeftChild(expr);
        aVar   = (Var_Variable_t *)Atm_ExprReadLeftChild(lexpr);
        aVarType = Atm_ExprReadType(lexpr);
        indexExpr =  Atm_ExprReadRightChild(expr);


        if(SymIsNumEnumRangeConstant(indexExpr)){
          arrayId = SymGetArrayId(symInfo, aVar, aVarType,  SymGetConstant(indexExpr));
          result = SymGetMddLiteral(symInfo->manager, arrayId, 1);
        }
        else {
          nVals = SymGetArrayNvals(aVar);
          result = mdd_zero(symInfo->manager);
          for ( i = 0; i < nVals; i++){
            exprMdd = SymExprBuildExprConstMdd(symInfo, indexExpr, i, nVals);
            arrayId = SymGetArrayId( symInfo, aVar, aVarType, i);

            litMdd =  SymGetMddLiteral(symInfo->manager, arrayId, 1);
            temp1  = mdd_and(exprMdd, litMdd, 1, 1);
            temp2  = mdd_or(temp1, result, 1, 1);
            mdd_free(result);
            mdd_free(temp1);
            mdd_free(exprMdd);
            mdd_free(litMdd);
            result = temp2;
          }
        }
        break;
      }

      case Atm_UnaryMinus_c:
      case Atm_Plus_c:
      case Atm_Minus_c:
      case Atm_NumConst_c:
      case Atm_EnumConst_c:
      default:
      {
        Main_MochaErrorPrint( "Error: Sym_ExprBuildMdd, unsupported operator\n");
        exit(1);
      }
      
  }
  
  return(result);
}

/**Function********************************************************************

  Synopsis           [build MDD for an expression-constant equality]

  Description        [Given an expression e and a constant c,
                      build MDD for the boolean expression e = c.
		      Note that e and c are multivalued]

  SideEffects        [none]

  SeeAlso            [Sym_ExprBuildMdd]

******************************************************************************/
mdd_t *
SymExprBuildExprConstMdd(
  Sym_Info_t *symInfo,
  Atm_Expr_t *expr,
  int c,
  int nVals)
{
  
  Atm_Expr_t *constant = Atm_ExprAlloc( Atm_RangeConst_c , (Atm_Expr_t *) (long) c,
                                        (Atm_Expr_t *) (long) nVals);
  Atm_Expr_t *eqExpr = Atm_ExprAlloc( Atm_Equal_c, expr, constant);

  mdd_t * result = Sym_ExprBuildMdd(symInfo,  eqExpr);

  
  FREE(constant);
  FREE(eqExpr);
  return(result);
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/



