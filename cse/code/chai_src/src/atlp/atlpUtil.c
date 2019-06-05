/**CFile***********************************************************************

  FileName    [atlpUtil.c]

  PackageName [atlp]

  Synopsis    [Routines for manipulating ATL formulas.]

  Description [This file contains routines for accessing the fields of the ATL
  formula data structure, for printing ATL formulas, for reading ATL formulas
  from a file, and for converting formulas to the existential form.]

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

#include  "atlpInt.h"

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
EXTERN FILE* AtlpYyin;


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
/**Enum************************************************************************

  Synopsis    [Data Type of Atlp_Formula_t]

  Description [These are the data type being represented by a atlp_formula_t.
  They are: boolean, number and universal. All well-formed atl formula should
  have boolean type. All operands of arithmetic operators should have number
  type. All variables should have universal type.  Note that this is used in
  checking the well-formedness of the atl formula.  The full type checking is
  done only when the formula is being model-checked against a module, when the
  exact data type of the variables are defined.]

  SeeAlso     [AtlpFormulaIsWellFormed]

******************************************************************************/
typedef enum {
  AtlpBool_c,
  AtlpNum_c,
  AtlpUniversal_c,
  AtlpError_c
} AtlpSimpleDataType;



/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static Atlp_Formula_t * FormulaCreateWithType(Atlp_FormulaType type);
static int FormulaCompare(char *key1, char *key2);
static int FormulaHash(char *key, int modulus);
static Atlp_Formula_t * FormulaHashIntoUniqueTable(Atlp_Formula_t *formula, st_table *uniqueTable);
static Atlp_Formula_t * FormulaConvertToExistentialDAG(Atlp_Formula_t *formula);
static lsList FormulaAgentListDup(lsList agentList);
static boolean FormulaHaveSameAgents(lsList list1, lsList list2);
static AtlpSimpleDataType AtlpSimpleDataTypeCompatible(AtlpSimpleDataType type1, AtlpSimpleDataType type2);
static AtlpSimpleDataType AtlpFormulaReturnSimpleType(Atlp_Formula_t * formula);
static boolean CheckFormulaContainQuantifiers(Atlp_Formula_t* formula);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Allocate an agent with its name only.]

  Description        [This function allocates a new Atlp_Agent_t. Only the
  name is set, while all the other members are set to NIL.]

  SideEffects        [required]

  SeeAlso            [Atlp_AgentAlloc Atlp_AgentFree]

******************************************************************************/
Atlp_Agent_t *
Atlp_AgentAllocWithName(char * name)
{

  return (Atlp_AgentAlloc(name, TRUE, NIL(Atm_Atom_t)));

}

/**Function********************************************************************

  Synopsis           [Allocates an agent.]

  Description        [Allocates an agent structure, and initialize all its
  members. ]

  SideEffects        [required]

  SeeAlso            [Atlp_AgentAllocWithType, Atlp_AgentFree]

******************************************************************************/
Atlp_Agent_t *
Atlp_AgentAlloc(
  char *name,
  boolean isAgent,
  Atm_Atom_t * atom) 
{
  Atlp_Agent_t * agent = ALLOC(Atlp_Agent_t, 1);

  agent-> name = util_strsav(name);
  agent-> isAgent = isAgent;
  agent-> atom = atom;

  return agent;
}

/**Function********************************************************************

  Synopsis           [Free an Agent structure.]

  Description        [It frees an agent structure.]

  SideEffects        [none]

  SeeAlso            [optional]

******************************************************************************/
void
Atlp_AgentFree(
  Atlp_Agent_t * agent) 
{

  FREE (agent-> name);

  /* note atom is not freed */

  FREE(agent);
}

/**Function********************************************************************

  Synopsis           [Reads the name of an agent.]

  Description        [This function returns the name of the agent.]

  SideEffects        [Do not free the name.]

  SeeAlso            [optional]

******************************************************************************/
char *
Atlp_AgentReadName(
  Atlp_Agent_t * agent)
{

  return agent->name;

}


/**Function********************************************************************

  Synopsis           [Is this agent specified in the formula?]

  Description        [This function returns TRUE if the agent specified is
  an agent, FALSE otherwise.]

  SideEffects        [none]

  SeeAlso            [optional]

******************************************************************************/
boolean
Atlp_AgentIsAgent(
  Atlp_Agent_t * agent)
{
  
  return agent->isAgent;

}

/**Function********************************************************************

  Synopsis           [Reads the atom of an agent.]

  Description        [This function returns the atom of the agent.]

  SideEffects        [Do not free the atom.]

  SeeAlso            [optional]

******************************************************************************/
Atm_Atom_t *
Atlp_AgentReadAtom(
  Atlp_Agent_t * agent)
{

  return agent->atom;

}


/**Function********************************************************************

  Synopsis           [Read the left pointer of the formula]

  Description        [This function returns the left pointer of the formula.
  Should not free the pointer.]

  SideEffects        [required]

  SeeAlso            [Atlp_FormulaReadRight]

******************************************************************************/
Atlp_Formula_t *
Atlp_FormulaReadLeft (
  Atlp_Formula_t * formula
  ) 
{
  if (formula != NIL(Atlp_Formula_t)) {
    return (formula->left);
  }
}

/**Function********************************************************************

  Synopsis           [Read the right pointer of the formula]

  Description        [This function returns the left pointer of the formula.
  Should not free the pointer.]

  SideEffects        [required]

  SeeAlso            [Atlp_FormulaReadLeft]

******************************************************************************/
Atlp_Formula_t *
Atlp_FormulaReadRight (
  Atlp_Formula_t * formula
  ) 
{
  if (formula != NIL(Atlp_Formula_t)) {
    return (formula->right);
  }
}



