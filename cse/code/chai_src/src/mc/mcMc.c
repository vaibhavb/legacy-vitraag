/**CFile***********************************************************************

  FileName    [mcMc.c]

  PackageName [mc]

  Synopsis    [Main routines for performing ATL model checking.]

  Description [This file contains routines for performing ATL model checking.
  The main routine is McFormulaEvaluate, which recursively evaluates the
  the states satisfying the sub-formulas.  At this moment, counter-example/
  error trace is not supported. Also, fairness is not supported neither.
  They will be added very soon.]

  SeeAlso     [mcMain.c]

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

#include  "mcInt.h"


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

static boolean McFormulaSemanticCheckAtomicFormula(Atlp_Formula_t * formula, Mdl_Module_t * module, Var_TypeManager_t * typeManager);
static Atm_Expr_t * McAtmExprBuiltFromAtomicFormula(Atlp_Formula_t * formula, Mdl_Module_t * module, Var_TypeManager_t * typeManager);
static boolean McAtmExprCheckDataType(Atm_Expr_t * atmExpr, Var_Type_t * boolTypePtr);
static mdd_t * McAtomicFormulaComputeStates(Sym_Info_t * symInfo, Atlp_Formula_t * formula);
static boolean McFormulaSemanticCheckAgents(Atlp_Formula_t* formula, Mdl_Module_t * module);
static void McAgentReverseAgents(lsList agentList);
static lsList McSimAtomArrayConvertToAgentList(array_t *atomArray);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Checking the semantics of the formula]

  Description        [Given a formula, this function will check its semantic
  against the specified module.  The following will be checked: 1. whether
  the variables appear in the formula also appears in the module; 2. type of
  the formula (a type checking routine similar to the typechecking in atm
  package is used); 3. for formula with path quantifer, check if the
  formula's agents are refering to the correct ATOMS. For the atomic formulas
  (formulas with type Atlp_LT_c, Atlp_LE_c, Atlp_GE_c, Atlp_GT_c, Atlp_EQ_c),
  an Atm_Expr_t is built and stored in the atmExpr field of the formula. This
  will later be used to compute the set of states in Mc_FormulaEvaluate.]

  SideEffects        [It is an error to call this function, if the formula
  has type Atlp_PLUS_c, Atlp_MINUS_c, Atlp_UMINUS or Atlp_NATNUM_c, since
  these formula do not evaluate to boolean type.]

  SeeAlso            [optional]

******************************************************************************/
boolean
Mc_FormulaSemanticCheck(
  Atlp_Formula_t * formula,
  Mdl_Module_t * module,
  Var_TypeManager_t * typeManager)
{

  Atlp_FormulaType type;
  Atlp_Formula_t * left, * right;
  
  if (formula == NIL(Atlp_Formula_t)) {
    return FALSE;
  }
  
  type = Atlp_FormulaReadType (formula);
  left = Atlp_FormulaReadLeft (formula);
  right = Atlp_FormulaReadRight (formula);
    
  if (type == Atlp_ID_c)
    /* check existence and its type */
    return (McFormulaSemanticCheckAtomicFormula(formula, module, typeManager));
  
  if (type == Atlp_TRUE_c || type == Atlp_FALSE_c)
    return TRUE;

  switch (type) {
        
      case Atlp_ESX_c:     /* << agents >> O  */
      case Atlp_ESG_c:     /* << agents >> [] */
      case Atlp_ESF_c:     /* << agents >> <> */
      case Atlp_NAX_c:     /* [[ agents ]] O  */
      case Atlp_NAG_c:     /* [[ agents ]] [] */
      case Atlp_NAF_c:     /* [[ agents ]] <> */
        return (McFormulaSemanticCheckAgents(formula, module) &&
                Mc_FormulaSemanticCheck(left, module, typeManager));
        break;


      case Atlp_ESU_c:     /* << agents >> U */
      case Atlp_ESW_c:     /* << agents >> W */
      case Atlp_NAU_c:     /* [[ agents ]] U */
      case Atlp_NAW_c:     /* [[ agents ]] W */

        return (McFormulaSemanticCheckAgents(formula, module) &&
                Mc_FormulaSemanticCheck(left, module, typeManager)
                &&
                Mc_FormulaSemanticCheck(right, module, typeManager));
        break;
        
      case Atlp_NOT_c:     /* Boolean negation*/
        return (Mc_FormulaSemanticCheck(left, module, typeManager));
        break;

      case Atlp_OR_c:      /* Boolean disjunction */
      case Atlp_AND_c:     /* Boolean conjunction */
      case Atlp_IMPLIES_c: /* Boolean implies (not symmetric) */
      case Atlp_XOR_c:     /* Boolean not equal */
      case Atlp_EQUIV_c:   /* Boolean equal */
        return (Mc_FormulaSemanticCheck(left,  module, typeManager) &&
                Mc_FormulaSemanticCheck(right, module, typeManager));
        break;
        /* these are atomic formulas, ie. they cannot be further broken
           down into subexpressions that have boolean type */
      case Atlp_GE_c:     /* >= */
      case Atlp_LE_c:     /* <= */
      case Atlp_GT_c:     /* >  */
      case Atlp_LT_c:     /* <  */
      case Atlp_EQ_c:     /* =  */
      case Atlp_INDEX_c:  /* Index type */
        return McFormulaSemanticCheckAtomicFormula(formula, module, typeManager);
        break;
        /*
          operators that do not return boolean should not appear at this
          level:
          Atlp_PLUS_c, Atlp_MINUS_c, Atlp_UMINUS_c, Atlp_NATNUM_c.
          */
        
      default:
        fprintf(stderr, "unknown type in SemanticChecking.\n");
        exit(1);
  }
}


