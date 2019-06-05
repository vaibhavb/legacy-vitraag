/**CFile***********************************************************************

  FileName    [mvfMvf.c]

  PackageName [mvf]

  Synopsis    [Routines to create, manipulate and free multi-valued functions.]

  SeeAlso     [mvf.h]

  Author      [Tom Shiple]

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

#include  "mvfInt.h"

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Initializes the mvf package.]

  SideEffects []

  SeeAlso     [Mvf_End]

******************************************************************************/
void
Mvf_Init(
   )
{
}


/**Function********************************************************************

  Synopsis    [Ends the mvf package.]

  SideEffects []

  SeeAlso     [Mvf_Init]

******************************************************************************/
void
Mvf_End(
   )
{
}


/**Function********************************************************************

  Synopsis    [Allocates a multi-valued function of n components.]

  Description [Allocates a multi-valued function of n components.  Each
  component is initialized to the zero MDD.]

  SideEffects []

  SeeAlso     [Mvf_FunctionAddMintermsToComponent Mvf_FunctionCreateFromVariable]

******************************************************************************/
Mvf_Function_t *
Mvf_FunctionAlloc(
  mdd_manager *mddManager,
  int n)
{
  int      i;
  array_t *mddArray = array_alloc(mdd_t *, n);

  for (i = 0; i < n; i++) {
    array_insert(mdd_t *, mddArray, i, mdd_zero(mddManager));
  }
  return ((Mvf_Function_t *) mddArray);
}


/**Function********************************************************************

  Synopsis    [Adds a set of minterms to the ith component of a function.]

  Description [Adds a set of minterms, represented by the onset of an MDD g,
  to the onset of the ith component of a function.  The MDD g is not freed.]

  SideEffects []

  SeeAlso     [Mvf_FunctionAlloc]

******************************************************************************/
void
Mvf_FunctionAddMintermsToComponent(
  Mvf_Function_t *function,
  int             i,
  mdd_t          *g)
{
  mdd_t   *oldComponent;
  mdd_t   *newComponent;
  array_t *mddArray = (array_t *) function;

  assert((i >= 0) && (i < array_n(mddArray)));

  oldComponent = array_fetch(mdd_t *, mddArray, i);
  newComponent = mdd_or(oldComponent, g, 1, 1);
  mdd_free(oldComponent);
  array_insert(mdd_t *, mddArray, i, newComponent);
}


/**Function********************************************************************

  Synopsis [Returns the MDD representation of the relation (var == function).]

  Description [Given a variable x, represented by MDD var "mddId", and a
  function f:y->z, represented by "function", where x and z take the same
  number of values, returns the MDD for a (binary) function F(x,y) such that
  F(x,y) = 1 iff x = f(y).  In the binary case it reduces to F(x,y) = x XNOR
  f(y).  Intuitively it describes a function of multi-valued variables by the
  characteristic function of its input-output relation.]

  SideEffects []

******************************************************************************/
mdd_t *
Mvf_FunctionBuildRelationWithVariable(
  Mvf_Function_t *function,
  int mddId)
{
  int          i;
  mvar_type    mddVar;
  array_t     *mddArray     = (array_t *) function;
  mdd_manager *mddManager   = Mvf_FunctionReadMddManager(function);
  mdd_t       *sumOfFactors = mdd_zero(mddManager);
  
  mddVar = array_fetch(mvar_type, mdd_ret_mvar_list(mddManager), mddId);
  assert(mddVar.values == Mvf_FunctionReadNumComponents(function));

  for (i = 0; i < array_n(mddArray); i++) {
    mdd_t *varLiteral;
    mdd_t *factor;
    mdd_t *tmp;
    mdd_t *fComponent = array_fetch(mdd_t *, mddArray, i);

    varLiteral = mdd_eq_c(mddManager, mddId, i);
    factor = mdd_and(fComponent, varLiteral, 1, 1);
    mdd_free(varLiteral);

    /* Take the or of the sumOfFactors so far and the new factor */
    tmp = mdd_or(sumOfFactors, factor, 1, 1);
    mdd_free(factor);
    mdd_free(sumOfFactors);
    sumOfFactors = tmp;
  } 

  return sumOfFactors;
} 


/**Function********************************************************************

  Synopsis    [Returns the number of components of a multi-valued function.]

  Description [Returns the number of components of a multi-valued function.
  This is the same number as the value of the parameter passed to
  Mvf_FunctionAlloc.]

  SideEffects []

  SeeAlso     [Mvf_FunctionAlloc]

******************************************************************************/
int
Mvf_FunctionReadNumComponents(
  Mvf_Function_t *function)
{
  return (array_n((array_t *) function));
}


