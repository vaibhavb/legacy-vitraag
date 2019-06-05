
/**CFile***********************************************************************

  FileName    [slFreddy.c]

  PackageName [sl]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [Luca de Alfaro & Freddy Mang]

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

/* to change merge await list */
/* make sure all externvarl and contrvarl are sorted */

#include "slInt.h" 

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct VarOrderStruct VarOrder_t;

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
struct VarOrderStruct {
  Pair_t * pair;
  int prime;
  array_t *bddIdArray;
};


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

static void MddGroupVariables(mdd_manager *mddMgr, int initMddId, int blockSize);
static void PrintsBddIds(array_t * moduleArray);
static int getNVals(int id);
static mdd_t * conjoin(array_t *bddArray);
static int compareInt(char *obj1, char *obj2);
static int comparePair(char **obj1, char **obj2);
static void mergeAwaitList(st_table * idToPairTable, array_t * list1, array_t * list2, Pair_t * pair);
static boolean traverse(array_t * dep);
static int traverse1(array_t * dep, Pair_t * pair, int * count);
/* Make the static function public.
 * static Pair_t * findPair(array_t * dep, int id);*/
static Pair_t * ObtainListOfSuccessors(array_t * dep, int x, Pair_t ** lastpair);
static Pair_t * DetermineControlledIdToRemove(Pair_t * hd);
static Pair_t * ObtainNonAwaitingElementsList(array_t * dep);
static boolean IdIsExternal(Sl_FSM_t fsm, int id);
static void SlWriteOrder(FILE *fp);
static array_t * VarBuildBddIdArray(Pair_t * pair, boolean primed);
static int IntegersCompare(char ** obj1, char ** obj2);
static int VarOrdersCompareBddIdArray(lsGeneric node1, lsGeneric node2);
static int VarOrdersCompareVariable(lsGeneric node1, lsGeneric node2);
static array_t * SlReadOrder(FILE *fp, array_t * moduleArray);
static int PairIsInArray(array_t *pairArray, Pair_t * pair);
static mdd_t * getMddFromLeftAndRightAtmExpr(Var_Variable_t * var, Atm_Expr_t * leftExpr, Atm_Expr_t * rightExpr);
static void PrintTransitiveAwaitVarsRecursively( char * varName,
                                                 Pair_t * pair);
static int parseNameEqValue(char *vv,  char ** name,  int * value);
static int ObtainUnPrimedVarName(char * varName, char ** unPrimedVarName);



