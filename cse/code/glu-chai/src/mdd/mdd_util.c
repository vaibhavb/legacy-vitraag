#include <stdio.h>
#include <math.h>
#include "util.h"
#include "mdd.h"

/*
 * MDD Package
 * $Source: /projects/bubble/cvs/code/glu-chai/src/mdd/mdd_util.c,v $
 *
 * Author: Timothy Kam
 * $Date: 2003/08/11 18:34:38 $
 * $Revision: 1.11 $
 * $Id: mdd_util.c,v 1.11 2003/08/11 18:34:38 ashwini Exp $
 * $Log: mdd_util.c,v $
 * Revision 1.11  2003/08/11 18:34:38  ashwini
 * some comment changes.
 *
 * Revision 1.10  2003/06/25 22:18:56  ashwini
 * some code cleanup for mdd_simulate
 *
 * Revision 1.9  2003/06/25 21:50:13  ashwini
 * also conside rthe initial condition mdd in mdd_simulate.
 *
 * Revision 1.8  2003/06/24 00:57:41  ashwini
 * fixed bug regarding the change of primed bdd vars to unprimed ones
 * for the next iteration.
 *
 * Revision 1.7  2003/06/11 19:50:56  ashwini
 * some debug changes
 *
 * Revision 1.6  2003/06/07 01:56:55  ashwini
 * code clean up for mdd_simulate function.
 *
 * Revision 1.5  2003/06/07 01:08:22  ashwini
 * mdd_simulate works!!
 *
 * Revision 1.4  2003/06/05 21:14:46  ashwini
 * first cut of the mdd_simulate file. Implementation done, but buggy, as both tau0 and tau1 mdds
 * sometimes evaluate to 0.
 *
 * Revision 1.3  2003/04/29 02:33:29  luca
 * gone back to the version which limits var range
 *
 * Revision 1.2  2003/04/25 00:43:14  ashwini
 * added function mdd_pick_one_minterm() to pick a random minterm from a given mdd.
 *
 * Revision 1.1.1.1  2003/04/24 23:35:11  ashwini
 * import the glu-1.3 to glu-chai
 *
 * Revision 1.12  1998/09/16 00:21:29  mooni
 * Added mdd_get_var_by_id.
 *
 * Revision 1.11  1998/09/15 21:08:16  ravi
 * Merged changes.
 *
 * Revision 1.1.1.7  1998/05/04 01:44:13  hsv
 * Berkeley 1.3 Alpha release
 *
 * Revision 1.2  1997/02/25 00:27:07  boldvis
 * Alignment between Boulder and Berkeley source code before the release
 *
 * Revision 1.5  1997/02/12 21:26:52  hsv
 * Fixed import conflicts.
 *
 * Revision 1.4  1996/11/04 22:59:45  hsv
 * Fixed spurious conflicts.
 *
 * Revision 1.3  1996/03/06 22:11:19  hsv
 * fixed the conflict
 *
 * Revision 1.2  1995/12/28 00:57:23  hsv
 * New version inside glu1.0
 * Revision 1.1.1.5  1996/11/04 22:52:29  hsv
 * Glu Version 1.1
 *
 * Revision 1.1  1996/01/18 19:00:37  shaz
 * Initial revision
 *
 * Revision 1.1  1996/01/18 18:21:27  shaz
 * Initial revision
 *
 * Revision 1.1  1996/01/18 02:31:47  shaz
 * Initial revision
 *
 * Revision 1.16  1995/08/08 22:40:49  rajeev
 * Changes made by shazqadeer.420 as of 8/8/95
 *
 * Revision 1.14  1994/07/08  23:08:42  gms
 * Bug fixed: Was too enthusiastic about freeing arrays
 *
 *
 * Copyright 1992 by the Regents of the University of California.
 *
 * All rights reserved.  Permission to use, copy, modify and distribute
 * this software is hereby granted, provided that the above copyright
 * notice and this permission notice appear in all copies.  This software
 * is made available as is, with no warranties.
 */

/****************************** FROM RAMIN ******************************/
	double		mdd_count_onset( );
	array_t*	mddRetBinVars( );
	array_t*	mdd_ret_arr_bin_vars( ); /* was mddRetArrBinVars */

static	bdd_t*		mddRetOnvalBdd( );
static	bdd_t*		mddIntRetOnvalBdd( );
	void		mddFreeBddArr( );
        mdd_t*          mdd_cproject( );
/***************************send bugs to gms***********************************/
extern bdd_t* bdd_cproject();
extern bdd_t* mdd_cproject();

/************************************************************************/
extern	double		bdd_count_onset( );
extern	mdd_t*		mdd_forall( );
	
/************************************************************************/
#define		mddGetVarById( mgr, id )	\
    array_fetch(mvar_type, mdd_ret_mvar_list((mgr)),(id))


int
toggle(x)
int x;
{
    if (x == 0) return 1;
    else {
	if (x == 1) return 0;
	else {
	    fail("toggle: invalid boolean value\n");
	    return -1;
	}
    }
}

int
no_bit_encode(n)
int n;
{
    int i = 0;
    int j = 1;

    if (n < 2) return 1; /* Takes care of mv.values <= 1 */

    while (j < n) {
	j = j * 2;
	i++;
    }
    return i;
}

void
print_mvar_list(mgr)
mdd_manager *mgr;
{
    mvar_type mv;
    int i;
    int no_mvar;
    array_t *mvar_list = mdd_ret_mvar_list(mgr);

    no_mvar = array_n(mvar_list);
    printf("print_mvar_list:\n");
    printf("id\tname\tvalues\tbits\tstride\tstart_vertex\n");
    /*ashwini:changed printf to fprintf*/
    /*fprintf(stderr, "print_mvar_list:\n");
    fprintf(stderr, "id\tname\tvalues\tbits\tstride\tstart_vertex\n");*/
    for (i=0; i<no_mvar; i++) {
        mv = array_fetch(mvar_type, mvar_list, i);
        (void) printf("%d\t%s\t%d\t%d\n", 
        /*(void) fprintf(stderr, "%d\t%s\t%d\t%d\n", */
		mv.mvar_id, mv.name, mv.values, 
		mv.encode_length);
    }
}

void
print_strides(mvar_strides)
array_t *mvar_strides;
{
    int i, s;

    (void) printf("mvar_strides: ");
    for (i=0; i<array_n(mvar_strides); i++) {
        s = array_fetch(int, mvar_strides, i);
        (void) printf("%d ", s);
    }
    (void) printf("\n");
}

void
print_bdd_list_id(bdd_list)
array_t *bdd_list;
{
    bdd_t *b;
    int i, is_complemented;

    (void) printf("bdd_list id's: ");
    /*fprintf(stderr, "bdd_list id's: ");*/
    for (i=0; i<array_n(bdd_list); i++) {
        b = array_fetch(bdd_t *, bdd_list, i);
        (void)bdd_get_node(b, &is_complemented);
        if (is_complemented) (void) printf("!");
        (void) printf("%d ", bdd_top_var_id(b));
        /*if (is_complemented) (void) fprintf(stderr, "!");
        (void) fprintf(stderr, "%d ", bdd_top_var_id(b));*/
    }
    (void) printf("\n");
    /*(void) fprintf(stderr, "\n");*/
}

