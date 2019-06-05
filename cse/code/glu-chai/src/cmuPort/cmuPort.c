/**CFile***********************************************************************

  FileName    [cmuPort.c]

  PackageName [cmu_port]

  Synopsis    [Port routines for CMU package.]

  Description [optional]

  SeeAlso     [optional]

  Author      [Thomas R. Shiple. Some changes by Rajeev K. Ranjan.]

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

  Revision    [$Id: cmuPort.c,v 1.1.1.1 2003/04/24 23:35:11 ashwini Exp $]

******************************************************************************/

#include "cmuPortInt.h" 

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

  Synopsis    [Builds the bdd_t structure.]

  Description [Builds the bdd_t structure from manager and node.
  Assumes that the reference count of the node has already been
  increased.]

  SideEffects []

******************************************************************************/
bdd_t *
bdd_construct_bdd_t(bdd_manager *manager, bdd_node *func)
{
    bdd_t *result;
    cmu_bdd_manager mgr = (cmu_bdd_manager)manager;
    bdd fn = (bdd)func;

    if (fn == (struct bdd_ *) 0) {
	cmu_bdd_fatal("bdd_construct_bdd_t: possible memory overflow");
    }

    result = ALLOC(bdd_t, 1);
    result->mgr = mgr;
    result->node = fn;
    result->free = FALSE;
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
  return CMU;
}

/*
BDD Manager Allocation And Destruction ----------------------------------------
*/
void
bdd_end(bdd_manager *manager)
{
    bdd_external_hooks *hooks;
    cmu_bdd_manager mgr = (cmu_bdd_manager)manager;
    hooks = (bdd_external_hooks *) mgr->hooks;
    FREE(hooks); 
    cmu_bdd_quit(mgr);
}


bdd_manager *
bdd_start(nvariables)
int nvariables;
{
    struct bdd_manager_ *mgr;
    int i;
    bdd_external_hooks *hooks;
   
    mgr = cmu_bdd_init();    /*no args*/

    /*
     * Calls to UCB bdd_get_variable are translated into cmu_bdd_var_with_id calls.  However,
     * cmu_bdd_var_with_id assumes that single variable BDDs have already been created for 
     * all the variables that we wish to access.  Thus, following, we explicitly create n
     * variables.  We do not care about the return value of cmu_bdd_new_var_last; in the 
     * CMU package, the single variable BDDs are NEVER garbage collected.
     */
    for (i = 0; i < nvariables; i++) {
	(void) cmu_bdd_new_var_last(mgr);
    }

    hooks = ALLOC(bdd_external_hooks, 1);
    hooks->mdd = hooks->network = hooks->undef1 = (char *) 0;
    mgr->hooks = (char *) hooks;  /* new field added to CMU manager */

    return (bdd_manager *) mgr;
}

/*
BDD Variable Allocation -------------------------------------------------------
*/

bdd_t *
bdd_create_variable(bdd_manager *manager)
{
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  return bdd_construct_bdd_t(mgr, cmu_bdd_new_var_last(mgr));
}

bdd_t *
bdd_create_variable_after(bdd_manager *manager, bdd_variableId after_id)
{
  struct bdd_ *after_var;
  bdd_t 	*result;
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;

  after_var = cmu_bdd_var_with_id(mgr, (long)after_id + 1);
  
  result =  bdd_construct_bdd_t(mgr, cmu_bdd_new_var_after(mgr, after_var));
  
  /* No need to free after_var, since single variable BDDs are never garbage collected */
  
  return result;
}



bdd_t *
bdd_get_variable(bdd_manager *manager, bdd_variableId variable_ID)
{
  struct bdd_ *fn;
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  fn = cmu_bdd_var_with_id(mgr, (long) (variable_ID + 1));

  if (fn == (struct bdd_ *) 0) {
	/* variable should always be found, since they are created at bdd_start */
	cmu_bdd_fatal("bdd_get_variable: assumption violated");
  }
  
  return bdd_construct_bdd_t(mgr, fn);
}

/*
BDD Formula Management --------------------------------------------------------
*/