/**Function********************************************************************
  
  Synopsis           [Evaluate an Atl formula]
  
  Description        [This function is called by McModelCheck to evaluate the
  ATL formula.]
  
  SideEffects        [required]
  
  SeeAlso            [optional]
  
******************************************************************************/
mdd_t * 
Mc_FormulaEvaluate(
  Sym_Info_t *symInfo,
  Atlp_Formula_t *formula, 
  Mc_VerbosityLevel verbosity)
{
  mdd_t *leftMdd=NIL(mdd_t);
  mdd_t *rightMdd=NIL(mdd_t);
  mdd_t *result=NIL(mdd_t);
  
  mdd_t *formulaStates = Atlp_FormulaObtainStates( formula );
  mdd_t *tmpMdd;
  mdd_manager * mddMgr = Sym_SymInfoReadManager(symInfo);
  Atlp_FormulaType formulaType = Atlp_FormulaReadType( formula );

    
  if ( formulaStates ) {
    return formulaStates;
  }
  
  /* evaluate the atomic formulas */
  switch (formulaType){
      case Atlp_ID_c:
      case Atlp_GE_c:     /* >= */
      case Atlp_LE_c:     /* <= */
      case Atlp_GT_c:     /* >  */
      case Atlp_LT_c:     /* <  */
      case Atlp_EQ_c:     /* =  */
      case Atlp_INDEX_c:

        result = McAtomicFormulaComputeStates(symInfo, formula);
        Atlp_FormulaSetStates( formula, result );
        return mdd_dup(result);
        break;
      default:
        break;
  }

  /* evaluating left child */
  { 
    Atlp_Formula_t *leftChild = Atlp_FormulaReadLeftChild( formula );
    if ( leftChild ) {
      leftMdd =
          Mc_FormulaEvaluate(symInfo, leftChild, verbosity);
      if ( !leftMdd ) {
        return NIL(mdd_t);
      }
    }
  }

  /* evaluating right child */
  {
    Atlp_Formula_t *rightChild = Atlp_FormulaReadRightChild( formula );
    if ( rightChild ) {
      rightMdd = Mc_FormulaEvaluate( symInfo, rightChild, verbosity);
      if ( !rightMdd ) {
        return NIL(mdd_t);
      }
    }
  }

  /* evaluating the formula itself */
  switch ( formulaType ) {

      case Atlp_TRUE_c :
        result = mdd_one( mddMgr ); break;
        
      case Atlp_FALSE_c :
        result = mdd_zero( mddMgr ); break;

      case Atlp_NOT_c :
        result = mdd_not( leftMdd ); break;

      case Atlp_EQUIV_c :
        result = mdd_xnor( leftMdd, rightMdd ); break;

      case Atlp_XOR_c :
        result = mdd_xor( leftMdd, rightMdd ); break;

      case Atlp_IMPLIES_c :
        result = mdd_or( leftMdd, rightMdd, 0, 1 ); break;

      case Atlp_AND_c:
        result = mdd_and( leftMdd, rightMdd, 1, 1 ); break;

      case Atlp_OR_c:
        result = mdd_or( leftMdd, rightMdd, 1, 1 ); break;

      case Atlp_ESX_c:
      {
        lsList agentList = Atlp_FormulaReadAgentList(formula);

        result = McEvaluateEXFormula(symInfo, agentList, leftMdd, verbosity);

        break;
      }
      
      case Atlp_ESU_c:
      {
        array_t *onionRings = array_alloc( mdd_t *, 0 );
        lsList agentList = Atlp_FormulaReadAgentList(formula);
        
        result = McEvaluateEUFormula( symInfo, agentList, leftMdd, rightMdd,
                                       onionRings, verbosity);
        array_free(onionRings);
        break;
      }

      case Atlp_ESW_c:
      {
        array_t *onionRings = array_alloc( mdd_t *, 0 );
        lsList agentList = Atlp_FormulaReadAgentList(formula);
        
        result = McEvaluateEWFormula( symInfo, agentList, leftMdd, rightMdd,
                                      onionRings, verbosity);
        array_free(onionRings);
        break;
      }
  
        
      
      /*
      case Atlp_ESG_c:
      {
        array_t *arrayOfOnionRings = array_alloc( array_t *, 0 );
        lsList agentList = Atlp_FormulaReadAgentList(formula);
                
        result = McEvaluateEGFormula(
          symInfo, agentList, leftMdd, arrayOfOnionRings, verbosity);

        array_free(arrayOfOnionRings);
        break;
      }
      */

      case Atlp_NAU_c:
      {
        array_t *onionRings = array_alloc( mdd_t *, 0 );
        lsList agentList = Atlp_FormulaReadAgentList(formula);

        McAgentReverseAgents(agentList);
        result = McEvaluateEUFormula( symInfo, agentList, leftMdd, rightMdd,
                                       onionRings, verbosity);
        McAgentReverseAgents(agentList);
        array_free(onionRings);
        break;
      }

      case Atlp_NAW_c:
      {
        array_t *onionRings = array_alloc( mdd_t *, 0 );
        lsList agentList = Atlp_FormulaReadAgentList(formula);

        McAgentReverseAgents(agentList);
        result = McEvaluateEWFormula( symInfo, agentList, leftMdd, rightMdd,
                                      onionRings, verbosity);
        McAgentReverseAgents(agentList);
        array_free(onionRings);
        break;
      }
      
      default:
        Main_MochaErrorPrint(
          "Error: Illegal Atl type encountered in evaluating formula\n");
        exit(1);
        break;
  }
  
  
  Atlp_FormulaSetStates( formula, result );
  
  if( leftMdd )
    mdd_free( leftMdd );
  if( rightMdd )
    mdd_free( rightMdd );
  tmpMdd = mdd_dup(result);
  return tmpMdd;
  
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis    [Evaluate states satisfying <<A>>X target]

  Description [Evaluate states satisfying <<A>>X target. Since variables that
  are controlled by the agents in the set A will be existentially quantified
  out, while the remaining will be universally quantified out, this function
  iterates through each atom and calls the functions in the mdd_package
  directly, to avoid any overhead during optimizations such as those in the
  img package. Clearly, a better optimization strategy should be explored.]
  
  SideEffects []

******************************************************************************/
mdd_t *
McEvaluateEXFormula( 
  Sym_Info_t *symInfo,
  lsList agentList,
  mdd_t * target,
  Mc_VerbosityLevel verbosity)
{

  Atlp_Agent_t * agent;
  lsGen gen = lsEnd(agentList);
  mdd_manager *manager = Sym_SymInfoReadManager(symInfo);
  mdd_t * result;
  
  /* rename the variables from unprimed to primed in target */
  mdd_t * toCareSet = mdd_substitute(manager, target,
                                     Sym_SymInfoReadUnprimedIdArray(symInfo),
                                     Sym_SymInfoReadPrimedIdArray(symInfo));
  
  

  /* read the agents one by one, and do quantification */
  while (lsPrev(gen, (lsGeneric *) &agent, LS_NH) != LS_NOMORE) {
    char * agentName;
    array_t *rngvarArray = array_alloc(int, 0);
    mdd_t * conjunct;
    
    agentName = Atlp_AgentReadName(agent);
    if (*agentName == '$') { /* artificial external agent */

      Var_Variable_t * v = (Var_Variable_t *) Atlp_AgentReadAtom(agent);
      int varid;
      boolean flag;
      
      
      flag = Sym_SymInfoLookupPrimedVariableId(symInfo,
                                        (Var_Variable_t *)v, &varid );

      assert (flag);
      
      array_insert_last(int, rngvarArray, varid);
      conjunct = mdd_one(manager);
    } else {
      /* read the atom, and extract all the controlled variables */
      Atm_Atom_t * atom = Atlp_AgentReadAtom(agent);
      lsList varList = Atm_AtomReadControlVarList(atom);
      Var_Variable_t * var;
      int varid;
      lsGen gen;
      
      lsForEachItem(varList, gen, var){
        boolean flag;
        flag = Sym_SymInfoLookupPrimedVariableId(symInfo,
                                            (Var_Variable_t *)var, &varid );

        assert (flag);
        array_insert_last(int, rngvarArray, varid);
      }

      conjunct = mdd_dup(Sym_SymInfoReadConjunctForAtom(symInfo, atom));
    }
    
    if (Atlp_AgentIsAgent(agent)){
      result =
          mdd_and_smooth(manager, conjunct, toCareSet, rngvarArray);
    } else {
      mdd_t * tmp1 = mdd_not(toCareSet);
      mdd_t * tmp2 = mdd_and_smooth(manager, conjunct, tmp1, rngvarArray);

      result = mdd_not(tmp2);

      FREE(tmp1);
      FREE(tmp2);
    }
    
    FREE(conjunct);
    FREE(toCareSet);
    array_free(rngvarArray);
    toCareSet = result;

  }

  if ( verbosity == McVerbosityMax_c ) {
    static int refCount=0;
    Main_MochaErrorPrint("--EX called %d (bdd_size - %d)\n", ++refCount, mdd_size(result) );
  }

  lsFinish(gen);

  return result;
}

/**Function********************************************************************

  Synopsis    [Evaluate states satisfying <<A>>G invariant]

  Description [Evaluate states satisfying <<A>>G invariant. Conceptually,
  this is done by starting with all states marked with the invariant.
  From this initial set, recursively remove states which can not reach
  these invariant states.]


  SideEffects []

******************************************************************************/
mdd_t *
McEvaluateEGFormula( 
  Sym_Info_t * symInfo,
  lsList agentList,
  mdd_t *invariantMdd,
  array_t *onionRingsArrayForDbg,
  Mc_VerbosityLevel verbosity)
{
  int i;
/*
  array_t *onionRings = NIL(array_t);
  array_t *tmpOnionRingsArrayForDbg = NIL(array_t);
*/
  mdd_manager *mddManager = Sym_SymInfoReadManager(symInfo);
  mdd_t *Zmdd;
 
/*
  if ( onionRingsArrayForDbg !=NIL(array_t) ) {
    tmpOnionRingsArrayForDbg = array_alloc( array_t *, 0 );
  }
*/

  Zmdd = mdd_dup( invariantMdd );
  while (TRUE) {

    mdd_t *ZprimeMdd = mdd_dup(Zmdd);
    mdd_t *AAmdd = McEvaluateEXFormula(
      symInfo, agentList, ZprimeMdd, verbosity);

    mdd_free(ZprimeMdd);
    ZprimeMdd = mdd_and(invariantMdd, AAmdd, 1, 1);
    mdd_free(AAmdd);


    if ( mdd_equal( ZprimeMdd, Zmdd ) ) {
      mdd_free( ZprimeMdd );
      break;
    }

    mdd_free( Zmdd );
    Zmdd = ZprimeMdd;
  }
  
    /*
    if ( tmpOnionRingsArrayForDbg ) {
      arrayForEachItem(array_t *, tmpOnionRingsArrayForDbg, i, onionRings ) {
        mdd_array_free( onionRings );
      }
      array_free( tmpOnionRingsArrayForDbg );
      tmpOnionRingsArrayForDbg = array_alloc( array_t *, 0 );
    }
  }

  if ( onionRingsArrayForDbg != NIL(array_t) ) {
    arrayForEachItem( array_t *, tmpOnionRingsArrayForDbg, i, onionRings ) {
      array_insert_last( array_t *, onionRingsArrayForDbg, onionRings );
    }
    array_free( tmpOnionRingsArrayForDbg );
  }
  */


    return Zmdd;
}


/**Function********************************************************************

  Synopsis    [Evaluate states satisfying <<A>>(invariant U target)]

  Description [Evaluate states satisfying <<A>>U(invariant U target).
  Performed by a ``least fixed point computation'' -- start with target AND
  fair, see what can get to this set by a path solely in invariant. If the
  onionRings array is not NIL(array_t *), the ``onion rings'' arising in the
  greatest fixed point computation are stored in the array, starting from the
  target.]

  Comment []
  
  SideEffects []

******************************************************************************/
mdd_t *
McEvaluateEUFormula( 
  Sym_Info_t * symInfo,
  lsList agentList,
  mdd_t *invariantMdd,
  mdd_t *targetMdd,
  array_t *onionRings,
  Mc_VerbosityLevel verbosity)
{

  mdd_t *Zmdd = mdd_dup( targetMdd );

  while (TRUE) { 
    mdd_t *ZprimeMdd = mdd_dup(Zmdd);
    mdd_t *AAmdd = McEvaluateEXFormula(
      symInfo, agentList, ZprimeMdd, verbosity);
    mdd_t *BBmdd = mdd_and(invariantMdd, AAmdd, 1, 1);
    
    mdd_free(ZprimeMdd);
    mdd_free(AAmdd);
    
    ZprimeMdd = mdd_or(targetMdd, BBmdd, 1, 1);
    mdd_free(BBmdd);

    if ( mdd_equal (Zmdd, ZprimeMdd)){
      mdd_free( ZprimeMdd );
      break;
    }

    mdd_free(Zmdd);
    Zmdd = ZprimeMdd;

  }

  return Zmdd;
}


/**Function********************************************************************

  Synopsis    [Evaluate states satisfying <<A>>(invariant W target)]

  Description [Evaluate states satisfying <<A>>(invariant W target).
  Performed by a ``greatest fixed point computation'' -- start with target AND
  fair, see what can get to this set by a path solely in invariant. If the
  onionRings array is not NIL(array_t *), the ``onion rings'' arising in the
  greatest fixed point computation are stored in the array, starting from the
  target.]

  Comment []
  
  SideEffects []

******************************************************************************/
mdd_t *
McEvaluateEWFormula( 
  Sym_Info_t * symInfo,
  lsList agentList,
  mdd_t *invariantMdd,
  mdd_t *targetMdd,
  array_t *onionRings,
  Mc_VerbosityLevel verbosity)
{

  mdd_t *Zmdd = mdd_or( targetMdd, invariantMdd, 1, 1 );

  while (TRUE) { 
    mdd_t *ZprimeMdd = mdd_dup(Zmdd);
    mdd_t *AAmdd = McEvaluateEXFormula(
      symInfo, agentList, ZprimeMdd, verbosity);
    mdd_t *BBmdd = mdd_and(invariantMdd, AAmdd, 1, 1);
    
    mdd_free(ZprimeMdd);
    mdd_free(AAmdd);
    
    ZprimeMdd = mdd_or(targetMdd, BBmdd, 1, 1);
    mdd_free(BBmdd);

    if ( mdd_equal (Zmdd, ZprimeMdd)){
      mdd_free( ZprimeMdd );
      break;
    }

    mdd_free(Zmdd);
    Zmdd = ZprimeMdd;

  }

  return Zmdd;
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Check the Semantics of the Atomic Formula.]

  Description        [This routines checks the semantics of the atomic
  formula. This routine is called by Mc_FormulaSemanticCheck.]

  SideEffects        [A typed atm expression is built and stored in the
  formula.]

  SeeAlso            [Mc_FormulaSemanticCheck.]

******************************************************************************/
static boolean
McFormulaSemanticCheckAtomicFormula(
  Atlp_Formula_t * formula,
  Mdl_Module_t * module,
  Var_TypeManager_t * typeManager)  
{
  /* In fact, if the bdd/atmExpr for this atomic formula is already there,
     there is no need to do semantic check again */
  
  Atm_Expr_t * atmExpr =
      McAtmExprBuiltFromAtomicFormula(formula, module, typeManager);
  Atm_Expr_t *typedAtmExpr;
  
  if (atmExpr == NIL(Atm_Expr_t)) {
    return FALSE;
  }

  /* Create the typed expression */
  if (!Atm_TypeCheckNew(BoolType, &atmExpr)) {
    
    char *s;
    Atm_ExprFreeExpression(atmExpr);
    
    s=Atlp_FormulaConvertToString(formula);
    Main_MochaErrorPrint("Error: Type mismatch in \"%s\"\n", s);
    FREE(s);
    
    return FALSE;
  }

  /* put the computed atm_expr_t inside the formula */
  /* this expression will be freed immediately after the
     mdd for this atomic formula is built */
  /* for cleanliness, the mdd is not computed here, though
     it is possible to do so */
  
  Atlp_FormulaSetAtmExpr(formula, atmExpr);

  
  return TRUE;
}



/**Function********************************************************************

  Synopsis           [Build the atom expression from an atomic  formula]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static Atm_Expr_t *
McAtmExprBuiltFromAtomicFormula(
  Atlp_Formula_t * formula,
  Mdl_Module_t * module,
  Var_TypeManager_t * typeManager)
{
  Atm_Expr_t * leftAtmExpr;
  Atm_Expr_t * rightAtmExpr;
  Atm_ExprType atmExprType;
  Atlp_Formula_t * left = Atlp_FormulaReadLeft(formula);
  Atlp_Formula_t * right = Atlp_FormulaReadRight(formula);
  Atlp_FormulaType type = Atlp_FormulaReadType(formula);

  /* leaf node */

  if (type == Atlp_ID_c) {
    /* this can be an enum constant, or a variable */
    char * name = Atlp_FormulaReadVariableName(formula);
    Var_Variable_t * var =
        Mdl_ModuleReadVariableFromName(name, module);
    
    if (var != NIL(Var_Variable_t)) {
      return Atm_ExprAlloc(Atm_UnPrimedVar_c, (Atm_Expr_t*) var,
                           NIL(Atm_Expr_t));
    } else {
      Var_EnumElement_t * enumConst =
          Var_TypeManagerReadEnumElementFromName(typeManager, name);
      if (enumConst != NIL(Var_EnumElement_t)) {
        return Atm_ExprAlloc(Atm_EnumConst_c, (Atm_Expr_t*) enumConst,
                             NIL(Atm_Expr_t));
      } else {

        Main_MochaErrorPrint("Error: Variable \"%s\" not found in module \"%s\"\n",
                             name, Mdl_ModuleReadName(module));

        return NIL(Atm_Expr_t);
        
      }
    }
  }
  
  if (type == Atlp_NATNUM_c) {
    char * numString = Atlp_FormulaReadVariableName(formula);
    return Atm_ExprAlloc(Atm_NumConst_c,
                         (Atm_Expr_t *) (long) atoi(numString),
                         NIL(Atm_Expr_t));
  }

  if (type == Atlp_TRUE_c) {
    return Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) (long) 1, 
                         NIL(Atm_Expr_t));
  }
  
  if (type ==  Atlp_FALSE_c) {
    return Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) (long) 0, 
                         NIL(Atm_Expr_t));
  }
  

  /* internal node */
  leftAtmExpr = McAtmExprBuiltFromAtomicFormula(left, module, typeManager);

  if (leftAtmExpr == NIL(Atm_Expr_t))
    return NIL(Atm_Expr_t);

  rightAtmExpr = McAtmExprBuiltFromAtomicFormula(right, module, typeManager);

  if (rightAtmExpr == NIL(Atm_Expr_t) && type != Atlp_UMINUS_c) {
    Atm_ExprFreeExpression(leftAtmExpr);
    return NIL(Atm_Expr_t);
  }

  switch (type) {
      case Atlp_GE_c:     /* >= */
        atmExprType = Atm_GreaterEqual_c;
        break;
        
      case Atlp_LE_c:     /* <= */
        atmExprType = Atm_LessEqual_c;
        break;

      case Atlp_GT_c:     /* > */
        atmExprType = Atm_Greater_c;
        break;

      case Atlp_LT_c:     /* < */
        atmExprType = Atm_Less_c;
        break;

      case Atlp_EQ_c:     /* = */
        atmExprType = Atm_Equal_c;
        break;
        
      case Atlp_PLUS_c:   /* +  */
        atmExprType = Atm_Plus_c;
        break;
                
      case Atlp_MINUS_c:  /* -  */
        atmExprType = Atm_Minus_c;
        break;

      case Atlp_UMINUS_c: /* -x */
        atmExprType = Atm_UnaryMinus_c;
        break;

      case Atlp_INDEX_c:
	 atmExprType = Atm_Index_c;
	 break;

      default:
        fprintf(stderr, "Illegal Type in McAtmExprBuiltFromAtomicFormula\n");
        exit(1);
        break;
  }

  return Atm_ExprAlloc(atmExprType, leftAtmExpr, rightAtmExpr);
  
}
  