void
print_bvar_list_id(mgr)
mdd_manager *mgr;
{
    bvar_type bv;
    int i, is_complemented;
    array_t *bvar_list = mdd_ret_bvar_list(mgr);
 
    (void) printf("bvar_list id's: ");
    for (i=0; i<array_n(bvar_list); i++) {
        bv = array_fetch(bvar_type, bvar_list, i);
        (void)bdd_get_node(bv.node,&is_complemented);
        if (is_complemented) (void) printf("!");
	(void) printf("%d ", bdd_top_var_id(bv.node));
    }
    (void) printf("\n");
}

void
print_bdd(mgr, top)
bdd_manager *mgr;
bdd_t *top;  
{

    int is_complemented;
    bdd_t *child, *top_uncomp;

    if (bdd_is_tautology(top,1)) {
	(void) printf("ONE ");
        return;
    }
    if (bdd_is_tautology(top,0)) {
	(void) printf("ZERO ");
        return;
    }
    (void)bdd_get_node(top, &is_complemented);
    if (is_complemented != 0) (void) printf("!");
    (void) printf("%d ", bdd_top_var_id(top));
    (void) printf("v ");
    (void) printf("< ");

    if (is_complemented) top_uncomp = bdd_not(top);
    else top_uncomp = mdd_dup(top);

    child = bdd_then(top);

    print_bdd(mgr, child);
    (void) printf("> ");

    mdd_free(child);
    child = bdd_else(top);

    print_bdd(mgr, child);
    (void) printf("^ ");
    
    mdd_free(top_uncomp);
    mdd_free(child);

    return;
}

mvar_type 
find_mvar_id(mgr, id)
mdd_manager *mgr;
unsigned short id;
{
    mvar_type mv;
    bvar_type bv;
    array_t *mvar_list = mdd_ret_mvar_list(mgr);
    array_t *bvar_list = mdd_ret_bvar_list(mgr);

    if (id >= array_n(bvar_list))
    	fail("find_mvar_id: invalid parameter range for id\n");
    bv = array_fetch(bvar_type, bvar_list, id);
    if ((bv.mvar_id < 0) || (bv.mvar_id >= array_n(mvar_list)))
    	fail("find_mvar_id: bvar contains invalid mvar_id\n");
    mv = array_fetch(mvar_type, mvar_list, bv.mvar_id);
    return mv;
}

void
clear_all_marks(mgr)
mdd_manager *mgr;
{
    int i, j;
    mvar_type mv;
    array_t *mvar_list = mdd_ret_mvar_list(mgr);


    for (i=0; i<array_n(mvar_list); i++) {
	mv = array_fetch(mvar_type, mvar_list, i);
	for (j=0; j<mv.encode_length; j++)
	    mv.encoding[j] = 2;
    }
}

void
mdd_mark(mgr, top, phase)
mdd_manager *mgr;
bdd_t *top; /**** was bdd_node *bnode; --- changed by Serdar ***/
int phase;
{
    int i, top_id, found = 0;
    int bit_position = 0; /* initialize for lint */
    mvar_type mv;

    top_id = bdd_top_var_id(top);
    mv = find_mvar_id(mgr, top_id);

    for (i=0; i<(mv.encode_length); i++){
		if ( mdd_ret_bvar_id( &mv, i) == top_id ){
			bit_position = i;
			found = 1;
			break;
		};
    };
   
    
    if (found == 0)
	fail("mdd_mark: interleaving error\n");

    mv.encoding[bit_position] = phase;
    
}

void
mdd_unmark(mgr, top)
mdd_manager *mgr;
bdd_t *top; 
{
    int i, top_id, found = 0;
    int bit_position = 0; /* initialize for lint */
    mvar_type mv;


    top_id = bdd_top_var_id(top);
    mv = find_mvar_id(mgr, top_id);

    for (i=0; i<mv.encode_length; i++) 
		if ( mdd_ret_bvar_id( &mv, i) == top_id ){
			bit_position = i;
			found = 1;
			break;
		};

    if (found == 0)
	fail("mdd_unmark: interleaving error\n");
    mv.encoding[bit_position] = 2;
}

mvar_type 
find_mvar(mgr, name)
mdd_manager *mgr;
char *name;
{
    int i;
    mvar_type mv;
    array_t *mvar_list = mdd_ret_mvar_list(mgr);

    for (i=0; i<array_n(mvar_list); i++) {
	mv = array_fetch(mvar_type, mvar_list, i);
        if (strcmp(mv.name, name) == 0) return mv;
    }
    fail("find_mvar: cannot find name in mvar_list\n");
    return mv;
}

array_t *
mdd_ret_mvar_list(mgr)
mdd_manager *mgr;
{
    bdd_external_hooks *hook;    
    array_t *mvar_list;

    hook =  bdd_get_external_hooks(mgr);
    mvar_list = ((mdd_hook_type *)(hook->mdd))->mvar_list;

    return mvar_list;
}

void
mdd_set_mvar_list(mgr, mvar_list)
mdd_manager *mgr;
array_t *mvar_list;
{
    bdd_external_hooks *hook;    

    hook =  bdd_get_external_hooks(mgr);
    ((mdd_hook_type *)(hook->mdd))->mvar_list = mvar_list;
}


array_t *
mdd_ret_bvar_list(mgr)
mdd_manager *mgr;
{
    bdd_external_hooks *hook;    
    array_t *bvar_list;

    hook =  bdd_get_external_hooks(mgr);
    bvar_list = ((mdd_hook_type *)(hook->mdd))->bvar_list;

    return bvar_list;
}


int
mdd_ret_bvar_id(mvar_ptr, i)
mvar_type *mvar_ptr;
int i;
{
	
	return ( array_fetch(int, mvar_ptr->bvars, i) );
}

bvar_type
mdd_ret_bvar(mvar_ptr, i, bvar_list)
mvar_type *mvar_ptr;
int i;
array_t *bvar_list;
{
	int bvar_id;
	
	bvar_id = array_fetch(int, mvar_ptr->bvars, i);
	
	return array_fetch(bvar_type, bvar_list, bvar_id);
}

/************************************************************************/
/* This pacakege implements counting the num of onset points of an Mdd.	*/
/************************************************************************/
/* Given an Mdd, returns the num of onset points.  By construction of	*/
/* Mdd's, some points not in the range of Mdd vars may be inculded 	*/
/* in the onset. These fake points must first be removed.		*/

double
mdd_count_onset( mddMgr, aMdd, mddIdArr )
	mdd_manager	*mddMgr;
	mdd_t		*aMdd;
	array_t		*mddIdArr;
{	
	bdd_t		*onvalBdd, *aOnvalBdd, *onsetBdd, *tmpBdd;
	double		onsetNum;
	array_t		*bddVarArr;
	int		i, arrSize, mddId;

	arrSize = array_n( mddIdArr );
	onvalBdd = bdd_one( mddMgr );

	for ( i = 0 ; i < arrSize ; i++ ) {
	    mddId = array_fetch( int, mddIdArr, i );
	    aOnvalBdd = mddRetOnvalBdd( mddMgr, mddId );

	    tmpBdd = bdd_and( onvalBdd, aOnvalBdd, 1, 1 );
	    bdd_free( onvalBdd );
	    bdd_free( aOnvalBdd );
	    onvalBdd = tmpBdd;
	}
	onsetBdd = bdd_and( onvalBdd, aMdd, 1, 1 );
	bdd_free( onvalBdd );

	bddVarArr = mdd_ret_arr_bin_vars( mddMgr, mddIdArr );
	onsetNum = bdd_count_onset( onsetBdd, bddVarArr );
	bdd_free( onsetBdd );
	mddFreeBddArr( bddVarArr );
	return( onsetNum );
}		/* mdd_count_onset */

