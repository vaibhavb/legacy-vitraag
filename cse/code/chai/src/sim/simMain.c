/**CFile***********************************************************************

  FileName    [simMain.c]

  PackageName [sim]

  Synopsis    [Routines for commands and interface with Tcl]

  Description []

  SeeAlso     [Other files from the sim package.]

  Author      [Freddy Mang]

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

#include  "simInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define Usage_sim_start "sim_start <module> [submodule1 submodule2 ...]"
#define Usage_sim_end "sim_end <module>"
#define Usage_sim_choice "sim_choice [-n | -N <num> | -i <num> -j <num>] <module>"
#define Usage_sim_select "sim_select module selection"
#define Usage_sim_mode "sim_mode"
#define Usage_sim_prev_state_print "sim_prev_state_print"
#define Usage_sim_info "sim_info"

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

static int SimStartCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int SimEndCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int SimChoiceCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int SimSelectCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int SimModeCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int SimPrevStatePrintCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int SimInfoCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static boolean SimIsInitialRound(Sim_Info_t *simInfo);
static boolean SimIsUserTurn(Sim_Info_t * simInfo);
static void FreeStringArray(array_t* array);
static boolean SimIsStartOfRound(Sim_Info_t *simInfo);
static char * SimModeOfSimulation(Sim_Info_t * simInfo);
static array_t * SimObtainAtomArrayFromAtomNames(Mdl_Module_t * module, array_t *nameArray);
static char * SimAtomControlVariablesConvertToString(Sim_Atom_t *simAtom);
static boolean SimModuleIsInfinite(Mdl_Module_t * module);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Initialization of the sim package.]

  Description        [This function is needed by tcl/tk to initialize this
  package. Initialization includes registering of commands and initialization
  of internal data structures used by this package.]

  SideEffects        [SimManagerInit() is called and a "global" data
  structure, namely the simulation manager is created.]

  SeeAlso            [optional]

******************************************************************************/
int
Sim_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{

  Tcl_CreateCommand(interp, "sim_start",
                    SimStartCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sim_end",
                    SimEndCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sim_choice",
                    SimChoiceCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sim_select",
                    SimSelectCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sim_mode",
                    SimModeCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sim_prev_state_print",
                    SimPrevStatePrintCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "sim_info",
                    SimInfoCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  /* also initialize some data structures used in this package */
  SimManagerInit();

  return TCL_OK;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Command for sim_start]

  SideEffects        [required]

  CommandName        [sim_start] 	   

  CommandSynopsis    [start simulation for module]  

  CommandArguments   [\[-hsun\] &lt;module&gt; \[&lt;component&gt; ...\]]  

  CommandDescription [This command starts and initialize simulation of the
  specified module.  There are three modes of simulations: random, manual and
  game simulation.  In general, user also specify the components that are
  controlled by the user to start a game simulation. For random and manual
  simulations which are two extreme cases of game simulation, there is no need
  to specify the components: for random simulation, all components are
  controlled by the system, while in manual simulation, all are controlled by
  the user. <p>

  Command options: <p>

  <dl>
  <dt> -h
  <dd> Prints usage.

  <dt> -s
  <dd> Specify that the specified components are controlled by the system.
  If no component is specified, the simulation is the same as manual
  simulation (no component is controlled by the system).

  <dt> -u
  <dd> Specify that the specified components are controlled by the user.
  If no component is specified, the simulation is the same as random
  simulation (no component is controlled by the user).  This is the default
  option. 

  <dt> -n
  <dd> Specify that the component names given are actually atom names. It is
  illegal to mix both sub-module names and atom names. Without this option,
  the component names are assumed to be sub-module names.
  
  <dt> &lt;component&gt ... ;
  <dd> Names of the sub-modules that are controlled by the user (system in the
  case of -s is specified). If -n is used, the names are atom names.

  </dl>]



******************************************************************************/
static int
SimStartCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Mdl_Module_t *module;
  array_t *nameArray, *atomArray;
  boolean isUserAtom = TRUE, atomNamesGiven=FALSE;
  char c, *name;
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "hsun")) != -1) {
    switch (c) { 
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_sim_start);
          return TCL_OK;
          break;
        case 's':
          isUserAtom = FALSE;
          break;
        case 'u':
          isUserAtom = TRUE;
          break;
        case 'n':
          atomNamesGiven = TRUE;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_sim_start);
          return TCL_ERROR;
          break;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("%s\n", Usage_sim_start);
    return TCL_ERROR;
  }

  /* read the module */
  name = argv[util_optind];
  module = Mdl_ModuleReadFromName(mdlManager, name);

  if (!module) {
    Main_MochaErrorPrint("Module %s not exists.\n", name);
    return TCL_ERROR;
  }

  /* also check to make sure the module is a finite module */
  if (SimModuleIsInfinite (module)) {
    Main_MochaErrorPrint("Module %s cannot be simulated.", name);
    Main_MochaErrorPrint("It contains variables of infinite type.\n");
    return TCL_ERROR;
  }

  /* collect all the userAtoms */
  nameArray = array_alloc(char*, 0);
  util_optind++;
  while (util_optind < argc){
    array_insert_last(char*, nameArray, argv[util_optind]);
    util_optind++;
  }
  
  /* remove duplicate names  */
  array_sort(nameArray, strcmp);
  array_uniq(nameArray, strcmp, free);

  /* search for the components */
  if (!atomNamesGiven)
    atomArray =
        Mdl_ModuleObtainComponentAtomArray(module, nameArray);
  else
    atomArray =
        SimObtainAtomArrayFromAtomNames(module, nameArray);
  
  array_free(nameArray);
  
  if (!atomArray){
    return TCL_ERROR;
  }
  
  SimSimulationStart(module, atomArray, isUserAtom);
  
  array_free(atomArray);
  
  return TCL_OK;

}