bdd_t *
bdd_dup(f)
bdd_t *f;
{
  return bdd_construct_bdd_t(f->mgr, cmu_bdd_identity(f->mgr, f->node));
}

void
bdd_free(f)
bdd_t *f;
{
  if (f == NIL(bdd_t)) {
	fail("bdd_free: trying to free a NIL bdd_t");			
  }
  
  if (f->free == TRUE) {
	fail("bdd_free: trying to free a freed bdd_t");			
  }	
  
  cmu_bdd_free(f->mgr, f->node);
  
  /*
   * In case the user tries to free this bdd_t again, set the free field to TRUE, 
     * and NIL out the other fields.  Then free the bdd_t structure itself.
     */
  f->free = TRUE;
  f->node = NIL(struct bdd_);
  f->mgr = NIL(struct bdd_manager_);
  FREE(f);  
}

/*
  Operations on BDD Formulas ----------------------------------------------------
  */

bdd_t *
bdd_and(f, g, f_phase, g_phase)
bdd_t *f;
bdd_t *g;
boolean f_phase;
boolean g_phase;
{
  struct bdd_ *temp1, *temp2;
  bdd_t *result;
  struct bdd_manager_ *mgr;
  
  mgr = f->mgr;
  temp1 = ( (f_phase == TRUE) ? cmu_bdd_identity(mgr, f->node) : cmu_bdd_not(mgr, f->node));
  temp2 = ( (g_phase == TRUE) ? cmu_bdd_identity(mgr, g->node) : cmu_bdd_not(mgr, g->node));
  result = bdd_construct_bdd_t(mgr, cmu_bdd_and(mgr, temp1, temp2));
  cmu_bdd_free(mgr, temp1);
  cmu_bdd_free(mgr, temp2);
  return result;
}

bdd_t *
bdd_multiway_and(bdd_manager *manager, array_t *bddArray)
{
  int i;
  bdd temp, result;
  bdd_t *operand;
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  result = cmu_bdd_one(mgr);
  for (i=0; i<array_n(bddArray); i++){
    operand = array_fetch(bdd_t *, bddArray, i);
    temp = cmu_bdd_and(mgr, result, operand->node);
    cmu_bdd_free(mgr, result);
    result = temp;
  }
  return bdd_construct_bdd_t(mgr, result);
}

bdd_t *
bdd_multiway_or(bdd_manager *manager, array_t *bddArray)
{
  int i;
  bdd temp, result;
  bdd_t *operand;
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  result = cmu_bdd_zero(mgr);
  for (i=0; i<array_n(bddArray); i++){
    operand = array_fetch(bdd_t *, bddArray, i);
    temp = cmu_bdd_or(mgr, result, operand->node);
    cmu_bdd_free(mgr, result);
    result = temp;
  }
  return bdd_construct_bdd_t(mgr, result);
}

bdd_t *
bdd_multiway_xor(bdd_manager *manager, array_t *bddArray)
{
  int i;
  bdd temp, result;
  bdd_t *operand;
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  result = cmu_bdd_zero(mgr);
  for (i=0; i<array_n(bddArray); i++){
    operand = array_fetch(bdd_t *, bddArray, i);
    temp = cmu_bdd_xor(mgr, result, operand->node);
    cmu_bdd_free(mgr, result);
    result = temp;
  }
  return bdd_construct_bdd_t(mgr, result);
}

array_t *
bdd_pairwise_or(bdd_manager *manager, array_t *bddArray1, array_t
                *bddArray2) 
{
  int i;
  bdd_t *operand1, *operand2;
  array_t *resultArray;
  bdd result;
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  if (array_n(bddArray1) != array_n(bddArray2)){
    fprintf(stderr, "bdd_pairwise_or: Arrays of different lengths.\n");
    return NIL(array_t);
  }
  resultArray = array_alloc(bdd_t *, 0);
  for (i=0; i<array_n(bddArray1); i++){
    operand1 = array_fetch(bdd_t *, bddArray1, i);
    operand2 = array_fetch(bdd_t *, bddArray2, i);
    result = cmu_bdd_or(mgr, operand1->node, operand2->node);
    array_insert_last(bdd_t*, resultArray,
                      bdd_construct_bdd_t(mgr, result)); 
  }
  return resultArray;
}

