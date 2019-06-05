/**CFile***********************************************************************

  FileName    [slIntf.c]

  PackageName [sl]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [Luca de Alfaro]

  Copyright   [Copyright (c) 1994-2001 The Regents of the Univ. of California.
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

  Revision    [$Id: slIntf.c,v 1.30 2003/05/02 05:27:56 luca Exp $]

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

static array_t * make_levels (array_t *merged_dependency, array_t *ivars, array_t *ovars);
static void assign_levels (array_t *levels,
			   array_t *Iinit,  array_t *Oinit, 
			   array_t *Itrans, array_t *Otrans);
static void copy_single_level_composition (int       intf_n, 
					   array_t * out_levels, 
					   array_t * out_pseudo_levels, 
					   Level_t * lev);
static void copy_single_level_refinement (int       intf_n, 
					  array_t * out_levels, 
					  array_t * out_pseudo_levels, 
					  Level_t * lev);
static void merge_single_levels_composition (array_t * out_levels, 
					     array_t * out_pseudo_levels, 
					     Level_t * lev1, 
					     Level_t * lev2);
static void merge_single_levels_refinement (array_t * out_levels, 
					    array_t * out_pseudo_levels, 
					    Level_t * lev1, 
					    Level_t * lev2);
static int zip_variable_levels (int        mode,
				array_t ** levels_p, 
				array_t ** pseudo_levels_p, 
				array_t *  levels1, 
				array_t *  levels2);
static int compose_interfaces (array_t *level1, array_t *level2, array_t **out_levels);
static int game_compose_interfaces (array_t * levels, array_t * pseudo_levels);
static void print_levels_array (array_t *levels);
static array_t * filter_primed_and_unprime (array_t * vars);
static int refinement_check (array_t * spec_levels, 
			     array_t * impl_levels, 
			     int verbose); 
static int check_interface_refinement (array_t * levels1, 
				       array_t * levels2, 
				       int       verbose); 
static int strengthen_interface (array_t **out_levels, 
				 array_t * levels, 
				 array_t * spec, 
				 int       verbose);

/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


int SlCreateCommandIntf(
			 Tcl_Interp *interp,
			 Main_Manager_t *manager)
{

    Tcl_CreateCommand(interp, "sl_make_intf", SlMakeIntfCmd, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "sl_make_intf_out", SlMakeIntfOutCmd, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "sl_print_levels", SlPrintLevelsCmd, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "sl_compose_intf", SlComposeIntfCmd, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "sl_check_intf_ref", SlCheckIntfRefCmd,
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "sl_print_intf", SlPrintIntfCmd, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "sl_strengthen_intf_inv", SlStrengthenIntfCmd, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "sl_print_intf_support", SlPrintIntfSupportCmd, 
		      (ClientData) manager,
		      (Tcl_CmdDeleteProc *) NULL);
    return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_make_intf.
                      This function, given two modules, one describing the 
		      input evolution, the other describing the output evolution, 
		      creates a single new interface module that combines the two.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int SlMakeIntfCmd ( 
		   ClientData clientData,
		   Tcl_Interp *interp,
		   int argc,
		   char** argv)
{
    char usage[] = "Usage: sl_make_intf <outIntf> <IFsm> <OFsm>";
    char *IfsmName, *OfsmName, *IntfName;
    Sl_GSTEntry_t *Ientry, *Oentry, *intfEntry;
    array_t *Ivars, *Ovars, *Ivars1, *Ovars1, *Ivars2, *Ovars2;
    array_t *merged_dependency, levels; 
    
    util_getopt_reset();
    util_optind++;
    if (argc != 4) {
        Main_MochaErrorPrint("Syntax error. %s\n", usage);
	return TCL_OK;
    }

    IntfName = argv[util_optind];
    if (st_is_member(Sl_GST, IntfName)) {
	Main_MochaErrorPrint("Error: Object %s exists.\n", IntfName);
	return TCL_OK;
    }

    util_optind++;

    IfsmName = argv[util_optind];
    if (!st_lookup(Sl_GST, IfsmName, (char **) &Ientry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", IfsmName);
	return TCL_OK;
    }
    if (Ientry -> kind != Sl_FSM_c) {
	Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", IfsmName);
	return TCL_OK;
    }

    util_optind++;

    OfsmName = argv[util_optind];
    if (!st_lookup(Sl_GST, OfsmName, (char **) &Oentry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", OfsmName);
	return TCL_OK;
    }
    if (Oentry -> kind != Sl_FSM_c) {
	Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", OfsmName);
	return TCL_OK;
    }

    /* First, checks that the modules can indeed be paired up as an interface. 
       They should describe inputs and outputs, and have no erased variables. */ 

    Ivars1 = Ientry -> obj.fsm.contrvarL; 
    Ivars2 = Oentry -> obj.fsm.extvarL; 

    Ovars1 = Oentry -> obj.fsm.contrvarL; 
    Ovars2 = Ientry -> obj.fsm.extvarL; 

    /* All output variables must be controlled by the output module. 
       There is no such restriction for inputs: an interface can have expectations on inputs it does not need to compute its outputs. */ 
    if ((!SlIsArraySubset (Ovars2, Ovars1)) ||
	/* No erased variables */ 
	(array_n(Ientry -> obj.fsm.erasedvarL) > 0) ||
	(array_n(Oentry -> obj.fsm.erasedvarL) > 0)) {
	Main_MochaErrorPrint ("Interface error:\n");
	Main_MochaErrorPrint ("Forming an interface from modules with incompatible variables!\n");
    }

    /* Now merges and checks the dependencies for cycles. */ 
    merged_dependency = SlDependencyMerge(Ientry -> obj.fsm.dependency,
					  Oentry -> obj.fsm.dependency);
    if (SlDetectCycles (merged_dependency)) { 
	Main_MochaErrorPrint ("Interface error:\n");
	Main_MochaErrorPrint ("Error: dependency cycle in interface formation!\n"); 
	return TCL_OK; 
    }

    /* The input vars are the union of the input vars */ 
    Ivars = SlArrayUnion (Ivars1, Ivars2);
    /* The output vars are the output of the O module */ 
    Ovars = array_dup (Ovars1); 

    /* Allocates the symbol table entry for the output. */ 
    intfEntry = ALLOC(Sl_GSTEntry_t, 1);
    intfEntry -> name = util_strsav(IntfName);
    intfEntry -> kind = Sl_INTF_c; 
    /* and fills it up, assigning the variables to levels... */ 
    intfEntry -> obj.intf.levels     = make_levels (merged_dependency, Ivars, Ovars); 
    /* ...and assigning the various BDDs to the various levels */ 
    assign_levels (intfEntry -> obj.intf.levels, 
		   Ientry -> obj.fsm.initBdd, Oentry -> obj.fsm.initBdd, 
		   Ientry -> obj.fsm.updateBdd, Oentry -> obj.fsm.updateBdd);

    /* cleans up afterwards */ 
    /* There is a bug here in slFreddy.c in how the merged dependency is created that 
       prevents us from freeing this. */ 
    /* SlFreeDependency (merged_dependency); */ 

    /* inserts in the symbol table, and it's done. */ 
    st_insert(Sl_GST, intfEntry -> name, (char *) intfEntry);
    return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_make_intf_out.
                      This function creates an interface having only an output 
		      portion (with no input assumptions). 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int SlMakeIntfOutCmd (
		      ClientData clientData,
		      Tcl_Interp *interp,
		      int argc,
		      char** argv)
{
    char usage[] = "Usage: sl_make_intf_out <outIntf> <OFsm>";
    char *OfsmName, *IntfName;
    Sl_GSTEntry_t *Oentry, *intfEntry;
    array_t *Ivars, *Ovars, *Ivars1, *Ovars1, *Ivars2, *Ovars2;
    array_t *merged_dependency, levels; 
    array_t * mdd_array; 

    util_getopt_reset();
    util_optind++;
    if (argc != 3) {
	Main_MochaErrorPrint("Syntax error. %s\n", usage);
	return TCL_OK;
    }

    IntfName = argv[util_optind];
    if (st_is_member(Sl_GST, IntfName)) {
	Main_MochaErrorPrint("Error: Object %s exists.\n", IntfName);
	return TCL_OK;
    }

    util_optind++;

    OfsmName = argv[util_optind];
    if (!st_lookup(Sl_GST, OfsmName, (char **) &Oentry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", OfsmName);
	return TCL_OK;
    }
    if (Oentry -> kind != Sl_FSM_c) {
	Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", OfsmName);
	return TCL_OK;
    }

    /* One of the two interfaces has no variables. */ 

    Ivars = Oentry -> obj.fsm.extvarL; 
    Ovars = Oentry -> obj.fsm.contrvarL; 

    /* No erased variables */ 
    if (array_n(Oentry -> obj.fsm.erasedvarL) > 0) {
	Main_MochaErrorPrint ("Interface error:\n");
	Main_MochaErrorPrint ("Forming an interface from a module with erased variables!\n"); 
    }

    /* Now merges and checks the dependencies for cycles. */ 
    merged_dependency = Oentry -> obj.fsm.dependency;

    /* Allocates the symbol table entry for the output. */ 
    intfEntry = ALLOC(Sl_GSTEntry_t, 1);
    intfEntry -> name = util_strsav(IntfName);
    intfEntry -> kind = Sl_INTF_c; 
    /* and fills it up, assigning the variables to levels... */ 
    intfEntry -> obj.intf.levels     = make_levels (merged_dependency, Ivars, Ovars); 
    /* ...and assigning the various BDDs to the various levels */ 
    mdd_array = array_alloc (mdd_t *, 0); 
    assign_levels (intfEntry -> obj.intf.levels, 
		   mdd_array, Oentry -> obj.fsm.initBdd, 
		   mdd_array, Oentry -> obj.fsm.updateBdd);

    /* inserts in the symbol table, and it's done. */ 
    st_insert(Sl_GST, intfEntry -> name, (char *) intfEntry);
    return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Prints the list of levels of an interface.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/


int SlPrintLevelsCmd ( 
		      ClientData clientData,
		      Tcl_Interp *interp,
		      int argc,
		      char** argv)
{
    char usage[] = "Usage: sl_print_levels <intf>";
    char *IntfName;
    Sl_GSTEntry_t *entry;
    int i, j, var; 
    Level_t *level; 
    char *varName;

    util_getopt_reset();
    util_optind++;
    if (argc != 2) {
	Main_MochaErrorPrint("Syntax error. %s\n", usage);
	return TCL_OK;
    }

    IntfName = argv[util_optind];
    if (!st_lookup(Sl_GST, IntfName, (char **) &entry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", IntfName);
	return TCL_OK;
    }
    if (entry -> kind != Sl_INTF_c) {
	Main_MochaErrorPrint("Error: Object %s is not an interface.\n", IntfName);
	return TCL_OK;
    }

    arrayForEachItem (Level_t *, entry -> obj.intf.levels, i, level) { 
	Main_MochaPrint ("Level %d  input vars:\n", i); 
	arrayForEachItem (int, level -> Ivars, j, var) { 
	    varName = Var_VariableReadName (SlReturnVariableFromId (var));
	    Main_MochaPrint (" %s", varName); 
	}
	Main_MochaPrint ("\nLevel %d output vars:\n", i); 
	arrayForEachItem (int, level -> Ovars, j, var) { 
	    varName = Var_VariableReadName (SlReturnVariableFromId (var));
	    Main_MochaPrint (" %s", varName); 
	}
	Main_MochaPrint ("\n"); 
    }
    return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_compose_intf.
                      This function, given two interfaces, composes them and 
		      checks if they are compatible. 
		      If they are not compatible, says so. 
		      If they are compatible, says so, and returns the composition. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int SlComposeIntfCmd ( 
		      ClientData clientData,
		      Tcl_Interp *interp,
		      int argc,
		      char** argv)
{
    /* boilerplate */ 
    char usage[] = "Usage: sl_compose_intf <outIntf> <Intf1> <Intf2>";
    char *outName, *Intf1Name, *Intf2Name;
    Sl_GSTEntry_t *outEntry, *Intf1Entry, *Intf2Entry;
    /* custom */ 
    array_t * levels; /* levels of the newly constructed interface */ 
    int outcome; 

    util_getopt_reset();
    util_optind++;
    if (argc != 4) {
	Main_MochaErrorPrint("Syntax error. %s\n", usage);
	return TCL_OK;
    }

    outName = argv[util_optind];
    if (st_is_member(Sl_GST, outName)) {
	Main_MochaErrorPrint("Error: Object %s exists.\n", outName);
	return TCL_OK;
    }

    util_optind++;

    Intf1Name = argv[util_optind];
    if (!st_lookup(Sl_GST, Intf1Name, (char **) &Intf1Entry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", Intf1Name);
	return TCL_OK;
    }
    if (Intf1Entry -> kind != Sl_INTF_c) {
	Main_MochaErrorPrint("Error: Object %s is not an interface.\n", Intf1Name);
	return TCL_OK;
    }

    util_optind++;

    Intf2Name = argv[util_optind];
    if (!st_lookup(Sl_GST, Intf2Name, (char **) &Intf2Entry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", Intf2Name);
	return TCL_OK;
    }
    if (Intf2Entry -> kind != Sl_INTF_c) {
	Main_MochaErrorPrint("Error: Object %s is not an interface.\n", Intf2Name);
	return TCL_OK;
    }

    /* End of argument processing */ 
    outcome = compose_interfaces (Intf1Entry -> obj.intf.levels, 
				  Intf2Entry -> obj.intf.levels, 
				  &levels); 
    if (outcome == 1) { 
	Main_MochaErrorPrint ("Interfaces not compatible:\n"); 
	Main_MochaErrorPrint ("Variable levels cannot be merged!\n"); 
	return TCL_OK; 
    } else if (outcome == 2) { 
	/* deallocates the levels: presently, we do not use them for guidance */ 
	SlFreeLevels (levels); 
	Main_MochaErrorPrint ("Interfaces not compatible:\n"); 
	Main_MochaErrorPrint ("Input assumptions incompatible under all environments!"); 
	return TCL_OK; 
    } else { 
	assert (outcome == 0); 
	Main_MochaPrint ("Interfaces %s and %s are compatible.\n", Intf1Name, Intf2Name); 
	/* Allocates the output, and finishes. */ 
	outEntry = ALLOC(Sl_GSTEntry_t, 1);
	outEntry -> name = util_strsav(outName);
	outEntry -> kind = Sl_INTF_c; 
	outEntry -> obj.intf.levels = levels; 
	st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
	return TCL_OK; 
    }
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_check_intf_ref
                      This function, given two interfaces intf1 and intf2, 
		      checks whether intf2 is a refinement of intf1. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int SlCheckIntfRefCmd ( 
		       ClientData clientData,
		       Tcl_Interp *interp,
		       int argc,
		       char** argv)
{
    /* boilerplate */ 
    char usage[] = "Usage: sl_check_intf_ref <Intf1> <Intf2> <s|v>";
    char *Intf1Name, *Intf2Name;
    Sl_GSTEntry_t *Intf1Entry, *Intf2Entry;
    /* custom */ 
    int verbose, outcome; 

    util_getopt_reset();
    util_optind++;
    if (argc != 4) {
	Main_MochaErrorPrint("Syntax error. %s\n", usage);
	return TCL_OK;
    }

    Intf1Name = argv[util_optind];
    if (!st_lookup(Sl_GST, Intf1Name, (char **) &Intf1Entry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", Intf1Name);
	return TCL_OK;
    }
    if (Intf1Entry -> kind != Sl_INTF_c) {
	Main_MochaErrorPrint("Error: Object %s is not an interface.\n", Intf1Name);
	return TCL_OK;
    }

    util_optind++;

    Intf2Name = argv[util_optind];
    if (!st_lookup(Sl_GST, Intf2Name, (char **) &Intf2Entry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", Intf2Name);
	return TCL_OK;
    }
    if (Intf2Entry -> kind != Sl_INTF_c) {
	Main_MochaErrorPrint("Error: Object %s is not an interface.\n", Intf2Name);
	return TCL_OK;
    }

    util_optind++;
    
    if (strcmp (argv[util_optind], "s") == 0) {
	verbose = 0; 
    } else if (strcmp (argv[util_optind], "v") == 0) {
	verbose = 1; 
    } else { 
	Main_MochaErrorPrint("Syntax error. %s\n", usage);
	return TCL_OK;
    }

    /* End of argument processing */ 
    outcome = check_interface_refinement (Intf1Entry -> obj.intf.levels, 
					  Intf2Entry -> obj.intf.levels, 
					  verbose); 
    if (outcome == 1) { 
	Main_MochaPrint ("%s refines %s: No ", Intf2Name, Intf1Name);  
	Main_MochaPrint ("(variable levels cannot be merged)\n"); 
	return TCL_OK; 
    } else if (outcome == 2) { 
	Main_MochaPrint ("%s refines %s: No\n", Intf2Name, Intf1Name); 
	return TCL_OK; 
    } else { 
	assert (outcome == 0); 
	Main_MochaPrint ("%s refines %s: Yes\n", Intf2Name, Intf1Name); 
	return TCL_OK; 
    }
}


/**Function********************************************************************

  Synopsis           [Implements the command sl_print_intf.] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int SlPrintIntfCmd ( 
		    ClientData clientData,
		    Tcl_Interp *interp,
		    int argc,
		    char** argv)
{
    /* boilerplate */ 
    char usage[] = "Usage: sl_print_intf <Intf>";
    char *IntfName;
    Sl_GSTEntry_t *IntfEntry;

    /* custom */ 
    array_t * levels; /* levels of the newly constructed interface */ 
    int i, j, var; 
    Level_t * level; 
    char *varName;
    mdd_t *conj; 

    util_getopt_reset();
    util_optind++;
    if (argc != 2) {
	Main_MochaErrorPrint("Syntax error. %s\n", usage);
	return TCL_OK;
    }

    IntfName = argv[util_optind];
    if (!st_lookup(Sl_GST, IntfName, (char **) &IntfEntry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", IntfName);
	return TCL_OK;
    }
    if (IntfEntry -> kind != Sl_INTF_c) {
	Main_MochaErrorPrint("Error: Object %s is not an interface.\n", IntfName);
	return TCL_OK;
    }

    Main_MochaPrint ("=====================================================\n"); 
    Main_MochaPrint ("Interface %s\n"); 
    Main_MochaPrint ("=====================================================\n"); 
    arrayForEachItem (Level_t *, IntfEntry -> obj.intf.levels, i, level) { 
	Main_MochaPrint ("Level %d  input vars:\n", i); 
	arrayForEachItem (int, level -> Ivars, j, var) { 
	    varName = Var_VariableReadName (SlReturnVariableFromId (var));
	    Main_MochaPrint (" %s", varName); 
	}
	Main_MochaPrint ("\nLevel %d output vars:\n", i); 
	arrayForEachItem (int, level -> Ovars, j, var) { 
	    varName = Var_VariableReadName (SlReturnVariableFromId (var));
	    Main_MochaPrint (" %s", varName); 
	}
	Main_MochaPrint ("\n"); 
	Main_MochaPrint ("Number of Iinit   Bdds: %d\n", array_n (level -> IinitBdds)); 
	Main_MochaPrint ("Number of Oinit   Bdds: %d\n", array_n (level -> OinitBdds)); 
	Main_MochaPrint ("Number of Iupdate Bdds: %d\n", array_n (level -> IupdateBdds)); 
	Main_MochaPrint ("Number of Oupdate Bdds: %d\n", array_n (level -> OupdateBdds)); 
	Main_MochaPrint ("----------------------------------\n"); 
	Main_MochaPrint ("Iinit:\n"); 
	arrayForEachItem (mdd_t *, level -> IinitBdds, j, conj) { 
	    Main_MochaPrint ("MDD %d: ", j); 
	    SlMddPrintCubes (conj, 0);
	}
	Main_MochaPrint ("----------------------------------\n"); 
	Main_MochaPrint ("Oinit:\n"); 
	arrayForEachItem (mdd_t *, level -> OinitBdds, j, conj) { 
	    Main_MochaPrint ("MDD %d: ", j); 
	    SlMddPrintCubes (conj, 0);
	}
	Main_MochaPrint ("----------------------------------\n"); 
	Main_MochaPrint ("Iupdate:\n"); 
	arrayForEachItem (mdd_t *, level -> IupdateBdds, j, conj) { 
	    Main_MochaPrint ("MDD %d: ", j); 
	    SlMddPrintCubes (conj, 0);
	}
	Main_MochaPrint ("----------------------------------\n"); 
	Main_MochaPrint ("Oupdate:\n"); 
	arrayForEachItem (mdd_t *, level -> OupdateBdds, j, conj) { 
	    Main_MochaPrint ("MDD %d: ", j); 
	    SlMddPrintCubes (conj, 0);
	}
    }
    Main_MochaPrint ("=====================================================\n"); 
    return TCL_OK; 
}


/**Function********************************************************************

  Synopsis           [Function implementing command sl_strengthen_intf_inv
                      This function, given a predicate phi and an 
		      interface intf, strengthens the input assumption of 
		      intf to ensure that the invariant inv holds.  ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int SlStrengthenIntfCmd ( 
			 ClientData clientData,
			 Tcl_Interp *interp,
			 int argc,
			 char** argv)
{
    /* boilerplate */ 
    char usage[] = "Usage: sl_strengthen_intf_inv <outIntf> <phi> <Intf> <s|v>";
    char *outName, *specBddName, *IntfName; 
    Sl_GSTEntry_t *outEntry, *IntfEntry, *specEntry; 
    /* custom */ 
    array_t * levels; /* levels of the newly constructed interface */ 
    int verbose, ok;

    util_getopt_reset();
    util_optind++;
    if (argc != 5) {
	Main_MochaErrorPrint("Syntax error. %s\n", usage);
	return TCL_OK;
    }

    outName = argv[util_optind];
    if (st_is_member(Sl_GST, outName)) {
	Main_MochaErrorPrint("Error: Object %s exists.\n", outName);
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

    IntfName = argv[util_optind];
    if (!st_lookup(Sl_GST, IntfName, (char **) &IntfEntry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", IntfName);
	return TCL_OK;
    }
    if (IntfEntry -> kind != Sl_INTF_c) {
	Main_MochaErrorPrint("Error: Object %s is not an interface.\n", IntfName);
	return TCL_OK;
    }

    util_optind++;

    if (strcmp (argv[util_optind], "s") == 0) {
	verbose = 0; 
    } else if (strcmp (argv[util_optind], "v") == 0) {
	verbose = 1; 
    } else { 
	Main_MochaErrorPrint("Syntax error. %s\n", usage);
	return TCL_OK;
    }

    /* End of argument processing */ 
    /* Calls a subfunction that does everything */ 
    ok = strengthen_interface (&levels, 
			       IntfEntry -> obj.intf.levels, 
			       specEntry -> obj.bddArray, 
			       verbose); 
    if (!ok) { 
	Main_MochaPrint ("Error: interface becomes empty (no behaviors left)!\n"); 
	return TCL_OK; 
    } else { 
	/* Inserts the result in the symbol table */ 
	outEntry = ALLOC(Sl_GSTEntry_t, 1);
	outEntry -> name = util_strsav(outName);
	outEntry -> kind = Sl_INTF_c; 
	outEntry -> obj.intf.levels = levels; 
	st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
	return TCL_OK; 
    }
}
    


/**Function********************************************************************

  Synopsis           [Implements the command sl_print_intf_support. ] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

int SlPrintIntfSupportCmd (
		    ClientData clientData,
		    Tcl_Interp *interp,
		    int argc,
		    char** argv)
{
    /* boilerplate */ 
    char usage[] = "Usage: sl_print_intf_support <Intf>";
    char *IntfName;
    Sl_GSTEntry_t *IntfEntry;

    /* custom */ 
    array_t * levels; /* levels of the newly constructed interface */ 
    int i, j, var; 
    Level_t * level; 
    char *varName;
    mdd_t *conj; 
    array_t * support; 

    util_getopt_reset();
    util_optind++;
    if (argc != 2) {
	Main_MochaErrorPrint("Syntax error. %s\n", usage);
	return TCL_OK;
    }

    IntfName = argv[util_optind];
    if (!st_lookup(Sl_GST, IntfName, (char **) &IntfEntry)) {
	Main_MochaErrorPrint("Error: Object %s does not exist.\n", IntfName);
	return TCL_OK;
    }
    if (IntfEntry -> kind != Sl_INTF_c) {
	Main_MochaErrorPrint("Error: Object %s is not an interface.\n", IntfName);
	return TCL_OK;
    }

    Main_MochaPrint ("=====================================================\n"); 
    Main_MochaPrint ("Interface %s\n"); 
    Main_MochaPrint ("=====================================================\n"); 
    arrayForEachItem (Level_t *, IntfEntry -> obj.intf.levels, i, level) { 
	Main_MochaPrint ("Level %d  input vars:\n", i); 
	arrayForEachItem (int, level -> Ivars, j, var) { 
	    varName = Var_VariableReadName (SlReturnVariableFromId (var));
	    Main_MochaPrint (" %s", varName); 
	}
	Main_MochaPrint ("\nLevel %d output vars:\n", i); 
	arrayForEachItem (int, level -> Ovars, j, var) { 
	    varName = Var_VariableReadName (SlReturnVariableFromId (var));
	    Main_MochaPrint (" %s", varName); 
	}
	Main_MochaPrint ("\n"); 
	Main_MochaPrint ("Number of Iinit   Bdds: %d\n", array_n (level -> IinitBdds)); 
	Main_MochaPrint ("Number of Oinit   Bdds: %d\n", array_n (level -> OinitBdds)); 
	Main_MochaPrint ("Number of Iupdate Bdds: %d\n", array_n (level -> IupdateBdds)); 
	Main_MochaPrint ("Number of Oupdate Bdds: %d\n", array_n (level -> OupdateBdds)); 
	Main_MochaPrint ("----------------------------------\n"); 
	Main_MochaPrint ("Iinit:\n"); 
	arrayForEachItem (mdd_t *, level -> IinitBdds, j, conj) { 
	    Main_MochaPrint ("MDD %d: ", j); 
	    support = mdd_get_support(SlGetMddManager(), conj);
	    SlPrintVarNames (support); 
	    array_free (support); 
	}
	Main_MochaPrint ("----------------------------------\n"); 
	Main_MochaPrint ("Oinit:\n"); 
	arrayForEachItem (mdd_t *, level -> OinitBdds, j, conj) { 
	    Main_MochaPrint ("MDD %d: ", j); 
	    support = mdd_get_support(SlGetMddManager(), conj);
	    SlPrintVarNames (support); 
	    array_free (support); 
	}
	Main_MochaPrint ("----------------------------------\n"); 
	Main_MochaPrint ("Iupdate:\n"); 
	arrayForEachItem (mdd_t *, level -> IupdateBdds, j, conj) { 
	    Main_MochaPrint ("MDD %d: ", j); 
	    support = mdd_get_support(SlGetMddManager(), conj);
	    SlPrintVarNames (support); 
	    array_free (support); 
	}
	Main_MochaPrint ("----------------------------------\n"); 
	Main_MochaPrint ("Oupdate:\n"); 
	arrayForEachItem (mdd_t *, level -> OupdateBdds, j, conj) { 
	    Main_MochaPrint ("MDD %d: ", j); 
	    support = mdd_get_support(SlGetMddManager(), conj);
	    SlPrintVarNames (support); 
	    array_free (support); 
	}
    }
    Main_MochaPrint ("=====================================================\n"); 
    return TCL_OK; 
}


/******************************************************************************
                     STATIC FUNCTIONS BELOW
 ******************************************************************************/


/**Function********************************************************************

  Synopsis           [Creates the level list of variables from the merged 
                      dependency list.] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static array_t * make_levels (array_t *merged_dependency, array_t *Ivars, array_t *Ovars)
{
    array_t *levels = array_alloc (Level_t *, 0); 
    array_t *reversed_dependency = array_alloc (Pair_t *, 0);
    Int_Stack_t *frontier = SlStackAlloc (); 
    st_table *idPairTableForward  = st_init_table(st_numcmp, st_numhash);
    st_table *idPairTableBackward = st_init_table(st_numcmp, st_numhash);
    st_table *IvarsTable = st_init_table(st_numcmp, st_numhash); 
    Pair_t *pair, *newpair, *revpair, *forwpair, *dep_pair;
    Level_t *new_level; 
    int i, j, level; 
    int var, dummy; 
    int is_bottom, polarity, finished;
    
    /* First, reverses the merged dependency list, so that every variable
       is followed by the list of variables it depends on. */ 
    /* Copies the variables (with no dependency so far) into a new array,
       and generates the two hash tables. */ 
    arrayForEachItem (Pair_t *, merged_dependency, i, pair) {
	st_insert(idPairTableForward, (char *) (long) pair -> first, (char *) pair);
	newpair = SlPairAlloc (); 
	newpair -> info = (char *) array_alloc (Pair_t *, 0);
	newpair -> first = pair -> first; 
	array_insert_last (Pair_t *, reversed_dependency, newpair); 
	st_insert(idPairTableBackward, (char *) (long) newpair -> first, (char *) newpair);
    }
    /* Generates a hash table of input variables, to distinguish them from 
       output ones. */ 
    arrayForEachItem (int, Ivars, i, var) { 
	st_insert (IvarsTable, (char *) (long) var, (char *) 1); 
    }
    /* Now looks at all the links of the original array, and enters the 
       reversed versions in the reversed dependency list. */ 
    arrayForEachItem (Pair_t *, merged_dependency, i, pair) {
	arrayForEachItem (Pair_t *, (array_t *) pair -> info, j, dep_pair) { 
	    /* Adds the fact that dep_pair depends on pair */ 
	    st_lookup (idPairTableBackward, (char *) (long) dep_pair -> first, (char **) &revpair);
	    newpair = SlPairAlloc (); 
	    newpair -> first = pair -> first; 
	    array_insert_last (Pair_t *, (array_t *) revpair -> info, newpair); 
	}
    }

    /* Ok, now we have to produce the levels. */ 
    level    = 0; 
    finished = 0; 
    do { 
	level++; 
	/* Puts all vars that haven't yet been put into a level into the frontier */ 
	arrayForEachItem (Pair_t *, reversed_dependency, i, revpair) {
	    if (revpair -> flag == 0) { 
		frontier = SlStackPush (frontier, revpair -> first); 
	    }
	}
	/* If there are some variables in the frontier, creates a new level */ 
	if (!SlStackIsEmpty (frontier)) { 
	    new_level = SlLevelAlloc (); 
	    array_insert_last (Level_t *, levels, new_level); 
	} else { 
	    /* otherwise, we are done. */ 
	    finished = 1; 
	}
	/* Now works on the frontier, while it is not empty */ 
	while (!SlStackIsEmpty (frontier)) {
	    /* Picks a variable from the frontier */ 
	    frontier = SlStackPop (frontier, &var);
	    /* Finds the position in the reverse array, to find what it depends on. */ 
	    st_lookup (idPairTableBackward, (char *) (long) var, (char **) &revpair); 
	    /* If the variable is already in the levels, do nothing. */ 
	    if (revpair -> flag == 0) { 
		/* Checks whether it is input or output */ 
		if (st_lookup (IvarsTable, (char *) (long) var, (char**) &dummy)) { 
		    polarity = -1; /* input */ 
		} else { 
		    polarity = +1; /* output */ 
		}
		/* Checks if all the variables it depends upon are already in the level. */ 
		is_bottom = 1; 
		arrayForEachItem (Pair_t *, (array_t *) revpair -> info, j, pair) { 
		    /* pair -> next is the variable it depends upon. 
		       Looks in the idPairTableBackward table, to see what the variable really is */ 
		    st_lookup (idPairTableBackward, (char *) (long) pair -> first, (char **) &dep_pair); 
		    /* When we are at level 3, a flag is fine if it is +-1, +-2, or -3 (input) 
		       or +3 (output).
		       Remember: cannot be _more_ than level! */ 
		    if ((dep_pair -> flag == 0) 
			|| (dep_pair -> flag == - polarity * level)) { 
			/* The variable depends on something that is not yet in the levels. */ 
			is_bottom = 0; 
		    }
		}
		if (is_bottom) {
		    /* Ok, we have identified an input variable that must go into the current level. */ 
		    /* Flags it. */ 
		    revpair -> flag = polarity * level; 
		    /* And puts everything that depended on it on the stack, for good measure.
		       In order to do this, we need the other list. */ 
		    st_lookup (idPairTableForward, (char *) (long) var, (char **) &forwpair); 
		    arrayForEachItem (Pair_t *, (array_t *) forwpair -> info, j, pair) { 
			frontier = SlStackPush (frontier, pair -> first); 
		    }
		    /* Finally, we put the variable in the level list, input or output. */ 
		    if (polarity == 1) { 
			array_insert_last (int, new_level -> Ovars, var); 
		    } else { 
			array_insert_last (int, new_level -> Ivars, var); 
		    }
		} /* If is_bottom (it has to be inserted) */ 
	    } /* If the variable is not yet in the level list */ 
	} /* While the frontier is not empty */ 
    } while (!finished); 
    /* Great.  Now we should be done.  Deallocates most of the stuff that was allocated, 
       and returns the levels array. */ 
    SlFreeDependency (reversed_dependency); 
    /* Frees the symbol tables */ 
    st_free_table (idPairTableBackward); 
    st_free_table (idPairTableForward); 
    st_free_table (IvarsTable); 
    /* and returns the levels crated */ 
    return (levels); 
}


/**Function********************************************************************

  Synopsis           [Assigns each BDD to the right level, for the initial condition
                      and transition relation.  Duplicates the MDDs, so that 
		      free can be used later. ] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [Should I put a mdd_one in the levels that otherwise 
                      have no MDDs?  In order for the algorithms to be 
		      more homogeneous...]

******************************************************************************/

static void assign_levels (array_t *levels,
			   array_t *Iinit,  array_t *Oinit, 
			   array_t *Itrans, array_t *Otrans) 
{
    /* These arrays represent the variables that have not yet appeared in a level */ 
    array_t * Iinit_vars  = array_alloc (array_t *, 0); 
    array_t * Oinit_vars  = array_alloc (array_t *, 0); 
    array_t * Itrans_vars = array_alloc (array_t *, 0); 
    array_t * Otrans_vars = array_alloc (array_t *, 0); 
    /* Arrays of flags, telling us whether the conjuncts must be assigned a level */ 
    array_t * Iinit_vars_f  = array_alloc (int, 0); 
    array_t * Oinit_vars_f  = array_alloc (int, 0); 
    array_t * Itrans_vars_f = array_alloc (int, 0); 
    array_t * Otrans_vars_f = array_alloc (int, 0); 

    Level_t * level; 
    int mdd_n, level_n, flag; 
    mdd_t * conj; 
    array_t * vars, * bvars, * unprimed_vars, * newvars; 
    mdd_manager *mgr;
  
    /* Gets the BDD manager */ 
    mgr = SlGetMddManager ();
    /* Initializes the arrays to the variables occurring in each BDD. */ 
    arrayForEachItem (mdd_t *, Iinit, mdd_n, conj) { 
	vars  = mdd_get_support (mgr, conj); 
	bvars = filter_primed_and_unprime (vars); 
	array_free (vars); 
	array_insert_last (array_t *, Iinit_vars, bvars); 
	array_insert_last (int,       Iinit_vars_f, 1); 
    }
    arrayForEachItem (mdd_t *, Oinit, mdd_n, conj) { 
	vars  = mdd_get_support (mgr, conj); 
	bvars = filter_primed_and_unprime (vars); 
	array_free (vars); 
	array_insert_last (array_t *, Oinit_vars, bvars); 
	array_insert_last (int,       Oinit_vars_f, 1); 
    }
    arrayForEachItem (mdd_t *, Itrans, mdd_n, conj) { 
	vars  = mdd_get_support (mgr, conj); 
	bvars = filter_primed_and_unprime (vars); 
	array_free (vars); 
	array_insert_last (array_t *, Itrans_vars, bvars); 
	array_insert_last (int,       Itrans_vars_f, 1); 
    }
    arrayForEachItem (mdd_t *, Otrans, mdd_n, conj) { 
	vars  = mdd_get_support (mgr, conj); 
	bvars = filter_primed_and_unprime (vars); 
	array_free (vars); 
	array_insert_last (array_t *, Otrans_vars, bvars); 
	array_insert_last (int,       Otrans_vars_f, 1); 
    }
    /* Now loops for each level, subtracting the level variables of the level 
       to the ones of the conjuncts, until they can be assigned a level. */ 
    arrayForEachItem (Level_t *, levels, level_n, level) {
	/* Initial input */ 
	arrayForEachItem (array_t *, Iinit_vars, mdd_n, vars) { 
	    /* checks if the conjunct must still be assigned a level */ 
	    flag = array_fetch (int, Iinit_vars_f, mdd_n); 
	    if (flag) { 
		/* Yes.  Subtracts the variables of the level from those 
		   of the conjunct. */ 
		newvars = SlArrayDifference (vars, level -> Ivars); 
		array_free (vars); 
		vars = newvars; 
		newvars = SlArrayDifference (vars, level -> Ovars); 
		array_free (vars); 
		vars = newvars; 
		array_insert (array_t *, Iinit_vars, mdd_n, vars);  
		/* Checks if no variables remain */ 
		if (array_n (vars) == 0) { 
		    /* No. Marks the flag... */ 
		    array_insert (int, Iinit_vars_f, mdd_n, 0);  
		    /* ...and adds a dup of the conjunct to the proper level */ 
		    conj = array_fetch (mdd_t *, Iinit, mdd_n); 
		    array_insert_last  (mdd_t *, level -> IinitBdds, mdd_dup (conj)); 
		}
	    }
	}
	/* Initial output */ 
	arrayForEachItem (array_t *, Oinit_vars, mdd_n, vars) { 
	    /* checks if the conjunct must still be assigned a level */ 
	    flag = array_fetch (int, Oinit_vars_f, mdd_n); 
	    if (flag) { 
		/* Yes.  Subtracts the variables of the level from those 
		   of the conjunct. */ 
		newvars = SlArrayDifference (vars, level -> Ivars); 
		array_free (vars); 
		vars = newvars; 
		newvars = SlArrayDifference (vars, level -> Ovars); 
		array_free (vars); 
		vars = newvars; 
		array_insert (array_t *, Oinit_vars, mdd_n, vars);  
		/* Checks if no variables remain */ 
		if (array_n (vars) == 0) { 
		    /* No. Marks the flag... */ 
		    array_insert (int, Oinit_vars_f, mdd_n, 0);  
		    /* ...and adds a dup of the conjunct to the proper level */ 
		    conj = array_fetch (mdd_t *, Oinit, mdd_n); 
		    array_insert_last  (mdd_t *, level -> OinitBdds, mdd_dup (conj)); 
		}
	    }
	}
	/* Trans Input */ 
	arrayForEachItem (array_t *, Itrans_vars, mdd_n, vars) { 
	    /* checks if the conjunct must still be assigned a level */ 
	    flag = array_fetch (int, Itrans_vars_f, mdd_n); 
	    if (flag) { 
		/* Yes.  Subtracts the variables of the level from those 
		   of the conjunct. */ 
		newvars = SlArrayDifference (vars, level -> Ivars); 
		array_free (vars); 
		vars = newvars; 
		newvars = SlArrayDifference (vars, level -> Ovars); 
		array_free (vars); 
		vars = newvars; 
		array_insert (array_t *, Itrans_vars, mdd_n, vars);  
		/* Checks if no variables remain */ 
		if (array_n (vars) == 0) { 
		    /* No. Marks the flag... */ 
		    array_insert (int, Itrans_vars_f, mdd_n, 0);  
		    /* ...and adds a dup of the conjunct to the proper level */ 
		    conj = array_fetch (mdd_t *, Itrans, mdd_n); 
		    array_insert_last  (mdd_t *, level -> IupdateBdds, mdd_dup (conj)); 
		}
	    }
	} 
	/* Trans Output */ 
	arrayForEachItem (array_t *, Otrans_vars, mdd_n, vars) { 
	    /* checks if the conjunct must still be assigned a level */ 
	    flag = array_fetch (int, Otrans_vars_f, mdd_n); 
	    if (flag) { 
		/* Yes.  Subtracts the variables of the level from those 
		   of the conjunct. */ 
		newvars = SlArrayDifference (vars, level -> Ivars); 
		array_free (vars); 
		vars = newvars; 
		newvars = SlArrayDifference (vars, level -> Ovars); 
		array_free (vars); 
		vars = newvars; 
		array_insert (array_t *, Otrans_vars, mdd_n, vars);  
		/* Checks if no variables remain */ 
		if (array_n (vars) == 0) { 
		    /* No. Marks the flag... */ 
		    array_insert (int, Otrans_vars_f, mdd_n, 0);  
		    /* ...and adds a dup of the conjunct to the proper level */ 
		    conj = array_fetch (mdd_t *, Otrans, mdd_n); 
		    array_insert_last  (mdd_t *, level -> OupdateBdds, mdd_dup (conj)); 
		}
	    }
	}
    } /* levels */ 

    array_free (Iinit_vars); 
    array_free (Oinit_vars); 
    array_free (Itrans_vars); 
    array_free (Otrans_vars); 
    array_free (Iinit_vars_f); 
    array_free (Oinit_vars_f); 
    array_free (Itrans_vars_f); 
    array_free (Otrans_vars_f); 

}


/**Function********************************************************************

  Synopsis           [Filters the primed variables out of an array, and 
                      unprimes them. ] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static array_t * filter_primed_and_unprime (array_t * vars)
{
    array_t * result = array_alloc (int, 0); 
    int i, id, unprimed_id; 
    Pair_t * pair; 
    
    arrayForEachItem (int, vars, i, id) { 
	pair = array_fetch (Pair_t *, SlIdPairArray, id); 
	if (id == pair -> second) { 
	    /* The variable is primed.  Inserts the unprimed version 
	       in the result. */ 
	    array_insert_last (int, result, pair -> first); 
	}
    }
    return result; 
}


/**Function********************************************************************

  Synopsis           [Zips together variable levels into levels, and also appends 
                      the lists of conjuncts of input relations into 
		      pseudo_levels, and of output relations into levels.
		      Version useful for both refinement and composition. 
		      In composition, copies the variables in levels_p, 
		      the output conjuncts (merged from interfaces 1 and 2) 
		      in levels_p, and the input conjuncts (again merged)
		      in pseudo_levels_p. 
		      In refinement, copies all the variables in levels_p
		      (as in composition), but keeps the i/o conjuncts
		      of interface1 into levels_p, and those of interface2
		      into pseudo_levels_p. 
		      Additionally, in refinement it is tolerant of 
		      non-linearizable orders, since there's no direct game.
		      The variable mode indicates what to do: 
		      1 = composition
		      0 = refinement. ] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static int zip_variable_levels (int        mode,
				array_t ** levels_p, 
				array_t ** pseudo_levels_p, 
				array_t *  levels1, 
				array_t *  levels2)
{
    array_t * out_levels; 
    array_t * out_pseudo_levels; 
    int n_lev1, n_lev2, i_lev1, i_lev2; 
    array_t *vars1, *vars2, *shared_vars; 
    array_t *shared1, *shared2; 
    int i, j; 
    Level_t *lev, *lev1, *lev2, *new_lev, *new_pseudo_lev; 
    array_t *allvars_lev1, *allvars_lev2; 

    out_levels        = array_alloc (Level_t *, 0); 
    out_pseudo_levels = array_alloc (Level_t *, 0); 
    *levels_p         = out_levels; 
    *pseudo_levels_p  = out_pseudo_levels; 
    n_lev1 = array_n (levels1); 
    n_lev2 = array_n (levels2); 
    /* Computes the set shared_vars of shared variables of the two modules */ 
    vars1 = SlVarsOfAllLevels (levels1); 
    vars2 = SlVarsOfAllLevels (levels2); 
    shared_vars = SlArrayIntersection (vars1, vars2); 
    array_free (vars1); 
    array_free (vars2); 
    /* Now zips together the lists */ 
    i_lev1 = 0; 
    i_lev2 = 0; 
    while (i_lev1 < n_lev1 || i_lev2 < n_lev2) { 
	if (i_lev1 == n_lev1) { 
	    /* levels of 1 are done, copies levels of 2 */ 
	    lev2 = array_fetch (Level_t *, levels2, i_lev2); 
	    /* copies the vars in out_levels */ 
	    if (mode) { 
		copy_single_level_composition (2, out_levels, out_pseudo_levels, lev2); 
	    } else { 
		copy_single_level_refinement (2, out_levels, out_pseudo_levels, lev2); 
	    }
	    /* Done, move to next */ 
	    i_lev2++; 
	} else if (i_lev2 == n_lev2) { 
	    /* same, but for interface 1 */ 
	    lev1 = array_fetch (Level_t *, levels1, i_lev1); 
	    if (mode) { 
		copy_single_level_composition (1, out_levels, out_pseudo_levels, lev1); 
	    } else { 
		copy_single_level_refinement (1, out_levels, out_pseudo_levels, lev1); 
	    }
	    /* Done, move to next */ 
	    i_lev1++; 
	} else { 
	    /* i_lev1 and i_lev2 point to two valid levels. 
	       We must merge them, using the algorithm */ 
	    lev1         = array_fetch (Level_t *, levels1, i_lev1); 
	    lev2         = array_fetch (Level_t *, levels2, i_lev2); 
	    allvars_lev1 = SlArrayUnion (lev1 -> Ivars, lev1 -> Ovars); 
	    allvars_lev2 = SlArrayUnion (lev2 -> Ivars, lev2 -> Ovars); 
	    shared1      = SlArrayIntersection (allvars_lev1, shared_vars); 
	    shared2      = SlArrayIntersection (allvars_lev2, shared_vars); 
	    array_free (allvars_lev1); 
	    array_free (allvars_lev2); 
	    if (array_n (shared1) == 0 && array_n (shared2) == 0) { 
		/* No shared variables.  Ok if the variables at the level 
		   are all both inputs or both outputs. */ 
		if ((array_n (lev1 -> Ivars) == 0 && array_n (lev2 -> Ivars) == 0) ||
		    (array_n (lev1 -> Ovars) == 0 && array_n (lev2 -> Ovars) == 0)) { 
		    /* Compatible: all inputs or all outputs. */ 
		    if (mode) { 
			/* composition */ 
			merge_single_levels_composition (out_levels, out_pseudo_levels, lev1, lev2); 
		    } else { 
			/* refinement */ 
			merge_single_levels_refinement (out_levels, out_pseudo_levels, lev1, lev2); 
		    }
		} else if (mode == 0) { 
		    /* Refinement.  Compatible anyway... */ 
		    merge_single_levels_refinement (out_levels, out_pseudo_levels, lev1, lev2); 
		} else { 
		    assert (mode == 1); 
		    /* Composition.  Not compatible: signals error. */ 
		    array_free (shared1); 
		    array_free (shared2); 
		    array_free (shared_vars); 
		    SlFreeLevels (out_levels); 
		    SlFreeLevels (out_pseudo_levels); 
		    return 0; 
		}
	    } else if (array_n (shared1) == 0 && array_n (shared2) > 0) { 
		/* intf1 has private variables; they go now */ 
		if (mode) { 
		    copy_single_level_composition (1, out_levels, out_pseudo_levels, lev1); 
		} else { 
		    copy_single_level_refinement (1, out_levels, out_pseudo_levels, lev1); 
		}
	    } else if (array_n (shared1) > 0 && array_n (shared2) == 0) { 
		/* intf2 has private variables; they go now */ 
		if (mode) { 
		    copy_single_level_composition (2, out_levels, out_pseudo_levels, lev2); 
		} else { 
		    copy_single_level_refinement (2, out_levels, out_pseudo_levels, lev2); 
		}

	    } else { 
		/* both shared1 and shared2 are non-empty: 
		   the interfaces are composable iff shared1 = shared2. */ 
		if (SlAreArrayEqual (shared1, shared2)) { 
		    if (mode) { 
			/* composition */ 
			merge_single_levels_composition (out_levels, out_pseudo_levels, lev1, lev2); 
		    } else { 
			/* refinement */ 
			merge_single_levels_refinement (out_levels, out_pseudo_levels, lev1, lev2); 
		    }
		} else { 
		    /* Not compatible: signals error. */ 
		    array_free (shared1); 
		    array_free (shared2); 
		    array_free (shared_vars); 
		    SlFreeLevels (out_levels); 
		    SlFreeLevels (out_pseudo_levels); 
		    return 0; 
		}		    
	    }
	    array_free (shared1); 
	    array_free (shared2); 
	    i_lev1++; 
	    i_lev2++;
	} /* else there are two levels to be merged */ 
    } /* while levels to be merged */ 
    /* frees and returns success */ 
    array_free (shared_vars); 
    return 1; 
}


/**Function********************************************************************

   Synopsis           [Merges two single levels, taking the union of the input
                       and output variables, and bdds... called by zip_levels.
		       Version for composition. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/



static void merge_single_levels_composition (array_t * out_levels, 
					     array_t * out_pseudo_levels, 
					     Level_t * lev1, 
					     Level_t * lev2) 
{
    Level_t * new_lev; 
    Level_t * new_pseudo_lev; 
    array_t * vars, * bvars; 
    mdd_t   * one; 
    mdd_manager * mgr; 

    mgr            = SlGetMddManager ();
    new_lev        = SlLevelAlloc (); 
    new_pseudo_lev = SlLevelAlloc ();
    array_insert_last (Level_t *, out_levels, new_lev); 
    array_insert_last (Level_t *, out_pseudo_levels, new_pseudo_lev); 
    
    /* Vo = Vo1 union Vo2 */ 
    vars = SlArrayUnion (lev1 -> Ovars, lev2 -> Ovars); 
    /* use array_append because the array is already allocated */ 
    array_append (new_lev -> Ovars, vars);
    array_free (vars); 
    /* Vi = (Vi1 union Vi2) \ Vo */ 
    vars  = SlArrayUnion (lev1 -> Ivars, lev2 -> Ivars); 
    bvars = SlArrayDifference (vars, new_lev -> Ovars); 
    array_free (vars); 
    array_append (new_lev -> Ivars, bvars); 
    array_free (bvars); 
    /* Copies the transition predicates, duping the MDDs */ 
    /* from intf 1 */ 
    SlMddArrayAppend (new_pseudo_lev -> IinitBdds,   lev1 -> IinitBdds); 
    SlMddArrayAppend (new_lev        -> OinitBdds,   lev1 -> OinitBdds); 
    SlMddArrayAppend (new_pseudo_lev -> IupdateBdds, lev1 -> IupdateBdds); 
    SlMddArrayAppend (new_lev        -> OupdateBdds, lev1 -> OupdateBdds); 
    /* from intf 2 */ 
    SlMddArrayAppend (new_pseudo_lev -> IinitBdds,   lev2 -> IinitBdds); 
    SlMddArrayAppend (new_lev        -> OinitBdds,   lev2 -> OinitBdds); 
    SlMddArrayAppend (new_pseudo_lev -> IupdateBdds, lev2 -> IupdateBdds); 
    SlMddArrayAppend (new_lev        -> OupdateBdds, lev2 -> OupdateBdds); 
} 


/**Function********************************************************************

   Synopsis           [Merges two single levels, taking the union of the input
                       and output variables, and bdds... called by zip_levels.
		       Version used for refinement. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/



static void merge_single_levels_refinement (array_t * out_levels, 
					    array_t * out_pseudo_levels, 
					    Level_t * lev1, 
					    Level_t * lev2) 
{
    Level_t * new_lev; 
    Level_t * new_pseudo_lev; 
    mdd_t   * one; 
    mdd_manager * mgr; 

    mgr            = SlGetMddManager ();
    new_lev        = SlLevelAlloc (); 
    new_pseudo_lev = SlLevelAlloc ();
    array_insert_last (Level_t *, out_levels, new_lev); 
    array_insert_last (Level_t *, out_pseudo_levels, new_pseudo_lev); 
    
    /* Copies lev1 into out_levels, and lev2 into out_pseudo_levels */ 
    /* Variables. */ 
    array_append (new_lev -> Ivars,        lev1 -> Ivars); 
    array_append (new_lev -> Ovars,        lev1 -> Ovars); 
    array_append (new_pseudo_lev -> Ivars, lev2 -> Ivars); 
    array_append (new_pseudo_lev -> Ovars, lev2 -> Ovars); 
    /* Copies the transition predicates, duping the MDDs */ 
    /* from intf 1 */ 
    SlMddArrayAppend (new_lev        -> IinitBdds,   lev1 -> IinitBdds); 
    SlMddArrayAppend (new_lev        -> OinitBdds,   lev1 -> OinitBdds); 
    SlMddArrayAppend (new_lev        -> IupdateBdds, lev1 -> IupdateBdds); 
    SlMddArrayAppend (new_lev        -> OupdateBdds, lev1 -> OupdateBdds); 
    /* from intf 2 */ 
    SlMddArrayAppend (new_pseudo_lev -> IinitBdds,   lev2 -> IinitBdds); 
    SlMddArrayAppend (new_pseudo_lev -> OinitBdds,   lev2 -> OinitBdds); 
    SlMddArrayAppend (new_pseudo_lev -> IupdateBdds, lev2 -> IupdateBdds); 
    SlMddArrayAppend (new_pseudo_lev -> OupdateBdds, lev2 -> OupdateBdds); 
    /* adds a one to the empty arrays of BDDs */ 
    one = mdd_one (mgr); 
    if (!array_n (new_lev        -> IinitBdds))   { array_insert_last (mdd_t *, new_lev        -> IinitBdds,   mdd_dup (one)); }
    if (!array_n (new_lev        -> OinitBdds))   { array_insert_last (mdd_t *, new_lev        -> OinitBdds,   mdd_dup (one)); }
    if (!array_n (new_lev        -> IupdateBdds)) { array_insert_last (mdd_t *, new_lev        -> IupdateBdds, mdd_dup (one)); }
    if (!array_n (new_lev        -> OupdateBdds)) { array_insert_last (mdd_t *, new_lev        -> OupdateBdds, mdd_dup (one)); }
    if (!array_n (new_pseudo_lev -> IinitBdds))   { array_insert_last (mdd_t *, new_pseudo_lev -> IinitBdds,   mdd_dup (one)); }
    if (!array_n (new_pseudo_lev -> OinitBdds))   { array_insert_last (mdd_t *, new_pseudo_lev -> OinitBdds,   mdd_dup (one)); }
    if (!array_n (new_pseudo_lev -> IupdateBdds)) { array_insert_last (mdd_t *, new_pseudo_lev -> IupdateBdds, mdd_dup (one)); }
    if (!array_n (new_pseudo_lev -> OupdateBdds)) { array_insert_last (mdd_t *, new_pseudo_lev -> OupdateBdds, mdd_dup (one)); }
    mdd_free (one); 
}


/**Function********************************************************************

   Synopsis          [Copies a single level... called by zip_levels.
                      mode is as in zip_levels. 
		      Version for composition. ] 

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static void copy_single_level_composition (int       intf_n, 
					   array_t * out_levels, 
					   array_t * out_pseudo_levels, 
					   Level_t * lev)
{
    Level_t * new_lev; 
    Level_t * new_pseudo_lev;
    mdd_t   * one;
    Level_t * new_lev_dest; 
    Level_t * new_lev_fill; 

    mdd_manager * mgr; 

    mgr            = SlGetMddManager ();
    new_lev        = SlLevelAlloc (); 
    new_pseudo_lev = SlLevelAlloc ();
    array_insert_last (Level_t *, out_levels, new_lev); 
    array_insert_last (Level_t *, out_pseudo_levels, new_pseudo_lev); 
    /* use append because the array is already allocated */     
    array_append (new_lev -> Ovars, lev -> Ovars);
    array_append (new_lev -> Ivars, lev -> Ivars); 
    /* Copies the transition predicates, duping the MDDs */ 
    SlMddArrayAppend (new_pseudo_lev -> IinitBdds,   lev -> IinitBdds); 
    SlMddArrayAppend (new_lev        -> OinitBdds,   lev -> OinitBdds); 
    SlMddArrayAppend (new_pseudo_lev -> IupdateBdds, lev -> IupdateBdds); 
    SlMddArrayAppend (new_lev        -> OupdateBdds, lev -> OupdateBdds); 
}



/**Function********************************************************************

   Synopsis          [Copies a single level... called by zip_levels.
                      mode is as in zip_levels. 
		      intf_n is used to denote whether the source is 
		      intf1 or intf2.
		      Version for refinement. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static void copy_single_level_refinement (int       intf_n, 
					  array_t * out_levels, 
					  array_t * out_pseudo_levels, 
					  Level_t * lev)
{
    Level_t * new_lev; 
    Level_t * new_pseudo_lev;
    mdd_t   * one;
    Level_t * new_lev_dest; 
    Level_t * new_lev_fill; 

    mdd_manager * mgr; 

    mgr            = SlGetMddManager ();
    new_lev        = SlLevelAlloc (); 
    new_pseudo_lev = SlLevelAlloc ();
    array_insert_last (Level_t *, out_levels, new_lev); 
    array_insert_last (Level_t *, out_pseudo_levels, new_pseudo_lev); 
    /* switches where to insert */ 
    if (intf_n == 1) { 
	new_lev_dest = new_lev; 
	new_lev_fill = new_pseudo_lev; 
    } else { 
	assert (intf_n == 2); 
	new_lev_dest = new_pseudo_lev; 
	new_lev_fill = new_lev; 
    }	    
    /* use append because the array is already allocated */     
    array_append (new_lev_dest -> Ovars, lev -> Ovars);
    array_append (new_lev_dest -> Ivars, lev -> Ivars); 
    /* Copies the transition predicates, duping the MDDs */ 
    one = mdd_one (mgr); 
    SlMddArrayAppend (new_lev_dest -> IinitBdds,   lev -> IinitBdds); 
    SlMddArrayAppend (new_lev_dest -> OinitBdds,   lev -> OinitBdds); 
    SlMddArrayAppend (new_lev_dest -> IupdateBdds, lev -> IupdateBdds); 
    SlMddArrayAppend (new_lev_dest -> OupdateBdds, lev -> OupdateBdds); 
    /* and ones otherwise */ 
    array_insert_last (mdd_t *, new_lev_fill -> IinitBdds,   mdd_dup (one)); 
    array_insert_last (mdd_t *, new_lev_fill -> OinitBdds,   mdd_dup (one)); 
    array_insert_last (mdd_t *, new_lev_fill -> IupdateBdds, mdd_dup (one)); 
    array_insert_last (mdd_t *, new_lev_fill -> OupdateBdds, mdd_dup (one)); 
    mdd_free (one); 
}


