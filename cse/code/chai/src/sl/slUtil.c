/**CFile***********************************************************************

  FileName    [slUtil.c]

  PackageName [sl]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [Freddy Mang and Luca de Alfaro]

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

   Revision    [$Id: slUtil.c,v 1.20 2003/05/07 21:44:13 ashwini Exp $]

******************************************************************************/

#include "slInt.h" 


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

static int DFS_dependency (st_table *idToPairTable, Pair_t *pair, int level); 

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlDeleteGstEntry(
  char * name) 
{
  Sl_GSTEntry_t * gstEntry;
  
  if (st_delete(Sl_GST, (char **) &name, (char **) & gstEntry)) {
    SlGstEntryFree(gstEntry);
  }
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlInsertGstEntry(
  Sl_GSTEntry_t * gstEntry)
{
  st_insert(Sl_GST, gstEntry -> name, (char *) gstEntry);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlGstEntryFree(
  Sl_GSTEntry_t * gstEntry)
{
  FREE(gstEntry -> name);
  switch (gstEntry -> kind) {
      case Sl_FSM_c:
        SlFreeArrayOfBdd(gstEntry -> obj.fsm.initBdd);
        SlFreeArrayOfBdd(gstEntry -> obj.fsm.updateBdd);
        array_free(gstEntry -> obj.fsm.contrvarL);
        array_free(gstEntry -> obj.fsm.extvarL);
        array_free(gstEntry -> obj.fsm.erasedvarL);
        break;
        
      case Sl_BDD_c:
        SlFreeArrayOfBdd(gstEntry -> obj.bddArray);
        break;

      default:
        break;
  }

  FREE(gstEntry);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Sl_GSTEntry_t *
SlGstGetEntry(
  char * name
  )
{
  Sl_GSTEntry_t * entry;
  
  if (!st_lookup(Sl_GST, name, (char **) &entry )) {
    return  NIL(Sl_GSTEntry_t);
  } else {
    return entry;
  }
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlFreeArrayOfBdd(
  array_t * bddArray)
{
  int i;
  mdd_t * bdd;
  
  arrayForEachItem(mdd_t *, bddArray, i, bdd){
    mdd_free(bdd);
  }

  array_free(bddArray);

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Pair_t * SlPairAlloc()
{
  Pair_t * pair = ALLOC(Pair_t, 1);
  pair -> info = NIL(char);
  pair -> first = 0;
  pair -> second = 0;
  pair -> flag = 0; 

  return pair;
}


/**Function********************************************************************

  Synopsis           [Allocates a new level.] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

Level_t * SlLevelAlloc()
{ 
    Level_t * level = ALLOC(Level_t, 1); 
    level -> Ivars       = array_alloc (int, 0); 
    level -> Ovars       = array_alloc (int, 0); 
    level -> IinitBdds   = array_alloc (mdd_t *, 0); 
    level -> OinitBdds   = array_alloc (mdd_t *, 0); 
    level -> IupdateBdds = array_alloc (mdd_t *, 0); 
    level -> OupdateBdds = array_alloc (mdd_t *, 0); 
    return (level); 
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlCompareInt (char *obj1, char *obj2)
{
  int n1, n2;

  n1 = *((int *) obj1);
  n2 = *((int *) obj2);
  
  return n1 - n2;
}


/**Function********************************************************************

  Synopsis           [Checks whether implication holds between two bdd arrays.
                      Returns a boolean, depending on whether it does, or not.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

 int
SlImplicationCheck (array_t *bddArray1, array_t *bddArray2)
{
  array_t *lhsMdds;
  int i, nLastEl, holds;
  mdd_t *lhsMdd, *rhsMdd, *negOfRhs, *resultMdd;
  array_t *lhsIds;
  mdd_manager *mgr;
  array_t *emptyArray;

  mgr = SlGetMddManager ();

  /* Construct the array of all the Ids appearing on the lhs.

  This code is commented out, to experiment with making this faster. 

  lhsIds = array_alloc(int, 0);
  arrayForEachItem (mdd_t *, bddArray1, i, lhsMdd) {
    array_append (lhsIds, mdd_get_support(mgr, lhsMdd));
  }
  Eliminate multiple occurrences of Ids

  array_sort (lhsIds, SlCompareInt);
  array_uniq (lhsIds, SlCompareInt, 0);
  */ 

  /* gets a duplicate, modifiable array for the lhs Bdds */

  lhsMdds = array_dup (bddArray1);
  
  /* We add a null BDD to the array lhsMdds, 
     as a placeholder for the negation of the rhs. */

  nLastEl = array_n(bddArray1);
  array_insert_last (mdd_t *, lhsMdds, NULL);
  emptyArray = array_alloc(int, 0);
    
  /* Now, we loop one by one on the conjuncts of bddArray2, and we show that they
     are implied by bddArray1. */

  arrayForEachItem (mdd_t *, bddArray2, i, rhsMdd) {

    /* puts the negation of the rhs at the end of the array of the lhs */
    negOfRhs = mdd_not (rhsMdd);
    array_insert (mdd_t *, lhsMdds, nLastEl, negOfRhs);
    /* This is the original code, with the quantification 
    resultMdd = Img_MultiwayLinearAndSmooth(mgr, lhsMdds, lhsIds, emptyArray);
    */ 
    resultMdd = Img_MultiwayLinearAndSmooth(mgr, lhsMdds, emptyArray, emptyArray); 
    holds = mdd_is_tautology (resultMdd, 0);
    mdd_free (negOfRhs);
    mdd_free (resultMdd);
    if (!holds) break;
  }

  array_free (lhsMdds);
  /* array_free (lhsIds); --original code */
  array_free (emptyArray);

  return holds; 
}


/**Function********************************************************************

  Synopsis           [Computes the union between two arrays, 
                      eliminating duplicates.]

  Description        [optional]

  SideEffects        [leaves the result in the first argument, 
                      second argument is unchanged.]

  SeeAlso            [optional]

******************************************************************************/
 array_t* 
SlArrayUnion (array_t *inArray1, array_t *inArray2) 
{
  array_t *result; 

  result = array_join(inArray1, inArray2);
  array_sort(result, SlCompareInt);
  array_uniq(result, SlCompareInt, NULL);
  return (result);
}


/**Function********************************************************************

  Synopsis           [Computes the set difference between two arrays of ints, 
                      eliminating duplicates. ]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/
 array_t *
SlArrayDifference (array_t *lhs, array_t *rhs)
{
  array_t *result, *a, *b;
  int posa, posb, sizea, sizeb, ela, elb; 

  result = array_alloc(int, 0);
  a = array_dup(lhs);
  b = array_dup(rhs);

  /* sorts input arrays */
  array_sort(a, SlCompareInt);
  array_sort(b, SlCompareInt);

  sizea = array_n(a);
  sizeb = array_n(b);
  posa = 0;
  posb = 0;

  /* does the array difference */ 

  while (posa < sizea) {

    ela = array_fetch(int, a, posa);

    if (posb >= sizeb) {
      array_insert_last(int, result, ela);
      posa++; 
   } else {
      elb = array_fetch(int, b, posb);
      if        (ela  < elb) { 
	array_insert_last(int, result, ela);
	posa++; 
      } else if (ela == elb) { 
	posa++; 
      } else {
	posb++;
      }
    }
  }

  array_free(a);
  array_free(b);

  return result;
}

/**Function********************************************************************

  Synopsis           [Computes the intersection between two arrays of ints, 
                      eliminating duplicates. ]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

array_t * SlArrayIntersection (array_t *lhs, array_t *rhs)
{
  array_t *result, *a, *b;
  int posa, posb, sizea, sizeb, ela, elb; 

  result = array_alloc(int, 0);
  a = array_dup(lhs);
  b = array_dup(rhs);

  /* sorts input arrays */
  array_sort(a, SlCompareInt);
  array_sort(b, SlCompareInt);

  sizea = array_n(a);
  sizeb = array_n(b);
  posa = 0;
  posb = 0;

  /* does the array intersection */ 

  while (posa < sizea && posb < sizeb) {
    ela = array_fetch(int, a, posa);
    elb = array_fetch(int, b, posb);
    if (ela < elb) { 
      posa++; 
    } else if (elb < ela) { 
      posb++; 
    } else { /* elb == ela */ 
      posa++; 
      posb++; 
      array_insert_last(int, result, ela);
    } 
  }

  array_free(a);
  array_free(b);

  return result;
}


/**Function********************************************************************

  Synopsis           [Checks if lhs is a subset of rhs.] 

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

int SlIsArraySubset (array_t *lhs, array_t *rhs)
{
  array_t *a, *b;
  int posa, posb, sizea, sizeb, ela, elb, result; 

  a = array_dup(lhs);
  b = array_dup(rhs);

  /* sorts input arrays */
  array_sort(a, SlCompareInt);
  array_sort(b, SlCompareInt);
  array_uniq(a, SlCompareInt, NULL); 
  array_uniq(b, SlCompareInt, NULL); 

  sizea = array_n(a);
  sizeb = array_n(b);
  if (sizea > sizeb) {
      result = 0; 
  } else { 
      posa = 0;
      posb = 0;
      while (posa < sizea && posb < sizeb) {
	  ela = array_fetch(int, a, posa);
	  elb = array_fetch(int, b, posb);
	  if (ela < elb) { 
	      posa++; 
	  } else if (elb < ela) { 
	      posb++; 
	  } else { /* elb == ela */ 
	      posa++; 
	      posb++; 
	  } 
      }
      /* a is a subset of b if we could scan all of a */ 
      result = (posa == sizea); 
  }
  array_free(a);
  array_free(b);

  return result;
}


/**Function********************************************************************

  Synopsis           [Returns 1 if the arrays are equal, 0 otherwise. ]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

int SlAreArrayEqual (array_t *lhs, array_t *rhs)
{
  array_t *a, *b;
  int pos, sizea, sizeb, ela, elb, result; 

  a = array_dup(lhs);
  b = array_dup(rhs);

  /* sorts input arrays */
  array_sort(a, SlCompareInt);
  array_sort(b, SlCompareInt);
  array_uniq(a, SlCompareInt, NULL); 
  array_uniq(b, SlCompareInt, NULL); 

  sizea = array_n(a);
  sizeb = array_n(b);
  if (!(sizea == sizeb)) { 
      result = 0; 
  } else { 
      result = 1; 
      pos = 0;
      while (pos < sizea) {
	  ela = array_fetch(int, a, pos);
	  elb = array_fetch(int, b, pos);
	  if (!(ela == elb)) {
	      result = 0; 
	      break; 
	  }
	  pos++;
      }
  }
  array_free(a);
  array_free(b);
  return result; 
}


