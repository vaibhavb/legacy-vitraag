/**CFile***********************************************************************

  FileName    [slSim.c]

  PackageName [sl]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [ Luca De Alfaro and  Ashwini Ananthateerta]

  Copyright   [Copyright (c) 1994-2002 The Regents of the Univ. of California.
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

  Revision    [$Id: slSim.c,v 1.49 2003/05/19 20:36:56 ashwini Exp $]

******************************************************************************/

#include <time.h>
#include "slInt.h" 
#include "../inv/inv.h" 


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

static void getRandomCubeInMdd(mdd_t *mdd, mdd_t **resultCube, unsigned int seed);
static int ReadDomainSize(Var_Variable_t * var);
static mdd_t * BuildInvMdd( Sym_Info_t * symInfo, Atm_Expr_t* expr);
static mdd_t * BuildIdentityMdd(mdd_manager *mgr, array_t * unprimedVars);
static array_t * tauarrayDepSort(Sl_GSTEntry_t *fsmEntry );
static TauStruct * newTauStruct();
static TauStruct * findControllingtauStruct(array_t * taustructarray, int varid);
static int isElementFound( int element, array_t *rhs);
static void depthFirstSearch(TauStruct * parent, array_t * resultArray);
static void deleteTauStruct(TauStruct *taustruct); 




/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


int SlCreateCommandSim(
			 Tcl_Interp *interp,
			 Main_Manager_t *manager)
{

        Tcl_CreateCommand(interp, "sl_ran_invchk", SlRandomcubeInvChkCmd, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL); 
        Tcl_CreateCommand(interp, "sl_inv_buildMdd", SlInvBuildMdd, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL); 
   
        Tcl_CreateCommand(interp, "sl_tau_plus_and_tau_minus", 
		SlTauPlusMinus, (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL); 
   
        Tcl_CreateCommand(interp, "sl_mdd_substitute", SlMddSubstitute,
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL); 
   
        Tcl_CreateCommand(interp, "sl_better_upre_comb", SlBetterUPreComb,
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL); 
   
        Tcl_CreateCommand(interp, "sl_better_upre", SlBetterUPre,
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL); 
   
        Tcl_CreateCommand(interp, "sl_mdd_equal", SlMddEqual, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL); 

        Tcl_CreateCommand(interp, "sl_atom_simulate", SlAtomSimulateCmd, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL); 

        Tcl_CreateCommand(interp, "sl_sort_and_conjoin_tau", 
		SlSortAndConjoinTau, (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL); 
  return TCL_OK; 
}

/**Function********************************************************************

  Synopsis           [Function implementing the function sl_mdd_substitute]


  Description        [wrapper for the mdd_substitute function]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

int SlMddSubstitute(
            ClientData clientdata,
            Tcl_Interp *interp,
            int argc,
            char ** argv)
{
    char usage[] = "Usage: sl_mdd_substitute <MDD_in> <MDD_out> <var_num> <old_vars...> <new_vars...>";
    char * bdd1name, *bdd2name, *varName;
    Sl_GSTEntry_t *bdd1Entry, *outbddEntry;
    mdd_t * mdd1, *mdd2;
    array_t *old_vars, *new_vars, *old_prime_vars, *new_prime_vars;
    array_t *all_old_vars, *all_new_vars;
    int var_num, varid, i;
    mdd_manager *mgr = SlGetMddManager ();

  util_getopt_reset();
  util_optind++;

 if (argc < 5) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  bdd1name = argv[util_optind];
   if (!st_lookup(Sl_GST, bdd1name, (char **) &bdd1Entry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd1name);
    return TCL_OK;
  }
  if (bdd1Entry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd1name);
    return TCL_OK;
  }

  util_optind++;
  bdd2name = argv[util_optind];
  if (st_is_member(Sl_GST, bdd2name)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", bdd2name);
    return TCL_OK;
  }

  util_optind++;
  var_num = atoi(argv[util_optind]);

  util_optind++;
  old_vars = array_alloc(int,0);
  for( i = 0; i < var_num; i++) {
      varName = argv[util_optind++];
      varid = SlObtainUnprimedBddId(varName);
      if(varid == -1) {
	   Main_MochaErrorPrint("Error: variable %s invalid\n",
		   argv[--util_optind]);
	   array_free(old_vars);
	   return TCL_OK;
      }
      array_insert_last(int, old_vars, varid);
  }

  new_vars = array_alloc(int,0);
  for( i = 0; i < var_num; i++) {
      varName = argv[util_optind++];
      varid = SlObtainUnprimedBddId(varName);
      if(varid == -1) {
	   Main_MochaErrorPrint("Error: variable %s invalid\n",
		   argv[--util_optind]);
	   array_free(old_vars);
	   array_free(new_vars);
	   return TCL_OK;
      }
      array_insert_last(int, new_vars, varid);
  }

  new_prime_vars = SlPrimeVars(new_vars);

  /*all_old_vars = array_join(old_vars, old_prime_vars);
  array_free(old_vars);
  array_free(old_prime_vars);

  all_new_vars = array_join(new_vars, new_prime_vars);
  array_free(new_vars);
  array_free(new_prime_vars);
  */

  /*SlPrintVarNames(old_vars);
  Main_MochaPrint("These are the old vars\n");
  SlPrintVarNames(new_vars);*/
  mdd1 =  SlConjoin (bdd1Entry->obj.bddArray);
  mdd2 = mdd_substitute(mgr, mdd1, old_vars, new_vars);
  array_free(old_vars);
  array_free(new_vars);

  outbddEntry = ALLOC(Sl_GSTEntry_t, 1);
  outbddEntry->name = util_strsav(bdd2name);
  outbddEntry -> kind = Sl_BDD_c;
  outbddEntry -> obj.bddArray =  SlSingleMddArray(mdd2);

  st_insert(Sl_GST, outbddEntry -> name, (char *) outbddEntry);

  return TCL_OK;

}


/**Function********************************************************************

  Synopsis           [Function implementing the function sl_mdd_equal]


  Description        [wrapper for the mdd_equal function]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

int SlMddEqual(
            ClientData clientdata,
            Tcl_Interp *interp,
            int argc,
            char ** argv)
{
    char usage[] = "Usage: sl_mdd_equal <BDD> <BDD> ";
    char * bdd1name, *bdd2name;
    Sl_GSTEntry_t *bdd1Entry, *bdd2Entry;
    mdd_t * mdd1, *mdd2;

  util_getopt_reset();
  util_optind++;

 if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  bdd1name = argv[util_optind];
   if (!st_lookup(Sl_GST, bdd1name, (char **) &bdd1Entry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd1name);
    return TCL_OK;
  }
  if (bdd1Entry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd1name);
    return TCL_OK;
  }

  util_optind++;
  bdd2name = argv[util_optind];
   if (!st_lookup(Sl_GST, bdd2name, (char **) &bdd2Entry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd2name);
    return TCL_OK;
  }
  if (bdd2Entry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd2name);
    return TCL_OK;
  }

  mdd1 =  SlConjoin (bdd1Entry->obj.bddArray);
  mdd2 =  SlConjoin (bdd2Entry->obj.bddArray);

  if( mdd_equal(mdd1, mdd2) )
      Main_MochaPrint("The 2 mdds are equal\n");
  else
      Main_MochaPrint("The 2 mdds are not equal\n");

  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [Function implementing the function sl_atom_simulate]


  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]


******************************************************************************/

int SlAtomSimulateCmd(
	      ClientData clientdata,
	      Tcl_Interp *interp,
	      int argc,
	      char ** argv)
{

  char usage[] = "Usage: sl_atom_simulate <FSM>  <seed> <max_iterations> <s|v> <BDD1> <BDD2>...";
  char          *fsmName, *bddName;
  Sl_GSTEntry_t *inFsm, *bddEntry;
  int  mode, i, id, outcome;
  mdd_t * minterm, *init_mdd, *tau, *newMinterm;
  int maxIter, loop;
  array_t * sortedTauArray, *invariantBddArray, *conjArray2;
  int      niterations = 0; 
  array_t *allvars, *emptyArray, *conjArray, *invariant; 
  array_t *allPvars, *allsupportvars;
  mdd_t   *newImage, *unpImage, *inv, *mintermAndTau; 
  long seed;
  array_t * supp2, *supp1, *suppU, *temp, *mddvars;
  int doSanityTest = 0;
  mdd_manager *mgr = SlGetMddManager ();

  util_getopt_reset();
  util_optind++;

 if (argc < 6) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &inFsm)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (inFsm -> kind != Sl_FSM_c) {
      Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
      return TCL_OK;
  }

  util_optind++;
  seed = atol (argv[util_optind]);

  util_optind++;
  maxIter = atoi (argv[util_optind]);

  util_optind++;

  if (strcmp (argv[util_optind], "s") == 0) {
    mode = 0;
  } else if (strcmp (argv[util_optind], "v") == 0) {
    mode = 1;
  } else {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  util_optind++;
  /* allvars = controlled(output) + external(input) */
  allvars = array_join(inFsm -> obj.fsm.contrvarL, inFsm -> obj.fsm.extvarL);
  /* invariantBddArray is an array of all the invariant bdd arrays
   * to be to be verified */
  invariantBddArray = array_alloc(array_t *, 0);
  while(argc - util_optind != 0) {
      bddName = argv[util_optind++];
       if (!st_lookup(Sl_GST, bddName, (char **) &bddEntry)) {
        Main_MochaErrorPrint("Error: Object %s does not exist.\n", 
		bddName);
	array_free(allvars);
	array_free(invariantBddArray);
        return TCL_OK;
      }
      if (bddEntry -> kind != Sl_BDD_c) {
        Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bddName);
	array_free(allvars);
	array_free(invariantBddArray);
        return TCL_OK;
      }
      array_insert_last(array_t *, invariantBddArray, 
	      bddEntry->obj.bddArray);
      arrayForEachItem(mdd_t*, bddEntry->obj.bddArray, i, inv) {
	  Main_MochaPrint("Statistics of invariant %s are \n", bddName);
	  SlPrintBddStats(inv, allvars);
      }
  }

  /* prepares the initial condition*/
  init_mdd = SlGetInit (inFsm); 
      if(mode) {
          Main_MochaPrint("init_mdd stats are \n");
          SlPrintBddStats(init_mdd, allvars);
      }

  /* set the random seed to pick a random cube */
  mdd_srandom(seed);

  /* I want the cube to have no more vars than the mdd. */ 
  mddvars = mdd_get_support (mgr, init_mdd); 
  minterm = mdd_pick_one_minterm(mgr, init_mdd, mddvars, array_n(mddvars)); 
  array_free (mddvars); 

  if (minterm == NULL) {
      Main_MochaPrint("ERROR:Minterm picked was NULL\n");
      mdd_free(init_mdd);
      array_free(allvars);
      array_free(invariantBddArray);
      return TCL_OK;
  }
  if (doSanityTest) {
      /*Add sanity test for support */
      supp1 = mdd_get_support(mgr, minterm);
      supp2 = mdd_get_support(mgr, init_mdd);
      temp = SlArrayDifference( supp1, supp2);
      if(array_n(temp) != 0) {
	  Main_MochaPrint ("ERROR: in support of init minterm \n");
          Main_MochaPrint ("Extra vars in minterm are \n");
          SlPrintVarNames(temp);
          Main_MochaPrint ("\nsupport of init_mdd is \n");
          SlPrintVarNames(supp2);
          Main_MochaPrint ("\nsupport of minterm is \n");
          SlPrintVarNames(supp1);
          array_free(supp1);
          array_free(supp2);
          array_free(temp);
	  mdd_free(init_mdd);
          array_free(allvars);
          array_free(invariantBddArray);
          return TCL_OK;
      } else {
	  Main_MochaPrint ("CORRECT: support of minterm is in support of init_mdd\n");
      }

      array_free(supp1);
      array_free(supp2);
      array_free(temp);

      /*Add test that cube picked is correct i.e. minterm => init_mdd */
      conjArray = SlSingleMddArray (minterm);
      conjArray2 = SlSingleMddArray (init_mdd);
      outcome = SlImplicationCheck(conjArray, conjArray2);
      if(!outcome) {
	  Main_MochaPrint ("ERROR: in implication check minterm => init_mdd in mdd_pick_one_minterm() alg\n");
	  array_free(conjArray);
	  array_free(conjArray2);
	  mdd_free(init_mdd);
	  array_free(allvars);
	  array_free(invariantBddArray);
          return TCL_OK;
      } else { 
	  Main_MochaPrint ("The implication holds.\n"); 
      }
      Main_MochaPrint ("CORRECT: the init cube statistics are: \n");
      SlPrintBddStats(minterm, allvars);
      array_free(conjArray);
      array_free(conjArray2);
  }

  mdd_free (init_mdd); 

  /* checks the initial condition. */ 
  conjArray = SlSingleMddArray (minterm); 
  arrayForEachItem(array_t *, invariantBddArray, i, invariant) {
      outcome = SlImplicationCheck (conjArray, invariant);
      if (!outcome) {
	  Main_MochaPrint ("*Invariant %d failed for initial condition\n",
		  i);
	  /*NO longer check this failed invariant. So remove
	   * it from the invariantBddArray? TODO.*/
      }
  }
  array_free (conjArray); 

  allPvars = SlPrimeVars(allvars);
  allsupportvars = array_join(allPvars, allvars);
  array_free(allPvars);

  /* sort the transition relation array of the module according to the
   * await-dependency relation */
  sortedTauArray = tauarrayDepSort(inFsm);
  emptyArray = array_alloc (int, 0);

  /* The initial condition minterm is in, well, 'minterm' */ 

  do {
    niterations++;

    /* Now compute the minterm for the next state using the new mdd 
     * mdd_pick_one_minterm() function.  */

    arrayForEachItem(mdd_t *, sortedTauArray, i, tau) {
	mintermAndTau = mdd_and(minterm, tau, 1,1);
	mdd_free(minterm);

	/* debug */ 
	/*Main_MochaPrint ("======================================\n"); 
	Main_MochaPrint ("mintermAndTau %d and tau has, on V u V': \n", i); 
	SlPrintBddStats(mintermAndTau, allsupportvars);
	SlMddPrintCubes(mintermAndTau, 0); 
	Main_MochaPrint ("======================================\n"); */

	/* now we pick a new cube out of the conjuction with tau */ 
	mddvars = mdd_get_support (mgr, mintermAndTau); 
        newMinterm = mdd_pick_one_minterm(mgr, mintermAndTau, 
					  mddvars, array_n(mddvars));
	array_free (mddvars); 

        if (newMinterm == NULL) {
            Main_MochaPrint("ERROR:Minterm picked was NULL\n");
	    mdd_free(mintermAndTau);
	    array_free(allsupportvars);
            array_free (emptyArray); 
            array_free(invariantBddArray);
            array_free (allvars); 
            array_free (sortedTauArray); 
            return TCL_OK;
        }
	if (doSanityTest) {
	    /*sanity test 1 */
	    supp1 = mdd_get_support(mgr, mintermAndTau); 
	    supp2 = mdd_get_support(mgr, newMinterm); 
	    temp = SlArrayDifference(supp2, supp1);

	    if( array_n(temp) != 0) {
	        Main_MochaPrint("ERROR:newMinterm support not correct\n");
	        Main_MochaPrint("Extra vars in support are \n");
	        SlPrintVarNames(temp);
	        Main_MochaPrint("support of newMinterm for %d is \n", i);
	        SlPrintVarNames(supp2); 
	        Main_MochaPrint("support of old minterm U tau is \n");
	        SlPrintVarNames(supp1);
	    } else { 
	        Main_MochaPrint ("newMinterm %d has support in mintermAndTau \n", i);
	    } 
	    array_free (supp1); 
	    array_free (supp2); 
	    array_free (temp); 

	    /*Sanity test 2 */
	    conjArray = SlSingleMddArray (newMinterm);
	    conjArray2 = SlSingleMddArray (mintermAndTau);
            outcome = SlImplicationCheck(conjArray, conjArray2);
	    array_free(conjArray);
	    array_free(conjArray2);
            if(!outcome) {
	        Main_MochaPrint ("ERROR: implication newMinterm -> mintermAndTau fails for %d \n", i);
	        mdd_free(mintermAndTau);
	        mdd_free(newMinterm);
	        array_free(allsupportvars);
                array_free (emptyArray); 
                array_free(invariantBddArray);
                array_free (allvars); 
                array_free (sortedTauArray); 
                return TCL_OK;
            } else { 
	        Main_MochaPrint ("Implication newMinterm -> mintermAndTau holds for %d \n", i);
	    }

	    /*sanity test end */
	}
	minterm = newMinterm;
	mdd_free (mintermAndTau); 
	/*if (mode) {
	    Main_MochaPrint ("++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	    Main_MochaPrint ("Stats of newMinterm %d is \n", i);
            SlPrintBddStats (newMinterm, allsupportvars);
	    Main_MochaPrint ("++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	    SlMddPrintCubes(newMinterm, 0); 
	}*/

    }
    if (mode) {
        Main_MochaPrint ("TEST4:after computing the minterm ()\n");
    }

    /* quantify out the unprimed vars in the minterm */
    newImage = mdd_smooth(mgr, minterm, allvars);
    mdd_free(minterm);

    minterm = SlUnprimeMdd(newImage);
    mdd_free(newImage);

    if (mode) { 
      Main_MochaErrorPrint ("Random minterm stats are\n");
      SlPrintBddStats (minterm, allvars);
      /*Main_MochaErrorPrint ("Cubes at iteration %d:\n", niterations); 
      SlMddPrintCubes (cube, 0); */
    }
    /* now that the new image is a random minterm we can test it against 
     * the invariant */
    conjArray = SlSingleMddArray (minterm); 
    arrayForEachItem(array_t *, invariantBddArray, i, invariant) {
        outcome = SlImplicationCheck (conjArray, invariant);
        if (!outcome) {
  	    Main_MochaPrint ("*Invariant %d failed at iteration %d.\n",
  	 	    i, niterations);
	}
    }
    array_free (conjArray); 

    if (mode) { 
      Main_MochaPrint ("*Iteration %7d\n ", niterations);
    }
  } while(niterations < maxIter ); /* end of the reachability iteration. */ 
  
  /* I print the n. of iterations to converge, not the n. of iterations to 
     detect convergence (which is bigger by 1). */ 
  Main_MochaPrint ("*Computation terminated at %d iterations.\n", niterations - 1);

  mdd_free   (minterm); 
  array_free (emptyArray); 
  array_free(invariantBddArray);
  array_free (allvars); 
  array_free (sortedTauArray); 
  array_free(allsupportvars);

  /*Ashwini: re-enable dynamic bdd reordering.*/
   /*bdd_dynamic_reordering(mgr, BDD_REORDER_SIFT,
    BDD_REORDER_VERBOSITY_DEFAULT);
  bdd_reorder(mgr);*/

  return TCL_OK;
}