array_t *
bdd_pairwise_and(bdd_manager *manager, array_t *bddArray1, array_t
                 *bddArray2) 
{
  int i;
  bdd_t *operand1, *operand2;
  array_t *resultArray;
  bdd result;
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  if (array_n(bddArray1) != array_n(bddArray2)){
    fprintf(stderr, "bdd_pairwise_and: Arrays of different lengths.\n");
    return NIL(array_t);
  }
  resultArray = array_alloc(bdd_t *, 0);
  for (i=0; i<array_n(bddArray1); i++){
    operand1 = array_fetch(bdd_t *, bddArray1, i);
    operand2 = array_fetch(bdd_t *, bddArray2, i);
    result = cmu_bdd_and(mgr, operand1->node, operand2->node);
    array_insert_last(bdd_t*, resultArray,
                      bdd_construct_bdd_t(mgr, result)); 
  }
  return resultArray;
}

array_t *
bdd_pairwise_xor(bdd_manager *manager, array_t *bddArray1, array_t
                 *bddArray2) 
{
  int i;
  bdd_t *operand1, *operand2;
  array_t *resultArray;
  bdd result;
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  if (array_n(bddArray1) != array_n(bddArray2)){
    fprintf(stderr, "bdd_pairwise_xor: Arrays of different lengths.\n");
    return NIL(array_t);
  }
  resultArray = array_alloc(bdd_t *, 0);
  for (i=0; i<array_n(bddArray1); i++){
    operand1 = array_fetch(bdd_t *, bddArray1, i);
    operand2 = array_fetch(bdd_t *, bddArray2, i);
    result = cmu_bdd_xor(mgr, operand1->node, operand2->node);
    array_insert_last(bdd_t*, resultArray,
                      bdd_construct_bdd_t(mgr, result)); 
  }
  return resultArray;
}

bdd_t *
bdd_and_smooth(f, g, smoothing_vars)
bdd_t *f;
bdd_t *g;
array_t *smoothing_vars;	/* of bdd_t *'s */
{
  int num_vars, i;
  bdd_t *fn, *result;
  struct bdd_ **assoc;
  struct bdd_manager_ *mgr;
  
  num_vars = array_n(smoothing_vars);
  if (num_vars <= 0) {
	cmu_bdd_fatal("bdd_and_smooth: no smoothing variables");
  }
  
  assoc = ALLOC(struct bdd_ *, num_vars+1);
  
  for (i = 0; i < num_vars; i++) {
	fn = array_fetch(bdd_t *, smoothing_vars, i);
	assoc[i] = fn->node;
  }
  assoc[num_vars] = (struct bdd_ *) 0;
  
  mgr = f->mgr;
  cmu_bdd_temp_assoc(mgr, assoc, 0);
  (void) cmu_bdd_assoc(mgr, -1);  /* set the temp association as the current association */
  
  result = bdd_construct_bdd_t(mgr, cmu_bdd_rel_prod(mgr, f->node, g->node));
  FREE(assoc);
  return result;
}

bdd_t *
bdd_between(f_min, f_max)
bdd_t *f_min;
bdd_t *f_max;
{
  bdd_t *temp, *ret;
  long size1, size2, size3; 
  temp = bdd_or(f_min, f_max, 1, 0);
  ret = bdd_minimize(f_min, temp);
  bdd_free(temp);
  size1 = bdd_size(f_min);
  size2 = bdd_size(f_max);
  size3 = bdd_size(ret);
  if (size3 < size1) {
	if (size3 < size2){
      return ret;
    }
    else {
      bdd_free(ret);
      return bdd_dup(f_max);
    }
  }
  else {
    bdd_free(ret);
    if (size1 < size2){
      return bdd_dup(f_min);
    }
    else {
      return bdd_dup(f_max);
    }
  }
}

bdd_t *
bdd_subset(f, numVars, threshold)
bdd_t *f;
int numVars;
int threshold;
{
  return bdd_construct_bdd_t(f->mgr, f->node);
}