/**Function********************************************************************

  Synopsis           [Command for sim_end]

  SideEffects        [required]

  CommandName        [sim_end] 

  CommandSynopsis    [Ends the current simulation for the specified module.]

  CommandArguments   [\[-h\] &lt;module&gt;]  

  CommandDescription [This command ends the simulation for the specified
  module. Note that only one simulation can be started for the each module at
  one time. This command can be used to end the current simulation and restart
  the simulation. <p>

  Command options: <p>

  <dl>
  <dt> -h
  <dd> Prints usage.

  <dt> &lt;module&gt;
  <dd> Specify the module whose simulation to be ended.
  
  </dl>

  ]  

******************************************************************************/
static int
SimEndCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  char c, *moduleName;
  Sim_Info_t *simInfo;

  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) { 
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_sim_end);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_sim_end);
          return TCL_ERROR;
          break;
    }
  }

  if (argc - util_optind != 1) {
    Main_MochaErrorPrint("%s\n", Usage_sim_end);
    return TCL_ERROR;
  }

  moduleName = argv[util_optind];
  if (!Mdl_ModuleReadFromName(mdlManager, moduleName)){
    Main_MochaErrorPrint("Module %s not exists.\n", moduleName);
    return TCL_ERROR;
  }

  simInfo = SimManagerDeleteSimInfo(moduleName);

  if (simInfo == NIL(Sim_Info_t)) {
    Main_MochaErrorPrint("Warning: Module %s is not being simulated.\n",
                         moduleName);
  }

  SimSimInfoFree(simInfo);

  Main_AppendResult("1\n");
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Command for sim_choice]

  SideEffects        [required]

  CommandName        [sim_choice] 

  CommandSynopsis    [Return the choices available to user.]  

  CommandArguments   [\[-hnx\] &lt;module&gt;]

  CommandDescription [This command returns the choices available to the
  user during the current simulation for the specified module. <p>

  Command options: <p>

  <dl>
  <dt> -h
  <dd> Prints usage.

  <dt> -n
  <dd> Returns only the number of choices.

  <dt> -x
  <dd> Returns the result in the format of a Tcl list. 
  
  <dt> &lt;module&gt;
  <dd> Specify the module.
  
  </dl>

  ]  