/**Function********************************************************************

  Synopsis           [Function implementing the function sl_ran_invchk]


  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]


******************************************************************************/

int SlRandomcubeInvChkCmd (
	      ClientData clientdata,
	      Tcl_Interp *interp,
	      int argc,
	      char ** argv)
{

  char usage[] = "Usage: sl_ran_invchk <FSM> <max_iterations> <seed> <s|v> <BDD1> <BDD2>...";
  char          *fsmName, *bddName;
  Sl_GSTEntry_t *inFsm, *bddEntry;
  int            mode;
  int task = 1;
  int maxIter;
  long seed;
  array_t * invariantBddArray;

  util_getopt_reset();
  util_optind++;

 if (argc < 6) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &inFsm)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (inFsm -> kind != Sl_FSM_c) {
      Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
      return TCL_OK;
  }

  util_optind++;


  maxIter = atoi (argv[util_optind]);

  util_optind++;

  seed = atol (argv[util_optind]);
  util_optind++;

  if (strcmp (argv[util_optind], "s") == 0) {
    mode = 0;
  } else if (strcmp (argv[util_optind], "v") == 0) {
    mode = 1;
  } else {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  util_optind++;
  /* invariantBddArray is an array of all the invariant bdd arrays
   * to be to be verified */
  invariantBddArray = array_alloc(array_t *, 0);
  while(argc - util_optind != 0) {
      bddName = argv[util_optind++];
       if (!st_lookup(Sl_GST, bddName, (char **) &bddEntry)) {
        Main_MochaErrorPrint("Error: Object %s does not exist.\n", 
		bddName);
	array_free(invariantBddArray);
        return TCL_OK;
      }
      if (bddEntry -> kind != Sl_BDD_c) {
        Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bddName);
	array_free(invariantBddArray);
        return TCL_OK;
      }
      array_insert_last(array_t *, invariantBddArray, 
	      bddEntry->obj.bddArray);
  }


  SlRandomcubeInvChk (inFsm, task,invariantBddArray, maxIter, mode,
	  seed);
  array_free(invariantBddArray);

  return TCL_OK;
}



/**Function********************************************************************

  Synopsis           [This function picks one cube at random from all the 
                      states that satisfy the transition invariant at a given
		      image and checks whether the given cube satisfies
		      the input invariant (the invariant checked depends on the task 
		      input variable). If the given cube satisfies the invariant
		      all reachable states from that cube are computed to
		      get the next image from which a random cube
		      is again picked. The function terminates when the invariant
		      at an image is not satisfied, or n_steps iterations are
		      done.]
                      
		      task=1: checks that the reachable states imply a formula
		      task=2: checks that a given transition invariant holds. 
		      Inputs: 
		      task:      see above. 
		      inFsm:     the FSM that has to be thus checked. 
		      invariant: the (transition?) invariant to be checked. 
		      n_steps:   number of simulation steps to be done. 
		      mode:  0 = silent (only max data), 1 = verbose ]


  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]


******************************************************************************/