bdd_t *
bdd_superset(f, numVars, threshold)
bdd_t *f;
int numVars;
int threshold;
{
  return bdd_construct_bdd_t(f->mgr, f->node);
}

bdd_t *
bdd_cofactor(f, g)
bdd_t *f;
bdd_t *g;
{
  return bdd_construct_bdd_t(f->mgr, cmu_bdd_cofactor(f->mgr, f->node, g->node));
}

bdd_t *
bdd_compose(f, v, g)
bdd_t *f;
bdd_t *v;
bdd_t *g;
{
  return bdd_construct_bdd_t(f->mgr, cmu_bdd_compose(f->mgr, f->node, v->node, g->node));
}

bdd_t *
bdd_consensus(f, quantifying_vars)
bdd_t *f;
array_t *quantifying_vars;	/* of bdd_t *'s */
{
  int num_vars, i;
  bdd_t *fn, *result;
  struct bdd_ **assoc;
  struct bdd_manager_ *mgr;
  
  num_vars = array_n(quantifying_vars);
  if (num_vars <= 0) {
	cmu_bdd_fatal("bdd_consensus: no quantifying variables");
  }
  
  assoc = ALLOC(struct bdd_ *, num_vars+1);
  
  for (i = 0; i < num_vars; i++) {
	fn = array_fetch(bdd_t *, quantifying_vars, i);
	assoc[i] = fn->node;
  }
  assoc[num_vars] = (struct bdd_ *) 0;
  
  mgr = f->mgr;
  cmu_bdd_temp_assoc(mgr, assoc, 0);
  (void) cmu_bdd_assoc(mgr, -1);  /* set the temp association as the current association */
  
  result = bdd_construct_bdd_t(mgr, cmu_bdd_forall(mgr, f->node));
  FREE(assoc);
  return result;
}


bdd_t *
bdd_cproject(f, quantifying_vars)
bdd_t *f;
array_t *quantifying_vars;	/* of bdd_t* */
{
  int num_vars, i;
  bdd_t *fn, *result;
  struct bdd_ **assoc;
  struct bdd_manager_ *mgr;
  
  if (f == NIL(bdd_t)) fail ("bdd_cproject: invalid BDD");
  
  num_vars = array_n(quantifying_vars);
  if (num_vars <= 0) {
    printf("Warning: bdd_cproject: no projection variables\n");
    result = bdd_dup(f);
  }
  else {
    assoc = ALLOC(struct bdd_ *, num_vars+1);
    for (i = 0; i < num_vars; i++) {
      fn = array_fetch(bdd_t *, quantifying_vars, i);
      assoc[i] = fn->node;
    }
    assoc[num_vars] = (struct bdd_ *) 0;
    mgr = f->mgr;
    cmu_bdd_temp_assoc(mgr, assoc, 0);
    (void) cmu_bdd_assoc(mgr, -1);  /* set the temp association as the current a
                                       ssociation */
    
    result = bdd_construct_bdd_t(mgr, cmu_bdd_project(mgr, f->node));
    FREE(assoc);
  }
  return result;
}


bdd_t *
bdd_else(f)
bdd_t *f;
{
  return bdd_construct_bdd_t(f->mgr, cmu_bdd_else(f->mgr, f->node));
}


bdd_t *
bdd_ite(i, t, e, i_phase, t_phase, e_phase)
bdd_t *i;
bdd_t *t;
bdd_t *e;
boolean i_phase;
boolean t_phase;
boolean e_phase;
{
  struct bdd_ *temp1, *temp2, *temp3;
  bdd_t *result;
  struct bdd_manager_ *mgr;
  
  mgr = i->mgr;
  temp1 = ( (i_phase == TRUE) ? cmu_bdd_identity(mgr, i->node) : cmu_bdd_not(mgr, i->node));
  temp2 = ( (t_phase == TRUE) ? cmu_bdd_identity(mgr, t->node) : cmu_bdd_not(mgr, t->node));
  temp3 = ( (e_phase == TRUE) ? cmu_bdd_identity(mgr, e->node) : cmu_bdd_not(mgr, e->node));
  result = bdd_construct_bdd_t(mgr, cmu_bdd_ite(mgr, temp1, temp2, temp3));
  cmu_bdd_free(mgr, temp1);
  cmu_bdd_free(mgr, temp2);
  cmu_bdd_free(mgr, temp3);
  return result;
}

