/**CFile***********************************************************************

  FileName    [symMV.c]

  PackageName [sym]

  Synopsis    [Builds the MDDs for the multi-valued expressions.]

  Description [optional]

  SeeAlso     [optional]

  Author      [Shaz Qadeer]

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

  Synopsis           [Given a parse tree for a multivalued expression compute its mvf]

  Description        [An mvf is an array of mdds.
                      A multivalued function can be represented with such an array of size
		      equal to the number of values that could be taken by the expression.
		      We build such an mvf for the given expression. For information on mvf
		      see the mvf package (borrowed, with thanks, from VIS)]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
Mvf_Function_t *
SymExprBuildMvf(
  Sym_Info_t *symInfo,
  Atm_Expr_t *expr)
{
  Atm_ExprType exprType = Atm_ExprReadType(expr);
  mdd_manager *mddManager = symInfo->manager;
  Mvf_Function_t *mvf;
  int index;

  switch (exprType){
      case Atm_EnumConst_c : {
        Var_EnumElement_t *element = (Var_EnumElement_t *) Atm_ExprReadLeftChild(expr);
        Var_Type_t *varType = Var_EnumElementReadType(element);
        
        mdd_t *one = mdd_one(mddManager);
        
        mvf = Mvf_FunctionAlloc(mddManager, Var_VarTypeReadDomainSize(varType));
        index = Var_EnumElementObtainIndex(element);
        Mvf_FunctionAddMintermsToComponent(mvf, index, one);
        mdd_free(one);
        break;
      }
  
      case Atm_NumConst_c : {
        Main_MochaPrint("SymExprBuildMvf: ITNERNAL  ERROR:  found Atm_NumConst_c \n");
        exit(1);
        break;
      }
      
      case Atm_RangeConst_c: {
        int c = (int) (long) Atm_ExprReadLeftChild(expr);
        int size =  (int) (long) Atm_ExprReadRightChild(expr);
        mdd_t *one = mdd_one(mddManager);    
        
        if( c > size){
          Main_MochaErrorPrint("SymExprBuildMvf: encounterd constant  %d for domain %d, using  modulo\n",
                               c, size);
          c = c % size;
        }
        
        mvf = Mvf_FunctionAlloc(mddManager,size);
        Mvf_FunctionAddMintermsToComponent(mvf, c, one);
        mdd_free(one);
        break;
      }
      
      case Atm_UnPrimedVar_c : {
        Var_Variable_t * var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        int id;
        
        if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, var, &id )){
          Main_MochaErrorPrint( "Error: Sym_BuildMvf, cant find mdd id for var\n");
          exit(1);
        }
        
        mvf = Mvf_FunctionCreateFromVariable(mddManager, id);
        break;
      }
      
      case Atm_PrimedVar_c : {
        Var_Variable_t * var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        int id;
        
        if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &id )){
          Main_MochaErrorPrint( "Error: Sym_BuildMvf, cant find mdd id for var\n");
          exit(1);
        }
        
        mvf = Mvf_FunctionCreateFromVariable(mddManager, id);
        break;
      }
      
      case Atm_Index_c : {
        Atm_Expr_t *lexpr = Atm_ExprReadLeftChild(expr);
        Var_Variable_t *aVar   = (Var_Variable_t *)Atm_ExprReadLeftChild(lexpr);
        Atm_ExprType aVarType = Atm_ExprReadType(lexpr);
        Atm_Expr_t *indexExpr =  Atm_ExprReadRightChild(expr);
        int arrayId, range, nIndices, i, j;
        
        if(SymIsNumEnumRangeConstant(indexExpr)){
          arrayId = SymGetArrayId(symInfo, aVar, aVarType,  SymGetConstant(indexExpr));
          mvf = Mvf_FunctionCreateFromVariable(mddManager, arrayId);
        }
        else {
          
          Mvf_Function_t *indexMvf = SymExprBuildMvf(symInfo, indexExpr);
          nIndices = SymGetArrayNvals(aVar);
          if(nIndices != Mvf_FunctionReadNumComponents(indexMvf)){
            Main_MochaErrorPrint("SymExprBuildMvf: Error array index and index expression are not compatible\n");
            exit(1);
          }
          
          /* get the range of multivalued variable */
          arrayId = SymGetArrayId(symInfo, aVar, aVarType, 0);
          range   = SymGetNvals(symInfo, arrayId);
          
          mvf = Mvf_FunctionAlloc(mddManager,range);
          for ( i = 0; i < range; i++){
            mdd_t *rMdd = mdd_zero(mddManager);
            for ( j = 0; j < nIndices; j++){
              mdd_t *exprMdd, *litMdd;
              mdd_t *temp1, *temp2;
              exprMdd = Mvf_FunctionObtainComponent(indexMvf, j);
              arrayId = SymGetArrayId( symInfo, aVar, aVarType, j);
              litMdd =  SymGetMddLiteral(symInfo->manager, arrayId, i);
              temp1  = mdd_and(exprMdd, litMdd, 1, 1);
              temp2  = mdd_or(temp1, rMdd, 1, 1);
              mdd_free(rMdd);
              mdd_free(temp1);
              mdd_free(exprMdd);
              mdd_free(litMdd);
              rMdd = temp2;
            }
            Mvf_FunctionAddMintermsToComponent(mvf, i, rMdd);
          }
          
          Mvf_FunctionFree(indexMvf);
        }
        
        break;    
      }
      
      case Atm_Plus_c : {
        Mvf_Function_t *mvf1 = SymExprBuildMvf(symInfo, Atm_ExprReadLeftChild(expr));
        Mvf_Function_t *mvf2 = SymExprBuildMvf(symInfo, Atm_ExprReadRightChild(expr));
        
        mvf = Mvf_FunctionComputeAddition(mvf1, mvf2);
        
        Mvf_FunctionFree(mvf1);
        Mvf_FunctionFree(mvf2);
        break;
      }
      
      case Atm_Minus_c : {
        Mvf_Function_t *mvf1 = SymExprBuildMvf(symInfo, Atm_ExprReadLeftChild(expr));
        Mvf_Function_t *mvf2 = SymExprBuildMvf(symInfo, Atm_ExprReadRightChild(expr));
        
        mvf = Mvf_FunctionComputeSubtraction(mvf1, mvf2);
        
        Mvf_FunctionFree(mvf1);
        Mvf_FunctionFree(mvf2);
        break;
      }
      
      case Atm_IfThenElseFi_c : {
        mdd_t *choice = Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        Mvf_Function_t *mvf1 = SymExprBuildMvf(symInfo, Atm_ExprReadLeftChild(Atm_ExprReadRightChild(expr)));
        Mvf_Function_t *mvf2 = SymExprBuildMvf(symInfo, Atm_ExprReadRightChild(Atm_ExprReadRightChild(expr)));
        
        mvf = Mvf_FunctionComputeITE(choice, mvf1, mvf2);
        
        mdd_free(choice);
        Mvf_FunctionFree(mvf1);
        Mvf_FunctionFree(mvf2);
        break;
      }
      
      default : {
        Main_MochaErrorPrint("Operation not supported on multi-valued expressions  %d\n", exprType);
        exit(1);
      }
  }
  
  return(mvf);
}