/**Function********************************************************************

  Synopsis           [Checks that the support of a given MDD is within a 
                      specified set of variables.  Returns 0 if ok, 1 if not.]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

int 
SlCheckSupportSubset (mdd_t *formula, array_t *allowedVars, char *errorMsg)
{
  array_t     *vars, *extraVars; 
  mdd_manager *mgr;
  int         outcome; 

  mgr       = SlGetMddManager ();
  vars      = mdd_get_support (mgr, formula); 
  extraVars = SlArrayDifference (vars, allowedVars); 

  if (array_n(extraVars)) { 
    Main_MochaErrorPrint ("%s\n", errorMsg); 
    SlPrintVarNames (extraVars); 
    outcome = 1; 
  } else { 
    outcome = 0; 
  }
  array_free (extraVars);
  array_free (vars); 
  return (outcome); 
}


/**Function********************************************************************

  Synopsis           [Duplicating an array of bdds.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

 array_t* 
SlBddArrayDupe(array_t *dupeme)
{
  int i;
  mdd_t * mdd;
  array_t * newArray;
  
  newArray = array_alloc(mdd_t*, 0);
  arrayForEachItem(mdd_t*, dupeme, i, mdd){
    array_insert_last(mdd_t *, newArray, mdd_dup(mdd));
  }

  return newArray;
}


/**Function********************************************************************

  Synopsis           [Duplicating an array of ints.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

 array_t * 
SlIntArrayDupe(array_t *dupeme)
{
  int i;
  int number;
  array_t * newArray;
  
  newArray = array_alloc(int, 0);
  arrayForEachItem(int, dupeme, i, number){
    array_insert_last(int, newArray, number);
  }

  return newArray;
}


/**Function********************************************************************

  Synopsis           [Input: an array of MDDs. Output: an MDD obtained by
                      conjoining them. ]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

/* IMPROVE: sort them in increasing size before conjoining */ 

 mdd_t * 