/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Register Commands defined in this file.]

  Description        []

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlCreateCommandFreddy(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Tcl_CreateCommand(interp, "sl_fsm", SlFSMCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_printbdd", SlPrintBddCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_printvar", SlPrintVarCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_printsupport", SlPrintSupportCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  
  Tcl_CreateCommand(interp, "sl_pickminterm", SlPickMintermCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_write_order", SlWriteOrderCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_free_bdd", SlFreeBddCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_dump_bdd", SlDumpBddCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_prime", SlPrimeCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_getCtrlVarIds", SlGetCtrlVarIdsCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_eQuantifyIds", SlEQuantifyIdsCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_getMddSupport", SlGetMddSupportCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_createIntArray", SlCreateIntArrayCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_addToArray", SlAddToArrayCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_getVarNamesFromIds", SlGetVarNamesFromIdsCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_printObj", SlPrintObjCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_pre_n", SlPreNCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_printAwaitDep", SlPrintAwaitDepCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
		    
  Tcl_CreateCommand(interp, "sl_printTcAwaitDep", SlPrintTCAwaitDepCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_freeObj", SlFreeObjCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_makeStringArray", SlMakeStringArrayCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  
  Tcl_CreateCommand(interp, "sl_isStateInBdd", SlIsStateInMddCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_isVVStateInBdd", SlIsVVStateInMddCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_makeIntArray", SlMakeIntArrayCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_makeIdArray", SlMakeIdArrayCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_getIdFromVarName", SlGetIdFromVarNameCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_getVarNameFromId", SlGetVarNameFromIdCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  
  Tcl_CreateCommand(interp, "sl_getPrimedIdFromVarName", SlGetPrimedIdFromVarNameCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_getPrimedIdFromId", SlGetPrimedIdFromIdCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_getUnprimedIdFromId", SlGetPrimedIdFromIdCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_primeIdArray", SlPrimeIdArrayCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_unprimeIdArray", SlUnprimeIdArrayCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_toTcl", SlToTclCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "sl_getPartitionedTr", SlGetPartitionedTRCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

/*
    Tcl_CreateCommand(interp, "sl_bdd", SlBDDCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
                    */

  /* command used for debugging  */

  Tcl_CreateCommand(interp, "sl_printawaitvar", SlPrintAwaitVarCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);


  return TCL_OK;
}
		     

/**Function********************************************************************

  Synopsis           [Function implementing command sl_fsm.]

  Description        [This command generates fsm's for the modules. These
  fsm's will have the same set of mdd ids. It is assumed that the variables
  which have the same name will have the same mdd id. For now, there is no
  check that variables having the same names have the same type. Hence, it is
  the user's responsibility to make sure that the variables are consistent.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlFSMCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_fsm [-h | -s <filename>] <module> ...";
  char* moduleName;
  char* fileName = NIL(char);
  Sl_GSTEntry_t * gstEntry;
  Mdl_Manager_t * mdlManager;
  Mdl_Module_t * module;
  array_t * moduleArray;
  int i;
  Sym_Info_t * symInfo;
  lsGen lsgen;
  char c;
  array_t * varArray; /* not really. should be pair array  */
  
  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  /* check if the modules exist */
  moduleArray = array_alloc(Mdl_Module_t *, 0);
  mdlManager = SlGetModuleManager();

  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "hs:")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", usage);
          return TCL_OK;
          break;
        case 's':
          fileName = util_optarg;
          break;
        default:
          Main_MochaErrorPrint("%s\n", usage);
          return TCL_ERROR;
          break;
    }
  }
  
  while (util_optind != argc) {
    module = Mdl_ModuleReadFromName(mdlManager, argv[util_optind]);
    if (!module){
      Main_MochaErrorPrint("Error: module \"%s\" does not exists.\n",
                           argv[util_optind]);
      array_free(moduleArray);
      return TCL_OK;
    }

    array_insert_last(Mdl_Module_t *, moduleArray, module);
    util_optind ++;
  }
  

  /* check if bdd ids are built */
  arrayForEachItem(Mdl_Module_t *, moduleArray, i, module) {
    if (Sym_RegionManagerReadSymInfo(SlGetRegionManager(), module)) {
      Main_MochaErrorPrint(
        "Module \"%s\" already has a its own bdd ids. FSM not built.\n",
        Mdl_ModuleReadName(module));
      return TCL_OK;
    }        
  }

  /* read static order file */
  if (fileName != NIL(char)) {
    FILE * fp;
    
    Main_MochaPrint("Reading in static order file \"%s\"...\n", fileName);
    if (!(fp = fopen(fileName, "r"))) {
      Main_MochaPrint("Cannot read file \"%s\". Ignored.\n", fileName);
    } else {
      varArray = SlReadOrder(fp, moduleArray);
      fclose(fp);
      if (!varArray)
        return TCL_OK;
    }
  } else{
    varArray = array_alloc(Pair_t *, 0);
  }
  
  /* here mdd ids for the variables will be assigned */
  SlAssignMddIds(moduleArray, varArray);
  
  /* as a test, prints out all the bdd ids for module array */
  /* PrintsBddIds(moduleArray); */

  /* SlComputeTransitionRelations(moduleArray); */
  

  /* for each of the module, build the init states and transition relations */
  arrayForEachItem(Mdl_Module_t *, moduleArray, i, module) {
    array_t * initArray, * updateArray;
    mdd_t * atomMdd;
    Atm_Atom_t * atom;
    lsList atomList = Mdl_ModuleObtainSortedAtomList(module);
    char * newFSMName;
    char cmd[512];
    
    initArray = array_alloc(mdd_t *, 0);
    
    /* call sym_trans to build the transition relations as well as the imginfo
     */
    
    Main_MochaPrint("Building Transition Relation for module \"%s\"\n",
                    Mdl_ModuleReadName(module));
    sprintf(cmd, "sym_trans %s", Mdl_ModuleReadName(module));
    Tcl_Eval(SlGetTclInterp(), cmd);

    /* also build the initial transition relation */
    symInfo = Sym_RegionManagerReadSymInfo(SlGetRegionManager(), module);
    
    lsForEachItem(atomList, lsgen, atom) {
      array_insert_last(mdd_t *, initArray,
                        Sym_AtomBuildInitialRegion(symInfo, atom));
    }
    
    /* create a new gst entry  */
    newFSMName = util_strcat3("fsm_", Mdl_ModuleReadName(module), "");
    
    gstEntry = ALLOC(Sl_GSTEntry_t, 1);
    gstEntry -> kind = Sl_FSM_c;
    gstEntry -> obj.fsm.initBdd = initArray;
    gstEntry -> obj.fsm.updateBdd = Sym_SymInfoReadConjuncts(symInfo);
    gstEntry -> obj.fsm.erasedvarL = array_alloc(int, 0);
    gstEntry -> name = newFSMName;

    /* determine if the variables are external, or controllable */
    SlDetermineControllabilityOfVariables(module, gstEntry);

    /* add the await dependency information */
    SlAddAwaitDependencyToFSM(module, gstEntry);

    /* delete the old gst, add the new one  */
    SlDeleteGstEntry(newFSMName);
    SlInsertGstEntry(gstEntry);
    Main_MochaPrint("\"%s\" created for module \"%s\"\n",
                    newFSMName,
                    Mdl_ModuleReadName(module));
  }
  
  return TCL_OK;
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPrintBddCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_printbdd <bdd>";
  Sl_GSTEntry_t *inEntry;
  char * bddName;
  int i;
  mdd_t * mdd;
  
  
  util_getopt_reset();
  util_optind++;
  if (argc != 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  bddName = argv[util_optind];
  if (!st_lookup(Sl_GST, bddName, (char **) &inEntry )) {
    Main_MochaErrorPrint("Error: bdd %s does not exist.\n", bddName);
    return TCL_OK;
  }  

  if (inEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: bdd %s does not exist.\n", bddName);
    return TCL_OK;
  }
  
  arrayForEachItem(mdd_t *, inEntry->obj.bddArray, i, mdd) {
    SlMddPrintCubes(mdd, 0);
  }

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlToTclCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_toTcl <in_sl_structure>";
  Sl_GSTEntry_t *inEntry;
  char * slName, *s;
  int i, id, value;
  
  util_getopt_reset();
  util_optind++;
  if (argc != 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  slName = argv[util_optind];
  if (!st_lookup(Sl_GST, slName, (char **) &inEntry )) {
    Main_MochaErrorPrint("Error: bdd %s does not exist.\n", slName);
    return TCL_OK;
  }  

  Main_ClearResult();
  switch (inEntry -> kind) {
      case Sl_FSM_c:
      case Sl_BDD_c:
        /* only the names of are returned */
        Main_AppendResult("%s",  inEntry -> name);
        break;

      case Sl_IdArray_c:
        arrayForEachItem(int, inEntry -> obj.idArray, i, id) {
          Main_AppendElement("%d", id);
        }
        break;

      case Sl_IntArray_c:
        arrayForEachItem(int, inEntry -> obj.intArray, i, value) {
          Main_AppendElement("%d", value);
        }
        break;

      case Sl_StringArray_c:
        arrayForEachItem(char *, inEntry -> obj.intArray, i, s) {
          Main_AppendElement("%s", s);
        }
        break;
        
      default:
        Main_MochaErrorPrint("Unknown Type in sl_toTcl.\n");
        exit(1);
        
  }

  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlIsVVStateInMddCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  
  char usage[] = "Usage: sl_isVVStateInBDD <inbdd> <varName>=<value> ... ";
  char *inBddName;
  Sl_GSTEntry_t *inBddEntry;
  array_t * varNameArray;
  array_t * varIdArray;
  array_t * varValueArray;
  int value, i;
  char * name;
  

  util_getopt_reset();
  if (argc < 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  inBddName = argv[++util_optind];
  if (!st_lookup(Sl_GST, inBddName, (char **) &inBddEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inBddName);
    /* return TCL_OK; */
  }

  /*
  if(!GSTEntryIsBdd(inBddEntry)) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD\n", inBddName);
    return TCL_OK; 
  }
  */
  
  varNameArray  = array_alloc(char *, 0);
  varValueArray = array_alloc(int,    0);
  varIdArray    = array_alloc(int, 0);
  util_optind++;

  while (util_optind < argc) {
    if (!parseNameEqValue(argv[util_optind], &name, &value)) {
      Main_MochaErrorPrint("Warning: \"%s\" is not a <var>=<value> pair. Ignored.\n", 
			 argv[util_optind]);
    } else {
      array_insert_last(char *, varNameArray, name);
      array_insert_last(int, varValueArray, value);
    }
    util_optind++;
  }

  arrayForEachItem(char *, varNameArray, i, name) {
    char *unPrimedVarName;
    int isPrimed = ObtainUnPrimedVarName(name, &unPrimedVarName);
    int id;
    
    if (isPrimed)
      id = SlObtainPrimedBddId(unPrimedVarName);
    else 
      id = SlObtainUnprimedBddId(unPrimedVarName);

    FREE(unPrimedVarName);
    if (id != -1) {
      array_insert_last(int, varIdArray, id);
    } else {
      Main_MochaErrorPrint(
        "Warning: variable \"%s\" does not have an id yet. Igorned.\n",
        name);
    }

    FREE(name);
  }

  array_free(varNameArray);
  
  {
    mdd_manager * mddManager;
    mdd_t * mdd;
    boolean flag = TRUE;
    int i;
    
    Main_ClearResult();

    mddManager = SlGetMddManager();
  
    arrayForEachItem(mdd_t *, inBddEntry -> obj.bddArray, i, mdd) {
      mdd_t * s;

      flag = Sym_StateInMddWithResidueMdd(mddManager, mdd, varIdArray, varValueArray, &s);
      
      if ((flag == 0)) {
        array_t * tmp = mdd_get_support(SlGetMddManager(), mdd);
        int j, id;
        Var_Variable_t * var;
        
        Main_MochaPrint("The state violates the mdd with the following variables in support:\n");
        arrayForEachItem(int, tmp, j, id) {
          var = SlReturnVariableFromId(id);
          if (SlReturnPrimedId(id) == id) {
            Main_MochaPrint("%s' ", Var_VariableReadName(var));
          } else {
            Main_MochaPrint("%s ", Var_VariableReadName(var));
          }
        }

        array_free(tmp);
        Main_MochaPrint("\n");
        break;

      } else if (flag == 2) {

        Main_MochaPrint("Not enough information to determine. The residue mdd is:\n");
        SlMddPrintCubes(s, 0);
        Main_MochaPrint("\n");
        mdd_free(s);
        flag = 1;
      }
    }

    Main_AppendResult("%d", flag);
  }
  
  array_free(varIdArray);
  array_free(varValueArray);
  
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
int
SlIsStateInMddCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  
  char usage[] = "Usage: sl_isStateInBDD <inbdd> <inIdArray> <inIntArray>";
  char *inBddName, *inIdArrayName, *inIntArrayName; 
  Sl_GSTEntry_t *inBddEntry, *inIdArrayEntry, *inIntArrayEntry;
 
  util_getopt_reset();
  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  inBddName = argv[++util_optind];
  if (!st_lookup(Sl_GST, inBddName, (char **) &inBddEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inBddName);
    return TCL_OK;
  }

  if(!GSTEntryIsBdd(inBddEntry)) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD\n", inBddName);
    return TCL_OK;
  }
  
  inIdArrayName = argv[++util_optind];
  if (!st_lookup(Sl_GST, inIdArrayName, (char **) &inIdArrayEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inIdArrayName);
    return TCL_OK;
  }
  
  if (!GSTEntryIsIdArray(inIdArrayEntry)) {
    Main_MochaErrorPrint("Error: Object %s is not an Id Array.\n", inIdArrayName);
    return TCL_OK;
  }

  inIntArrayName = argv[++util_optind];
  if (!st_lookup(Sl_GST, inIntArrayName, (char **) &inIntArrayEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inIntArrayName);
    return TCL_OK;
  }
  
  if (!GSTEntryIsIntArray(inIntArrayEntry)) {
    Main_MochaErrorPrint("Error: Object %s is not an Int Array.\n", inIntArrayName);
    return TCL_OK;
  }

  {
    array_t * idArray, *intArray;
    mdd_manager * mddManager;
    mdd_t * mdd;
    boolean flag = TRUE;
    int i;
    
    Main_ClearResult();

    idArray    = inIdArrayEntry  -> obj.idArray;
    intArray   = inIntArrayEntry -> obj.intArray;
    mddManager = SlGetMddManager();
  
    arrayForEachItem(mdd_t *, inBddEntry -> obj.bddArray, i, mdd) {
      mdd_t * s;

      flag = Sym_StateInMddWithResidueMdd(mddManager, mdd, idArray, intArray, &s);
      
      if ((flag == 0)) {
        array_t * tmp = mdd_get_support(SlGetMddManager(), mdd);
        int j, id;
        Var_Variable_t * var;
        
        Main_MochaPrint("The state violates the mdd with the following variables in support:\n");
        arrayForEachItem(int, tmp, j, id) {
          var = SlReturnVariableFromId(id);
          if (SlReturnPrimedId(id) == id) {
            Main_MochaPrint("%s' ", Var_VariableReadName(var));
          } else {
            Main_MochaPrint("%s ", Var_VariableReadName(var));
          }
        }

        array_free(tmp);
        Main_MochaPrint("\n");
        break;

      } else if (flag == 2) {

        Main_MochaPrint("Not enough information to determine. The residue mdd is:\n");
        SlMddPrintCubes(s, 0);
        Main_MochaPrint("\n");
        mdd_free(s);
        flag = 1;
      }
    }

    Main_AppendResult("%d", flag);

    return TCL_OK;
  }
}



/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPickMintermCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_pickminterm <outbdd> <inbdd>";
  char *inObjName, *outObjName; 
  Sl_GSTEntry_t *inEntry, *outEntry;
  mdd_t * mdd;
  
  
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
    Main_MochaErrorPrint("Error: Bdd %s does not exist.\n", inObjName);
    return TCL_OK;
  }
  
  if (inEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Bdd %s does not exist.\n", inObjName);
    return TCL_OK;
  }

  /* conjoin all the bdds and picks one minterm */
  {
    mdd_t * tmp = conjoin (inEntry->obj.bddArray);
    mdd = SlPickMddMinterm(tmp);
    mdd_free(tmp);
  }
  
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> kind = Sl_BDD_c;
  outEntry -> obj.bddArray = array_alloc(mdd_t *, 0);
  array_insert_last(mdd_t *, outEntry -> obj.bddArray, mdd);
  outEntry -> name = util_strsav(outObjName);
  SlInsertGstEntry(outEntry);

  return TCL_OK;
  
}
  
  
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlBDDCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  /* takes a spec and returns a bdd */
  
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPrintVarCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_printvar <id>";
  Var_Variable_t * var;
  
  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  var = SlReturnVariableFromId(atoi(argv[1]));

  if (var == 0) {
    Main_MochaPrint("Error: %d is not a valid bdd id\n", atoi(argv[1]));
    return TCL_OK;
  }

  Main_MochaPrint("%s\n", Var_VariableReadName(var));

  return TCL_OK;
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetIdFromVarNameCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getIdFromVarName <varName>";
  int i, id;
  char * varName;
  char * unPrimedVarName;
  int isPrimed;

  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

 
  varName = util_strsav(argv[1]);
  
  isPrimed = ObtainUnPrimedVarName(varName, &unPrimedVarName);
  FREE(varName);
  
  if (isPrimed) {
    id = SlObtainPrimedBddId(unPrimedVarName);
  } else {
    id = SlObtainUnprimedBddId(unPrimedVarName);
  }
  
  FREE(unPrimedVarName);
  
  if (id == -1) {
    Main_MochaErrorPrint("Error: Var \"%s\" does not have a bdd id yet.\n",
                         varName);
  } 

  Main_ClearResult();
  Main_AppendResult("%d", id);
  
  return TCL_OK;
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetPrimedIdFromVarNameCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getPrimedIdFromVarName <varName>";
  int i, id;
  char * varName;

  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  varName = argv[1];

  id = SlObtainPrimedBddId(varName);
  
  if (id == -1) {
    Main_MochaErrorPrint("Error: Var \"%s\" does not have a bdd id yet.\n",
                         varName);
  } 

  Main_ClearResult();
  Main_AppendResult("%d", id);
  
  return TCL_OK;
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetPrimedIdFromIdCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getPrimedIdFromId <id>";
  int i, id;
  int primedId;

  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  id = atoi(argv[1]);

  primedId = SlReturnPrimedId(id);
  
  if (primedId == -1) {
    Main_MochaErrorPrint("Error: id \"%s\" is not a valid id. \n", argv[1]);
    return TCL_ERROR;
  }

  Main_ClearResult();
  Main_AppendResult("%d", primedId);
  
  return TCL_OK;
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPrimeIdArrayCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_primeIdArray <outIdArray> <inIdArray>";
  int i, id;
  int primedId;
  char *  inIdArrayName, *outIdArrayName;
  Sl_GSTEntry_t * inIdArrayEntry, * outEntry;
  array_t * inIdArray, *outIdArray;

  util_getopt_reset();
  if (argc < 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  util_optind++;
  outIdArrayName = argv[util_optind];
  if (st_is_member(Sl_GST, outIdArrayName)) {
    Main_MochaErrorPrint("Error: Object %s already exist.\n", outIdArrayName);
    return TCL_OK;
  }

  util_optind++;
  inIdArrayName = argv[util_optind];
  if (!st_lookup(Sl_GST, inIdArrayName, (char **) &inIdArrayEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inIdArrayName);
    return TCL_OK;
  }
  
  if (!GSTEntryIsIdArray(inIdArrayEntry)) {
    Main_MochaErrorPrint("Error: Object %s is not an Id Array.\n", inIdArrayName);
    return TCL_OK;
  }
  
  inIdArray    = inIdArrayEntry  -> obj.idArray;
  outIdArray = array_alloc(int, 0);
  arrayForEachItem(int, inIdArray, i, id) {
    primedId = SlReturnPrimedId(id);
    if (primedId == -1) {
      Main_MochaErrorPrint("Error: id \"%d\" is not a valid id. \n", id);
      array_free(outIdArray);
      return TCL_ERROR;
    } 
    array_insert_last(int, outIdArray, primedId);
  }

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outIdArrayName);
  outEntry -> kind = Sl_IdArray_c;   
  outEntry -> obj.idArray = outIdArray; 

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  Main_ClearResult();
  Main_AppendResult("%s", outIdArrayName);
  return TCL_OK;

}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlUnprimeIdArrayCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_unprimeIdArray <outIdArray> <inIdArray>";
  int i, id;
  int primedId;
  char *  inIdArrayName, *outIdArrayName;
  Sl_GSTEntry_t * inIdArrayEntry, * outEntry;
  array_t * inIdArray, *outIdArray;

  util_getopt_reset();
  if (argc < 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  util_optind++;
  outIdArrayName = argv[util_optind];
  if (st_is_member(Sl_GST, outIdArrayName)) {
    Main_MochaErrorPrint("Error: Object %s already exist.\n", outIdArrayName);
    return TCL_OK;
  }

  util_optind++;
  inIdArrayName = argv[util_optind];
  if (!st_lookup(Sl_GST, inIdArrayName, (char **) &inIdArrayEntry)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inIdArrayName);
    return TCL_OK;
  }
  
  if (!GSTEntryIsIdArray(inIdArrayEntry)) {
    Main_MochaErrorPrint("Error: Object %s is not an Id Array.\n", inIdArrayName);
    return TCL_OK;
  }
  
  inIdArray    = inIdArrayEntry  -> obj.idArray;
  outIdArray = array_alloc(int, 0);
  arrayForEachItem(int, inIdArray, i, id) {
    primedId = SlReturnUnprimedId(id);
    if (primedId == -1) {
      Main_MochaErrorPrint("Error: id \"%d\" is not a valid id. \n", id);
      array_free(outIdArray);
      return TCL_ERROR;
    } 
    array_insert_last(int, outIdArray, primedId);
  }

  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outIdArrayName);
  outEntry -> kind = Sl_IdArray_c;   
  outEntry -> obj.idArray = outIdArray; 

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  Main_ClearResult();
  Main_AppendResult("%s", outIdArrayName);
  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetUnprimedIdFromIdCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getUnprimedIdFromId <id>";
  int i, id;
  int unprimedId;

  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  id = atoi(argv[1]);

  unprimedId = SlReturnUnprimedId(id);
  
  if (unprimedId == -1) {
    Main_MochaErrorPrint("Error: id \"%s\" is not a valid id. \n", argv[1]);
    return TCL_ERROR;
  }

  Main_ClearResult();
  Main_AppendResult("%d", unprimedId);
  
  return TCL_OK;
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPrintSupportCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_printsupport <bdd>";
  array_t * supportId;
  int i, id;
  char * bddName;
  Sl_GSTEntry_t *  obj;
  mdd_t * mdd;
  
  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  bddName = argv[1];
  if (!st_lookup(Sl_GST, bddName, (char **) &obj)) {
    Main_MochaErrorPrint("Error: Bdd %s does not exist.\n", bddName);
    return TCL_OK;
  }

  if (obj -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Bdd %s does not exist.\n", bddName);
    return TCL_OK;    
  }


  supportId = array_alloc(int, 0);
  arrayForEachItem(mdd_t *, obj -> obj.bddArray, i, mdd) {
    array_t * tmp = mdd_get_support(SlGetMddManager(), mdd);
    array_append(supportId, tmp);
    array_free(tmp);
  }

  array_sort(supportId, compareInt);
  array_uniq(supportId, compareInt, 0);

  arrayForEachItem(int, supportId, i, id) {
    Var_Variable_t * var = SlReturnVariableFromId(id);
    
    Main_MochaPrint("%s\n", Var_VariableReadName(var));
  }

  array_free(supportId);
  return TCL_OK;
}



