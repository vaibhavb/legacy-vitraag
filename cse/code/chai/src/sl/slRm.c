/**CFile***********************************************************************

  FileName    [slLuca.c]

  PackageName [sl]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [Luca de Alfaro and Freddy Mang]

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

  Revision    [$Id: slRm.c,v 1.24 2003/05/02 05:27:56 luca Exp $]

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

static mdd_t * computeControlReach(Sl_GSTEntry_t *fsmEntry, array_t *ctrlArray, array_t *specArray, array_t *initArray, int mode);
static void computeRandReach(Sl_GSTEntry_t *inFsm, int task, array_t *invariant, int sizeLimit, int iterLimit, int shrinkAlgo, int mode);
static void computeRandSim(Sl_GSTEntry_t *inFsm, int task, array_t *invariant, int sizeLimit, int iterLimit, int shrinkAlgo, int mode);
static int computeMaxTotalSize(int maxAllSize,  int sizeR, array_t * invariant, int task);
static int computeMaxInvariantSize(int maxInvariantSize, array_t * invariant, int task);
static void mddShrink  (int algo, mdd_t **R, int sizeR, array_t *supportVars, int sizeLimit, int mode);
static void randomMddCubeShrink  (mdd_t **R, int sizeR, array_t *supportVars, int sizeLimit, int mode);
static void randomMddStateShrink (mdd_t **R, int sizeR, array_t *supportVars, int sizeLimit, int mode);
static int get_polarity (int var, array_t *controlled_vars, array_t *existential_vars);

/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


int SlCreateCommandLuca(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{

  Tcl_CreateCommand(interp, "sl_copy", SlCopyCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_checkimpl", SlCheckImplCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_compose", SlComposeCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_wcontr", SlWcontrCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_modwcontr", SlModWcontrCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_lcontr", SlLcontrCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_lcontr_n", SlLcontrNCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_reach", SlReachCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_reach_histonly", SlReachHistoryCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_checkinv", SlCheckInvCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_checktrinv", SlCheckTrInvCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_initrand", SlInitRandCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_checkrandinv", SlCheckRandInvCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_checksim", SlCheckSimCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_trinv", SlTrInvCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_contrreachspec", SlContrReachSpecCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_contrreachfsm", SlContrReachFsmCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_erase", SlEraseCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_and", SlAndCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_or", SlOrCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_not", SlNotCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_true", SlTrueCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_restrict", SlRestrictCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_getinit", SlGetInitCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_gettr", SlGetTRCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_bddsize", SlBddSizeCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_predsize", SlPredSizeCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_maketrinv", SlMakeTrInvCmd, 
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_getfsmvars", SlGetFsmVarsCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_makevarlist", SlMakeVarListCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sl_conjoin_and_copy", SlConjoinAndCopyCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_copy.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlCopyCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_copy <outObj> <inObj>";
  char *inObjName, *outObjName; 
  Sl_FSM_t * FSM;
  Sl_GSTEntry_t *inEntry, *outEntry;
  
  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outObjName = argv[util_optind];
  if (st_is_member(Sl_GST, outObjName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outObjName);
    return TCL_OK;
  }
      
  util_optind++;

  inObjName = argv[util_optind];
  if (!st_lookup(Sl_GST, inObjName, (char **) &inEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inObjName);
    return TCL_OK;
  }

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outObjName);
  outEntry -> kind = inEntry -> kind;
  if (inEntry -> kind == Sl_BDD_c) {
    outEntry -> obj.bddArray = SlBddArrayDupe(inEntry -> obj.bddArray);
  } else {
    outEntry -> obj.fsm.initBdd    = SlBddArrayDupe(inEntry -> obj.fsm.initBdd);
    outEntry -> obj.fsm.updateBdd  = SlBddArrayDupe(inEntry -> obj.fsm.updateBdd);
    outEntry -> obj.fsm.contrvarL  = SlIntArrayDupe(inEntry -> obj.fsm.contrvarL);
    outEntry -> obj.fsm.extvarL    = SlIntArrayDupe(inEntry -> obj.fsm.extvarL);
    outEntry -> obj.fsm.erasedvarL = SlIntArrayDupe(inEntry -> obj.fsm.erasedvarL);
  }

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_checkimpl.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlCheckImplCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_checkimpl <Bdd1> <Bdd2> (checks Bdd1 -> Bdd2)";
  char *Bdd1Name, *Bdd2Name;
  Sl_GSTEntry_t *Entry1, *Entry2;
  array_t *bddArray1, *bddArray2;
      
  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_ERROR;
  }

  Bdd1Name = argv[util_optind];
  if (!st_lookup(Sl_GST, Bdd1Name, (char **) &Entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", Bdd1Name);
    return TCL_ERROR;
  }
  if (Entry1 -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", Bdd1Name);
    return TCL_ERROR;
  }

  util_optind++;

  Bdd2Name = argv[util_optind];
  if (!st_lookup(Sl_GST, Bdd2Name, (char **) &Entry2)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", Bdd2Name);
    return TCL_ERROR;
  }
  if (Entry2 -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", Bdd2Name);
    return TCL_ERROR;
  }

  /* Get the arrays of Bdds: call them bddArray1 and bddArray2. */

  bddArray1 = Entry1 -> obj.bddArray;
  bddArray2 = Entry2 -> obj.bddArray;
   
  /* And, check the implication. */ 

  Main_ClearResult();
  if (SlImplicationCheck (bddArray1, bddArray2)) {
    Main_AppendResult("%d", 1);
  } else {
    Main_AppendResult("%d", 0);
  }

  return TCL_OK;
}

  
/**Function********************************************************************

  Synopsis           [Function implementing command sl_compose.
                      Note that this function on purpose composes two 
		      FSMs even if they share controlled variables.
		      This can lead to misuse, but is also more general.

		      Dec 20, 2002: changed to take into account the 
		      list of history-free variables. -Luca
		      Feb 25, 2003: removed the histroy free vars from
		      the erasedvarL list, as they are constructed on the
		      fly in SlComputeReach. -Ashwini]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlComposeCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_compose <outFsm> <inFsm1> <inFsm2>";
  char *fsm1Name, *fsm2Name, *outFsmName;
  Sl_GSTEntry_t *entry1, *entry2, *outEntry;
  array_t *temp1, *temp2, *merged_dependency, *duplicated_vars; 
  array_t *allvars, *history_free; 
  mdd_manager *mgr;

  util_getopt_reset();
  util_optind++;
  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outFsmName = argv[util_optind];
  if (st_is_member(Sl_GST, outFsmName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outFsmName);
    return TCL_OK;
  }

  util_optind++;

  fsm1Name = argv[util_optind];
  if (!st_lookup(Sl_GST, fsm1Name, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsm1Name);
    return TCL_OK;
  }
  if (entry1 -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsm1Name);
    return TCL_OK;
  }

  util_optind++;

  fsm2Name = argv[util_optind];
  if (!st_lookup(Sl_GST, fsm2Name, (char **) &entry2)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsm2Name);
    return TCL_OK;
  }
  if (entry2 -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsm2Name);
    return TCL_OK;
  }


  /* Checks whether the FSMs can really be composed */ 

  /* First, checks that there are no shared controlled variables. */ 
  duplicated_vars = SlArrayIntersection (entry1 -> obj.fsm.contrvarL, 
					 entry2 -> obj.fsm.contrvarL);
  if (array_n (duplicated_vars) > 0) { 
    Main_MochaErrorPrint ("Error: FSMs being composed share controlled variables.\n"); 
    return TCL_OK; 
  }

  /* Second, checks that there are no cycles in the dependency relation */ 
  merged_dependency = SlDependencyMerge(entry1 -> obj.fsm.dependency,
					entry2 -> obj.fsm.dependency);
  if (SlDetectCycles (merged_dependency)) { 
    Main_MochaErrorPrint ("Error: dependency cycle in composition\n"); 
    return TCL_OK; 
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outFsmName);
  outEntry -> kind = Sl_FSM_c; 

  /* initial and update relation are obtained simply by concatenation. */
  outEntry -> obj.fsm.initBdd = array_join (entry1 -> obj.fsm.initBdd, 
					    entry2 -> obj.fsm.initBdd);
  outEntry -> obj.fsm.updateBdd = array_join (entry1 -> obj.fsm.updateBdd, 
					      entry2 -> obj.fsm.updateBdd);

  /* the controlled variables are the union of the two sets of controlled variables, 
     eliminating the duplicates. */ 
  outEntry -> obj.fsm.contrvarL = SlArrayUnion (entry1 -> obj.fsm.contrvarL, 
					      entry2 -> obj.fsm.contrvarL); 

  /* the external variables are (e1 \union e2) \setminus c, 
     eliminating the duplicates. */ 
  temp1 = SlArrayUnion (entry1 -> obj.fsm.extvarL, 
		      entry2 -> obj.fsm.extvarL); 
  outEntry -> obj.fsm.extvarL = SlArrayDifference (temp1, outEntry -> obj.fsm.contrvarL);
  array_free (temp1); 

  /* Generates the list of history-free variables. 
     Not very important, since it can be reconstructed on the fly. */ 
  /* Commented out - Ashwini */
  /*allvars = array_join (outEntry -> obj.fsm.contrvarL, outEntry -> obj.fsm.extvarL); 
  outEntry -> obj.fsm.erasedvarL = SlHistoryFree (allvars, outEntry -> obj.fsm.updateBdd); 
  array_free (allvars); */

  /* Ashwini: Add the original code to computet the erasedvarL of the outentry */
  /* the erased variables are x = (x1 \ c2) \union (x2 \ c1). */
  temp1 = SlArrayDifference (entry1 -> obj.fsm.erasedvarL,
	  entry2 -> obj.fsm.contrvarL); 
  temp2 = SlArrayDifference (entry2 -> obj.fsm.erasedvarL,
	  entry1 -> obj.fsm.contrvarL);
  outEntry -> obj.fsm.erasedvarL = SlArrayUnion (temp1, temp2); 
  array_free (temp1);
  array_free (temp2);

  outEntry -> obj.fsm.dependency = merged_dependency; 
  /* These lists need to be kept sorted */
  /* why? I don't remember. luca */ 
  array_sort(outEntry->obj.fsm.contrvarL, SlCompareInt);
  array_sort(outEntry->obj.fsm.extvarL, SlCompareInt);

  st_insert(Sl_GST, outEntry -> name,  (char *) outEntry); 

  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_lcontr.]

  Description        [This function computes the lazily controllable states, 
                      taking into account the array of erased variables. 

		      25 Feb 2003: Ashwini
		      changed the subtitution of primed vars to include the
		      transition relation of the FSM too, which may contain
		      primed external vars if there is an await relationship]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlLcontrCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_lcontr <outBDD> <specBDD> <FSM> <s|v>";
  char *fsmName, *specBddName, *outBddName;
  Sl_GSTEntry_t *fsmEntry, *specEntry, *outEntry;
  mdd_t   *U, *newU, *pU; 
  mdd_t   *Spec, *tau, *tauSubs;
  mdd_t   *subSpecAndU, *notSpecAndU; 
  array_t *conjArray, *emptyArray, *tauSubsArray, *tauArray;
  mdd_t   *resultMdd;
  array_t *pcontrEr, *primExtl, *externalVar;
  array_t *allVars, *supportContr; 
  array_t *result;
  int      niterations, i;
  mdd_manager *mgr;
  int     mode;   /* 0 = silent, 1 = verbose */ 
  int     maxsize; 
  int     sizeU; 
  int     maxCPCQSize;
  int     maxManagerSize;


  util_getopt_reset();
  util_optind++;
  if (argc != 5) {
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

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
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

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;
  
  mgr = SlGetMddManager ();

  /* Computes the conjunction Spec = SpecConj1 /\ ... /\ SpecConjn, 
     where SpecConj's are the pieces of the specification. */

  Spec = SlConjoin (specEntry->obj.bddArray); 

  /* Now computes pcontrNoner = contr' \union erased. */ 

  pcontrEr     = SlPrimeVars    (fsmEntry -> obj.fsm.contrvarL); 
  array_append (pcontrEr, fsmEntry -> obj.fsm.erasedvarL);

  /* Computes the primed external variables (for the substitution). */ 

  primExtl = SlPrimeVars (fsmEntry -> obj.fsm.extvarL); 

  /* Computes supportContr, which is the set of allowed 
     support for the controllability predicate. */

  allVars = array_join (fsmEntry -> obj.fsm.contrvarL, 
			     fsmEntry -> obj.fsm.extvarL); 
  supportContr = SlArrayDifference (allVars, fsmEntry -> obj.fsm.erasedvarL); 
  array_free (allVars); 

  /* U (and its new copy newU) will be the controllability fixpoint. */ 

  newU              = mdd_one (mgr); 
  U                 = NULL; 
  emptyArray        = array_alloc(int, 0);
  niterations       = 0; 
  maxsize           = 1; 

  /* Substitute the primExtl with externalVar in tauArray -Ashwini*/
  tauArray = array_dup (fsmEntry -> obj.fsm.updateBdd);
  externalVar = array_dup(fsmEntry -> obj.fsm.extvarL);
  tauSubsArray = array_alloc(mdd_t *, 0);
  arrayForEachItem(mdd_t*, tauArray, i, tau) {
    tauSubs = mdd_substitute (mgr, tau, primExtl, externalVar);
    array_insert_last(mdd_t*, tauSubsArray, tauSubs);
  }

  array_free(tauArray);
  array_free(externalVar);

  do {

    if (U != NULL) { 
      mdd_free (U); 
    }
    U = newU; 
    niterations++; 

    /* If the transition relation is T_P = t1 /\ ... /\tn, 
       creates in conjArray an array of BDDs 
       t1, ..., tn, not (Spec and U')[E/E']  */ 

    pU           = SlPrimeMdd(U); 
    notSpecAndU  = mdd_or  (Spec, pU, 0, 0);
    mdd_free       (pU); 
    subSpecAndU  = mdd_substitute (mgr, notSpecAndU, primExtl, 
				   fsmEntry -> obj.fsm.extvarL); 
    mdd_free       (notSpecAndU); 

    conjArray = array_dup(tauSubsArray);
    array_insert_last (mdd_t *, conjArray, subSpecAndU); 

    /* The computation of the fixpoint is written: 
       not Exists C'. conjArray, where conjArray has been smoothed, 
       and C' is as computed before.  */ 

    resultMdd  = Img_MultiwayLinearAndSmooth(mgr, conjArray, pcontrEr, emptyArray);
    mdd_free     (subSpecAndU); 
    array_free   (conjArray); 
    newU       = mdd_not (resultMdd); 
    mdd_free     (resultMdd); 

    if (mode) { 
      Main_MochaPrint ("Conrollability iteration n. %d\n", niterations);
      SlPrintBddStats (newU, supportContr); 
    }
    sizeU = mdd_size (newU); 
    if (sizeU > maxsize) { 
      maxsize = sizeU; 
    }

  } while (!mdd_equal(U, newU)); 

  result = array_alloc (mdd_t *, 0); 
  array_insert_last (mdd_t *, result, U); 
  outEntry -> obj.bddArray = result; 

  arrayForEachItem(mdd_t*, tauSubsArray, i, tauSubs) {
      mdd_free(tauSubs);
  }
  array_free(tauSubsArray);
  mdd_free   (newU); 
  array_free (pcontrEr); 
  array_free (emptyArray); 
  array_free (supportContr); 

  /* I pring the n. of iterations to converge, not the n. of iterations to 
     detect convergence (which is bigger by 1). */ 
  Main_MochaPrint ("*Controllability computed in %d iterations.\n", niterations - 1);
  /* I also print the max MDD size that occurred. */ 
  Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}
  