/************************************************************************/
array_t*
mddRetBinVars( mddMgr, mddId )
	mdd_manager	*mddMgr;
	int		mddId;
{
	array_t		*bddVarArr;
	mvar_type	mVar;
	unsigned int	i, j;

	mVar = mddGetVarById( mddMgr, mddId );
	bddVarArr = array_alloc( bdd_t *, mVar.encode_length );	

        for (i = 0 ; i <  mVar.encode_length ; i++ ) {
	    j = mdd_ret_bvar_id( &mVar, i);
	    array_insert_last( bdd_t *, bddVarArr, 
			       bdd_get_variable( mddMgr, j ));
	}
	return( bddVarArr );
}		/* mddRetBinVars */

/************************************************************************/
/* Returns an array of binary vars (bdd_t *'s), given an array of 	*/
/* Mdd id's.								*/

array_t*
mdd_ret_arr_bin_vars( mddMgr, mddIdArr )
	mdd_manager	*mddMgr;
	array_t		*mddIdArr;
{
	array_t		*bddVarArr;
	unsigned int	k, j, i, arrSize;
	mvar_type	mVar;

	bddVarArr = array_alloc( bdd_t *, 3 * array_n( mddIdArr ));	

	arrSize = array_n( mddIdArr );
	for ( i = 0 ; i < arrSize ; i++ ) {
	    mVar = mddGetVarById( mddMgr, array_fetch( int, mddIdArr, i ));
	     
            for ( k = 0 ; k < mVar.encode_length ; k++ ) {
		j = mdd_ret_bvar_id( &mVar, k);
		array_insert_last( bdd_t *, bddVarArr, 
				   bdd_get_variable( mddMgr, j ));
	    }

	}
	return( bddVarArr );
}		/* mdd_ret_arr_bin_vars */

/************************************************************************/
static	bdd_t*
mddRetOnvalBdd( mddMgr, mddId )
	mdd_manager	*mddMgr;
	int		mddId;
{
	bdd_t		*onvalBdd;
	mvar_type	mVar;
	int		valNum, high;
	array_t		*bddVarArr;	
	
	mVar = mddGetVarById( mddMgr, mddId );
	valNum = mVar.values;
	high = (int) pow( (double) 2, (double) mVar.encode_length ); 
	assert( (valNum == 1)  || ( (valNum <= high) && (valNum > high/2) ));
	if ( valNum == high )
	    onvalBdd = bdd_one( mddMgr );
	else {
	    bddVarArr = mddRetBinVars( mddMgr, mddId );
	    onvalBdd = mddIntRetOnvalBdd( mddMgr, valNum, 0, high, 
					  0, bddVarArr );
	    mddFreeBddArr( bddVarArr );
	}
	return( onvalBdd );
}		/* mddRetOnvalBdd */	

/************************************************************************/
static	bdd_t*
mddIntRetOnvalBdd( mddMgr, valNum, low, hi, level, bddVarArr )
	mdd_manager	*mddMgr;
	int		valNum, low, hi, level;
	array_t		*bddVarArr;
{
	int		mid;
	bdd_t		*curVar, *recBdd, *tmpBdd;
	bdd_t		*onvalBdd = NIL(bdd_t); /* initialized for lint */

	mid = (low + hi) / 2;
		curVar = array_fetch( bdd_t *, bddVarArr, level );

	if 	( valNum > mid ) {
	    recBdd = mddIntRetOnvalBdd( mddMgr, valNum, mid, hi, 
					level+1, bddVarArr );
	    tmpBdd = bdd_and( recBdd, curVar, 1, 1 );
	    bdd_free( recBdd );
	    onvalBdd = bdd_or( tmpBdd, curVar, 1, 0 );
	    bdd_free( tmpBdd );
	}
	else if ( valNum < mid ) {
	    recBdd = mddIntRetOnvalBdd( mddMgr, valNum, low, mid, 
					level+1, bddVarArr );
	    onvalBdd = bdd_and( recBdd, curVar, 1, 0 );
	    bdd_free( recBdd );
	}
	else if ( valNum == mid ) 
	    onvalBdd = bdd_not( curVar );
	return( onvalBdd );
}		/* mddIntRetOnvalBdd */

/************************************************************************/
/* Given an array of bdd nodes, frees the array.			*/

void
mddFreeBddArr( bddArr )
	array_t	*bddArr;
{
	int	i, arrSize;

	arrSize = array_n( bddArr );
	for ( i = 0 ; i < arrSize ; i++ ) 
	    bdd_free( array_fetch( bdd_t *, bddArr, i ) );
	array_free( bddArr );
}		/* mddFreeBddArr */

array_t  *
mdd_ret_bvars_of_mvar( mvar_ptr )
mvar_type	*mvar_ptr;
{

	return mvar_ptr->bvars;
}

/************************************************************************/
/* mdd_get_care_set returns the care set of the mdd manager */ 

mdd_t *mdd_get_care_set(mdd_mgr)
    mdd_manager *mdd_mgr;
{
    mdd_t *temp;
    mvar_type mv;
    mdd_manager *bdd_mgr;

    int mvar_id,i,j,val_j,value;
    array_t *mvar_list, *bvar_list;
    bdd_t *care_set, *care_val, *care_cube,*bit_j;
    
    mvar_list = mdd_ret_mvar_list(mdd_mgr);
    bvar_list = mdd_ret_bvar_list(mdd_mgr);
    bdd_mgr = mdd_mgr;
    
    care_set = bdd_one(bdd_mgr);
    
    for (mvar_id =0; mvar_id < array_n(mvar_list); mvar_id++)
        {
            mv = array_fetch(mvar_type, mvar_list, mvar_id);
            care_val = bdd_zero(bdd_mgr);
                
            for (i=0; i< (mv.values); i++)
                {
                    value = i;
                    care_cube = bdd_one(bdd_mgr);
                    for(j=0; j< mv.encode_length; j++ )
                        {
                            bit_j = bdd_get_variable(bdd_mgr,mdd_ret_bvar_id(&mv, j));
                            val_j = value % 2;
                            value = value/2;
                            temp = care_cube;
                            care_cube = bdd_and(temp,bit_j,1,val_j);
                            bdd_free(temp);
                        }
                    temp = care_val;
                    care_val = bdd_or(temp,care_cube,1,1);
                    bdd_free(temp);
                    bdd_free(care_cube);
                }
            temp = care_set;
            care_set = bdd_and(temp,care_val,1,1);
            bdd_free(care_val);
            bdd_free(temp);
        }
    return care_set;
}

/* Corrected mdd_cproject */
/* returns only valid carepoints */