void SlRandomcubeInvChk (
	      Sl_GSTEntry_t *inFsm, 
	      int            task, 
	      array_t       *invariantArray,
	      int            n_steps,
	      int            mode, 
	      long           seed)
{
  int      niterations = 0; 
  array_t *allvars;         
  array_t *emptyArray, *conjArray, *mddvars; 
  array_t *temp_array, *temp2_array, *invariant, *vars_in_invariantArray; 
  array_t * to_quantify_out0, *to_quantify_out1; 
  array_t *history_free, *vars_in_invariant, *next_state_vars;
  mdd_t   *newImage, *unpImage, *conj; 
  mdd_t   *R, *init_mdd;
  mdd_manager *mgr;
  int     outcome, i, j, id, numallvars; 
  array_t * conjArray2, *supp1, *supp2, *temp;
  int doSanityTest = 0;

  emptyArray = array_alloc (int, 0);
  mgr = SlGetMddManager ();

  /* Set the seed for the random number generation */
  mdd_srandom(seed);
  /* allvars = controlled(output) + external(input) */
  allvars = array_join(inFsm -> obj.fsm.contrvarL, inFsm -> obj.fsm.extvarL);

  /* the history-free vars of the fsm. These are the variables
   * that only appear primed in the transition rn. These vars are
   * awaited and not read in the module.*/
  history_free = SlHistoryFree (allvars, inFsm -> obj.fsm.updateBdd);


    /* Figures out which variables are in the array of invariants. 
       These are the UNPRIMED variables in the invariants. */ 
    /* All the variables in the invariantArray */ 
    vars_in_invariantArray = array_alloc (int, 0); 
    arrayForEachItem(array_t*, invariantArray, j, invariant) { 
        vars_in_invariant = array_alloc (int, 0); 
        arrayForEachItem(mdd_t*, invariant, i, conj) { 
          temp_array = mdd_get_support (mgr, conj); 
          /*Main_MochaPrint ("Support size: %d\n", array_n (temp_array)); */
          array_append (vars_in_invariant, temp_array); 
          array_free (temp_array); 
        }
    array_append(vars_in_invariantArray, vars_in_invariant);
    array_free(vars_in_invariant);
    }

    /* Eliminates duplicates */ 
    array_sort (vars_in_invariantArray, SlCompareInt);
    array_uniq (vars_in_invariantArray, SlCompareInt, 0);

    /* Only the unprimed vars in the invariant */ 
    temp_array = array_alloc (int, 0); 
    arrayForEachItem (int, vars_in_invariantArray, i, id) {
      if (SlReturnPrimedId(id) != id) {
	/* variable is unprimed */ 
	array_insert_last (int, temp_array, id); 
      }
    }
    array_free (vars_in_invariantArray); 
    vars_in_invariantArray = temp_array; 

    /*if(mode) {
	Main_MochaPrint ("vars in invariantArray are \n");
	SlPrintVarNames (vars_in_invariantArray);
    }*/
    /* Eliminates duplicates  -already done.*/ 
    /*array_sort (vars_in_invariant, SlCompareInt);
    array_uniq (vars_in_invariant, SlCompareInt, 0);*/

    /* Invariant, regular or transition.  
       All the history-free variables must eventually be 
       quantified out, but we:
       - first quantify out the history-free ones that do not appear in the invariant, 
         in to_quantify_out0, 
       - then we unprime the formula for the next states,
       - then we check the invariant, 
       - and last we quantify out the history-free variables that do appear in the invariant, 
         which are in to_quantify_out1 (and are unprimed) */ 
    /* round 0 */
    temp_array       = SlArrayDifference (history_free, vars_in_invariantArray);
    to_quantify_out0 = SlPrimeVars (temp_array); 
    array_append     (to_quantify_out0, allvars); 
    /* Now puts in next_state_vars the next-state vars that won't be quantified out */ 
    temp2_array      = SlArrayDifference (allvars, temp_array); 
    next_state_vars  = SlPrimeVars (temp2_array); 
    array_free       (temp2_array); 
    array_free       (temp_array); 
    /* round 1 */ 
    to_quantify_out1 = SlArrayIntersection (history_free, vars_in_invariantArray);
    array_free (history_free);
    array_free (vars_in_invariantArray);

    /*if(mode) {
	Main_MochaPrint ("vars to quantify out 1st\n");
	SlPrintVarNames (to_quantify_out0);
	Main_MochaPrint ("vars to quantify out later\n");
	SlPrintVarNames (to_quantify_out1);
    }*/

    /*Ashwini: disable dynamic bdd reordering for more efficiency 
     * during simulations*/
    /*bdd_dynamic_reordering(mgr, BDD_REORDER_NONE,
	    BDD_REORDER_VERBOSITY_DEFAULT);
    bdd_reorder(mgr);*/


  /* prepares the initial condition, and puts a cube of it in R. */ 
  init_mdd = SlGetInit (inFsm); 
  /*R = SlGetRandomCubeInMdd (init_mdd, seed); */

  numallvars = array_n(allvars);
  /*allPvars = SlPrimeVars(allvars);
  allsupportvars = array_join(allPvars, allvars);
  array_free(allPvars);
  numAllsupportvars = array_n(allsupportvars);*/
  mddvars = mdd_get_support(mgr, init_mdd);
  R = mdd_pick_one_minterm(mgr, init_mdd, mddvars, array_n(mddvars));
  array_free(mddvars);
  if (doSanityTest) {
      /*Add sanity test for support */
      supp1 = mdd_get_support(mgr, R);
      supp2 = mdd_get_support(mgr, init_mdd);
      temp = SlArrayDifference( supp1, supp2);
      if(array_n(temp) != 0) {
	  Main_MochaPrint ("ERROR: in support of init minterm \n");
          Main_MochaPrint ("Extra vars in minterm are \n");
          SlPrintVarNames(temp);
          Main_MochaPrint ("\nsupport of init_mdd is \n");
          SlPrintVarNames(supp2);
          Main_MochaPrint ("\nsupport of minterm is \n");
          SlPrintVarNames(supp1);
          array_free(supp1);
          array_free(supp2);
          array_free(temp);
	  mdd_free(init_mdd);
          array_free(allvars);
          return ;
      } else {
	  Main_MochaPrint ("CORRECT: support of minterm is in support of init_mdd\n");
      }

      array_free(supp1);
      array_free(supp2);
      array_free(temp);

      /*Add test that cube picked is correct i.e. R => init_mdd */
      conjArray = SlSingleMddArray (R);
      conjArray2 = SlSingleMddArray (init_mdd);
      outcome = SlImplicationCheck(conjArray, conjArray2);
      if(!outcome) {
	  Main_MochaPrint ("ERROR: in implication check R => init_mdd in mdd_pick_one_minterm() alg\n");
	  array_free(conjArray);
	  array_free(conjArray2);
	  mdd_free(init_mdd);
	  array_free(allvars);
          return ;
      } else { 
	  Main_MochaPrint ("The implication holds.\n"); 
      }
      Main_MochaPrint ("CORRECT: the init cube statistics are: \n");
      SlPrintBddStats(R, allvars);
      array_free(conjArray);
      array_free(conjArray2);
  }

  mdd_free (init_mdd); 

  /* checks the initial condition. */ 
  if (task == 1 ) { 
    conjArray = SlSingleMddArray (R); 
    arrayForEachItem(array_t *, invariantArray, i, invariant) {
        outcome = SlImplicationCheck (conjArray, invariant);
	if (!outcome) { 
	    Main_MochaPrint ("*Invariant %d failed for initial condition\n",
		     i);
	}
    }
    array_free (conjArray); 
  } else if (task == 2) { 
    /* transition invariant */ 
    conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
    array_insert_last (mdd_t*, conjArray, R); 
    arrayForEachItem(array_t *, invariantArray, i, invariant) {
        outcome = SlImplicationCheck (conjArray, invariant);
	if (!outcome) { 
	    Main_MochaPrint ("*Invariant %d failed for initial condition\n",
		     i);
	}
    }
    array_free (conjArray); 
  }     

  /* now loops, finding the next-states of a random cube from the given set 
   * of states  and then checking if the next-states satisfy the invariance.*/ 
  do {
    niterations++;
    /* now we compute the next image. using this random_cube*/ 
    conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
    array_insert_last (mdd_t *, conjArray, R); 
    newImage  = Img_MultiwayLinearAndSmooth(mgr, conjArray, to_quantify_out0, next_state_vars); 
    array_free  (conjArray); 
    mdd_free    (R); 
    unpImage  = SlUnprimeMdd (newImage); 
    mdd_free (newImage);
    /* Puts into R a random cube from the image. */ 
    /*R = SlGetRandomCubeInMdd (unpImage, seed); */

    mddvars = mdd_get_support(mgr, unpImage);
    R = mdd_pick_one_minterm(mgr, unpImage, mddvars, array_n(mddvars));
    array_free(mddvars);
    if (doSanityTest) {
      /* Prints the stats of before and after */ 
      Main_MochaPrint ("============BEFORE===================\n"); 
      SlPrintBddStats(unpImage, allvars);
      SlPrintBddStats(R, allvars);
      Main_MochaPrint ("============AFTER====================\n"); 
      /*Add sanity test for support */
      supp1 = mdd_get_support(mgr, R);
      supp2 = mdd_get_support(mgr, unpImage);
      temp = SlArrayDifference( supp1, supp2);
      if(array_n(temp) != 0) {
	  Main_MochaPrint ("ERROR: in support of init minterm \n");
          Main_MochaPrint ("Extra vars in minterm are \n");
          SlPrintVarNames(temp);
          Main_MochaPrint ("\nsupport of init_mdd is \n");
          SlPrintVarNames(supp2);
          Main_MochaPrint ("\nsupport of minterm is \n");
          SlPrintVarNames(supp1);
          array_free(supp1);
          array_free(supp2);
          array_free(temp);
	  mdd_free(unpImage);
          array_free(allvars);
          array_free(next_state_vars); 
	  array_free (emptyArray); 
          return ;
      } else {
	  Main_MochaPrint ("CORRECT: support of minterm is in support of init_mdd\n");
      }

      array_free(supp1);
      array_free(supp2);
      array_free(temp);

      /*Add test that cube picked is correct i.e. R => init_mdd */
      conjArray = SlSingleMddArray (R);
      conjArray2 = SlSingleMddArray (unpImage);
      outcome = SlImplicationCheck(conjArray, conjArray2);
      if(!outcome) {
	  Main_MochaPrint ("ERROR: in implication check R => init_mdd in mdd_pick_one_minterm() alg\n");
	  array_free(conjArray);
	  array_free(conjArray2);
	  array_free(allvars);
	  mdd_free(unpImage);
          array_free(next_state_vars); 
	  array_free (emptyArray); 
          return ;
      } else { 
	  Main_MochaPrint ("The implication holds.\n"); 
      }
      Main_MochaPrint ("CORRECT: the init cube statistics are: \n");
      SlPrintBddStats(R, allvars);
      array_free(conjArray);
      array_free(conjArray2);
  }

    if (mode) { 
        Main_MochaPrint ("============BEFORE===================\n"); 
        SlPrintBddStats(unpImage, allvars);
        SlPrintBddStats(R, allvars);
        Main_MochaPrint ("============AFTER===================\n"); 
    }
    mdd_free (unpImage); 
    /* now that the new image is random_cube we can test it against the invariant */
    if (task == 1) { 
      conjArray = SlSingleMddArray (R); 
      arrayForEachItem(array_t *, invariantArray, i, invariant) {
        outcome = SlImplicationCheck (conjArray, invariant);
	if (!outcome) { 
	    Main_MochaPrint ("*Invariant %d failed for iteration %d\n",
		     i, niterations );
	}
      }
      array_free (conjArray); 
    } else if (task == 2) { 
      conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
      array_insert_last (mdd_t*, conjArray, R); 
      arrayForEachItem(array_t *, invariantArray, i, invariant) {
        outcome = SlImplicationCheck (conjArray, invariant);
	if (!outcome) { 
	    Main_MochaPrint ("*Invariant %d failed for iteration %d\n",
		     i, niterations);
	}
      }
      array_free (conjArray); 
    } 

    /*quantify away the extra variables */
    conjArray = SlSingleMddArray (R);
    conj = Img_MultiwayLinearAndSmooth(mgr, conjArray, to_quantify_out1, emptyArray);
    array_free(conjArray);
    mdd_free(R);
    R = conj;
    /*if (mode) { 
        Main_MochaPrint ("final minterm picked at iteration %d:\n", niterations); 
        SlPrintBddStats(R, allvars);
    }*/

    if (mode) { 
      Main_MochaPrint ("*Iteration %7d\n ", niterations);
    }
  } while(niterations < n_steps); /* end of the reachability iteration. */ 
  
  Main_MochaPrint ("*Computation terminated at %d iterations.\n", niterations);

  mdd_free   (R); 
  array_free (emptyArray); 
  array_free (allvars); 
  array_free (to_quantify_out0);
  array_free (to_quantify_out1);

  /*Ashwini: re-enable dynamic bdd reordering.*/
   /*bdd_dynamic_reordering(mgr, BDD_REORDER_SIFT,
    BDD_REORDER_VERBOSITY_DEFAULT);
  bdd_reorder(mgr);*/
}