/**Function********************************************************************

  Synopsis           [Compute the mdd for the atomic formula]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static mdd_t *
McAtomicFormulaComputeStates(
  Sym_Info_t * symInfo,
  Atlp_Formula_t * formula)
  
{
  mdd_t * states;
  Atm_Expr_t * atmExpr = Atlp_FormulaReadAtmExpr(formula);

  assert (symInfo);
  assert (atmExpr);

  states = Sym_ExprBuildMdd(symInfo, atmExpr);  
  Atlp_FormulaSetStates(formula, states);
  
  Atm_ExprFreeExpression(atmExpr);
  Atlp_FormulaSetAtmExpr(formula, NIL(Atm_Expr_t));


  return states;
}

/**Function********************************************************************

  Synopsis           [Check the agents of a formula against a module]

  Description        []

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
McFormulaSemanticCheckAgents(
  Atlp_Formula_t* formula,
  Mdl_Module_t * module)
{
  /* build an atom dependency graph of the module */
  /* this graph will be used throughout for determining the order
     of agents */
  lsList newAgentList;
  lsList agentList = Atlp_FormulaReadAgentList (formula);
  array_t *atomArray, * simAtomArray;
  
  assert (agentList);

  /* Mark all the McAtoms as NOT AGENT. */
  /* Then check if all the agents are actually atoms in the module */
  /* if so, mark the appropriate atoms as ISAGENT */
  if (!(atomArray = McAgentsCheckValid(agentList, module))) {
    return FALSE;
  }

  {
    boolean isUserAtom = TRUE;
    simAtomArray = Sim_SimAtomReturnSortedArray(module, atomArray,
                                                isUserAtom);
  }

  /* get the new agent list */
  newAgentList = McSimAtomArrayConvertToAgentList(simAtomArray);
  assert(newAgentList);

  /* free the old agentList */
  lsDestroy(agentList, Atlp_AgentFree);
  
  /* give it a new one */
  Atlp_FormulaSetAgentList (formula, newAgentList);

  /* Free used arrays */ 
  array_free(atomArray);
  Sim_SimAtomArrayFree(simAtomArray);
  
  return TRUE;
}

/**Function********************************************************************

  Synopsis           [This function reverse the role of the agents.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
McAgentReverseAgents(
  lsList agentList)
{
  Atlp_Agent_t *agent;
  lsGen gen;
      
  lsForEachItem(agentList, gen, agent){
    Atlp_AgentIsAgentSet(agent, !Atlp_AgentIsAgent(agent));
  }
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static lsList
McSimAtomArrayConvertToAgentList(
  array_t *atomArray)
{
  int i, numSimAtoms;
  lsList agentList;
  
  if (!atomArray)
    return NULL;

  agentList = lsCreate();
  numSimAtoms = array_n(atomArray);
  for (i=0; i<numSimAtoms; i++){
    Sim_Atom_t * simAtom = array_fetch(Sim_Atom_t*, atomArray, i);
    Atm_Atom_t * atom;
    Atlp_Agent_t *agent;

    agent = Atlp_AgentAlloc(Sim_SimAtomReadName(simAtom),
                            Sim_SimAtomReadIsUserAtom(simAtom),
                            Sim_SimAtomReadAtom(simAtom));

    lsNewEnd(agentList, (lsGeneric) agent, LS_NH);
  }

  return agentList;

}