bdd_t *
bdd_minimize(f, c)
bdd_t *f;
bdd_t *c;
{
  bdd_t *result = bdd_construct_bdd_t(f->mgr, cmu_bdd_reduce(f->mgr,
                                                             f->node,
                                                             c->node)); 
  if (bdd_size(result) < bdd_size(f)){
    return result;
  }
  else{
    bdd_free(result);
    return bdd_dup(f);
  }
}


bdd_t *
bdd_not(f)
bdd_t *f;
{
  return bdd_construct_bdd_t(f->mgr, cmu_bdd_not(f->mgr, f->node));
}

bdd_t *
bdd_one(bdd_manager *manager)
{
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  return bdd_construct_bdd_t(mgr, cmu_bdd_one(mgr));
}

bdd_t *
bdd_or(f, g, f_phase, g_phase)
bdd_t *f;
bdd_t *g;
boolean f_phase;
boolean g_phase;
{
  struct bdd_ *temp1, *temp2;
  bdd_t *result;
  struct bdd_manager_ *mgr;
  
  mgr = f->mgr;
  temp1 = ( (f_phase == TRUE) ? cmu_bdd_identity(mgr, f->node) : cmu_bdd_not(mgr, f->node));
  temp2 = ( (g_phase == TRUE) ? cmu_bdd_identity(mgr, g->node) : cmu_bdd_not(mgr, g->node));
  result = bdd_construct_bdd_t(mgr, cmu_bdd_or(mgr, temp1, temp2));
  cmu_bdd_free(mgr, temp1);
  cmu_bdd_free(mgr, temp2);
  return result;
}

bdd_t *
bdd_smooth(f, smoothing_vars)
bdd_t *f;
array_t *smoothing_vars;	/* of bdd_t *'s */
{
  int num_vars, i;
  bdd_t *fn, *result;
  struct bdd_ **assoc;
  struct bdd_manager_ *mgr;
  
  num_vars = array_n(smoothing_vars);
  if (num_vars <= 0) {
	cmu_bdd_fatal("bdd_smooth: no smoothing variables");
  }
  
  assoc = ALLOC(struct bdd_ *, num_vars+1);
  
  for (i = 0; i < num_vars; i++) {
	fn = array_fetch(bdd_t *, smoothing_vars, i);
	assoc[i] = fn->node;
  }
  assoc[num_vars] = (struct bdd_ *) 0;
  
  mgr = f->mgr;
  cmu_bdd_temp_assoc(mgr, assoc, 0);
  (void) cmu_bdd_assoc(mgr, -1);  /* set the temp association as the current association */
  
  result = bdd_construct_bdd_t(mgr, cmu_bdd_exists(mgr, f->node));
  FREE(assoc);
  return result;
}

bdd_t *
bdd_substitute(f, old_array, new_array)
bdd_t *f;
array_t *old_array;	/* of bdd_t *'s */
array_t *new_array;	/* of bdd_t *'s */
{
    int num_old_vars, num_new_vars, i;
    bdd_t *fn_old, *fn_new, *result;
    struct bdd_ **assoc;
    struct bdd_manager_ *mgr;

    num_old_vars = array_n(old_array);
    num_new_vars = array_n(new_array);
    if (num_old_vars != num_new_vars) {
	cmu_bdd_fatal("bdd_substitute: mismatch of number of new and old variables");
    }

    assoc = ALLOC(struct bdd_ *, 2*(num_old_vars+1));

    for (i = 0; i < num_old_vars; i++) {
	fn_old = array_fetch(bdd_t *, old_array, i);
	fn_new = array_fetch(bdd_t *, new_array, i);
	assoc[2*i]   = fn_old->node;
	assoc[2*i+1] = fn_new->node;
    }
    assoc[2*num_old_vars]   = (struct bdd_ *) 0;
    assoc[2*num_old_vars+1] = (struct bdd_ *) 0;  /* not sure if we need this second 0 */

    mgr = f->mgr;
    cmu_bdd_temp_assoc(mgr, assoc, 1);
    (void) cmu_bdd_assoc(mgr, -1);  /* set the temp association as the current association */

    result = bdd_construct_bdd_t(mgr, cmu_bdd_substitute(mgr,
                                                         f->node)); FREE(assoc); return result; }