/**Function********************************************************************

  Synopsis           [utility to pick a minterm form a MDD]

  Description        [utility to pick a minterm form a MDD]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
mdd_t  *
SlPickMddMinterm(
  mdd_t *mdd)
{
  int id, nvals, i;
  mdd_t *literal;
  mdd_t *rest, *result, *temp;
  
  if(mdd_is_tautology(mdd, 1)){
    return mdd_one(SlGetMddManager());
  }

  if(mdd_is_tautology(mdd, 0)){
    Main_MochaErrorPrint( "Error in Sym_MddPickMinterm\n");
  }

  id = SlGetTopMddId(mdd);
  nvals = getNVals(id);

  for ( i = 0; i < nvals; i++){
    literal = SlGetLiteral(id,i);
    temp = mdd_cofactor(SlGetMddManager(), mdd, literal);
    if(!mdd_is_tautology(temp, 0)){
      rest =SlPickMddMinterm(temp);
      result = mdd_and(literal, rest, 1, 1);
      
      mdd_free(rest);
      mdd_free(temp);
      mdd_free(literal);
      return(result);
    }
    
    mdd_free(temp);
    mdd_free(literal);
  }
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlAssignMddIds(
  array_t * moduleArray,
  array_t * varArray
  )
{
  /* first, build syminfo for each modules */
  Mdl_Module_t * module;
  int i;
  Sym_RegionManager_t * regionManager = SlGetRegionManager();
  Tcl_Interp * interp = SlGetTclInterp();
  mdd_manager * mddManager = SlGetMddManager();
  Var_Variable_t * var;
  Sym_Info_t * symInfo;
  Pair_t * pair;
  
  arrayForEachItem(Mdl_Module_t *, moduleArray, i, module) {
    symInfo = Sym_SymInfoAlloc(interp, module, mddManager);
    Sym_RegionManagerAddModuleToSymInfo(regionManager, module, symInfo);
  }
  
  arrayForEachItem(Pair_t *, varArray, i, pair) {
    var = (Var_Variable_t *) pair -> info;
    module = (Mdl_Module_t *) Var_VariableReadModule(var);
    symInfo = Sym_RegionManagerReadSymInfo(regionManager, module);
    
    SlAssignMddIdsForVariable(symInfo, var);
  }
    
  SlAssignMddIdsForEachModule(moduleArray);

}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlAssignMddIdsForEachModule(
  array_t * moduleArray
  )
{
  /* create a table storing the variables that have been assigned an id */
  Mdl_Module_t * module;
  st_table * varNameToIds;
  lsGen lsgen;
  int i;

  /* loop through the modules, and create the ids */
  arrayForEachItem(Mdl_Module_t *, moduleArray, i, module) {
    Var_Variable_t * var;
    lsList varL = Mdl_ModuleObtainVariableList(module);
    Sym_Info_t * symInfo = Sym_RegionManagerReadSymInfo(SlGetRegionManager(), module);  
    
    /* should check if the variables in this module have the same types as
       those whose bdd's are assigned before. */
    
    lsForEachItem(varL, lsgen, var){
      SlAssignMddIdsForVariable(symInfo, var);
    }

    /* group the bdd ids for this module together */
    {
      st_generator * stgen;
      mdd_manager * mddManager = SlGetMddManager();
      st_table * primedTable = Sym_SymInfoReadVarToPrimedId(symInfo);
      st_table * unprimedTable = Sym_SymInfoReadVarToUnprimedId(symInfo);
      long unprimedMddId, primedMddId;
      
      st_foreach_item(unprimedTable, stgen,
                      (char **) &var, (char **) &unprimedMddId){
        st_lookup(primedTable, (char *) var, (char **) &primedMddId); 
        if (unprimedMddId == primedMddId + 1) 
          MddGroupVariables(mddManager, primedMddId, 2); 
        else if (primedMddId == unprimedMddId + 1) 
          MddGroupVariables(mddManager, unprimedMddId, 2);   
      }
    }
  }
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlAssignMddIdsForVariable(
  Sym_Info_t * symInfo,
  Var_Variable_t * var
  )
{
  Pair_t * pair;
  mdd_manager * mddManager = SlGetMddManager();
  boolean isUnprimed = FALSE;
  st_table * varNameToId = SlGetNameToIdTable();
  boolean found = st_lookup(varNameToId, Var_VariableReadName(var),
                            (char **) &pair);
  
  if (found) {
    if ((Var_Variable_t *) pair -> info == var) {
      return;
    }
  }
  
  if (!found)
    pair = SlPairAlloc();
  
  do {
    int mddId;
    array_t * mvarValues;
    array_t * mvarNames;
    array_t * mvarArray;
    Var_DataType dataType;
    Var_Type_t * varType;

    isUnprimed = !isUnprimed;
    
    if (!found) {
      mvarValues = array_alloc(int, 0);
      mvarNames  = array_alloc(char *, 0);
      
      varType = Var_VariableReadType (var);
      dataType = Var_VarTypeReadDataType (varType);
      array_insert_last(char *, mvarNames, Var_VariableReadName(var));
      
      switch (dataType){
          case Var_Boolean_c:
          case Var_Event_c:
            array_insert_last(int, mvarValues, 2);
            break;
            
          case Var_Enumerated_c:
          case Var_Range_c:
          case Var_Timer_c:
            array_insert_last(int,
                              mvarValues,
                              Var_VarTypeReadDomainSize(varType));
            break;
          default:
            Main_MochaErrorPrint(
              "Cannot create MDD variable for nat/int/unknown data types %s\n",
              Var_VariableReadName(var));
            exit(1);
      }
      
      mvarArray = mdd_ret_mvar_list(mddManager);
      mddId = array_n(mvarArray);
      mdd_create_variables(mddManager, mvarValues, mvarNames, NIL(array_t));

      array_free(mvarValues);
      array_free(mvarNames);
            
    } else {
      mddId = isUnprimed? (pair -> first) : (pair->second);
    }
  
    if (isUnprimed) {
      /* unprimed id */
      st_insert(Sym_SymInfoReadVarToUnprimedId(symInfo),
                (char *) var,
                (char*)(long) mddId);

      array_insert_last(int,
                        Sym_SymInfoReadUnprimedIdArray(symInfo),
                        mddId);
      st_insert(Sym_SymInfoReadIdToVar(symInfo),
                (char *)(long) mddId,
                (char *) var);

      pair -> first = mddId;

    } else {
      /* primed id */
      st_insert(Sym_SymInfoReadVarToPrimedId(symInfo),
                (char *) var,
                (char*)(long) mddId);
      array_insert_last(int,
                        Sym_SymInfoReadPrimedIdArray(symInfo),
                        mddId);
      st_insert(Sym_SymInfoReadIdToVar(symInfo),
                (char *)(long) mddId, (char *) var);

      pair -> second = mddId;
    }
  } while (isUnprimed);

  if (!found) {
    st_insert(varNameToId, Var_VariableReadName(var), (char *) pair);

    /* also put the variable in the info field */
    pair -> info = (char *) var;
    SlAddIdPair(pair);
  }
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlDetermineControllabilityOfVariables(
  Mdl_Module_t * module,
  Sl_GSTEntry_t * gstEntry)
{
  int id, i;
  array_t * idArray;
  Var_Variable_t * var;
  
  Sym_Info_t * symInfo =
      Sym_RegionManagerReadSymInfo(SlGetRegionManager(), module);

  st_table * table = Sym_SymInfoReadIdToVar(symInfo);
  
  gstEntry -> obj.fsm.contrvarL = array_alloc(int, 0);
  gstEntry -> obj.fsm.extvarL = array_alloc(int, 0);

  idArray = Sym_SymInfoReadUnprimedIdArray(symInfo);

  arrayForEachItem(int, idArray, i, id) {
    int flag;

    flag = st_lookup(table, (char *) (long) id, (char **) & var);
    assert (flag);
    if (Var_VariableReadPEID(var) == 1) {
      array_insert_last(int, gstEntry -> obj.fsm.extvarL, id);      
    } else {
      array_insert_last(int, gstEntry -> obj.fsm.contrvarL, id);
    }
  }

  /* sort the ids to make search easier */
  array_sort(gstEntry -> obj.fsm.extvarL, compareInt);
  array_sort(gstEntry -> obj.fsm.contrvarL, compareInt);
  
}



/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlObtainUnprimedBddId(
  char * name
  )
{
  Pair_t * pair;
  
  if (!st_lookup(SlGetNameToIdTable(), name, (char **) &pair))
    return -1;
  else
    return (pair -> first);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlObtainPrimedBddId(
  char * name
  )
{
  Pair_t * pair;
  
  if (!st_lookup(SlGetNameToIdTable(), name, (char **) &pair))
    return -1;
  else
    return (pair -> second);
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlObtainBddId(
  char * name
  )
{
  Pair_t * pair;
  char *unprimedName;
  int length = strlen(name);

  boolean isPrimed = (*(name+length-1) == '\'');

  if (!isPrimed) {
    unprimedName = ALLOC(char, length + 1);
    memcpy(unprimedName, name, length);
    *(unprimedName + length) = '\0';
  } else {
    unprimedName = ALLOC(char, length );
    memcpy(unprimedName, name, length - 1);
    *(unprimedName + length - 1) = '\0';
  }
  
  if (!st_lookup(SlGetNameToIdTable(), name, (char **) &pair))
    return -1;
  else
    return isPrimed? (pair -> second): (pair -> first);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlMddPrintCubes(
  mdd_t *mdd,
  int level
 )
{
  mdd_t *s;
  int  id;
  char *varName;
  boolean ife = FALSE;
  int nvals, i;
  mdd_t *literal;
  Var_Variable_t *var;
  Var_DataType dataType;
  char * indentation;
  
  /* bypass printing 1s unnecessarily */
  if(mdd_is_tautology(mdd,1) && level > 0){
    return;
  }

  level++;

  Main_MochaPrint("( ");

  if(mdd_is_tautology(mdd,0)){
    Main_MochaPrint("0");
  }
  else if(mdd_is_tautology(mdd,1)){
    Main_MochaPrint("1");
  }
  else{
    char pOrUnp[2] = "'";

    id = SlGetTopMddId(mdd);

    if (!(var = SlReturnVariableFromId(id))) {
      Main_MochaErrorPrint("Error: No var for id : %d\n", id);
      exit (1);
    }

    if (SlReturnUnprimedId(id) == id) {
      pOrUnp[0] = '\0';
    }

    varName = Var_VariableReadName(var);
    dataType = Var_VariableReadDataType(var);
    
    nvals = getNVals(id);
    
    indentation = ALLOC(char, level*2);
    for( i=0; i<level*2; i++) {
      *(indentation+i) = ' ';
    }
    *(indentation + 2*level -1) = 0;
    
    for ( i =0; i < nvals; i++){

      literal = SlGetLiteral(id, i);
      
      s = mdd_cofactor(SlGetMddManager(), mdd, literal);

      if( !mdd_is_tautology(s, 0)){
        if(ife)
          Main_MochaPrint( "\n%s + ", indentation);

        if(dataType == Var_Enumerated_c){
          Main_MochaPrint( "\n%s(%s%s = %s)", indentation, varName, pOrUnp,
                           Var_EnumElementReadName(
                             Var_VariableObtainEnumElementFromIndex(var, i)));
        }
        else
          Main_MochaPrint( "\n%s(%s%s = %d)", indentation, varName,  pOrUnp, i);
        
        ife = TRUE;
        SlMddPrintCubes(s , level);
      }
    }
    
    FREE(indentation);
  }
  

  
  level--;

  Main_MochaPrint(") ");
  
  if(level == 0)
    Main_MochaPrint("\n");
}


/**Function********************************************************************

  Synopsis           [Dummy placeholder.] 

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

array_t* 
SlPrimeVars (array_t *unprimedIdArray)
{
  int i, id, primedId;
  array_t * primedIdArray = array_alloc(int, 0);

  arrayForEachItem(int, unprimedIdArray, i, id) {
    primedId = SlReturnPrimedId(id);
    if (primedId == -1) {
      Main_MochaErrorPrint(
        "Error in SlPrimeVars: bdd id not found, or bdd has no primed version\n");
      exit(1);
    }
    
    array_insert_last(int, primedIdArray, primedId);
  }

  return primedIdArray;

}

/**Function********************************************************************

  Synopsis           [Dummy placeholder.] 

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

array_t* 
SlUnprimeVars (array_t *primedIdArray)
{
  int i, id, unprimedId;
  array_t * unprimedIdArray = array_alloc(int, 0);

  arrayForEachItem(int, primedIdArray, i, id) {
    unprimedId = SlReturnUnprimedId(id);
    if (unprimedId == -1) {
      Main_MochaErrorPrint("Error in SlUnprimeVars: bdd id not found, or bdd has no unprimed version\n");
      exit(1);
    }
    
    array_insert_last(int, unprimedIdArray, unprimedId);
  }

  return unprimedIdArray;

}

/**Function********************************************************************

  Synopsis           [Dummy placeholder.] 

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

mdd_t* 
SlPrimeMdd (mdd_t* m)
{
  
  array_t * supportId = mdd_get_support(SlGetMddManager(), m);
  array_t * primedId = SlPrimeVars (supportId);
  mdd_t * primedMdd = mdd_substitute(SlGetMddManager(), m, supportId, primedId);

  array_free(supportId);
  array_free(primedId);
  
  return primedMdd;

}


/**Function********************************************************************

  Synopsis           [Dummy placeholder.] 

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

mdd_t* 
SlUnprimeMdd (mdd_t* m)
{
  array_t * supportId = mdd_get_support(SlGetMddManager(), m);
  array_t * unprimedId = SlUnprimeVars (supportId);
  mdd_t * unprimedMdd = mdd_substitute(SlGetMddManager(), m, supportId, unprimedId);

  array_free(supportId);
  array_free(unprimedId);
  
  return unprimedMdd;
}

/**Function********************************************************************

  Synopsis    [Add await dependency info]

  Description []

  SideEffects []

******************************************************************************/
void
SlAddAwaitDependencyToFSM 
(
 Mdl_Module_t * module,
 Sl_GSTEntry_t * gstEntry
 )
{
  int id, aid;
  array_t * idArray, *pairArray;
  Var_Variable_t *var, *avar;
  Atm_Atom_t * atom;
  lsList varList;
  lsGen gen, gen1, gen2;
  st_table * idToPairTable; /* a temporary table */
  Pair_t * pair;

  gstEntry -> obj.fsm.dependency = array_alloc(Pair_t * , 0);
  pairArray = gstEntry -> obj.fsm.dependency;
  
  varList = Mdl_ModuleObtainVariableList(module);

  idToPairTable = st_init_table(st_numcmp, st_numhash);
  
  /* create a entry in the idtopairtable and the dependency array for each
     id*/

  lsForEachItem(varList, gen, var) {
    pair = SlPairAlloc();
    pair -> first = SlObtainUnprimedBddId(Var_VariableReadName(var));
    pair -> second = -1;
    pair -> info = (char *) array_alloc(Pair_t *, 0);
    array_insert_last(Pair_t *, pairArray, pair);
    st_insert(idToPairTable, (char *) (long) pair->first, (char *) pair);
  }

  /* then add the await info */
  lsForEachItem(varList, gen, var){
    int id;
    lsList awaitAtomList;

    id = SlObtainUnprimedBddId(Var_VariableReadName(var));
    if (!st_lookup(idToPairTable, (char *) (long) id, (char **) &pair)){
      printf("error in addawaitdependency!\n");
      exit (1);
    }
    
    /* obtain list of atoms awaiting this variable */
    awaitAtomList = Var_VariableReadAwaitList(var);
    lsForEachItem(awaitAtomList, gen1, atom) {
      int id2;
      Pair_t * pair2;
      lsList awaitVarList = Atm_AtomReadControlVarList(atom);
      
      lsForEachItem(awaitVarList, gen2, avar) {
        id2 = SlObtainUnprimedBddId(Var_VariableReadName(avar));
        st_lookup(idToPairTable, (char *) (long) id2, (char **) &pair2);
	array_insert_last(Pair_t * , (array_t *) pair -> info, pair2);
      }
    }
    
    array_sort((array_t *) pair -> info, comparePair);
    array_uniq((array_t *) pair -> info, comparePair, 0);

  }

  array_sort(pairArray, comparePair);

  st_free_table(idToPairTable);

}

/**Function********************************************************************

  Synopsis           [Merge two await dependency tables for two FSMs.]

  Description        [returns a new await dependency table. It is the
  users' responsibility to free it after use.]

  SideEffects        [required]

  SeeAlso            [optional]



******************************************************************************/

array_t *
SlDependencyMerge(
  array_t * dep1,
  array_t * dep2
  )
{
  int index1, index2;
  int len1, len2;
  int id1, id2;
  Pair_t *pair, *pair1, *pair2;
  array_t *newDep;
  boolean finish1, finish2;
  int i;

  /* temporary table */
  st_table * idToPairTable = st_init_table(st_numcmp, st_numhash);
  
  /* the following prepares the temporary table as well
     as putting id pairs into the newDep */
  
  newDep = array_alloc(Pair_t *, 0);
  index1 = index2 = 0;

  len1 = array_n(dep1);
  len2 = array_n(dep2);
  
  finish1 = (index1 >= len1);
  finish2 = (index2 >= len2);

  while (!finish1 || !finish2){
    pair = SlPairAlloc();

    pair -> second = -1;
    pair -> info = (char *) array_alloc(Pair_t *, 0);
    
    pair1 = pair2 = NIL(Pair_t);

    if (!finish1)
      pair1 = array_fetch (Pair_t *, dep1, index1);
    
    if (!finish2)
      pair2 = array_fetch (Pair_t *, dep2, index2);

    if (finish2 || (!finish1 && pair1 -> first < pair2 -> first)) {
      pair -> first = pair1 -> first;
      index1 ++;
    } else if (finish1 || (!finish2 && pair1 -> first > pair2 -> first)) {
      pair -> first = pair2 -> first;
      index2 ++;
    } else { /* pair1 -> first == pair2 -> first */
      pair -> first = pair1 -> first;
      index1 ++;
      index2 ++;
    }

    st_insert(idToPairTable, (char *) (long) pair -> first, (char *) pair);
    array_insert_last(Pair_t *, newDep, pair);
    
    finish1 = (index1 >= len1);
    finish2 = (index2 >= len2);
  }

  assert(finish1);
  assert(finish2);

  /* then start from the begining and start merging */
  index1 = 0;
  index2 = 0;
  
  arrayForEachItem(Pair_t *, newDep, i, pair) {
    int id = pair -> first;
    array_t *list1, *list2;

    list1 = list2 = NIL(array_t);
    finish1 = (index1 >= len1);
    finish2 = (index2 >= len2);
    
    if (!finish1){
      pair1 = array_fetch(Pair_t *, dep1, index1);
      if (pair1 -> first == id){
        list1 = (array_t *) pair1 -> info;
        index1 ++;
      }
    }
    
    if (!finish2) {
      pair2 = array_fetch(Pair_t *, dep2, index2);
      if (pair2 -> first == id){
        list2 = (array_t *) pair2 -> info;
        index2 ++;
      }
    }
    
    mergeAwaitList(idToPairTable, list1, list2, pair);
  }

  assert (finish1 || finish2);
  
  st_free_table(idToPairTable);
  
  return newDep;
}
			  
/**Function********************************************************************

  Synopsis           [returns a sorted array of (unprimed) bdd ids from fsm]

  Description        [returns a null array if there is cycle in the
  await dependency.]

  SideEffects        [user should free the array after use.]

  SeeAlso            [optional]

******************************************************************************/

array_t *
SlReturnSortedIds(
 Sl_FSM_t fsm) 
{
  array_t * dep = fsm.dependency;
  array_t * sortedIds;
  int len, i;
  boolean ok;
  Pair_t * pair, *headptr, *idptr = NIL(Pair_t);
  
  /* ---qui--- */ 
  
  /* this one checks for loops */
  /*
  ok = traverse(dep);
  if (!ok) 
    return (NIL(array_t));
  */

  /* fill the array with external ids first */
  len = array_n(dep);
  
  sortedIds = array_alloc(int, 0);

  headptr = ObtainNonAwaitingElementsList(dep);
  

  while (headptr != NIL(Pair_t)){
    Pair_t *successorList;
    Pair_t ** preptr = &headptr;  

    while(*preptr!=NIL(Pair_t)){
      
      if (IdIsExternal(fsm, (*preptr) -> first) || (*preptr == idptr)) {
        Pair_t * lastpair, *tmpptr;
        
        array_insert_last(int, sortedIds, (*preptr) -> first);
        successorList =
            ObtainListOfSuccessors(dep, (*preptr) -> first, &lastpair);
        tmpptr = *preptr;
        if (successorList == NIL(Pair_t)) {
          *preptr = (Pair_t *) (*preptr) -> info;
        } else {
          lastpair -> info = (*preptr) -> info;
          *preptr = successorList;
        }
        FREE(tmpptr);
      } else {
        preptr = (Pair_t **) &((*preptr) -> info);
      }
    }
    
    /* no more external ids */
    /* has to determine which controlled id to remove */
    idptr = DetermineControlledIdToRemove(headptr);
  }

  return sortedIds;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlFreeBddCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_free_bdd <bdd>";
  char *inObjName, *outObjName; 
  Sl_GSTEntry_t *inEntry, *outEntry;
  mdd_t * mdd;
  int i;
  
  util_getopt_reset();
  util_optind++;
  if (argc != 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  inObjName = argv[util_optind];
  if (!st_lookup(Sl_GST, inObjName, (char **) &inEntry)) {
    Main_MochaErrorPrint("Error: Bdd \"%s\" does not exist.\n", inObjName);
    return TCL_OK;
  }
  
  if (inEntry -> kind != Sl_BDD_c) {
    Main_MochaErrorPrint("Error: Bdd \"%s\" does not exist.\n", inObjName);
    return TCL_OK;
  }

  st_delete(Sl_GST, &inObjName, (char **) &inEntry);
  
  FREE(inEntry -> name);
  arrayForEachItem(mdd_t *, inEntry -> obj.bddArray, i, mdd) {
    mdd_free(mdd);
  }

  array_free(inEntry -> obj.bddArray);
  FREE(inEntry);

  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlFreeObjCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_freeObj <obj>";
  char *inObjName, *outObjName, *s; 
  Sl_GSTEntry_t *inEntry, *outEntry;
  mdd_t * mdd;
  int i;
  
  util_getopt_reset();
  util_optind++;
  if (argc != 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_ERROR;
  }

  inObjName = argv[util_optind];
  if (!st_lookup(Sl_GST, inObjName, (char **) &inEntry)) {
    Main_MochaErrorPrint("Warning: Obj \"%s\" does not exist. Ignored.\n", inObjName);
    return TCL_ERROR;
  }
  
  switch (inEntry -> kind) {
      case Sl_BDD_c:
        arrayForEachItem(mdd_t *, inEntry -> obj.bddArray, i, mdd) {
          mdd_free(mdd);
        }
        
        array_free(inEntry -> obj.bddArray);
        break;

      case Sl_FSM_c:
        Main_MochaPrint(
          "Obj \"%s\" is a FSM. Not implemented yet. Ignored.\n", inObjName);
        return TCL_OK;
        
      case Sl_IdArray_c:
        array_free(inEntry -> obj.idArray);
        break;

      case Sl_IntArray_c:
        array_free(inEntry -> obj.intArray);
        break;

      case Sl_StringArray_c:
        arrayForEachItem(char *, inEntry -> obj.stringArray, i, s) {
          FREE(s);
        }
        array_free(inEntry -> obj.stringArray);
        break;

      default:
        Main_MochaErrorPrint(
          "Error: unknown type encountered in SlFreeObjCmd. \n");
        exit(1);
        break;
        
  }
  

  st_delete(Sl_GST, &inObjName, (char **) &inEntry);
  FREE(inEntry -> name);
  FREE(inEntry);
  


  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPrimeCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_prime <outBdd> <inBdd>";
  char *bdd1Name, *outBddName;
  Sl_GSTEntry_t *entry1, *outEntry;
  mdd_t *mdd;
  int i;
  mdd_manager * mddManager;
    
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
  outEntry -> obj.bddArray = array_alloc(mdd_t *, 0);

  /* get the unprimed ids in the support of the BDD */
  /* then get their primed ids */
  /* then do substitution */

  mddManager = SlGetMddManager();

  arrayForEachItem(mdd_t *, entry1 -> obj.bddArray, i, mdd) {
    array_t * unPrimedIdArray = array_alloc(int, 0);
    array_t * primedIdArray = array_alloc(int, 0);
    array_t * tempIdArray;
    int id, primedId;
    mdd_t * primedMdd;
    int j;

    tempIdArray = mdd_get_support(mddManager, mdd);
    arrayForEachItem(int, tempIdArray, j, id) {
      primedId = SlReturnPrimedId(id);
      if (id != primedId) {
	array_insert_last(int, unPrimedIdArray, id);
	array_insert_last(int, primedIdArray, primedId);
      }
    }
    array_free(tempIdArray);
    primedMdd = mdd_substitute(mddManager, mdd, unPrimedIdArray, primedIdArray);
    array_insert_last(mdd_t *, outEntry -> obj.bddArray, primedMdd);
    array_free(unPrimedIdArray);
    array_free(primedIdArray);
  }

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetCtrlVarIdsCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getCtrlVarIds <outIdArray> <inFsm>";
  char *fsmName, *outIdArrayName;
  Sl_GSTEntry_t *entry1, *outEntry;
  mdd_t *mdd;
  int i;
  mdd_manager * mddManager;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outIdArrayName = argv[util_optind];
  if (st_is_member(Sl_GST, outIdArrayName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outIdArrayName);
    return TCL_OK;
  }

  util_optind++;

  fsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, fsmName, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  if (!GSTEntryIsFsm(entry1)) {
    Main_MochaErrorPrint("Error: Object %s is not a FSM.\n", fsmName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outIdArrayName);
  outEntry -> kind = Sl_IdArray_c;   
  outEntry -> obj.idArray = array_dup(entry1->obj.fsm.contrvarL);

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlEQuantifyIdsCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_eQuantifyIds <outBdd> <inBdd> <inIdArray>";
  char *outBddName, *inBddName, *inIdArrayName;
  Sl_GSTEntry_t *entry1, *entry2, *outEntry;
  mdd_manager * mddManager;
  array_t * emptyArray;
    
  util_getopt_reset();
  if (argc != 4) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  util_optind++;
  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }

  util_optind++;
  inBddName = argv[util_optind];
  if (!st_lookup(Sl_GST, inBddName, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inBddName);
    return TCL_OK;
  }

  if (!GSTEntryIsBdd(entry1)) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", inBddName);
    return TCL_OK;
  }

  util_optind++;
  inIdArrayName = argv[util_optind];
  if (!st_lookup(Sl_GST, inIdArrayName, (char **) &entry2)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inIdArrayName);
    return TCL_OK;
  }

  if (!GSTEntryIsIdArray(entry2)) {
    Main_MochaErrorPrint("Error: Object %s is not a Id Array.\n", inIdArrayName);
    return TCL_OK;
  }


  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;   
  outEntry -> obj.bddArray = array_alloc(mdd_t *, 0);

  emptyArray = array_alloc(int, 0);
  array_insert_last(mdd_t *, outEntry -> obj.bddArray,
		    Img_MultiwayLinearAndSmooth(SlGetMddManager(),
						entry1 -> obj.bddArray, 
						entry2 -> obj.idArray,
						emptyArray));
  array_free(emptyArray);
  
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPrintObjCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_printObj <inSlObj>";
  char *inObjName;
  Sl_GSTEntry_t *entry1;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  inObjName = argv[util_optind];
  if (!st_lookup(Sl_GST, inObjName, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inObjName);
    return TCL_OK;
  }

  Main_ClearResult();
  
  if (GSTEntryIsBdd(entry1)) {
    Main_MochaPrint("<bdd>: unprintable.\n");
  } else if (GSTEntryIsFsm(entry1)) {
    Main_MochaPrint("<fsm>: unprintable.\n");
  } else if (GSTEntryIsIdArray(entry1)) {
    int id, i;
    arrayForEachItem(int, entry1 -> obj.idArray, i, id) {
      Main_AppendElement("%d", id);
    }
  } else if (GSTEntryIsIntArray(entry1)) {
    int v, i;
    arrayForEachItem(int, entry1 -> obj.intArray, i, v) {
      Main_AppendElement("%d", v);
    }
  } else  if (GSTEntryIsStringArray(entry1)) {
    char * s;
    int i;
    arrayForEachItem(char * , entry1 -> obj.stringArray, i, s) {
      Main_AppendElement("%s", s);
    }
  } else {
    Main_MochaErrorPrint("unknown obj.\n");
  }

  
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
mdd_t *   
SlRandomSliceBdd (
  int id
  ) 
{
  mdd_t * mdd;
  Var_Variable_t * var = SlReturnVariableFromId(id);
  int randomValue;
  Atm_Expr_t * leftExpr;
  Atm_Expr_t * rightExpr;
  Atm_Expr_t * atmExpr;
  Var_Type_t * type;
  Var_EnumElement_t * enumConst;
  
  /* want to create a mdd representing a random assignment of the
     variable to the value.
     what I am to do is to build an atmExpr and pass it to Sym_ExprBuildMdd
     */

  switch (Var_VariableReadDataType(var)) {
      case Var_Boolean_c:
        randomValue = SlReturnRandomValue(2);
        rightExpr = Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) (long) randomValue, 
                                  NIL(Atm_Expr_t));
        break;
        
      case Var_Enumerated_c:
        type = Var_VariableReadType(var);
        randomValue = SlReturnRandomValue(Var_VarTypeReadDomainSize(type));
        enumConst = Var_VarTypeReadSymbolicValueFromIndex(type, randomValue);
        rightExpr =  Atm_ExprAlloc(Atm_EnumConst_c, (Atm_Expr_t*) enumConst,
                                   NIL(Atm_Expr_t));
        break;
        
      case Var_Range_c:
        randomValue = SlReturnRandomValue(Var_VarTypeReadDomainSize(Var_VariableReadType(var)));
        rightExpr =  Atm_ExprAlloc(Atm_NumConst_c,
                                   (Atm_Expr_t *) (long) randomValue,
                                   NIL(Atm_Expr_t));
        break;
        
      default:
        /* all other cases, i.e. Var_Event_c, Var_Array_c,  Var_Bitvector_c, Var_Integer_c,
           Var_Natural_c, Var_NumConst_c, Var_Timer_c,  Var_TimeIncrement_c */
        /* Main_MochaPrint("Warning: sliced var \"%s\" has incompatible type. Ignored.\n",
	   Var_VariableReadName(var)); */ 
        return mdd_one(SlGetMddManager());
        break;
  }

  if (id == SlReturnPrimedId(id)) {
    leftExpr = Atm_ExprAlloc(Atm_PrimedVar_c, (Atm_Expr_t*) var,
                             NIL(Atm_Expr_t));
  } else {
    leftExpr = Atm_ExprAlloc(Atm_UnPrimedVar_c, (Atm_Expr_t*) var,
                             NIL(Atm_Expr_t));
  }
  
  /* now, build the atmExpr and mdd  */
  atmExpr = Atm_ExprAlloc(Atm_Equal_c,  leftExpr, rightExpr);

  /* type check, as well as adding type information */
  Atm_TypeCheckNew(BoolType, &atmExpr);

  mdd = Sym_ExprBuildMdd(
    Sym_RegionManagerReadSymInfo(SlGetRegionManager(),
                                 (Mdl_Module_t *) Var_VariableReadModule(var)),
    atmExpr);

  Atm_ExprFreeExpression(atmExpr);
  return mdd;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
array_t *
SlGetArrayOfCubes (
  int id
  ) 
{
  mdd_t * mdd;
  Var_Variable_t * var = SlReturnVariableFromId(id);
  int randomValue;
  Atm_Expr_t * leftExpr;
  Atm_Expr_t * rightExpr;
  Atm_Expr_t * atmExpr;
  Atm_Expr_t * typedAtmExpr;
  Var_Type_t * type;
  Var_EnumElement_t * enumConst;
  int i;
  array_t * returnArray = array_alloc(mdd_t *, 0);
  
  if (id == SlReturnPrimedId(id)) {
    leftExpr = Atm_ExprAlloc(Atm_PrimedVar_c, (Atm_Expr_t*) var,
                             NIL(Atm_Expr_t));
  } else {
    leftExpr = Atm_ExprAlloc(Atm_UnPrimedVar_c, (Atm_Expr_t*) var,
                             NIL(Atm_Expr_t));
  }
  
  type = Var_VariableReadType(var);
  switch (Var_VariableReadDataType(var)) {
      case Var_Boolean_c:
        for (i=0; i < 2; i++) {
          rightExpr = Atm_ExprAlloc(Atm_BoolConst_c, (Atm_Expr_t *) (long) i,
                                    NIL(Atm_Expr_t));
          mdd = getMddFromLeftAndRightAtmExpr(var, leftExpr, rightExpr);
          array_insert_last(mdd_t *, returnArray, mdd);
        }
        break;
        
      case Var_Enumerated_c:
        for (i=0; i < Var_VarTypeReadDomainSize(type); i++) {
          enumConst = Var_VarTypeReadSymbolicValueFromIndex(type, i);
          rightExpr =  Atm_ExprAlloc(Atm_EnumConst_c, (Atm_Expr_t *) enumConst,
                                     NIL(Atm_Expr_t));
          mdd = getMddFromLeftAndRightAtmExpr(var, leftExpr, rightExpr);
          array_insert_last(mdd_t *, returnArray, mdd);
        }
        break;
        
      case Var_Range_c:
        for (i=0; i < Var_VarTypeReadDomainSize(type); i++) {
          rightExpr =  Atm_ExprAlloc(Atm_NumConst_c, (Atm_Expr_t *) (long) i, 
                                     NIL(Atm_Expr_t));
          mdd = getMddFromLeftAndRightAtmExpr(var, leftExpr, rightExpr);
          array_insert_last(mdd_t *, returnArray, mdd);
        }
        break;
        
      default:
        /* all other cases, i.e. Var_Event_c, Var_Array_c,  Var_Bitvector_c, Var_Integer_c,
           Var_Natural_c, Var_NumConst_c, Var_Timer_c,  Var_TimeIncrement_c */
        /* Main_MochaPrint("Warning: sliced var \"%s\" has incompatible type. Ignored.\n",
	   Var_VariableReadName(var)); */ 
        Atm_ExprFreeExpression(leftExpr);
        array_insert_last(mdd_t *, returnArray, mdd_one(SlGetMddManager()));
        return returnArray;
        break;
  }
  
  return returnArray;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetMddSupportCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getMddSupport <outIdArray> <inBDD>";
  char *inBDDName, *outIdArrayName;
  Sl_GSTEntry_t *entry1, *outEntry;
  mdd_t *mdd;
  int i;
  mdd_manager * mddManager;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  outIdArrayName = argv[util_optind];
  if (st_is_member(Sl_GST, outIdArrayName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outIdArrayName);
    return TCL_OK;
  }

  util_optind++;

  inBDDName = argv[util_optind];
  if (!st_lookup(Sl_GST, inBDDName, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inBDDName);
    return TCL_OK;
  }
  if (!GSTEntryIsBdd(entry1)) {
    Main_MochaErrorPrint("Error: Object %s is not a BDD.\n", inBDDName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outIdArrayName);
  outEntry -> kind = Sl_IdArray_c;   
  outEntry -> obj.idArray = array_alloc(int, 0);
  
  arrayForEachItem(mdd_t *, entry1 -> obj.bddArray, i, mdd) {
    array_t * tmpArray =  mdd_get_support(SlGetMddManager(), mdd);
    int j, x;
    arrayForEachItem(int, tmpArray, j, x) {
      array_insert_last(int, outEntry -> obj.idArray, x);
    }
    array_free(tmpArray);
    
  }

  array_sort (outEntry -> obj.idArray, SlCompareInt);
  array_uniq (outEntry -> obj.idArray, SlCompareInt, 0);
  
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlCreateIntArrayCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_createIntArray <outIntArray>";
  char *outIntArrayName;
  Sl_GSTEntry_t  *outEntry;
    
  util_getopt_reset();
  if (argc != 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  util_optind++;
  outIntArrayName = argv[util_optind];
  if (st_is_member(Sl_GST, outIntArrayName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outIntArrayName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outIntArrayName);
  outEntry -> kind = Sl_IntArray_c;   
  outEntry -> obj.intArray = array_alloc(int, 0);

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlMakeStringArrayCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] =
      "Usage: sl_makeStringArray <outStringArray> <string1> <string2> ...";
  char *outStringArrayName;
  Sl_GSTEntry_t  *outEntry;
    
  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  util_optind++;
  outStringArrayName = argv[util_optind];
  if (st_is_member(Sl_GST, outStringArrayName)) {
    Main_MochaErrorPrint("Error: Object \"%s\" exists. Ignored.\n", outStringArrayName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outStringArrayName);
  outEntry -> kind = Sl_StringArray_c;   
  outEntry -> obj.stringArray = array_alloc(char *, 0);

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  util_optind++;
  while(util_optind < argc) {
    array_insert_last(char *, outEntry -> obj.stringArray,
                      util_strsav(argv[util_optind]));
    util_optind ++;
  }
  
  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlMakeIntArrayCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] =
      "Usage: sl_makeIntArray <outIntArray> <int1> <int2> ...";
  char *outIntArrayName;
  Sl_GSTEntry_t  *outEntry;
    
  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  util_optind++;
  outIntArrayName = argv[util_optind];
  if (st_is_member(Sl_GST, outIntArrayName)) {
    Main_MochaErrorPrint("Error: Object \"%s\" exists. Ignored.\n", outIntArrayName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outIntArrayName);
  outEntry -> kind = Sl_IntArray_c;   
  outEntry -> obj.intArray = array_alloc(int, 0);

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  util_optind++;
  while(util_optind < argc) {
    array_insert_last(int, outEntry -> obj.intArray,
                      atoi(argv[util_optind]));
    util_optind ++;
  }
  
  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlMakeIdArrayCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] =
      "Usage: sl_makeIdArray <outIdArray> <id1> <id2> ...";
  char *outIdArrayName;
  Sl_GSTEntry_t  *outEntry;
    
  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  util_optind++;
  outIdArrayName = argv[util_optind];
  if (st_is_member(Sl_GST, outIdArrayName)) {
    Main_MochaErrorPrint("Error: Object \"%s\" exists. Ignored.\n", outIdArrayName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outIdArrayName);
  outEntry -> kind = Sl_IdArray_c;   
  outEntry -> obj.idArray = array_alloc(int, 0);

  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  util_optind++;
  while(util_optind < argc) {
    array_insert_last(int, outEntry -> obj.idArray,
                      atoi(argv[util_optind]));
    util_optind ++;
  }
  
  return TCL_OK;

}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlAddToArrayCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_addToArray <inOuttIdArray> <integer>";
  char *inOutIntArrayName;
  Sl_GSTEntry_t *entry1, *outEntry;
  int integer;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  inOutIntArrayName = argv[util_optind];
  if (!st_lookup(Sl_GST, inOutIntArrayName, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inOutIntArrayName);
    return TCL_OK;
  }
  if (!GSTEntryIsIntArray(entry1)) {
    Main_MochaErrorPrint("Error: Object %s is not an integer array.\n", inOutIntArrayName);
    return TCL_OK;
  }

  util_optind++;
  integer = atoi(argv[util_optind]);
  
  array_insert_last(int, entry1 -> obj.intArray, integer);

  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetVarNamesFromIdsCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getVarNamesFromIdsCmd <outVarArray> <inIdArray>";
  char *outVarArrayName, *inIdArrayName;
  Sl_GSTEntry_t *entry1, *outEntry;
  int i, id;
    
  util_getopt_reset();
  util_optind++;
  if (argc != 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }
  
  outVarArrayName = argv[util_optind];
  if (st_is_member(Sl_GST, outVarArrayName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outVarArrayName);
    return TCL_OK;
  }

  util_optind++;

  inIdArrayName = argv[util_optind];
  if (!st_lookup(Sl_GST, inIdArrayName, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inIdArrayName);
    return TCL_OK;
  }
  if (!GSTEntryIsIdArray(entry1)) {
    Main_MochaErrorPrint("Error: Object %s is not an Id array.\n", inIdArrayName);
    return TCL_OK;
  }

  /* Allocates the symbol table entry for the output. */ 
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outVarArrayName);
  outEntry -> kind = Sl_StringArray_c;   
  outEntry -> obj.stringArray = array_alloc(char *, 0);
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);

  arrayForEachItem(int, entry1 -> obj.idArray, i, id) {
    char *tmpName;
    int isPrimed = (SlReturnPrimedId(id) == id);
    char * name = Var_VariableReadName(SlReturnVariableFromId(id));


    if (isPrimed) {
      tmpName = Main_strConcat(2, name, "'");
    } else {
      tmpName = util_strsav(name);
    }

    array_insert_last(char *, outEntry -> obj.stringArray, tmpName);
    
  }
  

  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetVarNameFromIdCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getVarNameFromIdCmd <inId>";
  char *name;
  int isPrimed, id;
  Var_Variable_t* var;
    
  if (argc != 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }
  
  id = atoi(argv[1]);
  if (id < 0) {
    Main_MochaErrorPrint("Error: \"%d\" should be non-negative.\n", id);
    return TCL_OK;
  }
  
  var = SlReturnVariableFromId(id);
  if (!var) {
    Main_MochaErrorPrint("Error: no variable has id \"%d\"\n", id);
    return TCL_OK;
  }
  
  name = Var_VariableReadName(var);
  isPrimed = (SlReturnPrimedId(id) == id);

  Main_ClearResult();
  if (isPrimed) {
    Main_AppendResult("%s'", name);
  } else {
    Main_AppendResult("%s", name);
  }

  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [Function implementing command sl_gettr. ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlGetPartitionedTRCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_getPartitionedTr <outBdd> <inFsm>";
  char *fsmName, *outName;
  Sl_GSTEntry_t *fsmEntry, *outEntry;
  int i;
  mdd_t * mdd;
    
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

  outEntry -> obj.bddArray = array_alloc(mdd_t *, 0);
  arrayForEachItem(mdd_t *, fsmEntry -> obj.fsm.updateBdd, i, mdd) {
    array_insert_last(mdd_t *, outEntry -> obj.bddArray, mdd_dup(mdd));
  }
 
  
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
    
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPreNCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_pre_n <outBdd> <inFSM> <inBdd> <numSteps>";
  char *outBddName, *inFsmName, *inBddName;
  Sl_GSTEntry_t *entry1, *entry2, *outEntry;
  mdd_manager * mddManager;
  array_t * emptyArray;
  mdd_t * mdd;
  int numSteps, i, id;
  array_t * targetMddArray, * primedIdArray;
  
    
  util_getopt_reset();
  if (argc != 5) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  util_optind++;
  outBddName = argv[util_optind];
  if (st_is_member(Sl_GST, outBddName)) {
    Main_MochaErrorPrint("Error: Object %s exists.\n", outBddName);
    return TCL_OK;
  }

  util_optind++;
  inFsmName = argv[util_optind];
  if (!st_lookup(Sl_GST, inFsmName, (char **) &entry1)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inFsmName);
    return TCL_OK;
  }
  
  if (!GSTEntryIsFsm(entry1)) {
    Main_MochaErrorPrint("Error: Object %s is not a Fsm.\n", inFsmName);
    return TCL_OK;
  }

  util_optind++;
  inBddName = argv[util_optind];
  if (!st_lookup(Sl_GST, inBddName, (char **) &entry2)) {
    Main_MochaErrorPrint("Error: Object %s does not exist.\n", inBddName);
    return TCL_OK;
  }

  if (!GSTEntryIsBdd(entry2)) {
    Main_MochaErrorPrint("Error: Object %s is not a Bdd.\n", inBddName);
    return TCL_OK;
  }

  util_optind ++;
  numSteps = atoi(argv[util_optind]);
  if (numSteps < 0) {
    Main_MochaErrorPrint("Error: <numSteps> is not non-negative..\n");
    return TCL_OK;
  }
  
  
  /* Allocates the symbol table entry for the output. */
  outEntry = ALLOC(Sl_GSTEntry_t, 1);
  outEntry -> name = util_strsav(outBddName);
  outEntry -> kind = Sl_BDD_c;   
  outEntry -> obj.bddArray = array_alloc(mdd_t *, 0);
  st_insert(Sl_GST, outEntry -> name, (char *) outEntry);
  
  if (numSteps == 0) {
    arrayForEachItem(mdd_t *, entry2 -> obj.bddArray, i, mdd) {
      array_insert_last(mdd_t *, outEntry -> obj.bddArray, mdd_dup(mdd));
    }
    return TCL_OK;
  }
  
  /* numSteps > 0 */

  emptyArray = array_alloc(int, 0);

  targetMddArray = array_alloc(mdd_t *, 0);
  primedIdArray = array_alloc(int, 0);
  
  arrayForEachItem(mdd_t *, entry2 -> obj.bddArray, i, mdd) {
    mdd_t * primedMdd = SlPrimeMdd(mdd);
    array_t * tmpIdArray;
    
    array_insert_last(mdd_t *, targetMddArray, primedMdd);
    tmpIdArray = mdd_get_support(SlGetMddManager(), primedMdd);
    array_append(primedIdArray, tmpIdArray);
    array_free(tmpIdArray);
  }

  arrayForEachItem(int, entry1->obj.fsm.contrvarL, i, id) {
    array_insert_last(int, primedIdArray, SlReturnPrimedId(id));
  }
  
  arrayForEachItem(int, entry1 -> obj.fsm.extvarL, i, id) {
    array_insert_last(int, primedIdArray, SlReturnPrimedId(id));
  }
  
  array_sort(primedIdArray, compareInt);
  array_uniq(primedIdArray, compareInt, 0);

  for (i=0; i<numSteps; i++) {
    array_t * mddArray;

    Main_MochaPrint("sl_pre_n step = %d\n", i+1);
    
    if (i != 0) {
      mdd_t * primedMdd = SlPrimeMdd(mdd);
      
      targetMddArray = array_alloc(mdd_t *, 0);
      array_insert_last(mdd_t *, targetMddArray, primedMdd);
      
      mdd_free(mdd);
    }
    
    mddArray = array_join(entry1 -> obj.fsm.updateBdd,
                          targetMddArray);

    mdd = Img_MultiwayLinearAndSmooth(SlGetMddManager(),
                                      mddArray,
                                      primedIdArray,
                                      emptyArray);
    array_free(mddArray);
    SlFreeArrayOfBdd(targetMddArray);
  }

  array_insert_last(mdd_t *, outEntry -> obj.bddArray, mdd);
  
  array_free(primedIdArray);
  array_free(emptyArray);

  return TCL_OK;

}




/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis    [Group all bdd vars corresponding to mdd vars initMddId to
  initMddId + (blockSize-1) in a block which will not be split in reordering.]

  Description [Group all bdd vars corresponding to mdd vars initMddId to
  initMddId + (blockSize-1) in a block which will not be reordered internally.
  Ths bdd's corresponding to these mdd's should be contiguous; if not the
  function will fail.]

  SideEffects []

******************************************************************************/
static void
MddGroupVariables(
  mdd_manager *mddMgr,
  int initMddId,
  int blockSize )
{
  int i, j;
  int length;
  int aIndex;
  int startingBddIndex;
  int sanityCheck;
  mvar_type initMv, anMv;
  bvar_type initBvar, aBvar;
  array_t *mvar_list, *bvar_list;
  bdd_t *bdd;
  
  mvar_list = mdd_ret_mvar_list(mddMgr);
  bvar_list = mdd_ret_bvar_list(mddMgr);

  /*
   * mvar for initMddId 
   */
  initMv = array_fetch(mvar_type, mvar_list, initMddId);

  /*
   * bvar for first bdd for initMddId 
   */
  initBvar = mdd_ret_bvar(&initMv, 0, bvar_list);

  /*
   * number of bdd variables to group 
   */
  length = 0;

  /*
   * startingBddIndex is the level of the first bdd variable 
   */
  startingBddIndex = bdd_top_var_level( mddMgr, initBvar.node );
  sanityCheck = startingBddIndex;

  /*
   * in this loop we are simply ensuring that the bdd variables
   * corresponding to the mdd vars are infact contiguous. If this
   * is not the case we fail. length is the total number of bdd variables
   * to be grouped.
   */
  for( i = 0; i < blockSize; i++) {
    anMv = array_fetch(mvar_type, mvar_list, ( initMddId + i ) );
    for ( j = 0 ; j < anMv.encode_length; j++ ) {

      aBvar = mdd_ret_bvar( & anMv, j, bvar_list );
      aIndex = bdd_top_var_level( mddMgr,  aBvar.node );

      if ( sanityCheck != aIndex ) {
        /* bdd vars are not contiguous!! */
        fprintf(stderr, "Badly formed block - bdd vars for %s (mvar_id = %d) are not contiguous!!\n",
                             anMv.name, anMv.mvar_id );
        fail("Wont go on\n");
      }
      else {
        /* number of variables to group increased by one */
        sanityCheck++;
        length++;
      }
    }
  }
  
  bdd = bdd_var_with_index(mddMgr, startingBddIndex);
  (void) bdd_new_var_block(bdd, length);
  bdd_free(bdd);
}


/**Function********************************************************************
   
  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
PrintsBddIds(
  array_t * moduleArray)
{
  Mdl_Module_t * module;
  int i;
  Sym_Info_t * symInfo;
  st_table * unprimedTable, *primedTable;
  Var_Variable_t * var;
  long id, pid;

  arrayForEachItem(Mdl_Module_t *, moduleArray, i, module) {
    lsList varL = Mdl_ModuleObtainVariableList(module);
    lsGen lsgen;
        
    symInfo = Sym_RegionManagerReadSymInfo(SlGetRegionManager(), module);

    unprimedTable = Sym_SymInfoReadVarToUnprimedId(symInfo);
    primedTable = Sym_SymInfoReadVarToPrimedId(symInfo);

    lsForEachItem(varL, lsgen, var){
      int flag=0;
      
      flag = st_lookup(unprimedTable, (char*) var, (char **) & id);      
      flag = st_lookup(primedTable, (char*) var, (char **) & pid);
      
      Main_MochaPrint("name = %s, id = %d, id'= %d\n",
                      Var_VariableReadName(var), id, pid);
    }

  }
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [optional] 	   

  CommandSynopsis    [optional]  

  CommandArguments   [optional]  

  CommandDescription [optional]  

******************************************************************************/
static int
getNVals(
  int id)
{
  mvar_type x = array_fetch(mvar_type,
                            mdd_ret_mvar_list(SlGetMddManager()), id);
  return x.values;

}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [optional] 	   

  CommandSynopsis    [optional]  

  CommandArguments   [optional]  

  CommandDescription [optional]  

******************************************************************************/
mdd_t *
SlGetLiteral(
  int id,
  int pos)
{
  array_t * values = array_alloc(int ,0);
  mdd_t * literal;
  
  array_insert(int, values, 0, pos);
  literal = mdd_literal(SlGetMddManager(), id, values);
  array_free(values);
  assert(literal);

  return literal;
}


/**Function********************************************************************

  Synopsis           [Input: an array of MDDs. Output: an MDD obtained by
                      conjoining them. ]

  Description        [optional]

  SideEffects        [none.]

  SeeAlso            [optional]

******************************************************************************/

static mdd_t * 
conjoin (array_t *bddArray) 
{
  mdd_t        *out, *newOut, *conj; 
  int          i;
  mdd_manager *mgr;

  mgr = SlGetMddManager ();

  out = mdd_one (mgr); 
  arrayForEachItem(mdd_t*, bddArray, i, conj) {
    newOut = mdd_and (out, conj, 1, 1);
    mdd_free (out); 
    out = newOut; 
  }

  return (out); 
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [optional] 	   

  CommandSynopsis    [optional]  

  CommandArguments   [optional]  

  CommandDescription [optional]  

******************************************************************************/
int SlGetTopMddId(mdd_t * mdd) 
{
  array_t * varidArray = mdd_get_support(SlGetMddManager(), mdd);
  int id;
  
  if (array_n(varidArray) == 0){
    Main_MochaErrorPrint("Error: mdd has empty support\n");
    exit(1);
  }

  id = (int)array_fetch( int, varidArray, 0);
  array_free(varidArray);
  
  return id;
}


/**Function********************************************************************

  Synopsis           [Comparing two integers! (to sort an array)]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/

static int
compareInt (char *obj1, char *obj2)
{
  int n1, n2;

  n1 = *((int *) obj1);
  n2 = *((int *) obj2);
  
  return n1 - n2;
}


/**Function********************************************************************

  Synopsis           [Comparing two integers! (to sort an array)]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
comparePair (char **obj1, char **obj2)
{
  int n1, n2;

  n1 = ((Pair_t *) *obj1) -> first;
  n2 = ((Pair_t *) *obj2) -> first;

  return (n1 - n2);
}



/**Function********************************************************************

  Synopsis           [Merge two await dependency lists, for the same
                      variable id, in two different FSMs.
		      I guess it simply computes the union.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
/* must check for cyclic dependencies? */ 

static void mergeAwaitList(
  st_table * idToPairTable,
  array_t * list1,
  array_t * list2,
  Pair_t * pair
  )
{

  int index1, index2;
  int len1, len2;
  boolean finish1, finish2;
  int i;
  
  index1 = 0;
  index2 = 0;
  len1 = list1? array_n(list1): 0;
  len2 = list2? array_n(list2): 0;

  finish1 = (index1 >= len1);
  finish2 = (index2 >= len2);
  
  while (!finish1 || !finish2) {
    Pair_t * pair1, *pair2, *tmpPair;
    
    if (!finish1)
      pair1 = array_fetch(Pair_t *, list1, index1);
    
    if (!finish2)
      pair2 = array_fetch(Pair_t *, list2, index2);
    
    if (finish2 || (!finish1 && pair1 -> first < pair2 -> first)) { 
      st_lookup(idToPairTable, (char *) (long) pair1 -> first,
                (char **) &tmpPair);
      array_insert_last(Pair_t *, (array_t *) pair -> info,
                        tmpPair);
      index1 ++;
    } else if (finish1 || (!finish2 && pair1 -> first > pair2 -> first)) {
      st_lookup(idToPairTable, (char *) (long) pair2 -> first,
                (char **) &tmpPair);
      array_insert_last(Pair_t * , (array_t *) pair -> info,
                        tmpPair);
      index2 ++;
    } else { /* pair1 -> first > pair2 -> first */
      st_lookup(idToPairTable, (char *) (long) pair1 -> first,
                (char **) &tmpPair);
      array_insert_last(Pair_t *, (array_t *) pair -> info,
                        tmpPair);
      index1 ++;
      index2 ++;
    }

    finish1 = (index1 >= len1);
    finish2 = (index2 >= len2);
  }

  assert(finish1);
  assert(finish2);
  
}

/**Function********************************************************************

  Synopsis           []

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
traverse(
 array_t * dep)
{
  /* do depth first search, first check if not traversed before */
  Pair_t * pair;
  int i, count;
  boolean ok;

  count = 0;
  arrayForEachItem(Pair_t *, dep, i, pair) {
    ok = traverse1(dep, pair, &count);
    if (!ok) break;
  }

  return ok;
  
}

/**Function********************************************************************

  Synopsis           []

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
traverse1(
  array_t * dep,
  Pair_t * pair,
  int * count) 
{
  boolean ok;
  int i, id;

  if (pair -> second >= 0 ) {
    return TRUE;
  }

  if (pair -> second == -2) {
    Var_Variable_t * var = SlReturnVariableFromId(pair -> first);
    Main_MochaErrorPrint("loop: %s\n", 
			 Var_VariableReadName(var));
    return FALSE;
  }

  if (pair -> second == -1) {
    pair -> second = -2;
    arrayForEachItem(int, (array_t*) pair -> info, i, id) {
      Pair_t * pair1 = SlfindPair(dep, id);
      assert(pair1);
      ok = traverse1(dep,  pair1, count);
      if (!ok) {
	Var_Variable_t * var = SlReturnVariableFromId(pair -> first);
	Main_MochaErrorPrint("loop: %s\n", 
			     Var_VariableReadName(var));
	return FALSE;
      }
    }

    pair -> second = *count;
    (*count) ++;
    return TRUE;
  }
}

/**Function********************************************************************

  Synopsis           [find pair, given id.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Pair_t *
SlfindPair(
  array_t * dep, 
  int id)
{
  int len = array_n (dep);
  int hi, lo, mid;
  Pair_t * pair;

  lo = 0;
  hi = len - 1;

  while (hi >= lo) {

    mid = (hi+lo) >> 1;
    pair = array_fetch(Pair_t *, dep, mid);
    if (pair -> first == id)
      return pair;
    
    if (pair -> first < id)
      lo = mid+1;

    if (pair -> first > id)
      hi = mid-1;
  }

  return NIL(Pair_t);
}


/**Function********************************************************************

  Synopsis           [print variable dependency relation.]

  Description        [ if a < b, a < c, b < d, then the output would be
                       {a, b}, {a, c}, {b, c}.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPrintAwaitDepCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_printawaitDep <fsm>";
  int i, id;
  char * fsmName;
  Sl_GSTEntry_t *  obj;
  array_t * dep;
  Pair_t * pair;

  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName = argv[1];
  if (!st_lookup(Sl_GST, fsmName, (char **) &obj)) {
    Main_MochaErrorPrint("Error: FSM %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  
  if (obj -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: FSM %s does not exist.\n", fsmName);
    return TCL_OK;    
  }


  Main_ClearResult();
  dep = obj -> obj.fsm.dependency;
  
  if (dep == NIL(array_t)) {
    Main_MochaErrorPrint("Error: Await dependency contains cycle. \n");
    return TCL_OK;
  }
  
  arrayForEachItem(Pair_t *, dep, i, pair) {
    char * varName;
    array_t * awaitVars;
    int j;
    Pair_t * pair1;
   
    varName = Var_VariableReadName(SlReturnVariableFromId(pair -> first));
    awaitVars = (array_t *) (pair -> info);

    if (array_n(awaitVars) == 0) {
      Main_AppendResult("{");
      Main_AppendElement("%s", varName);
      Main_AppendResult("} ");
    } else {
      arrayForEachItem(Pair_t *, awaitVars, j, pair1) {
        Main_AppendResult ("{");
        Main_AppendElement("%s", varName);
        Main_AppendElement("%s",
                           Var_VariableReadName(SlReturnVariableFromId(pair1
                                                                       ->
                                                                       first)));
        
        Main_AppendResult ("} ");
      }
    }
  }
  
  return TCL_OK;

}


/**Function********************************************************************

  Synopsis           [print the irreflexive transitive closure of dependency relation.]

  Description        [ if a < b, a < c, b < d, then the output would be
                       {a, b}, {a, c}, {b, d}, {a, d}]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPrintTCAwaitDepCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_printTcAwaitDep <fsm>";
  int i, id;
  char * fsmName;
  Sl_GSTEntry_t *  obj;
  array_t * dep;
  Pair_t * pair;

  util_getopt_reset();
  if (argc < 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName = argv[1];
  if (!st_lookup(Sl_GST, fsmName, (char **) &obj)) {
    Main_MochaErrorPrint("Error: FSM %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  
  if (obj -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: FSM %s does not exist.\n", fsmName);
    return TCL_OK;    
  }


  Main_ClearResult();
  dep = obj -> obj.fsm.dependency;
  
  if (dep == NIL(array_t)) {
    Main_MochaErrorPrint("Error: Await dependency contains cycle. \n");
    return TCL_OK;
  }
  
  arrayForEachItem(Pair_t *, dep, i, pair) {
    char * varName;
    array_t * awaitVars;
    int j;
    Pair_t * pair1;
   
    varName = Var_VariableReadName(SlReturnVariableFromId(pair -> first));
    awaitVars = (array_t *) (pair -> info);

    if (array_n(awaitVars) == 0) {
      Main_AppendResult("{");
      Main_AppendElement("%s", varName);
      Main_AppendResult("} ");
    } else {
      arrayForEachItem(Pair_t *, awaitVars, j, pair1) {
        PrintTransitiveAwaitVarsRecursively(varName, pair1);
      }
    }
  }
  
  return TCL_OK;

}



/**Function********************************************************************

  Synopsis           [print variables awaiting the given variable in the fsm]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlPrintAwaitVarCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_printawaitvar <fsm> <var>";
  int i, id;
  char * fsmName, *varName;
  Sl_GSTEntry_t *  obj;
  Pair_t * pair, *depPair;

  util_getopt_reset();
  if (argc < 3) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  fsmName = argv[1];
  varName = argv[2];
  if (!st_lookup(Sl_GST, fsmName, (char **) &obj)) {
    Main_MochaErrorPrint("Error: FSM %s does not exist.\n", fsmName);
    return TCL_OK;
  }
  
  if (obj -> kind != Sl_FSM_c) {
    Main_MochaErrorPrint("Error: FSM %s does not exist.\n", fsmName);
    return TCL_OK;    
  }

  id = SlObtainUnprimedBddId (varName);  
  if (id < 0) {
    Main_MochaErrorPrint("Error: Variable %s does not exists.\n", varName);
    return TCL_OK;
  }

  pair = SlfindPair(obj -> obj.fsm.dependency, id);
  if (!pair) {
    Main_MochaErrorPrint("Error: FSM %s does not have variable %s.\n",
			 fsmName, varName);
    return TCL_OK;
  }

  arrayForEachItem(Pair_t *, (array_t *) pair -> info, i, depPair) {
    Main_MochaPrint("%s\n", 
		    Var_VariableReadName(SlReturnVariableFromId(
			    depPair->first)));
  }

  /*arrayForEachItem(int, (array_t *) pair -> info, i, id) {
    Main_MochaPrint("%s\n", 
		    Var_VariableReadName(SlReturnVariableFromId(id)));
  }*/

  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [print variables awaiting the given variable in the fsm]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlWriteOrderCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char usage[] = "Usage: sl_write_order";
  char * fileName;
  FILE * fp;

  util_getopt_reset();
  if (argc > 2) {
    Main_MochaErrorPrint("Syntax error. %s\n", usage);
    return TCL_OK;
  }

  if (argc == 1) {
    fp = stdout;
  } else {
    fileName = argv[1];
    if (! (fp = fopen(fileName, "w"))) {
      Main_MochaErrorPrint("Cannot write file \"%s\".\n", fileName);
      return TCL_OK;
    }
  }
  
  SlWriteOrder(fp);
  if (fp != stdout) {
    fclose(fp);
    Main_MochaPrint("Variable Order written to \"%s\".\n", fileName);
  }
  
  return TCL_OK;

}




/**Function********************************************************************

  Synopsis           [Given an id, give a list of immediate successors.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static Pair_t *
ObtainListOfSuccessors(
  array_t * dep,
  int x,
  Pair_t ** lastpair
  )
{
  /* first search for the id in the dependency graph */
  int i = 0;
  int len = array_n(dep);
  Pair_t * pair, *pair2, **ptr, *successorList;
  
  while(i < len){
    pair = array_fetch(Pair_t *, dep, i);
    if (pair -> first == x)
      break;
    i ++;
  }

  assert(i!=len);
  
  /* for each id that awaits x, create a node in the returning list */
  successorList = NIL(Pair_t);
  ptr = &successorList;
  arrayForEachItem(Pair_t *, (array_t *) pair -> info, i, pair2) {
    if (pair2 -> second == 0) {
      pair2 -> second = 1;
      *ptr = SlPairAlloc();
      (*ptr) -> first = pair2 -> first;
      (*ptr) -> info = (char *) NIL(Pair_t);
      *lastpair = *ptr;
      ptr = (Pair_t **) &((*ptr) -> info);
    }
  }
  

  return successorList;

}

/**Function********************************************************************

  Synopsis           [Return the pointer to the controlled id to be removed.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static Pair_t *
DetermineControlledIdToRemove(
  Pair_t * hd)
{
  return hd;
}

/**Function********************************************************************

  Synopsis           [return a list of ids which do not await anything.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static Pair_t *
ObtainNonAwaitingElementsList(
  array_t * dep)
{
  Pair_t * pair, *pair1, *list, **ptr;
  int i,j;
  
  /* this is a linear time algorithm that checks each id
     in the table */

  /* in linear time, set the flag in each id to 0 to indicate it is
     not awaiting anybody */

  arrayForEachItem(Pair_t *, dep, i, pair){
    pair -> second = 0;
  }

  /* then set the flag to 1 if the id is awaiting some others */
  arrayForEachItem(Pair_t *, dep, i, pair){
    arrayForEachItem (Pair_t *, (array_t *) pair->info, j, pair1){
      pair1 -> second = 1;
    }
  }

  /* then create a list of those pairs whose flags are still 0 */
  list = NIL(Pair_t);
  ptr = &list;

  arrayForEachItem(Pair_t *, dep, i, pair){
    if (pair -> second == 0) {
      *ptr = SlPairAlloc();
      (*ptr) -> first = pair -> first;
      (*ptr) -> info = (char *) NIL(Pair_t);
      ptr = (Pair_t **) &((*ptr) -> info);
    }
  }

  /* reset the flags to zero */
  arrayForEachItem(Pair_t *, dep, i, pair){
    pair -> second = 0;
  }
  

  return list;
  
}

/**Function********************************************************************

  Synopsis           [Return 1 if id in fsm is external]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
IdIsExternal(
  Sl_FSM_t fsm,
  int id)
{
  int unprimedId = SlReturnUnprimedId(id);

  /* check if it is in externvarl of fsm */
  int len = array_n(fsm.extvarL);
  int l=0, u=len;

  while (u > l){
    int mid = (l+u)/2;
    
    if (unprimedId == array_fetch(int, fsm.extvarL, mid))
      return TRUE;
    else if (unprimedId > array_fetch(int, fsm.extvarL, mid))
      l = mid+1;
    else
      u = mid;
  }
  
  return FALSE;
}


