#include "mdd.h"

/*
 * MDD Package
 * $Source: /projects/bubble/cvs/code/glu-chai/src/mdd/mdd_func1c.c,v $
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
mdd_func1c(mgr, mvar1, constant, func2)
mdd_manager *mgr; 
int mvar1; 
int constant;
boolean (*func2)();
{
    mvar_type x;
    array_t *child_list_x;
    int i;
    mdd_t *tx;
    array_t *mvar_list = mdd_ret_mvar_list(mgr);
    mdd_t *zero, *one;

    zero = mdd_zero(mgr);
    one = mdd_one(mgr);
    x = array_fetch(mvar_type, mvar_list, mvar1);
    if (x.status == MDD_BUNDLED) 
	printf("\nWarning: mdd_func1c, bundled variable %s is used\n", x.name);

    child_list_x = array_alloc(mdd_t *, 0);
    for (i=0; i<x.values; i++) {
	if (func2(i,constant))
	    array_insert_last(mdd_t *, child_list_x, one);
        else
            array_insert_last(mdd_t *, child_list_x, zero);
    }
    tx = mdd_case(mgr, mvar1, child_list_x);
    array_free(child_list_x);

    mdd_free(one);
    mdd_free(zero);

    return tx;
}