bdd_t *
bdd_then(f)
bdd_t *f;
{
    return bdd_construct_bdd_t(f->mgr, cmu_bdd_then(f->mgr, f->node));
}

bdd_t *
bdd_top_var(f)
bdd_t *f;
{
    return bdd_construct_bdd_t(f->mgr, cmu_bdd_if(f->mgr, f->node));
}

bdd_t *
bdd_xnor(f, g)
bdd_t *f;
bdd_t *g;
{
    return bdd_construct_bdd_t(f->mgr, cmu_bdd_xnor(f->mgr, f->node, g->node));
}

bdd_t *
bdd_xor(f, g)
bdd_t *f;
bdd_t *g;
{
    return bdd_construct_bdd_t(f->mgr, cmu_bdd_xor(f->mgr, f->node, g->node));
}

bdd_t *
bdd_zero(bdd_manager *manager)
{
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  return bdd_construct_bdd_t(mgr, cmu_bdd_zero(mgr));
}

/*
Queries about BDD Formulas ----------------------------------------------------
*/

boolean
bdd_equal(f, g)
bdd_t *f;
bdd_t *g;
{
    return (f->node == g->node);
}

bdd_t *
bdd_intersects(f, g)
bdd_t *f;
bdd_t *g;
{
    return bdd_construct_bdd_t(f->mgr, cmu_bdd_intersects(f->mgr, f->node, g->node));
}

boolean
bdd_is_tautology(f, phase)
bdd_t *f;
boolean phase;
{
    return ((phase == TRUE) ? (f->node == cmu_bdd_one(f->mgr)) : (f->node == cmu_bdd_zero(f->mgr)));
}

boolean
bdd_leq(f, g, f_phase, g_phase)
bdd_t *f;
bdd_t *g;
boolean f_phase;
boolean g_phase;
{
    struct bdd_ *temp1, *temp2, *implies_fn;
    struct bdd_manager_ *mgr;
    boolean result_value;

    mgr = f->mgr;
    temp1 = ( (f_phase == TRUE) ? cmu_bdd_identity(mgr, f->node) : cmu_bdd_not(mgr, f->node));
    temp2 = ( (g_phase == TRUE) ? cmu_bdd_identity(mgr, g->node) : cmu_bdd_not(mgr, g->node));
    implies_fn = cmu_bdd_implies(mgr, temp1, temp2); /* returns a minterm of temp1*!temp2 */
    result_value = (implies_fn == cmu_bdd_zero(mgr));
    cmu_bdd_free(mgr, temp1);
    cmu_bdd_free(mgr, temp2);
    cmu_bdd_free(mgr, implies_fn);
    return result_value;
}

/*
Statistics and Other Queries --------------------------------------------------
*/

double 
bdd_count_onset(f, var_array)
bdd_t *f;
array_t *var_array;  	/* of bdd_t *'s */
{
    int num_vars;
    double fraction;

    num_vars = array_n(var_array);
    fraction = cmu_bdd_satisfying_fraction(f->mgr, f->node); /* cannot give support vars */
    return (fraction * pow((double) 2, (double) num_vars));
}

int
bdd_get_free(f)
bdd_t *f;
{
    return (f->free);
}

bdd_manager *
bdd_get_manager(f)
bdd_t *f;
{
    return (bdd_manager *) (f->mgr);
}

bdd_node *
bdd_get_node(f, is_complemented)
bdd_t *f;
boolean *is_complemented;    /* return */
{
    *is_complemented = (boolean) TAG0(f->node);  /* using bddint.h */
    return ((bdd_node *) BDD_POINTER(f->node));  /* using bddint.h */
}