/**Function********************************************************************

  Synopsis           [Allocate a atlp manager.]

  Description        [This function allocates the a new Atlp_Manager_t, and
  initialize the hash table in the manager.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Atlp_Manager_t *
Atlp_ManagerAlloc()
{
  Atlp_Manager_t * manager;
  manager = ALLOC(Atlp_Manager_t, 1);
  
  manager -> nameToFormula = st_init_table(strcmp, st_strhash);
  
  return manager;

}

/**Function********************************************************************

  Synopsis           [Frees a atlp manager]

  Description        [This function frees a Atlp_Manager_t, frees the names of the
  atl formula, and decrements the ref count of the formula. If the reference
  count of the formula comes to zero, the formula is freed.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void Atlp_ManagerFree(
  Atlp_Manager_t * manager
  )
{
  st_generator * gen;
  char * name;
  Atlp_Formula_t *formula;
  
  if (manager != NIL(Atlp_Manager_t)) {
    st_foreach_item (manager -> nameToFormula, gen, &name, (char **) &formula){
      FREE(name);
      AtlpFormulaDecrementRefCount(formula);
    }

    st_free_table(manager->nameToFormula);
    FREE(manager);
  }
}


/**Function********************************************************************

  Synopsis    [Parses a file containing a set of ATL formulas.]

  Description [Parses a file containing a set of semicolon-ending ATL
  formulas, and returns an array of Atlp_Formula_t representing those
  formulas.  If an error is detected while parsing the file, the routine frees
  any allocated memory and returns NULL.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
lsList
Atlp_FileParseFormula(
  Atlp_Manager_t *manager,
  FILE * fp
  )
{
  
  AtlpYyin = fp;

  return (AtlpParse(manager));

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
Atlp_ManagerReadFormulas(
  Atlp_Manager_t *manager)
{
  array_t * formulaArray;
  Atlp_Formula_t * formula;
  char * name;
  int i;
  st_generator * gen;
  
  if (manager == NIL(Atlp_Manager_t)) {
    return NIL(array_t);
  }

  formulaArray = array_alloc(Atlp_Formula_t *,
                             st_count(manager->nameToFormula));

  i = 0;
  st_foreach_item (manager->nameToFormula, gen, &name, (char **) &formula) {
    array_insert(Atlp_Formula_t *, formulaArray, i, formula);
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
Atlp_ManagerReadFormulaNames(
  Atlp_Manager_t *manager)
{
  array_t * nameArray;
  char * name;
  int i;
  st_generator * gen;
  
  if (manager == NIL(Atlp_Manager_t)) {
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
Atlp_Formula_t *
Atlp_FormulaReadByName(
  char * name,
  Atlp_Manager_t * manager)
{
  Atlp_Formula_t * formula = NIL(Atlp_Formula_t);
  
  if (manager == NIL (Atlp_Manager_t) || name == NIL(char))
    return NIL(Atlp_Formula_t);

  st_lookup(manager -> nameToFormula, name, (char **) &formula);

  return formula;
  
}



/**Function********************************************************************

  Synopsis    [Returns formula as a character string.]

  Description [Returns formula as a character string. All subformulas are
  delimited by parenthesis. The syntax used is the same as used by the ATL
  parser.  Does nothing if passed a NULL formula.]

  SideEffects [Atlp_ExprConvertToString]

******************************************************************************/
char *
Atlp_FormulaConvertToString(
  Atlp_Formula_t * formula)
{
  char *s1        = NIL(char);
  char *s2        = NIL(char);
  char *s3        = NIL(char);
  char *tmpString = NIL(char);
  char *result;
  
  if (formula == NIL(Atlp_Formula_t)) {
    return NIL(char);
  }

  /* The formula is an name or a number */
  if (formula->type == Atlp_ID_c || formula->type == Atlp_NATNUM_c){
    return util_strsav((char *) formula->left);
  }

  /* If the formula is a non-leaf, the function is called recursively */
  s1 = Atlp_FormulaConvertToString(formula->left);
  s2 = Atlp_FormulaConvertToString(formula->right);
  s3 = AtlpFormulaAgentsConvertToString (formula -> agentList);
      
  switch(formula->type) {
    /*
     * The cases are listed in rough order of their expected frequency.
     */

    /* boolean connectives */
      case Atlp_OR_c: 
        tmpString = util_strcat3(s1, " | ",s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_AND_c: 
        tmpString = util_strcat3(s1, " & ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_IMPLIES_c: 
        tmpString = util_strcat3(s1, " => ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_XOR_c: 
        tmpString = util_strcat3(s1, " ^ ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_EQUIV_c: 
        tmpString = util_strcat3(s1, " <=> ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_NOT_c: 
        tmpString = util_strcat3("(", s1, ")");
        result    = util_strcat3("!", tmpString, "");
        break;
        /* temporal operators */
      case Atlp_ESX_c:
        tmpString = util_strcat3("<< ", s3, " >> X(");
        result = util_strcat3(tmpString, s1, ")");
        break;
      case Atlp_ESG_c:
        tmpString = util_strcat3("<< ", s3, " >> G(");
        result = util_strcat3(tmpString, s1, ")");
        break;
      case Atlp_ESF_c:
        tmpString = util_strcat3("<< ", s3, " >> F(");
        result = util_strcat3(tmpString, s1, ")");
        break;
      case Atlp_ESU_c:
        result    = util_strcat3("<< ", s3, " >> (");
        tmpString = util_strcat3(result, s1, " U ");
        FREE(result);
        result = util_strcat3(tmpString, s2, ")");
        break;
      case Atlp_ESW_c:
        result    = util_strcat3("<< ", s3, " >> (");
        tmpString = util_strcat3(result, s1, " W ");
        FREE(result);
        result = util_strcat3(tmpString, s2, ")");
        break;
      case Atlp_NAX_c:
        tmpString = util_strcat3("[[ ", s3, " ]] X(");
        result = util_strcat3(tmpString, s1, ")");
        break;
      case Atlp_NAG_c:
        tmpString = util_strcat3("[[ ", s3, " ]] G(");
        result = util_strcat3(tmpString, s1, ")");
        break;
      case Atlp_NAF_c:
        tmpString = util_strcat3("[[ ", s3, " ]] F(");
        result = util_strcat3(tmpString, s1, ")");
        break;
      case Atlp_NAU_c:
        result    = util_strcat3("[[ ", s3, " ]] (");
        tmpString = util_strcat3(result, s1, " U ");
        FREE(result);
        result = util_strcat3(tmpString, s2, ")");
        break;
      case Atlp_NAW_c:
        result    = util_strcat3("[[ ", s3, " ]] (");
        tmpString = util_strcat3(result, s1, " W ");
        FREE(result);
        result = util_strcat3(tmpString, s2, ")");
        break;
      case Atlp_TRUE_c:
        result = util_strsav("TRUE");
        break;
      case Atlp_FALSE_c:
        result = util_strsav("FALSE");
        break;
        /* arithmetic comparisons */
      case Atlp_GT_c:
        tmpString = util_strcat3(s1, " > ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_LT_c:
        tmpString = util_strcat3(s1, " < ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_GE_c:
        tmpString = util_strcat3(s1, " >= ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_LE_c:
        tmpString = util_strcat3(s1, " <= ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_EQ_c:
        tmpString = util_strcat3(s1, " = ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break; 
        /* arithmetic expressions */
      case Atlp_PLUS_c:
        tmpString = util_strcat3(s1, " + ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_MINUS_c:
        tmpString = util_strcat3(s1, " - ", s2);
        result    = util_strcat3("(", tmpString, ")");
        break;
      case Atlp_UMINUS_c:
        tmpString = util_strcat3("(", s1, ")");
        result    = util_strcat3("-", tmpString, "");
        break;
      case Atlp_INDEX_c:
        tmpString = util_strcat3("[", s2, "]");
        result    = util_strcat3(s1, tmpString, "");
        break;

      default:
        Main_MochaErrorPrint(
          "Unexpected type in Atlp_formulaConvertToString\n");
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

  Synopsis           [Converts a list of agents to string.]

  Description        [This function converts a list of agents to string,
  each agent being seperated by a comma.]

  SideEffects        [The user should free the string after use.]

  SeeAlso            [optional]

******************************************************************************/
char *
AtlpFormulaAgentsConvertToString (
  lsList agentList
  )
{
  lsGen gen;
  lsStatus status;
  char *s;
  Atlp_Agent_t *agent;

  if (agentList == NULL)
    return NIL(char);

  gen = lsStart(agentList);
  status = lsNext(gen, (lsGeneric *) &agent, LS_NH);

  if ((status == LS_OK) && (agent->isAgent)){
    s = util_strsav(agent->name);
  } else {
    s = util_strsav("");
  }
  
    
  while ((status = lsNext(gen, (lsGeneric *) &agent, LS_NH))!= LS_NOMORE) {
    char * tmp;

    if (agent->isAgent){
      tmp = util_strcat3(s, ",", agent->name);
      FREE(s);
      s = tmp;
    }
  }

  lsFinish(gen);
  return s;
}


/**Function********************************************************************

  Synopsis    [Prints a formula to a file.]

  Description [Prints a formula to a file. All subformulas are delimited by
  parenthesis. The syntax used is the same as used by the ATL parser.  Does
  nothing if passed a NULL formula.]

  SideEffects []

******************************************************************************/
void
Atlp_FormulaPrint(
  FILE * fp,
  Atlp_Formula_t * formula)
{
  char *tmpString;
  if (formula == NIL(Atlp_Formula_t)) {
    return;
  }
  tmpString = Atlp_FormulaConvertToString(formula);
  (void) fprintf(fp, tmpString);
  FREE(tmpString);
}


/**Function********************************************************************

  Synopsis    [Gets the type of a formula.]

  Description [Gets the type of a formula. See atlp.h for all the types. It is
  an error to call this function on a NULL formula.]

  SideEffects []

  SeeAlso     [atlp.h]

******************************************************************************/
Atlp_FormulaType
Atlp_FormulaReadType(
  Atlp_Formula_t * formula)
{
  return (formula->type);
}


/**Function********************************************************************

  Synopsis    [Reads the variable name of a leaf formula.]

  Description [Reads the variable name of a leaf formula. 
  It is an error to call this function on a non-leaf formula.]

  SideEffects []
  
******************************************************************************/
char *
Atlp_FormulaReadVariableName(
  Atlp_Formula_t * formula)
{
  if (formula->type != Atlp_ID_c && formula->type != Atlp_NATNUM_c){
    fail("Atlp_FormulaReadVariableName() was called on a non-leaf formula.");
  }
  return ((char *)(formula->left));
}

/**Function********************************************************************

  Synopsis    [Gets the left child of a formula.]

  Description [Gets the left child of a formula.  User must not free this
  formula. If a formula is a leaf formula, NIL(Atlp_Formula_t) is returned.]

  SideEffects []

  SeeAlso     [Atlp_FormulaReadRightChild]

******************************************************************************/
Atlp_Formula_t *
Atlp_FormulaReadLeftChild(
  Atlp_Formula_t * formula)
{
  if (formula->type != Atlp_ID_c){
    return (formula->left);
  }
  return NIL(Atlp_Formula_t);
}


/**Function********************************************************************

  Synopsis    [Gets the right child of a formula.]

  Description [Gets the right child of a formula.  User must not free this
  formula. If a formula is a leaf formula, NIL(Atlp_Formula_t) is returned.]

  SideEffects []

  SeeAlso     [Atlp_FormulaReadLeftChild]

******************************************************************************/
Atlp_Formula_t *
Atlp_FormulaReadRightChild(
  Atlp_Formula_t * formula)
{
  if (formula->type != Atlp_ID_c){
    return (formula->right);
  }
  return NIL(Atlp_Formula_t);
}


/**Function********************************************************************

  Synopsis           [Reads the atmexpr in the formula]

  Description        [It returns the atmExpr in the formula.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Atm_Expr_t *
Atlp_FormulaReadAtmExpr(
  Atlp_Formula_t * formula)
{

  return formula->atmExpr;

}


/**Function********************************************************************

  Synopsis    [Gets a copy of the set of states for which this formula is true.]

  Description [Gets a copy of the MDD representing the set of states for which
  this formula is true.  It is the user's responsibility to free this MDD. If
  the set of states has not yet been computed, then a NULL mdd_t is
  returned. It is an error to call this function on a NULL formula.]

  SideEffects []

  SeeAlso     [Atlp_FormulaSetStates]

******************************************************************************/
mdd_t *
Atlp_FormulaObtainStates(
  Atlp_Formula_t * formula)
{
  if (formula->states == NIL(mdd_t)) {
    return NIL(mdd_t);
  }
  else {
    return mdd_dup(formula->states);
  }
}

/**Function********************************************************************

  Synopsis           [Reads the agent list of a formula]

  Description        [It returns the agent list specified in the formula.]

  SideEffects        [Should not free the list]

  SeeAlso            [optional]

******************************************************************************/
lsList
Atlp_FormulaReadAgentList(
  Atlp_Formula_t * formula)
{
  return formula->agentList;

}


/**Function********************************************************************

  Synopsis    [Stores the set of states with the formula.]

  Description [Stores the MDD with the formula (a copy is not made, and hence,
  the caller should not later free this MDD). This MDD is intended to
  represent the set of states for which the formula is true. It is an error to
  call this function on a NULL formula.]

  SideEffects []

  SeeAlso     [Atlp_FormulaObtainStates]

******************************************************************************/
void
Atlp_FormulaSetStates(
  Atlp_Formula_t * formula,
  mdd_t * states)
{
  formula->states = states;
}


/**Function********************************************************************

  Synopsis           [Stores the atmExpr with the formula.]

  Description        [Stores the atm_expr_t with the formula.  This expression
  only occurs with atomic formulas. It is usually built during the
  semantic check of the formula, and it should freed (by calling
  Atm_ExprFreeExpression) once the states for this formula is built. It is an
  error to call this function with a NULL formula.]

  SideEffects        [required]

  SeeAlso            [McFormulaSemanticCheck Atlp_FormulaSetStates]

******************************************************************************/
void
Atlp_FormulaSetAtmExpr(
  Atlp_Formula_t * formula,
  Atm_Expr_t * atmExpr)
{
  formula->atmExpr = atmExpr;
}



/**Function********************************************************************

  Synopsis           [Stores the agentList with the formula]

  Description        [Sets the agentList field of the formula.]

  SideEffects        [It is an error to call this function with an NULL
  formula.]

  SeeAlso            [optional]

******************************************************************************/
void
Atlp_FormulaSetAgentList(
  Atlp_Formula_t * formula,
  lsList agentList)
{
  formula->agentList = agentList;
}


/**Function********************************************************************

  Synopsis    [Sets the debug information of a formula.]

  Description [Sets the debug information of a ATL formula.  The data is
  uninterpreted.  FreeFn is a pointer to a function that takes a formula as
  input and returns void.  FreeFn should free all the memory associated with
  the debug data; it is called when this formula is freed.]

  SideEffects []

  SeeAlso     [Atlp_FormulaReadDebugData]
  
******************************************************************************/
void 
Atlp_FormulaSetDbgInfo(
  Atlp_Formula_t * formula,
  void *data,
  Atlp_DbgInfoFreeFn freeFn)
{
  formula->dbgInfo.data   = data;
  formula->dbgInfo.freeFn = freeFn;
}


/**Function********************************************************************

  Synopsis    [Returns the debug data associated with a formula.]

  Description [Returns the debug data associated with a formula.  This data is
  uninterpreted by the atlp package.]

  SideEffects []
  
  SeeAlso     [Atlp_FormulaSetDbgInfo]
  
******************************************************************************/
void *
Atlp_FormulaReadDebugData(
  Atlp_Formula_t * formula)
{
  return formula->dbgInfo.data;
}


/**Function********************************************************************

  Synopsis    [Returns TRUE if formula was converted, else FALSE.]
  from AX/AG/AU/AF]

  Description [Returns TRUE if formula was converted from a formula of type
  AG, AX, AU, AF, or EF via a call to
  Atlp_FormulaConvertToExistentialFormTree or
  Atlp_FormulaConvertToExistentialFormDAG. Otherwise, returns FALSE.] 
  
  SideEffects []
  
******************************************************************************/
boolean
Atlp_FormulaTestIsConverted(
  Atlp_Formula_t * formula)
{
  return formula->dbgInfo.convertedFlag;
}


/**Function********************************************************************

  Synopsis    [Returns TRUE if formula contains no path quantifiers.]

  Description [Test if a atl formula has any path quantifiers in it;
  if so return false, else true.]

  SideEffects []

******************************************************************************/
boolean
Atlp_FormulaTestIsQuantifierFree(
  Atlp_Formula_t *formula)
{
  /* to do */
  boolean lCheck;
  boolean rCheck;
  Atlp_Formula_t *leftChild;
  Atlp_Formula_t *rightChild;

  if ( formula == NIL( Atlp_Formula_t ) ) {
    return TRUE;
  }

  if ( ( Atlp_FormulaReadType( formula ) == Atlp_ID_c ) ||
       ( Atlp_FormulaReadType( formula ) == Atlp_TRUE_c ) ||
       ( Atlp_FormulaReadType( formula ) == Atlp_FALSE_c ) ) {
    return TRUE;
  }

  if ( ( Atlp_FormulaReadType( formula ) !=  Atlp_OR_c  )   &&
       ( Atlp_FormulaReadType( formula ) !=  Atlp_AND_c  )  &&
       ( Atlp_FormulaReadType( formula ) !=  Atlp_NOT_c  )  &&
       ( Atlp_FormulaReadType( formula ) !=  Atlp_IMPLIES_c  ) &&
       ( Atlp_FormulaReadType( formula ) !=  Atlp_XOR_c  )  &&
       ( Atlp_FormulaReadType( formula ) !=  Atlp_EQUIV_c  )   &&
       ( Atlp_FormulaReadType( formula ) !=  Atlp_TRUE_c  ) &&
       ( Atlp_FormulaReadType( formula ) !=  Atlp_FALSE_c  ) ) {

    return FALSE;
  }

  leftChild = Atlp_FormulaReadLeftChild( formula );
  rightChild = Atlp_FormulaReadRightChild( formula );

  lCheck = Atlp_FormulaTestIsQuantifierFree( leftChild );
  rCheck = Atlp_FormulaTestIsQuantifierFree( rightChild );

  return (lCheck && rCheck);
}



/**Function********************************************************************

  Synopsis    [Returns original formula corresponding to converted formula.]

  SideEffects []
  
******************************************************************************/
Atlp_Formula_t *
Atlp_FormulaReadOriginalFormula(
  Atlp_Formula_t * formula)
{
  return formula->dbgInfo.originalFormula;
}


/**Function********************************************************************

  Synopsis    [Frees a formula if no other formula refers to it as a
  sub-formula.] 

  Description [The function decrements the refCount of the formula. As a
  consequence, if the refCount becomes 0, the formula is freed.]

  SideEffects []

  SeeAlso     [AtlpFormulaFree, AtlpDecrementRefCount]

******************************************************************************/
void
Atlp_FormulaFree(
  Atlp_Formula_t *formula)
{
  AtlpFormulaDecrementRefCount(formula);
}

/**Function********************************************************************

  Synopsis    [Frees States field of Atlp_Formula_t recursively]

  Description []

  SideEffects []

  SeeAlso     [Atlp_FormulaFree]

******************************************************************************/
void
Atlp_FlushStates(
  Atlp_Formula_t * formula)
{
  if (formula != NIL(Atlp_Formula_t)) {

    if (formula->type != Atlp_ID_c){
      if (formula->left  != NIL(Atlp_Formula_t)) {
        Atlp_FlushStates(formula->left);
      }
      if (formula->right != NIL(Atlp_Formula_t)) {
        Atlp_FlushStates(formula->right);
      }
    }

    if (formula->states != NIL(mdd_t)){
      mdd_free(formula->states);
      formula->states = NIL(mdd_t);
    }

    if (formula->dbgInfo.data != NIL(void)){
      (*formula->dbgInfo.freeFn)(formula);
      formula->dbgInfo.data = NIL(void);
    }

  }

}

/**Function********************************************************************


  Synopsis    [Duplicates a ATL formula.]

  Description [Recursively duplicate a formula. Does nothing if the formula
  is NIL. Does not copy mdd for states, dbgInfo.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Atlp_Formula_t *
Atlp_FormulaDup(
  Atlp_Formula_t * formula)
{
  Atlp_Formula_t *result = NIL(Atlp_Formula_t);

  if ( formula == NIL(Atlp_Formula_t)) {
	return NIL(Atlp_Formula_t);
  }

  result = FormulaCreateWithType(formula->type);

  result->states                  = NIL(mdd_t);
  result->refCount                = 1;
  result->dbgInfo.data            = NIL(void);
  result->dbgInfo.freeFn          = (Atlp_DbgInfoFreeFn) NULL;
  result->dbgInfo.convertedFlag   = FALSE;
  result->dbgInfo.originalFormula = NIL(Atlp_Formula_t);

  if ( formula->type != Atlp_ID_c && formula->type != Atlp_NATNUM_c )  {
    result->left                    = Atlp_FormulaDup( formula->left );
    result->right                   = Atlp_FormulaDup( formula->right );
    /* to do */
    result->agentList               = FormulaAgentListDup( formula->agentList);
  }
  else {
	result->left  = (Atlp_Formula_t *) util_strsav((char *)formula->left );
  }

  return result;
}

/**Function********************************************************************

  Synopsis    [Frees an array of ATL formulas.]

  Description [Calls AtlpFormulaDecrementRefCount on each formula in
  formulaArray, and then frees the array itself.] 

  SideEffects []

  SeeAlso     [Atlp_FormulaFree]

******************************************************************************/
void
Atlp_FormulaArrayFree(
  array_t * formulaArray /* of Atlp_Formula_t  */)
{
  if (formulaArray != NIL(array_t)) {
    int i; 
    int numFormulas = array_n(formulaArray);
    
    for (i = 0; i < numFormulas; i++) {
      Atlp_Formula_t *formula = array_fetch(Atlp_Formula_t *, formulaArray, i);
      
      AtlpFormulaDecrementRefCount(formula);
    }
    
    array_free(formulaArray);
  }
}


/**Function********************************************************************

  Synopsis    [Converts a ATL formula to existential form.]

  Description [Converts a ATL formula to existential form.  That is, all
  universal path quantifiers are replaced with the appropriate combination of
  existential quantifiers and Boolean negation.  Also converts "finally"
  operators to "until" operators.<p>

  Returns a new formula that shares absolutely nothing with the original
  formula (not even the strings). Also, the new formula does not have any MDDs
  associated with it.  The "originalFormula" field of the returned formula is
  set to point to the formula passed as an argument.  In addition, if the
  original formula is of type AG, AX, AU, AF, or EF, the "converted flag" is
  set. Returns NULL if called with a NULL formula.]

  SideEffects []

  SeeAlso     [Atlp_FormulaArrayConvertToExistentialForm]

******************************************************************************/
Atlp_Formula_t *
Atlp_FormulaConvertToExistentialForm(
  Atlp_Formula_t * formula)
{
  Atlp_Formula_t *new; 
  char *variableNameCopy, *valueNameCopy;

  if (formula == NIL(Atlp_Formula_t)) {
    return NIL(Atlp_Formula_t);
  }

  /*
   * Recursively convert each subformula.
   */
  
  switch(formula->type) {
      case Atlp_ESF_c:
        /* <<A>>F f --> <<A>> (true U f) */ 
        new = FormulaCreateWithType(Atlp_ESU_c);
        new->agentList = FormulaAgentListDup(formula->agentList);
        new->left  = FormulaCreateWithType(Atlp_TRUE_c);
        new->right = Atlp_FormulaConvertToExistentialForm(formula->left);
        new->dbgInfo.convertedFlag = TRUE;
        break;

      case Atlp_ESG_c:
        /* <<A>>G f --> <<A>> (f W false) */ 
        new = FormulaCreateWithType(Atlp_ESW_c);
        new->agentList = FormulaAgentListDup(formula->agentList);
        new->left = Atlp_FormulaConvertToExistentialForm(formula->left);
        new->right  = FormulaCreateWithType(Atlp_FALSE_c);
        new->dbgInfo.convertedFlag = TRUE;
        break;

      case Atlp_NAX_c:
        /* [[A]]X f --> !(<<A>>X (!f)) */
        new = FormulaCreateWithType(Atlp_NOT_c);
        new->left = FormulaCreateWithType(Atlp_ESX_c);
        new->left->agentList = FormulaAgentListDup(formula->agentList);
        new->left->left = FormulaCreateWithType(Atlp_NOT_c);
        new->left->left->left =
            Atlp_FormulaConvertToExistentialForm(formula->left);
        new->dbgInfo.convertedFlag = TRUE;
        break;
        
      case Atlp_NAG_c:
        /* [[A]]G f --> !(<<A>> (true U !f)) */
        new = FormulaCreateWithType(Atlp_NOT_c);
        new->left = FormulaCreateWithType(Atlp_ESU_c);
        new->left->agentList = FormulaAgentListDup(formula->agentList);
        new->left->left = FormulaCreateWithType(Atlp_TRUE_c);
        new->left->right = FormulaCreateWithType(Atlp_NOT_c);
        new->left->right->left =
            Atlp_FormulaConvertToExistentialForm(formula->left);
        new->dbgInfo.convertedFlag = TRUE;
        break;
        
      case Atlp_NAF_c:
        /* [[A]]F f --> !(<<A>> (!f W false)) */
        new = FormulaCreateWithType(Atlp_NOT_c);
        new->left = FormulaCreateWithType(Atlp_ESW_c);
        new->left->agentList = FormulaAgentListDup(formula->agentList);
        new->left->right = FormulaCreateWithType(Atlp_FALSE_c);
        new->left->left = FormulaCreateWithType(Atlp_NOT_c);
        new->left->left->left =
            Atlp_FormulaConvertToExistentialForm(formula->left);
        new->dbgInfo.convertedFlag = TRUE;
        break;

      case Atlp_ID_c:
        /* Make a copy of the name, and create a new formula. */
        variableNameCopy = util_strsav((char *)(formula->left));
        new = Atlp_FormulaCreate(Atlp_ID_c, variableNameCopy,
                                 NIL(Atlp_Formula_t));
        break;
        
      case Atlp_NATNUM_c:
        /* Make a copy of the value, and create a new formula */
      {
        char* tmp = util_strsav((char *) (formula->left));
        new = Atlp_FormulaCreate(Atlp_NATNUM_c, tmp,
                                 NIL(Atlp_Formula_t));
        break;
      }
      case Atlp_IMPLIES_c:	
      case Atlp_ESX_c:
      case Atlp_ESU_c:
      case Atlp_ESW_c:
      case Atlp_NAU_c:
      case Atlp_NAW_c:
      case Atlp_OR_c:
      case Atlp_AND_c:
      case Atlp_NOT_c:
      case Atlp_XOR_c:
      case Atlp_EQUIV_c:
      case Atlp_TRUE_c:
      case Atlp_FALSE_c:
      case Atlp_PLUS_c:
      case Atlp_MINUS_c:
      case Atlp_UMINUS_c:
      case Atlp_EQ_c:
      case Atlp_GT_c:
      case Atlp_GE_c:
      case Atlp_LT_c:
      case Atlp_LE_c:
      case Atlp_INDEX_c:
        /* These are already in the correct form.  Just convert subformulas. */
        new = FormulaCreateWithType(formula->type);
        new->left = Atlp_FormulaConvertToExistentialForm(formula->left);
        new->right = Atlp_FormulaConvertToExistentialForm(formula->right);
        break;
        
      default: 
        fail("Unexpected type"); 
  }

  new->dbgInfo.originalFormula = formula;
  return new;
}


/**Function********************************************************************

  Synopsis    [Converts an array of ATL formulas to existential form.]

  Description [Calls Atlp_FormulaConvertToExistentialForm on each formula. It
  is okay to call this function with an empty array (in which case an empty
  array is returned), but it is an error to call it with a NULL array.]

  SideEffects []

  SeeAlso     [Atlp_FormulaConvertToExistentialForm]

******************************************************************************/
array_t *
Atlp_FormulaArrayConvertToExistentialFormTree(
  array_t * formulaArray /* of Atlp_Formula_t  */)
{
  int i; 
  int numFormulas = array_n(formulaArray);
  array_t *convertedArray = array_alloc(Atlp_Formula_t *, numFormulas);
  
  for (i = 0; i < numFormulas; i++) {
    Atlp_Formula_t *formula          = array_fetch(Atlp_Formula_t *, formulaArray, i);
    Atlp_Formula_t *convertedFormula = Atlp_FormulaConvertToExistentialForm(formula);

    array_insert(Atlp_Formula_t *, convertedArray, i, convertedFormula);
  }

  return convertedArray;
}

/**Function********************************************************************

  Synopsis    [Converts an array of ATL formulae to a multi-rooted DAG.]

  Description [The function hashes each subformula of a formula (including the
  formula itself) into a uniqueTable. It returns an array containing the roots
  of the multi-rooted DAG thus created by the sharing of the subformulae. It
  is okay to call this function with an empty array (in which case an empty
  array is returned), but it is an error to call it with a NULL array.] 

  SideEffects [A formula in formulaArray might be freed if it had been
  encountered as a subformula of some other formula. Other formulae in
  formulaArray might be present in the returned array. Therefore, the formulae
  in formulaArray should not be freed. Only formulaArray itself should be
  freed.]
  
  SeeAlso     []

******************************************************************************/
array_t *
Atlp_FormulaArrayConvertToDAG(
  array_t *formulaArray)
{
  int i;
  Atlp_Formula_t *formula, *uniqueFormula;
  st_table *uniqueTable = st_init_table(FormulaCompare, FormulaHash);
  int numFormulae = array_n(formulaArray);
  array_t *rootsOfFormulaDAG = array_alloc(Atlp_Formula_t *, numFormulae);
  
  for(i=0; i < numFormulae; i++) {
    formula = array_fetch(Atlp_Formula_t *, formulaArray, i);
    uniqueFormula = FormulaHashIntoUniqueTable(formula, uniqueTable);
    if(uniqueFormula != formula) {
      AtlpFormulaDecrementRefCount(formula);
      AtlpFormulaIncrementRefCount(uniqueFormula);
      array_insert(Atlp_Formula_t *, rootsOfFormulaDAG, i, uniqueFormula);
    }
    else
      array_insert(Atlp_Formula_t *, rootsOfFormulaDAG, i, formula);
  }

  
  /* debug */
  /* 
  {
    /* this prints all the formula in the unique table 

    char *key, * formulaString;
    st_generator *gen;
    
    st_foreach_item(uniqueTable, gen, &key, (char **) &formula) {
      formulaString = Atlp_FormulaConvertToString(formula);
      fprintf(stderr, "%s\n", formulaString);
      FREE(formulaString);
    }

  }

  */
  
  
  st_free_table(uniqueTable);
  return rootsOfFormulaDAG;
}

/**Function********************************************************************

  Synopsis    [Converts a DAG of ATL formulae to a DAG of existential ATL
  formulae.] 

  Description [The function converts a DAG of ATL formulae to a DAG of
  existential ATL formulae. The function recursively converts each subformula
  of each of the formulae in the DAG and remembers the converted formula in
  the field states. It
  is okay to call this function with an empty array (in which case an empty
  array is returned), but it is an error to call it with a NULL array.] 

  SideEffects []
  
  SeeAlso     [FormulaConvertToExistentialDAG]

******************************************************************************/
array_t *
Atlp_FormulaDAGConvertToExistentialFormDAG(
  array_t *formulaDAG)
{
  int i;
  Atlp_Formula_t *formula;
  int numFormulae = array_n(formulaDAG);
  array_t *existentialFormulaDAG = array_alloc(Atlp_Formula_t *, numFormulae);
  
  for(i=0; i<numFormulae; i++) {
    formula = array_fetch(Atlp_Formula_t *, formulaDAG, i);
    array_insert(Atlp_Formula_t *, existentialFormulaDAG, i,
                 FormulaConvertToExistentialDAG(formula));
  }
  for(i=0; i<numFormulae; i++) {
    formula = array_fetch(Atlp_Formula_t *, formulaDAG, i);
    AtlpFormulaSetStatesToNULL(formula);
  }
  return existentialFormulaDAG;
}

/**Function********************************************************************

  Synopsis    [Sets the field states in every subformula of formula to
  NULL.] 

  Description [The function sets the field states in every subformula
  of formula to NULL.]
  
  SideEffects []

******************************************************************************/
void
AtlpFormulaSetStatesToNULL(
  Atlp_Formula_t *formula)
{
  if(formula!=NIL(Atlp_Formula_t)) {
    formula->states = NIL(mdd_t);
    if(formula->type != Atlp_ID_c && formula->type!=Atlp_NATNUM_c) {
      AtlpFormulaSetStatesToNULL(formula->left);
      AtlpFormulaSetStatesToNULL(formula->right);
    }
  }
}

    
/**Function********************************************************************

  Synopsis    [Creates a ATL formula with the specified fields.]

  Description [Allocates a Atlp_Formula_t, and sets the 2 fields given as
  arguments.  If the type is Atlp_ID_c, then the left and right fields 
  should contain a pointer to a variable name and a pointer to a value 
  respectively. Otherwise, the two fields point to subformulas. refCount is
  set to 1. The states field is set to NULL, the converted flag is set to
  FALSE, and the originalFormula field is set to NULL.]

  Comment     []

  SideEffects []
  
  SeeAlso     [AtlpFormulaDecrementRefCount]

******************************************************************************/
Atlp_Formula_t *
Atlp_FormulaCreateWithAgents(
  Atlp_FormulaType  type,
  void * left,
  void * right,
  lsList agentList)
{
  Atlp_Formula_t *formula = ALLOC(Atlp_Formula_t, 1);

  formula->type                    = type;
  switch (type) {
      case Atlp_ID_c:
      case Atlp_NATNUM_c:
        formula->left                  = (Atlp_Formula_t *)left;
        formula->right                 = NIL(Atlp_Formula_t);
        formula->agentList             = NULL;
        break;
        
      case Atlp_ESG_c:
      case Atlp_ESF_c:
      case Atlp_ESX_c:
      case Atlp_ESU_c:
      case Atlp_ESW_c:
      case Atlp_NAG_c:
      case Atlp_NAF_c:
      case Atlp_NAX_c:
      case Atlp_NAU_c:
      case Atlp_NAW_c:
        formula->left                  = left;
        formula->right                 = right;
        formula->agentList             = agentList;
        break;

      default:
        formula->left                  = left;
        formula->right                 = right;
        formula->agentList             = NULL;
        break;
  }
  
  formula->states                  = NIL(mdd_t);
  formula->refCount                = 1;
  formula->atmExpr                 = NIL(Atm_Expr_t);
  formula->dbgInfo.data            = NIL(void);
  formula->dbgInfo.freeFn          = (Atlp_DbgInfoFreeFn) NULL;
  formula->dbgInfo.convertedFlag   = FALSE;
  formula->dbgInfo.originalFormula = NIL(Atlp_Formula_t);

  return formula;
}

/**Function********************************************************************

  Synopsis    [Creates a ATL formula, without the list of agents.]

  Description [Equivalent to Atlp_FormulaCreateWithAgents(type, left, right,
  NIL(lsList)).]
  
  Comment     []

  SideEffects []
  
  SeeAlso     [Atlp_FormulaCreateWithAgents]

******************************************************************************/
Atlp_Formula_t *
Atlp_FormulaCreate(
  Atlp_FormulaType  type,
  void * left,
  void * right)
{
  return (Atlp_FormulaCreateWithAgents(type, left, right, NULL));
}

/**Function********************************************************************

  Synopsis           [Set the isAgent flag of the agent]

  Description        [Sets the isAgent flag.]

  SideEffects        [none]

  SeeAlso            [optional]

******************************************************************************/
void
Atlp_AgentIsAgentSet(
  Atlp_Agent_t * agent,
  boolean value)
{
  agent->isAgent = value;
  
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Increments the reference count of a formula.]

  Description [The function increments the reference count of a formula. If
  the formula is NULL, the function does nothing.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void
AtlpFormulaIncrementRefCount(
  Atlp_Formula_t *formula)
{
  if(formula!=NIL(Atlp_Formula_t)) {
    ++(formula->refCount);
  }
}

/**Function********************************************************************

  Synopsis    [Decrements the reference count of a formula.]

  Description [The function decrements the reference count of formula and if
  the reference count reaches 0, the formula is freed. If the formula is NULL,
  the function does nothing. It is an error to decrement the reference count
  below 0.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void
AtlpFormulaDecrementRefCount(
  Atlp_Formula_t *formula)
{

  if(formula!=NIL(Atlp_Formula_t)) {
    assert(formula->refCount>0);
    (formula->refCount) --;
    
    if(formula->refCount == 0) 
      AtlpFormulaFree(formula);
  }
}


/**Function********************************************************************

  Synopsis    [Adds formula to the atlp manager]

  SideEffects [Manipulates the global variable globalFormulaManager.]

  SeeAlso     [AtlpYyparse]

******************************************************************************/
void
AtlpFormulaAddToManager(
  Atlp_Manager_t *manager,
  char * name,
  Atlp_Formula_t * formula)
{
  Atlp_Formula_t * oldFormula;
  char * tmpname = name;
  
  if (st_delete (manager->nameToFormula, (char **) &tmpname, (char **)
                 &oldFormula)) {
    /* there is a formula with the same name */
    AtlpFormulaDecrementRefCount(oldFormula);
    FREE(tmpname);
  }

  /* add the new formula */
  st_insert (manager->nameToFormula, name, (char *) formula);
  
}




/**Function********************************************************************

  Synopsis    [Frees a ATL formula.]

  Description [The function frees all memory associated with the formula,
  including all MDDs, all character strings (however, does not free
  dbgInfo.originalFormula) and the agentList. It also decrements the reference
  counts of its two chidren. The function does nothing if formula is NULL.]

  SideEffects []

  SeeAlso     [Atlp_FormulaArrayFree]

******************************************************************************/
void
AtlpFormulaFree(
  Atlp_Formula_t * formula)
{
  if (formula != NIL(Atlp_Formula_t)) {

    /*
     * Free any fields that are not NULL.
     */
    
    if (formula->type == Atlp_ID_c || formula->type == Atlp_NATNUM_c){
      FREE(formula->left);
    }
    else {
      if (formula->left  != NIL(Atlp_Formula_t)) {
        AtlpFormulaDecrementRefCount(formula->left);
      }
      if (formula->right != NIL(Atlp_Formula_t)) {
        AtlpFormulaDecrementRefCount(formula->right);
      }

      if (formula->agentList != NULL) {
        lsDestroy (formula->agentList, Atlp_AgentFree);
      }
    } 

    if (formula->states != NIL(mdd_t)){
      mdd_free(formula->states);
    }

    if (formula->atmExpr != NIL(Atm_Expr_t)) {
      Atm_ExprFreeExpression(formula->atmExpr);
    }
    

    if (formula->dbgInfo.data != NIL(void)){
      (*formula->dbgInfo.freeFn)(formula);
    }
    
    FREE(formula);
  }
  
}

/**Function********************************************************************

  Synopsis           [Check if the atl formula is well formed]

  Description        [This is a preliminary checking of well-formedness of the
  formula. This is different from full type checking in that variables are
  considered to have "universal" type.  The full type checking cannot be done
  until the formula is associated with the module.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
AtlpFormulaIsWellFormed(
  Atlp_Formula_t * formula)
{
  AtlpSimpleDataType t;
  
  t = AtlpSimpleDataTypeCompatible(
    AtlpFormulaReturnSimpleType(formula),AtlpBool_c);

  return (t != AtlpError_c);
  
}



/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Creates a ATL formula with just the type set.]

  Description [Calls Atlp_FormulaCreate with type, and all other fields NULL.]

  SideEffects []

  SeeAlso     [Atlp_FormulaCreate]

******************************************************************************/
static Atlp_Formula_t *
FormulaCreateWithType(
  Atlp_FormulaType  type)
{
  return (Atlp_FormulaCreate(
    type, NIL(Atlp_Formula_t), NIL(Atlp_Formula_t)));
}


/**Function********************************************************************

  Synopsis    [The comparison function for the formula unique table.]

  Description [The function takes as parameters two ATL formulae. It compares
  the formula type, the left child and the right child, and returns 0 if they
  match. Otherwise, it returns -1.]

  SideEffects []

  SeeAlso     [FormulaHash]

******************************************************************************/
static int
FormulaCompare(
  char *key1,
  char *key2)
{
  Atlp_Formula_t *formula1 = (Atlp_Formula_t *) key1;
  Atlp_Formula_t *formula2 = (Atlp_Formula_t *) key2;

  assert(key1 != NIL(char));
  assert(key2 != NIL(char));


  if(formula1->type != formula2->type) {
    return -1;
  }

  switch (formula1->type) {
      case Atlp_GT_c:
      case Atlp_LT_c:
      case Atlp_EQ_c:
      case Atlp_GE_c:
      case Atlp_LE_c:
      {
        char* formulaString1 = Atlp_FormulaConvertToString(formula1);
        char* formulaString2 = Atlp_FormulaConvertToString(formula2);
        int flag;
        
        if (strcmp (formulaString1, formulaString2))
          flag = -1;
        else
          flag =  0;

        FREE(formulaString1);
        FREE(formulaString2);

        return flag;
        break;
      }
  
      case Atlp_ID_c:
        if(strcmp((char *) (formula1->left),
                  (char *) (formula2->left)))
          return -1;
        else
          return 0;
        break;

      case Atlp_INDEX_c: 
      {
        int flag = FormulaCompare((char *)formula1->left, (char *) formula2->left);
        if ( flag == 0) {
          return FormulaCompare((char *) formula1->right, (char *) formula2->right);
        } else {
          return flag;
        }
      }

      case Atlp_OR_c:
      case Atlp_AND_c:
      case Atlp_NOT_c:
      case Atlp_IMPLIES_c:
      case Atlp_XOR_c:
      case Atlp_EQUIV_c:
        if(formula1->left != formula2->left)
          return -1;
        if(formula1->right != formula2->right)
          return -1;
        return 0;
        break;

      case Atlp_ESG_c:
      case Atlp_ESF_c:
      case Atlp_ESX_c:
      case Atlp_ESU_c:
      case Atlp_ESW_c:
      case Atlp_NAG_c:
      case Atlp_NAF_c:
      case Atlp_NAX_c:
      case Atlp_NAU_c:
      case Atlp_NAW_c:
        if (!FormulaHaveSameAgents(formula1->agentList, formula2->agentList))
          return -1;
        if(formula1->left != formula2->left)
          return -1;
        if(formula1->right != formula2->right)
          return -1;
        return 0;
        break;

      case Atlp_PLUS_c:
      case Atlp_MINUS_c:
      case Atlp_UMINUS_c:
      case Atlp_NATNUM_c:
        fprintf (stderr, "type does not return boolean\n");
        exit(1);
        break;
        
  }
}

/**Function********************************************************************

  Synopsis    [The hash function for the formula unique table.]

  Description [The function takes as parameter a ATL formula. If the formula
  type is Atlp_ID_c, st_strhash is used with the left child as the key string.
  If type is GT, LT, EQ, GE, LE, then the formula is flattened (by calling
  Atlp_FormulaConvertToSting) and the flattened string is the key string.
  Otherwise, something very similar to st_ptrhash is done.]

  SideEffects []

  SeeAlso     [FormulaCompare]

******************************************************************************/
static int
FormulaHash(
  char *key,
  int modulus)
{
  char *hashString;
  int hashValue;
  Atlp_Formula_t *formula = (Atlp_Formula_t *) key;
  
  switch (formula->type) {
      case Atlp_GT_c:
      case Atlp_LT_c:
      case Atlp_EQ_c:
      case Atlp_GE_c:
      case Atlp_LE_c:
      case Atlp_ID_c:
      case Atlp_INDEX_c:
        hashString = Atlp_FormulaConvertToString(formula); 
        hashValue = st_strhash(hashString, modulus);
        FREE(hashString);
        return hashValue;
        break;

      case Atlp_PLUS_c:
      case Atlp_MINUS_c:
      case Atlp_UMINUS_c:
      case Atlp_NATNUM_c:
        fprintf (stderr, "type does not return boolean\n");
        exit(1);
        break;

      default:
        return (int) ((((unsigned long) formula->left >>2) +
                       ((unsigned long) formula->right >>2)) % modulus);
  }
}


/**Function********************************************************************

  Synopsis    [Hashes the formula into the unique table.]

  Description [The function takes a formula and hashes it and all its
  subformulae into a unique table. It returns the unique formula identical to
  the formula being hashed. The formula returned will have maximum sharing
  with the formulae that are already present in uniqueTable. It returns
  NIL(Atlp_Formula_t) if the formula is NIL(Atlp_Formula_t).]

  SideEffects [If a copy of some subformula of formula is present in
  uniqueTable then the copy is substituted for it and the reference count of
  the subformula is decremented.]

  SeeAlso     [FormulaCompare]

******************************************************************************/
static Atlp_Formula_t *
FormulaHashIntoUniqueTable(
  Atlp_Formula_t *formula,
  st_table *uniqueTable)
{
  Atlp_Formula_t *uniqueFormula, *uniqueLeft, *uniqueRight;

  if(formula == NIL(Atlp_Formula_t))
    return NIL(Atlp_Formula_t);
  if(st_lookup(uniqueTable, (char *) formula, (char **) &uniqueFormula)) {
    return uniqueFormula;
  }
  else {
    switch (formula->type) {
        case Atlp_GT_c:
        case Atlp_LT_c:
        case Atlp_EQ_c:
        case Atlp_GE_c:
        case Atlp_LE_c:
        case Atlp_ID_c:
        case Atlp_INDEX_c:
          st_insert(uniqueTable, (char *) formula, (char *) formula);
          return formula;
          break;
          
        case Atlp_PLUS_c:
        case Atlp_MINUS_c:
        case Atlp_UMINUS_c:
        case Atlp_NATNUM_c:
          fprintf (stderr, "type does not return boolean\n");
          exit(1);
          break;
          
        default:
          uniqueLeft = FormulaHashIntoUniqueTable(formula->left, uniqueTable);
          if(uniqueLeft != NIL(Atlp_Formula_t))
            if(uniqueLeft != formula->left) {
              AtlpFormulaDecrementRefCount(formula->left);
              formula->left = uniqueLeft;
              AtlpFormulaIncrementRefCount(formula->left);
            }
          uniqueRight = FormulaHashIntoUniqueTable(formula->right, uniqueTable);
          if(uniqueRight != NIL(Atlp_Formula_t))
            if(uniqueRight != formula->right) {
              AtlpFormulaDecrementRefCount(formula->right);
              formula->right = uniqueRight;
              AtlpFormulaIncrementRefCount(formula->right);
            }
          if(st_lookup(uniqueTable, (char *) formula, (char **) &uniqueFormula)) {
            return uniqueFormula;
          }
          else {
            st_insert(uniqueTable, (char *) formula, (char *) formula);
            return formula;
          }
    }
  }
}

/**Function********************************************************************

  Synopsis    [Converts the formula tree to a DAG.]

  Description [The function recursively builds an existential DAG for the
  formula. If a particular sub-formula has been encountered before, the
  converted existential sub-formula is pointed to by the field states and it
  is returned without creating a new formula.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static Atlp_Formula_t *
FormulaConvertToExistentialDAG(
  Atlp_Formula_t *formula)
{

  Atlp_Formula_t *new; 
  char *variableNameCopy, *valueNameCopy;

  if(formula==NIL(Atlp_Formula_t))
    return NIL(Atlp_Formula_t);
  
  if(formula->states!=NIL(mdd_t)) {
    Atlp_Formula_t *temp = (Atlp_Formula_t *) (formula->states);

    ++(temp->refCount);
    return temp;
  }
    
  /*
   * Recursively convert each subformula.
   */
  
  switch(formula->type) {
      case Atlp_ESF_c:
        /* <<A>>F f --> (<<A>> true U f)  */
        new = FormulaCreateWithType(Atlp_ESU_c);
        new->agentList = FormulaAgentListDup(formula->agentList);
        new->left  = FormulaCreateWithType(Atlp_TRUE_c);
        new->right = FormulaConvertToExistentialDAG(formula->left);
        new->dbgInfo.convertedFlag = TRUE;
        break;

      case Atlp_ESG_c:
        /* <<A>>G f --> (<<A>> f W false)  */
        new = FormulaCreateWithType(Atlp_ESW_c);
        new->agentList = FormulaAgentListDup(formula->agentList);
        new->left = FormulaConvertToExistentialDAG(formula->left);
        new->right= FormulaCreateWithType(Atlp_FALSE_c);
        new->dbgInfo.convertedFlag = TRUE;
        break;

      case Atlp_NAX_c:
        /* [[A]]X f --> !(<<A>> X (!f)) */
        new = FormulaCreateWithType(Atlp_NOT_c);
        new->left = FormulaCreateWithType(Atlp_ESX_c);
        new->left->agentList = FormulaAgentListDup(formula->agentList);
        new->left->left = FormulaCreateWithType(Atlp_NOT_c);
        new->left->left->left = FormulaConvertToExistentialDAG(formula->left);
        new->dbgInfo.convertedFlag = TRUE;
        break;
        
      case Atlp_NAG_c:
        /* [[A]]Gf --> ![(<<A>> true U !f)] */
        new = FormulaCreateWithType(Atlp_NOT_c);
        new->left = FormulaCreateWithType(Atlp_ESU_c);
        new->left->agentList = FormulaAgentListDup(formula->agentList);
        new->left->left = FormulaCreateWithType(Atlp_TRUE_c);
        new->left->right = FormulaCreateWithType(Atlp_NOT_c);
        new->left->right->left = FormulaConvertToExistentialDAG(formula->left);
        new->dbgInfo.convertedFlag = TRUE;
        break;
        
      case Atlp_NAF_c:
        /* [[A]]F f --> ![<<A>> ((!f) W false)] */
        new = FormulaCreateWithType(Atlp_NOT_c);
        new->left = FormulaCreateWithType(Atlp_ESW_c);
        new->left->agentList = FormulaAgentListDup(formula->agentList);
        new->left->left = FormulaCreateWithType(Atlp_NOT_c);
        new->left->left->left = FormulaConvertToExistentialDAG(formula->left);
        new->left->right = FormulaCreateWithType(Atlp_FALSE_c);
        new->dbgInfo.convertedFlag = TRUE;
        break;

      case Atlp_ID_c:
        /* Make a copy of the name, and create a new formula. */
        variableNameCopy = util_strsav((char *)(formula->left));
        new = Atlp_FormulaCreate(Atlp_ID_c, variableNameCopy,
                                 NIL(Atlp_Formula_t));
        break;
        
      case Atlp_NATNUM_c:
        /* Make a copy of the value, and create a new formula */
      {
        char* tmp = util_strsav((char *) (formula->left));
        new = Atlp_FormulaCreate(Atlp_NATNUM_c, tmp,
                                 NIL(Atlp_Formula_t));
        break;
      }

        
      case Atlp_IMPLIES_c:	
      case Atlp_ESX_c:
      case Atlp_ESU_c:
      case Atlp_ESW_c:
      case Atlp_NAU_c:
      case Atlp_NAW_c:
      case Atlp_OR_c:
      case Atlp_AND_c:
      case Atlp_NOT_c:
      case Atlp_XOR_c:
      case Atlp_EQUIV_c:	
      case Atlp_TRUE_c:
      case Atlp_FALSE_c:
      case Atlp_PLUS_c:
      case Atlp_MINUS_c:
      case Atlp_UMINUS_c:
      case Atlp_EQ_c:
      case Atlp_GT_c:
      case Atlp_GE_c:
      case Atlp_LT_c:
      case Atlp_LE_c:
      case Atlp_INDEX_c:
        
        /* These are already in the correct form.  Just convert subformulas. */
        new = FormulaCreateWithType(formula->type);
        new->left = FormulaConvertToExistentialDAG(formula->left);
        new->right = FormulaConvertToExistentialDAG(formula->right);
        new->agentList = FormulaAgentListDup (formula->agentList);
        break;
        
      default: 
        fail("Unexpected type"); 
  }

  formula->states = (mdd_t *) new; /*using states as pointer to the converted
                                     formula */
  new->dbgInfo.originalFormula = formula;
  return new;
}


/**Function********************************************************************

  Synopsis           [Duplicate a list of agents]

  Description        [This functions takes in a list of agents, and
  make an exact duplicate of it. The duplicated list is then returned.
  If the given list is NULL, a NULL pointer is returned.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static lsList
FormulaAgentListDup (
  lsList agentList
  )
{
  lsList duplist;
  lsGen gen;
  Atlp_Agent_t * agent;
  
  if (agentList == NULL) {
    return NULL;
  }

  duplist = lsCreate();

  lsForEachItem(agentList, gen, agent) {

    Atlp_Agent_t * newAgent;
    
    newAgent = Atlp_AgentAlloc (
      agent->name,
      agent->isAgent,
      agent->atom) ;
      
    lsNewEnd(duplist, (lsGeneric) newAgent, NIL(lsHandle));
  }

  return duplist;
  
}

/**Function********************************************************************

  Synopsis           [Check if two agent lists are the same. ]

  Description        [Compares two sorted lists of agents and test if
  they are the same. It returns TRUE if they are same, FALSE otherwise.
  Note, there should not be any duplication in the list.  Also, the
  two lists can be empty, but not NULL.]

  SideEffects        [required]

  SeeAlso            [optional]
  
******************************************************************************/
static boolean
FormulaHaveSameAgents(
  lsList list1,
  lsList list2
  )
{
  boolean flag;
  lsGen gen1, gen2;
  lsStatus f1, f2;
  Atlp_Agent_t *agent1, *agent2;
  
  /* make sure the two lists are not NULL */
  
  assert (list1);
  assert (list2);
  
  gen1 = lsStart(list1);
  gen2 = lsStart(list2);

  while (flag == TRUE && f1 == LS_OK) {
    f1 = lsNext (gen1, (lsGeneric *) &agent1, NIL(lsHandle));
    f2 = lsNext (gen2, (lsGeneric *) &agent2, NIL(lsHandle));
    
    if (f1 != f2) {
      flag = FALSE;
    } else {
      if (f1 == LS_OK && strcmp(agent1->name, agent2->name)) {
        flag = FALSE;
      }
    }
  }

  lsFinish (gen1);
  lsFinish (gen2);
  return flag;
  
}



/**Function********************************************************************

  Synopsis           [See if the simple type are compatible.]

  Description        [Check if the simple types are compatible.]

  SideEffects        [The infered type is returned.]

  SeeAlso            [optional]

******************************************************************************/
static AtlpSimpleDataType
AtlpSimpleDataTypeCompatible(
  AtlpSimpleDataType type1,
  AtlpSimpleDataType type2
  )
{
  if (type1 == type2) {
    return type1;
  }
  
  if (type1 == AtlpUniversal_c) {
    return type2;
  }

  if (type2 == AtlpUniversal_c) {
    return type1;
  }

  return AtlpError_c;
}

  
/**Function********************************************************************

  Synopsis           [Return the simple type of the formula]

  Description        [It returns the simple type of the given formula.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static AtlpSimpleDataType
AtlpFormulaReturnSimpleType (
  Atlp_Formula_t * formula)
{
  AtlpSimpleDataType t1, t2;
  Atlp_FormulaType type;

  if (formula == NIL(Atlp_Formula_t)) {
    return AtlpError_c;
  }
  
  type = Atlp_FormulaReadType (formula);
  
  if (type == Atlp_ID_c)
    return AtlpUniversal_c;
  
  if (type == Atlp_NATNUM_c)
    return AtlpNum_c;
  
  if (type == Atlp_TRUE_c || type == Atlp_FALSE_c)
    return AtlpBool_c;

  t1 = AtlpFormulaReturnSimpleType (formula->left);
  t2 = AtlpFormulaReturnSimpleType (formula->right);

  switch (type) {
      case Atlp_ESX_c:     /* << agents >> O  */
      case Atlp_ESG_c:     /* << agents >> [] */
      case Atlp_ESF_c:     /* << agents >> <> */
      case Atlp_NAX_c:     /* [[ agents ]] O  */
      case Atlp_NAG_c:     /* [[ agents ]] [] */
      case Atlp_NAF_c:     /* [[ agents ]] <> */
      case Atlp_NOT_c:    /* Boolean negation*/
        return AtlpSimpleDataTypeCompatible(t1, AtlpBool_c);
        break;

      case Atlp_ESU_c:     /* << agents >> U */
      case Atlp_NAU_c:     /* [[ agents ]] U  */
      case Atlp_ESW_c:     /* << agents >> W */
      case Atlp_NAW_c:     /* [[ agents ]] W  */
      case Atlp_OR_c:      /* Boolean disjunction */
      case Atlp_AND_c:     /* Boolean conjunction */
      case Atlp_IMPLIES_c: /* Boolean implies (not symmetric) */
      case Atlp_XOR_c:     /* Boolean not equal */
      case Atlp_EQUIV_c:   /* Boolean equal */
        return AtlpSimpleDataTypeCompatible(
          AtlpSimpleDataTypeCompatible(t1, AtlpBool_c),
          AtlpSimpleDataTypeCompatible(t2, AtlpBool_c));
        break;

      case Atlp_GE_c:     /* >= */
      case Atlp_LE_c:     /* <= */
      case Atlp_GT_c:     /* >  */
      case Atlp_LT_c:     /* <  */
        if (AtlpSimpleDataTypeCompatible (
          AtlpSimpleDataTypeCompatible(t1, AtlpNum_c),
          AtlpSimpleDataTypeCompatible(t2, AtlpNum_c)) != AtlpError_c)
          return AtlpBool_c;
        else
          return AtlpError_c;
        break;

        /* operators that do not return boolean */
      case Atlp_PLUS_c:   /* +  */
      case Atlp_MINUS_c:  /* -  */
        return AtlpSimpleDataTypeCompatible(
          AtlpSimpleDataTypeCompatible(t1, AtlpNum_c),
          AtlpSimpleDataTypeCompatible(t2, AtlpNum_c));
        break;

      case Atlp_UMINUS_c: /* -x */        
        return AtlpSimpleDataTypeCompatible(t1, AtlpNum_c);
        break;
        
      case Atlp_EQ_c:     /* =  */
        /* restriction has it that = is not for boolean value comparison.
           use <=> instead */
        /* this restriction makes an expression with = an atomic formula */

        if (AtlpSimpleDataTypeCompatible (
          AtlpSimpleDataTypeCompatible(t1, AtlpNum_c),
          AtlpSimpleDataTypeCompatible(t2, AtlpNum_c)) != AtlpError_c)
          return AtlpBool_c;
        else
          return AtlpError_c;
        break;

      case Atlp_INDEX_c:
        /* have to check if the index is "quantifier free",
         ie. it does not contain any path quantifiers,
         or temporal operators. */
        
        if (CheckFormulaContainQuantifiers(formula->right)) {
          Main_MochaErrorPrint(
            "Error: Index not well defined.\n");
          return AtlpError_c;
        }
        
        if (t2 != AtlpError_c)
          return t1;
        else
          return AtlpError_c;
        break;
        
      default:
        fprintf(stderr, "unknown type in SimpleTypeChecking.\n");
        exit(1);
  }
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
CheckFormulaContainQuantifiers(
  Atlp_Formula_t* formula)
{
  if (!formula) {
    return FALSE;
  }

  switch (formula->type) {
      case Atlp_ID_c:
      case Atlp_NATNUM_c:
      case Atlp_TRUE_c:
      case Atlp_FALSE_c:
        return FALSE;
        break;

      case Atlp_ESX_c:     /* << agents >> O  */
      case Atlp_ESG_c:     /* << agents >> [] */
      case Atlp_ESF_c:     /* << agents >> <> */
      case Atlp_NAX_c:     /* [[ agents ]] O  */
      case Atlp_NAG_c:     /* [[ agents ]] [] */
      case Atlp_NAF_c:     /* [[ agents ]] <> */
      case Atlp_ESU_c:     /* << agents >> U */
      case Atlp_NAU_c:     /* [[ agents ]] U  */
      case Atlp_ESW_c:     /* << agents >> W */
      case Atlp_NAW_c:     /* [[ agents ]] W  */
        return TRUE;
        break;

        
      default:
        return (CheckFormulaContainQuantifiers(formula->left) ||
                CheckFormulaContainQuantifiers(formula->right));
        break;
  }
  

}