SlConjoin (array_t *bddArray) 
{
  mdd_t        *out, *newOut, *conj; 
  int          i;
  mdd_manager *mgr;

  mgr = SlGetMddManager ();

  out = mdd_one (mgr); 
  arrayForEachItem(mdd_t*, bddArray, i, conj) {
    newOut = mdd_and (out, conj, 1, 1);
    mdd_free (out); 
    out = newOut; 
  }

  return (out); 
}


/**Function********************************************************************

  Synopsis           [Input: An array of MDDs, and an array of smoothing 
                      variables. 
		      Output: the MDD obtained by smoothly conjoining the 
		      variables. ]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

 mdd_t * 
SlSmoothConjoin (array_t *bddArray, array_t *variables) 
{
  mdd_t        *out, *newOut, *conj; 
  mdd_manager  *mgr;
  int          i;

  mgr = SlGetMddManager ();
  out = mdd_one (mgr); 

  arrayForEachItem(mdd_t*, bddArray, i, conj) {
    newOut = mdd_and (out, conj, 1, 1); 
    mdd_free (out); 
    out = newOut; 
  }
  newOut = mdd_smooth (mgr, out, variables); 
  mdd_free (out); 
  
  return (newOut); 
}


/**Function********************************************************************

  Synopsis           [Given an mdd, produces a singleton array containing the mdd.]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

 array_t *
SlSingleMddArray (mdd_t *inputMdd)
{
  array_t *result; 
  
  result = array_alloc (mdd_t*, 0); 
  array_insert_last (mdd_t *, result, inputMdd); 
  return (result); 
}


/**Function********************************************************************

  Synopsis           [Given an mdd and a set of variables, prints statistics
                      for the mdd: size of support set and n. of nodes. ]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

 int
SlPrintBddStats (mdd_t *inBdd, array_t* support)
{
  int outcome; 
  mdd_manager *mgr;

  mgr = SlGetMddManager ();

  /* first, checks that the Bdd array has the right support. */ 
  outcome = SlCheckSupportSubset(inBdd, support, "ERL06: Bdd with too large support!\n");
  
  Main_MochaPrint ("*Bdd size = %8d, N. of states = %5g\n",
		   mdd_size        (inBdd), 
		   (double) mdd_count_onset (mgr, inBdd, support)); 
    
  return (0); 
}