var_set_t *
bdd_get_support(f)
bdd_t *f;
{
    struct bdd_ **support, *var;
    struct bdd_manager_ *mgr;
    long num_vars;
    var_set_t *result;
    int id, i;

    mgr = f->mgr;
    num_vars = cmu_bdd_vars(mgr);

    result = var_set_new((int) num_vars);
    support = (struct bdd_ **) mem_get_block((num_vars+1) * sizeof(struct bdd_ *));
    (void) cmu_bdd_support(mgr, f->node, support);

    for (i = 0; i < num_vars; ++i) {  /* can never have more than num_var non-zero entries in array */
	var = support[i]; 
	if (var == (struct bdd_ *) 0) {
	    break;  /* have reach end of null-terminated array */
	}
	id = (int) (cmu_bdd_if_id(mgr, var) - 1);  /* a variable is never garbage collected, so no need to free */
	var_set_set_elt(result, id);
    }

    mem_free_block((pointer)support);

    return result;
}

array_t *
bdd_get_varids(var_array)
array_t *var_array;
{
  int i;
  bdd_t *var;
  array_t *result;
 
  result = array_alloc(bdd_variableId, 0);
  for (i = 0; i < array_n(var_array); i++) {
    var = array_fetch(bdd_t *, var_array, i);
    array_insert_last(bdd_variableId, result, bdd_top_var_id(var));
  }
  return result;
}

unsigned int 
bdd_num_vars(bdd_manager *manager)
{
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  return (cmu_bdd_vars(mgr));
}

void
bdd_print(f)
bdd_t *f;
{
    cmu_bdd_print_bdd(f->mgr, f->node, bdd_naming_fn_none, bdd_terminal_id_fn_none, (pointer) 0, stdout);
}

void
bdd_print_stats(bdd_manager *manager, FILE *file)
{
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  cmu_bdd_stats(mgr, file);
}

/**Function********************************************************************

  Synopsis [Sets the internal parameters of the package to the given values.]

  SideEffects []

******************************************************************************/
int
bdd_set_parameters(
  bdd_manager *mgr,
  avl_tree *valueTable,
  FILE *file)
{
  (void) fprintf(file, "Functionality not supported yet in the CMU package\n");
  return 1;
} /* End of bdd_set_parameters */

int
bdd_size(f)
bdd_t *f;
{
    return ((int) cmu_bdd_size(f->mgr, f->node, 1));
}

int
bdd_node_size(f)
bdd_node *f;
{
  return(0);
}

long
bdd_size_multiple(bdd_array)
array_t *bdd_array;
{
    long result;
    struct bdd_ **vector_bdd;
    bdd_t *f;
    int i;
    struct bdd_manager_ *mgr;

    if ((bdd_array == NIL(array_t)) || (array_n(bdd_array) == 0))
        return 0;

    f = array_fetch(bdd_t*, bdd_array, 0);
    mgr = f->mgr;

    vector_bdd = (struct bdd_ **)
                        malloc((array_n(bdd_array)+1)*sizeof(struct bdd_ *));

    for(i=0; i<array_n(bdd_array);i++){
        f = array_fetch(bdd_t*, bdd_array, i);
        vector_bdd[i] = f->node;
    }
    vector_bdd[array_n(bdd_array)] = 0;
    result =  cmu_bdd_size_multiple(mgr, vector_bdd,1);
    FREE(vector_bdd);
    return result;
}

bdd_variableId
bdd_top_var_id(f)
bdd_t *f;
{
    return ((bdd_variableId) (cmu_bdd_if_id(f->mgr, f->node) - 1));
}

/*
Miscellaneous -----------------------------------------------------------------
*/

bdd_external_hooks *
bdd_get_external_hooks(bdd_manager *manager)
{
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  return ((bdd_external_hooks *) mgr->hooks);
}


void
bdd_set_gc_mode(bdd_manager *manager, boolean no_gc)
{
  cmu_bdd_warning("bdd_set_gc_mode: translated to no-op in CMU package");
}