******************************************************************************/
static int
SimChoiceCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
      Main_ManagerReadStateManager((Main_Manager_t *) clientData);
  char c, *moduleName;
  Sim_Info_t *simInfo;
  lsList choice;
  Mdl_Module_t *module;
  Enum_VarInfo_t *varInfo;
  Enum_State_t *partialState;
  lsGen gen;
  int count;
  boolean opt_n = FALSE, opt_x = FALSE, opt_i = FALSE;
  char * arg_i;
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "hnxi:")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_sim_choice);
          return TCL_ERROR;
          break;
        case 'n':
          opt_n = TRUE;
          break;
        case 'x':
          opt_x = TRUE;
          break;
        case 'i':
          opt_i = TRUE;
          arg_i = util_optarg;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_sim_choice);
          return TCL_OK;
          break;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("%s\n", Usage_sim_choice);
    return TCL_ERROR;
  }

  moduleName = argv[util_optind];
  module = Mdl_ModuleReadFromName(mdlManager, moduleName);
  if (!module){
    Main_MochaErrorPrint("Module %s not exists.\n", moduleName);
    return TCL_ERROR;
  }

  simInfo = SimManagerReadSimInfo(moduleName);

  if (simInfo == NIL(Sim_Info_t)) {
    Main_MochaErrorPrint("Warning: Module %s is not simulated.\n",
                         moduleName);
    return TCL_ERROR;
  }

  choice = SimSimInfoReadChoice(simInfo);
  varInfo = SimSimInfoReadVarInfo(simInfo);

  count = 0;
  Main_ClearResult();
  if (opt_n) {
    /* only number of choices wanted */
    Main_AppendResult("%d", lsLength(choice));
    return TCL_OK;
  }

  if (opt_i) {
    int i = atoi(arg_i);
    if (! (i < lsLength(choice))) {
      Main_MochaErrorPrint(
        "Error: Index (opt_i) greater than the number of choices.\n");
      return TCL_ERROR;
    } else {
      lsGen gen;
      Enum_State_t * state;
      int j = 0;
      char * stateName;

      lsForEachItem(choice, gen, state) {
        if (j == i) {
          stateName = Enum_StateNameTableAddStateWithNameSuffix(
            stateManager, moduleName, state, "temp");
        } else {
          j++;
        }
      }
      Main_AppendElement("%s", stateName);
      FREE(stateName);
      return TCL_OK;
    }
  }
    
  lsForEachItem(choice, gen, partialState){
    char * string = Enum_PartialStatePrintToString(varInfo, partialState);

    if (opt_x)
      Main_AppendElement("%s", string);
    else
      Main_AppendResult("%d. %s\n", count, string);
    FREE(string);
    count ++;
  }
  
  return TCL_OK;
}



/**Function********************************************************************

  Synopsis           [Picks the choice during simulation.]

  SideEffects        [required]

  CommandName        [sim_select] 	   

  CommandSynopsis    [Select the choice for the current simulation.]  

  CommandArguments   [\[-h\] &lt;module&gt; &lt;selection&gt;]  

  CommandDescription [This command selects the choice for the simulation of
  the specified module. Typically, if the execution is the user's turn, the
  user uses the command "sim_choice &lt;module&gt;" to examine the available
  choices. Then the user selects one of the choices by providing a number
  corresponding to the desired choice. <p>

  Command options:<p>

  <dl>
  <dt> -h
  <dd> Prints usage.

  <dt> &lt;module&gt;
  <dd> Specifies the module.

  <dt> &lt;selection&gt;
  <dd> A number that specifies the desired selection. The choices are shown
  using the command "sim_choice".
  
  </dl>

  ]

******************************************************************************/
static int
SimSelectCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  int numChoice, flag, index;
  char *moduleName;
  Sim_Info_t *simInfo;
  Mdl_Module_t *module;
  lsList choice;
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  char c;
  char * stateName;
  
  /* parse the options and do necessary checking first */
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "hn")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_sim_select);
          return TCL_ERROR;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_sim_select);
          return TCL_ERROR;
          break;
    }
  }

  if (argc - util_optind != 2) {
    Main_MochaErrorPrint("%s\n", Usage_sim_select);
    return TCL_ERROR;
  }
  
  moduleName = argv[util_optind];
  index = atoi(argv[util_optind+1]);
  
  module = Mdl_ModuleReadFromName(mdlManager, moduleName);
  if (!module){
    Main_MochaErrorPrint("Module %s not exists.\n", moduleName);
    return TCL_OK;
  }

  simInfo = SimManagerReadSimInfo(moduleName);
  
  if (simInfo == NIL(Sim_Info_t)) {
    Main_MochaErrorPrint("Warning: Module %s is not simulated.\n",
                         moduleName);
    return TCL_OK;
  }
  
  choice = SimSimInfoReadChoice(simInfo);
  numChoice = lsLength(choice);

  Main_ClearResult();
  if (index >= 0 &&  index<numChoice){
    Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
        Main_ManagerReadStateManager((Main_Manager_t *) clientData);
    stateName = SimStateUpdateWithChoice(stateManager, simInfo, index);
    Main_AppendResult("%s", stateName);
    return TCL_OK;
  } else {
    Main_MochaErrorPrint("Error: Choice index out of range.\n");
    return TCL_OK;
  }

}