/**Function********************************************************************

  Synopsis           [Function implementing command sl_lcontr_n.]

  Description        [This function computes the lazily controllable states, 
                      for at most a specified number of iterations, 
                      taking into account the array of erased variables. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlLcontrNCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_lcontr_n <outBDD> <specBDD> <FSM> <n_iterations> <s|v>";
  char *fsmName, *specBddName, *outBddName;
  Sl_GSTEntry_t *fsmEntry, *specEntry, *outEntry;
  mdd_t   *U, *newU, *pU; 
  mdd_t   *Spec;
  mdd_t   *subSpecAndU, *notSpecAndU; 
  array_t *conjArray, *emptyArray;
  mdd_t   *resultMdd;
  array_t *pcontrEr, *primExtl;
  array_t *allVars, *supportContr; 
  array_t *result;
  int      max_iterations, niterations;
  mdd_manager *mgr;
  int     mode;   /* 0 = silent, 1 = verbose */ 
  int     maxsize; 
  int     sizeU; 
  int     maxCPCQSize;
  int     maxManagerSize;


  util_getopt_reset();
  util_optind++;
  if (argc != 6) {
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

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
    return TCL_OK;
  }

  util_optind++;
  max_iterations = atoi (argv[util_optind]); 

  util_optind++;

  if (strcmp (argv[util_optind], "s") == 0) {
    mode = 0; 
  } else if (strcmp (argv[util_optind], "v") == 0) {
    mode = 1; 
  } else { 
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;
  
  mgr = SlGetMddManager ();

  /* Computes the conjunction Spec = SpecConj1 /\ ... /\ SpecConjn, 
     where SpecConj's are the pieces of the specification. */

  Spec = SlConjoin (specEntry->obj.bddArray); 

  /* Now computes pcontrNoner = contr' \union erased. */ 

  pcontrEr     = SlPrimeVars    (fsmEntry -> obj.fsm.contrvarL); 
  array_append (pcontrEr, fsmEntry -> obj.fsm.erasedvarL);

  /* Computes the primed external variables (for the substitution). */ 

  primExtl = SlPrimeVars (fsmEntry -> obj.fsm.extvarL); 

  /* Computes supportContr, which is the set of allowed 
     support for the controllability predicate. */

  allVars = array_join (fsmEntry -> obj.fsm.contrvarL, 
			     fsmEntry -> obj.fsm.extvarL); 
  supportContr = SlArrayDifference (allVars, fsmEntry -> obj.fsm.erasedvarL); 
  array_free (allVars); 

  /* U (and its new copy newU) will be the controllability fixpoint. */ 

  newU              = mdd_one (mgr); 
  U                 = NULL; 
  emptyArray        = array_alloc(int, 0);
  niterations       = 0; 
  maxsize           = 1; 

  do {

    if (U != NULL) { 
      mdd_free (U); 
    }
    U = newU; 
    niterations++; 

    /* If the transition relation is T_P = t1 /\ ... /\tn, 
       creates in conjArray an array of BDDs 
       t1, ..., tn, not (Spec and U')[E/E']  */ 

    pU           = SlPrimeMdd(U); 
    notSpecAndU  = mdd_or  (Spec, pU, 0, 0);
    mdd_free       (pU); 
    subSpecAndU  = mdd_substitute (mgr, notSpecAndU, primExtl, 
				   fsmEntry -> obj.fsm.extvarL); 
    mdd_free       (notSpecAndU); 

    conjArray = array_dup (fsmEntry -> obj.fsm.updateBdd);
    array_insert_last (mdd_t *, conjArray, subSpecAndU); 

    /* The computation of the fixpoint is written: 
       not Exists C'. conjArray, where conjArray has been smoothed, 
       and C' is as computed before.  */ 

    resultMdd  = Img_MultiwayLinearAndSmooth(mgr, conjArray, pcontrEr, emptyArray);
    mdd_free     (subSpecAndU); 
    array_free   (conjArray); 
    newU       = mdd_not (resultMdd); 
    mdd_free     (resultMdd); 

    if (mode) { 
      Main_MochaPrint ("Conrollability iteration n. %d\n", niterations);
      SlPrintBddStats (newU, supportContr); 
    }
    sizeU = mdd_size (newU); 
    if (sizeU > maxsize) { 
      maxsize = sizeU; 
    }

  } while (niterations < max_iterations && !mdd_equal(U, newU)); 

  result = array_alloc (mdd_t *, 0); 
  array_insert_last (mdd_t *, result, newU); 
  outEntry -> obj.bddArray = result; 

  mdd_free   (U); 
  array_free (pcontrEr); 
  array_free (emptyArray); 
  array_free (supportContr); 

  /* I pring the n. of iterations to converge, not the n. of iterations to 
     detect convergence (which is bigger by 1). */ 
  Main_MochaPrint ("*Controllability computed in %d iterations.\n", niterations - 1);
  /* I also print the max MDD size that occurred. */ 
  Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}
  

/**Function********************************************************************

  Synopsis           [Function implementing command sl_wcontr.]

  Description        [This function computes the weakly controllable states, 
                      taking into account the array of erased variables. 
		      phi' / phi bug fixed -lda]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlWcontrCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_wcontr <outBDD> <specBDD> <FSM> <s|v>";
  char *fsmName, *specBddName, *outBddName;
  Sl_GSTEntry_t *fsmEntry, *specEntry, *outEntry;
  mdd_t   *U, *newU, *pU, *NUp, *CPre; 
  mdd_t   *Spec, *notSpecAndU; 
  array_t *conjArray, *emptyArray;
  mdd_t   *resultMdd, *negMdd; 
  array_t *pcontrEr, *extlNoner, *pextlNoner, *primedErased; 
  array_t *allVars, *supportContr; 
  array_t *result;
  int      niterations;
  mdd_manager *mgr;
  int     mode;   /* 0 = silent, 1 = verbose */ 
  int     maxsize; 
  int     sizeU; 


  util_getopt_reset();
  util_optind++;
  if (argc != 5) {
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

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
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

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;
  
  mgr = SlGetMddManager ();

  /* Computes the conjunction Spec = SpecConj1 /\ ... /\ SpecConjn, 
     where SpecConj's are the pieces of the specification. */

  Spec = SlConjoin (specEntry->obj.bddArray);

  /* Now computes pextlNoner, the array of non-erased external variables primed, 
     and pcontrNoner = contr' \union erased' \union erased. */ 

  extlNoner    = SlArrayDifference (fsmEntry -> obj.fsm.extvarL, 
				  fsmEntry -> obj.fsm.erasedvarL); 
  pextlNoner   = SlPrimeVars (extlNoner); 
  array_free     (extlNoner); 

  pcontrEr     = SlPrimeVars    (fsmEntry -> obj.fsm.contrvarL); 
  array_append (pcontrEr, fsmEntry -> obj.fsm.erasedvarL);
  primedErased = SlPrimeVars    (fsmEntry -> obj.fsm.erasedvarL); 
  array_append (pcontrEr, primedErased); 
  array_free     (primedErased); 
  array_sort     (pcontrEr, SlCompareInt); 
  array_uniq     (pcontrEr, SlCompareInt, NULL); 


  /* Computes supportContr, which is the set of allowed 
     support for the controllability predicate. */

  allVars = array_join (fsmEntry -> obj.fsm.contrvarL, 
			     fsmEntry -> obj.fsm.extvarL); 
  supportContr = SlArrayDifference (allVars, fsmEntry -> obj.fsm.erasedvarL); 
  array_free (allVars); 

  /* U (and its new copy newU) will be the controllability fixpoint. */ 

  newU              = Spec; 
  U                 = NULL; 
  emptyArray        = array_alloc(int, 0);
  niterations       = 0; 
  maxsize           = 1; 

  do {

    if (U != NULL) { 
      mdd_free (U); 
    }
    U = newU; 
    niterations++; 

    /* If the transition relation is T_P = t1 /\ ... /\tn, 
       creates in conjArray an array of BDDs 
       t1, ..., tn, not (Spec' and U')  */ 

    pU           = SlPrimeMdd (U);
    NUp          = mdd_not (pU); 
    mdd_free       (pU);

    conjArray = array_dup (fsmEntry -> obj.fsm.updateBdd);
    array_insert_last (mdd_t *, conjArray, NUp); 

    /* The computation of the fixpoint is written: 
       Exists E'. not Exists C'. conjArray, where conjArray 
       has been smoothed, and where E' and C' are as computed 
       before.  It is computed in a few steps. */ 

    resultMdd  = Img_MultiwayLinearAndSmooth(mgr, conjArray, pcontrEr, emptyArray);
    array_free   (conjArray); 

    negMdd     = mdd_not (resultMdd); 
    mdd_free     (resultMdd);

    CPre       = mdd_smooth (mgr, negMdd, pextlNoner);
    mdd_free     (negMdd); 
 
    newU       = mdd_and (U, CPre, 1, 1); 
    mdd_free     (CPre); 

    if (mode) { 
      Main_MochaPrint ("*Conrollability iteration n. %d\n", niterations);
      SlPrintBddStats (newU, supportContr); 
    }
    sizeU = mdd_size (newU); 
    if (sizeU > maxsize) { 
      maxsize = sizeU; 
    }

  } while (!mdd_equal(U, newU)); 

  result = array_alloc (mdd_t *, 0); 
  array_insert_last (mdd_t *, result, U); 
  outEntry -> obj.bddArray = result; 

  mdd_free   (newU); 
  array_free (pcontrEr); 
  array_free (pextlNoner); 
  array_free (emptyArray); 
  array_free (supportContr); 

  /* I pring the n. of iterations to converge, not the n. of iterations to 
     detect convergence (which is bigger by 1). */ 
  Main_MochaPrint ("*Controllability computed in %d iterations.\n", niterations - 1);
  /* I also print the max MDD size that occurred. */ 
  Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}
  