/**Function********************************************************************

  Synopsis    [Returns the MDD manager of a multi-valued function.]

  Description [Returns the MDD manager of a multi-valued function.  This
  procedure assumes that the function has at least one component.]

  SideEffects []

  SeeAlso     [Mvf_FunctionAlloc]

******************************************************************************/
mdd_manager *
Mvf_FunctionReadMddManager(
  Mvf_Function_t *function)
{
  mdd_t *component = array_fetch(mdd_t *, (array_t *) function, 0);
  
  return (mdd_get_manager(component));
}


/**Function********************************************************************

  Synopsis    [Duplicates a multi-valued output function.]

  Description [Returns a new multi-valued output function, whose constituent
  MDDs have been duplicated. Assumes that function is not NULL.] 

  SideEffects []

  SeeAlso     [Mvf_FunctionFree]

******************************************************************************/
Mvf_Function_t *
Mvf_FunctionDuplicate(
  Mvf_Function_t *function)
{
  return ((Mvf_Function_t *) mdd_array_duplicate((array_t *) function));
}


/**Function********************************************************************

  Synopsis    [Frees a multi-valued output function.]

  Description [Frees a multi-valued output function. Does nothing if
  function is NULL.]
  
  SideEffects []

  SeeAlso     [Mvf_FunctionAlloc]

******************************************************************************/
void
Mvf_FunctionFree(
  Mvf_Function_t *function)
{
  mdd_array_free((array_t *) function);
}


/**Function********************************************************************

  Synopsis    [Frees an array of multi-valued output functions.]

  Description [Frees an array of multi-valued output functions.  Does nothing
  if functionArray is NULL.]

  SideEffects []

  SeeAlso     [Mvf_FunctionFree]

******************************************************************************/
void
Mvf_FunctionArrayFree(
  array_t *functionArray)
{
  int i;

  if (functionArray != NIL(array_t)) {
    for (i = 0; i < array_n(functionArray); i++) {
      Mvf_Function_t *function = array_fetch(Mvf_Function_t *, functionArray, i);
      Mvf_FunctionFree(function);
    }
    array_free(functionArray);
  }
}

/**Function********************************************************************

  Synopsis [Returns a copy of the ith component of a multi-valued function.]

  Synopsis [Returns a copy of the MDD giving the minterms for which a
  multi-valued function evaluates to its ith value.]

  SideEffects []

  SeeAlso     [Mvf_FunctionAlloc Mvf_FunctionCreateFromVariable]

******************************************************************************/
mdd_t *
Mvf_FunctionObtainComponent(
  Mvf_Function_t *function,
  int i)
{
  mdd_t *component = array_fetch(mdd_t *, (array_t *) function, i);
  return (mdd_dup(component));
}

/**Function********************************************************************

  Synopsis [Returns the ith component of a multi-valued function.]

  Synopsis [Returns the MDD giving the minterms for which a
  multi-valued function evaluates to its ith value. The user should not free
  this MDD.]

  SideEffects []

  SeeAlso     [Mvf_FunctionObtainComponent Mvf_FunctionAlloc
  Mvf_FunctionCreateFromVariable]

******************************************************************************/
mdd_t *
Mvf_FunctionReadComponent(
  Mvf_Function_t *function,
  int i)
{
  mdd_t *component = array_fetch(mdd_t *, (array_t *) function, i);
  return (component);
}


/**Function********************************************************************

  Synopsis [Creates the multi-output function for a variable.]

  Description [Given a variable, creates a function with as many components as
  values of the variable.  The ith component of the function is true exactly
  when the variable is equal to the ith value (i.e. fi(x) = (x==i), where x is
  the variable specified by mddId).  For the case where x is binary valued,
  the result is \[!x, x\]. Assumes that mddId is non-negative.]

  SideEffects []

  SeeAlso [Mvf_FunctionAlloc]

******************************************************************************/
Mvf_Function_t *
Mvf_FunctionCreateFromVariable(
  mdd_manager *mddManager,
  int mddId)
{
  int        i;
  array_t   *mvar_list = mdd_ret_mvar_list(mddManager);
  mvar_type  varInfo   = array_fetch(mvar_type, mvar_list, mddId);
  array_t   *result    = array_alloc(mdd_t *, varInfo.values);

  assert(mddId >= 0);

  for(i = 0; i < varInfo.values; i++) {
    mdd_t *literal;

    literal = mdd_eq_c(mddManager, mddId, i);
    array_insert(mdd_t *, result, i, literal);
  } 

  return ((Mvf_Function_t *) result);
} 