/**Function********************************************************************

  Synopsis           [Composes two interfaces, computing the controllability.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static int game_compose_interfaces (array_t * levels, array_t * pseudo_levels)
{
    mdd_t * C, * newC, * T, * newT, * conjT;
    mdd_t * one, * zero; 
    mdd_manager * mgr;
    int n_iterations = 0; 
    int done = 0; 
    int n_levels, level_n; 
    int i, j;
    Level_t * lev, * pseudo_lev; 
    array_t * conj_array, * empty_array, * primed_vars; 
    mdd_t * in_conj, *not_in_conj, *next_not_conj, *not_result, *result;
    
    mgr         = SlGetMddManager ();
    empty_array = array_alloc (int, 0); 


    one         = mdd_one (mgr); 
    C           = mdd_one (mgr); 
    n_levels    = array_n (levels);
	
    /* Loop for computing the set of controllable states */ 
    do { 
	n_iterations++; 
	/* Deallocates the arrays from the input part of levels, in preparation
	   for re-computing it */
	arrayForEachItem (Level_t *, levels, i, lev) { 
	    arrayForEachItem (mdd_t *, lev -> IupdateBdds, j, in_conj) { 
		mdd_free (in_conj); 
	    }
	    array_free (lev -> IupdateBdds); 
	    /* Prepares the stub for the next use */ 
	    lev -> IupdateBdds = array_alloc (mdd_t *, 0); 
	}
	/* Now does the controllability, from the last iteration to the first. */ 
	T = mdd_dup (C); 
	for (level_n = n_levels - 1; level_n >= 0; level_n--) { 
	    lev        = array_fetch (Level_t *, levels,        level_n); 
	    pseudo_lev = array_fetch (Level_t *, pseudo_levels, level_n); 
	    /* Computes one by one the pieces of the new input transition relation 
	       at that level */ 
	    arrayForEachItem (mdd_t *, pseudo_lev -> IupdateBdds, i, in_conj) { 
		conj_array  = array_dup (lev -> OupdateBdds); 
		not_in_conj = mdd_not (in_conj); 
		array_insert_last (mdd_t *, conj_array, not_in_conj);
		primed_vars = SlPrimeVars (lev -> Ovars); 
		result      = Img_MultiwayLinearAndSmooth(mgr, conj_array, primed_vars, empty_array);
		not_result  = mdd_not (result); 
		mdd_free   (result); 
		mdd_free   (not_in_conj); 
		array_free (conj_array); 
		array_free (primed_vars); 
		/* Inserts only if not one */ 
		if (mdd_equal (one, not_result)) { 
		    mdd_free (not_result); 
		} else { 
		    array_insert_last (mdd_t *, lev -> IupdateBdds, not_result); 
		}
	    }
	    /* Now does the not T part */ 
	    conj_array    = array_dup (lev -> OupdateBdds); 
	    not_in_conj   = mdd_not (T);
	    next_not_conj = SlPrimeMdd (not_in_conj); 
	    mdd_free   (not_in_conj); 
	    array_insert_last (mdd_t *, conj_array, next_not_conj); 
	    primed_vars   = SlPrimeVars (lev -> Ovars); 
	    result        = Img_MultiwayLinearAndSmooth(mgr, conj_array, primed_vars, empty_array);
	    not_result    = mdd_not (result); 
	    mdd_free   (result); 
	    mdd_free   (next_not_conj); 
	    array_free (conj_array); 
	    array_free (primed_vars); 
	    if (mdd_equal (one, not_result)) { 
		mdd_free (not_result); 
	    } else { 
		array_insert_last (mdd_t *, lev -> IupdateBdds, not_result); 
	    }
	    /* Fine!  The new input restriction is computed.  
	       The new T is obtained by existentially quantifying the input variables. */ 
	    /* Normalizes the IupdateBdds: if it does not contain any conjunct, puts a 1. */ 
	    if (array_n (lev -> IupdateBdds) == 0) { 
		array_insert_last (mdd_t *, lev -> IupdateBdds, mdd_dup (one)); 
	    }
	    primed_vars = SlPrimeVars (lev -> Ivars); 
	    newT = Img_MultiwayLinearAndSmooth(mgr, lev -> IupdateBdds, primed_vars, empty_array);
	    array_free (primed_vars); 
	    mdd_free (T); 
	    T = newT; 
	}
	/* Strengthens T by conjoining it with C, thus ensuring that we do not get out
	   of the specification; leaves the result in newT. */ 
	newT = mdd_and (T, C, 1, 1); 
	mdd_free (T);
	/* At this point, newT is equal to the new value for C.
	   Checks if we are done. */ 
	if (mdd_equal (C, newT)) { 
	    /* They are equal: we are done, and we can break out of the loop for 
	       computing the set of controllable states. */ 
	    done = 1; 
	    mdd_free (newT); 
	} else { 
	    /* Not yet done.  Sets C to the new value, and iterates. */ 
	    mdd_free (C); 
	    C = newT; 
	}
    } while (!done); 
    /* At this point, the set of controllable states is left in C, and the initial update 
       relation is in C. 
       We must now compute (again using controllability) the new initial condition. 
       The loop is the same as for the update condition, except that it uses the 
       initial condition.  The treatment of next is the same. */ 
    T = mdd_dup (C); 
    for (level_n = n_levels - 1; level_n >= 0; level_n--) { 
	lev        = array_fetch (Level_t *, levels,        level_n); 
	pseudo_lev = array_fetch (Level_t *, pseudo_levels, level_n); 
	/* Computes one by one the pieces of the new input transition relation 
	   at that level */ 
	arrayForEachItem (mdd_t *, pseudo_lev -> IinitBdds, i, in_conj) { 
	    conj_array  = array_dup (lev -> OinitBdds); 
	    not_in_conj = mdd_not (in_conj); 
	    array_insert_last (mdd_t *, conj_array, not_in_conj);
	    primed_vars = SlPrimeVars (lev -> Ovars); 
	    result      = Img_MultiwayLinearAndSmooth(mgr, conj_array, primed_vars, empty_array);
	    not_result  = mdd_not (result); 
	    mdd_free   (result); 
	    mdd_free   (not_in_conj); 
	    array_free (conj_array); 
	    array_free (primed_vars); 
	    if (mdd_equal (one, not_result)) { 
		mdd_free (not_result); 
	    } else { 
		array_insert_last (mdd_t *, lev -> IinitBdds, not_result); 
	    }
	}
	/* Now does the not T part */ 
	conj_array    = array_dup (lev -> OinitBdds); 
	not_in_conj   = mdd_not (T);
	next_not_conj = SlPrimeMdd (not_in_conj); 
	mdd_free   (not_in_conj); 
	array_insert_last (mdd_t *, conj_array, next_not_conj); 
	primed_vars   = SlPrimeVars (lev -> Ovars); 
	result        = Img_MultiwayLinearAndSmooth(mgr, conj_array, primed_vars, empty_array);
	not_result    = mdd_not (result); 
	mdd_free   (result); 
	mdd_free   (next_not_conj); 
	array_free (conj_array); 
	array_free (primed_vars); 
	if (mdd_equal (one, not_result)) { 
	    mdd_free (not_result); 
	} else { 
	    array_insert_last (mdd_t *, lev -> IinitBdds, not_result); 
	}
	/* Fine!  The new input restriction is computed.  
	   The new T is obtained by existentially quantifying the input variables. */ 
	primed_vars = SlPrimeVars (lev -> Ivars); 
	/* Normalizes the IinitBdds: if it does not contain any conjunct, puts a 1. */ 
	if (array_n (lev -> IinitBdds) == 0) { 
	    array_insert_last (mdd_t *, lev -> IinitBdds, mdd_dup (one)); 
	}
	newT = Img_MultiwayLinearAndSmooth(mgr, lev -> IinitBdds, primed_vars, empty_array);
	mdd_free (T); 
	array_free (primed_vars); 
	T = newT; 
    }
    /* For the moment, we waste C, but we could print it for debugging / user info reasons */ 
    mdd_free (C); 
    /* Now checks if T is false.  If it is, then we cannot compose the two modules; 
       otherwise, we can. */ 
    zero = mdd_zero (mgr); 
    if (mdd_equal (T, zero)) { 
	/* Returns deallocating T, but both levels and pseudo_levels are returned, should they 
	   be useful to provide diagnostics. */ 
	mdd_free (T); 
	mdd_free (zero); 
	mdd_free (one); 
	return 0;
    } else { 
	/* No error */ 
	/* sanity check */ 
	assert (mdd_equal (T, one)); 
	mdd_free (T); 
	mdd_free (zero); 
	mdd_free (one); 
	return 1; 
    }
}
	
	
/**Function********************************************************************

  Synopsis           [Composes two interfaces.
                      Returns:
		      0: and fill in out_levels, if they are composable. 
		      1: if their variables are not composable (shared 
		         outputs, or non-composable levels)
		      2: not composable (no environment can avoid illegal states)]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static int compose_interfaces (array_t *level1, array_t *level2, array_t **out_levels) 
{
    array_t * levels, * pseudo_levels; 
    array_t * ovar1, * ovar2, * ovarshared; 
    int ok; 

    /* First, checks that there are no shared output variables. */ 
    ovar1      = SlOVarsOfAllLevels (level1);
    ovar2      = SlOVarsOfAllLevels (level2);
    ovarshared = SlArrayIntersection (ovar1, ovar2); 
    if (array_n (ovarshared)) { 
	/* there are shared output variables */ 
	return 1; 
    }
    array_free (ovar1); 
    array_free (ovar2);
    array_free (ovarshared); 
    /* Zips together the variable levels, checking if they are compatible */ 
    ok = zip_variable_levels (1, &levels, &pseudo_levels, level1, level2);
    *out_levels = levels; 
    if (!ok) { 
	return 1; 
    }
    /* And now does the main part of the job: computing the composition, checking
       compatibility, and producing the new input assumptions. */ 
    ok = game_compose_interfaces (levels, pseudo_levels);
    if (!ok) { 
	/* deallocates the levels: presently, we do not use them for guidance */ 
	SlFreeLevels (pseudo_levels); 
	return 2; 
    }
    return 0; 
}