/**Function********************************************************************

  Synopsis           [Function implementing command sl_modwcontr.]

  Description        [This function computes the modularly weakly controllable 
                      states, taking into account the array of erased variables and 
		      an additional module as a controller.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlModWcontrCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_modwcontr <outBDD> <specBDD> <FSMcontrolled> <FSMcontroller> <s|v>";
  char *fsmName, *fsmContrName, *specBddName, *outBddName;
  Sl_GSTEntry_t *fsmEntry, *fsmContrEntry, *specEntry, *outEntry;
  mdd_t   *U, *newU, *newU1, *pU, *resultMdd; 
  mdd_t   *Spec, *notSpecAndU; 
  array_t *conjArray, *emptyArray, *controllingTrans;
  array_t *pcontrEr, *extlNoner, *pextlNoner, *primedErased; 
  array_t *allVars, *supportContr, *controllerVars, *uControllerVars, *pControllerVars, *cVars; 
  array_t *result;
  int      niterations;
  mdd_manager *mgr;
  int     mode;   /* 0 = silent, 1 = verbose */ 
  int     maxsize; 
  int     sizeU; 


  util_getopt_reset();
  util_optind++;
  if (argc != 6) {
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

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
    return TCL_OK;
  }

  util_optind++;

  fsmContrName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmContrName, (char **) &fsmContrEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmContrName);
    return TCL_OK;
  }
  if (fsmContrEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmContrName);
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

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;
  
  mgr = SlGetMddManager ();

  /* Computes the conjunction Spec = SpecConj1 /\ ... /\ SpecConjn, 
     where SpecConj's are the pieces of the specification. */

  Spec = SlConjoin (specEntry->obj.bddArray);

  /* Computes supportContr, which is the set of allowed 
     support for the controllability predicate. */

  allVars = array_join (fsmEntry -> obj.fsm.contrvarL, 
			     fsmEntry -> obj.fsm.extvarL); 
  supportContr = SlArrayDifference (allVars, fsmEntry -> obj.fsm.erasedvarL); 
  array_free (allVars); 

  /* Now computes pextlNoner, the array of non-erased external variables primed, 
     and pcontrNoner = contr' \union erased' \union erased. */ 

  extlNoner    = SlArrayDifference (fsmEntry -> obj.fsm.extvarL, 
				  fsmEntry -> obj.fsm.erasedvarL); 
  pextlNoner   = SlPrimeVars (extlNoner); 
  array_free     (extlNoner); 

  pcontrEr     = SlPrimeVars    (fsmEntry -> obj.fsm.contrvarL); 
  array_append (pcontrEr, fsmEntry -> obj.fsm.erasedvarL);
  primedErased = SlPrimeVars    (fsmEntry -> obj.fsm.erasedvarL); 
  array_append (pcontrEr, primedErased); 
  array_free     (primedErased); 
  array_sort     (pcontrEr, SlCompareInt); 
  array_uniq     (pcontrEr, SlCompareInt, NULL); 


  /* Now computes uControllerVars, which is the array of unprimed controller 
     variables that has to be existentially quantified.  It includes all 
     the variables of the controller Q, except for those in supportContr. */ 

  cVars = array_dup (fsmContrEntry -> obj.fsm.extvarL); 
  array_append      (cVars, fsmContrEntry -> obj.fsm.contrvarL); 
  uControllerVars = SlArrayDifference (cVars, supportContr); 
  array_free (cVars); 

  /* Now computes pControllerVars, the array of primed controller variables 
     that has to be existentially quantified.  
     It includes all the variables of Q, and all those in pextlNoner.
     Take the union, and eliminate the duplicates. */ 

  cVars = array_dup    (fsmContrEntry -> obj.fsm.extvarL); 
  array_append         (cVars, fsmContrEntry -> obj.fsm.contrvarL); 
  pControllerVars = SlPrimeVars (cVars); 
  array_free (cVars); 

  array_append (pControllerVars, pextlNoner); 
  array_sort   (pControllerVars, SlCompareInt); 
  array_uniq   (pControllerVars, SlCompareInt, NULL); 

  /* Finally, controllerVars is the union of uControllerVars and 
     pControllerVars.  I should not need to check for duplicates. */ 

  controllerVars = array_join (uControllerVars, pControllerVars); 
  array_free (uControllerVars); 
  array_free (pControllerVars); 

  /* U (and its new copy newU) will be the controllability fixpoint. */ 

  newU              = mdd_one (mgr); 
  U                 = NULL; 
  emptyArray        = array_alloc(int, 0);
  niterations       = 0; 
  maxsize           = 1; 

  do {

    if (U != NULL) { 
      mdd_free (U); 
    }
    U = newU; 
    niterations++; 

    /* If the transition relation is T_P = t1 /\ ... /\tn, 
       creates in conjArray an array of BDDs 
       t1, ..., tn, not (Spec and U')  */ 

    pU           = SlPrimeMdd(U); 
    notSpecAndU  = mdd_or (Spec, pU, 0, 0); 
    mdd_free       (pU); 

    conjArray = array_dup (fsmEntry -> obj.fsm.updateBdd);
    array_insert_last (mdd_t *, conjArray, notSpecAndU); 

    /* The computation of the fixpoint, part 1, is written: 
       Exists E'. not Exists C'. conjArray, where conjArray 
       has been smoothed, and where E' and C' are as computed 
       before.  It is computed in a few steps.  The result is 
       left in newU1. */ 

    resultMdd  = Img_MultiwayLinearAndSmooth (mgr, conjArray, pcontrEr, emptyArray);
    mdd_free     (notSpecAndU); 
    array_free   (conjArray); 
    newU1 =      mdd_not (resultMdd);
    mdd_free     (resultMdd); 

    /* The computation of the fixpoint, part 2, is written: 
       Exists E'. Exists Pr . Exists Pr' . (T_Q /\ newU1) 
       where Pr, Pr' are the sets of private vars of Q (the 
       controller module), and T_Q is the transition relation 
       of Q.  The union of these three sets of variables has been 
       precomputed, and is in controllerVars.  The conjunction 
       T_Q /\ newU1 is in controllingTrans. */ 

    controllingTrans = array_dup (fsmContrEntry -> obj.fsm.updateBdd);
    array_insert_last (mdd_t *, controllingTrans, newU1); 
    newU = Img_MultiwayLinearAndSmooth (mgr, controllingTrans, controllerVars, emptyArray); 
    mdd_free (newU1); 

    /* Ok!  Now prints info if requested. */ 

    if (mode) { 
      Main_MochaPrint ("*Conrollability iteration n. %d\n", niterations);
      SlPrintBddStats (newU, supportContr); 
    }
    sizeU = mdd_size (newU); 
    if (sizeU > maxsize) { 
      maxsize = sizeU; 
    }

  } while (!mdd_equal(U, newU)); 

  result = array_alloc (mdd_t *, 0); 
  array_insert_last (mdd_t *, result, U); 
  outEntry -> obj.bddArray = result; 

  mdd_free   (newU); 
  array_free (pcontrEr); 
  array_free (pextlNoner); 
  array_free (emptyArray); 
  array_free (supportContr); 

  /* I pring the n. of iterations to converge, not the n. of iterations to 
     detect convergence (which is bigger by 1). */ 
  Main_MochaPrint ("*Controllability computed in %d iterations.\n", niterations - 1);
  /* I also print the max MDD size that occurred. */ 
  Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}
  