mdd_t *mdd_cproject(mgr,T,mvars)
    mdd_manager *mgr;
    mdd_t *T;
    array_t *mvars;
{
    mdd_t *care_set, *new_T, *T_proj;
     array_t *bdd_vars;
    int i, mv_no;
    unsigned int j;
    mvar_type mv;
    bdd_t *temp;
    array_t *mvar_list = mdd_ret_mvar_list(mgr);


    care_set = mdd_get_care_set(mgr);
    new_T = bdd_and(T,care_set,1,1);
    bdd_free(care_set);

      if ( mvars == NIL(array_t) ) {
        T_proj = bdd_dup(T);
        printf("\nWARNING: Empty Array of Smoothing Variables\n");
        return T_proj;
    }
    else if ( array_n(mvars) == 0) {
        T_proj = bdd_dup(T);
        printf("\nWARNING: Empty Array of Smoothing Variables\n");
        return T_proj;
    }
                
        
    bdd_vars = array_alloc(bdd_t*, 0);     
    for (i=0; i<array_n(mvars); i++) {
        mv_no = array_fetch(int, mvars, i);
        mv = array_fetch(mvar_type, mvar_list, mv_no);
        if (mv.status == MDD_BUNDLED) {
            (void) fprintf(stderr, 
                        "\nmdd_smooth: bundled variable %s used\n",mv.name);
            fail("");
        }

        for (j = 0;j < mv.encode_length; j ++) {
            temp = bdd_get_variable(mgr, mdd_ret_bvar_id(&mv,j) );
            array_insert_last(bdd_t *, bdd_vars, temp);
        }
    }
        
  
    T_proj = bdd_cproject(new_T,bdd_vars);
    bdd_free(new_T);

    for (i=0; i<array_n(bdd_vars); i++) {
        temp = array_fetch(bdd_t *, bdd_vars, i);
        bdd_free(temp);
    }
    array_free(bdd_vars);

    
    return T_proj;
}

void
mdd_print_support(mdd_manager *mgr, mdd_t *f)
{
    array_t *support_list = mdd_get_support(mgr, f);
    array_t *mvar_list = mdd_ret_mvar_list(mgr);
    int nSupports = array_n(support_list);
    int i, j;
    mvar_type mv;
    int id;

    for (i = 0; i < nSupports; i++) {
	id = array_fetch(int, support_list, i);
	mv = array_fetch(mvar_type, mvar_list, id);
	if (id == mv.mvar_id)
	    printf("[%d] = %s\n", i, mv.name);
	else { /* needs to be checked */
	    for (j = 0; j < array_n(mvar_list); j++) {
		mv = array_fetch(mvar_type, mvar_list, j);
		if (id == mv.mvar_id) {
		    printf(" [%d] = %s", i, mv.name);
		    break;
		}
	    }
	}
    }
    printf("\n");
}

/**Function********************************************************************

  Synopsis    [Returns an array of BDD ids corresponding to a MDD variable.]

  Description [This function takes an MddId. It returns an array of BDD ids
  corresponding to the bits.]

  SideEffects []

******************************************************************************/
array_t *
mdd_id_to_bdd_id_array(mdd_manager *mddManager, int mddId)
{
  array_t     *bddIdArray;
  mvar_type   mddVar;
  unsigned int    i, j;
  
  mddVar = array_fetch(mvar_type, mdd_ret_mvar_list(mddManager), mddId);
  bddIdArray = array_alloc(int, mddVar.encode_length);
  
  for (i=0; i<mddVar.encode_length; i++){
    j = mdd_ret_bvar_id(&mddVar, i);
    array_insert_last(int, bddIdArray, j);
  }
  return bddIdArray;
}

/**Function********************************************************************

  Synopsis    [Returns an array of Bdd_t's corresponding to a Mdd variable.]

  Description [This function takes an MddId. It returns an array of bdd_t's
  corresponding to the bits.]

  SideEffects []

******************************************************************************/
array_t *
mdd_id_to_bdd_array(mdd_manager *mddManager, int mddId)
{
  array_t     *bddArray;
  mvar_type   mddVar;
  unsigned int    i, j;
  
  mddVar = array_fetch(mvar_type, mdd_ret_mvar_list(mddManager), mddId);
  bddArray = array_alloc(bdd_t*, mddVar.encode_length);
  
  for (i=0; i<mddVar.encode_length; i++){
    j = mdd_ret_bvar_id(&mddVar, i);
    array_insert_last(bdd_t*, bddArray,
                      bdd_get_variable((bdd_manager *)mddManager, j));
  }
  return bddArray;
}


/**Function********************************************************************

  Synopsis    [Returns an array of Bdd_t's corresponding to an array of Mdd
  ids.] 

  Description [This function takes an array of MddId's. For each MddId it
  returns an array of bdd_t's corresponding to the bits. These arrays of bdd_ts
  are concatenated together and returned.]

  SideEffects []

******************************************************************************/
array_t *
mdd_id_array_to_bdd_array(mdd_manager *mddManager, array_t *mddIdArray)
{
  array_t *bddArray;
  int i;

  bddArray = array_alloc(bdd_t*, 0);
  for (i=0; i<array_n(mddIdArray); i++){
    int mddId;
    array_t *tmpBddArray;
    
    mddId = array_fetch(int, mddIdArray, i);
    tmpBddArray = mdd_id_to_bdd_array(mddManager, mddId);
    array_append(bddArray, tmpBddArray);
    array_free(tmpBddArray);
  }
  return bddArray;
}

/**Function********************************************************************

  Synopsis    [Returns an array of bddId's corresponding to an array of Mdd
  ids.] 

  Description [This function takes an array of MddId's. For each MddId it
  returns an array of bddId's corresponding to the bits. These arrays of bddId's
  are concatenated together and returned.]

  SideEffects []

******************************************************************************/
array_t *
mdd_id_array_to_bdd_id_array(mdd_manager *mddManager, array_t *mddIdArray)
{
  array_t *bddIdArray;
  int i;

  bddIdArray = array_alloc(int, 0);
  for (i=0; i<array_n(mddIdArray); i++){
    int mddId;
    array_t *tmpBddIdArray;
    mddId = array_fetch(int, mddIdArray, i);
    tmpBddIdArray = mdd_id_to_bdd_id_array(mddManager, mddId);
    array_append(bddIdArray, tmpBddIdArray);
    array_free(tmpBddIdArray);
  }
  return bddIdArray;
}