/**Function********************************************************************

  Synopsis           [Prints an array of vars, for debug.]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

 void
SlPrintVarArray (array_t *a)
{
  int i; 
  int v; 

  Main_MochaPrint ("*Variables: "); 
  arrayForEachItem(int, a, i, v) {
    Main_MochaPrint(" %d", v); 
  }
  Main_MochaPrint ("\n"); 
}

/**Function********************************************************************

  Synopsis           [gets the initial condition of an FSM in unprimed form.] 

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

mdd_t * 
SlGetInit (Sl_GSTEntry_t *fsmEntry)
{ 
  mdd_t   *conj, *unprimedConj, *out, *newOut; 
  int     i; 
  mdd_manager *mgr;

  mgr = SlGetMddManager ();
  out = mdd_one (mgr); 

  arrayForEachItem(mdd_t*, fsmEntry -> obj.fsm.initBdd, i, conj) {
    unprimedConj = SlUnprimeMdd (conj); 
    newOut       = mdd_and (out, unprimedConj, 1, 1); 
    mdd_free (unprimedConj); 
    mdd_free (out); 
    out = newOut; 
  }
  newOut = mdd_smooth (mgr, out, fsmEntry -> obj.fsm.erasedvarL);
  mdd_free (out); 
  return (newOut); 
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
 int
SlPrintVarNames (array_t *idArray) 
{
  int i, id;
  Var_Variable_t * var;

  arrayForEachItem(int, idArray, i, id) {
    var = SlReturnVariableFromId(id);
    if (SlReturnPrimedId(id) != id) {
      Main_MochaPrint("%s ", Var_VariableReadName(var));
    } else {
      Main_MochaPrint("%s' ", Var_VariableReadName(var));
    }
  }
  Main_MochaPrint("\n");
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlTclPrintVarNames (array_t *idArray) 
{
  int i, id;
  Var_Variable_t * var;

  arrayForEachItem(int, idArray, i, id) {
    var = SlReturnVariableFromId(id);
    if (SlReturnPrimedId(id) != id) {
      /* Main_AppendResult("%s ", Var_VariableReadName(var)); */ 
      Main_MochaPrint ("%s ", Var_VariableReadName(var));
    } else {
      /* Main_AppendResult("%s' ", Var_VariableReadName(var)); */ 
      Main_MochaPrint ("%s' ", Var_VariableReadName(var));
    }
  }
}