/**Function********************************************************************

  Synopsis           [Write variable order for slMddManager into a file]

  Description        [Write variable order for slMddManager into a file]

  SideEffects        [none]
  
  SeeAlso            [SlReadOrder]

******************************************************************************/
static void
SlWriteOrder(
  FILE *fp)
{
  VarOrder_t *varOrder;
  lsList varOrderList = lsCreate();
  int nameLength, maxNameLength, i;
  st_generator *stgen;
  char *name;
  Var_Variable_t *var;
  lsGen lsgen;
  Pair_t * pair;
  
  arrayForEachItem(Pair_t *, SlGetIdPairArray(), i, pair){
    Var_Type_t * type;
    var = (Var_Variable_t *) pair -> info;
    type = Var_VariableReadType(var);
    
    if (!Var_VariableIsDummy(var) &&
        !Var_TypeIsArray(type) &&
        !Var_TypeIsBV(type)) {

      varOrder = ALLOC(VarOrder_t, 1);
      varOrder->pair = pair;
      varOrder->prime = 0;
      varOrder->bddIdArray = VarBuildBddIdArray(pair, 0);
      lsNewEnd(varOrderList, (lsGeneric) varOrder, LS_NH);
      
      varOrder = ALLOC(VarOrder_t, 1);
      varOrder->pair = pair;
      varOrder->prime = 1;
      varOrder->bddIdArray = VarBuildBddIdArray(pair, 1);
      lsNewEnd(varOrderList, (lsGeneric) varOrder, LS_NH);
    }
  }

  lsSort(varOrderList, VarOrdersCompareBddIdArray);
  lsUniq(varOrderList, VarOrdersCompareVariable, free);
  
  /*
   * Compute the maximum length of a node name, for pretty printing.
   */
  maxNameLength = 0;
  lsForEachItem(varOrderList, lsgen, varOrder) {
    int nameLength = strlen(Var_VariableReadName((Var_Variable_t *)
                                                 varOrder->pair->info));
    maxNameLength = (maxNameLength < nameLength) ? nameLength : maxNameLength;
  }

  /* Increase by one to account for the " ' " at the end. */
  
  maxNameLength++;
  
  lsForEachItem(varOrderList, lsgen, varOrder) {
    int i;
    array_t *bddIdArray = varOrder->bddIdArray;
    Var_Variable_t *var = (Var_Variable_t *) varOrder->pair->info;
    char *varTypeName = Var_VarTypeReadName(Var_VariableReadType(var));
    int mddId;
    
    if (varOrder->prime) {
      char *primedName = util_strcat3(Var_VariableReadName(var), "'", "");
      mddId = varOrder->pair -> second;
      /* Sym_SymInfoLookupPrimedVariableId(symInfo, var, &mddId); */
      (void) Main_MochaFilePrint(fp, "%-*s  %-16s %5d ",
                     maxNameLength,
                     primedName,
                     varTypeName,
                     mddId);

      FREE(primedName);
    }
    else {
      mddId = varOrder -> pair -> first;
      /* Sym_SymInfoLookupUnprimedVariableId(symInfo, var, &mddId); */
      (void) Main_MochaFilePrint(fp, "%-*s  %-16s %5d ",
                     maxNameLength,
                     Var_VariableReadName(var),
                     varTypeName,
                     mddId);
      }
        
    (void) Main_MochaFilePrint(fp, "(");
    for (i = 0; i < array_n(bddIdArray); i++) {
      int level = array_fetch(int, bddIdArray, i);
      
      if (i == 0) {
        (void) Main_MochaFilePrint(fp, "%d", level);
      }
      else {
        (void) Main_MochaFilePrint(fp, ", %d", level);
      }
    }
    (void) Main_MochaFilePrint(fp, ")\n");
    array_free(bddIdArray);
    FREE(varOrder);
  }

  lsDestroy(varOrderList, (void (*) ()) 0);
}