/**Function********************************************************************

  Synopsis           [Checks whether intf2 refines intf1.
                      Returns: 
		      0: yes. 
		      1: if their variables do not match. 
		      2: no refinement relation. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/


static int check_interface_refinement (array_t * levels1, 
				       array_t * levels2, 
				       int       verbose) 
{
    int ok; 
    array_t * levels, * beta_levels; 
    array_t * ivars, * ovars, * v1, * v2, * iovars; 

    /* First, checks that there is no variable used as input in one module and 
       output in the other. */ 
    v1 = SlIVarsOfAllLevels (levels1); 
    v2 = SlIVarsOfAllLevels (levels2); 
    ivars = SlArrayUnion (v1, v2); 
    array_free (v1); 
    array_free (v2); 
    v1 = SlOVarsOfAllLevels (levels1); 
    v2 = SlOVarsOfAllLevels (levels2); 
    ovars = SlArrayUnion (v1, v2); 
    array_free (v1); 
    array_free (v2); 
    iovars = SlArrayIntersection (ivars, ovars); 
    array_free (ivars); 
    array_free (ovars); 
    if (array_n (iovars)) { 
	if (verbose) { 
	    Main_MochaErrorPrint ("Error: variables used both as input and output:\n"); 
	    SlPrintVarNames (iovars); 
	}
	array_free (iovars); 
	return 1; 
    }
    array_free (iovars); 
    /* Now checks the refinement */ 
    ok = zip_variable_levels (0, &levels, &beta_levels, levels1, levels2); 
    if (!ok) { 
	return 1; 
    }
    ok = refinement_check (levels, beta_levels, verbose); 
    if (!ok) { 
	SlFreeLevels (levels); 
	SlFreeLevels (beta_levels); 
	return 2; 
    }
    return 0; 
}


