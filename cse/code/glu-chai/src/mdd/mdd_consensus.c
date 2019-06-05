#include "mdd.h"

/*
 * MDD Package
 * $Source: /projects/bubble/cvs/code/glu-chai/src/mdd/mdd_consensus.c,v $
 * 
 * Author: Timothy Kam
 * $Date: 2003/04/24 23:35:11 $
 * $Revision: 1.1.1.1 $
 *
 * Copyright 1992 by the Regents of the University of California.
 *
 * All rights reserved.  Permission to use, copy, modify and distribute
 * this software is hereby granted, provided that the above copyright
 * notice and this permission notice appear in all copies.  This software
 * is made available as is, with no warranties.
 */

mdd_t *
mdd_consensus(mgr, fn, mvars)
mdd_manager *mgr;
mdd_t *fn;
array_t *mvars;
{
    array_t *bdd_vars;
    int i, j, mv_no, num;
    mvar_type mv;
    mdd_t *top;
	bdd_t *tmp;
    array_t *mvar_list = mdd_ret_mvar_list(mgr);

    bdd_vars = array_alloc(bdd_t *, 0);


    if ( mvars == NIL( array_t ) ) {
        printf("\nWARNING: Empty Array of Consensus Variables\n");
        array_free(bdd_vars);
        return ( mdd_dup(fn) ) ;
    }

    else if ( array_n(mvars) == 0)  {
        printf("\nWARNING: Empty Array of Consensus Variables\n");
        array_free(bdd_vars);
        return ( mdd_dup(fn) ) ;
    }


    for (i=0; i<array_n(mvars); i++) {
        mv_no = array_fetch(int, mvars, i);
	mv = array_fetch(mvar_type, mvar_list, mv_no);
	if (mv.status == MDD_BUNDLED) {
		(void) fprintf(stderr, 
		"\nmdd_consensus: bundled variable %s used\n",mv.name);
		fail("");
	}

        for (j = 0; j < mv.encode_length; j ++) {
	    tmp = bdd_get_variable(mgr, (unsigned int) mdd_ret_bvar_id(&mv, j) );
	    array_insert_last(bdd_t *, bdd_vars, tmp);
	}
    }
    top = bdd_consensus(fn, bdd_vars);
	num = array_n(bdd_vars);
	for(i=0; i<num; i++){
		bdd_free(array_fetch(bdd_t *, bdd_vars, i));
	}
    array_free(bdd_vars);
    return top;
}