/**Function********************************************************************

  Synopsis           [required]

  SideEffects        [required]

  CommandName        [sim_mode] 	   

  CommandSynopsis    [Changes the mode of simulation.]

  CommandArguments   [\[-hsun\] &lt;module&gt; \[&lt;component&gt; ...\]

  CommandDescription [This routine changes the mode of simulation. Note that
  the mode of simulation can only be changed at the begining of a round.  Thus
  if the user wants to change the mode, say from game simulation to random
  simulation, the user has to complete the current round before making the
  change. <p>

  For command options, see the command documentation for sim_start.

  ]  

******************************************************************************/
static int
SimModeCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Mdl_Module_t *module;
  array_t *nameArray, *atomArray, *simAtomOrder;
  Sim_Info_t *simInfo;
  boolean isUserAtom = TRUE, atomNamesGiven = FALSE;
  char c, *name;
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "sunh")) != -1) {
    switch (c) { 
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_sim_mode);
          return TCL_OK;
          break;
        case 's':
          /* the specified components are controlled by the system */
          isUserAtom = FALSE;
          break;
        case 'u':
          /* the specified components are controlled by the user */
          isUserAtom = TRUE;
          break;
        case 'n':
          /* the names given are atom names, not module names */
          atomNamesGiven = TRUE;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_sim_mode);
          return TCL_ERROR;
          break;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("%s\n", Usage_sim_mode);
    return TCL_ERROR;
  }

  /* read the module */
  name = argv[util_optind];
  module = Mdl_ModuleReadFromName(mdlManager, name);

  if (!module) {
    Main_MochaErrorPrint("Module %s not exists.\n", name);
    return TCL_ERROR;
  }

  simInfo = SimManagerReadSimInfo(name);

  if (!simInfo) {
    Main_MochaErrorPrint("Module %s is not being simulated.\n", name);
    return TCL_ERROR;
  }
  
  /* Check if currently in the middle of a round */
  simAtomOrder = SimSimInfoReadSimAtomOrder(simInfo);
  
  if (!SimIsStartOfRound(simInfo)) {
    Main_MochaErrorPrint(
      "Cannot change simulation mode in the middle of a round.\n");
    Main_MochaErrorPrint("Please complete the current round first.\n");
    return TCL_ERROR;
  }
  
  /* collect all the userAtoms */
  nameArray = array_alloc(char*, 0);
  util_optind++;
  while (util_optind < argc){
    array_insert_last(char*, nameArray, argv[util_optind]);
    util_optind++;
  }
  
  /* remove duplicate names  */
  array_sort(nameArray, strcmp);
  array_uniq(nameArray, strcmp, free);

  /* collect all the components */
  if (!atomNamesGiven)
    atomArray =
        Mdl_ModuleObtainComponentAtomArray(module, nameArray);
  else
    atomArray =
        SimObtainAtomArrayFromAtomNames(module, nameArray);
  
  array_free(nameArray);
  
  if (!atomArray){
    return TCL_ERROR;
  }
  
  /* obtain the simulation atom order */
  Sim_SimAtomArrayFree(simAtomOrder);
  simAtomOrder = Sim_SimAtomReturnSortedArray(module, atomArray, isUserAtom);
  array_free(atomArray);
    
  /* redo computing choices */
  assert (array_n(simAtomOrder) != 0);
  SimSimInfoSetSimAtomOrder(simInfo, simAtomOrder);
  SimSimInfoSetIndex(simInfo, 0);
  SimComputeChoice(simInfo);
  
  return TCL_OK;

}