/**Function********************************************************************

  Synopsis [Given an Mvf representing the functionality of a multi-valued
  variable, it returns an array of Bdd's representing the characteristic
  function of the relation of the various bits of the multi-valued variable.] 

  Description [Suppose y is a k-valued variable and takes values
              0,1,..,k-1. Then the input to this function is an array with k
              Mdds each representing the onset of the respective value of the
              variable (the ith Mdd representing the onset when y takes the
              value (i-1). Suppose m bits are needed to encode the k values of
              y. Then internally y is represented as y_0, y_1, ...,
              y_(m-1). Now the functionality of each bit of y can be computed
              by proper boolean operation on the functions representing the
              onsets of various values of y. For instance if y is a 4-valued
              variable. To achieve that we do the following:
              For each bit b{
                relation = 0;
                For each value j of the variable{
                  Ej = Encoding function of the jth value
                  Fj = Onset function of the jth value
                  If (b appears in the positive phase in Ej) then
                     relation += b * Fj 
                  else if (b appears in the negative phase in Ej) then
                     relation += b'* Fj
                  else if (b does not appear in Ej) then
                     relation += Ej
                }
              }
              Note that the above algorithm does not handle the case when a
              bit appears in both phases in the encoding of any value of the
              variable. Hence the assumption behind the above algorithm is that
              the values are encoded as cubes.
              The case when the encoding are functions can be handled by more
              complex algorithm. In that case, we will not be able to build the
              relation for each bit separately. Something to be dealt with in
              the later work.
              ]              
  SideEffects []

******************************************************************************/
array_t *
mdd_fn_array_to_bdd_rel_array(mdd_manager *mddManager, int mddId,
                              array_t *mddFnArray)
{
  array_t *bddRelationArray, *mddLiteralArray, *valueArray, *bddArray;
  mvar_type mddVar;
  int i, j, numValues, numEncodingBits;
  
  mddVar = array_fetch(mvar_type, mdd_ret_mvar_list(mddManager), mddId);
  numValues = array_n(mddFnArray);
  assert(mddVar.values == numValues);

  /*
   * The following is to check whether each encoding is cube or not.
   * Since Berkeley MDD package always does the cube encoding this checking has
   * been turned off currently.
   */
  
  valueArray = array_alloc(int, 1);
  mddLiteralArray = array_alloc(mdd_t*, 0);
  for (i=0; i<numValues; i++){
    mdd_t *mddLiteral;
    
    array_insert(int, valueArray, 0, i);
    /* Form the Mdd corresponding to this value */
    mddLiteral = mdd_literal(mddManager, mddId, valueArray);
    /* Check if this is a cube */
    if (bdd_is_cube(mddLiteral) == FALSE){ 
       fprintf(stderr, "The encoding of the variable %s for the value %d isnot a cube.\n It can result in wrong answers.\n", mddVar.name, i); 
    } 
    array_insert_last(mdd_t*, mddLiteralArray, mddLiteral);
  }
  array_free(valueArray);

  bddRelationArray = array_alloc(bdd_t*, 0);
  numEncodingBits = mddVar.encode_length;
  bddArray = mdd_id_to_bdd_array(mddManager, mddId);
  for (i=0; i<numEncodingBits; i++){
    bdd_t *bdd, *bddRelation, *bddNot;

    bddRelation = bdd_zero((bdd_manager *)mddManager);
    bdd = array_fetch(bdd_t*, bddArray, i);
    bddNot = bdd_not(bdd);
    for (j=0; j<numValues; j++){
      bdd_t *mddFn, *posCofactor, *negCofactor, *tmpBddRelation;
      mdd_t *mddLiteral, *literalRelation;
      
      mddLiteral = array_fetch(mdd_t*, mddLiteralArray, j);
      posCofactor = bdd_cofactor(mddLiteral, bdd);
      negCofactor = bdd_cofactor(mddLiteral, bddNot);
      mddFn = array_fetch(mdd_t*, mddFnArray, j);
      if (bdd_is_tautology(posCofactor, 0)){
        literalRelation = bdd_and(bddNot, mddFn, 1, 1);
      }
      else if (bdd_is_tautology(negCofactor, 0)){
        literalRelation = bdd_and(bdd, mddFn, 1, 1);
      }
      else {
        assert(bdd_equal(posCofactor, negCofactor));
        literalRelation = bdd_dup(mddFn);
      }
      bdd_free(posCofactor);
      bdd_free(negCofactor);
      tmpBddRelation = bdd_or(bddRelation, literalRelation, 1, 1);
      bdd_free(literalRelation);
      bdd_free(bddRelation);
      bddRelation = tmpBddRelation;
    }
    array_insert_last(bdd_t*, bddRelationArray, bddRelation);
    bdd_free(bdd);
    bdd_free(bddNot);
  }
  /* Free stuff */
  mdd_array_free(mddLiteralArray);
  array_free(bddArray);
  return bddRelationArray;
}

/*Ashwini :add function mdd_pick_one_minterm() 
 * as a wrapper to the bdd_bdd_pick_one_minterm_random() function.*/
mdd_t *
mdd_pick_one_minterm(mddMgr, aMdd, mddIdArr, n)
  mdd_manager	*mddMgr;
  mdd_t		*aMdd;
  array_t 	*mddIdArr;
  int		n;
{
    bdd_t	*onvalBdd, *aOnvalBdd, *onsetBdd, *tmpBdd;
    array_t	*bddVarArr;
    int		i, arrSize, mddId;
    mdd_t	*minterm;


    arrSize = array_n( mddIdArr );
    onvalBdd = bdd_one( mddMgr );

    for ( i = 0 ; i < arrSize ; i++ ) {
	mddId = array_fetch( int, mddIdArr, i );
	aOnvalBdd = mddRetOnvalBdd( mddMgr, mddId );

	tmpBdd = bdd_and( onvalBdd, aOnvalBdd, 1, 1 );
	bdd_free( onvalBdd );
	bdd_free( aOnvalBdd );
	onvalBdd = tmpBdd;
    }
    onsetBdd = bdd_and( onvalBdd, aMdd, 1, 1 );
    bdd_free( onvalBdd );

    /* I bet this function returns the list of boolean variables 
       encoding the mdd */ 
    bddVarArr = mdd_ret_arr_bin_vars(mddMgr, mddIdArr);

    minterm = bdd_bdd_pick_one_minterm_random(onsetBdd, bddVarArr,
	array_n(bddVarArr));

    bdd_free(onsetBdd);
    mddFreeBddArr(bddVarArr);
    return(minterm);
}

void
mdd_srandom(seed)
        long seed;
{
        bdd_bdd_srandom(seed);
}

/**Function********************************************************************

  Synopsis    [Simulates the system represented by the transition relation
              array, tauArray and checks at each iteration, if the invariants,
	      given by the invArray are satisfied.]

  Description []

  SideEffects []
******************************************************************************/

