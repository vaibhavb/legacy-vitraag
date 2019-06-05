/**CFile***********************************************************************

  FileName    [symAssignMdd.c]

  PackageName [sym]

  Synopsis    [routines to build mdd for an assignment]

  Description [An assignment has an LHS var and an RHS expression.
               Assignments can be classified based on the type of LHS.
	       This file has routines to build MDDs corresponding to assignments.
	       (1) If the LHS is boolean, MDDs are created separately for 
	           LHS and RHS, and are then xnored. 
	       (2) If the LHS is bitvector or multivalued specialized functions
	           (implemented on symMV.c or symBitVector.c) are called.
	       ]

  SeeAlso     [SymExprMdd.c]

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
/**Function********************************************************************

  Synopsis           [build MDD for an assignment]

  Description        [Given an assignment, build an MDD for the assignment]

  SideEffects        [The variables that are assigned to, are added to the
                      assignedVarTable that is passed in]

  SeeAlso            [SymAssignmentBuildMddStandard]

******************************************************************************/
mdd_t *
Sym_AssignmentBuildMdd(
  Sym_Info_t *symInfo,
  Atm_Assign_t *assign,
  st_table *assignedVarTable)
{

  Atm_AssignType aType;
  mdd_manager *manager = symInfo->manager;
  mdd_t *assignmentMdd;
  Var_Variable_t *var;  
  Var_Type_t *  varType;
  Atm_Expr_t *expr;
  int pid, size, i;
  Atm_Assign_t *newAssignment;
  Mdl_Module_t *module = SymInfoReadModule(symInfo);
  char *actualVarName, *tmpString;
  Var_Variable_t *actualVar;
  
  aType  = Atm_AssignReadType(assign);
  
  switch(aType) {
      case Atm_AssignStd_c:
      {
	Atm_ExprType exprType;
        var = Atm_AssignReadVariable(assign);
        expr = Atm_AssignReadExpr(assign);
        varType = Var_VariableReadType(var);
	exprType = Atm_ExprReadType(expr);

	/* handle if then else constructs right here for correct handling of  nondets */
	if( (exprType == Atm_IfThenElseFi_c) || (exprType == Atm_BitwiseIfThenElseFi_c)){
	  mdd_t *condMdd, *ifAssignMdd, *thenAssignMdd, *temp1, *temp2;
	  Atm_Expr_t *condExpr = Atm_ExprReadLeftChild(expr);  
	  Atm_Expr_t *ifExpr   = Atm_ExprReadLeftChild(Atm_ExprReadRightChild(expr));  
	  Atm_Expr_t *thenExpr = Atm_ExprReadRightChild(Atm_ExprReadRightChild(expr));  

	  condMdd = Sym_ExprBuildMdd(symInfo, condExpr );

	  newAssignment = Atm_AssignStdAlloc(var, ifExpr);
	  ifAssignMdd   = Sym_AssignmentBuildMdd(symInfo, newAssignment, assignedVarTable); 
	  FREE(newAssignment);

	  newAssignment = Atm_AssignStdAlloc(var, thenExpr);
	  thenAssignMdd   = Sym_AssignmentBuildMdd(symInfo, newAssignment, assignedVarTable); 
	  FREE(newAssignment);

	  temp1 = mdd_and(condMdd, ifAssignMdd,   1, 1);  
	  temp2 = mdd_and(condMdd, thenAssignMdd, 0, 1);  
	  assignmentMdd = mdd_or(temp1, temp2, 1, 1);
	  mdd_free(temp1);
	  mdd_free(temp2);
	  mdd_free(condMdd);
	  mdd_free(ifAssignMdd);
	  mdd_free(thenAssignMdd);
	}
	else{
	  if (Var_TypeIsBoolean(varType) ||
              Var_TypeIsEnum(varType) ||
              Var_TypeIsRange(varType) ||
              Var_TypeIsTimer(varType) ) {
	    
	    st_insert(assignedVarTable, (char *) var, NIL(char));
	    
	    if (Atm_ExprReadType(expr) == Atm_Nondet_c) 
	      assignmentMdd = mdd_one(symInfo->manager);
	    else
	      assignmentMdd = SymAssignmentBuildMddStandard(symInfo,var,expr);
	  }
	  else if(Var_TypeIsBV(varType)){
	    size =
              Var_VarTypeArrayOrBitvectorReadSize(Var_VariableReadType(var));
	    for (i = 0; i < size; i++) {
	      tmpString = ALLOC(char, INDEX_STRING_LENGTH);
              sprintf(tmpString, "%d", i);
	      actualVarName = util_strcat4(Var_VariableReadName(var), "[",
					   tmpString, "]");
	      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
	      st_insert(assignedVarTable, (char *) actualVar, NIL(char));
	      FREE(tmpString);
	      FREE(actualVarName);
	    }
	    
	    if (Atm_ExprReadType(expr) == Atm_Nondet_c) 
	      assignmentMdd = mdd_one(symInfo->manager);
	    else
	      assignmentMdd = SymBitVectorBuildAssignmentMdd(symInfo,var,expr);
	  }
	  else {
	    Main_MochaErrorPrint( "Error: Sym_AtomBuildTransitionRelation, bad assignment\n");
	    exit(1);
	  }
	}
        break;
      }
      
      case Atm_AssignIndex_c:
      {
        Var_Variable_t *indexedVar;
        Var_DataType  indexedVarDataType;
        Atm_Expr_t *indexExpr;
        var = Atm_AssignReadVariable(assign);
        indexExpr = Atm_AssignReadIndexExpr(assign);
        expr = Atm_AssignReadExpr(assign);

        /* this should be have been checked during type checking */
        if(!SymIsNumEnumRangeConstant(indexExpr)){
          Main_MochaErrorPrint("Error: Sym_AtomBuildTransitionRelation: array index of LHS is not a constant expression\n");
          exit(1);
        }
        
        indexedVar = SymGetArrayElementVar(symInfo, var, SymGetConstant(indexExpr));
        indexedVarDataType =  Var_VariableReadDataType(indexedVar);
        switch(indexedVarDataType){
            case Var_Boolean_c:
            case Var_Enumerated_c:
            case Var_Range_c:
            case Var_Timer_c:
            case Var_Bitvector_c:
            case Var_BitvectorN_c:
              
              newAssignment = Atm_AssignStdAlloc( indexedVar, expr);
              assignmentMdd = Sym_AssignmentBuildMdd(symInfo, newAssignment, assignedVarTable);
              FREE(newAssignment);
              break;
            default:
              Main_MochaErrorPrint("Error: Sym_AtomBuildTransitionRelation: unsupported array variable type\n");
              exit(1);
        }
        break;
      }

      case Atm_AssignForall_c:
      {
        Var_Variable_t *indexedVar;
        Var_DataType  indexedVarDataType;
        int nVals, i;
        var = Atm_AssignReadVariable(assign);
        expr = Atm_AssignReadExpr(assign);

        nVals = Var_VarTypeArrayOrBitvectorReadSize(Var_VariableReadType(var));
        assignmentMdd  = mdd_one(symInfo->manager);

        for ( i= 0; i < nVals; i++){
          mdd_t *newAssignmentMdd, *temp;
          Atm_Expr_t *newExpr;
          Var_DataType entryDataType;
          boolean indexTypeIsEnum;

          indexedVar = SymGetArrayElementVar(symInfo, var, i);
          
          if (Var_TypeIsBV(Var_VariableReadType(var))) {

            indexTypeIsEnum = FALSE;
            entryDataType = Var_Boolean_c;
          } else {
            
            indexTypeIsEnum =
                Var_TypeIsEnum(Var_VarTypeReadIndexType(Var_VariableReadType(var)));
            entryDataType   =  Var_VariableReadDataType(indexedVar);
          }
          
          switch(entryDataType){
              case Var_Boolean_c:
              case Var_Enumerated_c:
              case Var_Range_c:
              case Var_Timer_c:
              case Var_Bitvector_c:
              case Var_BitvectorN_c:
              {
                Var_Variable_t * dummyVar = Atm_AssignReadIndexVar(assign);

                newExpr =
                    Atm_ExprSubstituteConstantForDummyVar(expr, dummyVar, i,
                                                          indexTypeIsEnum);
                newAssignment = Atm_AssignStdAlloc( indexedVar, newExpr);
                newAssignmentMdd = Sym_AssignmentBuildMdd(symInfo,
                                                         newAssignment, assignedVarTable);
                temp = mdd_and(assignmentMdd, newAssignmentMdd, 1, 1);
                mdd_free(assignmentMdd);
                mdd_free(newAssignmentMdd);
                assignmentMdd = temp;
                Atm_ExprFreeExpression(newExpr);
                FREE(newAssignment);
                break;
              }
              default:
                Main_MochaErrorPrint("Error: Sym_AtomBuildTransitionRelation: unsupported array variable type\n");
                exit(1);
          }
        }
        break;
      }
      
      
      case Atm_AssignEvent_c:
      {
        Atm_Expr_t *indexExpr;
        Var_Variable_t *indexedVar;
        var = Atm_AssignReadVariable(assign);
        varType = Var_VariableReadType(var);
        if (Var_TypeIsEvent(varType)) {
          if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &pid )){
            Main_MochaErrorPrint( "Error: Sym_AtomBuildTransitionRelation, cant find mdd id for var\n");
          }
          st_insert(assignedVarTable, (char *) var, NIL(char));
          assignmentMdd = SymGetMddLiteral(symInfo->manager, pid, 1);
        } else if (Var_TypeIsArray(varType)) {
          indexExpr = Atm_AssignReadIndexExpr(assign);
          actualVar = SymGetArrayElementVar(symInfo, var, SymGetConstant(indexExpr));
          pid = SymGetArrayId(symInfo, var, Atm_PrimedVar_c, SymGetConstant(indexExpr));
          st_insert(assignedVarTable, (char *)actualVar, NIL(char));
          assignmentMdd = SymGetMddLiteral(symInfo->manager, pid, 1);
        } else {
          Main_MochaErrorPrint( "Error: Sym_AtomBuildTransitionRelation, unexpected variable type -- event expected\n");
          exit(1);
        }
        break;
      }
        
      default:
        Main_MochaErrorPrint( "Error: Sym_AtomBuildTransitionRelation, unsupported assignment type\n");
        exit(1);
  }
  
  return(assignmentMdd);
}
 