/**Function********************************************************************

  Synopsis           [Returns 1 if the dependency relation has cycles, 
                      and 0 otherwise.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int SlDetectCycles (array_t *dependency) 
{
  int i; 
  Pair_t *pair; 

  /* Temporary table to be able to find the pair given the id */ 
  st_table *idToPairTable = st_init_table(st_numcmp, st_numhash);

  /* First goes over the list, and ensures that the flag is 0 */ 
  arrayForEachItem(Pair_t *, dependency, i, pair) {
    st_insert(idToPairTable, (char *) (long) pair -> first, (char *) pair);
    pair -> flag = 0; 
  }
  /* Now picks a vertex with flag = 0, and does DFS from that vertex, 
     checking for back edges. */ 
  arrayForEachItem(Pair_t *, dependency, i, pair) {
    if ((pair -> flag == 0) && DFS_dependency (idToPairTable, pair, 1)) { 
      st_free_table(idToPairTable);
      return (int) 1; 
    }
  }
  /* No cycle found */ 
  st_free_table(idToPairTable);
  return (int) 0; 
}


/**Function********************************************************************

  Synopsis           [Does DFS search of the dependency graph to check 
                      for cycles. Returns 1 if the dependency relation 
		      has cycles, and 0 otherwise. 
		      The flag is positive while the exploration is 
		      going on, and negative when it is closed. See 
		      Intro to Algo, Cormen et al, Ch. 23, for colors.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static int DFS_dependency (st_table *idToPairTable, Pair_t *pair, int level)
{
  int i; 
  Pair_t *child_id, *child; 

  /* Marks the level of the current pair.  Pair is grey: descendants not 
     yet explored. */ 
  pair -> flag = level;   
  /* Now looks at all the successors of the pair */ 
  arrayForEachItem(Pair_t *, (array_t *) pair -> info, i, child_id) { 
    /* Looks up the child, to find out which color it is */ 
    st_lookup(idToPairTable, (char *) (long) child_id -> first, (char **) &child);
    if (child -> flag == 0) { 
      /* If child is white, explore it, and report any error found */ 
      if (DFS_dependency (idToPairTable, child, level + 1)) { 
	return (int) 1; 
      }
    } else if (child -> flag > 0) { 
      /* If child is gray, we have found a loop */ 
      return (int) 1; 
    }
    /* For black children, do nothing. */ 
  }
  /* If we have found no loops, we have concluded the exploration of the present level. 
     Paint child black, and return. */ 
  pair -> flag = -level; 
  return (int) 0; 
}