void 
mdd_simulate(mdd_mgr, init_mdd, tauArray, outvarsArray,
	unpvarsArray, invmddArray, maxIter, mode)
    mdd_manager	*mdd_mgr;
    mdd_t 	*init_mdd;
    array_t	*tauArray;   /* sorted transition relation array*/
    array_t 	*outvarsArray;
    array_t 	*unpvarsArray;
    array_t 	*invmddArray;   /* array if invariant bdds to be checked */
    int		maxIter;     /* max iterations to run the simulations */
    int 	mode;	     /* 0= silent, 1= verbose mode*/
{
    bdd_t 	* onvalBdd, *aOnvalBdd, *tmpBdd, *inv;
    bdd_t 	* tausetbdd,  *tauAndVar, *tau1, *tau0, *bddid, *unpbddid;
    array_t	*bddIdArr, * oMddvars, *invArray, *unpbddIdArr;
    array_t 	*smoothvars, *tempArray, *mddTauArray, *bddtauArray;
    int 	i, j, k, mddid, numElements, eval0, eval1, debug =0, loop;
    int 	numbddvars = 0, *pvalArray, *unpvalArray, iterations, valid;
    mdd_t 	*tau;
    array_t *allpbddvars, *msupp, *bsupp, *temp2Array, *iMddvars;
    bdd_t  *taubdd, *one, *initsetbdd, *bdd0, *bdd1, *bdd0E, *bdd1E;
    array_t  *initUnpbddIdArr, *initbddArray, *init_support;
    unsigned int index;
    int sanitytest = 1, done =0;
    bdd_t * temp, *notTemp, *stemp;
    bdd_node * pnode, *unpnode;
    unsigned int pindex, unpindex;
    array_t * allunpbddvars;

    one = bdd_one(mdd_mgr);
    mddTauArray = array_alloc(array_t *, 0);
    allpbddvars = array_alloc(array_t *, 0);
    allunpbddvars =  array_alloc(array_t *, 0);

    /* print the mvars list and the corresponding bvars */
    /*if(debug) {
	array_t * mvar_list = mdd_ret_mvar_list(mdd_mgr);
	mvar_type mv;
	for( i = 0; i < array_n(mvar_list) ; i++) {
	    mv = array_fetch(mvar_type, mvar_list, i);
	    fprintf(stderr, " %s : ", mv.name);
	    for ( k = 0 ; k < mv.encode_length ; k++ ) {
		j = mdd_ret_bvar_id( &mv, k);
		fprintf(stderr," %d , ", j);
	    }
	    fprintf(stderr,"\n");
	}
    }*/

    /* first get initsetbdd, the init_mdd within the domain of its 
     * support vars*/
    init_support = mdd_get_support(mdd_mgr, init_mdd);
    onvalBdd = bdd_one(mdd_mgr);
    arrayForEachItem(int, init_support, j, mddid) {
	/*returns the bdd in the domain mddid */
	aOnvalBdd = mddRetOnvalBdd( mdd_mgr, mddid);
	tmpBdd = bdd_and( onvalBdd, aOnvalBdd, 1, 1 );
	bdd_free( onvalBdd );
	bdd_free( aOnvalBdd );
	onvalBdd = tmpBdd;
    }
    initsetbdd = bdd_and(init_mdd, onvalBdd, 1,1);
    bdd_free(onvalBdd);

    smoothvars = array_alloc(bdd_t *, 0);
    initbddArray = array_alloc(bdd_t *, 0);

    initUnpbddIdArr = mdd_ret_arr_bin_vars( mdd_mgr, init_support);
    array_free(init_support);

    /* as the inititial condition bdd has been unprimed, it only
     * has the unprimed vars */
    arrayForEachItem(bdd_t *, initUnpbddIdArr, i, bddid) {
	bdd1 = bdd_and(initsetbdd, bddid, 1,1);
	array_insert_last(bdd_t *, smoothvars, bddid);
	bdd1E = bdd_smooth(bdd1, smoothvars);
	bdd_free(bdd1);
	array_insert_last(bdd_t *, initbddArray, bdd1E);
	bdd0 = bdd_and(initsetbdd, bddid, 1,0);
	bdd0E = bdd_smooth(bdd0, smoothvars);
	bdd_free(bdd0);
	array_insert_last(bdd_t *, initbddArray, bdd0E);
    }

    bdd_free(initsetbdd);
    array_free(smoothvars);

    /*reverse the initbddArray, for the bdd with most vars
     * existentially quantified has to be evaluated first*/
    tempArray = array_alloc(bdd_t *, 0); 
    numElements = array_n(initbddArray);
    for( j = numElements-1 ; j >= 0; j-- ) {
	bdd1 = array_fetch(bdd_t *, initbddArray, j);
	array_insert_last(bdd_t *, tempArray, bdd1);
    }
    array_free(initbddArray);
    initbddArray = tempArray;

    /* Also reverse the initUnpbddIdArr*/
    tempArray = array_alloc(bdd_t *, 0); 
    numElements = array_n(initUnpbddIdArr);
    for( j = numElements-1 ; j >= 0; j-- ) {
	temp = array_fetch(bdd_t *, initUnpbddIdArr, j);
	array_insert_last(bdd_t *, tempArray, temp);
    }
    array_free(initUnpbddIdArr);
    initUnpbddIdArr = tempArray;

    /*sanity test for the initbddArray */
    if(sanitytest) {
	k = 0;
	smoothvars = array_alloc(bdd_t *, 0);
	arrayForEachItem(bdd_t *, initUnpbddIdArr, j, bddid) {
	    bdd0 = array_fetch(bdd_t *, initbddArray, k++);
	    bdd1 = array_fetch(bdd_t *, initbddArray, k++);

	    msupp = mdd_get_support(mdd_mgr,bdd1);
	    bsupp = mdd_ret_arr_bin_vars(mdd_mgr, msupp);
	    array_insert_last(bdd_t *, smoothvars, bddid); 

	    temp = bdd_or(bdd0, bdd1, 1,1);
	    stemp = bdd_smooth(temp, smoothvars);
            mdd_free(temp);
            notTemp = mdd_not(stemp);
            mdd_free(stemp);
            temp = bdd_smooth(notTemp, bsupp);
            mdd_free(notTemp);
            notTemp = mdd_not(temp);
            if(!mdd_equal(notTemp, one)) {
		fprintf(stderr,"sanity test failed for %d the initbdd var\n ",
			j);
	    }
	    mdd_free(notTemp);
            array_free(msupp);
            mddFreeBddArr(bsupp);
	}
	array_free(smoothvars);
    }

    /* now compute the tau0's and tau1's for each output bdd var
     * of all the taus and put them in the mddTauArray*/
    arrayForEachItem(mdd_t *, tauArray, i, tau) {
	/* compute tausetbdd to be the bdd within the 
	 * domain of the output mddids */
	/* oMddvars are the output primed mdd vars of the ith tau */
	oMddvars = array_fetch(array_t *, outvarsArray, i);
	/* iMddvars are the corresponding unprimed mdd vars */
	iMddvars = array_fetch(array_t *, unpvarsArray, i);
	onvalBdd = bdd_one(mdd_mgr);
	arrayForEachItem(int, oMddvars, j, mddid ) {
	    /*returns the bdd in the domain mddid */
	    aOnvalBdd = mddRetOnvalBdd( mdd_mgr, mddid);
	    tmpBdd = bdd_and( onvalBdd, aOnvalBdd, 1, 1 );
	    bdd_free( onvalBdd );
	    bdd_free( aOnvalBdd );
	    onvalBdd = tmpBdd;
	}
	/*tausetbdd is the tau bdd where the output vars can 
	 * only be chosen from the domain of the mddvars*/
	tausetbdd = bdd_and(tau, onvalBdd, 1,1);
	bdd_free(onvalBdd);

	/* get the array of the bddid's, given the mddids*/
	bddIdArr = mdd_ret_arr_bin_vars( mdd_mgr, oMddvars);
	unpbddIdArr = mdd_ret_arr_bin_vars( mdd_mgr, iMddvars);

	smoothvars = array_alloc(bdd_t *, 0);
	bddtauArray = array_alloc(bdd_t *, 0);

	/* For each output bddid of the tausetbdd compute
	 * tau1 = (tausetbdd /\ (bddid = 0)) and 
	 * tau0 = (tausetbdd /\ (bddid = 1)). Existentially quantify
	 * the smoothvars from these bdds to get the 
	 * tausetbdd when the value of bddid = 1/0.
	 * */
	arrayForEachItem(bdd_t *, bddIdArr, j, bddid) {
	    tauAndVar = bdd_and(tausetbdd, bddid, 1,1);
	    array_insert_last(bdd_t *, smoothvars, bddid);
	    tau1 = bdd_smooth( tauAndVar, smoothvars);
	    bdd_free(tauAndVar);
	    array_insert_last(bdd_t *, bddtauArray, tau1);
	    tauAndVar = bdd_and(tausetbdd, bddid, 1,0);
	    tau0 = bdd_smooth( tauAndVar, smoothvars);
	    bdd_free(tauAndVar);
	    array_insert_last(bdd_t *, bddtauArray, tau0);
	}

	bdd_free(tausetbdd);
	array_free(smoothvars);

	/* reverse the bddtauArray, for the tau which has the most
	 * variables existentially quantified has to be evaluated
	 * first.*/
	tempArray = array_alloc(bdd_t *, 0);
	numElements = array_n(bddtauArray);
	for( j = numElements-1 ; j >= 0; j-- ) {
	    tau1 = array_fetch(bdd_t *, bddtauArray, j);
	    array_insert_last(bdd_t *, tempArray,  tau1);
	}
	array_free( bddtauArray);
	bddtauArray = tempArray;
	array_insert_last(array_t *, mddTauArray, bddtauArray);

	/* we have to now reverse the bddIdArry and the unpbddIdArr 
	 * too as it has to map to the reversed bddtauArray. The ith 
	 * var of the bddIdArr maps to the ith tau0 and tau1 of 
	 * bddtauArray*/
	tempArray= array_alloc(bdd_t *, 0);
	temp2Array= array_alloc(bdd_t *, 0);
	numElements = array_n(bddIdArr);
	for ( j = numElements-1 ; j >= 0; j-- ) {
	    temp = array_fetch(bdd_t *, bddIdArr, j);
	    array_insert_last(bdd_t *, tempArray, temp);
	    temp = array_fetch(bdd_t *, unpbddIdArr, j);
	    array_insert_last(bdd_t *, temp2Array, temp);
	}
	array_free(bddIdArr);
	array_free(unpbddIdArr);
	bddIdArr = tempArray;
	unpbddIdArr = temp2Array;
	array_insert_last(array_t *, allpbddvars, bddIdArr);
	array_insert_last(array_t *, allunpbddvars, unpbddIdArr);

	/*numbddvars has to be the total number of primed (or unprimed) bdd 
	 * vars in the system */
	numbddvars += array_n(bddIdArr);
	if(sanitytest) {
	    /* this sanity test checks that the bddtauArray got 
	     * correctly evaluates the output vars of the system
	     * We first smooth from the tau0 and tau1, those vars
	     * which will be evaluated before we evaluate tau0 and 
	     * tau1 for say var x. We then check to see
	     * Forall V (tau0 /\ tau1) = 1
	     * where V are the vars in the support of tau0 (or tau1).
	     * Here Forall v is done as (not Exists not)
	     */
	    k = 0;
	    smoothvars = array_alloc(bdd_t *, 0);
	    arrayForEachItem(bdd_t *, bddIdArr, j, bddid) {
		tau0 = array_fetch(bdd_t *, bddtauArray, k++);
		tau1 = array_fetch(bdd_t *, bddtauArray, k++);

		msupp = mdd_get_support(mdd_mgr,tau1);
		bsupp = mdd_ret_arr_bin_vars(mdd_mgr, msupp);

		array_insert_last(bdd_t *, smoothvars, bddid);
		temp = bdd_or(tau1, tau0, 1,1);
		/* smooth the vars whose values are already known
		 * or those which will be evaluated before temp */
		stemp = bdd_smooth(temp, smoothvars);
		mdd_free(temp);
		notTemp = mdd_not(stemp);
		mdd_free(stemp);
		temp = bdd_smooth(notTemp, bsupp);
		mdd_free(notTemp);
		notTemp = mdd_not(temp);
		if(!mdd_equal(notTemp, one)) {
		    fprintf(stderr,"sanity test failed for %d the tau of %d array\n"
			    , j, i);
		}
		mdd_free(notTemp);
		array_free(msupp);
		mddFreeBddArr(bsupp);
	    }
	    array_free(smoothvars);
        }
    }
    bdd_free(one);

    if (debug ) {
	fprintf(stderr,"The total number of output bddvars in the system are %d\n",
		numbddvars);
    }

    /* construct two int arrays of all the bdd vars in the system and
     * initially assign a value of 0 for these vars. The initial values
     * assigned do not matter, for we consider a closed system that
     * does not take any inputs. The total number of
     * vars in the system = numbddvars*2 because numbddvars is only the 
     * primed bdd vars. There are also the unprimed vars for each
     * primed one. We take two arrays of primed and unprimed vars, both
     * of size numbddvars*2, for these arrays are indexed by the
     * bddids of the vars which are not in the numerical order. */
    pvalArray = calloc (numbddvars*2, sizeof(int));
    unpvalArray = calloc (numbddvars*2, sizeof(int));

    if (sanitytest) {
	if( array_n(initbddArray) != 2*array_n(initUnpbddIdArr) ) {
	    fprintf(stderr,"ERROR: Wrong sizes of initbddArray and initUnpbddIdArr\n");
	    fprintf(stderr,"array size of initbddArray = %d\n",
		    array_n(initbddArray));
	    fprintf(stderr,"array size of initUnpbddIdArr = %d\n",
		    array_n(initUnpbddIdArr));
	}
    }


    /* Set the unprimed vars in the unpvalArray as per the initial 
     * condition bdds */
    k = 0;
    arrayForEachItem(bdd_t *, initUnpbddIdArr, j, bddid) {
        bdd0 = array_fetch(bdd_t *, initbddArray, k++);
        bdd1 = array_fetch(bdd_t *, initbddArray, k++);

	eval0 =  bdd_bdd_eval_bdd( bdd0, unpvalArray);
	eval1 =  bdd_bdd_eval_bdd( bdd1, unpvalArray);

	unpnode = bdd_extract_node_as_is(bddid);
        unpindex =  bdd_node_read_index(unpnode);

        if((eval0 == -1) || (eval1 == -1)) {
	    fprintf(stderr, "ERROR evaluating the bdd0 and bdd1 bdds for %d initial condition\n",
		j);
	} else if ((eval0 == 0) && (eval1 == 0)) {
	    fprintf(stderr, "ERROR:both values bdd0 and bdd1 are zeroes. \n");
	} else if ((eval0 == 1) && (eval1 == 1))  {
	    /* both 0 and 1 are valid assignments to bddid. 
	     * So choose a random value for the next state
	     * assignment */
	    unpvalArray[unpindex] = ((Cudd_Random() & 0x1) ? 1 : 0);
	} else if (eval0 == 1) {
	    unpvalArray[unpindex] = 0;
	} else if (eval1 == 1) {
	    unpvalArray[unpindex] = 1;
	}
    }

    /* free the initial condition bdds, and bddids */
    mddFreeBddArr(initUnpbddIdArr);
    arrayForEachItem( bdd_t *, initbddArray, j, bdd0) {
	bdd_free(bdd0);
    }
    array_free(initbddArray);

    /* check whether the initial condition array satisfies
     * the invariants */
    arrayForEachItem(array_t *,invmddArray, i, invArray) {
        arrayForEachItem(bdd_t *, invArray, j, inv) {
           valid = bdd_bdd_eval_bdd(inv, unpvalArray);
	   if( valid == 0) {
		fprintf(stderr, " Invariant %d failed for initial condition \n"
			, i);
	    } else if (valid == -1) {
		fprintf(stderr, " ERROR: evaluating invariant %d \n", i);
	    }
	}
    }

    if(debug) {
	fprintf(stderr, "\nvalues of the unprimed vals array for init condition \n");
	for( loop = 0; loop < numbddvars*2; loop++) {
	    if(unpvalArray[loop] == 1) {
	        fprintf(stderr, " %d:%d ", loop, unpvalArray[loop]);
	    }
	}
	fprintf(stderr,"\n");
    }

    /* Copy the unpvalarray to the pvalarray to read for the next iteration */
    for( loop = 0; loop < numbddvars*2; loop++) {
	pvalArray[loop] = unpvalArray[loop];
    }

     /* mddTauArray is now an array of arrays. Each array of the 
     * mddTauArray contains the tau0 and tau1 bdds for all the 
     * output bdd vars decided by that tau. Here tau0 bdd represents
     * the value of tau when the var is 0 and tau1 the bdd when the 
     * variable is 1. */
    /* iterate over the mddTauArray for maxIter and check for each
     * iteration, whether the invariant is satisfied for the
     * given assignment of pvalArray */

    iterations = 0;
    done =0;
    do {

    arrayForEachItem( array_t *, mddTauArray, i, tauArray) {
	/* bddIdArr contains all the primed output bddid vars which are
	 * decided by the taus in the tauArray */
	bddIdArr = array_fetch(array_t *,  allpbddvars, i);
	/* unpbddIdArr contains the corresponding unprimed vars */
	unpbddIdArr = array_fetch(array_t *,  allunpbddvars, i);

	if (debug) {
	    /*fprintf(stderr,"\ngot the primed bddids in iter \n");
	    print_bdd_list_id(bddIdArr);
	    fprintf(stderr,"\ngot the unprimed bddids in iter \n");
	    print_bdd_list_id(unpbddIdArr);*/
	}

	/* evaluate each of the taubdds in the tauArray for the 
	 * given assignment of variables and find out if the 
	 * value assigned is a valid one. */
	k = 0;
	arrayForEachItem(bdd_t *, bddIdArr, j, bddid) {
	    taubdd = array_fetch(bdd_t *, tauArray, k++);
	    /* eval0 = value of taubdd when bddid = 0 */
            eval0 = bdd_bdd_eval_bdd( taubdd, pvalArray);
	    taubdd = array_fetch(bdd_t *, tauArray, k++);
	    /* eval1 = value of taubdd when bddid = 1 */
	    eval1 =  bdd_bdd_eval_bdd( taubdd, pvalArray);

	    /*pindex is the index in the pvalArray of the variable 
	     * bddid being evaluated. There is a global index
	     * given to each variable, on creation, and this
	     * does not change with the variable ordering of 
	     * the bdds, and is a const. for the life of  that
	     * variable.*/
	    pnode = bdd_extract_node_as_is(bddid);
	    pindex =  bdd_node_read_index(pnode);
	    /*pindex = (unsigned int) bdd_top_var_id(bddid);*/
	    /* unpbddid is the corresponding unprimed bddid of the
	     * bddid*/
	    unpbddid = array_fetch(bdd_t *, unpbddIdArr, j);
	    unpnode = bdd_extract_node_as_is(unpbddid);
	    unpindex =  bdd_node_read_index(unpnode);
	    /*unpindex = (unsigned int) bdd_top_var_id(unpbddid);*/

	    if (debug) {
		if(!done) {
		fprintf(stderr," unpindex = %d  pindex = %d ; ", unpindex,
			pindex);
		}
	    }

	    if((eval0 == -1) || (eval1 == -1)) {
		fprintf(stderr, "ERROR evaluating the tau0 and tau1 bdds for %d iteration\n",
			j);
	    } else if ((eval0 == 0) && (eval1 == 0)) {
		fprintf(stderr, "ERROR:both values tau0 and tau1 are zeroes. \n");
	    } else if ((eval0 == 1) && (eval1 == 1))  {
		/* both 0 and 1 are valid assignments to bddid. 
		 * So choose a random value for the next state
		 * assignment */
		pvalArray[pindex] = ((Cudd_Random() & 0x1) ? 1 : 0);
		/* also set the unprimed bddid to read for the next iter. */
		unpvalArray[unpindex] =  pvalArray[pindex];
	    } else if (eval0 == 1) {
		pvalArray[pindex] = 0;
		unpvalArray[unpindex] = 0;
	    } else if (eval1 == 1) {
		pvalArray[pindex] = 1;
		unpvalArray[unpindex] = 1;
	    }
	}
    }

    done =1;
    /* check whether the assignment of unpvalArray satifies the 
     * invariants. We check the unprimed image against the
     * invariant.*/
    arrayForEachItem(array_t *,invmddArray, i, invArray) {
	arrayForEachItem(bdd_t *, invArray, j, inv) {
	    valid = bdd_bdd_eval_bdd(inv, unpvalArray);
	    if( valid == 0) {
		fprintf(stderr, " Invariant %d failed for iteration %d \n"
			, i, iterations);
	    } else if (valid == -1) {
		fprintf(stderr, " ERROR: evaluating invariant %d \n", i);
	    }
	}
    }

    if(debug) {
	fprintf(stderr, "\nvalues of the unprimed vals array for iteration %d\n",
		iterations);
	for( loop = 0; loop < numbddvars*2; loop++) {
	    if(unpvalArray[loop] == 1) {
	        fprintf(stderr, " %d:%d ", loop, unpvalArray[loop]);
	    }
	}
	fprintf(stderr,"\n");

	fprintf(stderr, "\nvalues of the primed vals array for iteration %d\n",
		iterations);
	for( loop = 0; loop < numbddvars*2; loop++) {
	    if(pvalArray[loop] == 1) {
	        fprintf(stderr, " %d:%d ", loop, pvalArray[loop]);
	    }
	}
	fprintf(stderr,"\n");
    }
	

    /* Copy the unpvalarray to the pvalarray for the next iteration */
    for( loop = 0; loop < numbddvars*2; loop++) {
	pvalArray[loop] = unpvalArray[loop];
    }

    /*if(mode) fprintf(stderr,"iteration %d completed\n", iterations);*/

    } while( ++iterations < maxIter);

    if(mode) fprintf(stderr,"%d iterations completed\n", iterations);

    arrayForEachItem(array_t *, allpbddvars, i, bddIdArr) {
	mddFreeBddArr(bddIdArr);
    }
    array_free(allpbddvars);

    arrayForEachItem(array_t *, allunpbddvars, i, bddIdArr) {
	mddFreeBddArr(bddIdArr);
    }
    array_free(allunpbddvars);
    arrayForEachItem( array_t *, mddTauArray, i, tauArray) {
        arrayForEachItem( bdd_t *, tauArray, j, tau0) {
            bdd_free(tau0);
        }
	array_free(tauArray);
    }
    array_free(mddTauArray);
    free(pvalArray);
    free(unpvalArray);
}