/**Function********************************************************************

  Synopsis [Substitutes a set of variables by a set of functions in a function.]

  Description [Given a multi-valued function f, an array of variables
  x1,...,xk, and an array of multi-valued functions g1,...,gk, iteratively
  calls Mvf_MddComposeWithFunction to substitute each xi by gi.  The
  parameters of the ith call to Mvf_MddComposeWithFunction are the result of
  composing the first i-1 variables, xi, and gi.  The multi-valued function gi
  must not depend on xi.]

  SideEffects []

  SeeAlso     [Mvf_MddComposeWithFunction]

******************************************************************************/
Mvf_Function_t *
Mvf_FunctionComposeWithFunctionArray(
  Mvf_Function_t *f,
  array_t        *mddIdArray /* of int */,   
  array_t        *functionArray /* of Mvf_Function_t* */)
{
  Mvf_Function_t *result;
  int i;

  assert(array_n(mddIdArray) == array_n(functionArray));

  /* Make an initial copy of the function */
  result = Mvf_FunctionDuplicate(f);

  for(i = 0; i < array_n(mddIdArray); i++) {
    Mvf_Function_t *tmp;
    int             mddId = array_fetch(int, mddIdArray, i);
    Mvf_Function_t *g     = array_fetch(Mvf_Function_t *, functionArray, i);
    
    tmp = Mvf_FunctionComposeWithFunction(result, mddId, g);

    Mvf_FunctionFree(result);
    result = tmp;
  } 

  return result;
}


/**Function********************************************************************

  Synopsis [Substitutes a variable by a function in a function.]

  Description [Given a multi-valued function f, a variable x (mddId), and a
  multi-valued function g, the procedure replaces every appearance of x in f
  by function g.  That is, if the function f is f(..., x, ...), then the
  result is f(..., g(), ...).  The number of values that x can take and the
  number of components of g must be equal. The algorithm first computes the
  sum of factors (x==i)*gi for every value i in the domain of x, where gi is
  the ith component of g.  Then, for each component fi of f, the sum of
  factors is conjuncted with fi, and x is existentially quantified. The
  function g must not depend on x.  The result depends on all the variables of
  g and all the variables of f, except for x.]

  SideEffects []

  SeeAlso     [Mvf_MddComposeWithFunction]

******************************************************************************/
Mvf_Function_t *
Mvf_FunctionComposeWithFunction(
  Mvf_Function_t *f,
  int             mddId,
  Mvf_Function_t *g)
{
  mdd_t   *sumOfFactors;
  array_t *result;
  array_t *values;
  int      i;
  mdd_manager *mddManager = Mvf_FunctionReadMddManager(f);

  /* Allocate array to hold values for the literal and the result */
  result = array_alloc(mdd_t *, array_n(f));

  /*
   * Create the sum of factors (x==i * gi). This function will verify that the
   * domain of x and the range of g have the same cardinality.
   */
  sumOfFactors = Mvf_FunctionBuildRelationWithVariable(g, mddId);

  /* 
   * Result is an array of mdd_t * , result[i] = Exists(x) (f[i] * sumOfFactors)
   * The array values holds now the index of the variable to smooth out.
   */
  values = array_alloc(int, 1);
  array_insert(int, values, 0, mddId);
  for(i = 0; i < array_n(f); i++) {
    mdd_t *functionUnit = array_fetch(mdd_t *, f, i);
    mdd_t *tmp          = mdd_and_smooth(mddManager, functionUnit, sumOfFactors, values);

    array_insert(mdd_t *, result, i, tmp);
  } 

  /* Clean up */
  array_free(values);
  mdd_free(sumOfFactors);

  return ((Mvf_Function_t *) result);
}