/**Function********************************************************************

  Synopsis           [Function implementing command sl_reach.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlReachCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_reach <outBDD> <FSM> <s|v>";
  char          *fsmName, *outBddName;
  Sl_GSTEntry_t *fsmEntry, *outEntry;
  mdd_t         *R;
  int            mode; 

  util_getopt_reset();
  util_optind++;

  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }
  
  util_optind++;

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
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

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;

  R = SlComputeReach (fsmEntry, 0, NULL, mode); 
  outEntry -> obj.bddArray = SlSingleMddArray (R);
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
  return TCL_OK; 
}

/**Function********************************************************************

  Synopsis           [Function implementing command sl_reach_histonly.
                      Computes the set of reachable states, projected onto the 
		      history variables only.] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlReachHistoryCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_reach_histonly <outBDD> <FSM> <s|v>";
  char          *fsmName, *outBddName;
  Sl_GSTEntry_t *fsmEntry, *outEntry;
  mdd_t         *R;
  int            mode; 

  util_getopt_reset();
  util_optind++;

  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }
  
  util_optind++;

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
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

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;

  R = SlComputeReach (fsmEntry, 3, NULL, mode); 
  outEntry -> obj.bddArray = SlSingleMddArray (R);
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_checkinv.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlCheckInvCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_checkinv <FSM> <BDD> <s|v>";
  char          *fsmName, *bddName;
  Sl_GSTEntry_t *fsmEntry, *bddEntry;
  int            mode; 

  util_getopt_reset();
  util_optind++;
  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
    return TCL_OK;
  }
  
  util_optind++;

  bddName = argv[util_optind];
  if (!st_lookup(Sl_GST, bddName, (char **) &bddEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bddName);
    return TCL_OK;
  }
  if (bddEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bddName);
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

  SlComputeReach (fsmEntry, 1, bddEntry -> obj.bddArray, mode); 
  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_checktrinv.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlCheckTrInvCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_checktrinv <FSM> <BDD> <s|v>";
  char          *fsmName, *bddName;
  Sl_GSTEntry_t *fsmEntry, *bddEntry;
  int            mode; 

  util_getopt_reset();
  util_optind++;
  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
    return TCL_OK;
  }
  
  util_optind++;

  bddName = argv[util_optind];
  if (!st_lookup(Sl_GST, bddName, (char **) &bddEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bddName);
    return TCL_OK;
  }
  if (bddEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bddName);
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

  SlComputeReach (fsmEntry, 2, bddEntry -> obj.bddArray, mode); 
  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_checksim.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlCheckSimCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_checksim <FSM1> <FSM2> <s|v>";
  char          *fsmName1,  *fsmName2;
  Sl_GSTEntry_t *fsmEntry1, *fsmEntry2;
  int            mode; 

  util_getopt_reset();
  util_optind++;
  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName1 = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName1, (char **) &fsmEntry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName1);
    return TCL_OK;
  }
  if (fsmEntry1 -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName1);
    return TCL_OK;
  }
  
  util_optind++;

  fsmName2 = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName2, (char **) &fsmEntry2)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName2);
    return TCL_OK;
  }
  if (fsmEntry2 -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName2);
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

  /* First, checks implication between the initial conditions. */ 
  if (!SlImplicationCheck (fsmEntry1 -> obj.fsm.initBdd, 
			   fsmEntry2 -> obj.fsm.initBdd)) {
    Main_MochaErrorPrint ("*Simulation fails for initial condition.\n"); 
    return TCL_OK; 
  }
    
  /* Then, checks that the simulation holds for the rest of the computation. */ 
    
  SlComputeReach (fsmEntry1, 2, fsmEntry2 -> obj.fsm.updateBdd, mode); 
  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_initrand.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlInitRandCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_initrand <seed>";
  int            seed; 

  util_getopt_reset();
  util_optind++;
  if (argc != 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  seed = atoi (argv[util_optind]);
  
  /* CHECK: this is really a simplification.  It would be much better to 
     use the functions initstate, setstate, to ensure better randomization. 
     However, this is ok for a quick try. */ 
  srandom (seed); 

  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_checkrandinv.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlCheckRandInvCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_checkrandinv <FSM> <BDD> <max_iter> <max_size> \n \
   <exploration_algo> <shrink_algo> <s|v> \n \
       exploration_algo = 0 : with reinjection of initial condition \n \
       exploration_algo = 1 : without reinjection (like simulation)\n \
       shrink_algo = 0: pick at random a cube-complement \n \
       shrink_algo = 1: pick a cube with probability proportional to the states \n"; 
  char          *fsmName, *bddName;
  Sl_GSTEntry_t *fsmEntry, *bddEntry;
  int            maxIter, maxSize, mode; 
  int            exploration_algo, shrink_algo; 

  util_getopt_reset();
  util_optind++;
  if (argc != 8) {
    Main_MochaErrorPrint("%s", usage);
    return TCL_OK;
  }

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
    return TCL_OK;
  }
  
  util_optind++;

  bddName = argv[util_optind];
  if (!st_lookup(Sl_GST, bddName, (char **) &bddEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bddName);
    return TCL_OK;
  }
  if (bddEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bddName);
    return TCL_OK;
  }

  util_optind++;
  maxIter = atoi (argv[util_optind]); 

  util_optind++;
  maxSize = atoi (argv[util_optind]); 

  util_optind++;
  exploration_algo = atoi (argv[util_optind]); 

  util_optind++;
  shrink_algo = atoi (argv[util_optind]); 

  util_optind++;

  if (strcmp (argv[util_optind], "s") == 0) {
    mode = 0; 
  } else if (strcmp (argv[util_optind], "v") == 0) {
    mode = 1; 
  } else { 
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  switch (exploration_algo) {
  case 0: 
    computeRandReach (fsmEntry, 1, bddEntry -> obj.bddArray, maxSize, maxIter, shrink_algo, mode); 
    break; 
  case 1:
    computeRandSim (fsmEntry, 1, bddEntry -> obj.bddArray, maxSize, maxIter, shrink_algo, mode); 
    break; 
  }

  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_trinv.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlTrInvCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_trinv <outBDD> <FSM> <s|v>";
  char          *fsmName, *outBddName;
  Sl_GSTEntry_t *fsmEntry, *outEntry;
  mdd_t         *R;
  array_t       *trinv; 
  int            mode; 

  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }
  
  util_optind++;

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
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

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;

  R = SlComputeReach (fsmEntry, 0, NULL, mode); 
  trinv = array_dup (fsmEntry -> obj.fsm.updateBdd); 
  array_insert_last (mdd_t*, trinv, R); 
  outEntry -> obj.bddArray = trinv; 
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_contrreachspec.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlContrReachSpecCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_contrreachspec <outBDD> <inFSM> <inCtrlBDD> <inSpecBDD> <s|v>";
  char *fsmName, *specBddName, *outBddName, *ctrlBddName;
  Sl_GSTEntry_t *fsmEntry, *specEntry, *outEntry, *ctrlEntry;
  mdd_t   *resultMdd; 
  array_t *result; 
  int      mode; 

  util_getopt_reset();
  util_optind++;
  if (argc != 6) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }
  
  util_optind++;

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
    return TCL_OK;
  }
 
  util_optind++; 

  ctrlBddName = argv[util_optind];
  if (!st_lookup(Sl_GST, ctrlBddName, (char **) &ctrlEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", ctrlBddName);
    return TCL_OK;
  }
  if (ctrlEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", ctrlBddName);
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

  if (strcmp (argv[util_optind], "s") == 0) {
    mode = 0; 
  } else if (strcmp (argv[util_optind], "v") == 0) {
    mode = 1; 
  } else { 
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;
  
  resultMdd = computeControlReach (fsmEntry, 
				   ctrlEntry -> obj.bddArray, 
				   specEntry -> obj.bddArray,
				   NULL, 
				   mode); 

  outEntry -> obj.bddArray = SlSingleMddArray (resultMdd); 
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_contrreachfsm.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlContrReachFsmCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{


  char usage[] = "Usage: sl_contrreachfsm <outBDD> <inFSM> <inCtrlBDD> <inSpecFSM> <s|v>";
  char *fsmName, *specFsmName, *outBddName, *ctrlBddName;
  Sl_GSTEntry_t *fsmEntry, *specEntry, *outEntry, *ctrlEntry;
  mdd_t   *conj, *resultMdd; 
  array_t *transInvArray, *initCondArray, *result; 
  int      i; 
  int      mode; 

  util_getopt_reset();
  util_optind++;
  if (argc != 6) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }
  
  util_optind++;

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
    return TCL_OK;
  }
 
  util_optind++; 

  ctrlBddName = argv[util_optind];
  if (!st_lookup(Sl_GST, ctrlBddName, (char **) &ctrlEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", ctrlBddName);
    return TCL_OK;
  }
  if (ctrlEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", ctrlBddName);
    return TCL_OK;
  }

  util_optind++;

  specFsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, specFsmName, (char **) &specEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", specFsmName);
    return TCL_OK;
  }
  if (specEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not an FSM.\n", specFsmName);
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

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;
  
  /* puts in transInvArray the transition relation of the module, 
     smoothing away the variables that have been hidden. 
     Same for the initial condition. */ 

  transInvArray = SlSingleMddArray(SlSmoothConjoin (specEntry -> obj.fsm.updateBdd, 
						specEntry -> obj.fsm.erasedvarL)); 
  initCondArray = SlSingleMddArray(SlGetInit (specEntry)); 

  resultMdd = computeControlReach (fsmEntry, 
				   ctrlEntry -> obj.bddArray, 
	      			   transInvArray, 
				   initCondArray, 
				   mode); 

  /* now frees each MDD of the smoothed arrays, along with the arrays themselves. */ 

  arrayForEachItem(mdd_t*, transInvArray, i, conj) { 
    mdd_free (conj); 
  }
  array_free (transInvArray); 

  arrayForEachItem(mdd_t*, initCondArray, i, conj) { 
    mdd_free (conj); 
  }
  array_free (initCondArray); 

  /* puts the resulting MDD in the right form for the answer. */ 

  
  outEntry -> obj.bddArray = SlSingleMddArray (resultMdd); 
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [function implementing sl_erase] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlEraseCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char          usage[] = "Usage: sl_erase <outFSM> <inFSM> <var>...";
  char          *inFsmName, *outFsmName; 
  Sl_FSM_t      *inFsm, *outFsm; 
  Sl_GSTEntry_t *inEntry, *outEntry; 
  array_t       *bddIdArray;
  int           varid; 
  
  util_getopt_reset();
  util_optind++;
  if (argc < 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outFsmName = argv[util_optind];
  if (st_is_member(Sl_GST, outFsmName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outFsmName);
    return TCL_OK;
  }

  util_optind++;

  inFsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, inFsmName, (char **) &inEntry)) {
    Main_MochaErrorPrint("Error: FSM %s do not exists\n", inFsmName);
    return TCL_OK;
  }
  if (inEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: %s is not an FSM\n", inFsmName);
    return TCL_OK;
  }

  util_optind++;

  bddIdArray = array_alloc(int, 0);
  while(argc - util_optind != 0){
    varid = SlObtainUnprimedBddId(argv[util_optind]);
    if (varid == -1) {
      Main_MochaErrorPrint("Error: variable %s invalid\n",
                          argv[util_optind]);
      array_free(bddIdArray);
      return TCL_OK;
    }
    array_insert_last(int, bddIdArray, varid);
    util_optind++;
  }

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outFsmName);
  outEntry -> kind = Sl_FSM_c; 
  outEntry -> obj.fsm.initBdd    = inEntry -> obj.fsm.initBdd; 
  outEntry -> obj.fsm.updateBdd  = inEntry -> obj.fsm.updateBdd;
  outEntry -> obj.fsm.contrvarL  = inEntry -> obj.fsm.contrvarL; 
  outEntry -> obj.fsm.extvarL    = inEntry -> obj.fsm.extvarL;
  outEntry -> obj.fsm.erasedvarL = SlArrayUnion (inEntry -> obj.fsm.erasedvarL, bddIdArray); 
  array_free(bddIdArray);
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Function implementing command sl_and. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlAndCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_and <outBdd> <inBdd1> <inBdd2>";
  char *bdd1Name, *bdd2Name, *outBddName;
  Sl_GSTEntry_t *entry1, *entry2, *outEntry;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }

  util_optind++;

  bdd1Name = argv[util_optind];
  if (!st_lookup(Sl_GST, bdd1Name, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd1Name);
    return TCL_OK;
  }
  if (entry1 -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd1Name);
    return TCL_OK;
  }

  util_optind++;

  bdd2Name = argv[util_optind];
  if (!st_lookup(Sl_GST, bdd2Name, (char **) &entry2)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd2Name);
    return TCL_OK;
  }
  if (entry2 -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd2Name);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c; 

  /* Does the AND simply by concatenating the sequences of BDDs. */ 
  outEntry -> obj.bddArray = array_join (entry1 -> obj.bddArray, entry2 -> obj.bddArray);
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_or. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlOrCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_or <outBdd> <inBdd1> <inBdd2>";
  char *bdd1Name, *bdd2Name, *outBddName;
  Sl_GSTEntry_t *entry1, *entry2, *outEntry;
  mdd_t *temp1, *temp2; 
    
  util_getopt_reset();
  util_optind++;
  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }

  util_optind++;

  bdd1Name = argv[util_optind];
  if (!st_lookup(Sl_GST, bdd1Name, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd1Name);
    return TCL_OK;
  }
  if (entry1 -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd1Name);
    return TCL_OK;
  }

  util_optind++;

  bdd2Name = argv[util_optind];
  if (!st_lookup(Sl_GST, bdd2Name, (char **) &entry2)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd2Name);
    return TCL_OK;
  }
  if (entry2 -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd2Name);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c; 

  /* Does the OR by actually building the BDDs. */ 
  temp1 = SlConjoin (entry1 -> obj.bddArray); 
  temp2 = SlConjoin (entry2 -> obj.bddArray); 
  outEntry -> obj.bddArray = SlSingleMddArray(mdd_or (temp1, temp2, 1, 1)); 
  mdd_free (temp1); 
  mdd_free (temp2); 

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_not. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlNotCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_not <outBdd> <inBdd>";
  char *bdd1Name, *outBddName;
  Sl_GSTEntry_t *entry1, *outEntry;
  mdd_t *temp1;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }

  util_optind++;

  bdd1Name = argv[util_optind];
  if (!st_lookup(Sl_GST, bdd1Name, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd1Name);
    return TCL_OK;
  }
  if (entry1 -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd1Name);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c; 

  /* Does the NOT by actually building the BDDs. */ 
  temp1 = SlConjoin (entry1 -> obj.bddArray); 
  outEntry -> obj.bddArray = SlSingleMddArray(mdd_not (temp1)); 
  mdd_free (temp1); 

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_true. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlTrueCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_true <outBdd>";
  char *outBddName;
  Sl_GSTEntry_t *outEntry;
  mdd_t *temp1;
  mdd_manager *mgr; 

  mgr = SlGetMddManager ();
    
  util_getopt_reset();
  util_optind++;
  if (argc != 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c; 

  outEntry -> obj.bddArray = SlSingleMddArray (mdd_one (mgr)); 
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_restrict. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlRestrictCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_restrict <outFsm> <inBdd> <inFsm>";
  char *bddName, *fsmName, *outFsmName;
  Sl_GSTEntry_t *fsmEntry, *bddEntry, *outEntry;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outFsmName = argv[util_optind];
  if (st_is_member(Sl_GST, outFsmName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outFsmName);
    return TCL_OK;
  }

  util_optind++;

  bddName = argv[util_optind];
  if (!st_lookup(Sl_GST, bddName, (char **) &bddEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bddName);
    return TCL_OK;
  }
  if (bddEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bddName);
    return TCL_OK;
  }

  util_optind++;

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not an FSM.\n", fsmName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outFsmName);
  outEntry -> kind = Sl_FSM_c; 

  /* Copies all the information, except for the transition relation, 
     to which we also add the input bddArray. */ 

  outEntry -> obj.fsm.initBdd    = array_dup (fsmEntry -> obj.fsm.initBdd); 
  outEntry -> obj.fsm.contrvarL  = array_dup (fsmEntry -> obj.fsm.contrvarL); 
  outEntry -> obj.fsm.extvarL    = array_dup (fsmEntry -> obj.fsm.extvarL); 
  outEntry -> obj.fsm.erasedvarL = array_dup (fsmEntry -> obj.fsm.erasedvarL); 
  outEntry -> obj.fsm.updateBdd  = array_join (fsmEntry -> obj.fsm.updateBdd, 
					       bddEntry -> obj.bddArray); 

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_getinit. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetInitCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getinit <outBdd> <inFsm>";
  char *fsmName, *outName;
  Sl_GSTEntry_t *fsmEntry, *outEntry;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outName = argv[util_optind];
  if (st_is_member(Sl_GST, outName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outName);
    return TCL_OK;
  }

  util_optind++;

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not an FSM.\n", fsmName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outName);
  outEntry -> kind = Sl_BDD_c; 

  /* copies the initial condition into it, unprimed, 
     and in a single MDD, and returns. */ 
  outEntry -> obj.bddArray = SlSingleMddArray (SlGetInit (fsmEntry)); 
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Function implementing command sl_gettr. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetTRCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_gettr <outBdd> <inFsm>";
  char *fsmName, *outName;
  Sl_GSTEntry_t *fsmEntry, *outEntry;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outName = argv[util_optind];
  if (st_is_member(Sl_GST, outName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outName);
    return TCL_OK;
  }

  util_optind++;

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not an FSM.\n", fsmName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outName);
  outEntry -> kind = Sl_BDD_c; 

  /* copies the transition relation into it, 
     conjoined as a single MDD, and returns. */

  outEntry -> obj.bddArray = SlSingleMddArray (SlSmoothConjoin (fsmEntry -> obj.fsm.updateBdd, 
					     fsmEntry -> obj.fsm.erasedvarL)); 
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_bddsize. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlBddSizeCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_bddsize <inBdd>";
  char *bddName;
  Sl_GSTEntry_t *bddEntry;
  mdd_manager *mgr; 

  mgr = SlGetMddManager ();
    
  util_getopt_reset();
  util_optind++;
  if (argc != 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  bddName = argv[util_optind];
  if (!st_lookup(Sl_GST, bddName, (char **) &bddEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bddName);
    return TCL_OK;
  }
  if (bddEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bddName);
    return TCL_OK;
  }

  Main_MochaPrint ("*N. of BDD nodes = %d\n", (int) bdd_size_multiple (bddEntry -> obj.bddArray)); 
    
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Function implementing command sl_predsize. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPredSizeCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_predsize <inFsm> <inBdd>";
  char          *fsmName, *bddName;
  Sl_GSTEntry_t *fsmEntry, *bddEntry;
  mdd_manager   *mgr; 
  array_t       *tempArray, *supportVars, *emptyArray; 
  mdd_t         *singlePred; 

  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
    return TCL_OK;
  }
  
  util_optind++;

  bddName = argv[util_optind];
  if (!st_lookup(Sl_GST, bddName, (char **) &bddEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bddName);
    return TCL_OK;
  }
  if (bddEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bddName);
    return TCL_OK;
  }

  /* Prepares the list of variables for the support. */ 
  /* supportVars = (controlled + external) \ erased.  Used to check support, and 
     count of states that satisfy the predicate. */ 
  tempArray   = array_join      (fsmEntry -> obj.fsm.contrvarL, fsmEntry->obj.fsm.extvarL); 
  supportVars = SlArrayDifference (tempArray, fsmEntry->obj.fsm.erasedvarL); 
  array_free    (tempArray); 
  emptyArray  = array_alloc (int, 0); 

  /* Prepares a single predicates, and prints its support. */ 
  singlePred  = SlSmoothConjoin (bddEntry -> obj.bddArray, emptyArray); 
  SlPrintBddStats (singlePred, supportVars); 
  
  array_free (emptyArray); 
  array_free (supportVars);
  mdd_free (singlePred); 
  return TCL_OK; 
}

/**Function********************************************************************

  Synopsis           [Function implementing command sl_maketrinv. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlMakeTrInvCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_maketrinv <outBdd> <inBdd>";
  char          *bdd1Name, *outBddName;
  Sl_GSTEntry_t *entry1, *outEntry;
  mdd_t         *conj; 
  array_t       *primedArray; 
  int           i; 
    
  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }

  util_optind++;

  bdd1Name = argv[util_optind];
  if (!st_lookup(Sl_GST, bdd1Name, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd1Name);
    return TCL_OK;
  }
  if (entry1 -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd1Name);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c; 

  /* Constructs another MDD array, containing the same conjuncts
     as the given one, but primed. */ 
  primedArray = array_alloc (mdd_t*, 0);
  arrayForEachItem(mdd_t*, entry1 -> obj.bddArray, i, conj) { 
    array_insert_last (mdd_t *, primedArray, SlPrimeMdd (conj)); 
  }

  /* Inserts result and returns. */ 
  outEntry -> obj.bddArray = array_join (entry1 -> obj.bddArray, primedArray); 
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
  array_free (primedArray); 
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_control. 
                      This is general controllability for Mealy 
		      modules.  The input arguments are: 
		      outBDD: the output BDD
		      specBDD: the specification BDD.
		      FSM: the FSM that is being controlled.
		      VarOrd: the variable ordering being used. 
		      ErasedVars: list of erased variables. 
		      And of course, a silent/verbose flag. 
		      ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int
SlControlCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_control <outBDD> <specBDD> <VarOrd> <ErasedVars> <FSM> <s|v>";

  char *fsmName, *specBddName, *outBddName, *VarOrdName, *erasedName;
  char *orderingName; 
  Sl_GSTEntry_t *fsmEntry, *specEntry, *outEntry, *varOrdering, *erasedVars;
  mdd_manager *mgr;
  int     mode;   /* 0 = silent, 1 = verbose */ 
  mdd_t   *Spec, *PSpec, *NPSpec, *notObjectv, *Contr, *NContr, *QContr;
  mdd_t   *FinalContr; 
  array_t *conjArray, *existential_vars, *universal_vars; 
  array_t *emptyArray, *forallVars, *quant_var_list, *result; 
  array_t *controllerVars, *ordering, *erased; 
  int     var, n_vars, var_index, top_index, bottom_index, first_block_end;
  int     polarity, new_polarity;

  util_getopt_reset();
  util_optind++;
  if (argc != 7) {
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

  orderingName = argv[util_optind];
  if (!st_lookup(Sl_GST, orderingName, (char **) &varOrdering)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", orderingName);
    return TCL_OK;
  }
  if (varOrdering -> kind != Sl_IdArray_c) {
    Main_MochaErrorPrint("Error: Object %s is not a list of variables.\n", orderingName);
    return TCL_OK;
  }

  util_optind++;

  erasedName = argv[util_optind];
  if (!st_lookup(Sl_GST, erasedName, (char **) &erasedVars)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", erasedName);
    return TCL_OK;
  }
  if (erasedVars -> kind != Sl_IdArray_c) {
    Main_MochaErrorPrint("Error: Object %s is not a list of variables.\n", erasedName);
    return TCL_OK;
  }

  /* end of var lists, back to our normal business... */ 

  util_optind++;

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
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

  /* --------- finished reading the input. */

  /* 

     To do: 

     1) universal and existential are lists of unprimed vars. 
     Hence, I must prime from ordering before applying quantification. 

     2) Throw away from the transition relation the irrelevant conjoints. 

     3) Can we do better the initial quantification block in the two 
     forall, exists cases? 

  */ 

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;
  
  mgr = SlGetMddManager ();

  ordering = varOrdering -> obj.idArray; 
  erased   = erasedVars  -> obj.idArray; 

  /* Computes the conjunction Spec = SpecConj1 /\ ... /\ SpecConjn, 
     where SpecConj's are the pieces of the specification. */

  Spec  = SlConjoin (specEntry->obj.bddArray);
  PSpec = SlPrimeMdd (Spec);
  mdd_free (Spec);

  /* If the transition relation is T_P = t1 /\ ... /\tn, 
     creates in conjArray an array of BDDs 
     t1, ..., tn, not (Spec')  */ 

  NPSpec = mdd_not (PSpec); 
  mdd_free (PSpec); 
  conjArray = array_dup (fsmEntry -> obj.fsm.updateBdd);
  array_insert_last (mdd_t *, conjArray, NPSpec); 
  
  /* The computation of the result is now: 
     Exists W . Exists / Forall x . conjArray , 
     where the polarity of x in the variable ordering depends 
     on who owns it, and on whether it has been deleted or not. 
     This computation takes various steps. */ 

  /* Computes the lists of universal and existential variables. */ 
  /* FIXME this assumes that we work with unprimed versions, 
     and that we only prime variables here to compute the quantification. */ 

  existential_vars = array_dup (erased); 
  array_append (existential_vars, fsmEntry -> obj.fsm.extvarL);
  universal_vars = SlArrayDifference (fsmEntry -> obj.fsm.contrvarL, erased);

  n_vars = array_n (ordering); 

  /* The innermost Forall-block is special, because it can be
     computed as not Exists not conjArray.  The rest will have
     to be computed after conjArray has been merged in a single
     mdd. */ 
  /* Finds the limits of this first block. */ 
  /* polarity: 0 = exists, 1 = forall */ 

  for (var_index = n_vars - 1; 1; var_index --) { 
    if (var_index < 0) break; /* I don't think it can happen */ 
    var = array_fetch (int, ordering, var_index); 
    polarity = get_polarity (var, universal_vars, existential_vars);
    if (polarity == 0) break; 
  }
  first_block_end = var_index + 1; 

  /* Ok, now constructs forallVars, which is the first block. */ 
  forallVars = array_alloc (int, 0);
  for (var_index = n_vars - 1; var_index >= first_block_end; var_index --) {
    var = array_fetch (int, ordering, var_index);
    array_insert_last (int, forallVars, var); 
  }

  /* Computes Forall forallVars . (tau -> Spec'), as 
     not Exists forallVars . conjArray */ 

  emptyArray = array_alloc (int, 0);
  notObjectv = Img_MultiwayLinearAndSmooth(mgr, conjArray, forallVars, emptyArray);
  array_free (emptyArray); 
  array_free (forallVars); 
  array_free (conjArray); 
  Contr = mdd_not (notObjectv); 
  mdd_free (notObjectv); 

  /* ok, now the other quantification must be done.  Gathers the 
     variables in groups with the same polarity for efficiency. */ 
     
  top_index = first_block_end - 1; 
  while (top_index >= 0) { 

    /* remembers the polarity of this variable. 0 = exists, 1 = forall. */ 
    var = array_fetch (int, ordering, top_index); 
    polarity = get_polarity (var, universal_vars, existential_vars);
    /* now determines how many more contiguous variables have the same 
       polarity. */ 
    bottom_index = top_index; 
    do { 
      bottom_index --; 
      if (bottom_index >= 0) { 
	var          = array_fetch (int, ordering, bottom_index);
	new_polarity = get_polarity (var, universal_vars, existential_vars);
      }
    } while (bottom_index >= 0 && new_polarity == polarity); 
    /* I went one too low */ 
    bottom_index ++; 

    /* Now quantifies from top to bottom. */ 

    quant_var_list = array_alloc (int, 0); 
    for (var_index = top_index; var_index >= bottom_index; var_index --) {
      var = array_fetch (int, ordering, var_index);
      array_insert_last (int, quant_var_list, var); 
    }
    if (polarity) { 
      /* forall */ 
      NContr = mdd_not (Contr); 
      mdd_free (Contr); 
      QContr = mdd_smooth (mgr, NContr, quant_var_list); 
      mdd_free (NContr); 
      Contr  = mdd_not (QContr); 
      mdd_free (QContr); 
    } else { 
      /* exists */ 
      NContr = mdd_smooth (mgr, Contr, quant_var_list);
      mdd_free (Contr); 
      Contr  = NContr; 
    }
    array_free (quant_var_list); 
    
    /* now, all the variables down to bottom_index have been taken care of. */ 
    top_index = bottom_index - 1;
  }

  /* Ok, before we are done, we need also to existentially quantify 
     the (unprimed) erased variables. */ 

  FinalContr = mdd_smooth (mgr, Contr, erased);
  mdd_free (Contr); 
  array_free (existential_vars); 
  array_free (universal_vars); 
  if (mode) { 
    Main_MochaPrint ("Final BDD Size = %d\n", mdd_size (FinalContr)); 
  }
  result = array_alloc (mdd_t *, 0); 
  array_insert_last (mdd_t *, result, FinalContr); 
  outEntry -> obj.bddArray = result; 
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
  return TCL_OK;
}



/**Function********************************************************************

  Synopsis           [Prints a list of variables of an fsm.] 
                      Input: the fsm, and: 
		      'erased', 'controlled', or 'external'. 
		      ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int
SlGetFsmVarsCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getfsmvars <FSM> <'erased'/'controlled'/'external'>";

  char *fsmName;
  array_t *var_list; 
  Sl_GSTEntry_t *fsmEntry;

  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &fsmEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (fsmEntry -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
    return TCL_OK;
  }

  util_optind++;

  if (strcmp (argv[util_optind], "erased") == 0) {
    var_list = fsmEntry -> obj.fsm.erasedvarL;
  } else if (strcmp (argv[util_optind], "controlled") == 0) {
    var_list = fsmEntry -> obj.fsm.contrvarL;
  } else if (strcmp (argv[util_optind], "external") == 0) { 
    var_list = fsmEntry -> obj.fsm.extvarL; 
  } else { 
    Main_MochaErrorPrint ("Error: %s\n", usage); 
    return TCL_OK; 
  }

  SlTclPrintVarNames (var_list); 

  return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Make an array_t out of the names of variables. 
		      ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int
SlMakeVarListCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_makevarlist <varlist> <var1> <var2> ...";

  char *varlistName;
  array_t *var_list;
  Sl_GSTEntry_t *outEntry;
  int varid; 

  util_getopt_reset();
  util_optind++;
  if (argc < 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  varlistName = argv[util_optind];
  if (st_is_member(Sl_GST, varlistName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", varlistName);
    return TCL_OK;
  }

  util_optind++;

  var_list = array_alloc(int, 0);
  while(argc - util_optind != 0){
    varid = SlObtainBddId(argv[util_optind]);
    if (varid == -1) {
      Main_MochaErrorPrint("Error: variable %s invalid\n",
                          argv[util_optind]);
      array_free(var_list);
      return TCL_OK;
    }
    array_insert_last(int, var_list, varid);
    util_optind++;
  }

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(varlistName);
  outEntry -> kind = Sl_IdArray_c; 
  outEntry -> obj.idArray = var_list; 

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  return TCL_OK; 
}

/**Function********************************************************************

  Synopsis           [Function implementing command sl_cojoin_and_copy. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlConjoinAndCopyCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_conjoin_and_copy <outBdd> <inBdd1> <inBdd2>";
  char *bdd1Name, *bdd2Name, *outBddName;
  Sl_GSTEntry_t *entry1, *entry2, *outEntry;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }

  util_optind++;

  bdd1Name = argv[util_optind];
  if (!st_lookup(Sl_GST, bdd1Name, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd1Name);
    return TCL_OK;
  }
  if (entry1 -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd1Name);
    return TCL_OK;
  }

  util_optind++;

  bdd2Name = argv[util_optind];
  if (!st_lookup(Sl_GST, bdd2Name, (char **) &entry2)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", bdd2Name);
    return TCL_OK;
  }
  if (entry2 -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", bdd2Name);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c; 

  /* Does the AND simply by concatenating the sequences of BDDs. */ 
  {
    array_t * tmpArray =
        array_join (entry1 -> obj.bddArray, entry2 -> obj.bddArray);
    outEntry -> obj.bddArray = SlBddArrayDupe(tmpArray);
    array_free(tmpArray);
  }
  
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Decides the polarity of a variable.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static int get_polarity (int var, array_t *universal_vars, array_t *existential_vars)
{
  int i, v, id; 


  arrayForEachItem(int, universal_vars, i, v) {
    if (var == v) { 
      return 1; 
    }
  }
  arrayForEachItem(int, existential_vars, i, v) {
    if (var == v) { 
      return 0; 
    }
  }
  Main_MochaErrorPrint ("Error: some variable is neither forall nor exists.\n"); 
  return 0; /* boh? */ 
}