/**Function********************************************************************

  Synopsis    [Gets the levels of the BDD variables corresponding to the MDD
  variable of a node.]

  Description [Returns an array (of int's) of the levels of the BDD variables
  corresponding to the MDD variable of a node.  The level of a BDD variable is
  it place in the current variable ordering of the BDD manager.  The returned
  array is sorted in ascending order.  It is the responsibility of the caller
  to free this array.]

  SideEffects []

******************************************************************************/
static array_t *
VarBuildBddIdArray(
  Pair_t * pair,
  boolean primed)
{
  int i;
  mdd_manager *mddManager = SlGetMddManager();
  array_t *mvarArray = mdd_ret_mvar_list(mddManager);
  int mddId;
  mvar_type mv;
  array_t *bddIdArray;
  
  if (!primed)
    mddId = pair -> first;
  else
    mddId = pair -> second;

  mv = array_fetch(mvar_type, mvarArray, mddId);
  bddIdArray = array_alloc(int, mv.encode_length);

  for (i = 0; i < mv.encode_length; i++) {
    int    bddId    = mdd_ret_bvar_id(&mv, i);
    bdd_t *varBdd   = bdd_get_variable((bdd_manager *) mddManager, bddId);
    int    varLevel = (int) bdd_top_var_level((bdd_manager *) mddManager, varBdd);

    bdd_free(varBdd);
    array_insert_last(int, bddIdArray, varLevel);
  }
  array_sort(bddIdArray, IntegersCompare);

  return (bddIdArray);
}