/**Function********************************************************************

  Synopsis    [Substitutes a variable by a function in an mdd_t *.]

  Description [Given a binary-valued function f, a variable x, and a
  multi-valued function g, the procedure replaces every appearance of x in f
  by function g.  That is, if the function f is f(..., x, ...), then the
  result is f(..., g(), ...). The number of values that x can take and the
  number of components of g must be equal. The algorithm first computes the
  sum of factors (x==i)*gi for every value i in the domain of x, where gi is
  the ith component of g, then conjuncts this with f, and finally
  existentially quantifies x.  The function g must not depend on x.  The
  result depends on all the variables of g and all the variables of f, except
  for x.]

  SideEffects []

******************************************************************************/
mdd_t *
Mvf_MddComposeWithFunction(
  mdd_t          *f,
  int             mddId,
  Mvf_Function_t *g)
{
  mdd_t       *sumOfFactors;
  mdd_t       *result;
  array_t     *values;
  mdd_manager *mddManager = mdd_get_manager(f);
  
  /*
   * Create the sum of factors (x==i * gi). This function will verify that the
   * domain of x and the range of g have the same cardinality.
   */
  sumOfFactors = Mvf_FunctionBuildRelationWithVariable(g, mddId);

  /* Result is an mdd_t * , result = Exists(x) (f * sumOfFactors) */
  values = array_alloc(int, 1);
  array_insert(int, values, 0, mddId);
  result = mdd_and_smooth(mddManager, f, sumOfFactors, values);

  /* Clean up */
  array_free(values);
  mdd_free(sumOfFactors);

  return result;
} 


/**Function********************************************************************

  Synopsis [Returns true if a multi-valued function is deterministic, else
  false.]

  Description [Returns true if a multi-valued function is deterministic, else
  false.  A function is deterministic if, for every minterm over the input
  space of the function, the function takes at most one value. The complexity
  of this procedure is linear in the number of values the function can
  take.]

  SideEffects []

  SeeAlso     [Mvf_FunctionTestIsCompletelySpecified Mvf_FunctionTestIsWellFormed]

******************************************************************************/
boolean
Mvf_FunctionTestIsDeterministic(
  Mvf_Function_t *function)
{
  int      i;
  array_t *mddArray      = (array_t *) function;
  int      numComponents = array_n(mddArray);
  mdd_t   *sum;

  if (numComponents == 0) {
    return TRUE;
  }
  
  sum = mdd_dup(array_fetch(mdd_t *, mddArray, 0));
  
  for (i = 1; i < numComponents; i++) {
    mdd_t *temp         = sum;
    mdd_t *ithComponent = array_fetch(mdd_t *, mddArray, i);
    mdd_t *intersection = mdd_and(ithComponent, sum, 1, 1);
    boolean intersectionIsEmpty = mdd_is_tautology(intersection, 0);
    
    mdd_free(intersection);

    /* If the intersection is not empty, then return FALSE. */
    if (!intersectionIsEmpty) {
      mdd_free(sum);
      return FALSE;
    }

    sum = mdd_or(temp, ithComponent, 1, 1);
    mdd_free(temp);
  }
  mdd_free(sum);
  
  /* The components are pairwise disjoint. */
  return TRUE;
}


/**Function********************************************************************

  Synopsis [Returns true if a multi-valued function is completely specified, else
  false.]

  Description [Returns true if a multi-valued function is completely
  specified, else false.  A function is completely specified if, for every
  minterm over the input space of the function, the function takes at least
  one value. The complexity of this procedure is linear in the number of
  values the function can take.]

  SideEffects []

  SeeAlso     [Mvf_FunctionTestIsDeterministic Mvf_FunctionTestIsWellFormed]

******************************************************************************/
boolean
Mvf_FunctionTestIsCompletelySpecified(
  Mvf_Function_t *function)
{
  mdd_t   *sum            = Mvf_FunctionComputeDomain(function);
  boolean  sumIsTautology = mdd_is_tautology(sum, 1);

  mdd_free(sum);
  return sumIsTautology;
}


/**Function********************************************************************

  Synopsis [Returns true if a multi-valued function is constant, else
  false.]

  Description [Returns true if a multi-valued function is constant, else
  false.  A function is constant if exactly one component is the tautology,
  and the remaining components are zero.  If the function is a constant, then
  "value" is set to the constant value of the function. The complexity of this
  procedure is linear in the number of values the function can take.]

  SideEffects []

  SeeAlso     [Mvf_FunctionTestIsNonDeterministicConstant]

******************************************************************************/
boolean
Mvf_FunctionTestIsConstant(
  Mvf_Function_t *function,
  int            *constantValue /* return value */ )
{
  int      i;
  array_t *mddArray      = (array_t *) function;
  int      numComponents = array_n(mddArray);
  int      numTautComps  = 0;

  for (i = 0; i < numComponents; i++) {
    mdd_t *ithComponent = array_fetch(mdd_t *, mddArray, i);

    if (mdd_is_tautology(ithComponent, 1)) {
      *constantValue = i;
      numTautComps++;
    }
    else if (!mdd_is_tautology(ithComponent, 0)) {
      /* this component is not 1 nor 0, so function can't be a constant */
      return FALSE;
    }
    /* else, must be the zero function */
  }

  return (numTautComps == 1);
}