/*ashwini :end */


array_t *
mdd_pick_arbitrary_minterms(mddMgr, aMdd, mddIdArr, n)
  mdd_manager	*mddMgr;
  mdd_t		*aMdd;
  array_t	*mddIdArr;
  int		n;
{
    bdd_t	*onvalBdd, *aOnvalBdd, *onsetBdd, *tmpBdd;
    array_t	*bddVarArr;
    int		i, arrSize, mddId;
    array_t	*mintermArray;

    arrSize = array_n( mddIdArr );
    onvalBdd = bdd_one( mddMgr );

    for ( i = 0 ; i < arrSize ; i++ ) {
	mddId = array_fetch( int, mddIdArr, i );
	aOnvalBdd = mddRetOnvalBdd( mddMgr, mddId );

	tmpBdd = bdd_and( onvalBdd, aOnvalBdd, 1, 1 );
	bdd_free( onvalBdd );
	bdd_free( aOnvalBdd );
	onvalBdd = tmpBdd;
    }
    onsetBdd = bdd_and( onvalBdd, aMdd, 1, 1 );
    bdd_free( onvalBdd );

    bddVarArr = mdd_ret_arr_bin_vars(mddMgr, mddIdArr);
    mintermArray = bdd_bdd_pick_arbitrary_minterms(onsetBdd, bddVarArr,
	array_n(bddVarArr), n);
    bdd_free(onsetBdd);
    mddFreeBddArr(bddVarArr);
    return(mintermArray);
}


/* Internal macro to access the mvar_type structure for each MDD variable. */
mvar_type
mdd_get_var_by_id(mddMgr, id)
  mdd_manager	*mddMgr;
  int		id;
{
    return(mddGetVarById(mddMgr, id));
}