/**Function********************************************************************

  Synopsis           [Functions implementing a stack of integers.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

Int_Stack_t * SlStackAlloc ()
{ 
    return NULL; 
} 

Int_Stack_t * SlStackPush (Int_Stack_t *stack, int i)
{ 
    Int_Stack_t *newel = ALLOC(Int_Stack_t, 1); 
    newel -> next = stack; 
    newel -> n    = i; 
    return newel; 
}

Int_Stack_t * SlStackPop (Int_Stack_t *stack, int *ip)
{
    Int_Stack_t * newstack = stack -> next; 
    *ip = stack -> n; 
    FREE (stack); 
    return newstack;
}

int SlStackIsEmpty (Int_Stack_t *stack)
{
    return (stack == NULL); 
}

/**Function********************************************************************

  Synopsis           [Frees a dependency list, following also the list of pairs.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

void SlFreeDependency (array_t * dependency)
{
    Pair_t *pair, *apair; 
    int i, j; 

    arrayForEachItem (Pair_t *, dependency, i, pair) { 
	arrayForEachItem (Pair_t *, (array_t *) pair -> info, j, apair) { 
	    FREE (apair); 
	}
	array_free ((array_t *) pair -> info); 
	FREE (pair); 
    }
    array_free (dependency); 
}


/**Function********************************************************************

  Synopsis           [Returns the union of all the variables of an interface, 
                      given a pointer to the levels array.] 
  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

array_t * SlVarsOfAllLevels (array_t *levels)
{
    Level_t *lev; 
    int i; 
    array_t *result = array_alloc (int, 0); 

    arrayForEachItem (Level_t *, levels, i, lev) { 
	array_append (result, lev -> Ivars); 
	array_append (result, lev -> Ovars); 
    }
    return result; 
}


/**Function********************************************************************

  Synopsis           [Returns the union of all the output variables 
                      of an interface, given a pointer to the levels array.] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

array_t * SlOVarsOfAllLevels (array_t *levels)
{
    Level_t *lev; 
    int i; 
    array_t *result = array_alloc (int, 0); 

    arrayForEachItem (Level_t *, levels, i, lev) { 
	array_append (result, lev -> Ovars); 
    }
    return result; 
}


/**Function********************************************************************

  Synopsis           [Returns the union of all the input variables 
                      of an interface, given a pointer to the levels array.] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

array_t * SlIVarsOfAllLevels (array_t *levels)
{
    Level_t *lev; 
    int i; 
    array_t *result = array_alloc (int, 0); 

    arrayForEachItem (Level_t *, levels, i, lev) { 
	array_append (result, lev -> Ivars); 
    }
    return result; 
}


/**Function********************************************************************

  Synopsis           [Frees a level list, including the MDDs. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/


void SlFreeLevels (array_t * levels)
{
    int i, j; 
    Level_t * lev; 
    mdd_t * conj; 

    arrayForEachItem (Level_t *, levels, i, lev) { 
	array_free (lev -> Ivars); 
	array_free (lev -> Ovars); 
	SlFreeMddList (lev -> IinitBdds); 
	SlFreeMddList (lev -> OinitBdds); 
	SlFreeMddList (lev -> IupdateBdds); 
	SlFreeMddList (lev -> OupdateBdds); 
    }
    array_free (levels); 
}


/**Function********************************************************************

  Synopsis           [Frees a list of MDDs, freeing both the MDDs and the list.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

void SlFreeMddList (array_t * list) 
{ 
    int i; 
    mdd_t * conj;

    arrayForEachItem (mdd_t *, list, i, conj) { 
	mdd_free (conj); 
    }
    array_free (list); 
}


/**Function********************************************************************

  Synopsis           [Appends an array of MDDs to another, duping the MDDs. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

void SlMddArrayAppend (array_t *a, array_t *b)
{ 
    int i; 
    mdd_t * m; 
    arrayForEachItem (mdd_t *, b, i, m) { 
	array_insert_last (mdd_t *, a, mdd_dup (m));
    }
}


/**Function********************************************************************

  Synopsis           [Existentially quantifies an array of conjuncts. 
                      It uses the classical image computation routine, 
		      but is robust to the variables being empty.
		      It returns AN ARRAY, so that if the variables 
		      are empty, the same array is returned. ] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

array_t * SlEArrayQuantify (array_t * conjuncts, array_t * variables)
{
    mdd_manager *mgr;
    array_t * empty_array;
    mdd_t * result, * conj; 
    int i; 
    array_t * a_result; 

    if (array_n (variables) > 0) { 
	empty_array  = array_alloc (int, 0); 
	mgr  = SlGetMddManager ();
	result = Img_MultiwayLinearAndSmooth (mgr, conjuncts, variables, empty_array);
	array_free (empty_array); 
	return SlSingleMddArray (result); 
    } else { 
	/* Not the array itself: should be de-allocatable independently. */ 
	a_result = array_alloc (mdd_t *, 0); 
	arrayForEachItem (mdd_t *, conjuncts, i, conj) { 
	    array_insert_last (mdd_t *, a_result, mdd_dup (conj)); 
	}
	return (a_result); 
    }
}