/**Function********************************************************************

  Synopsis           [Checks refinement between two interfaces, using 
                      the simple refinement check algorithm based on 
		      shared variables.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static int refinement_check (array_t * spec_levels, 
			     array_t * impl_levels, 
			     int verbose) 
{
    
    mdd_manager *mgr;
    mdd_t * R, * newT, * singleT; 
    array_t * T, * a_result, * conj_array;
    mdd_t * result, * not_result;
    Level_t * spec_lev, * impl_lev, * diff_lev;
    int level_n, n_levels, m_levels; 
    mdd_t * zero, * one;
    mdd_t * conj, * not_conj;
    int niterations = 0; 
    array_t * vardiffs;
    array_t * input_vars, * output_vars, * input_varsP, * output_varsP; 
    int ok, i; 
    int done = 0; 
    array_t * allvars_spec, * allvars_impl, * allvars; 

    mgr  = SlGetMddManager ();
    zero = mdd_zero (mgr); 
    one  = mdd_one (mgr); 
    n_levels = array_n (spec_levels);
    m_levels = array_n (impl_levels);
    assert (n_levels == m_levels); 

    /* Collects the variables, used to print bdd stats */ 
    allvars_spec = SlVarsOfAllLevels (spec_levels); 
    allvars_impl = SlVarsOfAllLevels (impl_levels); 
    allvars = SlArrayUnion (allvars_spec, allvars_impl); 
    array_free (allvars_spec); 
    array_free (allvars_impl); 

    /* Prepares a level array used only to hold the variable differences. */ 
    vardiffs  = array_alloc (Level_t *, 0); 
    for (level_n = 0; level_n < n_levels; level_n ++) { 
	spec_lev = array_fetch (Level_t *, spec_levels, level_n); 
	impl_lev = array_fetch (Level_t *, impl_levels, level_n); 
	diff_lev = SlLevelAlloc (); 
	/* Does the variable differences */ 
	input_vars   = SlArrayDifference (impl_lev -> Ivars, spec_lev -> Ivars); 
	output_vars  = SlArrayDifference (spec_lev -> Ovars, impl_lev -> Ovars); 
	input_varsP  = SlPrimeVars (input_vars); 
	output_varsP = SlPrimeVars (output_vars); 
	array_free (input_vars); 
	array_free (output_vars); 
	/* And stores them, by convention, as input variables. */ 
	array_append (diff_lev -> Ivars, input_varsP); 
	array_append (diff_lev -> Ivars, output_varsP); 
	array_free (input_varsP); 
	array_free (output_varsP); 
	/* Does the variable unions */ 
	input_vars   = SlArrayUnion (impl_lev -> Ivars, spec_lev -> Ivars);
	output_vars  = SlArrayUnion (impl_lev -> Ovars, spec_lev -> Ovars);
	input_varsP  = SlPrimeVars (input_vars); 
	output_varsP = SlPrimeVars (output_vars); 
	array_free (input_vars); 
	array_free (output_vars); 
	/* And stores them, by convention, as output variables. */ 
	array_append (diff_lev -> Ovars, input_varsP); 
	array_append (diff_lev -> Ovars, output_varsP); 
	array_free (input_varsP); 
	array_free (output_varsP); 
	/* Inserts the newly created level */ 
	array_insert_last (Level_t *, vardiffs, diff_lev); 
    }

    /* Now sets R = 1, to begin the iteration. */ 
    R = mdd_dup (one); 
    /* And goes back refining the relation, one level at a time. Pfew! */ 
    do { 
	niterations++; 
	/* We keep T as an array, as an optimization */ 
	T = SlSingleMddArray (SlPrimeMdd (R)); 
	for (level_n = n_levels - 1; level_n >= 0; level_n --) { 
	    /* Retrieves the levels */ 
	    spec_lev  = array_fetch (Level_t *, spec_levels, level_n); 
	    impl_lev  = array_fetch (Level_t *, impl_levels, level_n); 
	    diff_lev  = array_fetch (Level_t *, vardiffs,    level_n); 
	    /* Builds the array of conjoints for the first quantification. 
	       dups the mdds, so that it can free everything afterwards. */ 
	    SlMddArrayAppend (T, impl_lev -> IupdateBdds); 
	    SlMddArrayAppend (T, spec_lev -> OupdateBdds); 
	    /* remember that this time the result of the quantification is an array */ 
	    a_result = SlEArrayQuantify (T, diff_lev -> Ivars);
	    /* Frees the whole of T */ 
	    SlFreeMddList (T); 
	    /* Now computes the answer, by iterating on a_result, 
	       quantifying, and taking the disjunctions. */ 
	    T = array_alloc (mdd_t *, 0); 
	    arrayForEachItem (mdd_t *, a_result, i, conj) {
		if (verbose) { 
		    Main_MochaPrint ("    L1: size = %8d\n", mdd_size (conj)); 
		}
		conj_array = array_dup   (impl_lev -> OupdateBdds); 
		array_append (conj_array, spec_lev -> IupdateBdds); 
		not_conj = mdd_not (conj); 
		array_insert_last (mdd_t *, conj_array, not_conj); 
		result = SlEMddQuantify (conj_array, diff_lev -> Ovars); 
		if (verbose) { 
		    Main_MochaPrint ("    L2: size = %8d\n", mdd_size (conj)); 
		}
		mdd_free (not_conj); 
		array_free (conj_array); 
		not_result = mdd_not (result); 
		mdd_free (result); 
		/* Adds not_result to the list of T */ 
		array_insert_last (mdd_t *, T, not_result); 
	    }
	    /* Deallocates a_result */ 
	    SlFreeMddList (a_result); 
	    /* Ok, now we have the new T.  This concludes the level loop. */ 
	} /* for each level */ 
	/* Now, T can be a list.  To compare with R, we need a single mdd. */ 
	singleT = SlConjoin (T); 
	SlFreeMddList (T); 
	/* refines the relation */ 
	newT = mdd_and (R, singleT, 1, 1); 
	mdd_free (singleT); 
	/* prints, if it has to */ 
	if (verbose) { 
	    Main_MochaPrint ("At refinement iteration %d:\n", niterations); 
	    SlPrintBddStats (newT, allvars); 
	}
	/* At this point, checks if newT = R.  If so, we are done. */ 
	if (mdd_equal (newT, R)) { 
	    done = 1; 
	    mdd_free (newT); 
	} else { 
	    mdd_free (R); 
	    R = newT; 
	}
    } while (!done); 
    /* Fine.  Now we have to do a similar thing for the initial condition. */ 
    T = SlSingleMddArray (SlPrimeMdd (R)); 
    mdd_free (R); 
    for (level_n = n_levels - 1; level_n >= 0; level_n --) { 
	/* Retrieves the levels */ 
	spec_lev  = array_fetch (Level_t *, spec_levels, level_n); 
	impl_lev  = array_fetch (Level_t *, impl_levels, level_n); 
	diff_lev  = array_fetch (Level_t *, vardiffs,    level_n); 
	/* Builds the array of conjoints for the first quantification. 
	   dups the mdds, so that it can free everything afterwards. */ 
	SlMddArrayAppend (T, impl_lev -> IinitBdds); 
	SlMddArrayAppend (T, spec_lev -> OinitBdds); 
	/* remember that this time the result of the quantification is an array */ 
	a_result = SlEArrayQuantify (T, diff_lev -> Ivars);
	/* Frees the whole of T */ 
	SlFreeMddList (T); 
	/* Now computes the answer, by iterating on a_result, 
	   quantifying, and taking the disjunctions. */ 
	T = array_alloc (mdd_t *, 0); 
	arrayForEachItem (mdd_t *, a_result, i, conj) {
	    conj_array = array_dup   (impl_lev -> OinitBdds); 
	    array_append (conj_array, spec_lev -> IinitBdds); 
	    not_conj = mdd_not (conj); 
	    array_insert_last (mdd_t *, conj_array, not_conj); 
	    result = SlEMddQuantify (conj_array, diff_lev -> Ovars); 
	    mdd_free (not_conj); 
	    array_free (conj_array); 
	    not_result = mdd_not (result); 
	    mdd_free (result); 
	    /* Adds not_result to the list of T */ 
	    array_insert_last (mdd_t *, T, not_result); 
	}
	/* Deallocates a_result */ 
	SlFreeMddList (a_result); 
	/* Ok, now we have the new T.  This concludes the level loop. */ 
    } /* for each level */ 
    singleT = SlConjoin (T); 
    SlFreeMddList (T); 
    /* The refinement holds if we get 1 at the end */ 
    ok = mdd_equal (singleT, one); 
    /* Frees up everything */ 
    mdd_free (one); 
    mdd_free (zero); 
    array_free (allvars); 
    /* Frees up the arrays of variable differences and unions */ 
    SlFreeLevels (vardiffs); 
    
    return (ok); 
}


