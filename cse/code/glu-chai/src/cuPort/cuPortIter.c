/**CFile***********************************************************************

  FileName    [cuPortIter.c]

  PackageName [cu_port]

  Synopsis    [Port routines for CU package.]

  Description [optional]

  SeeAlso     [optional]

  Author      [Abelardo Pardo <abel@vlsi.colorado.edu> ]

  Copyright [This file was created at the University of Colorado at Boulder.
  The University of Colorado at Boulder makes no warranty about the suitability
  of this software for any purpose.  It is presented on an AS IS basis.]

  Revision    [$Id: cuPortIter.c,v 1.1.1.1 2003/04/24 23:35:11 ashwini Exp $]

******************************************************************************/

#include "cuPortInt.h"

#ifndef lint
static char rcsid[] UNUSED = "$Id";
#endif

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
typedef struct {
    DdManager   *manager;
    DdGen	*ddGen;
    array_t	*cube;
} cu_bdd_gen;

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

bdd_gen_status
bdd_gen_read_status(bdd_gen *gen)
{
  if (Cudd_IsGenEmpty(((cu_bdd_gen *)gen)->ddGen)) {
    return bdd_EMPTY;
  }
  else {
    return bdd_NONEMPTY;
  }
}

/**Function********************************************************************

  Synopsis    [Returns the first cube of the function.
  A generator is also returned, which will iterate over the rest.]

  Description [Defines an iterator on the onset of a BDD.  Two routines
  are provided: bdd_first_cube, which extracts one cube from a BDD and
  returns a bdd_gen structure containing the information necessary to
  continue the enumeration; and bdd_next_cube, which returns 1 if
  another cube was found, and 0 otherwise. A cube is represented as an
  array of bdd_literal (which are integers in {0, 1, 2}), where 0
  represents negated literal, 1 for literal, and 2 for don't care.
  Returns a disjoint cover.  A third routine is there to clean up.]

  SideEffects []

  SeeAlso [bdd_next_cube bdd_gen_free]

******************************************************************************/
bdd_gen *
bdd_first_cube(fn, cube)
bdd_t *fn;
array_t **cube;	/* of bdd_literal */
{
    DdManager *manager;
    cu_bdd_gen *gen;
    int i;
    int *icube;
    CUDD_VALUE_TYPE value;

    /* Make sure we receive a valid bdd_t. (So to speak.) */
    assert(fn != 0);

    manager = (DdManager *)fn->mgr;

    /* Initialize the generator. */
    gen = ALLOC(cu_bdd_gen,1);
    if (gen == NULL) {
      return(NIL(bdd_gen));
    }
    gen->manager = manager;

    gen->cube = array_alloc(bdd_literal, manager->size);
    if (gen->cube == NULL) {
	fail("Bdd Package: Out of memory in bdd_first_cube");
    }

    gen->ddGen = Cudd_FirstCube(manager,(DdNode *)fn->node,&icube,&value);
    if (gen->ddGen == NULL) {
	fail("Cudd Package: Out of memory in bdd_first_cube");
    }

    if (!Cudd_IsGenEmpty(gen->ddGen)) {
	/* Copy icube to the array_t cube. */
	for (i = 0; i < manager->size; i++) {
	    int myconst = icube[i];
	    array_insert(bdd_literal, gen->cube, i, myconst);
	}
	*cube = gen->cube;
    }

    return(gen);

} /* end of bdd_first_cube */


/**Function********************************************************************

  Synopsis    [Gets the next cube on the generator. Returns {TRUE,
  FALSE} when {more, no more}.]

  SideEffects []

  SeeAlso     [bdd_first_cube bdd_gen_free]

******************************************************************************/
boolean
bdd_next_cube(gen_, cube)
bdd_gen *gen_;
array_t **cube;		/* of bdd_literal */
{
    cu_bdd_gen *gen;
    int retval;
    int *icube;
    CUDD_VALUE_TYPE value;
    int i;

    gen = (cu_bdd_gen *)gen_;

    retval = Cudd_NextCube(gen->ddGen,&icube,&value);
    if (!Cudd_IsGenEmpty(gen->ddGen)) {
	/* Copy icube to the array_t cube. */
	for (i = 0; i < gen->manager->size; i++) {
	    int myconst = icube[i];
	    array_insert(bdd_literal, gen->cube, i, myconst);
	}
	*cube = gen->cube;
    }

    return(retval);

} /* end of bdd_next_cube */


/**Function********************************************************************

  Synopsis    [Gets the first node in the BDD and returns a generator.]

  SideEffects []

  SeeAlso [bdd_next_node]

******************************************************************************/
bdd_gen *
bdd_first_node(fn, node)
bdd_t *fn;
bdd_node **node;	/* return */
{
    bdd_manager *manager;
    cu_bdd_gen *gen;

    /* Make sure we receive a valid bdd_t. (So to speak.) */
    assert(fn != 0);

    manager = fn->mgr;

    /* Initialize the generator. */
    gen = ALLOC(cu_bdd_gen,1);
    if (gen == NULL) return(NULL);
    gen->manager = manager;
    gen->cube = NULL;

    gen->ddGen = Cudd_FirstNode(manager,(DdNode *)fn->node, (DdNode **)node);
    if (gen->ddGen == NULL) {
	fail("Cudd Package: Out of memory in bdd_first_node");
    }

    return(gen);

} /* end of bdd_first_node */


/**Function********************************************************************

  Synopsis    [Gets the next node in the BDD. Returns {TRUE, FALSE} when
  {more, no more}.]

  SideEffects []

  SeeAlso [bdd_first_node]

******************************************************************************/
boolean
bdd_next_node(gen, node)
bdd_gen *gen;
bdd_node **node;	/* return */
{
    return(Cudd_NextNode(((cu_bdd_gen *)gen)->ddGen,(DdNode **)node));
} /* end of bdd_next_node */


/**Function********************************************************************

  Synopsis    [Frees up the space used by the generator. Returns an int
  so that it is easier to fit in a foreach macro. Returns 0 (to make it
  easy to put in expressions).]

  SideEffects []

  SeeAlso []

******************************************************************************/
int
bdd_gen_free(gen_)
bdd_gen *gen_;
{
    cu_bdd_gen *gen;

    gen = (cu_bdd_gen *)gen_;
    if (gen->cube != NULL) array_free(gen->cube);
    Cudd_GenFree(gen->ddGen);
    FREE(gen);
    return(0);

} /* end of bdd_gen_free */

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