/**Function********************************************************************

  Synopsis           [implements the sl_tau_plus_and_tau_minus command.]

  Description        [The set of variables, absvars are abstracted  from the
  			transition relation of the inFsm, to get the transition
			relations tauplus and tauminus , which only considers 
			allvars\absvars.]

  SideEffects        [none]


******************************************************************************/
int SlTauPlusMinus(
	      ClientData clientdata,
	      Tcl_Interp *interp,
	      int argc,
	      char ** argv)
{
    char usage[] = "Usage: sl_tau_plus_and_tau_minus <outPlusMdd> <outMinusMdd> <FSM> <one|all> <abstract|notabstract> <s|v> <list of controlled vars of the module to be abstracted , or list of vars to be not abstracted, depending on the <abstract|notabstract> parameter >...";
    Sl_GSTEntry_t *inFsm, *outPlusBddEntry, *outMinusBddEntry; 
    array_t *absvars, *commonVars, *vars;         
    int  mode, varid, computeReachable, abstract; 
    array_t *conjArray, *emptyArray;
    array_t *allAbsVars, *pabsvars, *varsList; 
    mdd_manager *mgr;
    mdd_t * reachableStates, *tauPlusMdd, *tauMinusMdd;
    mdd_t  *object1, *notObject1, *object2;
    char * outPlusBddName, *outMinusBddName, *fsmName, *varName;

    mgr = SlGetMddManager ();
    emptyArray = array_alloc (int, 0);
    util_getopt_reset();
    util_optind++;

    if (argc < 7) {
       Main_MochaErrorPrint("Syntax error. %s\n", usage);
       return TCL_OK;
     }

     outPlusBddName = argv[util_optind];
      if (st_is_member(Sl_GST, outPlusBddName)) { 
       Main_MochaErrorPrint("Error: Object %s exists.\n", outPlusBddName);
       return TCL_OK;
     }

    util_optind++;
     outMinusBddName = argv[util_optind];
      if (st_is_member(Sl_GST, outMinusBddName)) { 
       Main_MochaErrorPrint("Error: Object %s exists.\n", outMinusBddName);
       return TCL_OK;
     }

    util_optind++;
    fsmName = argv[util_optind];
    Main_MochaPrint("TEST 0:infsm for computereach name is %s\n", fsmName);
    if (!st_lookup(Sl_GST, fsmName, (char **) &inFsm)) {
       Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
       return TCL_OK;
    }
    if (inFsm -> kind != Sl_FSM_c) {
         Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
         return TCL_OK;
    }

    util_optind++;
    /* use mdd of size 1, if reachability cannot be computed because
     * of a large state space */
    if (strcmp (argv[util_optind], "one") == 0) {
	computeReachable = 0;
    } else {
	computeReachable = 1;
    }

    util_optind++;
    if (strcmp (argv[util_optind], "abstract") == 0) {
	abstract = 1;
    } else if (strcmp (argv[util_optind], "notabstract") == 0) {
	abstract = 0;
    } else {
	 Main_MochaErrorPrint("Syntax error. %s\n", usage);
	 return TCL_OK;
    }

    util_optind++;
    if (strcmp (argv[util_optind], "s") == 0) {
	mode = 0;
    } else if (strcmp (argv[util_optind], "v") == 0) {
	mode = 1;
    } else {
	 Main_MochaErrorPrint("Syntax error. %s\n", usage);
	 return TCL_OK;
    }

    util_optind++;
    varsList = array_alloc(int, 0);
    while(argc - util_optind != 0) {
	varName = argv[util_optind];
	varid = SlObtainUnprimedBddId(varName);
	if( varid == -1) {
	    Main_MochaErrorPrint("Error: variable %s invalid\n",
		    argv[util_optind]);
	    array_free(varsList);
	    return TCL_OK;
	}
	array_insert_last(int, varsList, varid);
	util_optind++;
    }

    if(abstract) {
	/* varsList is the set of variables that are to be abstracted */
	absvars = varsList;
    } else {
	/*varsList is the set of controllable vars of FSM which are
	 * not to be abstracted in the computation of tauPlus and tauMinus
	 * Therefore absvars = contrVarL \ varsList */
	absvars = SlArrayDifference(inFsm ->obj.fsm.contrvarL, varsList);
	array_free(varsList);
    }

    /* check that the vars to be abstracted do not contain the external
     * vars of the module. */
    vars = SlArrayIntersection(inFsm ->obj.fsm.extvarL, absvars);
    if(array_n(vars) > 0) {
	 Main_MochaErrorPrint ("Error: Trying to abstract the external vars of the 
		 fsm %s \n", fsmName);
	 return TCL_OK;
    }



    /*absvars = SlArrayDifference(inFsm ->obj.fsm.contrvarL, notAbsVars); */
    /* pabsvars = absvars', the prime of the vars to be abstracted */
    pabsvars = SlPrimeVars(absvars);


    /* allAbsVars has the set of both primed and unprimed abstracted vars */
    allAbsVars = array_join(absvars, pabsvars);

    /* Compute the reachable states of the input FSM */
    if( computeReachable) {
        reachableStates =  SlComputeReach (inFsm, 3, NULL, 1);
    } else {
	reachableStates = mdd_one(mgr);
    }

    conjArray = array_dup (inFsm -> obj.fsm.updateBdd);
    /* get conjArray = (reachableStates /\ tau) */
    array_insert_last(mdd_t *, conjArray, reachableStates);

    /* tauPlusMdd = thereExists allAbsVars . (reachableStates /\ tau) */
    tauPlusMdd = Img_MultiwayLinearAndSmooth(mgr, conjArray, allAbsVars, emptyArray);
    array_free(allAbsVars);

    if(mode) {
	Main_MochaPrint("tauplus mdd size is %d\n", mdd_size(tauPlusMdd));
    }

    /* tauMinusMdd computation */
    /*if( lazy) {
	/* we need to compute the lazy controllable states. So tau_minus
	 * is taken to be the identityMdd of the un-abstracted vars
	 * of the input module
         tauMinusMdd = BuildIdentityMdd(mgr, notAbsVars);
    } else */
      /* object1 = thereExists pabsvars . (reachableStates /\ tau) */
      object1 = Img_MultiwayLinearAndSmooth(mgr, conjArray, pabsvars, 
	      emptyArray);
  
      array_free(conjArray); 
      mdd_free(reachableStates);
      array_free(pabsvars);
      array_free(emptyArray); 
  
      /* We need "forall absvars thereExists pabsvars .(reachableStates /\
       * tau) "
       * We compute "not thereExists absvars not (thereExists pabsvars
       * (reachableStates /\ tau))"
       * */
  
      notObject1 = mdd_not(object1);
      mdd_free(object1);
  
      object2 = mdd_smooth(mgr, notObject1, absvars);
      mdd_free(notObject1);
      array_free(absvars);
  
      tauMinusMdd = mdd_not(object2);
      mdd_free(object2); 

    if(mode) {
	Main_MochaPrint("tauminus mdd size is %d\n", 
		mdd_size(tauMinusMdd));
	 /*SlMddPrintCubes (tauMinusMdd, 0);*/
    }


    outPlusBddEntry = ALLOC(Sl_GSTEntry_t, 1);
    outPlusBddEntry->name = util_strsav(outPlusBddName);
    outPlusBddEntry->kind = Sl_BDD_c;
    outPlusBddEntry->obj.bddArray = SlSingleMddArray(tauPlusMdd);
    st_insert(Sl_GST, outPlusBddEntry->name, (char *) outPlusBddEntry);

    outMinusBddEntry = ALLOC(Sl_GSTEntry_t, 1);
    outMinusBddEntry->name = util_strsav(outMinusBddName);
    outMinusBddEntry->kind = Sl_BDD_c;
    outMinusBddEntry->obj.bddArray = SlSingleMddArray(tauMinusMdd);
    st_insert(Sl_GST, outMinusBddEntry->name, (char *) outMinusBddEntry);

    return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [implements the sl_better_upre_comb  command.]

  Description        [This function implements the uncontrollable
  		      predecessor operator and computes the uncontrollable
		      predecessor states of FSM1 with respect to the specBdd.
		      It makes use of the tau_plus and tau_minus
		      mdd's of FSM2 to get a bigger target enlargement.]

  SideEffects        [none]


******************************************************************************/
int SlBetterUPreComb(
	      ClientData clientdata,
	      Tcl_Interp *interp,
	      int argc,
	      char ** argv)
{

    char usage[] = "Usage: sl_better_upre_comb <outBDD> <specBDD> <FSM1> <FSM2> <mealy|moore> <abstract|notabstract> <time in seconds> <s|v>  <var>... ";

    mdd_manager *mgr;
    array_t *conjArray, *emptyArray, *result, *objectArray, *tau1Array;
    array_t *tau2Array;
    mdd_t   *specMdd, *nSpecMdd, *notObject2, *object2, *image, *betterUPre;
    mdd_t  *zero, *object1, *object,*notimage2, *notObject; 
    mdd_t *conjMdd, *notConj, *reachableStates2;
    mdd_t *pR, *R, *newR;
    mdd_t *image1, *image2, *conj;
    Sl_GSTEntry_t *fsm1Entry, *specEntry, *outEntry; 
    Sl_GSTEntry_t *fsm2Entry;
    int      varid, mealy, i, abstract, niterations, noObject2 = 0;
    time_t maxTime, startTime, curTime, maxRunTime;
    int     mode;   /* 0 = silent, 1 = verbose */
    char * outBddName, *fsm1Name, *specBddName;
    char *varName, *fsm2Name;
    array_t *toQuantifyPlus, *notAbsvars2, *supportVars, *varsList; 
    array_t * cntrlvars1, *pcntrlvars1, * allPvars, *absvars2, *pcntrlvars2; 


    mgr = SlGetMddManager ();
    emptyArray = array_alloc (int, 0);
    util_getopt_reset();
    util_optind++;

    if (argc < 10) {
       Main_MochaErrorPrint("Syntax error. %s\n", usage);
       return TCL_OK;
     }

     outBddName = argv[util_optind];
     if (st_is_member(Sl_GST, outBddName)) { 
       Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
       return TCL_OK;
     }

     util_optind++;
     specBddName = argv[util_optind];
     if (!st_lookup(Sl_GST, specBddName, (char **) &specEntry)) {
	 Main_MochaErrorPrint("Error: Object %s does not exist.\n", specBddName);
	 return TCL_OK;
     }
     if (specEntry -> kind != Sl_BDD_c) {
	 Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", specBddName);
	 return TCL_OK;
     }

    util_optind++;
    fsm1Name = argv[util_optind];
    if (!st_lookup(Sl_GST, fsm1Name, (char **) &fsm1Entry)) {
       Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsm1Name);
       return TCL_OK;
    }
    if (fsm1Entry -> kind != Sl_FSM_c) {
         Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsm1Name);
         return TCL_OK;
    }

    util_optind++;
    fsm2Name = argv[util_optind];
    if (!st_lookup(Sl_GST, fsm2Name, (char **) &fsm2Entry)) {
       Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsm2Name);
       return TCL_OK;
    }
    if (fsm2Entry -> kind != Sl_FSM_c) {
         Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsm2Name);
         return TCL_OK;
    }

    util_optind++;
    if (strcmp (argv[util_optind], "mealy") == 0) {
	mealy = 1;
    } else if (strcmp (argv[util_optind], "moore") == 0) {
	mealy = 0;
    } else {
	 Main_MochaErrorPrint("Syntax error. %s\n", usage);
	 return TCL_OK;
    }

    util_optind++;
    if (strcmp (argv[util_optind], "abstract") == 0) {
	abstract = 1;
    } else if (strcmp (argv[util_optind], "notabstract") == 0) {
	abstract = 0;
    } else {
	 Main_MochaErrorPrint("Syntax error. %s\n", usage);
	 return TCL_OK;
    }

    util_optind++;
    maxRunTime = (time_t) atol(argv[util_optind]);

    util_optind++;
    if (strcmp (argv[util_optind], "s") == 0) {
	mode = 0;
    } else if (strcmp (argv[util_optind], "v") == 0) {
	mode = 1;
    } else {
	 Main_MochaErrorPrint("Syntax error. %s\n", usage);
	 return TCL_OK;
    }

    util_optind++;
    /* varsList is the set of variables of module 2. It can contain either
     * the abstracted controllable vars of module 2(if the input argument
     * <abstract> is set) or the controllable vars of module 2 which have 
     * NOT been abstracted (if input argument is <notabstract>).
     */
    varsList = array_alloc(int, 0);
    while(argc - util_optind != 0) {
	varName = argv[util_optind];
	varid = SlObtainUnprimedBddId(varName);
	if( varid == -1) {
	    Main_MochaErrorPrint("Error: variable %s invalid\n",
		    argv[util_optind]);
	    array_free(varsList );
	    return TCL_OK;
	}
	array_insert_last(int, varsList , varid);
	util_optind++;
    }

    if(abstract) {
	/* varsList is the set of variables that are to be abstracted */
	absvars2 = varsList;
	notAbsvars2 = SlArrayDifference(fsm2Entry -> obj.fsm.contrvarL,
		absvars2);
    } else {
	/*varsList is the set of controllable vars of FSM 2 which are
	 * not to be abstracted. */
	notAbsvars2 = varsList;
	absvars2 = SlArrayDifference(fsm2Entry -> obj.fsm.contrvarL,
		notAbsvars2);
    }


    cntrlvars1 = array_dup(fsm1Entry -> obj.fsm.contrvarL);
    pcntrlvars1 = SlPrimeVars (cntrlvars1);
    pcntrlvars2 = SlPrimeVars(fsm2Entry -> obj.fsm.contrvarL);

    toQuantifyPlus = array_join(pcntrlvars2, absvars2);

    allPvars = array_join(pcntrlvars2, pcntrlvars1);
    array_free(pcntrlvars2);


    /* The support of the invariant, which are all the vars that
     * the invariant mdd can contain 
     *  NOTE: Ashwini TODO : This is different for transition invariants
     *  which will also contain the primed vars */
    supportVars = array_join(cntrlvars1, notAbsvars2);
    array_free(cntrlvars1);
    array_free(notAbsvars2);

    /* startTIme contains the current time */
    time(&startTime);
    maxTime = startTime + maxRunTime;

    /* compute the conjunction of the specification */
    /* As we are doing UPre, we use "not Spec" instead of "Spec" */
    specMdd = SlConjoin (specEntry->obj.bddArray);
    nSpecMdd = mdd_not(specMdd);

    /* Compute the reachable states of the input FSM2 */
    Main_MochaPrint("TEST 2 : before slcomputereach()\n");
    reachableStates2 =  SlComputeReach (fsm2Entry, 3, NULL, 1);
    Main_MochaPrint("TEST 3 : after slcomputereach()\n");

    if (mode) {
	Main_MochaPrint("The state size of the initial invariant is \n");
	SlPrintBddStats (nSpecMdd, supportVars);
    }
    newR = nSpecMdd;
    R = mdd_zero(mgr);
    zero = mdd_zero(mgr);

    niterations = 0;
    do {
        mdd_free(R);
        R = newR;
        pR = SlPrimeMdd(R);
	if( mealy) {

	    /* we need thereExists cntrlvars1' . forall notAbsvars2' .
	     * [thereExists absvars2 thereExists absvars2'( tau2 /\
	     * reachableStates2) -> (tau1 /\ R') ]
	     * We do E cntrlvars1' . A notAbsvars2' . A absvars2 .
	     * A absvars2' [ not(reachablestates2 \/ not(tau2) \/
	     * not(tau1 /\ R'))]
	     * We can do the above for we only abstract the private
	     * vars(absvars2 and absvars2') of module 2 which do not 
	     * appear in tau1 or the invariant R'.
	     *
	     * Here as tau1 and tau2 are arrays we take care to not
	     * conjoin them to be efficient in our computations.
	     * So we do
	     *  E cntrlvars1' [not(E notAbsvars2' . E absvars2 .
	     *  E absvars2' [ reachablestates2 /\ tau2 /\ not(tau11)]]
	     *                /\
	     *   [not(E notAbsvars2' . E absvars2 .  E absvars2'
	     *  [reachablestates2 /\ tau2 /\ not(tau12)]] ....
	     *               /\
	     *   [not(E notAbsvars2' . E absvars2 .  E absvars2'
	     *  [reachablestates2 /\ tau2 /\ not(R')]]
	     *
	     */
	    tau2Array = array_dup(fsm2Entry -> obj.fsm.updateBdd);
	    array_insert_last (mdd_t*, tau2Array, reachableStates2);

	    tau1Array = array_dup(fsm1Entry -> obj.fsm.updateBdd);
	    array_insert_last (mdd_t*, tau1Array, pR);

            objectArray = array_alloc(mdd_t *, 0);
	    arrayForEachItem(mdd_t*, tau1Array,i, conj) {
		notConj = mdd_not(conj);
		conjArray = array_dup(tau2Array);
		array_insert_last(mdd_t *, conjArray , notConj);
		notObject = Img_MultiwayLinearAndSmooth(mgr, conjArray, 
			toQuantifyPlus, emptyArray);
		array_free(conjArray);
		mdd_free(notConj);
		object = mdd_not(notObject);
		mdd_free(notObject);
		array_insert_last(mdd_t*, objectArray, object);
		if (mode) {
			Main_MochaPrint("object %d size is %d\n",
					i, mdd_size(object));
		}
	    }
	    /*array_free(tau1Array); We are using it again */

	    object1 = Img_MultiwayLinearAndSmooth(mgr, objectArray, pcntrlvars1,
		    emptyArray);

	    arrayForEachItem(mdd_t*, objectArray,i, object) {
		mdd_free(object);
	    }
	    array_free(objectArray);

            if(mode) {
        	Main_MochaPrint("size of mealy Object1 Mdd is %d\n", 
        		mdd_size(object1));
	        SlPrintBddStats (object1, supportVars);
            }
	} else {
	    Main_MochaPrint("New Moore modules not implemented\n");
	    /* moore modules */
            /*tau1AndPR = mdd_and(tau1Mdd, pR, 1, 1);
            conjMdd = mdd_or(notTau2PlusMdd, tau1AndPR,1,1);
	    mdd_free(tau1AndPR);*/
        
            /* image = thereExists cntrlvars1'. ( tau2Plus -> (tau1 /\ R')) 
	     * image = thereExists cntrlvars1' ( not Tau2plus \/ (tau1
	     * 		/\ R'))
	     * image = (thereExists cntrlvars1' ( not tau2plus)) \/
	     * 		(thereExists cntrlvars1' (tau1 /\ R'))*/

	    /*image1 = mdd_smooth(mgr, notTau2PlusMdd, pcntrlvars1);
	    conjArray = array_dup (fsm1Entry -> obj.fsm.updateBdd);
	    array_insert_last (mdd_t*, conjArray, pR);
	    image2 = Img_MultiwayLinearAndSmooth(mgr, conjArray, pcntrlvars1,
		    emptyArray);
	    array_free(conjArray);*/


            /* we need object1 = forall notAbsvars2' . thereExists cntrlvars1'.
	     * ( tau2Plus -> (tau1 /\ R'))
             * we do object1 = not thereExists cntrlAvsVars2' not .(image1 \/
	     * image2)
	     */
	    /*image = mdd_and(image1, image2, 0,0);
	    mdd_free(image1);
	    mdd_free(image2);

	    notObject = mdd_smooth(mgr, image, pNotAbsvars2);
            mdd_free(image);
	    object1 = mdd_not(notObject);
	    mdd_free(notObject);
            if(mode) {
        	Main_MochaPrint("size of moore object1 Mdd is %d\n", 
        		mdd_size(object1));
            }*/
	}
    
	if(noObject2 == 0) {
            /* object2 computation */
	    /* bupMinusMinus */
	    /* we have
	     * bup--(R) = A absvars2. E cntrlvars1'. E notAbsvars2'.
	     * E absvars2'. ( reachablestates2 /\ tau2 /\ tau1 /\
	     * R')
	     */

	    /* We have already inserted the reachablestates2 mdd into
	     * the tau2Array. So tau2Array = tau2 /\ reachablestates2.
	     * Also tau1Array = tau1 /\ R'.
	     */

	    conjArray = array_join( tau2Array, tau1Array);
	    array_free(tau1Array);
	    array_free(tau2Array);

	    objectArray = array_alloc(mdd_t *, 0);
	    arrayForEachItem(mdd_t*, conjArray, i, conj) {
	        object = mdd_smooth(mgr, conj, allPvars);
	        notObject = mdd_not(object);
	        mdd_free(object);
	        array_insert_last(mdd_t*, objectArray, notObject);
	        if(mode) {
	             Main_MochaPrint("Size of object %d is %d\n",
				 i, mdd_size(notObject));
	        }
	    }
	    Main_MochaPrint("TEST:Done obj2 1st part\n");
	    array_free(conjArray);
	    mdd_free(pR);
	    object2 = Img_MultiwayLinearAndSmooth(mgr, objectArray,
			    absvars2, emptyArray);

	    arrayForEachItem(mdd_t*, objectArray,i, object) {
                mdd_free(object);
	    }
	    array_free(objectArray);


            /*image2 = Img_MultiwayLinearAndSmooth(mgr, conjArray, allPvars,
		    emptyArray);

            array_free(conjArray);
	    mdd_free(pR);
	    notimage2 = mdd_not(image2);
	    mdd_free(image2);
	    notObject2 = mdd_smooth(mgr, notimage2, absvars2);
	    mdd_free(notimage2);
	    object2 = mdd_not(notObject2);
	    mdd_free(notObject2);*/

            if(mode) {
		Main_MochaPrint("Object2 mdd is ");
	        SlPrintBddStats (object2,supportVars);
            }

	    if( mdd_size(object2) == 1) {
		/* do not compute object2 for the next iteration, for 
		 * better upre minus does not exist */
                noObject2 = 1;
	    }
        }
        
        /* betterUPre = object1 \/ object2 */
	if (noObject2) {
            betterUPre = object1;
	} else {
            betterUPre = mdd_or(object1, object2, 1,0);
            mdd_free(object1);
            mdd_free(object2);
	}
        
        /* compute the new frontier */
        newR = mdd_or( R, betterUPre, 1, 1);
        mdd_free(betterUPre);
        if(mode) {
	    Main_MochaPrint("betterUPred Mdd stats at iteration %d is \n",
		    niterations++);
	    SlPrintBddStats (newR,supportVars);
        } 
        time(&curTime );
	/* continue if fix point is not reached, or time quota is not exceeded */
    } while( !mdd_equal(R, newR) && (curTime < maxTime));

    if(mode) {
	if( curTime > maxTime) {
	    Main_MochaPrint("Time limit exceeded. 
	 So computation stopped at iteration %d \n", niterations);
	}
	Main_MochaPrint("size of target UPre Mdd is \n") ;
	SlPrintBddStats(newR, supportVars);
    }
    result = SlSingleMddArray(newR);
    outEntry = ALLOC(Sl_GSTEntry_t, 1);
    outEntry -> name = util_strsav(outBddName);
    outEntry -> kind = Sl_BDD_c;
    outEntry -> obj.bddArray = result;
    st_insert(Sl_GST, outEntry->name, (char *) outEntry);

    mdd_free(reachableStates2);
    mdd_free(R);
    array_free(pcntrlvars1);
    array_free(allPvars);
    array_free(emptyArray);
    array_free(absvars2);

    return TCL_OK;

}


