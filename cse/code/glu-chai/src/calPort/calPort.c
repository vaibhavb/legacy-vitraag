/**CFile***********************************************************************

  FileName    [calPort.c]

  PackageName [cal_port]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [Rajeev K. Ranjan]

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
#include "calPortInt.h"

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

static void InvalidType(FILE *file, char *field, char *expected);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Function to construct a bdd_t.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
bdd_t *
bdd_construct_bdd_t(bdd_manager *mgr, bdd_node *fn)
{
  bdd_t *result;
  if (!fn){
	fail("bdd_construct_bdd_t: possible memory overflow");
  }
  result = Cal_MemAlloc(bdd_t, 1);
  result->bddManager = mgr;
  result->calBdd = fn;
  result->free = 0;
  return result;
}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_package_type_t
bdd_get_package_name(void)
{
  return CAL;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
void
bdd_end(void *manager)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  void *hooks;
  hooks = Cal_BddManagerGetHooks(mgr);
  Cal_MemFree(hooks); 
  Cal_BddManagerQuit(mgr);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_manager *
bdd_start(int nvariables)
{
  Cal_BddManager mgr;
  int i;
  bdd_external_hooks *hooks;
  
  mgr = Cal_BddManagerInit();
  for (i = 0; i < nvariables; i++) {
	(void) Cal_BddManagerCreateNewVarLast(mgr);
  }
  hooks = Cal_MemAlloc(bdd_external_hooks, 1);
  hooks->mdd = hooks->network = hooks->undef1 = (char *) 0;
  Cal_BddManagerSetHooks(mgr, (void *)hooks);
  return (bdd_manager *)mgr;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_create_variable(bdd_manager *manager)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  return bdd_construct_bdd_t(mgr, Cal_BddManagerCreateNewVarLast(mgr));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_create_variable_after(bdd_manager *manager, bdd_variableId afterId)
{
  Cal_Bdd afterVar;
  bdd_t 	*result;
  Cal_BddManager mgr = (Cal_BddManager) manager;
  afterVar = Cal_BddManagerGetVarWithId(mgr, afterId + 1);
  result =  bdd_construct_bdd_t(mgr,
                                Cal_BddManagerCreateNewVarAfter(mgr,
                                                                afterVar));  
  
  /* No need to free after_var, since single variable BDDs are never garbage collected */
  
  return result;
}