/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [build MDD for a standard asssignment]

  Description        [A standard assignment assigns a scalar value (RHS)
                      to a scalar variable (LHS). Depending on whether the
		      LHS is boolean or multivalued, different functions are
		      called]

  SideEffects        [none]

  SeeAlso            [SymAssignmentBuildMddBoolean]

******************************************************************************/
mdd_t *
SymAssignmentBuildMddStandard(
  Sym_Info_t *symInfo,
  Var_Variable_t *var,
  Atm_Expr_t *expr
  )
{
  Var_DataType varType = Var_VariableReadDataType(var);
  mdd_t * result;

  if(varType == Var_Boolean_c){
    result = SymAssignmentBuildMddBoolean(symInfo, var, expr);
  }
  else{
    result = SymMVBuildAssignmentMdd(symInfo, var, expr);
  }

  return(result);
}
 
 

/**Function********************************************************************

  Synopsis           [Build MDD for boolean assignment with LHS and RHS
                      of type boolean]

  Description        [Simply build MDDs for LHS and RHS separately and xnor them]

  SideEffects        [none]

  SeeAlso            [SymMVBuildAssignmentMdd]

******************************************************************************/
mdd_t * SymAssignmentBuildMddBoolean(
    Sym_Info_t *symInfo,
    Var_Variable_t *var,
    Atm_Expr_t *expr)
{
  mdd_t *temp1, *temp2, *result;
  int lhsId;

  if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &lhsId )){
    Main_MochaErrorPrint( "Error: Sym_BuildMvf, cant find mdd id for var\n");
    exit(1);
  }

  /* RHS is a boolean expression */
  temp1 = Sym_ExprBuildMdd(symInfo, expr );
  temp2 = SymGetMddLiteral(symInfo->manager, lhsId, 1);
  result = mdd_xnor(temp1, temp2);
  mdd_free(temp1);
  mdd_free(temp2);
  return(result);
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/