/**Function********************************************************************

  Synopsis           [implements the sl_better_Upre command.]

  Description        [This function implements the uncontrollable
  		      predecessor operator and computes the uncontrollable
		      predecessor states of FSM1 with respect to the specBdd.
		      It makes use of the tau_plus and tau_minus
		      mdd's of FSM2 to get a bigger target enlargement.]

  SideEffects        [none]


******************************************************************************/
int SlBetterUPre(
	      ClientData clientdata,
	      Tcl_Interp *interp,
	      int argc,
	      char ** argv)
{

    char usage[] = "Usage: sl_better_upre <outBDD> <specBDD> <FSM1> <FSM2> <tauPlusMdd> <tauMinusMdd> <mealy|moore> <abstract|notabstract> <s|v>  <var>... ";

    mdd_manager *mgr;
    array_t *conjArray, *emptyArray, *result, *objectArray, *tau1Array;
    mdd_t   *specMdd, *nSpecMdd, *notObject, *object2, *image, *betterUPre;
    mdd_t  *zero, *object1, *tau2MinusAndPr, *object; 
    mdd_t *conjMdd, *tau2PlusMdd, *tau2MinusMdd, *notTau2PlusMdd, *notConj;
    mdd_t *pR, *tau1AndPR, *R, *newR, *notTau2PlusOrPR, *tau1AndNottau2plusOrPr;
    mdd_t *image1, *image2, *conj;
    Sl_GSTEntry_t *fsm1Entry, *specEntry, *outEntry, *tauPlusEntry; 
    Sl_GSTEntry_t *fsm2Entry, *tauMinusEntry;
    int      varid, object2exists, mealy, i, abstract;
    int     mode;   /* 0 = silent, 1 = verbose */
    char * outBddName, *fsm1Name, *specBddName, * tauPlusMddName;
    char *tauMinusMddName, *varName, *fsm2Name;
    array_t *pNotAbsvars2, *notAbsvars2, *supportVars, *varsList; 
    array_t * cntrlvars1, *pcntrlvars1, * allPvars; 
    array_t *tauPlusMinusUnPSupport, *tauPlusMinusPSupport,*tauPlusMinusTotalSupport;


    mgr = SlGetMddManager ();
    emptyArray = array_alloc (int, 0);
    util_getopt_reset();
    util_optind++;

    if (argc < 11) {
       Main_MochaErrorPrint("Syntax error. %s\n", usage);
       return TCL_OK;
     }

     outBddName = argv[util_optind];
     if (st_is_member(Sl_GST, outBddName)) { 
       Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
       return TCL_OK;
     }

     util_optind++;
     specBddName = argv[util_optind];
     if (!st_lookup(Sl_GST, specBddName, (char **) &specEntry)) {
	 Main_MochaErrorPrint("Error: Object %s does not exist.\n", specBddName);
	 return TCL_OK;
     }
     if (specEntry -> kind != Sl_BDD_c) {
	 Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", specBddName);
	 return TCL_OK;
     }

    util_optind++;
    fsm1Name = argv[util_optind];
    if (!st_lookup(Sl_GST, fsm1Name, (char **) &fsm1Entry)) {
       Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsm1Name);
       return TCL_OK;
    }
    if (fsm1Entry -> kind != Sl_FSM_c) {
         Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsm1Name);
         return TCL_OK;
    }

    util_optind++;
    fsm2Name = argv[util_optind];
    if (!st_lookup(Sl_GST, fsm2Name, (char **) &fsm2Entry)) {
       Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsm2Name);
       return TCL_OK;
    }
    if (fsm2Entry -> kind != Sl_FSM_c) {
         Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsm2Name);
         return TCL_OK;
    }

     util_optind++;
     tauPlusMddName = argv[util_optind];
     if (!st_lookup(Sl_GST, tauPlusMddName, (char **) &tauPlusEntry)) {
	 Main_MochaErrorPrint("Error: Object %s does not exist.\n", tauPlusMddName);
	 return TCL_OK;
     }
     if (specEntry -> kind != Sl_BDD_c) {
	 Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", tauPlusMddName);
	 return TCL_OK;
     }

     util_optind++;
     tauMinusMddName = argv[util_optind];
     if (!st_lookup(Sl_GST, tauMinusMddName, (char **) &tauMinusEntry)) {
	 Main_MochaErrorPrint("Error: Object %s does not exist.\n",
		 tauMinusMddName);
	 return TCL_OK;
     }
     if (specEntry -> kind != Sl_BDD_c) {
	 Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", tauMinusMddName);
	 return TCL_OK;
     }

    util_optind++;
    if (strcmp (argv[util_optind], "mealy") == 0) {
	mealy = 1;
    } else if (strcmp (argv[util_optind], "moore") == 0) {
	mealy = 0;
    } else {
	 Main_MochaErrorPrint("Syntax error. %s\n", usage);
	 return TCL_OK;
    }

    util_optind++;
    if (strcmp (argv[util_optind], "abstract") == 0) {
	abstract = 1;
    } else if (strcmp (argv[util_optind], "notabstract") == 0) {
	abstract = 0;
    } else {
	 Main_MochaErrorPrint("Syntax error. %s\n", usage);
	 return TCL_OK;
    }

    util_optind++;
    if (strcmp (argv[util_optind], "s") == 0) {
	mode = 0;
    } else if (strcmp (argv[util_optind], "v") == 0) {
	mode = 1;
    } else {
	 Main_MochaErrorPrint("Syntax error. %s\n", usage);
	 return TCL_OK;
    }

    util_optind++;
    /* varsList is the set of variables of module 2. It can contain either
     * the abstracted controllable vars of module 2 or the controllable
     * vars of module 2 which have NOT been abstracted depending on
     * whether the input parameter abstract has been set of not.
     */
    varsList = array_alloc(int, 0);
    while(argc - util_optind != 0) {
	varName = argv[util_optind];
	varid = SlObtainUnprimedBddId(varName);
	if( varid == -1) {
	    Main_MochaErrorPrint("Error: variable %s invalid\n",
		    argv[util_optind]);
	    array_free(varsList );
	    return TCL_OK;
	}
	array_insert_last(int, varsList , varid);
	util_optind++;
    }

    if(abstract) {
	/* varsList is the set of variables that have been abstracted */
	notAbsvars2 = SlArrayDifference(fsm2Entry -> obj.fsm.contrvarL,
		varsList);
	array_free(varsList);
    } else {
	/*varsList is the set of controllable vars of FSM 2 which have
	 * not been abstracted in the computation of tauPlus and tauMinus*/
	notAbsvars2 = varsList;
    }


    outEntry = ALLOC(Sl_GSTEntry_t, 1);
    outEntry -> name = util_strsav(outBddName);
    outEntry -> kind = Sl_BDD_c;

    cntrlvars1 = array_dup(fsm1Entry -> obj.fsm.contrvarL);
    pcntrlvars1 = SlPrimeVars (cntrlvars1);
    pNotAbsvars2 = SlPrimeVars (notAbsvars2);
    allPvars = array_join(pcntrlvars1, pNotAbsvars2);

    /* The support of the invariant, which are all the vars that
     * the invariant mdd can contain 
     *  NOTE: Ashwini TODO : This is different for transition invariants
     *  which will also contain the primed vars */
    supportVars = array_join(cntrlvars1, notAbsvars2);


    /* compute the conjunction of the specification */
    /* As we are doing UPre, we use "not Spec" instead of "Spec" */
    specMdd = SlConjoin (specEntry->obj.bddArray);
    nSpecMdd = mdd_not(specMdd);

    if (mode) {
	Main_MochaPrint("The state size of the initial invariant is \n");
	SlPrintBddStats (nSpecMdd, supportVars);
    }
    newR = nSpecMdd;
    R = mdd_zero(mgr);
    pR = mdd_zero(mgr);
    zero = mdd_zero(mgr);

    tau2PlusMdd = SlConjoin(tauPlusEntry->obj.bddArray); 
    notTau2PlusMdd = mdd_not(tau2PlusMdd);
    tau2MinusMdd = SlConjoin ( tauMinusEntry->obj.bddArray);
    
    /* Check that the tauPlus and tauMinus Mdds do not contain the
     * abstracted variables */
    tauPlusMinusUnPSupport = array_join(fsm2Entry->obj.fsm.extvarL,notAbsvars2);
    tauPlusMinusPSupport = SlPrimeVars (tauPlusMinusUnPSupport);
    tauPlusMinusTotalSupport = array_join(tauPlusMinusUnPSupport,
	    tauPlusMinusPSupport);
    SlCheckSupportSubset (tau2PlusMdd, tauPlusMinusTotalSupport,
	    "ERL10: tauPlusMdd with too large support");
    SlCheckSupportSubset (tau2MinusMdd, tauPlusMinusTotalSupport,
	    "ERL10: tauMinusMdd with too large support");

    /* check whether tau2MinusMdd exists */
    if(mdd_size(tau2MinusMdd) > 1) {
	object2exists = 1;
    } else {
	object2exists = 0;
    }

    do {
        mdd_free(R);
        mdd_free(pR);
        R = newR;
        pR = SlPrimeMdd(R);
	if( mealy) {
	    Main_MochaPrint("MEALY UPRE \n");

	    /* we need thereExists cntrlvars1' . forall notAbsvars2' .
	     * ( tau2Plus -> (tau1 /\ R')) 
	     * We do thereExists cntrlvars1' . ( forall notAbsvars2' .
	     * ((not tau2Plus \/ tau1) /\ ( not tau2Plus \/ R')))
	     *
	     * thereExists cntrlvars1' . ((not thereExists Absvars2' 
	     *  (tau2Plus /\ not tau1)) /\ (not thereExists Absvars2' .
	     *  (tau2Plus /\ not R')))
	     * 
	     */
	    tau1Array = array_dup(fsm1Entry -> obj.fsm.updateBdd);
	    array_insert_last (mdd_t*, tau1Array, pR);

            objectArray = array_alloc(mdd_t *, 0);
	    arrayForEachItem(mdd_t*, tau1Array,i, conj) {
		notConj = mdd_not(conj);
		conjArray = SlSingleMddArray ( notConj);
		array_insert_last(mdd_t *, conjArray , tau2PlusMdd);
		notObject = Img_MultiwayLinearAndSmooth(mgr, conjArray, pNotAbsvars2,
			emptyArray);
		array_free(conjArray);
		object = mdd_not(notObject);
		mdd_free(notObject);
		array_insert_last(mdd_t*, objectArray, object);
	    }
	    array_free(tau1Array);

	    object1 = Img_MultiwayLinearAndSmooth(mgr, objectArray, pcntrlvars1,
		    emptyArray);

	    arrayForEachItem(mdd_t*, objectArray,i, object) {
		mdd_free(object);
	    }
	    array_free(objectArray);


	    /*notTau2PlusOrPR = mdd_or(notTau2PlusMdd, pR, 1,1);
	    tau1AndNottau2plusOrPr = mdd_and(tau1Mdd, notTau2PlusOrPR,1,1);
	    conjMdd = mdd_not(tau1AndNottau2plusOrPr);
	    conjArray = SlSingleMddArray(conjMdd);
	    mdd_free(tau1AndNottau2plusOrPr);
	    mdd_free(notTau2PlusOrPR);*/


	    /* We need forall notAbsvars2' (tau1 /\ (tau2Plus -> R'))
	     * We do not thereExists notAbsvars2' not ( tau1 /\
	     *  (tau2Plus -> R'))
             * image = thereExists notAbsvars2'. not ( tau1 /\
	     * (tau2Plus -> R')) */
	    /*image = Img_MultiwayLinearAndSmooth(mgr, conjArray,
	     * pNotAbsvars2, emptyArray);
	    array_free(conjArray);*/
	    /*image = mdd_smooth(mgr, conjMdd, pNotAbsvars2);
	    not_image = mdd_not(image);
            mdd_free(image);
            if(mode) {
        	Main_MochaPrint("done1  size of image Mdd is %d\n", mdd_size(not_image));
	    }*/

	    /*conjArray = SlSingleMddArray(not_image);*/
	    /* object1 = thereExists cntrlvars1' . forall notAbsvars2' .
	     * (tau1 /\ (tau2Plus -> R'))
	     * object1 = thereExists cntrlvars1'. (not_image) */
	    /*object1 = Img_MultiwayLinearAndSmooth(mgr, conjArray, pcntrlvars1,
		    emptyArray);
	    array_free(conjArray);*/
	    /*object1 = mdd_smooth(mgr, not_image, pcntrlvars1);
	    mdd_free(not_image);*/
            if(mode) {
        	Main_MochaPrint("done 2 size of mealy Object1 Mdd is %d\n", 
        		mdd_size(object1));
	        /*SlPrintBddStats (object1, supportVars);*/
            }
	} else {
	    /* moore modules */
            /*tau1AndPR = mdd_and(tau1Mdd, pR, 1, 1);
            conjMdd = mdd_or(notTau2PlusMdd, tau1AndPR,1,1);
	    mdd_free(tau1AndPR);*/
        
            /* image = thereExists cntrlvars1'. ( tau2Plus -> (tau1 /\ R')) 
	     * image = thereExists cntrlvars1' ( not Tau2plus \/ (tau1
	     * 		/\ R'))
	     * image = (thereExists cntrlvars1' ( not tau2plus)) \/
	     * 		(thereExists cntrlvars1' (tau1 /\ R'))*/

	    image1 = mdd_smooth(mgr, notTau2PlusMdd, pcntrlvars1);
	    conjArray = array_dup (fsm1Entry -> obj.fsm.updateBdd);
	    array_insert_last (mdd_t*, conjArray, pR);
	    image2 = Img_MultiwayLinearAndSmooth(mgr, conjArray, pcntrlvars1,
		    emptyArray);
	    array_free(conjArray);


            /* we need object1 = forall notAbsvars2' . thereExists cntrlvars1'.
	     * ( tau2Plus -> (tau1 /\ R'))
             * we do object1 = not thereExists cntrlAvsVars2' not .(image1 \/
	     * image2)
	     */
	    image = mdd_and(image1, image2, 0,0);
	    mdd_free(image1);
	    mdd_free(image2);

	    notObject = mdd_smooth(mgr, image, pNotAbsvars2);
            mdd_free(image);
	    object1 = mdd_not(notObject);
	    mdd_free(notObject);
            if(mode) {
        	Main_MochaPrint("size of moore object1 Mdd is %d\n", 
        		mdd_size(object1));
            }
	}
    
        /* object2 computation */
	if(object2exists) {
	    Main_MochaPrint("Object 2 exists \n");
            /* conjArray = tau2MinusMdd /\ tau1 /\ R' */
	    tau2MinusAndPr = mdd_and( tau2MinusMdd, pR,1,1);
	    conjArray = array_dup(fsm1Entry -> obj.fsm.updateBdd);
	    array_insert_last(mdd_t *, conjArray, tau2MinusAndPr);
        
            /* object2 = thereExists allPvars. (tau2Minus /\ tau1 /\ R' ) */
            object2 = Img_MultiwayLinearAndSmooth(mgr, conjArray, allPvars,
        	    emptyArray);
        
            mdd_free(tau2MinusAndPr);
            array_free(conjArray);
            if(mode) {
        	Main_MochaPrint("size of object2 Mdd is %d\n", 
        		mdd_size(object2));
            }
        
            /* betterUPre = object1 \/ object2 */
            betterUPre = mdd_or(object1, object2, 1,1);
        
            mdd_free(object1);
            mdd_free(object2);
	} else {
	    /* object2 does not exist */
	    betterUPre = object1;
	}

    
        /* compute the new frontier */
        newR = mdd_or( R, betterUPre, 1, 1);
        mdd_free(betterUPre);
        if(mode) {
	    SlPrintBddStats (newR,supportVars);
        } 
    
    } while( !mdd_equal(R, newR));

    if(mode) {
	Main_MochaPrint("size of target UPre Mdd is %d\n\n", 
		mdd_size(newR));
    }
    result = SlSingleMddArray(newR);
    outEntry -> obj.bddArray = result;
    st_insert(Sl_GST, outEntry->name, (char *) outEntry);

    /*mdd_free(tau1Mdd);*/
    mdd_free(notTau2PlusMdd);
    mdd_free(R);
    mdd_free(pR);
    array_free(pcntrlvars1);
    array_free(pNotAbsvars2);
    array_free(allPvars);
    array_free(emptyArray);
    array_free(cntrlvars1);
    array_free(notAbsvars2);

    return TCL_OK;

}