/**Function********************************************************************

  Synopsis    [Used to sort an array of integers in ascending order.]

  SideEffects []

******************************************************************************/
static int
IntegersCompare(
  char ** obj1,
  char ** obj2)
{
  int int1 = *(int *) obj1;
  int int2 = *(int *) obj2;
  
  if (int1 < int2) {
    return -1;
  }
  else if (int1 == int2) {
    return 0;
  }
  else {
    return 1;
  }
}

/**Function********************************************************************

  Synopsis    [Used to sort an array of nodes in ascending order of lowest BDD
  level.]

  SideEffects []

******************************************************************************/
static int
VarOrdersCompareBddIdArray(
  lsGeneric node1,
  lsGeneric node2)
{
  array_t *bddIdArray1 = ((VarOrder_t *) node1)->bddIdArray;
  array_t *bddIdArray2 = ((VarOrder_t *) node2)->bddIdArray;
  int      firstLevel1    = array_fetch(int, bddIdArray1, 0);
  int      firstLevel2    = array_fetch(int, bddIdArray2, 0);
  
  if (firstLevel1 < firstLevel2) {
    return -1;
  }
  else if (firstLevel1 == firstLevel2) {
    return 0;
  }
  else {
    return 1;
  }
}

/**Function********************************************************************

  Synopsis    [Used to sort an array of nodes in ascending order of lowest BDD
  level.]

  SideEffects []

******************************************************************************/
static int
VarOrdersCompareVariable(
  lsGeneric node1,
  lsGeneric node2)
{
  VarOrder_t * varOrder1 = (VarOrder_t *) node1;
  VarOrder_t * varOrder2 = (VarOrder_t *) node2;

  return !((varOrder1 -> pair -> info == varOrder2 -> pair -> info) &&
           (varOrder1 -> prime == varOrder2 -> prime));
}



