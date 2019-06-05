#include "mdd.h"

/*
 * MDD Package
 * $Source: /projects/bubble/cvs/code/glu-chai/src/mdd/mdd_substit.c,v $
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
mdd_substitute(mgr, fn, old_mvars, new_mvars)
mdd_manager	*mgr;
mdd_t		*fn;
array_t		*old_mvars, *new_mvars;
{
    array_t 		*old_bdd_vars, *new_bdd_vars;
    int 		i, j, old_mv_no, new_mv_no, no_mvar;
    mvar_type 		old_mv, new_mv;
    mdd_t 		*top;
    array_t 		*mvar_list = mdd_ret_mvar_list(mgr);
    bdd_t		*varBdd;

    old_bdd_vars = array_alloc( bdd_t *, 0);
    new_bdd_vars = array_alloc( bdd_t *, 0);

    no_mvar = array_n(old_mvars);
    if (no_mvar != array_n(new_mvars)) 
        fail("mdd_substitute: arrays contains different no. of mvars.\n");

    for (i=0; i<no_mvar; i++) {
        old_mv_no = array_fetch(int, old_mvars, i);
	old_mv = array_fetch(mvar_type, mvar_list, old_mv_no);
	if (old_mv.status == MDD_BUNDLED) {
		(void) fprintf(stderr, 
		   "\nmdd_substitute: bundled variable %s used\n",old_mv.name);
		fail("");
	}

        new_mv_no = array_fetch(int, new_mvars, i);
	new_mv = array_fetch(mvar_type, mvar_list, new_mv_no);
	if (new_mv.status == MDD_BUNDLED) {
		(void) fprintf(stderr, 
		   "\nmdd_substitute: bundled variable %s used\n",new_mv.name);
		fail("");
	}

	if (old_mv.values != new_mv.values) 
            fail("mdd_substitute: mvars have different no. of values\n");

	for (j=0; j<old_mv.encode_length; j++) {	
	    varBdd = bdd_get_variable( mgr, mdd_ret_bvar_id(&old_mv,j) );
	    array_insert_last( bdd_t *, old_bdd_vars, varBdd );

	    varBdd = bdd_get_variable( mgr, mdd_ret_bvar_id(&new_mv, j) );
	    array_insert_last( bdd_t *, new_bdd_vars, varBdd);
	}
    }
    top = bdd_substitute(fn, old_bdd_vars, new_bdd_vars);

    for(j=0; j<array_n(old_bdd_vars); j++) {
        varBdd = array_fetch(bdd_t*,old_bdd_vars,j);
        bdd_free(varBdd);
    }
    array_free(old_bdd_vars);
    for(j=0; j<array_n(new_bdd_vars); j++) {
        varBdd = array_fetch(bdd_t*,new_bdd_vars,j);
        bdd_free(varBdd);
    }
    array_free(new_bdd_vars);

    return top;
}