/**Function********************************************************************

  Synopsis           [Wrapper function for getRandomCubeInMdd]

  Description        []

  SideEffects        [none]


******************************************************************************/
mdd_t * SlGetRandomCubeInMdd ( mdd_t *mdd , unsigned int seed) 
{
  mdd_t * cube;

  cube = mdd_dup (mdd); 
  getRandomCubeInMdd (mdd, &cube, seed); 
  return cube; 
}


/**Function********************************************************************

  Synopsis           [Function implementing the function sl_inv_buildMdd]


  Description        [Builds an MDD for a given invariance.
  		      The invariant(s) first have to read using the
		      inv_read command.	]

  SideEffects        [none.]

  SeeAlso            [optional]


******************************************************************************/

int SlInvBuildMdd (
	      ClientData clientdata,
	      Tcl_Interp *interp,
	      int argc,
	      char ** argv)
{

  char usage[] = "Usage: sl_inv_buildMdd <module_name> [<inv_name> <inv_name>... ] <s|v>";
  array_t *invNameArray, *invariantArray;
  char *invName,* moduleName;
  int numFormula, i;
  Inv_Invariant_t *invariant;
  Inv_Manager_t* InvManager;
  Mdl_Manager_t * mdlManager;
  Mdl_Module_t * module;
  mdd_t *invMdd;
  Sym_Info_t * symInfo;
  Atm_Expr_t * typedExpr;
  Var_TypeManager_t *typeManager;
  Main_Manager_t *mainManager = (Main_Manager_t *) clientdata;
  Sl_GSTEntry_t *outEntry;
  int mode;

  InvManager = Inv_ReadInvManager();
  typeManager = (Var_TypeManager_t *) Main_ManagerReadTypeManager(
	  mainManager);

  util_getopt_reset();
  if ( argc < 3 ) {
       Main_MochaErrorPrint("Syntax error. %s\n", usage);
       return TCL_OK;
  }

  util_optind++;
  mdlManager = SlGetModuleManager();
  moduleName = argv[util_optind++];
  module = Mdl_ModuleReadFromName(mdlManager,moduleName);
  if( !module) {
      Main_MochaErrorPrint("Error: module \"%s\" does not exists.\n",
	      moduleName);
      return TCL_OK;
  }

 if (argc - (1+ util_optind) == 0) {
     /* the invariants are not specifed, so build the Mdds for
      * all the invariants of the module */
       invNameArray = Inv_ManagerReadInvariantNames(InvManager);
       invariantArray = Inv_ManagerReadInvariants(InvManager);
  } else {
      invNameArray = array_alloc(char *, 0);
      invariantArray = array_alloc(Inv_Invariant_t *, 0);
      while ((1 +util_optind) < argc) {
	  invName = argv[util_optind];  
	  if ((invariant = Inv_ManagerReadInvariantFromName(InvManager,
			  invName)) != NIL(Inv_Invariant_t)) {
	      array_insert_last(char *, invNameArray, invName);
	      array_insert_last(Inv_Invariant_t *, invariantArray, invariant);
	  }   else {
	      Main_MochaErrorPrint("Invariant %s not found\n", invName);
	      array_free(invNameArray); 
	      array_free(invariantArray);
	      Main_MochaErrorPrint("%s\n", usage);
	      return TCL_OK;
	  }
	  util_optind++; 
      }
  }

  symInfo = Sym_RegionManagerReadSymInfo(SlGetRegionManager(), module);


  /* fetch each invariant from the invariantArray and type check it.
   * Then build the Mdd for that invariant */
  arrayForEachItem(Inv_Invariant_t *, invariantArray, i, invariant) {
      invName = array_fetch(char *, invNameArray, i);
      Main_MochaPrint("Typechecking invariant %s...\n", invName); 
      if ((typedExpr = (Atm_Expr_t *)InvInvariantCreateTypedExpr(module, 
		      typeManager, invariant)) == NIL(Atm_Expr_t)) {
	   Main_MochaErrorPrint("Error in typechecking invariant %s\n", 
		   invName);
	   Atm_ExprFreeExpression(typedExpr);
      } else {
	  /* typechecking successful, build Mdd for invariant */
	  invMdd = BuildInvMdd(symInfo, typedExpr);
	  /* Ashwini: debug info */
	  /*SlMddPrintCubes(invMdd,0);*/
	  /* Store Mdd in the symbol table */
	  outEntry = ALLOC(Sl_GSTEntry_t,1);
	  /* We add "inv_" as a prefix for the invariant */
	  outEntry->name = util_strcat3 ("inv_", invName, "");
	  outEntry->kind = Sl_BDD_c;
	  outEntry->obj.bddArray = SlSingleMddArray(invMdd);
	  st_insert(Sl_GST, outEntry->name, (char *) outEntry);
	  /* free the typed expression, as it is no longer needed */
	  Atm_ExprFreeExpression(typedExpr);
	   Main_MochaPrint("Typechecking and creation of Mdd for <%s> invariant successful\n",
		   invName);
      }
  }

  array_free(invariantArray);
  array_free(invNameArray);
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Function implementing sl_sort_and_conjoin_tau]


  Description        [Givben an FSM, sorts the transition relation array (tau 
                      array) of the FSM according to the await-dependency 
		      relation and conjoins the taus till the <maxMddSize> as
		      input is reached. It then returns the FSM with the new
		      tau array of the conjoined mdds (instead of the old one).
		      (So for FSMs with many small taus, we get fewer larger 
		      taus making the computation on them more efficient. ]

  SideEffects        [the updateBdd of the FSM is modified.]

  SeeAlso            [optional]


******************************************************************************/

int SlSortAndConjoinTau (
	      ClientData clientdata,
	      Tcl_Interp *interp,
	      int argc,
	      char ** argv)
{

  char usage[] = "Usage: sl_sort_and_conjoin_tau <fsm_name> <maxMddSize> <s|v>";
  char *fsmName;
  Sl_GSTEntry_t *inFsm;
  int mode, maxTauSize, i;
  array_t *sortedArray, *sortedConjoinedArray;
  mdd_t * tau, *newTau , *tauAndNewTau, *one;
  mdd_manager *mgr = SlGetMddManager ();
  array_t * allvars, *allPvars, *allUpvars;


  util_getopt_reset();
  util_optind++;

 if (argc < 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &inFsm)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (inFsm -> kind != Sl_FSM_c) {
      Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
      return TCL_OK;
  }

  util_optind++;
  maxTauSize = atoi (argv[util_optind]); 

  util_optind++;
  if (strcmp (argv[util_optind], "s") == 0) {
      mode = 0;
  }  else if (strcmp (argv[util_optind], "v") == 0) {
      mode = 1;
  } else {
      Main_MochaErrorPrint("Syntax error. %s\n", usage);
      return TCL_OK;
  }

  /* sort the tau array according to the await dependency relation */
  sortedArray = tauarrayDepSort(inFsm);

  allUpvars = array_join(inFsm -> obj.fsm.contrvarL, inFsm -> obj.fsm.extvarL);
  allPvars = SlPrimeVars(allUpvars);
  allvars = array_join(allUpvars, allPvars);

  if (mode) {
       Main_MochaPrint("Init tau Array and sizes are \n");
       arrayForEachItem(mdd_t *, sortedArray, i, tau) {
	    Main_MochaPrint("tau %d  = ", i);
	    SlPrintBddStats(tau, allvars);
       }
       Main_MochaPrint("\n");
  }

  sortedConjoinedArray = array_alloc( mdd_t *, 0);
  newTau = mdd_one(mgr);
  one = mdd_one(mgr);
  Main_MochaPrint("size od mdd_one is %d\n", mdd_size(one));

  arrayForEachItem(mdd_t *, sortedArray, i, tau) {
      tauAndNewTau = mdd_and(tau, newTau,1,1);
      if (mdd_size(tauAndNewTau) > maxTauSize) {
	  mdd_free(tauAndNewTau);
	  if (mdd_equal(newTau, one )) {
	      /* conjoin of mdd_one() and tau is itself > than maxTauSize */
	      if ( i == 0) {
		  /* 1st iteration */
	          array_insert_last(mdd_t *, sortedConjoinedArray, tau);
	      } else {
		  /* nth iteration, where (n-1)th tau was = mdd_one()*/
	          array_insert_last(mdd_t *, sortedConjoinedArray, newTau);
		  newTau = tau;
	      }
	  } else {
	      array_insert_last(mdd_t *, sortedConjoinedArray, newTau);
	      newTau = tau;
	  }
      } else {
	  mdd_free(newTau);
	  mdd_free(tau);
	  newTau = tauAndNewTau;
      }
  }
  if (!mdd_equal(newTau, one ))
      array_insert_last(mdd_t *, sortedConjoinedArray, newTau);
  Main_MochaPrint("\n");

  array_free(sortedArray);
  array_free(inFsm -> obj.fsm.updateBdd);
  inFsm -> obj.fsm.updateBdd = sortedConjoinedArray;

  if (mode) {
       Main_MochaPrint("Final tau Array and sizes are \n");
       arrayForEachItem(mdd_t *, sortedConjoinedArray, i, tau) {
	    Main_MochaPrint("tau %d = ", i);
	    SlPrintBddStats(tau, allvars);
       }
  }

  return TCL_OK;
}









/*****************************************************************************r
               STATIC FUNCTIONS BELOW THIS POINT 
 ******************************************************************************/

/**Function********************************************************************

  Synopsis           [Computes a random cube from the input mdd]

  Description        [Optional]

  SideEffects        [none]

  SeeAlso            [Optional]

******************************************************************************/
static void getRandomCubeInMdd(
		      mdd_t *mdd, /* the mdd that is passed recursively, and gets shorter */ 
		      mdd_t **resultCube, /* the final mdd (same height as original) that 
				      contains the cube */ 
		      unsigned int seed
 )
{
  mdd_t *s, *s_and;
  int  id;
  int nvals, i;
  mdd_t *literal;
  Var_Variable_t *var;
  mdd_manager *mgr;
  int done = 0; 
  int mode = 0; 	/* debug info */

  mgr = SlGetMddManager ();
  
  if(mdd_is_tautology(mdd,1)){
    return;
  }


  id       = SlGetTopMddId(mdd); 
  var      = SlReturnVariableFromId (id); 
  nvals    = ReadDomainSize(var);

  /* WARNING: computes a random cube using a bad algorithm */ 
  do { 
    /*i = SlReturnRandomValue (nvals); */
      i = random () % nvals;

    /* This is the MDD for the value n. i of the variable */ 
    /* let's cross our fingers; why does SlRandomSliceBdd in 
       slFreddy.c use a different method? */ 
    literal = SlGetLiteral(id,i);
    if(mode) {
          Main_MochaPrint(" random value to be picked is %d from nvals %d\n", 
		  i, nvals);
    }

    s = mdd_cofactor(mgr, mdd, literal);
    /* If non-false, we have found a proper value for the state */ 
    if( !mdd_is_tautology(s, 0)){
      /* Found a good value for the literal */ 
      done = 1;
      /* Conjoins the literal to the result cube */ 
      s_and = mdd_and (literal, *resultCube, 1, 1); 
      mdd_free (*resultCube); 
      *resultCube = s_and; 
      /* Recursively call, to fix the literals below */ 
      getRandomCubeInMdd(s, resultCube, seed); 
    }
    mdd_free(literal);
    mdd_free (s); 
  } while (!done); 
}

/**Function********************************************************************

  Synopsis           [Computes a domain size of a variable ]

  Description        [Optional]

  SideEffects        [none]

  SeeAlso            [Optional]

******************************************************************************/
int ReadDomainSize(Var_Variable_t * var)
{
  Var_Type_t *type;

 /*Main_MochaPrint(" datatype of variable is: %d\n",
       Var_VariableReadDataType(var));*/

  switch (Var_VariableReadDataType(var)) {
    case Var_Boolean_c:
    case Var_Event_c: 	/* Boolean ??*/
      return 2;
      break;

    case Var_Enumerated_c:
    case Var_Range_c:
    case Var_Timer_c:
      type = Var_VariableReadType(var);
      return (Var_VarTypeReadDomainSize(type));
      break;
		      
    case Var_Bitvector_c:
    case Var_BitvectorN_c:
    case Var_Array_c:
      type = Var_VariableReadType(var);
      return(Var_VarTypeArrayOrBitvectorReadSize(type));
      break;

    default:
      Main_MochaPrint("Warning:cannot get the domain size for this datatype: %d\n",
       Var_VariableReadDataType(var));
  }
}



/**Function********************************************************************

  Synopsis           []


  Description        [ Builds an MDD for a given atomic expression.
  		       (This is essentially the Sym_ExprBuildMdd() function
		       taken from the sym/symExprMdd.c file. Modifications will
		       be done to this function if the MddId's are not assigned
		       correctly. -Ashwini) ]

  SideEffects        [none.]

  SeeAlso            [optional]


******************************************************************************/
static mdd_t * BuildInvMdd( Sym_Info_t * symInfo, Atm_Expr_t* expr)
{ 

  mdd_t *result; 
  mdd_t *temp1, *temp2, *temp3;
  mdd_t *exprMdd, *litMdd;
  mdd_manager *manager = SlGetMddManager();
  int id, arrayId;
  Var_Variable_t *var, *aVar;
  Atm_ExprType aVarType;
  int nVals;
  Atm_Expr_t *indexExpr;
  
  Atm_ExprType exprType = Atm_ExprReadType(expr);
  switch(exprType){
      case Atm_IfThenElseFi_c:
      {
        temp1 = Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =
            Sym_ExprBuildMdd(symInfo,
			     Atm_ExprReadLeftChild(Atm_ExprReadRightChild(expr)));
        temp3 =
            Sym_ExprBuildMdd(symInfo,
			     Atm_ExprReadRightChild(Atm_ExprReadRightChild(expr)));
        result = mdd_ite( temp1, temp2, temp3, 1, 1, 1);
        mdd_free(temp1);
        mdd_free(temp2);
        mdd_free(temp3);
        break;
      } 
      
      case Atm_UnPrimedVar_c:
      {

        var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        
        if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, var, &id )){
          Main_MochaErrorPrint( "Error: Sym_ExprBuildMdd, cant find mdd id for var\n");
          exit(1);
        }

        /* we have reached a variable as a leaf. Should be boolean!!  */
        if(!(Var_VariableIsBoolean(var) || Var_VariableIsEvent(var))){
          Main_MochaErrorPrint(
            "Error: Sym_ExprBuildMdd, encountered non-boolean leaf\n");
          exit(1);
        }
        
        result = SlGetLiteral(id, 1);
        break;
      }
      
      case Atm_PrimedVar_c:
      {
        var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        
        if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &id )){
          Main_MochaErrorPrint( "Error: Sym_ExprBuildMdd, cant find mddid for var\n");
          exit(1);
          
        }
        /* we have reached a variable as a leaf. Should be boolean!!  */
        if(!(Var_VariableIsBoolean(var) || Var_VariableIsEvent(var))){
          Main_MochaErrorPrint(
            "Error: Sym_ExprBuildMdd, encountered non-boolean leaf\n");
          exit(1);
        }
        
        result = SlGetLiteral(id, 1);
        break;
      }
      
      
      case Atm_BoolConst_c:
      {
        if (Atm_ExprReadLeftChild(expr)) 
          result = mdd_one(manager);
        else
          result = mdd_zero(manager);
        break;
      }

      case Atm_Or_c:
      {
        temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadRightChild(expr));
        result = mdd_or(temp1, temp2, 1, 1);
        mdd_free(temp1);
        mdd_free(temp2);
        break;
      }
      
      case Atm_And_c:
      {
        temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadRightChild(expr));
        result = mdd_and(temp1, temp2, 1, 1);
        mdd_free(temp1);
        mdd_free(temp2);
        break;
      }
      
      case Atm_Implies_c:
      {
	temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadRightChild(expr));
        result = mdd_or(temp1, temp2, 0, 1);
        mdd_free(temp1);
        mdd_free(temp2);
        break;
      }
      
      case Atm_Equiv_c:
      {
        temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadRightChild(expr));
        result = mdd_xnor(temp1, temp2);
        mdd_free(temp1);
        mdd_free(temp2);
        break;
      }

      case Atm_NotEquiv_c:
      case Atm_Xor_c:
      {
        temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        temp2 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadRightChild(expr));
        result = mdd_xor(temp1, temp2);
        mdd_free(temp1);
        mdd_free(temp2);
        break;
      }

      case Atm_Not_c:
      {
        temp1 =  Sym_ExprBuildMdd(symInfo, Atm_ExprReadLeftChild(expr));
        result = mdd_not(temp1);
        mdd_free(temp1);
        break;
      }
      

      case Atm_Equal_c:
      case Atm_NotEqual_c:
      case Atm_Greater_c:
      case Atm_GreaterEqual_c:
      case Atm_Less_c:
      case Atm_LessEqual_c:
      {
        result = (mdd_t *) SymMVBuildBooleanExpressionMdd(symInfo, expr);
        break;
      }

      case Atm_RedAnd_c:
      case Atm_RedOr_c:
      case Atm_RedXor_c:
      case Atm_BitwiseEqual_c:
      case Atm_BitwiseNotEqual_c:
      case Atm_BitwiseGreater_c:
      case Atm_BitwiseGreaterEqual_c:
      case Atm_BitwiseLess_c:
      case Atm_BitwiseLessEqual_c:
      {
        result = (mdd_t *)SymBitVectorBuildBooleanExpressionMdd(symInfo, expr);
        break;
      }
      
      case Atm_EventQuery_c:
      {
	Var_Variable_t *var = (Var_Variable_t *)
            Atm_ExprReadLeftChild(Atm_ExprReadLeftChild(expr));

        if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &id )){
          Main_MochaErrorPrint( "Error: Sym_ExprBuildMdd, cant find mdd id for var\n");
          exit(1);
        }         

        result = SlGetLiteral(id, 1);
        break;
      }

      case Atm_Index_c:
      {
        int i;
        Atm_Expr_t *lexpr = Atm_ExprReadLeftChild(expr);
        aVar   = (Var_Variable_t *)Atm_ExprReadLeftChild(lexpr);
        aVarType = Atm_ExprReadType(lexpr);
        indexExpr =  Atm_ExprReadRightChild(expr);


        if(SymIsNumEnumRangeConstant(indexExpr)){
          arrayId = SymGetArrayId(symInfo, aVar, aVarType,  SymGetConstant(indexExpr));
          result = SlGetLiteral(arrayId, 1);
        }
        else {
          nVals = SymGetArrayNvals(aVar);
          result = mdd_zero(manager);
          for ( i = 0; i < nVals; i++){
            exprMdd = (mdd_t *)SymExprBuildExprConstMdd(symInfo, indexExpr, 
		    i, nVals);
            arrayId = SymGetArrayId( symInfo, aVar, aVarType, i);

            litMdd =  SlGetLiteral( arrayId, 1);
            temp1  = mdd_and(exprMdd, litMdd, 1, 1);
            temp2  = mdd_or(temp1, result, 1, 1);
            mdd_free(result);
            mdd_free(temp1);
            mdd_free(exprMdd);
            mdd_free(litMdd);
            result = temp2;
          }
        }
        break;
      }

      case Atm_UnaryMinus_c:
      case Atm_Plus_c:
      case Atm_Minus_c:
      case Atm_NumConst_c:
      case Atm_EnumConst_c:
      default:
      {
        Main_MochaErrorPrint( "Error: Sym_ExprBuildMdd, unsupported operator\n");
        exit(1);
      }
      
  }
  
  return(result);
}