/**Function********************************************************************

  Synopsis           [Given a boolean expression involving multivalued
                      variables, build mdd for it]

  Description        [Given a multivalued expression of the form
                      e1 <relop> e2, where e1 and e2 are multivalued expressions
		      and <relop> is a comparison operator,
		      this routine builds an MDD for the comparison]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
mdd_t *
SymMVBuildBooleanExpressionMdd(
  Sym_Info_t *symInfo,
  Atm_Expr_t *expr)
{
  Atm_ExprType exprType = Atm_ExprReadType(expr);
  Mvf_Function_t *mvf1  = SymExprBuildMvf(symInfo, Atm_ExprReadLeftChild(expr));
  Mvf_Function_t *mvf2  = SymExprBuildMvf(symInfo, Atm_ExprReadRightChild(expr));
  int range = Mvf_FunctionReadNumComponents(mvf1);
  int id1, id2;
  mdd_t *exprMdd, *temp;
  
  if(range != Mvf_FunctionReadNumComponents(mvf2)){
    Main_MochaErrorPrint("SymBuildMVBooleanExpression: operands have unequal ranges\n");
    exit(1);
  }

  id1 = SymRangeReadDummyId(symInfo, range, 1);
  id2 = SymRangeReadDummyId(symInfo, range, 2);

  exprMdd = SymGetRelExprMdd(symInfo, exprType, id1, id2);
  
  temp = Mvf_MddComposeWithFunction(exprMdd, id1, mvf1);
  mdd_free(exprMdd);
  exprMdd = temp;

  temp = Mvf_MddComposeWithFunction(exprMdd, id2, mvf2);
  mdd_free(exprMdd);
  exprMdd = temp;

  Mvf_FunctionFree(mvf1);
  Mvf_FunctionFree(mvf2);
  return(exprMdd);
}

/**Function********************************************************************

  Synopsis           [Build MDD for multi valued assignment]

  Description        [Build MDD for multi valued assignment]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
mdd_t *
SymMVBuildAssignmentMdd(
  Sym_Info_t *symInfo,
  Var_Variable_t *var,
  Atm_Expr_t *expr)
{
  int id;
  mdd_t * result;
  Mvf_Function_t *mvf;
  
  if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &id )){
    Main_MochaErrorPrint( "Error: Sym_BuildMvf, cant find mdd id for var\n");
    exit(1);
  }

  mvf = SymExprBuildMvf(symInfo, expr);
  result = Mvf_FunctionBuildRelationWithVariable(mvf, id);
  Mvf_FunctionFree(mvf);
  return(result);
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/