/**Function********************************************************************

  Synopsis [Returns true if a multi-valued function is a non-deterministic
  constant, else false.]

  Description [Returns true if a multi-valued function is a non-deterministic
  constant, else false.  A function is a non-deterministic constant if more
  than one component is the tautology, and the remaining components are zero.
  The complexity of this procedure is linear in the number of values the
  function can take.]

  SideEffects []

  SeeAlso     [Mvf_FunctionTestIsConstant]

******************************************************************************/
boolean
Mvf_FunctionTestIsNonDeterministicConstant(
  Mvf_Function_t *function)
{
  int      i;
  array_t *mddArray      = (array_t *) function;
  int      numComponents = array_n(mddArray);
  int      numTautComps  = 0;

  for (i = 0; i < numComponents; i++) {
    mdd_t *ithComponent = array_fetch(mdd_t *, mddArray, i);

    if (mdd_is_tautology(ithComponent, 1)) {
      numTautComps++;
    }
    else if (!mdd_is_tautology(ithComponent, 0)) {
      /* this component is not 1 nor 0, so function can't be a non-det constant */
      return FALSE;
    }
    /* else, must be the zero function */
  }

  return (numTautComps > 1);
}


/**Function********************************************************************

  Synopsis    [Computes the domain of a multi-valued function.]

  Description [Returns an MDD representing the set of minterms which turn on
  some component of a function.  In other words, returns the union of the
  onsets of the components.  The domain is the tautology if and only if the
  function is completely specified.]

  SideEffects []

  SeeAlso     [Mvf_FunctionTestIsCompletelySpecified]

******************************************************************************/
mdd_t *
Mvf_FunctionComputeDomain(
  Mvf_Function_t *function)
{
  int          i;
  array_t     *mddArray      = (array_t *) function;
  int          numComponents = array_n(mddArray);
  mdd_manager *mddManager    = Mvf_FunctionReadMddManager(function);
  mdd_t       *sum           = mdd_zero(mddManager);

  for (i = 0; i < numComponents; i++) {
    mdd_t *ithComponent = array_fetch(mdd_t *, mddArray, i);
    mdd_t *temp = sum;
    
    sum = mdd_or(temp, ithComponent, 1, 1);
    mdd_free(temp);
  }
  return sum;
}


/**Function********************************************************************

  Synopsis    [Returns true if a function is deterministic and completely
  specified, else false.]

  SideEffects []

  SeeAlso     [Mvf_FunctionTestIsDeterministic
  Mvf_FunctionTestIsCompletelySpecified]

******************************************************************************/
boolean
Mvf_FunctionTestIsWellFormed(
  Mvf_Function_t *function)
{
  return (Mvf_FunctionTestIsDeterministic(function) 
          && Mvf_FunctionTestIsCompletelySpecified(function));
}


/**Function********************************************************************

  Synopsis    [Returns true if two multi-valued functions are equal, else false.]

  Description [Returns true if two multi-valued functions are equal, else
  false.  Two functions are equal if they have the same number of components,
  and the ith component of one is equal to the ith component of the other.]

  SideEffects []

******************************************************************************/
boolean
Mvf_FunctionTestIsEqualToFunction(
  Mvf_Function_t *function1,
  Mvf_Function_t *function2)
{
  int      i;
  array_t *mddArray1     = (array_t *) function1;
  array_t *mddArray2     = (array_t *) function2;
  int      numComponents = array_n(mddArray1);

  if (numComponents != array_n(mddArray2)) {
    return FALSE;
  }

  for (i = 0; i < numComponents; i++) {
    mdd_t *mdd1 = array_fetch(mdd_t *, mddArray1, i);
    mdd_t *mdd2 = array_fetch(mdd_t *, mddArray2, i);
    if (!mdd_equal(mdd1, mdd2)) {
      return FALSE;
    }
  }

  return TRUE;
}


