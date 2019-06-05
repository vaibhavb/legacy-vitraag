/**CFile***********************************************************************

  FileName    [InvUtil.c]

  PackageName [Inv]

  Synopsis    [Routines for manipulating invariants.]

  Description [This file contains routines for accessing the fields of the 
  invariant data structure, for printing invariants, and for reading invariants
  from a file.]

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

******************************************************************************/

#include  "invInt.h"
#define MAX_NUMCONST_LENGTH 50

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
EXTERN FILE* InvYyin;

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
/**Enum************************************************************************

  Synopsis    [Data Type of Inv_Invariant_t]

  Description [These are the data type being represented by a Inv_Invariant_t.
  They are: boolean, number and universal. All well-formed atl formula should
  have boolean type. All operands of arithmetic operators should have number
  type. All variables should have universal type.  Note that this is used in
  checking the well-formedness of the atl formula.  The full type checking is
  done only when the formula is being model-checked against a module, when the
  exact data type of the variables are defined.]

  SeeAlso     [InvExprIsWellFormed]

******************************************************************************/
typedef enum {
  InvBool_c,
  InvNum_c,
  InvUniversal_c,
  InvError_c
} InvSimpleDataType;



/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static Atm_Expr_t * InvExprConvertToAtmExpr(Mdl_Module_t * module, Var_TypeManager_t * typeManager, InvExpr_t *invExpr);
static InvSimpleDataType SimpleDataTypeCompatible(InvSimpleDataType type1, InvSimpleDataType type2);
static InvSimpleDataType ExprReturnSimpleType(InvExpr_t * expr);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Allocate a Inv manager.]

  Description        [This function allocates the a new Inv_Manager_t, and
  initialize the hash table in the manager.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Inv_Manager_t *
Inv_ManagerAlloc()
{
  Inv_Manager_t * manager;
  manager = ALLOC(Inv_Manager_t, 1);
  
  manager -> nameToFormula = st_init_table(strcmp, st_strhash);
  
  return manager;

}

/**Function********************************************************************

  Synopsis           [Frees a Inv manager]

  Description        [This function frees a Inv_Manager_t, frees the names of the
  atl formula, and decrements the ref count of the formula. If the reference
  count of the formula comes to zero, the formula is freed.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Inv_ManagerFree(
  Inv_Manager_t * manager)
{
  st_generator * gen;
  char * name;
  Inv_Invariant_t *formula;
  
  if (manager != NIL(Inv_Manager_t)) {
    st_foreach_item (manager -> nameToFormula, gen, &name, (char **) &formula){
      FREE(name);
      InvInvariantFree(formula);
    }

    st_free_table(manager->nameToFormula);
    FREE(manager);
  }
}


/**Function********************************************************************

  Synopsis    [Parses a file containing a set of ATL formulas.]

  Description [Parses a file containing a set of semicolon-ending ATL
  formulas, and returns an array of Inv_Invariant_t representing those
  formulas.  If an error is detected while parsing the file, the routine frees
  any allocated memory and returns NULL.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
lsList
Inv_FileParseFormula(
  Inv_Manager_t *manager,
  FILE * fp)
{
  
  InvYyin = fp;

  return (InvParse(manager));

}

/**Function********************************************************************

  Synopsis           [Returns an array of Atl Formula from the ATL Manager]

  Description        [Given an ATL Manager this function returns a list of
  all the ATL formula from the manager. ]

  SideEffects        [This creates an array, which should be freed after use.
  Beware that the formula should not be freed.]

  SeeAlso            [optional]

******************************************************************************/
array_t *
Inv_ManagerReadInvariants(
  Inv_Manager_t *manager)
{
  array_t * formulaArray;
  Inv_Invariant_t * formula;
  char * name;
  int i;
  st_generator * gen;
  
  if (manager == NIL(Inv_Manager_t)) {
    return NIL(array_t);
  }

  formulaArray = array_alloc(Inv_Invariant_t *,
                             st_count(manager->nameToFormula));

  i = 0;
  st_foreach_item (manager->nameToFormula, gen, &name, (char **) &formula) {
    array_insert(Inv_Invariant_t *, formulaArray, i, formula);
    i++;
  }
  
  return formulaArray;

}

/**Function********************************************************************

  Synopsis           [Returns a list of Atl formula Names]

  Description        [Given an ATL Manager, this function returns an array
  of all the formula names.]

  SideEffects        [User should free the array after use.  Note the
  content should not be freed.]

  SeeAlso            [optional]


******************************************************************************/
array_t *
Inv_ManagerReadInvariantNames(
  Inv_Manager_t *manager)
{
  array_t * nameArray;
  char * name;
  int i;
  st_generator * gen;
  
  if (manager == NIL(Inv_Manager_t)) {
    return NIL(array_t);
  }

  nameArray = array_alloc(char *, st_count(manager->nameToFormula));

  i = 0;
  st_foreach_item (manager->nameToFormula, gen, &name, NIL(char *)) {
    array_insert(char *, nameArray, i, name);
    i++;
  }
  
  return nameArray;

}

/**Function********************************************************************

  Synopsis           [Returns a pointer to the formula, given name.]

  Description        [This function returns a pointer to the formula
  with the given name.  If the formula is not found, a NIL pointer
  is returned.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Inv_Invariant_t *
Inv_ManagerReadInvariantFromName(
  Inv_Manager_t * manager,
  char * name)
{
  Inv_Invariant_t * formula = NIL(Inv_Invariant_t);
  
  if (manager == NIL (Inv_Manager_t) || name == NIL(char))
    return NIL(Inv_Invariant_t);

  st_lookup(manager -> nameToFormula, name, (char **) &formula);

  return formula;
  
}


/**Function********************************************************************

  Synopsis    [Frees an array of invariants.]

  Description [Calls InvInvariantFree on each formula in
  formulaArray, and then frees the array itself.] 

  SideEffects []

  SeeAlso     [InvInvariantFree]

******************************************************************************/
void
Inv_InvariantArrayFree(
  array_t * formulaArray)
{
  if (formulaArray != NIL(array_t)) {
    int i; 
    int numFormulas = array_n(formulaArray);
    
    for (i = 0; i < numFormulas; i++) {
      Inv_Invariant_t *formula = array_fetch(Inv_Invariant_t *, formulaArray, i);
      
      InvInvariantFree(formula);
    }
    
    array_free(formulaArray);
  }
}
    
/**Function********************************************************************

  Synopsis           [Returns the typed expression of an invariant.]

  Description        [This function returns the typed Atm_Expr_t of an
  invariant.]

  SideEffects        [None]

  SeeAlso            [Inv_InvariantReadName]

******************************************************************************/
Atm_Expr_t *
Inv_InvariantReadTypedExpr(
  Inv_Invariant_t *invariant)
{
   return invariant->typedExpr;
}


/**Function********************************************************************

  Synopsis           [Returns the name of the invariant.]

  Description        [This function returns a string containing the name of the
  invariant. The caller is not supposed to free the string.]

  SideEffects        [None]

  SeeAlso            [Inv_InvariantReadTypedExpr]

******************************************************************************/
char *
Inv_InvariantReadName(
  Inv_Invariant_t *invariant)
{
   return invariant->name;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Allocate an invariant structure.]

  Description        [Allocate an invariant structure and return a pointer to
                      it.]

  SideEffects        [None]

  SeeAlso            [InvInvariantFree]

******************************************************************************/
Inv_Invariant_t *
InvInvariantAlloc(
  char *name,
  InvExpr_t *expr)
{
  Inv_Invariant_t *invariant;

  invariant = ALLOC(Inv_Invariant_t, 1);
  invariant -> name = util_strsav(name);
  invariant -> invExpr = expr;
  invariant -> typedExpr = NIL(Atm_Expr_t);

  return invariant;
}

/**Function********************************************************************

  Synopsis           [Free an invariant structure.]

  Description        [Free an invariant structure.]

  SideEffects        [None]

  SeeAlso            [InvInvariantAlloc]

******************************************************************************/
void
InvInvariantFree(
  Inv_Invariant_t *invariant)
{
  if (invariant != NIL(Inv_Invariant_t)){
    if (invariant -> name)
      FREE (invariant -> name);
    if (invariant -> invExpr)
      InvExprFree(invariant->invExpr);

    FREE(invariant);
  }  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
InvExpr_t *
InvExprAlloc(
  InvExprType exprType,
  InvExpr_t *left,
  InvExpr_t *right)
{
   InvExpr_t *invExpr = ALLOC(InvExpr_t, 1);

   invExpr->type = exprType;
   invExpr->left = left;
   invExpr->right = right;

   return invExpr;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
InvExprFree(
  InvExpr_t *invExpr)
{
  InvExprType exprType;
  
   
  if (invExpr == NIL(InvExpr_t))
    return;
  
  exprType = invExpr->type;

  if (exprType != InvNumConst_c) {
    
    if (exprType == InvId_c) {
      FREE(invExpr->left);
    }
    else {
      InvExprFree(invExpr->left);
      InvExprFree(invExpr->right);
    }
  }
  
  FREE(invExpr);
}


/**Function********************************************************************

  Synopsis    [Adds formula to the Inv manager]

  SideEffects [Manipulates the global variable globalFormulaManager.]

  SeeAlso     [InvYyparse]

******************************************************************************/
void
InvFormulaAddToManager(
  Inv_Manager_t *manager,
  char * name,
  Inv_Invariant_t * formula)
{
  Inv_Invariant_t * oldFormula;
  char * tmpname = name;
  
  if (st_delete (manager->nameToFormula, (char **) &tmpname, (char **)
                 &oldFormula)) {
    /* there is a formula with the same name */
    InvInvariantFree(oldFormula);
    FREE(tmpname);
  }

  /* add the new formula */
  st_insert (manager->nameToFormula, name, (char *) formula);
  
}


/**Function********************************************************************

  Synopsis           [Check if the atl formula is well formed]

  Description        [This is a preliminary checking of well-formedness of the
  formula. This is different from full type checking in that variables are
  considered to have "universal" type.  The full type checking cannot be done
  until the formula is associated with the type module.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
InvExprIsWellFormed(
  InvExpr_t * expr)
{
  /*
  InvSimpleDataType t;


  t = SimpleDataTypeCompatible(ExprReturnSimpleType(expr),
			       InvBool_c);
  
  return (t != InvError_c);
  */
  return TRUE;
  
}


/**Function********************************************************************

  Synopsis    [Returns formula as a character string.]

  Description [Returns formula as a character string. All subformulas are
  delimited by parenthesis. The syntax used is the same as used by the ATL
  parser.  Does nothing if passed a NULL formula.]

  SideEffects [InvExprConvertToString]

******************************************************************************/
char *
InvExprConvertToString(
  InvExpr_t * expr)
{
  char *s1        = NIL(char);
  char *s2        = NIL(char);
  char *s3        = NIL(char);
  char *tmpString = NIL(char);
  char *result;
  
  if (expr == NIL(InvExpr_t)) {
    return NIL(char);
  }

  /* The expr is an name or a number */
  if (expr->type == InvId_c) {
    return util_strsav((char *) expr->left);
  }

  if (expr->type == InvBoolConst_c) {
     if (expr->left == NIL(InvExpr_t))
	return util_strsav("FALSE");
     else
	return util_strsav("TRUE");
  }

  if (expr->type == InvNumConst_c) {
     result = ALLOC(char, MAX_NUMCONST_LENGTH);
     sprintf(result, "%d", (long) expr->left);
     return result;
  }

  /* If the expr is a non-leaf, the function is called recursively */
  s1 = InvExprConvertToString(expr->left);
  s2 = InvExprConvertToString(expr->right);
      
  switch(expr->type) {

    /* boolean connectives */
      case InvOr_c: 
        tmpString = util_strcat3(s1, " | ",s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case InvAnd_c: 
        tmpString = util_strcat3(s1, " & ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case InvImplies_c: 
        tmpString = util_strcat3(s1, " => ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case InvEquiv_c: 
        tmpString = util_strcat3(s1, " <=> ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case InvNot_c: 
        tmpString = util_strcat3("(", s1, ")");
        result    = util_strcat3("!", tmpString, "");
        break;
        /* arithmetic comparisons */
      case InvGreater_c:
        tmpString = util_strcat3(s1, " > ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case InvLess_c:
        tmpString = util_strcat3(s1, " < ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case InvGreaterEqual_c:
        tmpString = util_strcat3(s1, " >= ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case InvLessEqual_c:
        tmpString = util_strcat3(s1, " <= ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case InvEqual_c:
        tmpString = util_strcat3(s1, " = ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break; 
        /* arithmetic expressions */
      case InvPlus_c:
        tmpString = util_strcat3(s1, " + ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case InvMinus_c:
        tmpString = util_strcat3(s1, " - ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case InvUnaryMinus_c:
        tmpString = util_strcat3("(", s1, ")");
        result    = util_strcat3("-", tmpString, "");
        break;
      case InvIndex_c:
        tmpString = util_strcat3("[", s2, "]");
        result    = util_strcat3(s1, tmpString, "");
        break;

	/* Ashwini: Add case InvXor_c */
      case InvXor_c:
        tmpString = util_strcat3(s1, " ^ ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
        
      default:
        Main_MochaErrorPrint(
          "Unexpected type in InvExprConvertToString\n");
        exit(1);
  }

  if (s1 != NIL(char)) {
    FREE(s1);
  }

  if (s2 != NIL(char)) {
    FREE(s2);
  }

  if (s3 != NIL(char)) {
    FREE(s3);
  }

  if (tmpString != NIL(char)) {
    FREE(tmpString);
  }

  return result;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Atm_Expr_t *
InvInvariantCreateTypedExpr(
  Mdl_Module_t *module,
  Var_TypeManager_t *typeManager,
  Inv_Invariant_t *invariant)
{
   InvExpr_t *invExpr = invariant->invExpr;
   Atm_Expr_t *expr = InvExprConvertToAtmExpr(module, typeManager, invExpr);

   if (expr == NIL(Atm_Expr_t)) {
     return NIL(Atm_Expr_t);
   }

   /* type check as well as adding type information */
     
   Atm_TypeCheckNew(BoolType, &expr);
   
   return expr;
   
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static Atm_Expr_t *
InvExprConvertToAtmExpr(
  Mdl_Module_t * module,
  Var_TypeManager_t * typeManager,
  InvExpr_t *invExpr)
{
  Atm_Expr_t * leftAtmExpr;
  Atm_Expr_t * rightAtmExpr;
  Atm_ExprType atmExprType;
  InvExpr_t * left = invExpr->left;
  InvExpr_t * right = invExpr->right;
  InvExprType type = invExpr->type;
  int isPrimed; /* Ashwini*/
  int len;      /* Ashwini*/

  /* leaf node */

  if (type == InvId_c) {
    /* this can be an enum constant, or a variable */
    char * name = (char *) left;
    Var_Variable_t *var;

    /* Ashwini: chk if the variable is primed or unprimed */
    len = strlen(name);
    if (*(name+len-1) != '\'')
	isPrimed = 0;
    else {
	*(name+len-1) = '\0';
	isPrimed = 1;
    }
    /* Ashwini: end */

    var = Mdl_ModuleReadVariableFromName(name, module);
    
    if (var != NIL(Var_Variable_t)) {
	/* Ashwini :start */
	if( !isPrimed) {
           return Atm_ExprAlloc(Atm_UnPrimedVar_c, (Atm_Expr_t*) var,
                           NIL(Atm_Expr_t));
	} else {
           return Atm_ExprAlloc(Atm_PrimedVar_c, (Atm_Expr_t*) var,
                           NIL(Atm_Expr_t));
	}
	/*Ashwini: end */

    } else {
      Var_EnumElement_t * enumConst =
          Var_TypeManagerReadEnumElementFromName(typeManager, name);
      if (enumConst != NIL(Var_EnumElement_t)) {
        return Atm_ExprAlloc(Atm_EnumConst_c, (Atm_Expr_t*) enumConst,
                             NIL(Atm_Expr_t));
      }
      else {
	 Main_MochaErrorPrint("Variable %s not found in module\n",  name);
	 return NIL(Atm_Expr_t);
      }
    }
  }
  
  if (type == InvNumConst_c) {
     return Atm_ExprAlloc(Atm_NumConst_c, (Atm_Expr_t *) left,
                         NIL(Atm_Expr_t));
  }  

  if (type == InvBoolConst_c) {
     return Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) left,
			  NIL(Atm_Expr_t));
  }  

  /* internal node */
  leftAtmExpr = InvExprConvertToAtmExpr(module, typeManager, left);

  if (leftAtmExpr == NIL(Atm_Expr_t))
    return NIL(Atm_Expr_t);

  if (!(type == InvUnaryMinus_c || type == InvNot_c)) {
     rightAtmExpr = InvExprConvertToAtmExpr(module, typeManager, right);
     if (rightAtmExpr == NIL(Atm_Expr_t)) {
	Atm_ExprFreeExpression(leftAtmExpr);
	return NIL(Atm_Expr_t);
     }
  }
  else {
     rightAtmExpr = NIL(Atm_Expr_t);
  }

  switch (type) {
      case InvNot_c:
	 atmExprType = Atm_Not_c;
         break;

      case InvAnd_c:
	 atmExprType = Atm_And_c;
	 break;

      case InvOr_c :
	 atmExprType = Atm_Or_c;
	 break;

      case InvImplies_c:
	 atmExprType = Atm_Implies_c;
	 break;

	 /* Ashwini */
      case InvXor_c:
	 atmExprType = Atm_Xor_c;
	 break;

      case InvEquiv_c:
	 atmExprType = Atm_Equiv_c;
	 break;

      case InvUnaryMinus_c:
         atmExprType = Atm_UnaryMinus_c;
         break;

      case InvGreaterEqual_c:     /* >= */
        atmExprType = Atm_GreaterEqual_c;
        break;
        
      case InvLessEqual_c:     /* <= */
        atmExprType = Atm_LessEqual_c;
        break;

      case InvGreater_c:     /* > */
        atmExprType = Atm_Greater_c;
        break;

      case InvLess_c:     /* < */
        atmExprType = Atm_Less_c;
        break;

      case InvEqual_c:     /* = */
        atmExprType = Atm_Equal_c;
        break;
        
      case InvPlus_c:   /* +  */
        atmExprType = Atm_Plus_c;
        break;
                
      case InvMinus_c:  /* -  */
        atmExprType = Atm_Minus_c;
        break;

      case InvIndex_c:
	 atmExprType = Atm_Index_c;
	 break;
	 
      default:
        fprintf(stderr, "Illegal type in InvExprConvertToAtmExpr\n");
        exit(1);
        break;
  }

  return Atm_ExprAlloc(atmExprType, leftAtmExpr, rightAtmExpr);
}


/**Function********************************************************************

  Synopsis           [See if the simple type are compatible.]

  Description        [optional]

  SideEffects        [The infered type is returned.]

  SeeAlso            [optional]

******************************************************************************/
static InvSimpleDataType
SimpleDataTypeCompatible(
  InvSimpleDataType type1,
  InvSimpleDataType type2)
{
  if (type1 == type2) {
    return type1;
  }
  
  if (type1 == InvUniversal_c) {
    return type2;
  }

  if (type2 == InvUniversal_c) {
    return type1;
  }

  return InvError_c;
}

  
/**Function********************************************************************

  Synopsis           [Return the simple type of the formula]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static InvSimpleDataType
ExprReturnSimpleType (
  InvExpr_t * expr)
{
  InvSimpleDataType t1, t2;
  InvExprType type;

  if (expr == NIL(InvExpr_t)) {
    return InvError_c;
  }
  
  type = expr->type;
  
  if (type == InvId_c)
    return InvUniversal_c;
  
  if (type == InvNumConst_c)
    return InvNum_c;
  
  if (type == InvBoolConst_c)
    return InvBool_c;

  t1 = ExprReturnSimpleType (expr->left);
  t2 = ExprReturnSimpleType (expr->right);

  switch (type) {
      case InvNot_c:    /* Boolean negation*/
        return SimpleDataTypeCompatible(t1, InvBool_c);
        break;

      case InvOr_c:      /* Boolean disjunction */
      case InvAnd_c:     /* Boolean conjunction */
      case InvImplies_c: /* Boolean implies (not symmetric) */
      case InvEquiv_c:   /* Boolean equal */
      case InvXor_c:   /* Ashwini: Boolean XOR */
        return SimpleDataTypeCompatible(
          SimpleDataTypeCompatible(t1, InvBool_c),
          SimpleDataTypeCompatible(t2, InvBool_c));
        break;

      case InvGreaterEqual_c:     /* >= */
      case InvLessEqual_c:     /* <= */
      case InvGreater_c:     /* >  */
      case InvLess_c:     /* <  */
        if (SimpleDataTypeCompatible (
          SimpleDataTypeCompatible(t1, InvNum_c),
          SimpleDataTypeCompatible(t2, InvNum_c)) != InvError_c)
          return InvBool_c;
        else
          return InvError_c;
        break;

        /* operators that do not return boolean */
      case InvPlus_c:   /* +  */
      case InvMinus_c:  /* -  */
        return SimpleDataTypeCompatible(
          SimpleDataTypeCompatible(t1, InvNum_c),
          SimpleDataTypeCompatible(t2, InvNum_c));
        break;

      case InvUnaryMinus_c: /* -x */        
        return SimpleDataTypeCompatible(t1, InvNum_c);
        break;
        
      case InvEqual_c:     /* =  */
        /* restriction has it that = is not for boolean value comparison.
           use <=> instead */
        /* this restriction makes an expression with = an atomic formula */

        if (SimpleDataTypeCompatible (
          SimpleDataTypeCompatible(t1, InvNum_c),
          SimpleDataTypeCompatible(t2, InvNum_c)) != InvError_c)
          return InvBool_c;
        else
          return InvError_c;
        break;

      case InvIndex_c:
	 if (t1 == InvUniversal_c &&
	     SimpleDataTypeCompatible(t2, InvNum_c) != InvError_c)
	    return InvUniversal_c;
	 else
	    return InvError_c;
	 break;

      default:
        fprintf(stderr, "unknown type in SimpleTypeChecking.\n");
        exit(1);
  }
  
}