void 
bdd_dynamic_reordering(bdd_manager *manager, bdd_reorder_type_t
                       algorithm_type, bdd_reorder_verbosity_t verbosity) 
{
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
    switch(algorithm_type) {
    case BDD_REORDER_SIFT:
	cmu_bdd_dynamic_reordering(mgr, cmu_bdd_reorder_sift);
	break;
    case BDD_REORDER_WINDOW:
	cmu_bdd_dynamic_reordering(mgr, cmu_bdd_reorder_stable_window3);
	break;
    case BDD_REORDER_NONE:
	cmu_bdd_dynamic_reordering(mgr, cmu_bdd_reorder_none);
	break;
    default:
      fprintf(stderr,"CMU: bdd_dynamic_reordering: unknown algorithm type\n");
      fprintf(stderr,"Using SIFT method instead\n");
      cmu_bdd_dynamic_reordering(mgr, cmu_bdd_reorder_sift);
    }
}

void 
bdd_dynamic_reordering_zdd(bdd_manager *manager, bdd_reorder_type_t
                       algorithm_type, bdd_reorder_verbosity_t verbosity) 
{
    return;
}

void 
bdd_reorder(bdd_manager *manager)
{
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  cmu_bdd_reorder(mgr);
}

bdd_variableId
bdd_get_id_from_level(bdd_manager *manager, long level)
{
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  struct bdd_ *fn;

  fn = cmu_bdd_var_with_index(mgr, level);
  
  if (fn == (struct bdd_ *) 0) {
    /* variable should always be found, since they are created at bdd_start */
    cmu_bdd_fatal("bdd_get_id_from_level: assumption violated");
  }
  
  return ((bdd_variableId)(cmu_bdd_if_id(mgr, fn) - 1 ));
  
}

long
bdd_top_var_level(bdd_manager *manager, bdd_t *fn)
{
  cmu_bdd_manager mgr = (cmu_bdd_manager) manager;
  return cmu_bdd_if_index(mgr, fn->node);
}

/*
 * Return TRUE if f is a cube, else return FALSE.
 */
boolean
bdd_is_cube(f)
bdd_t *f;
{
  struct bdd_manager_ *manager;

  if (f == NIL(bdd_t)) {
        fail("bdd_is_cube: invalid BDD");
  }
  if(  f->free ) fail ("Freed Bdd passed to bdd_is_cube");
  manager = f->mgr;
  return ((boolean)cmu_bdd_is_cube(manager, f->node));
}

bdd_block *
bdd_new_var_block(bdd_t *f, long length)
{
  struct bdd_manager_ *manager;
  if (f == NIL(bdd_t)) {
        fail("bdd_new_var_block: invalid BDD");
  }
  manager = f->mgr;
  return (bdd_block *)cmu_bdd_new_var_block(manager, f->node, length);
}

bdd_t *
bdd_var_with_index(bdd_manager *manager, int index)
{
  return bdd_construct_bdd_t(manager, 
		            cmu_bdd_var_with_index((cmu_bdd_manager) manager,
                                                   index));
}

bdd_t *
bdd_compact(bdd_t *f, bdd_t *g)
{
    return (NULL);
}


bdd_t *
bdd_squeeze(bdd_t *f, bdd_t *g)
{
    return (NULL);
}

double
bdd_correlation(bdd_t *f, bdd_t *g)
{
    return (0.0);
}

/**Function********************************************************************

  Synopsis    [Dummy functions defined in bdd.h]

  SideEffects []

******************************************************************************/
int
bdd_reordering_status(mgr, method)
bdd_manager *mgr;
bdd_reorder_type_t *method;
{
  return 0;
}

/**Function********************************************************************

  Synopsis    [Dummy functions defined in bdd.h]

  SideEffects []

******************************************************************************/

bdd_t *
bdd_compute_cube(mgr, vars)
bdd_manager *mgr;
array_t *vars;
{
  return NIL(bdd_t);
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
bdd_iter_decomp(f,  partType, conjArray)
bdd_t *f;
bdd_partition_type_t partType;
bdd_t  ***conjArray;
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
bdd_read_node_count(mgr)
bdd_manager *mgr;
{
  return 0;
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


array_t *
bdd_bdd_pick_arbitrary_minterms(f, varsArray, n, k)
bdd_t *f;
array_t *varsArray;
int n;
int k;
{
    return NIL(array_t);
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
  return NIL(bdd_node);
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
    return (-1);

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