/**Function********************************************************************

  Synopsis    [Returns the set of minterms on which two functions agree.]

  Description [Returns the set of minterms on which two functions agree.  For
  f = \[f1, f2, ..., fn\] and g = \[g1, g2, ..., gn\], the returned set is:
  AND(i = 1, ..., n) (fi XNOR gi).  For the special case where f and g are
  binary valued, this function computes (f XNOR g).  It is an error if the two
  functions have a different number of components.]

  SideEffects []

******************************************************************************/
mdd_t *
Mvf_FunctionsComputeEquivalentSet(
  Mvf_Function_t *function1,
  Mvf_Function_t *function2)
{
  int          i;
  array_t     *mddArray1     = (array_t *) function1;
  array_t     *mddArray2     = (array_t *) function2;
  int          numComponents = array_n(mddArray1);
  mdd_manager *mddManager    = Mvf_FunctionReadMddManager(function1);
  mdd_t       *product       = mdd_one(mddManager);

  assert(numComponents == array_n(mddArray2));
  
  for (i = 0; i < numComponents; i++) {
    mdd_t *mdd1 = array_fetch(mdd_t *, mddArray1, i);
    mdd_t *mdd2 = array_fetch(mdd_t *, mddArray2, i);
    mdd_t *xnor = mdd_xnor(mdd1, mdd2);
    mdd_t *temp = product;

    product = mdd_and(temp, xnor, 1, 1);
    mdd_free(temp);
    mdd_free(xnor);
  }

  return product;
}

  
/**Function********************************************************************

  Synopsis [Calls bdd_cofactor on each component of a multi-valued function.]

  Description [Calls bdd_cofactor on each component of a multi-valued
  function, cofactoring with respect to wrtMDD. Returns the cofactored
  function. It is an error to call this function with a multi-valued
  function that contains null MDDs or with a null wrtMdd.]

  SideEffects []

******************************************************************************/
Mvf_Function_t *
Mvf_FunctionCofactor(
  Mvf_Function_t * function,
  mdd_t          * wrtMdd)
{
  int      i;
  array_t *mddArray      = (array_t *) function;
  int      numComponents = array_n(mddArray);
  array_t *newFunction   = array_alloc(mdd_t *, numComponents);
  
  for(i = 0; i < numComponents; i++) {
    mdd_t *component = array_fetch(mdd_t *, mddArray, i);
    mdd_t *cofactor  = bdd_cofactor(component, wrtMdd);

    array_insert(mdd_t *, newFunction, i, cofactor);
  }
  return((Mvf_Function_t *)newFunction);
}


/**Function********************************************************************

  Synopsis [Calls bdd_minimize on each component of a multi-valued function.]

  Description [Calls bdd_minimize on each component of a multi-valued function
  f, minimizing with respect to the care function c.  The returned function
  agrees with f wherever c is true, and may or may not agree with f wherever c
  is false. It is an error to call this function with a multi-valued function
  that contains null MDDs or with a null care.]

  SideEffects []

******************************************************************************/
Mvf_Function_t *
Mvf_FunctionMinimize(
  Mvf_Function_t *f,
  mdd_t          *c)
{
  int      i;
  array_t *mddArray      = (array_t *) f;
  int      numComponents = array_n(mddArray);
  array_t *newFunction   = array_alloc(mdd_t *, numComponents);
  
  for(i = 0; i < numComponents; i++) {
    mdd_t *component = array_fetch(mdd_t *, mddArray, i);
    mdd_t *minimize  = bdd_minimize(component, c);

    array_insert(mdd_t *, newFunction, i, minimize);
  }
  return((Mvf_Function_t *)newFunction);
}


/**Function********************************************************************

  Synopsis [Returns the number of BDD nodes of an array of multi-valued
  functions.]

  Description [Returns the number of BDD nodes of an array of multi-valued
  functions. A node shared by several functions is counted only once.]

  SideEffects []

******************************************************************************/
long
Mvf_FunctionArrayComputeNumBddNodes(
  array_t * functionArray)
{
  int      i;
  long     numNodes;
  array_t *mddArray = array_alloc(mdd_t *, 0);

  /* Build an array of all MDDs */
  for(i = 0; i < array_n(functionArray); i++) {
    int      j;
    array_t *function = (array_t *)array_fetch(Mvf_Function_t *, functionArray, i);

    for(j = 0; j < array_n(function); j++) {
      mdd_t *component = array_fetch(mdd_t *, function, j);
      array_insert_last(mdd_t *, mddArray, component);
    }
  }
  
  /* Compute the reduced number of bdd nodes */
  numNodes = mdd_size_multiple(mddArray);
  array_free(mddArray);
  
  return(numNodes);
}