static mdd_t * BuildIdentityMdd(mdd_manager *mgr, array_t * unprimedVars)
{
    int primedId, i, id;
    mdd_t * partialIdentityMdd, *newIdentityMdd ;
    mdd_t * identityMdd = mdd_one(mgr);

    arrayForEachItem(int, unprimedVars, i, id) {
	primedId = SlReturnPrimedId(id);
	if (primedId == -1) {
	    Main_MochaErrorPrint("Error: id \"%d\" is not a valid id. \n", id);
	    return NULL;
	}
	partialIdentityMdd = mdd_eq_s(mgr, id, primedId);
	newIdentityMdd = mdd_and(partialIdentityMdd, identityMdd, 1,1);
	mdd_free(partialIdentityMdd);
	mdd_free(identityMdd);
	identityMdd = newIdentityMdd;
    }

    /*SlMddPrintCubes(identityMdd,0);*/

    return identityMdd;


}

/*typedef struct {
    mdd_t * tau;
    array_t * cntrlvars;
    array_t * dependArray;
} TauStruct;
*/

/* sort the transition relations array of the fsm according to the
 * await dependency relation. A topological sort of the conjuncts is done
 * according to the await-dependency relation with the awaited conjuncts
 * at the beginning of the array followed by the dependent ones.*/