/**Function********************************************************************

  Synopsis           [Strengthens the input assumptions of an interface, 
                      ensuring that it satisfies a given invariant.
		      Returns the levels array of the stronger interfaces. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static int strengthen_interface (array_t **output_levels,
				 array_t * levels, 
				 array_t * spec,
				 int       verbose)
{
    mdd_t * C, * newC, * T, * newT, * conjT;
    mdd_t * one, * zero;
    mdd_manager * mgr;
    int n_iterations = 0; 
    int done = 0; 
    int n_levels, level_n; 
    int i, j;
    Level_t * lev, * out_lev; 
    array_t * conj_array, * empty_array, * primed_vars; 
    mdd_t * in_conj, *not_in_conj, *next_not_conj, *not_result, *result;
    array_t * out_levels; 

    mgr         = SlGetMddManager ();
    zero        = mdd_zero (mgr); 
    one         = mdd_one  (mgr); 
    empty_array = array_alloc (int, 0); 

    /* Controllability predicate */ 
    C = SlConjoin (spec); 
    n_levels    = array_n (levels);

    /* Copies levels into out_levels, except for the input conditions */ 
    out_levels = array_alloc (Level_t *, 0); 
    *output_levels = out_levels; 
    arrayForEachItem (Level_t *, levels, i, lev) { 
	out_lev = SlLevelAlloc (); 
	array_insert_last (Level_t *, out_levels, out_lev); 
	out_lev -> Ivars       = array_dup (lev -> Ivars); 
	out_lev -> Ovars       = array_dup (lev -> Ovars); 
	SlMddArrayAppend (out_lev -> OinitBdds,   lev -> OinitBdds); 
	SlMddArrayAppend (out_lev -> OupdateBdds, lev -> OupdateBdds); 
    }

    /* Now computes the weakest input restriction that ensures that the 
       invariant holds. */ 
    
    /* Loop for computing the set of controllable states */ 
    do { 
	n_iterations++; 
	/* Deallocates the arrays from the input part of levels, in preparation
	   for re-computing it */
	arrayForEachItem (Level_t *, out_levels, i, lev) { 
	    arrayForEachItem (mdd_t *, lev -> IupdateBdds, j, in_conj) { 
		mdd_free (in_conj); 
	    }
	    array_free (lev -> IupdateBdds); 
	    /* Prepares the stub for the next use */ 
	    lev -> IupdateBdds = array_alloc (mdd_t *, 0); 
	}
	/* Now does the controllability, from the last iteration to the first. */ 
	T = mdd_dup (C); 
	for (level_n = n_levels - 1; level_n >= 0; level_n--) { 
	    lev = array_fetch (Level_t *, out_levels, level_n); 
	    /* Does only the T part */ 
	    conj_array    = array_dup (lev -> OupdateBdds); 
	    not_in_conj   = mdd_not (T);
	    next_not_conj = SlPrimeMdd (not_in_conj); 
	    mdd_free   (not_in_conj); 
	    array_insert_last (mdd_t *, conj_array, next_not_conj); 
	    primed_vars   = SlPrimeVars (lev -> Ovars); 
	    result        = Img_MultiwayLinearAndSmooth(mgr, conj_array, primed_vars, empty_array);
	    not_result    = mdd_not (result); 
	    mdd_free   (result); 
	    mdd_free   (next_not_conj); 
	    array_free (conj_array); 
	    array_free (primed_vars); 
	    if (mdd_equal (one, not_result)) { 
		mdd_free (not_result); 
	    } else { 
		array_insert_last (mdd_t *, lev -> IupdateBdds, not_result); 
	    }
	    /* Fine!  The new input restriction is computed.  
	       The new T is obtained by existentially quantifying the input variables. */ 
	    /* Normalizes the IupdateBdds: if it does not contain any conjunct, puts a 1. */ 
	    if (array_n (lev -> IupdateBdds) == 0) { 
		array_insert_last (mdd_t *, lev -> IupdateBdds, mdd_dup (one)); 
	    }
	    primed_vars = SlPrimeVars (lev -> Ivars); 
	    newT = Img_MultiwayLinearAndSmooth(mgr, lev -> IupdateBdds, primed_vars, empty_array);
	    array_free (primed_vars); 
	    mdd_free (T); 
	    T = newT; 
	    /* And iterates for the next level */ 
	}

	/* Strengthens T by conjoining it with C, thus ensuring that we do not get out
	   of the specification; leaves the result in newT. */ 
	newT = mdd_and (T, C, 1, 1); 
	mdd_free (T);
	if (verbose) { 
	    Main_MochaPrint ("At iteration %d size = %8d\n", n_iterations, mdd_size (newT)); 
	}
	/* Checks if we are done. */ 
	if (mdd_equal (C, newT)) { 
	    /* They are equal: we are done, and we can break out of the loop for 
	       computing the set of controllable states. */ 
	    done = 1; 
	    mdd_free (newT); 
	} else { 
	    /* Not yet done.  Sets C to the new value, and iterates. */ 
	    mdd_free (C); 
	    C = newT; 
	}
    } while (!done); 
    /* At this point, the set of controllable states is left in C, and the initial update 
       relation is in C. 
       We must now compute (again using controllability) the new initial condition. 
       The loop is the same as for the update condition, except that it uses the 
       initial condition.  The treatment of next is the same. */ 
    T = mdd_dup (C); 
    for (level_n = n_levels - 1; level_n >= 0; level_n--) { 
	lev        = array_fetch (Level_t *, out_levels, level_n); 
	/* Does only the T part */ 
	conj_array    = array_dup (lev -> OinitBdds); 
	not_in_conj   = mdd_not (T);
	next_not_conj = SlPrimeMdd (not_in_conj); 
	mdd_free   (not_in_conj); 
	array_insert_last (mdd_t *, conj_array, next_not_conj); 
	primed_vars   = SlPrimeVars (lev -> Ovars); 
	result        = Img_MultiwayLinearAndSmooth(mgr, conj_array, primed_vars, empty_array);
	not_result    = mdd_not (result); 
	mdd_free   (result); 
	mdd_free   (next_not_conj); 
	array_free (conj_array); 
	array_free (primed_vars); 
	if (mdd_equal (one, not_result)) { 
	    mdd_free (not_result); 
	} else { 
	    array_insert_last (mdd_t *, lev -> IinitBdds, not_result); 
	}
	/* Fine!  The new input restriction is computed.  
	   The new T is obtained by existentially quantifying the input variables. */ 
	primed_vars = SlPrimeVars (lev -> Ivars); 
	/* Normalizes the IinitBdds: if it does not contain any conjunct, puts a 1. */ 
	if (array_n (lev -> IinitBdds) == 0) { 
	    array_insert_last (mdd_t *, lev -> IinitBdds, mdd_dup (one)); 
	}
	newT = Img_MultiwayLinearAndSmooth(mgr, lev -> IinitBdds, primed_vars, empty_array);
	mdd_free (T); 
	array_free (primed_vars); 
	T = newT; 
    }
    /* For the moment, we waste C, but we could print it for debugging / user info reasons */ 
    mdd_free (C); 
    /* Now checks if T is false.  If it is, then we cannot compose the two modules; 
       otherwise, we can. */ 
    if (mdd_equal (T, zero)) { 
	/* Returns deallocating T, but both levels and out_levels are returned, should they 
	   be useful to provide diagnostics. */ 
	mdd_free (T); 
	mdd_free (zero); 
	mdd_free (one); 
	return 0;
    } else { 
	/* No error */ 
	/* sanity check */ 
	assert (mdd_equal (T, one)); 
	mdd_free (T); 
	mdd_free (zero); 
	mdd_free (one); 
	/* Appends the original input restrictions */ 
	arrayForEachItem (Level_t *, levels, i, lev) { 
	    SlMddArrayAppend (out_lev -> IinitBdds,   lev -> IinitBdds); 
	    SlMddArrayAppend (out_lev -> IupdateBdds, lev -> IupdateBdds); 
	}
	/* returns */ 
	return 1; 
    }
}