/**Function********************************************************************

  Synopsis           [Read variable order from a file]

  Description        [Read variable order from a file, adapted from reactive
  symIntf.c]

  SideEffects        [For the moment, primed and unprimed vars have to be
  adjacent.]
  
  SeeAlso            [SymWriteOrder]

******************************************************************************/
static array_t *
SlReadOrder(
  FILE *fp,
  array_t * moduleArray
  )
{
  #define MAX_NAME_LENGTH 200
  /*
 * States of the state machine used to parse the input node list file.
 */
  #define STATE_TEST 0 /* next char is in first column */
  #define STATE_WAIT 1 /* wait until end of line '\n' is reached */
  #define STATE_IN   2 /* parsing a node name */

  int     c, i, prime;
  int     state;
  int     curPosition = 0;
  boolean status;
  char    nameString[2][MAX_NAME_LENGTH];
  array_t *pairArray = array_alloc(Pair_t *, 0);
  Pair_t * pair;
  Var_Variable_t *var;
  int whichString = 0;
  
  state = STATE_TEST;
  /* second conjunct handle case where EOF terminates a name. */
  while (((c = fgetc(fp)) != EOF) || state == STATE_IN) {

    switch (state) {
        case STATE_TEST:
          /* At start of a new line. */
          if (c == '#') {
            /* Line starting with comment character; wait for newline */
            state = STATE_WAIT;
          }
          else if ((c == ' ') || (c == '\t')) {
            /* Line starting with white space; wait for newline */
            state = STATE_WAIT;
          }
          else if (c == '\n') {
            /* Line starting newline; go to next line */
            state = STATE_TEST;
          }
          else {
            /* Assume starting a node name. */
            curPosition = 0;
            nameString[whichString][curPosition++] = c;
            state = STATE_IN;
          }
          break;
        case STATE_WAIT:
          /*
           * Waiting for the newline character.
           */
          state = (c == '\n') ? STATE_TEST : STATE_WAIT;
          break;
        case STATE_IN:
          /*
           * Parsing a node name.  If white space reached, then terminate the
           * node name and process it.  Else, continue parsing.
           */
          if ((c == ' ') || (c == '\n') || (c == '\t') || (c==EOF)) {
            nameString[whichString][curPosition] = '\0';

            /* Check if the last character in the string is " ' " (the prime
             * symbol denoting that the variable is the next state variable.
             */

            if (nameString[whichString][curPosition-1] == '\'') {
              prime = 1;
              nameString[whichString][curPosition-1] = '\0';
            }
            else
              prime = 0;

            /* for the moment, require that primed and unprimed ids have
               to be adjacent
               */

            if (whichString == 1) {
              if (strcmp(nameString[0], nameString[1])) {
                Main_MochaErrorPrint(
                  "Error: Variables %s and %s' not adjacent.\n", nameString[0],
                  nameString[0]);
                goto bad;
              }
            }

            /* modify here */
            var = NIL(Var_Variable_t);
            i = 0;
            while (var == NIL(Var_Variable_t) && i < array_n(moduleArray)) {
              var = Mdl_ModuleReadVariableFromName(nameString[whichString],
                                                   array_fetch(Mdl_Module_t *,
                                                               moduleArray,
                                                               i));
              i ++;
            }
            
            if (var == NIL(Var_Variable_t)) {
              Main_MochaErrorPrint("Warning: Variable \"%s\" not found. Ignored.\n",
                                   nameString[whichString]);
            }
            else {
              pair = SlPairAlloc();
              pair -> info = (char *) var;
              pair -> first = prime;
              array_insert_last(Pair_t *, pairArray,  pair);  
            }

            whichString = 1 - whichString;
            state = (c == '\n') ? STATE_TEST : STATE_WAIT;


          } else {
            nameString[whichString][curPosition++] = c;
            if (curPosition >= MAX_NAME_LENGTH) {
              int NUM_CHAR_DISPLAY = 10;
              nameString[whichString][NUM_CHAR_DISPLAY] = '\0';
              Main_MochaErrorPrint("Warning: Variable name %s too long. Ignored.\n",
                                   nameString[whichString]);
              state = (c == '\n') ? STATE_TEST : STATE_WAIT;
            } else 
              state = STATE_IN; /* redundant, but be explicit */
          }
          break;
        default:
          fail("unrecognized state");
    }
  }
  
  /* Check for duplicate occurrences of variables. If it occurs, print a
   *  warning and remove the second occurrence.
   */
  {
    array_t *tempArray = array_alloc(Pair_t *, 0);
    arrayForEachItem(Pair_t *, pairArray, i, pair) {
      if (PairIsInArray(tempArray, pair) == -1)
        array_insert_last(Pair_t *, tempArray, pair);
      else {
        Main_MochaErrorPrint("Warning: duplicate names in the input file\n");
        FREE(pair);
      }
    }
    array_free(pairArray);
    return tempArray;
  }

  bad:
  arrayForEachItem(Pair_t *, pairArray, i, pair) {
      FREE(pair);
    }
    array_free(pairArray);
    return NIL(array_t);
}

