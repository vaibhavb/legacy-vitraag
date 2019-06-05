/**CFile***********************************************************************

  FileName    [atmMain.c]

  PackageName [atm]

  Synopsis    [The main file of the Atm package.]

  Description [This file provides functions for initializing and ending the Atm
  package, and for printing expressions.]

  SeeAlso     [atmInt.h, atm.h]

  Author      [Sriram Rajamani, Shaz Qadeer, Freddy Mang]

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

  Synopsis           [Initializes the atm package.]

  SideEffects        [None]

******************************************************************************/
int
Atm_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Var_TypeManager_t *typeManager = Main_ManagerReadTypeManager(manager);
  
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Reinitializes the atm package.]

  SideEffects        [None]

******************************************************************************/
int
Atm_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Var_TypeManager_t *typeManager = Main_ManagerReadTypeManager(manager);
  
  BoolType  = Var_TypeManagerReadTypeFromName(typeManager, "bool");            
  EventType = Var_TypeManagerReadTypeFromName(typeManager, "event");           
  NatType = Var_TypeManagerReadTypeFromName(typeManager, "nat");               
  IntType = Var_TypeManagerReadTypeFromName(typeManager, "int");               

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Ends the atm package.]

  SideEffects        [None]

******************************************************************************/
int
Atm_End(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
   return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Prints the expression.]

  Description        [This function prints the given expression in infix
                      notation.]

  SideEffects        [None]

  SeeAlso            [Atm_ExprTypeConvertToString]

******************************************************************************/
void
Atm_ExprPrintExpression(
  Atm_Expr_t* expr)
{
  char buf[Main_MAXMSGLEN];

  switch (expr->type){

      case Atm_IfThenElseFi_c:
      case Atm_BitwiseIfThenElseFi_c:
      {
        Main_AppendResult("if "); 
        Atm_ExprPrintExpression(expr->leftExpr);
        Main_AppendResult(" then "); 
        Atm_ExprPrintExpression(expr->rightExpr->leftExpr);
        Main_AppendResult(" else "); 
        Atm_ExprPrintExpression(expr->rightExpr->rightExpr);
        Main_AppendResult(" fi ");
        break;
      }
      
      case Atm_Not_c:
      case Atm_BitwiseNot_c:
      case Atm_UnaryMinus_c:
      case Atm_RedAnd_c:
      case Atm_RedOr_c:
      case Atm_RedXor_c:
      {
        sprintf(buf, "%s", Atm_ExprTypeConvertToString(expr->type));
        Main_AppendResult("%s", buf); 
        Atm_ExprPrintExpression(expr->leftExpr);
        break;
      }

      case Atm_EventQuery_c:
      {
        Atm_ExprPrintExpression(expr->leftExpr);
        sprintf(buf, "%s", Atm_ExprTypeConvertToString(expr->type));
        Main_AppendResult("%s", buf); 
        break;
      }
      
      case Atm_BoolConst_c:
      {
        if (expr->leftExpr == 0) 
          Main_AppendResult("false");
        else
          Main_AppendResult("true");
        break;
      }
      
      case Atm_NumConst_c:
      case Atm_RangeConst_c:        
      {
        sprintf(buf, "%d", (int) (long) expr->leftExpr);
        Main_AppendResult("%s",  buf); 
        break;
      }

      case Atm_BitwiseNumConst_c: 
      {
        int numBits = (int) (long) expr->rightExpr;
        char *bitArray = (char *) expr->leftExpr;
        int i;
        
        buf[0] = 'b';
        for (i = 0; i < numBits; i++) {
            buf[numBits-i] = (bitArray[i])? '1' :'0';
        }
        buf[numBits+1] = '\0';
        Main_AppendResult("%s", buf);
        break;
      }
      
      case Atm_PrimedVar_c:
      case Atm_BitwisePrimedVar_c:
      {
        sprintf(buf, "%s'", Var_VariableReadName((Var_Variable_t*) expr->leftExpr));
        Main_AppendResult("%s", buf); 
        break;
      }
      
      case Atm_UnPrimedVar_c:
      case Atm_BitwiseUnPrimedVar_c:
      {
        sprintf(buf, "%s", Var_VariableReadName((Var_Variable_t*) expr->leftExpr));
        Main_AppendResult("%s", buf); 
        break;
      }
      
      case Atm_EnumConst_c:
      {
        sprintf(buf, "%s",
                Var_EnumElementReadName((Var_EnumElement_t*)
                                        expr->leftExpr)); 
        Main_AppendResult("%s", buf); 
        break;
      }
      case Atm_Index_c:
      case Atm_BitwiseIndex_c:
      {
        Atm_ExprPrintExpression(expr->leftExpr);
        Main_AppendResult("[");
        Atm_ExprPrintExpression(expr->rightExpr);
        Main_AppendResult("]");
        break;
      }

      case Atm_Nondet_c:
      {
        Main_AppendResult("nondet");
        break;
      }
      
      default: {
        Main_AppendResult("("); 
        Atm_ExprPrintExpression(expr->leftExpr);
        sprintf(buf, "%s", Atm_ExprTypeConvertToString(expr->type));
        Main_AppendResult("%s", buf);
        Atm_ExprPrintExpression(expr->rightExpr);
        Main_AppendResult(")");
        break;
      }
  }
  
}

/**Function********************************************************************

  Synopsis           [Returns the symbolic character for an expression type.]

  Description        [This function returns a string containing the operator
  corresponding to an expression type.]

  SideEffects        [None]

  SeeAlso            [Atm_ExprPrintExpression]

******************************************************************************/
char* 
Atm_ExprTypeConvertToString(
  Atm_ExprType type)
{
  switch (type) {
      case Atm_Equal_c:
      case Atm_BitwiseEqual_c:
        return ("=");
      case Atm_NotEqual_c:
      case Atm_BitwiseNotEqual_c:
        return ("!=");
      case Atm_And_c:
      case Atm_BitwiseAnd_c:
        return (" & ");
      case Atm_Or_c:
      case Atm_BitwiseOr_c:
        return (" | "); 
      case Atm_Greater_c:
      case Atm_BitwiseGreater_c:
        return (" > "); 
      case Atm_Less_c:
      case Atm_BitwiseLess_c:
        return (" < ");
      case Atm_GreaterEqual_c:
      case Atm_BitwiseGreaterEqual_c:
        return (" >= ");
      case Atm_LessEqual_c:
      case Atm_BitwiseLessEqual_c:
        return (" <= ");
      case Atm_Plus_c:
      case Atm_BitwisePlus_c:
        return (" + "); 
      case Atm_Minus_c:
      case Atm_BitwiseMinus_c:
        return (" - "); 
      case Atm_Not_c:
      case Atm_BitwiseNot_c:
        return ("~"); 
      case Atm_UnaryMinus_c:
        return ("-"); 
      case Atm_EventQuery_c:
        return ("?");
      case Atm_Implies_c:
      case Atm_BitwiseImplies_c:
        return (" -> ");
      case Atm_Equiv_c:
      case Atm_BitwiseEquiv_c:
        return (" <-> "); 
      case Atm_NotEquiv_c:
        return (" != "); 
      case Atm_Xor_c:
      case Atm_BitwiseNotEquiv_c:
      case Atm_BitwiseXor_c:
        return (" ^ "); 
      case Atm_RedAnd_c:
        return ("&");
      case Atm_RedOr_c:
        return ("|");
      case Atm_RedXor_c:
        return ("^");
        
      default:
        return (""); 
  }
} 

    

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
      

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/