/**Function********************************************************************

  Synopsis           [Prints a level array, useful for debugging.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/


static void print_levels_array (array_t *levels) 
{ 
    int i, j, var; 
    Level_t * level; 
    char *varName;

    arrayForEachItem (Level_t *, levels, i, level) { 
	Main_MochaPrint ("Level %d  input vars:\n", i); 
	arrayForEachItem (int, level -> Ivars, j, var) { 
	    varName = Var_VariableReadName (SlReturnVariableFromId (var));
	    Main_MochaPrint (" %s", varName); 
	}
	Main_MochaPrint ("\nLevel %d output vars:\n", i); 
	arrayForEachItem (int, level -> Ovars, j, var) { 
	    varName = Var_VariableReadName (SlReturnVariableFromId (var));
	    Main_MochaPrint (" %s", varName); 
	}
	Main_MochaPrint ("\n"); 
	Main_MochaPrint ("Number of Iinit   Bdds: %d\n", array_n (level -> IinitBdds)); 
	Main_MochaPrint ("Number of Oinit   Bdds: %d\n", array_n (level -> OinitBdds)); 
	Main_MochaPrint ("Number of Iupdate Bdds: %d\n", array_n (level -> IupdateBdds)); 
	Main_MochaPrint ("Number of Oupdate Bdds: %d\n", array_n (level -> OupdateBdds)); 
    }
}

/* 
 * $Log: slIntf.c,v $
 * Revision 1.30  2003/05/02 05:27:56  luca
 * Added Log keywords
 *
 * 
 */ 