/**Function********************************************************************

  Synopsis           [check if a variable is in a varOrderArray]

  Description        [check if a variable is in a varOrderArray]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
static int
PairIsInArray(
  array_t *pairArray,
  Pair_t * pair)
{
  int i;
  Pair_t *temp;
  
  arrayForEachItem(Pair_t *, pairArray, i, temp) {
    if ((temp -> info == pair -> info) &&
        (temp -> first == pair -> first) &&
        (temp -> second == pair -> second))
      return i;
  }

  return -1;
}

/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
int SlReturnRandomValue(
  int range) 
{
  int r = (int) ((range - 1) * rand()/(RAND_MAX));
  return r;
}

/**Function********************************************************************

  Synopsis           []

  Description        []

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
static mdd_t *
getMddFromLeftAndRightAtmExpr(
  Var_Variable_t * var,
  Atm_Expr_t * leftExpr, 
  Atm_Expr_t * rightExpr)
{
  /* now, build the atmExpr and mdd  */
  Atm_Expr_t * typedAtmExpr;
  mdd_t * mdd;
  Atm_Expr_t * atmExpr = Atm_ExprAlloc(Atm_Equal_c,  leftExpr, rightExpr);

  /* type check, as well as adding type information */
  Atm_TypeCheckNew(BoolType, &atmExpr);
    
  mdd = Sym_ExprBuildMdd(
    Sym_RegionManagerReadSymInfo(SlGetRegionManager(),
                                 (Mdl_Module_t *) Var_VariableReadModule(var)),
    atmExpr);
  
  Atm_ExprFreeExpression(atmExpr);

  return mdd;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void PrintTransitiveAwaitVarsRecursively(
  char * varName,
  Pair_t * pair) 
{
  int i;
  Pair_t * pair1;
  
  Main_AppendResult ("{");
  Main_AppendElement("%s", varName);
  Main_AppendElement("%s",
                     Var_VariableReadName(SlReturnVariableFromId(pair
                                                                 ->
                                                                 first)));
  Main_AppendResult ("} ");

  arrayForEachItem(Pair_t *, (array_t *) pair -> info, i, pair1) {
    PrintTransitiveAwaitVarsRecursively(varName, pair1);
  }
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
ObtainUnPrimedVarName(
  char * varName,
  char ** unPrimedVarName
  ) 
{
  char *p, *q;
  int flag = FALSE;
  
  q = ALLOC(char, strlen(varName) + 1);
  *unPrimedVarName = q;
  
  p = varName;

  while(*p != '\0') {

    if (*p != '\'') {
      memset(q, *p, 1);
      q ++;
    } else {
      flag = 1;
    }
   
    p++;
    
  }

  memset(q, 0, 1);

  return flag;

}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
parseNameEqValue(
  char *vv,
  char ** name,
  int * value)
{
  char *p;
  int len;

  p = vv;
  while ((*p != '=') && (*p != '\0')){
    p++;
  }

  if (*p == '\0') {
    return 0;
  }

  if (p == vv) {
    return 0;
  }
  
  len = p-vv;
  
  *name = ALLOC(char, len + 1);
  memcpy(*name, vv, len);
  memset(*name+len, 0, 1) ;

  *value = atoi(p+1);
  
  return 1;
}


/* 
 * $Log: slBase.c,v $
 * Revision 1.9  2003/05/02 05:27:56  luca
 * Added Log keywords
 *
 * 
 */ 