/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_get_variable(bdd_manager *manager, bdd_variableId varId)
{
  Cal_Bdd varBdd;
  Cal_BddManager mgr = (Cal_BddManager) manager;
  varBdd = Cal_BddManagerGetVarWithId(mgr, varId + 1);
  if (!varBdd){
	fprintf(stderr,"bdd_get_variable: Variable has not been created");
    exit(-1);
  }
  return bdd_construct_bdd_t(mgr, varBdd);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_dup(bdd_t *f)
{
  return bdd_construct_bdd_t(f->bddManager, Cal_BddIdentity(f->bddManager, f->calBdd));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
void
bdd_free(bdd_t *f)
{
  if (f == NIL(bdd_t)) {
	fail("bdd_free: trying to free a NIL bdd_t");			
  }
  if (f->free){
    fail("bdd_free: Trying to free a freed bdd_t");
  }
  Cal_BddFree(f->bddManager, f->calBdd);
  f->calBdd = (Cal_Bdd) 0;
  f->bddManager = NIL(Cal_BddManager_t);
  f->free = 1;
  Cal_MemFree(f);  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_node *
bdd_get_node(bdd_t *f, boolean *isComplemented)
{
  *isComplemented = CAL_TAG0(f->calBdd);
  return (f->calBdd);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_and(bdd_t *f,bdd_t *g, boolean f_phase, boolean g_phase)
{
  Cal_Bdd temp1, temp2;
  bdd_t *result;
  Cal_BddManager mgr;
  
  mgr = f->bddManager;
  temp1 = ((f_phase == TRUE) ? Cal_BddIdentity(mgr, f->calBdd) :
           Cal_BddNot(mgr, f->calBdd)); 
  temp2 = ((g_phase == TRUE) ? Cal_BddIdentity(mgr, g->calBdd) :
           Cal_BddNot(mgr, g->calBdd));
  result = bdd_construct_bdd_t(mgr, Cal_BddAnd(mgr, temp1, temp2));
  Cal_BddFree(mgr, temp1);
  Cal_BddFree(mgr, temp2);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_multiway_and(bdd_manager *manager, array_t *bddArray)
{
  int i;
  Cal_Bdd *calBddArray;
  bdd_t *operand, *result;
  Cal_BddManager mgr = (Cal_BddManager) manager;
  
  calBddArray = Cal_MemAlloc(Cal_Bdd, array_n(bddArray)+1);
  for (i=0; i<array_n(bddArray); i++){
    operand = array_fetch(bdd_t*, bddArray, i);
    calBddArray[i] = operand->calBdd;
  }
  calBddArray[i] = (Cal_Bdd)0;
  result = bdd_construct_bdd_t(mgr,
                               Cal_BddMultiwayAnd(mgr, calBddArray));
  Cal_MemFree(calBddArray);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_multiway_or(bdd_manager *manager, array_t *bddArray)
{
  int i;
  Cal_Bdd *calBddArray;
  bdd_t *operand, *result;
  Cal_BddManager mgr = (Cal_BddManager) manager;
  
  calBddArray = Cal_MemAlloc(Cal_Bdd, array_n(bddArray)+1);
  for (i=0; i<array_n(bddArray); i++){
    operand = array_fetch(bdd_t*, bddArray, i);
    calBddArray[i] = operand->calBdd;
  }
  calBddArray[i] = (Cal_Bdd)0;
  result = bdd_construct_bdd_t(mgr,
                               Cal_BddMultiwayOr(mgr, calBddArray));
  Cal_MemFree(calBddArray);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_multiway_xor(bdd_manager *manager, array_t *bddArray)
{
  int i;
  Cal_Bdd *calBddArray;
  bdd_t *operand, *result;
  Cal_BddManager mgr = (Cal_BddManager) manager;
  
  calBddArray = Cal_MemAlloc(Cal_Bdd, array_n(bddArray)+1);
  for (i=0; i<array_n(bddArray); i++){
    operand = array_fetch(bdd_t*, bddArray, i);
    calBddArray[i] = operand->calBdd;
  }
  calBddArray[i] = (Cal_Bdd)0;
  result = bdd_construct_bdd_t(mgr,
                               Cal_BddMultiwayXor(mgr, calBddArray));
  Cal_MemFree(calBddArray);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
array_t *
bdd_pairwise_and(bdd_manager *manager, array_t *bddArray1,
                 array_t *bddArray2) 
{
  int i;
  array_t *resultArray;
  Cal_Bdd *calBddArray, *calBddResultArray;
  bdd_t *operand;
  Cal_BddManager mgr = (Cal_BddManager) manager;
  
  if (array_n(bddArray1) != array_n(bddArray2)){
    fprintf(stderr, "bdd_pairwise_and: Arrays of different lengths.\n");
    return NIL(array_t);
  }
  calBddArray = Cal_MemAlloc(Cal_Bdd, 2*array_n(bddArray1)+1);
  for (i=0; i<array_n(bddArray1); i++){
    operand = array_fetch(bdd_t*, bddArray1, i);
    calBddArray[i<<1] = operand->calBdd;
    operand = array_fetch(bdd_t*, bddArray2, i);
    calBddArray[(i<<1)+1] = operand->calBdd;
  }
  calBddArray[i<<1] = (Cal_Bdd)0;
  calBddResultArray = Cal_BddPairwiseAnd(mgr, calBddArray);
  resultArray = array_alloc(bdd_t*, 0);
  for (i=0; i<array_n(bddArray1); i++){
    array_insert_last(bdd_t *, resultArray, 
                      bdd_construct_bdd_t(mgr, calBddResultArray[i]));
  }
  Cal_MemFree(calBddArray);
  Cal_MemFree(calBddResultArray);
  return resultArray;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
array_t *
bdd_pairwise_or(bdd_manager *manager, array_t *bddArray1,
                array_t *bddArray2) 
{
  int i;
  array_t *resultArray;
  Cal_Bdd *calBddArray, *calBddResultArray;
  bdd_t *operand;
  Cal_BddManager mgr = (Cal_BddManager) manager;
  
  if (array_n(bddArray1) != array_n(bddArray2)){
    fprintf(stderr, "bdd_pairwise_or: Arrays of different lengths.\n");
    return NIL(array_t);
  }
  calBddArray = Cal_MemAlloc(Cal_Bdd, 2*array_n(bddArray1)+1);
  for (i=0; i<array_n(bddArray1); i++){
    operand = array_fetch(bdd_t*, bddArray1, i);
    calBddArray[i<<1] = operand->calBdd;
    operand = array_fetch(bdd_t*, bddArray2, i);
    calBddArray[(i<<1)+1] = operand->calBdd;
  }
  calBddArray[i<<1] = (Cal_Bdd)0;
  calBddResultArray = Cal_BddPairwiseOr(mgr, calBddArray);
  resultArray = array_alloc(bdd_t*, 0);
  for (i=0; i<array_n(bddArray1); i++){
    array_insert_last(bdd_t *, resultArray, 
                      bdd_construct_bdd_t(mgr, calBddResultArray[i]));
  }
  Cal_MemFree(calBddArray);
  Cal_MemFree(calBddResultArray);
  return resultArray;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
array_t *
bdd_pairwise_xor(bdd_manager *manager, array_t *bddArray1,
                 array_t *bddArray2) 
{
  int i;
  array_t *resultArray;
  Cal_Bdd *calBddArray, *calBddResultArray;
  bdd_t *operand;
  Cal_BddManager mgr = (Cal_BddManager) manager;
  
  if (array_n(bddArray1) != array_n(bddArray2)){
    fprintf(stderr, "bdd_pairwise_xor: Arrays of different lengths.\n");
    return NIL(array_t);
  }
  calBddArray = Cal_MemAlloc(Cal_Bdd, 2*array_n(bddArray1)+1);
  for (i=0; i<array_n(bddArray1); i++){
    operand = array_fetch(bdd_t*, bddArray1, i);
    calBddArray[i<<1] = operand->calBdd;
    operand = array_fetch(bdd_t*, bddArray2, i);
    calBddArray[(i<<1)+1] = operand->calBdd;
  }
  calBddArray[i<<1] = (Cal_Bdd)0;
  calBddResultArray = Cal_BddPairwiseXor(mgr, calBddArray);
  resultArray = array_alloc(bdd_t*, 0);
  for (i=0; i<array_n(bddArray1); i++){
    array_insert_last(bdd_t *, resultArray, 
                      bdd_construct_bdd_t(mgr, calBddResultArray[i]));
  }
  Cal_MemFree(calBddArray);
  Cal_MemFree(calBddResultArray);
  return resultArray;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_and_smooth(bdd_t *f, bdd_t *g, array_t *smoothingVars)
{
  int num_vars, i;
  bdd_t *fn, *result;
  Cal_Bdd *assoc;
  Cal_BddManager mgr;
  int assocId;
  
  num_vars = array_n(smoothingVars);
  if (num_vars == 0) {
    fprintf(stderr,"bdd_and_smooth: no smoothing variables");
    return bdd_and(f, g, 1, 1);
  }
  mgr = f->bddManager;
  assoc = Cal_MemAlloc(Cal_Bdd, num_vars+1);
  for (i = 0; i < num_vars; i++) {
	fn = array_fetch(bdd_t *, smoothingVars, i);
	assoc[i] = fn->calBdd;
  }
  assoc[num_vars] = 0;
  assocId = Cal_AssociationInit(mgr, assoc, 0);
  Cal_AssociationSetCurrent(mgr, assocId);
  result = bdd_construct_bdd_t(mgr, Cal_BddRelProd(mgr, f->calBdd, g->calBdd));
  Cal_MemFree(assoc);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_between(bdd_t *fMin, bdd_t *fMax)
{
  return bdd_construct_bdd_t(fMin->bddManager,
                             Cal_BddBetween(fMin->bddManager,
                                            fMin->calBdd,
                                            fMax->calBdd));
}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_cofactor(bdd_t *f,bdd_t *g)
{
  return bdd_construct_bdd_t(f->bddManager,
                             Cal_BddCofactor(f->bddManager,
                                             f->calBdd,
                                             g->calBdd));  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_compose(bdd_t *f,bdd_t *v,bdd_t *g)
{
  return bdd_construct_bdd_t(f->bddManager,
                             Cal_BddCompose(f->bddManager,
                                            f->calBdd,
                                            v->calBdd,
                                            g->calBdd)); 
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_consensus(bdd_t *f, array_t *quantifyingVars)
{
  int num_vars, i;
  bdd_t *fn, *result;
  Cal_Bdd *assoc;
  Cal_BddManager mgr;
  
  num_vars = array_n(quantifyingVars);
  if (num_vars == 0) {
    fprintf(stderr, "bdd_consensus: no smoothing variables");
    return f;
  }
  mgr = f->bddManager;
  assoc = Cal_MemAlloc(Cal_Bdd, num_vars+1);
  for (i = 0; i < num_vars; i++) {
	fn = array_fetch(bdd_t *, quantifyingVars, i);
	assoc[i] = fn->calBdd;
  }
  assoc[num_vars] = 0;
  Cal_TempAssociationInit(mgr, assoc, 0);
  Cal_AssociationSetCurrent(mgr, -1);
  result = bdd_construct_bdd_t(mgr, Cal_BddForAll(mgr, f->calBdd));
  Cal_MemFree(assoc);
  Cal_TempAssociationQuit(mgr);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_cproject(bdd_t *f,array_t *quantifyingVars)
{
  return NIL(bdd_t);
  
  /*
  int num_vars, i;
  bdd_t *fn, *result;
  Cal_Bdd_t*assoc;
  Cal_BddManager mgr;
  
  if (f == NIL(bdd_t))
	fail ("bdd_cproject: invalid BDD");
  
  num_vars = array_n(quantifying_vars);
  if (num_vars <= 0) {
      printf("Warning: bdd_cproject: no projection variables\n");
      result = bdd_dup(f);
  }
  else {
    assoc = Cal_MemAlloc(Cal_Bdd_t, num_vars+1);
    for (i = 0; i < num_vars; i++) {
      fn = array_fetch(bdd_t *, quantifying_vars, i);
      assoc[i] = fn->calBdd;
    }
    assoc[num_vars] = (struct bdd_ *) 0;
    mgr = f->bddManager;
    cmu_bdd_temp_assoc(mgr, assoc, 0);
    (void) cmu_bdd_assoc(mgr, -1); 
    result = bdd_construct_bdd_t(mgr, cmu_bdd_project(mgr, f->calBdd));
    Cal_MemFree(assoc);
  }
  return result;
  */
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_else(bdd_t *f)
{
  return bdd_construct_bdd_t(f->bddManager, Cal_BddElse(f->bddManager, f->calBdd));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_ite(bdd_t *i,bdd_t *t,bdd_t *e,boolean i_phase,
        boolean t_phase,boolean e_phase) 
{
  Cal_Bdd temp1, temp2, temp3;
  bdd_t *result;
  Cal_BddManager mgr;
  
  mgr = i->bddManager;
  temp1 = (i_phase ? Cal_BddIdentity(mgr, i->calBdd) : Cal_BddNot(mgr, i->calBdd));
  temp2 = (t_phase ? Cal_BddIdentity(mgr, t->calBdd) : Cal_BddNot(mgr, t->calBdd));
  temp3 = (e_phase ? Cal_BddIdentity(mgr, e->calBdd) : Cal_BddNot(mgr, e->calBdd));
  result = bdd_construct_bdd_t(mgr, Cal_BddITE(mgr, temp1, temp2, temp3));
  Cal_BddFree(mgr, temp1);
  Cal_BddFree(mgr, temp2);
  Cal_BddFree(mgr, temp3);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_minimize(bdd_t *f, bdd_t *c)
{
  return bdd_construct_bdd_t(f->bddManager,
                             Cal_BddReduce(f->bddManager,
                                           f->calBdd, c->calBdd));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_subset(bdd_t *f, int numVars, int threshold)
{
  return bdd_construct_bdd_t(f->bddManager, f->calBdd);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_superset(bdd_t *f, int numVars, int threshold)
{
  return bdd_construct_bdd_t(f->bddManager, f->calBdd);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_not(bdd_t *f)
{
  return bdd_construct_bdd_t(f->bddManager, Cal_BddNot(f->bddManager, f->calBdd));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_one(bdd_manager *manager)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  return bdd_construct_bdd_t(mgr, Cal_BddOne(mgr));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_or(bdd_t *f, bdd_t *g, boolean f_phase, boolean g_phase)
{
  Cal_Bdd temp1, temp2;
  bdd_t *result;
  Cal_BddManager mgr;

  mgr = f->bddManager;
  temp1 = (f_phase ? Cal_BddIdentity(mgr, f->calBdd) : Cal_BddNot(mgr, f->calBdd));
  temp2 = (g_phase ? Cal_BddIdentity(mgr, g->calBdd) : Cal_BddNot(mgr, g->calBdd));
  result = bdd_construct_bdd_t(mgr, Cal_BddOr(mgr, temp1, temp2));
  Cal_BddFree(mgr, temp1);
  Cal_BddFree(mgr, temp2);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_smooth(bdd_t *f, array_t *smoothingVars)
{
  int numVars, i;
  bdd_t *fn, *result;
  Cal_Bdd *assoc;
  Cal_BddManager mgr;
  int assocId;

  numVars = array_n(smoothingVars);
  if (numVars == 0) {
	fprintf(stderr,"bdd_smooth: no smoothing variables");
    return f;
  }
  mgr = f->bddManager;
  assoc = Cal_MemAlloc(Cal_Bdd, numVars+1);
  for (i = 0; i < numVars; i++) {
	fn = array_fetch(bdd_t *, smoothingVars, i);
	assoc[i] = fn->calBdd;
  }
  assoc[numVars] = 0;
  assocId = Cal_AssociationInit(mgr, assoc, 0);
  (void) Cal_AssociationSetCurrent(mgr, assocId);  /* set the temp
                                                 association as the
                                                 current association
                                                 */ 
  result = bdd_construct_bdd_t(mgr, Cal_BddExists(mgr, f->calBdd));
  Cal_MemFree(assoc);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_substitute(bdd_t *f, array_t *old_array, array_t *new_array)
{
  int num_old_vars, num_new_vars, i;
  bdd_t *fn_old, *fn_new, *result;
  Cal_Bdd *assoc;
  Cal_BddManager mgr;
  int assocId;

  num_old_vars = array_n(old_array);
  num_new_vars = array_n(new_array);
  if (num_old_vars != num_new_vars) {
	fprintf(stderr,"bdd_substitute: mismatch of number of new and old variables"); 
    exit(-1);
  }
  mgr = f->bddManager;
  assoc = Cal_MemAlloc(Cal_Bdd, 2*num_old_vars+1);
  for (i = 0; i < num_old_vars; i++) {
	fn_old = array_fetch(bdd_t *, old_array, i);
	fn_new = array_fetch(bdd_t *, new_array, i);
	assoc[2*i]   = fn_old->calBdd;
	assoc[2*i+1] = fn_new->calBdd;
  }
  assoc[2*num_old_vars] = 0;
  assocId = Cal_AssociationInit(mgr, assoc, 1);
  (void) Cal_AssociationSetCurrent(mgr, assocId);  
  result = bdd_construct_bdd_t(mgr, Cal_BddSubstitute(mgr, f->calBdd));
  Cal_MemFree(assoc);
  Cal_TempAssociationQuit(mgr);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_then(bdd_t *f)
{
  return bdd_construct_bdd_t(f->bddManager, Cal_BddThen(f->bddManager, f->calBdd));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_top_var(bdd_t *f)
{
  return bdd_construct_bdd_t(f->bddManager, Cal_BddIf(f->bddManager, f->calBdd));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_xnor(bdd_t *f,bdd_t *g)
{
  return bdd_construct_bdd_t(f->bddManager, Cal_BddXnor(f->bddManager, f->calBdd, g->calBdd));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_xor(bdd_t *f,bdd_t *g)
{
  return bdd_construct_bdd_t(f->bddManager, Cal_BddXor(f->bddManager, f->calBdd, g->calBdd));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_zero(bdd_manager *manager)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  return bdd_construct_bdd_t(mgr, Cal_BddZero(mgr));
}

/*
Queries about BDD Formulas ----------------------------------------------------
*/

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
boolean
bdd_equal(bdd_t *f,bdd_t *g)
{
  return Cal_BddIsEqual(f->bddManager, f->calBdd, g->calBdd);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_intersects(bdd_t *f,bdd_t *g)
{
  return bdd_construct_bdd_t(f->bddManager, Cal_BddIntersects(f->bddManager,
                                                       f->calBdd,
                                                       g->calBdd)); 
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
boolean
bdd_is_tautology(bdd_t *f, boolean phase)
{
  return ((phase == TRUE) ? Cal_BddIsBddOne(f->bddManager, f->calBdd):
          Cal_BddIsBddZero(f->bddManager, f->calBdd));
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
boolean
bdd_leq(bdd_t *f, bdd_t *g, boolean f_phase, boolean g_phase)
{
  Cal_Bdd temp1, temp2, impliesFn;
  Cal_BddManager mgr;
  boolean resultValue;
  
  mgr = f->bddManager;
  temp1 = (f_phase ? Cal_BddIdentity(mgr, f->calBdd) : Cal_BddNot(mgr, f->calBdd));
  temp2 = (g_phase ? Cal_BddIdentity(mgr, g->calBdd) : Cal_BddNot(mgr, g->calBdd));
  impliesFn = Cal_BddImplies(mgr, temp1, temp2); /* returns a minterm
                                                     of temp1*!temp2
                                                     */ 
  resultValue = Cal_BddIsBddZero(mgr, impliesFn);
  Cal_BddFree(mgr, temp1);
  Cal_BddFree(mgr, temp2);
  Cal_BddFree(mgr, impliesFn);
  return resultValue;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
double 
bdd_count_onset(bdd_t *f, array_t *var_array)
{
  int numVars;
  double fraction;
  numVars = array_n(var_array);
  fraction = Cal_BddSatisfyingFraction(f->bddManager, f->calBdd);
  return (fraction * pow((double) 2, (double) numVars));
}
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
int
bdd_get_free(bdd_t *f)
{
    return (f->free);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_manager *
bdd_get_manager(bdd_t *f)
{
    return (bdd_manager *) (f->bddManager);
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
var_set_t *
bdd_get_support(bdd_t *f)
{
  Cal_Bdd *support, var;
  Cal_BddManager mgr;
  long num_vars;
  var_set_t *result;
  int id, i;
  
  mgr = f->bddManager;
  num_vars = Cal_BddVars(mgr);
  result = var_set_new((int) num_vars);
  support = Cal_MemAlloc(Cal_Bdd, (num_vars+1) * sizeof(Cal_Bdd));
  for (i = 0; i <= num_vars; ++i) {
	support[i] = 0; 
  }
  
  (void) Cal_BddSupport(mgr, f->calBdd, support);
  for (i = 0; i < num_vars; ++i) {
	var = support[i]; 
	if (var) {
      id = (int) (Cal_BddGetIfId(mgr, var) - 1);
      var_set_set_elt(result, id);
    }
  }
  
  Cal_MemFree(support);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
array_t *
bdd_get_varids(array_t *var_array)
{
  int i;
  bdd_t *var;
  array_t *result;
  bdd_variableId varId;
  
  result = array_alloc(bdd_variableId, 0);
  for (i = 0; i < array_n(var_array); i++) {
    var = array_fetch(bdd_t *, var_array, i);
    varId = (int) bdd_top_var_id(var);
    array_insert_last(bdd_variableId, result, varId);
  }
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
unsigned int 
bdd_num_vars(bdd_manager *manager)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  return (Cal_BddVars(mgr));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
void
bdd_print(bdd_t *f)
{
  Cal_BddPrintBdd(f->bddManager, f->calBdd, Cal_BddNamingFnNone,
                  Cal_BddTerminalIdFnNone, (Cal_Pointer_t) 0, (FILE *)stdout); 
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
void
bdd_print_stats(bdd_manager *manager, FILE *file)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  Cal_BddManagerGC(mgr);
  Cal_BddStats(mgr, file);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
int
bdd_size(bdd_t *f)
{
  return ((int) Cal_BddSize(f->bddManager, f->calBdd, 1));
}


/**Function********************************************************************

  Synopsis    [Computes the number of nodes of a BDD.]

  SideEffects []

******************************************************************************/
int
bdd_node_size(f)
bdd_node *f;
{
    return(0);

} /* end of bdd_node_size */

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
long
bdd_size_multiple(array_t *bdd_array)
{
  long result;
  Cal_Bdd *vector_bdd;
  bdd_t *f;
  int i;
  Cal_BddManager mgr;
  
  if ((bdd_array == NIL(array_t)) || (array_n(bdd_array) == 0))
    return 0;
  
  f = array_fetch(bdd_t*, bdd_array, 0);
  mgr = f->bddManager;
  vector_bdd = Cal_MemAlloc(Cal_Bdd, array_n(bdd_array)+1);
  for(i=0; i<array_n(bdd_array);i++){
    f = array_fetch(bdd_t*, bdd_array, i);
    vector_bdd[i] = f->calBdd;
  }
  vector_bdd[array_n(bdd_array)] = 0;
  result =  Cal_BddSizeMultiple(mgr, vector_bdd,1);
  Cal_MemFree(vector_bdd);
  return result;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_variableId
bdd_top_var_id(bdd_t *f)
{
  return ((bdd_variableId) (Cal_BddGetIfId(f->bddManager, f->calBdd) - 1));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_external_hooks *
bdd_get_external_hooks(bdd_manager *manager)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  return (bdd_external_hooks *) Cal_BddManagerGetHooks(mgr);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
void
bdd_set_gc_mode(bdd_manager *manager,boolean no_gc)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  Cal_BddSetGCMode(mgr, (int) no_gc);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
void 
bdd_dynamic_reordering(bdd_manager *manager, bdd_reorder_type_t
                       algorithm_type, bdd_reorder_verbosity_t verbosity)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
    switch(algorithm_type) {
      case BDD_REORDER_SIFT:
        Cal_BddDynamicReordering(mgr, CAL_REORDER_SIFT);
      break;
      case BDD_REORDER_WINDOW:
        Cal_BddDynamicReordering(mgr, CAL_REORDER_WINDOW);
      break;
      case BDD_REORDER_NONE:
        Cal_BddDynamicReordering(mgr, CAL_REORDER_NONE);
      break;
      default:
        fprintf(stderr,"CAL: bdd_dynamic_reordering: unknown algorithm type\n");
        fprintf(stderr,"using SIFT method\n");
        Cal_BddDynamicReordering(mgr, CAL_REORDER_SIFT);
    }
  
}

void 
bdd_dynamic_reordering_zdd(bdd_manager *manager, bdd_reorder_type_t
                       algorithm_type, bdd_reorder_verbosity_t verbosity)
{
    return;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
void 
bdd_reorder(bdd_manager *manager)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  Cal_BddReorder(mgr);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_variableId
bdd_get_id_from_level(bdd_manager *manager, long level)
{
  Cal_Bdd fn;
  Cal_BddManager mgr = (Cal_BddManager) manager;
  fn = Cal_BddManagerGetVarWithIndex(mgr, level);
  if (!fn){
    /* variable should always be found, since they are created at bdd_start */
    fprintf(stderr, "bdd_get_id_from_level: assumption violated");
    exit(-1);
  }
  return ((bdd_variableId)(Cal_BddGetIfId(mgr, fn) - 1 ));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
long
bdd_top_var_level(bdd_manager *manager, bdd_t *fn)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  return (long)Cal_BddGetIfIndex(mgr, fn->calBdd);
}

/*
 */
/**Function********************************************************************

  Synopsis           [Return TRUE if f is a cube, else return FALSE.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
boolean
bdd_is_cube(bdd_t *f)
{
  Cal_BddManager mgr;
  if (f == NIL(bdd_t)) {
    fail("bdd_is_cube: invalid BDD");
  }
  if(f->free) fail ("Freed Bdd passed to bdd_is_cube");
  mgr = f->bddManager;
  return ((boolean)Cal_BddIsCube(mgr, f->calBdd));
}

/**Function********************************************************************

  Synopsis           []

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_block *
bdd_new_var_block(bdd_t *f, long length)
{
	return (bdd_block *) Cal_BddNewVarBlock(f->bddManager, f->calBdd, length);
}

/**Function********************************************************************

  Synopsis           [Return TRUE if f is a cube, else return FALSE.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
bdd_t *
bdd_var_with_index(bdd_manager *manager, int index)
{
  Cal_BddManager mgr = (Cal_BddManager) manager;
  return bdd_construct_bdd_t(mgr,
                             Cal_BddManagerGetVarWithIndex(mgr,
                                                           index)); 
}


/**Function********************************************************************

  Synopsis [Sets the internal parameters of the package to the given values.]

  Description [The CAL package has a set of parameters that can be assigned
  different values. This function receives a table which maps strings to
  values and sets the parameters represented by the strings to the pertinent
  values. Some basic type checking is done. It returns 1 if everything is
  correct and 0 otherwise.]

  SideEffects []

******************************************************************************/
int
bdd_set_parameters(mgr, valueTable, file)
bdd_manager *mgr;
avl_tree *valueTable;
FILE *file;
{
  /* int reorderMethod; */
  st_table *newValueTable;
  st_generator *stgen;
  avl_generator *avlgen;
  char *paramName;
  char *paramValue;
  Cal_BddManager bddManager = (Cal_BddManager)mgr;
  

  /* Build a new table with the parameter names but with
  ** the prefix removed. */
  newValueTable = st_init_table(st_ptrcmp, st_ptrhash);
  avl_foreach_item(valueTable, avlgen, AVL_FORWARD, (char **)&paramName, 
                   (char **)&paramValue) {
    if (strncmp(paramName, "BDD.", 4) == 0) {
      st_insert(newValueTable, (char *)&paramName[4],
                (char *)paramValue);
    }
  }
  
  st_foreach_item(newValueTable, stgen, (char **)&paramName, 
                  (char **)&paramValue) {
    unsigned int uvalue;
    double value;
    char *invalidChar;
    
    invalidChar = NIL(char);
    
    if (strcmp(paramName, "Node limit") == 0) {
      uvalue = (unsigned int) strtol(paramValue, &invalidChar, 10);
      /* RB an unsigned will never be < 0 */
      if (*invalidChar /*|| uvalue < 0*/) {
	InvalidType(file, "Node limit", "unsigned integer");
      }
      else {
	bddManager->nodeLimit = uvalue;
      }
    }
    else if (strcmp(paramName, "Garbage collection enabled") == 0) {
      if (strcmp(paramValue, "yes") == 0) {
	bddManager->gcMode = 1;
      }
      else if (strcmp(paramValue, "no") == 0) {
	bddManager->gcMode = 0;
      }
      else {
	InvalidType(file, "Garbage collection enabled", "(yes,no)");
      }
    }
    else if (strcmp(paramName, 
		    "Maximum number of variables sifted per reordering") == 0) {
      uvalue = (unsigned int) strtol(paramValue, &invalidChar, 10);
      if (*invalidChar /*|| uvalue < 0*/) {
	InvalidType(file, "Maximum number of variables sifted per reordering",
		    "unsigned integer");
      }
      else {
	bddManager->maxNumVarsSiftedPerReordering = uvalue;
      }
    }
    else if (strcmp(paramName,
		    "Maximum number of variable swaps per reordering") == 0) {
      uvalue = (unsigned int) strtol(paramValue, &invalidChar, 10);
      if (*invalidChar /*|| uvalue < 0*/) {
	InvalidType(file, "Maximum number of variable swaps per reordering", 
		    "unsigned integer");
      }
      else {
	bddManager->maxNumSwapsPerReordering = uvalue;
      }
    }
    else if (strcmp(paramName, 
		    "Maximum growth while sifting a variable") == 0) {
      value = strtod(paramValue, &invalidChar);
      if (*invalidChar) {
	InvalidType(file, "Maximum growth while sifting a variable",
		    "real");
      }
      else {
	bddManager->maxSiftingGrowth = value;
      }
    }
    else if (strcmp(paramName, "Dynamic reordering of BDDs enabled") == 0) {
      if (strcmp(paramValue, "yes") == 0) {
	bddManager->dynamicReorderingEnableFlag = 1;
      }
      else if (strcmp(paramValue, "no") == 0) {
	bddManager->dynamicReorderingEnableFlag = 0;
      }
      else {
	InvalidType(file, "Dynamic reordering of BDDs enabled",
		    "(yes,no)");
      }
    }
    else if (strcmp(paramName, "Use old reordering")
	     == 0) {
      if (strcmp(paramValue, "yes") == 0) {
	bddManager->reorderMethod = CAL_REORDER_METHOD_BF;
      }
      else if (strcmp(paramValue, "no") == 0) {
	bddManager->reorderMethod = CAL_REORDER_METHOD_DF;
      }
      else {
	InvalidType(file, "Dynamic reordering of BDDs enabled",
		    "(yes,no)");
      }
    }
    else if (strcmp(paramName, "Dynamic reordering threshold") == 0) {
      uvalue = (unsigned int) strtol(paramValue, &invalidChar, 10);
      if (*invalidChar /*|| uvalue < 0*/) {
        InvalidType(file, "Dynamic reordering threshold", "unsigned integer");
      }
      else {
        bddManager->reorderingThreshold = uvalue;
      }
    }
    else if (strcmp(paramName, "Repacking after GC threshold")
	     == 0) {
      value = strtod(paramValue, &invalidChar);
      if (*invalidChar || value < 0) {
        InvalidType(file, "Repacking after GC threshold", "unsigned real");
      }
      else {
        bddManager->repackAfterGCThreshold = value;
      }
    }
    else if (strcmp(paramName, "Table repacking threshold")
	     == 0) {
      value = strtod(paramValue, &invalidChar);
      if (*invalidChar || value < 0) {
        InvalidType(file, "Table repacking threshold", "unsigned real");
      }
      else {
        bddManager->tableRepackThreshold = value;
      }
    }
    else {
      (void) fprintf(file, "Warning: Parameter %s not recognized.",
                     paramName);
      (void) fprintf(file, " Ignored.\n");
    }
  } /* end of st_foreach_item */
  
  /* Clean up. */
  st_free_table(newValueTable);
  
  return(1);
  
} /* end of bdd_set_parameters */

/**Function********************************************************************

  Synopsis    [Dummy functions defined in bdd.h]

  SideEffects []

******************************************************************************/
bdd_t *
bdd_compact(f, g)
bdd_t *f;
bdd_t *g;
{
    return 0;
}

bdd_t *
bdd_squeeze(f, g)
bdd_t *f;
bdd_t *g;
{
    return 0;
}
bdd_t *
bdd_clipping_and_smooth(f, g, smoothing_vars, maxDepth, over)
bdd_t *f;
bdd_t *g;
array_t *smoothing_vars;	/* of bdd_t *'s */
int maxDepth;
int over;
{
  return NIL(bdd_t);
}

bdd_t *
bdd_approx_hb(f, approxDir, numVars, threshold)
bdd_t *f;
bdd_approx_dir_t approxDir;
int numVars;
int threshold;
{
  return NIL(bdd_t);
}

bdd_t *
bdd_approx_sp(f, approxDir, numVars, threshold, hardlimit)
bdd_t *f;
bdd_approx_dir_t approxDir;
int numVars;
int threshold;
int hardlimit;
{
  return NIL(bdd_t);
}

bdd_t *
bdd_approx_ua(f, approxDir, numVars, threshold, safe, quality)
bdd_t *f;
bdd_approx_dir_t approxDir;
int numVars;
int threshold;
int safe;
double quality;
{
  return NIL(bdd_t);
}

bdd_t *
bdd_approx_remap_ua(f, approxDir, numVars, threshold, quality)
bdd_t *f;
bdd_approx_dir_t approxDir;
int numVars;
int threshold;
double quality;
{
  return NIL(bdd_t);
}

bdd_t *
bdd_approx_biased_rua(f, approxDir, bias, numVars, threshold, quality, quality1)
bdd_t *f;
bdd_approx_dir_t approxDir;
bdd_t *bias;
int numVars;
int threshold;
double quality;
double quality1;
{
  return NIL(bdd_t);
}

bdd_t *
bdd_approx_compress(f, approxDir, numVars, threshold)
bdd_t *f;
bdd_approx_dir_t approxDir;
int numVars;
int threshold;
{
  return NIL(bdd_t);
}



int
bdd_gen_decomp(f,  partType, conjArray)
bdd_t *f;
bdd_partition_type_t partType;
bdd_t ***conjArray;
{
  return 0;
}

int
bdd_var_decomp(f,  partType, conjArray)
bdd_t *f;
bdd_partition_type_t partType;
bdd_t ***conjArray;
{
  return 0;
}

int 
bdd_approx_decomp(f,  partType, conjArray)
bdd_t *f;
bdd_partition_type_t partType;
bdd_t ***conjArray;
{
  return 0;
}

int
bdd_add_hook(mgr, procedure, whichHook)
bdd_manager *mgr;
int (*procedure)(bdd_manager *, char *, void *);
bdd_hook_type_t whichHook;
{
  return 0;
}

int
bdd_remove_hook(mgr, procedure, whichHook)
bdd_manager *mgr;
int (*procedure)(bdd_manager *, char *, void *);
bdd_hook_type_t whichHook;
{
  return 0;
}

int
bdd_enable_reordering_reporting(mgr)
bdd_manager *mgr;
{
  return 0;
}
int
bdd_disable_reordering_reporting(mgr)
bdd_manager *mgr;
{
  return 0;
}

bdd_reorder_verbosity_t 
bdd_reordering_reporting(mgr)
bdd_manager *mgr;
{
  return BDD_REORDER_VERBOSITY_DEFAULT;
}

int 
bdd_print_apa_minterm(fp, f, nvars, precision)
FILE *fp;
bdd_t *f;
int nvars;
int precision;
{
  return 0;
}

int 
bdd_apa_compare_ratios(nvars, f1, f2, f1Num, f2Num)
int nvars;
bdd_t *f1;
bdd_t *f2;
int f1Num;
int f2Num;
{
  return 0;
}

int
bdd_iter_decomp(f,  partType, conjArray)
bdd_t *f;
bdd_partition_type_t partType;
bdd_t  ***conjArray;
{
  return 0;
}

int
bdd_reordering_status(mgr, method)
bdd_manager *mgr;
bdd_reorder_type_t *method;
{
  return 0;
}

int
bdd_read_node_count(mgr)
bdd_manager *mgr;
{
  return 0;
}

double
bdd_correlation(f, g)
bdd_t *f;
bdd_t *g;
{
    return 0.0;
}

array_t *
bdd_bdd_pick_arbitrary_minterms(f, varsArray, n, k)
bdd_t *f;
array_t *varsArray;
int n;
int k;
{
    return NIL(array_t);
}


int
bdd_reordering_zdd_status(mgr, method)
bdd_manager *mgr;
bdd_reorder_type_t *method;
{
  return 0;
}


bdd_node *
bdd_bdd_to_add(mgr, fn)
bdd_manager *mgr;
bdd_node *fn;
{
  return NIL(bdd_node);
}

bdd_node *
bdd_add_permute(mgr, fn, permut)
bdd_manager *mgr;
bdd_node *fn;
int *permut;
{
  return NIL(bdd_node);
}

bdd_node *
bdd_bdd_permute(mgr, fn, permut)
bdd_manager *mgr;
bdd_node *fn;
int *permut;
{
  return NIL(bdd_node);
}


void
bdd_ref(fn)
bdd_node *fn;
{
  return ;
}


void
bdd_recursive_deref(mgr, f)
bdd_manager *mgr;
bdd_node *f;
{
  return;
}



bdd_node *
bdd_add_exist_abstract(mgr, fn, vars)
bdd_manager *mgr;
bdd_node *fn;
bdd_node *vars;
{
  return NIL(bdd_node);
}


bdd_node *
bdd_add_apply(mgr, operation, fn1, fn2)
bdd_manager *mgr;
bdd_node *(*operation)();
bdd_node *fn1;
bdd_node *fn2;
{
  return NIL(bdd_node);
}


bdd_node *
bdd_add_nonsim_compose(mgr, fn, vector)
bdd_manager *mgr;
bdd_node *fn;
bdd_node **vector;
{
  return NIL(bdd_node);
}


bdd_node *
bdd_add_residue(mgr, n, m, options, top)
bdd_manager *mgr;
int n;
int m;
int options;
int top;
{
  return NIL(bdd_node);
}


bdd_node *
bdd_add_vector_compose(mgr, fn, vector)
bdd_manager *mgr;
bdd_node *fn;
bdd_node **vector;
{
  return NIL(bdd_node);
}


bdd_node *
bdd_add_times(mgr, fn1, fn2)
bdd_manager *mgr;
bdd_node **fn1;
bdd_node **fn2;
{
  return NIL(bdd_node);
}
  

int
bdd_check_zero_ref(mgr)
bdd_manager *mgr;
{
  return 0;
}


void
bdd_dynamic_reordering_disable(mgr)
bdd_manager *mgr;
{
  return;
}

void
bdd_dynamic_reordering_zdd_disable(mgr)
bdd_manager *mgr;
{
  return;
}


bdd_node *
bdd_add_xnor(mgr, fn1, fn2)
bdd_manager *mgr;
bdd_node **fn1;
bdd_node **fn2;
{
  return NIL(bdd_node);
}


int
bdd_shuffle_heap(mgr, permut)
bdd_manager *mgr;
int *permut;
{
  return 0;
}


bdd_node *
bdd_add_compose(mgr, fn1, fn2, var)
bdd_manager *mgr;
bdd_node *fn1;
bdd_node *fn2;
int var;
{
  return NIL(bdd_node);
}


bdd_node *
bdd_add_ith_var(mgr, i)
bdd_manager *mgr;
int i;
{
  return NIL(bdd_node);
}


int
bdd_get_level_from_id(mgr, id)
bdd_manager *mgr;
int id;
{
  return 0;
}


bdd_node *
bdd_bdd_exist_abstract(mgr, fn, cube)
bdd_manager *mgr;
bdd_node *fn;
bdd_node *cube;
{
  return NIL(bdd_node);
}


int
bdd_equal_sup_norm(mgr, fn, gn, tolerance, pr)
bdd_manager *mgr;
bdd_node *fn;
bdd_node *gn;
BDD_VALUE_TYPE tolerance;
int pr;
{
  return 0;
}


bdd_node *
bdd_read_logic_zero(mgr)
bdd_manager *mgr;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_bdd_ith_var(mgr, i)
bdd_manager *mgr;
int i;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_add_divide(mgr, fn1, fn2)
bdd_manager *mgr;
bdd_node **fn1;
bdd_node **fn2;
{
  return NIL(bdd_node);
}


bdd_node *
bdd_bdd_constrain(mgr, f, c)
bdd_manager *mgr;
bdd_node *f;
bdd_node *c;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_bdd_restrict(mgr, f, c)
bdd_manager *mgr;
bdd_node *f;
bdd_node *c;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_add_hamming(mgr, xVars, yVars, nVars)
bdd_manager *mgr;
bdd_node **xVars;
bdd_node **yVars;
int nVars;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_add_ite(mgr, f, g, h)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
bdd_node *h;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_add_find_max(mgr, f)
bdd_manager *mgr;
bdd_node *f;
{
    return NIL(bdd_node);
}


int
bdd_bdd_pick_one_cube(mgr, node, string)
bdd_manager *mgr;
bdd_node *node;
char *string;
{
    return 0;
}


bdd_node *
bdd_add_swap_variables(mgr, f, x, y, n)
bdd_manager *mgr;
bdd_node *f;
bdd_node **x;
bdd_node **y;
int n;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_bdd_swap_variables(mgr, f, x, y, n)
bdd_manager *mgr;
bdd_node *f;
bdd_node **x;
bdd_node **y;
int n;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_bdd_or(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return NIL(bdd_node);
}

bdd_t *
bdd_compute_cube(mgr, vars)
bdd_manager *mgr;
array_t *vars;
{
  return NIL(bdd_t);
}



bdd_node *
bdd_bdd_compute_cube(mgr, vars, phase, n)
bdd_manager *mgr;
bdd_node **vars;
int *phase;
int n;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_bdd_and(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_add_matrix_multiply(mgr, A, B, z, nz)
bdd_manager *mgr;
bdd_node *A;
bdd_node *B;
bdd_node **z;
int nz;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_add_compute_cube(mgr, vars, phase, n)
bdd_manager *mgr;
bdd_node **vars;
int *phase;
int n;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_add_const(mgr, c)
bdd_manager *mgr;
BDD_VALUE_TYPE c;
{
    return NIL(bdd_node);
}


double
bdd_count_minterm(mgr, f, n)
bdd_manager *mgr;
bdd_node *f;
int n;
{
    return 0;
}


bdd_node *
bdd_add_bdd_threshold(mgr, f, value)
bdd_manager *mgr;
bdd_node *f;
BDD_VALUE_TYPE value;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_add_bdd_strict_threshold(mgr, f, value)
bdd_manager *mgr;
bdd_node *f;
BDD_VALUE_TYPE value;
{
    return NIL(bdd_node);
}

BDD_VALUE_TYPE
bdd_read_epsilon(mgr)
bdd_manager *mgr;
{
    return 0;
}

bdd_node *
bdd_read_one(mgr)
bdd_manager *mgr;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_bdd_pick_one_minterm(mgr, f, vars, n)
bdd_manager *mgr;
bdd_node *f;
bdd_node **vars;
int n;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_read_zero(mgr)
bdd_manager *mgr;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_bdd_new_var(mgr)
bdd_manager *mgr;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_bdd_and_abstract(mgr, f, g, cube)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
bdd_node *cube;
{
    return NIL(bdd_node);
}

void
bdd_deref(f)
bdd_node *f;
{
    return;
}

bdd_node *
bdd_add_plus(mgr, fn1, fn2)
bdd_manager *mgr;
bdd_node **fn1;
bdd_node **fn2;
{
  return NIL(bdd_node);
}


int
bdd_read_reorderings(mgr)
bdd_manager *mgr;
{
    return 0;
}

bdd_node *
bdd_bdd_xnor(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_bdd_vector_compose(mgr, f, vector)
bdd_manager *mgr;
bdd_node *f;
bdd_node **vector;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_extract_node_as_is(fn)
bdd_t *fn;
{
  return NIL(bdd_node);
}


bdd_node *
bdd_zdd_get_node(mgr, id, g, h)
bdd_manager *mgr;
int id;
bdd_node *g;
bdd_node *h;
{
    return NIL(bdd_node);
}


bdd_node *
bdd_zdd_get_univ(mgr, id)
bdd_manager *mgr;
int id;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_zdd_product(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_zdd_product_recur(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return NIL(bdd_node);
}
bdd_node *
bdd_zdd_union(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return(NIL(bdd_node));
}


bdd_node *
bdd_zdd_weak_div(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_zdd_weak_div_recur(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_zdd_isop_recur(mgr, L, U, zdd_I)
bdd_manager *mgr;
bdd_node *L;
bdd_node *U;
bdd_node **zdd_I;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_zdd_isop(mgr, L, U, zdd_I)
bdd_manager *mgr;
bdd_node *L;
bdd_node *U;
bdd_node **zdd_I;
{
    return NIL(bdd_node);
}

int
bdd_zdd_get_cofactors3(mgr, f, v, f1, f0, fd)
bdd_manager *mgr;
bdd_node *f;
int v;
bdd_node **f1;
bdd_node **f0;
bdd_node **fd;
{
    return 0;
}

bdd_node *
bdd_bdd_and_recur(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
     return NIL(bdd_node);
}

bdd_node *
bdd_unique_inter(mgr, v, f, g)
bdd_manager *mgr;
int v;
bdd_node *f;
bdd_node *g;
{
     return NIL(bdd_node);
}

bdd_node *
bdd_unique_inter_ivo(mgr, v, f, g)
bdd_manager *mgr;
int v;
bdd_node *f;
bdd_node *g;
{
     return NIL(bdd_node);
}


bdd_node *
bdd_zdd_diff(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return NIL(bdd_node);
} 
bdd_node *
bdd_zdd_diff_recur(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return NIL(bdd_node);
} 

int
bdd_num_zdd_vars(mgr)
bdd_manager *mgr;
{
    return -1;
}

bdd_node *
bdd_regular(f)
bdd_node *f;
{
    return NIL(bdd_node);
}

int
bdd_is_constant(f)
bdd_node *f;
{
    return 0;
}

int
bdd_is_complement(f)
bdd_node *f;
{
    return 0;
}

bdd_node *
bdd_bdd_T(f)
bdd_node *f;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_bdd_E(f)
bdd_node *f;
{
    return NIL(bdd_node);
}

bdd_node *
bdd_not_bdd_node(f)
bdd_node *f;
{
    return NIL(bdd_node);
} 

void
bdd_recursive_deref_zdd(mgr,f)
bdd_manager *mgr;
bdd_node *f;
{
    return;
} 

int
bdd_zdd_count(mgr, f)
bdd_manager *mgr;
bdd_node *f;
{
    return 0;
}

int
bdd_read_zdd_level(mgr, index)
bdd_manager *mgr;
int index;
{
    return -1;
} 

int
bdd_zdd_vars_from_bdd_vars(mgr, multiplicity)
bdd_manager *mgr;
int multiplicity;
{
   return 0;
} 

void
bdd_zdd_realign_enable(mgr)
bdd_manager *mgr;
{
    return;
} 
void
bdd_zdd_realign_disable(mgr)
bdd_manager *mgr;
{
    return;
} 

int
bdd_zdd_realignment_enabled(mgr)
bdd_manager *mgr;
{
    return 0;
} 

void
bdd_realign_enable(mgr)
bdd_manager *mgr;
{
    return;
} 

void
bdd_realign_disable(mgr)
bdd_manager *mgr;
{
    return;
} 

int
bdd_realignment_enabled(mgr)
bdd_manager *mgr;
{
    return 0;
} 

int
bdd_node_read_index(f)
bdd_node *f;
{
    return -1;
}

bdd_node *
bdd_read_next(f)
bdd_node *f;
{
    return NIL(bdd_node);
}

void
bdd_set_next(f, g)
bdd_node *f;
bdd_node *g;
{
    return;
}

int
bdd_read_reordered_field(mgr)
bdd_manager *mgr;
{
    return -1;
}

void
bdd_set_reordered_field(mgr, n)
bdd_manager *mgr;
int n;
{
    return;
}

bdd_node *
bdd_add_apply_recur(mgr, operation, fn1, fn2)
bdd_manager *mgr;
bdd_node *(*operation)();
bdd_node *fn1;
bdd_node *fn2;
{
    return NIL(bdd_node);
}


BDD_VALUE_TYPE
bdd_add_value(f)
bdd_node *f;
{
    return 0.0; 
}

int
bdd_print_minterm(f)
bdd_t *f;
{
  return 0;
}


bdd_t *
bdd_xor_smoth(f, g, smoothing_vars)
bdd_t *f;
bdd_t *g;
array_t *smoothing_vars;
{
    return NIL(bdd_t);
}


bdd_node *
bdd_read_plus_infinity(mgr)
bdd_manager *mgr;
{
    return NIL(bdd_node);

} /* end of bdd_read_plus_infinity */



bdd_node *
bdd_priority_select(mgr,R,x,y,z,Pi,n,Pifunc)
bdd_manager *mgr;
bdd_node *R,
    **x,**y,**z,*Pi;
int n;
bdd_node  *(*Pifunc)();
{
    return NIL(bdd_node);

} /* end of bdd_priority_select */


void
bdd_set_background(mgr,f)
bdd_manager *mgr;
bdd_node *f;
{
    return;
 
} /* end of bdd_set_background */



bdd_node *
bdd_read_background(mgr)
bdd_manager *mgr;
{
  return NIL(bdd_node);

} /* end of bdd_read_background */



bdd_node *
bdd_bdd_cofactor(mgr,f,g)
bdd_manager *mgr;
bdd_node *f,*g;
{

    return NIL(bdd_node);

} /* end of bdd_bdd_cofactor */



bdd_node *
bdd_bdd_ite(mgr,f,g,h)
bdd_manager *mgr;
bdd_node *f,*g,*h;
{

    return NIL(bdd_node);

} /* end of bdd_bdd_ite */



bdd_node *
bdd_add_minus(mgr, fn1, fn2)
bdd_manager *mgr;
bdd_node **fn1;
bdd_node **fn2;
{

    return NIL(bdd_node);

} /* end of bdd_add_plus */



bdd_node *
bdd_dxygtdxz(mgr, N, x, y, z)
bdd_manager *mgr;
int N;
bdd_node **x, **y, **z;
{

    return NIL(bdd_node);

} /* end of bdd_dxygtdxz */



bdd_node *
bdd_bdd_univ_abstract(mgr, fn, vars)
bdd_manager *mgr;
bdd_node *fn;
bdd_node *vars;
{

    return NIL(bdd_node);

} /* end of bdd_bdd_univ_abstract */



bdd_node *
bdd_bdd_cprojection(mgr,R,Y)
bdd_manager *mgr;
bdd_node *R;
bdd_node *Y;
{

    return NIL(bdd_node);

} /* end of bdd_bdd_cprojection */

bdd_node *
bdd_xeqy(mgr, N, x, y)
bdd_manager *mgr;
int N;
bdd_node **x, **y;
{
  return NIL(bdd_node);

} /* end of bdd_xeqy */

bdd_node *
bdd_add_roundoff(mgr, f, N)
bdd_manager *mgr;
bdd_node *f;
int N;
{
  return NIL(bdd_node);

} /* end of bdd_add_roundoff */

bdd_node *
bdd_xgty(mgr, N, x, y)
bdd_manager *mgr;
int N;
bdd_node **x, **y;
{
  return NIL(bdd_node);

} /* end of bdd_xgty */

bdd_node *
bdd_add_cmpl(mgr, f)
bdd_manager *mgr;
bdd_node *f;
{
  return NIL(bdd_node);

} /* end of bdd_add_cmpl */

bdd_node *
bdd_split_set(mgr, f, x, n, m)
bdd_manager *mgr;
bdd_node *f;
bdd_node **x;
int n;
double m;
{
  return NIL(bdd_node);

} /* end of bdd_split_set */



int
bdd_debug_check(mgr)
bdd_manager *mgr;
{
    return -1;

} /* end of bdd_debug_check */

bdd_node *
bdd_bdd_xor(mgr, f, g)
bdd_manager *mgr;
bdd_node *f;
bdd_node *g;
{
    return NIL(bdd_node);
}

void 
bdd_dump_blif(mgr, nBdds, bdds, inames, onames, model, fp)
bdd_manager *mgr;
int nBdds;
bdd_node **bdds;
char **inames;
char **onames;
char *model;
FILE *fp;
{
  return;
}

void 
bdd_dump_blif_body(mgr, nBdds, bdds, inames, onames, fp)
bdd_manager *mgr;
int nBdds;
bdd_node **bdds;
char **inames;
char **onames;
FILE *fp;
{
  return;
}

bdd_node *
bdd_make_bdd_from_zdd_cover(bdd_manager *mgr, bdd_node *node)
{
    return(NIL(bdd_node));
}

bdd_node *
bdd_zdd_complement(bdd_manager *mgr, bdd_node *node)
{
    return(NIL(bdd_node));
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis [Function to print a warning that an illegal value was read.]

  SideEffects        []

  SeeAlso            [bdd_set_parameters]

******************************************************************************/
static void
InvalidType(file, field, expected)
FILE *file;
char *field;
char *expected;
{
    (void) fprintf(file, "Warning: In parameter \"%s\"\n", field);
    (void) fprintf(file, "Illegal type detected. %s expected\n", expected);

} /* end of InvalidType */