/**Function********************************************************************

  Synopsis           [Existentially quantifies an array of conjuncts. 
                      It uses the classical image computation routine, 
		      but is robust to the variables being empty.
		      It returns AN MDD. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

mdd_t * SlEMddQuantify (array_t * conjuncts, array_t * variables)
{
    mdd_manager *mgr;
    array_t * empty_array;
    mdd_t * result; 

    if (array_n (variables) > 0) { 
	empty_array  = array_alloc (int, 0); 
	mgr  = SlGetMddManager ();
	result = Img_MultiwayLinearAndSmooth (mgr, conjuncts, variables, empty_array);
	array_free (empty_array); 
	return result; 
    } else { 
	return SlConjoin (conjuncts); 
    }
}


/**Function********************************************************************

  Synopsis           [This function prints an array of BDDs. 
                      Useful for debugging. ] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

void SlPrintMddArray (array_t * a) 
{
    mdd_t * b; 
    int i; 
    
    arrayForEachItem (mdd_t *, a, i, b) { 
	SlMddPrintCubes (b, 0); 
    }
}

/**Function********************************************************************

  Synopsis           [This function computes the history-free variables of a
                      module.
		      Input:  the list of all variables of a module, and the 
		              transition relation.
		      Output: the list of history-free variables.] 


  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

array_t *SlHistoryFree (array_t * allvars, array_t * trans_rel)
{
  mdd_manager *mgr;
  array_t *temp1, *temp2;
  array_t *conj_vars, *unprimed_vars, *history_free; 
  mdd_t *conj; 
  int i, j, id; 

  mgr = SlGetMddManager ();
  unprimed_vars = array_alloc (int, 0); 
  arrayForEachItem (mdd_t*, trans_rel, i, conj) {
    conj_vars = mdd_get_support (mgr, conj); 
    arrayForEachItem (int, conj_vars, j, id) { 
      if (SlReturnPrimedId(id) != id) {
	/* variable is unprimed */ 
	array_insert_last (int, unprimed_vars, id); 
      }
    } /* for each conjunct variable */ 
    array_free (conj_vars); 
  } /* for each conjunct */ 
  /* Now, eliminated duplicates from the arrays. */ 
  array_sort (unprimed_vars, SlCompareInt);
  array_uniq (unprimed_vars, SlCompareInt, 0);
  /* The history-free variables are the variables that 
     do not appear unprimed. */ 
  history_free = SlArrayDifference (allvars, unprimed_vars); 
  array_free (unprimed_vars); 
  return (history_free); 
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/* 
 * $Log: slUtil.c,v $
 * Revision 1.20  2003/05/07 21:44:13  ashwini
 * small bug in SlImplication chk, where array was freed without being used.
 *
 * Revision 1.19  2003/05/07 06:19:56  luca
 * changed SlImplicationCheck, doing away with the quantification of the variables (it may be more costly than beneficial??)
 *
 * Revision 1.18  2003/05/02 05:27:56  luca
 * Added Log keywords
 *
 * 
 */ 