/**Function********************************************************************

  Synopsis           [Computes the controllable and reachable states of 
                      a module. Gets parameters P C S I M, where P is an fsmEntry 
		      to the symbol table containing the module, C is an 
		      array containing the controllable states predicate, 
		      S is an array containing the specification 
		      (can be either an invariant, or a transition invariant), 
		      I is an array containing an initial condition, 
		      and M is a mode. 
		      The array I is used if the goal is to check controllability
		      with respect to an implementation relation, and it is used
		      to construct the transition relation.  
		      The mode M can be 0 for silent, and 1 for verbose.]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

static mdd_t * 
computeControlReach (
		     Sl_GSTEntry_t *fsmEntry, 
		     array_t       *ctrlArray, 
		     array_t       *specArray, 
		     array_t       *initArray,
		     int            mode)
{

  mdd_t       *contrPred, *specPred, *initPred; /* of the specification FSM */ 
  mdd_t       *primedContrPred; 
  mdd_t       *initCond;                        /* of the FSM being controlled */ 
  mdd_manager *mgr;
  mdd_t       *initAndNotContr, *smoothInitAndNotContr; 
  mdd_t       *smoothInitCond, *primedNewS, *newExternal, *notSpecAndContr; 
  mdd_t       *temp1; 
  mdd_t       *V, *newV; 
  array_t     *contrExtl, *erPr, *contrErPr;            /* temporary variables */ 
  array_t     *contrEr, *contrExtlErPr, *contrPrErErPr; /* freed at the end */ 
  array_t     *allowedUnpSupp, *allowedPrmSupp, *allowedSupport; 
  array_t     *conjArray, *emptyArray; 
  int          niterations; 
  int          maxsize; 
  int          sizeV; 

  mgr = SlGetMddManager ();

  /* If some variable of P that is erased appears in C or S or I, 
     then it complains. */ 

  /* contrExtl = (controlled \union external) */ 
  contrExtl      = array_join(fsmEntry -> obj.fsm.contrvarL, 
			      fsmEntry -> obj.fsm.extvarL);  

  allowedUnpSupp = SlArrayDifference (contrExtl, fsmEntry -> obj.fsm.erasedvarL); 
  allowedPrmSupp = SlPrimeVars (allowedUnpSupp); 
  allowedSupport = array_join (allowedUnpSupp, allowedPrmSupp); 

  /* Builds the controllable states predicate contrPred. */ 
  contrPred = SlConjoin (ctrlArray); 
  SlCheckSupportSubset (contrPred, allowedSupport, 
		      "ERL02: ctrl predicate with too large support!"); 
  /* ...and its primed version. */ 
  primedContrPred = SlPrimeMdd (contrPred); 

  /* Builds the specification predicate specPred. */ 
  specPred = SlConjoin (specArray); 
  SlCheckSupportSubset (specPred, allowedSupport, 
		      "ERL03: spec predicate with too large support!"); 

  /* Builds the initial predicate of the specification FSM. */ 
  if (initArray == NULL) { 
    initPred = mdd_one (mgr); 
  } else { 
    initPred = SlConjoin (initArray); 
    SlCheckSupportSubset (initPred, allowedUnpSupp, 
			"ERL04: init predicate with too large support!"); 
  }

  /* Builds the initial predicate of the controlled FSM. */ 
  initCond = SlGetInit (fsmEntry); 

  /* Computes the sets 
     contrEr       = (controlled  \union erased), 
     contrExtlErPr = (controlled  \union external \union \erased'), 
     contrPrErErPr = (controlled' \union erased \union erased'). */

  contrEr       = array_join (fsmEntry -> obj.fsm.contrvarL, 
			      fsmEntry -> obj.fsm.erasedvarL); 
  erPr          = SlPrimeVars  (fsmEntry -> obj.fsm.erasedvarL); 
  contrExtlErPr = array_join (contrExtl, erPr); 
  contrErPr     = SlPrimeVars  (contrEr); 
  contrPrErErPr = array_join (contrErPr, fsmEntry -> obj.fsm.erasedvarL); 
  array_free                 (contrExtl); 
  array_free                 (erPr); 
  array_free                 (contrErPr); 

  /* Time to build the initial V_0.  This takes several steps.  First, 
     we define it as := not exists contrEr. [init_fsm /\ not contrPred] */
  
  initAndNotContr       = mdd_and (initCond, contrPred, 1, 0); 
  smoothInitAndNotContr = mdd_smooth (mgr, initAndNotContr, contrEr);  
  mdd_free (initAndNotContr); 

  /* Now it smoothes from initCond the erased variables, and conjons the result 
     to not smoothInitAndNotContr. */ 

  smoothInitCond = mdd_smooth (mgr, initCond, fsmEntry -> obj.fsm.erasedvarL); 
  temp1 = mdd_and (smoothInitCond, smoothInitAndNotContr, 1, 0); 
  mdd_free (smoothInitAndNotContr);
  mdd_free (smoothInitCond);

  /* Finally, takes the conjunction with initPred (which might be = true). 
     Leaves the result in newV. Also puts "true" in V. */ 

  newV = mdd_and  (temp1, initPred, 1, 1); 
  mdd_free (temp1); 
  V    = mdd_one (mgr); 

  if (mode) { 
    SlPrintBddStats (newV, allowedUnpSupp); 
  }
  maxsize = mdd_size (newV); 
  emptyArray  = array_alloc(int, 0);
  niterations = 0; 

  /* debug
     Main_MochaPrint ("*Initial newV:\n");
     SlMddPrintCubes (newV, 0); */ 

  /* Now, the loop that iterates the construction of the controllable 
     and reachable states. */ 

  do { 
    mdd_free (V); 
    V = newV; 
    niterations++; 

    /* inner quantification: this specifies the next value for 
       the external vars.  Leaves the result in newExternal. */ 
    conjArray = array_dup (fsmEntry -> obj.fsm.updateBdd);
    notSpecAndContr = mdd_or (specPred, primedContrPred, 0, 0); 
    array_insert_last (mdd_t*, conjArray, notSpecAndContr); 
    temp1 = Img_MultiwayLinearAndSmooth(mgr, conjArray, contrPrErErPr, emptyArray); 
    newExternal = mdd_not (temp1);
    mdd_free (temp1); 
    mdd_free (notSpecAndContr);
    array_free (conjArray); 

    /* debug
       Main_MochaPrint ("*New value for external vars:\n");
       SlMddPrintCubes (newExternal, 0); */ 

    /* now the outer quantification, leaves the result in primedNewS. */ 
    conjArray = array_dup (fsmEntry -> obj.fsm.updateBdd);
    array_insert_last (mdd_t*, conjArray, newExternal);
    array_insert_last (mdd_t*, conjArray, V); 
    primedNewS = Img_MultiwayLinearAndSmooth(mgr, conjArray, contrExtlErPr, emptyArray); 
    mdd_free (newExternal); 
    array_free (conjArray); 
    
    /* debug
       Main_MochaPrint ("*New states added:\n"); 
       SlMddPrintCubes (primedNewS, 0); */ 

    /* newV = V or unprimed primedNewS */
    temp1 = SlUnprimeMdd (primedNewS); 
    mdd_free (primedNewS); 
    newV = mdd_or (V, temp1, 1, 1); 
    mdd_free (temp1); 

    if (mode) { 
      Main_MochaPrint ("*Controllability+Reachability Iteration n. %d\n", niterations);
      SlPrintBddStats (newV, allowedUnpSupp); 
    }
    sizeV = mdd_size (newV); 
    if (sizeV > maxsize) { 
      maxsize = sizeV; 
    }

    /* debug
       Main_MochaPrint ("*New newV:\n");
       SlMddPrintCubes (newV, 0); */ 

  } while (!mdd_equal(V, newV));

  mdd_free (newV); 
  Main_MochaPrint ("*Controllability+Reachability computed in %d iterations.\n", 
		   niterations - 1);
  /* I also print the max MDD size that occurred. */ 
  Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 
  
  mdd_free (contrPred); 
  mdd_free (specPred); 
  mdd_free (initPred); 
  mdd_free (initCond); 
  mdd_free (primedContrPred); 

  array_free (contrEr); 
  array_free (contrExtlErPr); 
  array_free (contrPrErErPr); 
  array_free (emptyArray); 

  array_free (allowedUnpSupp); 
  array_free (allowedPrmSupp); 
  array_free (allowedSupport); 
  
  return (V); 
}


/**Function********************************************************************

  Synopsis           [Computes the set of reachable states, and checks during the 
                      computation that one of these three conditions holds, 
		      depending on the value of the input variable task: 
		      task=0: does not check anything. 
		      task=1: checks that the reachable states imply a formula
		      task=2: checks that a given transition invariant holds. 
		      task=3: does not check anything. 
		      If task = 0, returns the reachable states. 
		      If task = 3, returns the set of reachable states projected
		      on the history-ful variables. 
		      Otherwise, returns NULL. 
		      Inputs: 
		      task:      see above. 
		      inFsm:     the FSM that has to be thus checked. 
		      invariant: the (transition?) invariant to be checked. 
		      mode:  0 = silent (only max data), 1 = verbose 

		      Revision: Dec 18, 2000, improved frontier computation -lda

		      Revision: Dec 20, 2002, erased variables in
                      erasedvarlist for reachability. -lda

		      Revision: Dec 21, 2002. Restored backward compatibility 
		      by adding task 3.  Also now it figures out alone what 
		      are the history-free variables.
		      
		      ]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

mdd_t* 
SlComputeReach (
	      Sl_GSTEntry_t *inFsm, 
	      int            task, 
	      array_t       *invariant, /* array of mdd_t * */ 
	      int            mode) 
{
  int      niterations = 0; 
  array_t *allvars, *to_quantify_out0, *to_quantify_out1, *tempArray;
  array_t *emptyArray, *conjArray; 
  array_t *history_free, *vars_in_invariant; 
  array_t *temp_array, *temp_array1; 
  mdd_t   *newImage, *unpImage, *conj; 
  mdd_t   *R, *newR, *zero, *globbedR;  
  mdd_manager *mgr;
  int     outcome=1; 
  int     maxsize, sizeR; 
  int     i, id, size, maxAllSize, maxInvariantSize;
  int     maxManagerSize;

  /* BEGIN SKIP */
  /* This stuff is just to keep track of the size of the BDDs. */  
  maxInvariantSize = maxAllSize = 0;
  maxManagerSize =0;
  /* END SKIP */ 
 
  emptyArray = array_alloc (int, 0);
  /* This is arcane code to get the "BDD manager"; just copy it. */ 
  mgr = SlGetMddManager ();

  /* This is the set of all variables of the module */ 
  /* allvars = controlled (output) + external (input) */ 
  allvars         = array_join (inFsm -> obj.fsm.contrvarL, inFsm->obj.fsm.extvarL); 

  /* Figures out what are the history-free variables, considering the module in 
     isolation (how else?). */ 
  history_free = SlHistoryFree (allvars, inFsm -> obj.fsm.updateBdd); 
  /* diagnostics */ 
  if (mode) { 
    temp_array = SlArrayDifference (allvars, history_free); 
    Main_MochaPrint ("History variables:\n"); 
    SlPrintVarNames (temp_array); 
    array_free (temp_array); 
  }
  /* Generates the list of state variables that can be quantified out, 
     since they are history-free. */ 
  if (task == 0 || task == 3) { 
    /* It must only compute the reachable states. 
       It can erase all history-free variables. */ 
    temp_array = SlPrimeVars (history_free); 
    to_quantify_out0 = array_join (allvars, temp_array); 
    /* I hate to have special cases when freeing */ 
    to_quantify_out1 = array_alloc (int, 0); 
    array_free (temp_array); 
  } else { 
    /* Figures out which variables are in the invariant. 
       These are the UNPRIMED variables in the invariant. */ 
    /* All the variables in the invariant */ 
    vars_in_invariant = array_alloc (int, 0); 
    arrayForEachItem(mdd_t*, invariant, i, conj) { 
      temp_array = mdd_get_support (mgr, conj); 
      Main_MochaPrint ("Support size: %d\n", array_n (temp_array)); 
      array_append (vars_in_invariant, temp_array); 
      array_free (temp_array); 
    }
    /* Only the unprimed vars in the invariant */ 
    temp_array = array_alloc (int, 0); 
    arrayForEachItem (int, vars_in_invariant, i, id) {
      if (SlReturnPrimedId(id) != id) {
	/* variable is unprimed */ 
	array_insert_last (int, temp_array, id); 
      }
    }
    array_free (vars_in_invariant); 
    vars_in_invariant = temp_array; 
    /* Eliminates duplicates */ 
    array_sort (vars_in_invariant, SlCompareInt);
    array_uniq (vars_in_invariant, SlCompareInt, 0);

    /* Invariant, regular or transition.  
       All the history-free variables must eventually be 
       quantified out, but we:
       - first quantify out the history-free ones that do not appear in the invariant, 
         in to_quantify_out1, 
       - then we unprime the formula for the next states,
       - then we check the invariant, 
       - and last we quantify out the history-free variables that do appear in the invariant, 
         which are in to_quantify_out1 (and are unprimed) */ 
    /* round 0 */
    temp_array       = SlArrayDifference (history_free, vars_in_invariant);
    to_quantify_out0 = SlPrimeVars (temp_array); 
    array_free       (temp_array); 
    array_append     (to_quantify_out0, allvars); 
    /* round 1 */ 
    to_quantify_out1 = SlArrayIntersection (history_free, vars_in_invariant);
  } 
  array_free (history_free); 
  /* end of variable list initialization for history-free variables */ 

  /* This line initializes the BDD newR to the BDD representing 
     the set of initial states of the module. */ 
  newR = SlGetInit (inFsm); 
  /* old sets of states, set to the empty set */ 
  R    = mdd_zero (mgr); 
  zero = mdd_zero (mgr); 

  /* Prints, or stores, size of initial condition. */ 
  if (mode) {
      /* prints how big the bdd is */ 
    SlPrintBddStats (newR, allvars); 
  }

  /* BEGIN SKIP */ 
  /* CMPE293: don't worry about this: it's just to keep track of the 
     maximum size of BDDs */ 
  maxsize = mdd_size (newR); 
  maxAllSize = computeMaxTotalSize(maxAllSize, maxsize, invariant, task);
  {   /* added by freddy */
    maxManagerSize = maxManagerSize > bdd_read_node_count(SlGetMddManager())?
        maxManagerSize: bdd_read_node_count(SlGetMddManager());
  }
  /* END SKIP */ 
  
  /* checks the initial condition. */ 
  if (task == 1) { 
    /* CMPE293: You should also do this; task == 1 is your case */ 
    /* makes an array containing newR as single BDD, for input
       to SlImplicationCheck below */ 
    conjArray = SlSingleMddArray (newR); 
    /* SlImplicationCheck takes as input two arrays of BDDs, whose
       elements are conjoined, and checks the implication. */ 
    outcome = SlImplicationCheck (conjArray, invariant);
    array_free (conjArray); 

  /* BEGIN SKIP: THIS STUFF IS FOR TRANSITION INVARIANT CHECKS */ 
  } else if (task == 2) { 
    conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
    array_insert_last (mdd_t*, conjArray, newR); 
    outcome = SlImplicationCheck (conjArray, invariant); 
    array_free (conjArray); 
  /* END SKIP */ 
  } 
 
  /* If the invariant failed, complains. */ 
  if (!outcome) { 
    Main_MochaPrint ("*Invariant failed for initial condition.\n"); 
    mdd_free   (R); 
    mdd_free   (newR); 
    array_free (emptyArray); 
    array_free (allvars); 
    array_free (to_quantify_out0); 
    array_free (to_quantify_out1); 
    return (NULL); 
  }

  /* now loops, producing the reachable states. */ 
  do { 
    niterations++; 
    /* This puts in conjArray the transition relation tau of the module */ 
    conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
    /* then, we add the frontier, computing newR /\ tau */ 
    array_insert_last (mdd_t *, conjArray, newR); 
    /* This command puts in newImage the BDD corresponding to 
       exists allvars . (newR /\ tau)
       that, as you know, consists only of primed (next-state) variables. */ 
    newImage  = Img_MultiwayLinearAndSmooth(mgr, conjArray, to_quantify_out0, emptyArray); 
    size = mdd_size (newImage); 
    if (size > maxsize) { 
      maxsize = size; 
    }
    array_free  (conjArray); 
      /* And now, just as we saw in class, we unprime the newImage, to get
       the new states expressed in terms of the normal state variables */ 
    /* CMPE293: if you need the converse, do: slPrimeMdd (Image). */ 
    unpImage  = SlUnprimeMdd (newImage); 
    mdd_free (newImage);

    /* now that the new image in in unpImage, we can test it as we did 
       for the initial condition.  I believe it is more efficient to 
       check the new image, than newR, if not else because we avoid 
       doing the OR. */ 
    if (task == 1) {
      /* CMPE293: This is your case. Checks whether the frontier
         satisfies the invariant, similarly to what was done
         for the initial condition. */ 
      conjArray = SlSingleMddArray (unpImage); 
      outcome   = SlImplicationCheck (conjArray, invariant);
      array_free (conjArray); 
    /* BEGIN SKIP */ 
    /* CMPE 293: this is the case for transition invariants */ 
    } else if (task == 2) { 
      conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
      array_insert_last (mdd_t*, conjArray, unpImage); 
      outcome = SlImplicationCheck (conjArray, invariant); 
      array_free (conjArray); 
    } 
    /* END SKIP */ 

    /* Ok, time to quantify away the extra variables. 
       It's not all that useful, but better to do it now than later
       when the new image is computed. */ 
    if (task == 1 || task == 2) { 
      conjArray = SlSingleMddArray (unpImage); 
      conj = Img_MultiwayLinearAndSmooth(mgr, conjArray, to_quantify_out1, emptyArray); 
      array_free (conjArray); 
      mdd_free (unpImage); 
      unpImage = conj; 
    }

    /* BEGIN SKIP */ 
    /* CMPE 293: this stuff is again to keep track of sizes of BDDs */ 
    maxAllSize = computeMaxTotalSize(maxAllSize, sizeR, invariant, task);
    /* added by freddy */
    {
      maxManagerSize = maxManagerSize > bdd_read_node_count(SlGetMddManager())?
          maxManagerSize: bdd_read_node_count(SlGetMddManager());
      
    }
    /* END SKIP */ 

    if (!outcome) { 
      Main_MochaPrint ("*Invariant failed at iteration %d.\n", niterations); 
      /* I also print the max MDD size that occurred. */ 
      Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 
       /* I also print the max of sum MDD (reachability + invariant) sizes */
      Main_MochaPrint ("*MaxTotal MDD size = %d\n", maxAllSize);
      /* I also print the max of sum invariant (invariant) sizes */
      Main_MochaPrint ("*MaxInvariant MDD size = %d\n", maxInvariantSize);
      Main_MochaPrint ("*MaxManager MDD size = %d\n", maxManagerSize);
      mdd_free   (R); 
      mdd_free   (unpImage); 
      array_free (emptyArray); 
      array_free (allvars); 
      array_free (to_quantify_out0); 
      array_free (to_quantify_out1); 
      return (NULL); 
    }

    /* Now recomputes the frontier */ 
    /* This puts globbedR = R \/ newR, and then copies globbedR into R.
       This memory allocation stuff is no fun. */ 
    /* The functions mdd_or(P,Q,nP,nQ) works as follows. 
       First, it computes PP = P if nP = 1, and PP = not P if nP = 0. 
       This means that nP is a flag that indicates whether P is to 
       be used as is (1) or complemented (0). 
       Similarly, it computes QQ = Q if nQ = 1, and QQ = not Q if nQ = 0. 
       Finally, it returns PP \/ QQ. 
       mdd_and, used below, works in a similar way. */ 
    globbedR = mdd_or (R, newR, 1, 1); 
    mdd_free (newR); 
    mdd_free (R); 
    R        = globbedR;
    /* sizes */ 
    sizeR = mdd_size (R); 
    if (sizeR > maxsize) { 
      maxsize = sizeR; 
    }
    /* newR is the new frontier. It is computed as: 
       newR = unpImage /\ not R
       note the use of the flags explained above */ 
    newR     = mdd_and (unpImage, R, 1, 0); 
    mdd_free (unpImage); 
    
    if (mode) { 
      Main_MochaPrint ("*Reachability Iteration n. %d\n", niterations);
      SlPrintBddStats (R, allvars);
    }

    /* Again stuff used to check the size */ 
    sizeR = mdd_size (R); 
    if (sizeR > maxsize) { 
      maxsize = sizeR; 
    }

    /* This is how to test for equality of BDDs */ 
  } while (!mdd_equal(zero, newR));
  
  /* I print the n. of iterations to converge, not the n. of iterations to 
     detect convergence (which is bigger by 1). */ 
  Main_MochaPrint ("*Reachability computed in %d iterations.\n", niterations - 1);
  /* I also print the max MDD size that occurred. */ 
  Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 
  Main_MochaPrint ("*MaxManager MDD size = %d\n", maxManagerSize);

  mdd_free   (newR); 
  array_free (to_quantify_out0);
  array_free (to_quantify_out1);

  /* CMPE293: Don't worry: just return NULL here */ 

  if (task == 3) { 
    array_free (emptyArray); 
    array_free (allvars); 
    return (R);
  } else if (task == 0) { 
    /* it must now add the missing variables. what a bore. */ 
    /* This puts in conjArray the transition relation tau of the module */ 
    conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
    /* then, we add the frontier, computing newR /\ tau */ 
    array_insert_last (mdd_t *, conjArray, R); 
    /* This command puts in newImage the BDD corresponding to 
       exists allvars . (newR /\ tau)
       that, as you know, consists only of primed (next-state) variables. */ 
    newImage  = Img_MultiwayLinearAndSmooth(mgr, conjArray, allvars, emptyArray); 
    array_free  (conjArray); 
    unpImage  = SlUnprimeMdd (newImage); 
    mdd_free (newImage);
    array_free (emptyArray); 
    array_free (allvars); 
    return (unpImage); 
  } else {  
    /* task is 1 or 2 */ 
    Main_MochaPrint ("*Passed.\n"); 
    mdd_free (R); 
    array_free (emptyArray); 
    array_free (allvars); 
    return (NULL); 
  }
}