/**Function********************************************************************

  Synopsis [Returns the number of BDD nodes of a multi-valued function.]

  SideEffects []

******************************************************************************/
long
Mvf_FunctionComputeNumBddNodes(
  Mvf_Function_t * function)
{
  return(mdd_size_multiple((array_t *)function));
}


/**Function********************************************************************

  Synopsis [Returns the index of the first component of a multi-valued
  function that is equal to the tautology.]

  Description [Returns the index of the first component of a multi-valued
  function that is equal to the tautology. If the multi-valued function is
  deterministic, this component is unique. Returns -1 if such a component is
  not found. It is an error to call this function with a multi-valued function
  that contains null MDDs.]

  SideEffects []

******************************************************************************/
int
Mvf_FunctionFindFirstTrueComponent(
  Mvf_Function_t * function)
{
  int      i;
  array_t *mddArray = (array_t *) function;
  
  for(i = 0; i < array_n(mddArray); i++) {
    mdd_t *component = array_fetch(mdd_t *, mddArray, i);
    if (mdd_is_tautology(component, 1)) {
      return(i);
    }
  }
  return(-1);
}

/**Function********************************************************************

  Synopsis [Hashes A multi-valued function.]

  Description [Hashes A multi-valued function. Each component's top variable id
  is multiplied by the index of component (+ 1). Returns the sum of this computation
  on every component. It is an error to call this function with a null
  multi-valued function.]

  SideEffects []

******************************************************************************/
int
Mvf_FunctionComputeHashValue(
  Mvf_Function_t * function)
{
  int      i;
  int      result   = 0;
  array_t *mddArray = (array_t *) function;
  
  for(i = 0; i < array_n(mddArray); i++) {
    mdd_t *component = array_fetch(mdd_t *, mddArray, i);
    result += (mdd_top_var_id(component)) * (i + 1);
  }
  return(result);
}



/**Function********************************************************************

  Synopsis [Computes the variables in the true support of a function.]

  Description [Computes the variables in the true support of a function.  Does
  this by taking the union of the result of mdd_get_support on each component
  of the function.  Returns the support as an (ascending) ordered array of MDD
  ids. If the function is a constant, then a NULL array is returned, and the
  constant value of the function is written in the "value" variable.]

  SideEffects []

  SeeAlso     [Mvf_FunctionTestIsConstant]

******************************************************************************/
array_t *
Mvf_FunctionComputeSupport(
  Mvf_Function_t *function,
  mdd_manager *mddMgr,
  int *value)
{
  int        i;
  mdd_t     *component;
  array_t   *totalSupportArray;
  var_set_t *totalSupportSet;
  int        numMddVars = array_n(mdd_ret_mvar_list(mddMgr));

  /*
   * Handle the case where function is just a constant.
   */
  if (Mvf_FunctionTestIsConstant(function, value)) {
    return NIL(array_t);
  }
  
  /*
   * Accumulate the union of supports of the function components into a bit
   * array.
   */
  totalSupportArray = array_alloc(int, 0);
  totalSupportSet   = var_set_new(numMddVars);

  Mvf_FunctionForEachComponent(function, i, component) {
    int      j;
    int      mddVarId;
    array_t *support = mdd_get_support(mddMgr, component);

    arrayForEachItem(int, support, j, mddVarId) {
      var_set_set_elt(totalSupportSet, mddVarId);
    }
    array_free(support);
  }
  
  /* Convert the bit array to an array of mdd ids. */
  for (i = 0; i < numMddVars; i++) {
    if (var_set_get_elt(totalSupportSet, i)) {
      array_insert_last(int, totalSupportArray, i);
    }
  }
  var_set_free(totalSupportSet);

  return totalSupportArray;
}