/**Function********************************************************************

  Synopsis           [sim_prev_state_print]

  SideEffects        [required]

  CommandName        [sim_prev_state_print] 	   

  CommandSynopsis    [Prints the previous state.]  

  CommandArguments   [\[-h\] &lt;module&gt;]  

  CommandDescription [This command prints the previous state of the module in
  the current simulation.  <p>

  Command Options: <p>

  <dl>
  <dt> -h
  <dd> Prints usage.

  <dt> &lt;module&gt;
  <dd> Specifies the module.

  </dl>
  ]  

******************************************************************************/
static int
SimPrevStatePrintCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Mdl_Module_t *module;
  Sim_Info_t *simInfo;
  Enum_State_t* state;
  char c;
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) { 
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_sim_prev_state_print);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_sim_prev_state_print);
          return TCL_ERROR;
          break;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("%s\n", Usage_sim_prev_state_print);
    return TCL_ERROR;
  }

  /* read the module */
  module = Mdl_ModuleReadFromName(mdlManager, argv[util_optind]);

  if (!module) {
    Main_MochaErrorPrint("Module %s not exists.\n", argv[1]);
    return TCL_ERROR;
  }

  simInfo = SimManagerReadSimInfo(argv[1]);

  if (!simInfo) {
    Main_MochaErrorPrint("Module %s is not being simulated.\n", argv[1]);
    return TCL_ERROR;
  }

  state = SimSimInfoReadPrevState(simInfo);
  Main_ClearResult();
  if (state) {
    Enum_StateManager_t *stateManager = (Enum_StateManager_t *)
        Main_ManagerReadStateManager((Main_Manager_t *) clientData);
    char * stateName = Enum_StateNameTableAddState(
      stateManager, Mdl_ModuleReadName(module), state);
    Main_AppendResult(stateName);
    FREE(stateName);
  } else {
    Main_AppendResult("");
  }
  
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Implements the sim_info command]

  SideEffects        [required]

  CommandName        [sim_info]

  CommandSynopsis    [Provides miscellaneous information about the current
  simulation.]

  CommandArguments   [\[-h\] &lt;module&gt; &lt;option&gt;]  

  CommandDescription [This command returns miscellaneous information about
  current simulation of the specified module, depending on the option
  supplied. <p>

  Command options:<p>

  <dl>
  <dt> -h
  <dd> Prints usage.

  <dt> &lt;option&gt;
  <dd> option can be one of the following keyword:

  <p>
  <dl>
  <dt> isStartOfRound
  <dd> Returns 1 if the simulation is at the start of a round, and 0
  otherwise.

  <p>
  <dt> isEndOfRound
  <dd> Returns 1 if the current round has just finished, and 0
  otherwise. Usually if the simulation is at the end of a round, it is also at
  the start of the round and vice versa. But there is a subtlety: at the start
  of the initial round, isStartOfRound will return 1, but isEndOfRound will
  return 0.

  <p>
  <dt> atomOrdering
  <dd> Returns a linear order of the atoms. The sub-rounds of execution are
  carried according to this order.
  
  <p>
  <dt> variableOrdering
  <dd> Returns a linear order of the variables. Variables controlled by the
  same atoms are grouped together.
  
  <p>
  <dt> isUserTurn
  <dd> Returns 1 if it is the user's turn during the simulation, and 0
  otherwise. 
  
  <p>
  <dt> isInitialRound
  <dd> Returns 1 if the current round is the initial round, and 0 otherwise.
  
  <p>
  <dt> mode
  <dd> Returns the mode of the current simulation. The result is "random",
  "manual" or "game".
  
  <p>
  <dt> isSimulated
  <dd> Returns 1 if the module is being simulated, 0 otherwise.
  
  </dl>

  </dl> 
  


  ]  