static array_t * tauarrayDepSort(Sl_GSTEntry_t *fsmEntry )
{
    array_t *tauArray, *tauStructArray, *depArray, *sortedTauArray; 
    array_t * finalSortedArray;
    int i, j, k, id, primedId, controlled, found, numElements;
    mdd_t * tau;
    Pair_t *depPair;
    TauStruct * taustruct;
    mdd_manager *mddManager = SlGetMddManager(); 
    int debug = 0;
    int count = 1;


    tauStructArray = array_alloc( TauStruct *, 0);
    sortedTauArray = array_alloc( mdd_t *, 0);
    finalSortedArray = array_alloc( mdd_t *, 0);
    tauArray = array_dup(fsmEntry -> obj.fsm.updateBdd); 
    depArray = array_dup(fsmEntry -> obj.fsm.dependency);

    arrayForEachItem(mdd_t *, tauArray, i, tau) {
	array_t * unPrimedIdsOfPrimedVars = array_alloc(int, 0);
	array_t * controlledVars = array_alloc(int, 0);
	array_t * supportVarsId;
	TauStruct * taustructPtr = newTauStruct();
	taustructPtr -> tau = tau;

	supportVarsId = mdd_get_support(mddManager, tau);
	/*if(debug) {
	    Main_MochaPrint("All support vars of tau 1 \n");
	    SlPrintVarNames (supportVarsId);
	}*/
	/* get only the primed vars from the support */
	arrayForEachItem(int, supportVarsId, j, id) {
	    primedId = SlReturnPrimedId(id);
	    if ( id == primedId) {
		array_insert_last(int, unPrimedIdsOfPrimedVars, 
			SlReturnUnprimedId (id));
	    }
	}
	array_free(supportVarsId);

	/* given the primed vars( = awaited vars + controlled vars),
	 *  get the controlled vars */
	 arrayForEachItem(int, unPrimedIdsOfPrimedVars, j, id) {
	     Pair_t * pair = SlfindPair(depArray, id);
	     if( !pair) {
		 Main_MochaErrorPrint("Error:invalid dependency relation for variable %s\n",
			  SlReturnVariableFromId(id));
		 array_free(tauArray);
		 array_free(depArray);
		 array_free(unPrimedIdsOfPrimedVars);
		 array_free(controlledVars);
		 deleteTauStruct(taustructPtr);
                 arrayForEachItem(TauStruct *, tauStructArray, i, taustruct) {
		     deleteTauStruct(taustruct);
                 }
                 array_free(tauStructArray);
		 return NULL;
	     }

	     /* If ANY of the variables in the pair->info array i.e the
	      * array of variables that depend on the variable "id" belong
	      * to the primed support of tau, then the atom of tau will
	      * await the var. "id", else it will control the variable "id"
	      * */
	     controlled = 1;
	     arrayForEachItem(Pair_t *,(array_t *) pair -> info, 
		     k, depPair) {
		 found = isElementFound( depPair->first,
			 unPrimedIdsOfPrimedVars);
		 if( found) {
		     controlled = 0;
		     /*if (debug) {
			 Main_MochaPrint("%s is a Awaited var by %s\n", 
				 Var_VariableReadName(SlReturnVariableFromId( id)),
				 Var_VariableReadName(SlReturnVariableFromId( 
					 depPair->first)));
		     }*/
		     break;
		     }
		 /* not found: so continue to search */
	     }
	     if (controlled == 1) {
		 array_insert_last(int, controlledVars, id);
		 /*if (debug) {
		     Main_MochaPrint("Controlled var %s and id %d\n", 
			 Var_VariableReadName(SlReturnVariableFromId( id)),
			 id);
		 }*/
	     }
	 }
	 array_free(unPrimedIdsOfPrimedVars);
	taustructPtr -> cntrlvars = controlledVars;

	/*if(debug) {
	    Main_MochaPrint("All primed controlled vars of tau 1 \n");
	    SlPrintVarNames ( controlledVars);
	    Main_MochaPrint("All primed controlled vars ids of tau 1 \n");
	    arrayForEachItem(int, controlledVars, j, id) {
		Main_MochaPrint(" %d ", id);
	    }
	}*/
	array_insert_last(TauStruct *, tauStructArray, taustructPtr);
    }
    array_free(tauArray);

    /* For each taustruct put in the dependArray member all the
     * taustructs that depend on it */
    arrayForEachItem(TauStruct *, tauStructArray, i, taustruct) {
	arrayForEachItem(int, taustruct->cntrlvars, j, id) {
	    Pair_t * pair = SlfindPair(depArray, id);
	    arrayForEachItem(Pair_t *,(array_t *) pair -> info, 
	        k, depPair) {
		TauStruct * cntrlTau = findControllingtauStruct(tauStructArray, 
			depPair->first);
		if(cntrlTau != NULL) {
		    /* add the edge from the awaited tau structure to
		     * the dependent one */
                    array_insert_last(TauStruct *, taustruct->dependArray, 
			    cntrlTau);
		}
	    }
	}
	array_sort(taustruct->dependArray, SlCompareInt);
	array_uniq(taustruct->dependArray, SlCompareInt, 0);
    }

    array_free (depArray);

    /*if (debug) {
	arrayForEachItem(TauStruct *, tauStructArray, i, taustruct) {
	    Main_MochaPrint("cntrlvars of tau %d\n", count++);
	    SlPrintVarNames (taustruct->cntrlvars);
	    Main_MochaPrint("Number of taus dependent on this tau: %d\n", 
		    array_n(taustruct->dependArray));

	}
    }*/

    /* run topological sort to arrange the transition relations according to
     * the await dependency relation */
    arrayForEachItem(TauStruct *, tauStructArray, i, taustruct) {
	depthFirstSearch(taustruct, sortedTauArray);
    }

    /* free the tauStructArray */
    arrayForEachItem(TauStruct *, tauStructArray, i, taustruct) {
	deleteTauStruct(taustruct);
    }
    array_free(tauStructArray);

    /* The sortedTauArray has to be reversed to get the taus sorted
     * such that the dependent taus are before the awaited ones.
     */ 
    numElements = array_n(sortedTauArray);
    for( i = numElements-1 ; i >= 0; i-- ) {
	tau = array_fetch(mdd_t *, sortedTauArray, i);
	array_insert_last(mdd_t *, finalSortedArray, tau);
    }

    array_free(sortedTauArray);

    count = 0;
    if (debug) {
	arrayForEachItem(mdd_t *, finalSortedArray, i, tau) {
	    Main_MochaPrint("Support of tau %d in the list is \n", count++);
	    SlPrintVarNames(mdd_get_support(mddManager, tau));
	}
    }
    return finalSortedArray;
}


static void depthFirstSearch(TauStruct * parent, array_t * resultArray)
{
    int i;
    TauStruct * child;

    arrayForEachItem(TauStruct *, parent->dependArray, i, child) {
	depthFirstSearch(child, resultArray);
    }
    if (!(parent->flag)) {
	array_insert_last(mdd_t *, resultArray, parent->tau);
	/*Main_MochaPrint("Inserting tau controlling vars \n");
	SlPrintVarNames(parent->cntrlvars);*/
	parent->flag = 1;
    }
}

/*static void addEdgeList(TauStruct * parent, TauStruct * child) 
{
    int found, i;
    TauStruct * dep;
     array_insert_last(TauStruct *,parent->dependArray, child);
}*/

static int isElementFound( int element, array_t *rhs)
{
  int i, var; 

  arrayForEachItem( int , rhs, i, var){
      if( element == var) {
	  return 1;
      }
  }
  return 0;
}

static TauStruct * findControllingtauStruct(array_t * taustructarray, int varid)
{
    int i, found;
    TauStruct * taustruct;

    arrayForEachItem(TauStruct *, taustructarray, i, taustruct) {
        found = isElementFound( varid, taustruct->cntrlvars);
	if(found) {
	    return taustruct;
	}
	/* not found. continue to search */
    }

    /*If u reach here, there is an error */
    Main_MochaErrorPrint("Error: variable %s not controlled by any conjunct\n",
	     Var_VariableReadName(SlReturnVariableFromId(varid)));
    return NULL;
}


static TauStruct * newTauStruct()
{
    TauStruct * newTau;
    newTau = ALLOC(TauStruct, 1);
    newTau->tau = NULL;
    newTau->cntrlvars = NULL;
    newTau->dependArray = array_alloc( TauStruct *, 0);
    newTau->flag = 0;
    return newTau;
}


static void deleteTauStruct(TauStruct *taustruct) {
    array_free(taustruct->cntrlvars);
    array_free(taustruct->dependArray);
    FREE(taustruct);
}

/* 
 * $Log: slSim.c,v $
 * Revision 1.49  2003/05/19 20:36:56  ashwini
 * modified better_upre_comb upre-- to reduce it's memory requirements.
 *
 * Revision 1.48  2003/05/13 23:54:00  ashwini
 * added sl_sort_and_conjoin_tau fn to conjoin the taus of a given fsm.
 *
 * Revision 1.47  2003/05/03 22:45:32  ashwini
 * addded mdd_srandom function call.
 *
 * Revision 1.46  2003/05/03 22:27:31  ashwini
 * modified SlRandomCubeInvChk function to take in an array of invariants.
 *
 * Revision 1.45  2003/05/03 20:50:04  ashwini
 * some diagnostic changes.
 *
 * Revision 1.44  2003/05/02 08:33:31  luca
 * Improved SlRandomcubeInvChk by passing the next-state vars to Img_multilinearandsmooth
 *
 * Revision 1.43  2003/05/02 05:27:56  luca
 * Added Log keywords
 *
 * 
 */ 