/**Function********************************************************************

  Synopsis           [Returns the mvf for the modulo addition of the two
  multi-valued functions.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mvf_Function_t *
Mvf_FunctionComputeAddition(
  Mvf_Function_t *mvf1,
  Mvf_Function_t *mvf2)
{
  mdd_manager *mddManager;
  int i, numComponents, index1, index2;
  mdd_t *mdd1, *mdd2, *tempMdd1, *tempMdd2, *resultMdd;
  Mvf_Function_t *resultMvf;
  
  assert(Mvf_FunctionReadNumComponents(mvf1) ==
         Mvf_FunctionReadNumComponents(mvf2));
  assert(Mvf_FunctionReadMddManager(mvf1) ==
         Mvf_FunctionReadMddManager(mvf2));
  
  numComponents = Mvf_FunctionReadNumComponents(mvf1);
  mddManager = Mvf_FunctionReadMddManager(mvf2);
  resultMvf = Mvf_FunctionAlloc(mddManager, numComponents);
  
  for (i = 0; i < numComponents; i++) {
    resultMdd = mdd_zero(mddManager);

    /* Generating the ith component of resultMvf */
    
    for (index1 = 0, index2 = i; index1 < numComponents; index1++) {
      mdd1 = Mvf_FunctionReadComponent(mvf1, index1);
      mdd2 = Mvf_FunctionReadComponent(mvf2, index2);
      tempMdd1 = mdd_and(mdd1, mdd2, 1, 1);
      tempMdd2 = mdd_or(resultMdd, tempMdd1, 1, 1);
      mdd_free(resultMdd);      
      mdd_free(tempMdd1);
      resultMdd = tempMdd2;
      
      index2--;
      if (index2 == -1)
        index2 = numComponents - 1;
    }
    
    Mvf_FunctionAddMintermsToComponent(resultMvf, i, resultMdd);
  }

  return resultMvf;
}


/**Function********************************************************************

  Synopsis           [Returns the mvf for the modulo subtraction of the two
  multi-valued functions.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mvf_Function_t *
Mvf_FunctionComputeSubtraction(
  Mvf_Function_t *mvf1,
  Mvf_Function_t *mvf2)
{
  mdd_manager *mddManager;
  int i, numComponents, index1, index2;
  mdd_t *mdd1, *mdd2, *tempMdd1, *tempMdd2, *resultMdd;
  Mvf_Function_t *resultMvf;
  
  assert(Mvf_FunctionReadNumComponents(mvf1) ==
         Mvf_FunctionReadNumComponents(mvf2));
  assert(Mvf_FunctionReadMddManager(mvf1) ==
         Mvf_FunctionReadMddManager(mvf2));
  
  numComponents = Mvf_FunctionReadNumComponents(mvf1);
  mddManager = Mvf_FunctionReadMddManager(mvf2);
  resultMvf = Mvf_FunctionAlloc(mddManager, numComponents);
  
  for (i = 0; i < numComponents; i++) {
    resultMdd = mdd_zero(mddManager);

    /* Generating the ith component of resultMvf */
    
    for (index1 = i, index2 = 0; index2 < numComponents; index2++) {
      mdd1 = Mvf_FunctionReadComponent(mvf1, index1);
      mdd2 = Mvf_FunctionReadComponent(mvf2, index2);
      tempMdd1 = mdd_and(mdd1, mdd2, 1, 1);
      tempMdd2 = mdd_or(resultMdd, tempMdd1, 1, 1);
      mdd_free(resultMdd);      
      mdd_free(tempMdd1);
      resultMdd = tempMdd2;
      
      index1++;
      if (index1 == numComponents)
        index1 = 0;
    }
    
    Mvf_FunctionAddMintermsToComponent(resultMvf, i, resultMdd);
  }

  return resultMvf;
}


/**Function********************************************************************

  Synopsis           [Returns the if then else of two mvfs.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mvf_Function_t *
Mvf_FunctionComputeITE(
  mdd_t *ifMdd,
  Mvf_Function_t *thenMvf,
  Mvf_Function_t *elseMvf)
{
  mdd_manager *mddManager;
  int i, numComponents;
  Mvf_Function_t *resultMvf;
  mdd_t *thenMdd, *elseMdd;
  
  assert(Mvf_FunctionReadNumComponents(thenMvf) ==
         Mvf_FunctionReadNumComponents(elseMvf));
  assert(Mvf_FunctionReadMddManager(thenMvf) ==
         Mvf_FunctionReadMddManager(elseMvf));
  
  numComponents = Mvf_FunctionReadNumComponents(thenMvf);
  mddManager = Mvf_FunctionReadMddManager(thenMvf);
  
  resultMvf = Mvf_FunctionAlloc(mddManager, numComponents);
  
  for (i = 0; i < numComponents; i++) {
    thenMdd = Mvf_FunctionReadComponent(thenMvf, i);
    elseMdd = Mvf_FunctionReadComponent(elseMvf, i);
    Mvf_FunctionAddMintermsToComponent(resultMvf, i, mdd_ite(ifMdd, thenMdd, elseMdd,
                                                             1, 1, 1));
  }

  return resultMvf;
}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