/**Function********************************************************************

  Synopsis           [Computes in a randomized fashion the set of reachable 
                      states, and checks during the 
                      computation that one of these conditions holds, 
		      depending on the value of the input variable task: 
		      task=1: checks that the reachable states imply a formula
		      task=2: checks that a given transition invariant holds. 
		      Does not return anything. 
		      Inputs: 
		      task:      see above. 
		      inFsm:     the FSM that has to be thus checked. 
		      invariant: the (transition?) invariant to be checked. 
		      sizeLimit: the size limit (in n. of BDD nodes) of the 
		                 reachability predicate. 
		      iterLimit: the maximum n. of iterations that will be 
                                 tried; -1 means no limit. 
		      shrinkAlgo: algorithm used for shrinking the mdd. 
		      mode:  0 = silent (only max data), 1 = verbose ]


  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]


******************************************************************************/

static void computeRandReach (
	      Sl_GSTEntry_t *inFsm, 
	      int            task, 
	      array_t       *invariant,
	      int            sizeLimit, 
	      int            iterLimit, 
	      int            shrinkAlgo, 
	      int            mode) 
{
  int      niterations = 0; 
  array_t *allvars, *primedEras, *tempArray, *supportVars;         
  array_t *emptyArray, *conjArray; 
  mdd_t   *newImage, *unpImage; 
  mdd_t   *initReg, *R, *newR; 
  mdd_manager *mgr;
  int     outcome; 
  int     hitCeiling = FALSE; 
  int     maxsize, sizeR; 

  emptyArray = array_alloc (int, 0);
  mgr = SlGetMddManager ();

  /* allvars = controlled + external + erased + erased' variables */
  allvars    = array_join (inFsm -> obj.fsm.contrvarL, inFsm->obj.fsm.extvarL); 
  /* this line is not needed: erased vars are among the controlled and external. 
     array_append (allvars, inFsm->obj.fsm.erasedvarL); */ 
  primedEras = SlPrimeVars(inFsm->obj.fsm.erasedvarL);
  array_append (allvars, primedEras); 
  array_free   (primedEras); 

  /* supportVars = (controlled + external) \ erased.  Used to check support, and 
     count the number of reachable (or controllable) states. */ 
  tempArray   = array_join      (inFsm -> obj.fsm.contrvarL, inFsm->obj.fsm.extvarL); 
  supportVars = SlArrayDifference (tempArray, inFsm->obj.fsm.erasedvarL); 
  array_free    (tempArray); 

  /* prepares the initial condition, and puts it in newR and initReg. */ 
  newR    = SlGetInit (inFsm); 
  initReg = mdd_dup (newR); 
  if (mode) {
    SlPrintBddStats (initReg, supportVars); 
  }
  maxsize = mdd_size (initReg); 

  /* checks the initial condition. */ 
  if (task == 1) { 
    conjArray = SlSingleMddArray (initReg); 
    outcome = SlImplicationCheck (conjArray, invariant);
    array_free (conjArray); 
  } else if (task == 2) { 
    conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
    array_insert_last (mdd_t*, conjArray, initReg); 
    outcome = SlImplicationCheck (conjArray, invariant); 
    array_free (conjArray); 
  } 

  if (!outcome) { 
    Main_MochaPrint ("*Invariant failed for initial condition.\n"); 
    mdd_free   (initReg); 
    mdd_free   (newR); 
    array_free (emptyArray); 
    array_free (allvars); 
    return; 
  }

  /* no previous set of reachable states */ 
  R = NULL; 
  /* so far, we don't know the invariant to be true */
  outcome = FALSE; 

  /* now loops, scanning the reachable states. */ 
  for (niterations = 1; niterations < iterLimit; niterations++) {

    /* if we haven't hit the ceiling yet, we remember the last set of 
       reachable states to detect termination. */ 
    if (!hitCeiling) {
      if (R) {
	mdd_free (R); 
      }
      R = newR; 
    }
    /* now we compute the next image. */ 
    conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
    array_insert_last (mdd_t *, conjArray, newR); 
    newImage  = Img_MultiwayLinearAndSmooth(mgr, conjArray, allvars, emptyArray); 
    array_free  (conjArray); 
    unpImage  = SlUnprimeMdd (newImage); 
    mdd_free (newImage);
    /* Just in case, if the image is larger than the set of reachable states... */ 
    sizeR = mdd_size (unpImage); 
    if (sizeR > maxsize) { 
      maxsize = sizeR; 
    }
    /* now that the new image in unpImage, we can test it as we did 
       for the initial condition.  I believe it is more efficient to 
       check the new image, than newR, if not else because we avoid 
       doing the OR. */ 
    if (task == 1) { 
      conjArray = SlSingleMddArray (unpImage); 
      outcome   = SlImplicationCheck (conjArray, invariant);
      array_free (conjArray); 
    } else if (task == 2) { 
      conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
      array_insert_last (mdd_t*, conjArray, unpImage); 
      outcome = SlImplicationCheck (conjArray, invariant); 
      array_free (conjArray); 
    } 
    if (!outcome) { 
      Main_MochaPrint ("*Invariant failed at iteration %d.\n", niterations); 
      /* I also print the max MDD size that occurred. */ 
      Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 
      mdd_free   (initReg); 
      mdd_free   (newR);
      /* no need to free R: in fact, either R is newR, or we have
	 hit the limit, and so R is NULL. */ 
      mdd_free   (unpImage); 
      array_free (emptyArray); 
      array_free (allvars); 
      return;
    }

    /* reinjects in the iteration the set of initial states,
       so that it's not necessary to restart the 
       simulation/reachability computation. */
    /* Aswini: it is a bad idea to reinject the initial states; don't do it */ 
    newR = mdd_or (initReg, unpImage, 1, 1);
    mdd_free (unpImage); 
    
    if (mode) { 
      Main_MochaPrint ("*Iteration %7d ", niterations);
      SlPrintBddStats (newR, supportVars);
    }
    sizeR = mdd_size (newR); 
    if (sizeR > maxsize) { 
      maxsize = sizeR; 
    }

    /* and now for the fun part: if the new region is larger than the 
       size limit, we have to randomly reduce it. */ 
    if (sizeR > sizeLimit) { 
      hitCeiling = TRUE; 
      /* to save space, free R right here, since no termination criterion 
	 will be possible. */ 
      if (R) {
	mdd_free (R); 
	R = NULL; 
      }
      mddShrink (shrinkAlgo, &newR, sizeR, allvars, sizeLimit, 1); 
    }

    /* checks for termination */ 
    if (!hitCeiling && mdd_equal(R, newR)) { 
      outcome = TRUE; 
      break; 
    }
    /* to signal that fixpoint not reached */ 
    outcome = FALSE; 

  } /* end of the reachability iteration. */ 
  
  /* I print the n. of iterations to converge, not the n. of iterations to 
     detect convergence (which is bigger by 1). */ 
  Main_MochaPrint ("*Reachability computed for %d iterations.\n", niterations - 1);
  /* I also print the max MDD size that occurred. */ 
  Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 

  mdd_free   (newR); 
  if (R) {
    mdd_free (R); 
  }
  mdd_free   (initReg); 
  array_free (emptyArray); 
  array_free (allvars); 

  if (outcome) { 
    Main_MochaPrint ("*Passed.\n"); 
  } else { 
    Main_MochaPrint ("*No error found, no fixpoint reached.\n"); 
  }
}