******************************************************************************/
static int
SimInfoCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Mdl_Module_t *module;
  Sim_Info_t *simInfo;
  char c;
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) { 
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_sim_info);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_sim_info);
          return TCL_ERROR;
          break;
    }
  }

  if (argc != 3) {
    Main_MochaErrorPrint("%s\n", Usage_sim_info);
    return TCL_ERROR;
  }

  /* read the module */
  module = Mdl_ModuleReadFromName(mdlManager, argv[1]);

  if (!module) {
    Main_MochaErrorPrint("Module %s not exists.\n", argv[1]);
    return TCL_ERROR;
  }

  simInfo = SimManagerReadSimInfo(argv[1]);
  
  if (!strcmp(argv[2], "isSimulated")) {
    if (simInfo) 
      Main_AppendResult("%d", 1);
    else
      Main_AppendResult("%d", 0);
    return TCL_OK;
  }


  if (!simInfo) {
    Main_MochaErrorPrint("Module %s is not being simulated.\n", argv[1]);
    return TCL_ERROR;
  }

  /*
  /* what query?
   */

  Main_ClearResult();
  if (!strcmp(argv[2], "isEndOfRound")) {
    Main_AppendResult("%d",
                      SimIsStartOfRound(simInfo) &&
                      !SimIsInitialRound(simInfo));
    return TCL_OK;
  }

  if (!strcmp(argv[2], "isStartOfRound")) {
    Main_AppendResult("%d",
                      SimIsStartOfRound(simInfo));
    return TCL_OK;
  }

  if (!strcmp(argv[2], "atomOrdering")) {

    char i = 0;
    array_t *simAtomOrder = SimSimInfoReadSimAtomOrder(simInfo);
    
    for (i=0; i<array_n(simAtomOrder); i++){
      Sim_Atom_t* simAtom = array_fetch(Sim_Atom_t*, simAtomOrder, i);
      char* name = Sim_SimAtomReadName(simAtom);
      Main_AppendElement("%s", name);
    }
    
    return TCL_OK;
  }
  
  if (!strcmp(argv[2], "variableOrdering")) {
    
    char i = 0;
    array_t *simAtomOrder = SimSimInfoReadSimAtomOrder(simInfo);
    
    for (i=0; i<array_n(simAtomOrder); i++){
      Sim_Atom_t* simAtom = array_fetch(Sim_Atom_t*, simAtomOrder, i);
      char * variableString = SimAtomControlVariablesConvertToString(simAtom);
      Main_AppendElement("%s", variableString);
      FREE(variableString);
    }
    
    return TCL_OK;
  }
  


  if (!strcmp(argv[2], "isUserTurn")) {
    Main_AppendResult("%d", SimIsUserTurn(simInfo));
    return TCL_OK;
  }

  if (!strcmp(argv[2], "isInitialRound")) {
    Main_AppendResult("%d", SimIsInitialRound(simInfo));
    return TCL_OK;
  }

  if (!strcmp(argv[2], "mode")) {
    char *s = SimModeOfSimulation(simInfo);
    Main_AppendResult("%s", s);
    FREE(s);
    return TCL_OK;
  }

  
  
  
  Main_MochaErrorPrint("%s\n", Usage_sim_info);
    return TCL_ERROR;
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
SimIsInitialRound(
  Sim_Info_t *simInfo)
{
  if (SimSimInfoReadPrevState(simInfo))
    return FALSE;
  else
    return TRUE;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
SimIsUserTurn(
  Sim_Info_t * simInfo)
{
  int index = SimSimInfoReadIndex(simInfo);
  array_t * simAtomArray = SimSimInfoReadSimAtomOrder(simInfo);
  int numSimAtoms = array_n(simAtomArray);
  Sim_Atom_t *simAtom;
  
  if (index == 0)
    index = numSimAtoms - 1;
  else
    index = index - 1;
  
  simAtom = array_fetch (Sim_Atom_t *, simAtomArray, index);

  return (Sim_SimAtomReadIsUserAtom(simAtom));
}


/**Function********************************************************************

  Synopsis           [Frees an array of strings.]

  Description        [Frees an array of strings.]

  SideEffects        [none]

  SeeAlso            [optional]

******************************************************************************/
static void
FreeStringArray(
  array_t* array) 
{
  int i;
  char *data;

  arrayForEachItem(char *, array, i, data) {
    FREE(data);
  }

  array_free(array);
}

/**Function********************************************************************

  Synopsis           [Determines if the simulation is at the start of a round.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
SimIsStartOfRound(
  Sim_Info_t *simInfo)
{
  int index = SimSimInfoReadIndex(simInfo);
  array_t *simAtomArray = SimSimInfoReadSimAtomOrder(simInfo);
  int numSimAtoms = array_n(simAtomArray);
  Sim_Atom_t *simAtom;
  int i, isUserAtom;
  
  /* get the first simAtom */
  simAtom = array_fetch(Sim_Atom_t*, simAtomArray, 0);
  isUserAtom = Sim_SimAtomReadIsUserAtom(simAtom);

  if (index == 0)
    index = numSimAtoms;

  for (i=1; i<index; i++){
    simAtom = array_fetch(Sim_Atom_t*, simAtomArray, i);
    if (isUserAtom != Sim_SimAtomReadIsUserAtom(simAtom))
      return FALSE;
  }
  
  return TRUE;
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static char *
SimModeOfSimulation(
  Sim_Info_t * simInfo) 
{

  array_t *simAtomArray = SimSimInfoReadSimAtomOrder(simInfo);
  int numSimAtoms = array_n(simAtomArray);
  Sim_Atom_t *simAtom;
  int i, isUserAtom;
  
  /* get the first simAtom */
  simAtom = array_fetch(Sim_Atom_t*, simAtomArray, 0);
  isUserAtom = Sim_SimAtomReadIsUserAtom(simAtom);

  for (i=1; i<numSimAtoms; i++){
    simAtom = array_fetch(Sim_Atom_t*, simAtomArray, i);
    if (isUserAtom != Sim_SimAtomReadIsUserAtom(simAtom))
      return util_strsav("game");
  }
  
  
  if (isUserAtom == 0)
    return util_strsav("random");
  if (isUserAtom == 1)
    return util_strsav("manual");

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static array_t *
SimObtainAtomArrayFromAtomNames(
  Mdl_Module_t * module,
  array_t *nameArray)
{
  Atm_Atom_t * atom;
  array_t *atomArray = array_alloc (Atm_Atom_t*, 0);
  boolean flag = TRUE;
  char * name;
  int i, numNames = array_n (nameArray);

  for (i=0; i<numNames; i++) {
    name = array_fetch (char *, nameArray, i);
    atom = Mdl_ModuleReadAtomFromName(name,  module);
    if (!atom) {
      Main_MochaErrorPrint("%s is not an atom of module.\n", name);
      flag = FALSE;
      break;
    }

    array_insert_last(Atm_Atom_t*, atomArray, atom);
  }

  if (!flag) {
    array_free(atomArray);
    return NIL(array_t);
  } else {
    return atomArray;
  }
}


/**Function********************************************************************

  Synopsis           [Returns a string of control variables.]

  Description        [This routine returns a string of variables controlled by
  the atom represented by the simAtom. If the simAtom represents an external
  variable, then (a copy) of the variable name is returned.]

  SideEffects        [User should free the returned string after use.]

  SeeAlso            [optional]

******************************************************************************/
static char *
SimAtomControlVariablesConvertToString(
  Sim_Atom_t *simAtom)
{
  Atm_Atom_t * atom;

  atom = Sim_SimAtomReadAtom(simAtom);
  if (SimSimAtomIsExternalVariable(simAtom)) {
    return util_strsav(Var_VariableReadName((Var_Variable_t *) atom));
  }
  
  {
    lsList variableList = Atm_AtomReadControlVarList(atom);
    Var_Variable_t * variable;
    lsGen gen;
    char *s = util_strsav("");
    
    lsForEachItem(variableList, gen, variable){
      char *name = Var_VariableReadName(variable);
      char *s1;

      s1 = util_strcat3(s, " ", name);
      FREE(s);
      s = s1;
    }
    
    return s;

  }
  
}

/**Function********************************************************************

  Synopsis           [Check if the module is infinite.]

  Description        [This routine checks if the module contains any variables
  that are of infinite domain, namely variables that are of type int
  or nat. If so, return TRUE, else return FALSE. This is a conservative test
  since modules that contain these variables may not be infinite.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
SimModuleIsInfinite (
  Mdl_Module_t * module)
{
  lsList varList = Mdl_ModuleObtainVariableList(module);
  boolean flag = FALSE;
  lsGen gen;
  Var_Variable_t *var;
  
  lsForEachItem(varList, gen, var) {
    Var_DataType dataType;

    dataType = Var_VariableReadDataType((Var_Variable_t *) var);
    if(dataType == Var_Integer_c || dataType == Var_Natural_c)
      flag = TRUE;
  }

  lsDestroy(varList, NULL);

  return flag;
}