/**Function********************************************************************

  Synopsis           [Does a BDD-based simulation, checking whether any state
                      violating the invariant is reached.  This function differs
		      from the above one because we do not re-inject the initial
		      states at each iteration.  Hence, this function needs to be
		      restarted, if a complete search is to be performed.  On the 
		      other hand, this function may guarantee better depth than the 
		      previous one. 
		      Does not return anything. 
		      Inputs: 
		      task: 
		        task=1: checks that the reachable states imply a formula
		        task=2: checks that a given transition invariant holds. 
		      inFsm:     the FSM that has to be thus checked. 
		      invariant: the (transition?) invariant to be checked. 
		      sizeLimit: the size limit (in n. of BDD nodes) of the 
		                 reachability predicate. 
		      iterLimit: the maximum n. of iterations that will be 
                                 tried; -1 means no limit. 
		      shrinkAlgo: algorithm to be used for shrinking. 
		      mode:  0 = silent (only max data), 1 = verbose ]


  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]


******************************************************************************/

static void computeRandSim (
	      Sl_GSTEntry_t *inFsm, 
	      int            task, 
	      array_t       *invariant,
	      int            sizeLimit, 
	      int            iterLimit, 
	      int            shrinkAlgo, 
	      int            mode) 
{
  int      niterations = 0; 
  array_t *allvars, *primedEras, *tempArray, *supportVars;         
  array_t *emptyArray, *conjArray; 
  mdd_t   *initReg, *newR, *newImage;
  mdd_manager *mgr;
  int     outcome; 
  int     hitCeiling = FALSE; 
  int     maxsize, sizeR; 

  emptyArray = array_alloc (int, 0);
  mgr = SlGetMddManager ();

  /* allvars = controlled + external + erased + erased' variables */
  allvars    = array_join (inFsm -> obj.fsm.contrvarL, inFsm->obj.fsm.extvarL); 
  /* this line is not needed: erased vars are among the controlled and external. 
     array_append (allvars, inFsm->obj.fsm.erasedvarL); */ 
  primedEras = SlPrimeVars(inFsm->obj.fsm.erasedvarL);
  array_append (allvars, primedEras); 
  array_free   (primedEras); 

  /* supportVars = (controlled + external) \ erased.  Used to check support, and 
     count the number of reachable (or controllable) states. */ 
  tempArray   = array_join      (inFsm -> obj.fsm.contrvarL, inFsm->obj.fsm.extvarL); 
  supportVars = SlArrayDifference (tempArray, inFsm->obj.fsm.erasedvarL); 
  array_free    (tempArray); 

  /* prepares the initial condition, and puts it in newR and initReg. */ 
  newR    = SlGetInit (inFsm); 
  initReg = mdd_dup (newR); 

  /* Prints, or stores, size of initial condition. */ 
  if (mode) {
    SlPrintBddStats (initReg, supportVars); 
  }
  maxsize = mdd_size (initReg); 

  /* checks the initial condition. */ 
  if (task == 1) { 
    conjArray = SlSingleMddArray (initReg); 
    outcome = SlImplicationCheck (conjArray, invariant);
    array_free (conjArray); 
  } else if (task == 2) { 
    conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
    array_insert_last (mdd_t*, conjArray, initReg); 
    outcome = SlImplicationCheck (conjArray, invariant); 
    array_free (conjArray); 
  } 

  if (!outcome) { 
    Main_MochaPrint ("*Invariant failed for initial condition.\n"); 
    mdd_free   (initReg); 
    mdd_free   (newR); 
    array_free (emptyArray); 
    array_free (allvars); 
    return; 
  }

  /* so far, we don't know the invariant to be true */
  outcome = FALSE; 

  /* now loops, scanning the reachable states. */ 
  for (niterations = 1; niterations < iterLimit; niterations++) {

    /* now we compute the next image. */ 
    conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
    array_insert_last (mdd_t *, conjArray, newR); 
    newImage  = Img_MultiwayLinearAndSmooth(mgr, conjArray, allvars, emptyArray); 
    array_free  (conjArray); 
    mdd_free (newR); 
    newR = SlUnprimeMdd (newImage); 
    /* keeps track of size */ 
    sizeR = mdd_size (newR); 
    if (sizeR > maxsize) { 
      maxsize = sizeR; 
    }
    /* check whether the new set of states satisfies the invariant. */ 
    if (task == 1) { 
      conjArray = SlSingleMddArray (newR); 
      outcome   = SlImplicationCheck (conjArray, invariant);
      array_free (conjArray); 
    } else if (task == 2) { 
      conjArray = array_dup (inFsm -> obj.fsm.updateBdd); 
      array_insert_last (mdd_t*, conjArray, newR); 
      outcome = SlImplicationCheck (conjArray, invariant); 
      array_free (conjArray); 
    } 
    if (!outcome) { 
      Main_MochaPrint ("*Invariant failed at iteration %d.\n", niterations); 
      /* I also print the max MDD size that occurred. */ 
      Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 
      mdd_free   (initReg); 
      mdd_free   (newR);
      array_free (emptyArray); 
      array_free (allvars); 
      return;
    }

    if (mode) { 
      Main_MochaPrint ("*Iteration %7d ", niterations);
      SlPrintBddStats (newR, supportVars);
    }

    /* and now for the fun part: if the new region is larger than the 
       size limit, we have to randomly reduce it. */ 
    if (sizeR > sizeLimit) { 
      hitCeiling = TRUE; 
      mddShrink (shrinkAlgo, &newR, sizeR, allvars, sizeLimit, 1); 
    }

  } /* end of the reachability iteration. */ 
  
  /* I print the n. of iterations to converge, not the n. of iterations to 
     detect convergence (which is bigger by 1). */ 
  Main_MochaPrint ("*Reachability computed for %d iterations.\n", niterations - 1);
  /* I also print the max MDD size that occurred. */ 
  Main_MochaPrint ("*Max MDD size = %d\n", maxsize); 

  mdd_free   (newR); 
  mdd_free   (initReg); 
  array_free (emptyArray); 
  array_free (allvars); 

  Main_MochaPrint ("*No error found, no fixpoint reached.\n"); 
  return; 
}


/**Function********************************************************************

  Synopsis           [Dispacting procedure, that shrinks a given mdd to a given 
                      size, using one of several algorithms. 

		      INPUT: 
		      algo: algorithm to be used. 
		        0 = random cube complement pick 
			1 = cube pick in proportion to n. of states
		      R: address of the mdd to shink
		      sizeR: size of above mdd
		      suppportVars: support variables of above MDD
		      sizeLimit: max size of returned MDD
		      mode: TRUE = verbose, FALSE = silent ] 

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

static void mddShrink (int algo, mdd_t **R, int sizeR, array_t *supportVars, int sizeLimit, int mode)

{
  switch (algo) {
  case 0: 
    randomMddCubeShrink (R, sizeR, supportVars, sizeLimit, mode); 
    break;
  case 1: 
    randomMddStateShrink (R, sizeR, supportVars, sizeLimit, mode); 
    break;
  }
}


/**Function********************************************************************

  Synopsis           [Given an mdd, a set of its support variables, and a size 
                      limit, randomly computes an mdd representing a subset of 
		      the states, and having no more than the specified size
		      limit.  It does so by repeatedly selecting a variable, 
		      selecting a cube, and -if the cube intersects the mdd-
		      taking the conjunction of the cube and the mdd. 

		      INPUT: 
		      R: address of the mdd to shink
		      sizeR: size of above mdd
		      suppportVars: support variables of above MDD
		      sizeLimit: max size of returned MDD
		      mode: TRUE = verbose, FALSE = silent ] 

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

static void randomMddCubeShrink (mdd_t **R, int sizeR, array_t *supportVars, int sizeLimit, int mode)
{
  
  int     nvars, slicepos, slicevar, sizeslice; 
  mdd_t   *sliceBdd, *sliced; 

  nvars = array_n (supportVars); 
  while (sizeR > sizeLimit) { 
    /* Picks a variable and a cube at random. 
       Keeps doing this until we find a complement-cube that intersects the mdd. */ 
    sliced = NULL; 
    do {
      if (sliced) {
	mdd_free (sliced); 
      }
      slicepos = random () % nvars; 
      slicevar = array_fetch (int, supportVars, slicepos); 
      /* sliceBdd is a random cube for slicevar. 
	 Example: if slicevar is x, then sliceBdd may be x <=> true */ 
      sliceBdd = SlRandomSliceBdd (slicevar); 
      /* Note: takes the conjunction with the complement of the cube, to yield 
	 a less drastic shrinking. */ 
      sliced = mdd_and (*R, sliceBdd, 1, 0); 
      mdd_free (sliceBdd); 
      sizeslice = mdd_size (sliced);
    } while (sizeslice == 1); 

    mdd_free (*R);
    *R = sliced; 
    sizeR = sizeslice; 
    if (mode) { 
      SlPrintBddStats (*R, supportVars);
    }
  } /* while (sizeR > sizeLimit) */ 
}


/**Function********************************************************************

  Synopsis           [Given an mdd, a set of its support variables, and a size 
                      limit, randomly computes an mdd representing a subset of 
		      the states, and having no more than the specified size
		      limit.  It does so by repeatedly selecting a variable, 
		      computing all cubes for that variable, and selecting each 
		      cube with a probability proportional to the n. of states 
		      in the intersection between the cube and the mdd. 
		      
		      INPUT: 
		      R: address of the mdd to shink
		      sizeR: size of above mdd
		      suppportVars: support variables of above MDD
		      sizeLimit: max size of returned MDD
		      mode: TRUE = verbose, FALSE = silent ] 

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/


static void randomMddStateShrink (mdd_t **R, int sizeR, array_t *supportVars, int sizeLimit, int mode) 
{
  
  int     n_vars_not_yet_sliced, n_cubes, var_slice_index, slicevar, sizeslice; 
  int     *statevars;
  double  n_states, n_states_so_far, randpick; 
  array_t *cubes; 
  mdd_t   *cube, *slice; 
  int     cube_index; 
  mdd_manager *mgr;

  /* normal array, for random modification */ 
  mgr                      = SlGetMddManager ();
  statevars                = array_data (int, supportVars); 
  n_vars_not_yet_sliced    = array_n (supportVars); 
  n_states                 = (double) mdd_count_onset (mgr, *R, supportVars); 

  Main_MochaPrint ("ok1 "); /* debug */
  while (sizeR > sizeLimit) { 
    /* Picks a variable at random (the variable must not
       have been picked before), and computes all cubes for it. */ 
    var_slice_index = random () % n_vars_not_yet_sliced; 
    slicevar        = statevars [var_slice_index];
    statevars [var_slice_index] = statevars [n_vars_not_yet_sliced - 1]; 
    n_vars_not_yet_sliced--; 
    Main_MochaPrint ("ok2 "); /* debug */
    cubes    = SlGetArrayOfCubes (slicevar); 
    n_cubes  = array_n (cubes); 
    /* Now, it picks a random state index randpick.
       To save on the computation, it only computes enough 
       slices to get to a slice containing this state... 
       on average, this should save half of the slice computations. 
       n_states_so_far is the n. of states we have seen so far, 
       and is used for this computation. */ 
    Main_MochaPrint ("What is RAND_MAX doing here?? "); /* debug */
    randpick        = (random () * n_states) / RAND_MAX; 
    n_states_so_far = 0; 
    cube_index      = 0; 
    slice           = NULL;
    Main_MochaPrint ("ok4 "); /* debug */

    while (n_states_so_far < randpick && cube_index < n_cubes) { 
      cube = array_fetch (mdd_t *, cubes, cube_index); 
      Main_MochaPrint ("ok5 "); /* debug */
      cube_index++;
      if (slice) { 
	mdd_free (slice); 
      }
      slice = mdd_and (*R, cube, 1, 1); 
      n_states_so_far += (double) mdd_count_onset (mgr, slice, supportVars); 
      Main_MochaPrint ("ok6 "); /* debug */
    }
    /* ok: however we got out of the while loop, the last slice is 
       the one we are after. */
    mdd_free (*R);
    *R    = slice; 
    sizeR = mdd_size (slice); 
    Main_MochaPrint ("ok7 "); /* debug */

    /* frees all the cubes */ 
    for (cube_index = 0; cube_index < n_cubes; cube_index++) {
      mdd_free (array_fetch (mdd_t *, cubes, cube_index)); 
    }
    Main_MochaPrint ("ok8 "); /* debug */
    array_free (cubes); 

    Main_MochaPrint ("ok9 "); /* debug */
    if (mode) { 
      Main_MochaPrint("    *Size reduction: "); 
      SlPrintBddStats(slice, supportVars); 
    }
  } /* while (sizeR > sizeLimit) */ 
  free (statevars);
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static int
computeMaxInvariantSize(
  int maxInvariantSize, 
  array_t * invariant,
  int task)
{
  int size = 0;
  int i;
  mdd_t * mdd;


  if (task == 1 || task == 2) {
    arrayForEachItem(mdd_t *, invariant, i, mdd) {
      size = size + mdd_size(mdd);
    }
  }
  
  if (size > maxInvariantSize) {
    return size;
  }
  
  return maxInvariantSize;
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
computeMaxTotalSize(
  int maxAllSize,
  int sizeR,
  array_t * invariant,
  int task)
{
  int size = sizeR;
  int i;
  mdd_t * mdd;

  if (task == 1 || task == 2) {
    arrayForEachItem(mdd_t *, invariant, i, mdd) {
      size = size + mdd_size(mdd);
    }
  }
  if (size > maxAllSize) {
    return size;
  }
  return maxAllSize;
}

/* 
 * $Log: slRm.c,v $
 * Revision 1.24  2003/05/02 05:27:56  luca
 * Added Log keywords
 *
 * 
 */ 
