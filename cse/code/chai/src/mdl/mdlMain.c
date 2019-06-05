/**CFile***********************************************************************

  FileName    [mdlIo.c]

  PackageName [mdl]

  Synopsis    [User available commands definition.]

  Description [This file contains all the user commands exported by this
  package. The commands are available on the command lines.]

  SeeAlso     [other files in this package.]

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

#include  "mdlInt.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define Usage_show_mdls		"Usage: show_mdls [haldg] [module]"
#define Usage_delete		"Usage: delete M1 M2 ..."
#define Usage_compose		"Usage: compose Module1 Module2"
#define Usage_hide		"Usage: hide var1 var2 ...  Module"
#define Usage_rename		"Usage: ren from_var to_var Module"
#define Usage_let		"Usage: let Module be Module1"
#define Usage_show_vars		"Usage: show_vars [-vHF|HD|EV|ALL] Module"
#define Usage_isEventVariable	"Usage: isEventVariable module variable"
#define Usage_isPrivateVariable	"Usage: isPrivateVariable module variable"
#define Usage_isInterfaceVariable "Usage: isInterfaceVariable module variable"
#define Usage_isExternalVariable  "Usage: isExternalVariable module variable"
#define Usage_isHistoryFree	  "Usage: isHistoryFree module variable"
#define Usage_isModuleUpdate	"Usage: isModuleUpdate module"
#define Usage_updateModule	"Usage: updateModule module"
#define Usage_showAtoms		"Usage: show_atoms module"
#define Usage_showComponents	"Usage: show_components module"

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

static int MdlShowModuleCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static void MdlModulePrintVariables(Tcl_Interp *interp, char* name, Mdl_Manager_t* mdlmanager);
static void MdlModulePrintAtoms(Tcl_Interp *interp, char* name, Mdl_Manager_t* mdlmanager, int mode);
static void MdlModulePrintGuardedCommand(Tcl_Interp* interp, Atm_Cmd_t* cmd);
static int MdlSaveCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlDeleteCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlShowVariableCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlComposeCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlHideCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlRenameCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlLetCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlIsEventVariableCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlIsPrivateVariableCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlIsInterfaceVariableCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlIsExternalVariableCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlIsHistoryFreeCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlIsModuleUpdateCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlUpdateModuleCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlShowAtomsCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static int MdlTraverseCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
static void traverse(Mdl_Expr_t * mexpr);
static char* ReturnWhiteSpacesNeeded(int numSpaces);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Init function for tcl.]

  Description        [It creates all the new tcl commands defined in this
  package.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Mdl_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Main_ManagerSetModuleManager(manager, (Main_MdlManagerGeneric) Mdl_ModuleManagerAlloc());
  
  Tcl_CreateCommand(interp, "show_mdls", MdlShowModuleCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "save", MdlSaveCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "delete", MdlDeleteCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "show_vars", MdlShowVariableCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "compose", MdlComposeCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "hide", MdlHideCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "ren", MdlRenameCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "let", MdlLetCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "isEventVariable", MdlIsEventVariableCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "isPrivateVariable", MdlIsPrivateVariableCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "isInterfaceVariable", MdlIsInterfaceVariableCmd,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "isExternalVariable", MdlIsExternalVariableCmd,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "isHistoryFree", MdlIsHistoryFreeCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "isModuleUpdate", MdlIsModuleUpdateCmd,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "updateModule", MdlUpdateModuleCmd,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "show_atoms", MdlShowAtomsCmd,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
/*  Tcl_CreateCommand(interp, "show_components", _MdlShowComponents,
    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL); */
  Tcl_CreateCommand(interp, "traverse", MdlTraverseCmd,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Reinitializes the mdl package.]

  Description        [The function frees all modules in the module manager and
                      allocates a new module manager. It does the same for the
                      type manager.]

  SideEffects        [None]

  SeeAlso            [Mdl_Init, Mdl_End]

******************************************************************************/
int
Mdl_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager(manager);

  Mdl_ModuleManagerFree(mdlManager);
  mdlManager = ALLOC (Mdl_Manager_t, 1);
  mdlManager->nameToModule = st_init_table (strcmp, st_strhash);
  Main_ManagerSetModuleManager(manager, (Main_MdlManagerGeneric) mdlManager);
  
  MdlModuleCounterReset();
    
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [End the mdl package]

  Description        [The function frees all memory associated with the mdl
                      package.]

  SideEffects        [None]

  SeeAlso            [Mdl_Reinit, Mdl_Init]

******************************************************************************/
int
Mdl_End(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager(manager);

  Mdl_ModuleManagerFree(mdlManager);

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [It creates a module manager.]

  Description        [optional]

  SideEffects        [required]

******************************************************************************/
Mdl_Manager_t *
Mdl_ModuleManagerAlloc(
  void)
{
  Mdl_Manager_t *mdlManager = ALLOC (Mdl_Manager_t, 1);
  
  mdlManager->nameToModule = st_init_table (strcmp, st_strhash);
  return mdlManager;
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Command for show_mdls.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [show_mdls] 	   

  CommandSynopsis    [Show all the parsed modules in the current session.]  

  CommandArguments   [\[-haldg\] \[&lt;module_name&gt;\]]  

  CommandDescription [List all the parsed modules in the current session.

  Command Options:<p>

  <dl>
  <dt> -h
  <dd> Prints usage.

  <dt> -a
  <dd> List all the modules, including the internally generated ones (those
  whose module name is prefixed by an @.)

  <dt> -l
  <dd> Long list of the module descriptions.  Also list all the atoms and
  variables in the module.

  <dt> -d 
  <dd> Prints the components of the module. Some modules are defined by
  operations (namely, parallel composition, variable hiding or variable
  renaming) on existing modules. This option prints the composing modules.
  
  <dt> -g
  <dd> Debug option. For developer and package maintainer uses.

  <dt> &lt;module_name\gt;
  <dd> Prints information about the specified module only.

  </dl>]
  

******************************************************************************/
static int
MdlShowModuleCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  lsList list;
  lsGen lsgen;
  char* name;
  char c;
  Mdl_Module_t* mdl;
  
  boolean listall = FALSE;    /* print all, including internally
                                 created modules? */
  boolean listlong = FALSE;   /* long list? */
  boolean listmodule = FALSE; /* list particular module? */
  boolean listdebug = FALSE;  /* debug mode, print pre/postordering of atoms
                               */
  boolean listdescription = FALSE;

  Main_ClearResult();
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "aldgh")) != -1) {
    switch (c) {
        case 'a':
          listall = 1;
          break;
        case 'l':
          listlong = 1;
          break;
        case 'g':
          listdebug = 1;
          break;
        case 'd':
          listdescription = 1;
          break;
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_show_mdls);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_show_mdls);
          return TCL_ERROR;
          break;
    }
  }
  
  if (argc - util_optind != 0) {
    listmodule = 1;
    name = argv[util_optind];
  }
  
  if (listmodule) {
    mdl = Mdl_ModuleReadFromName(mdlManager, name);
    if (mdl == NIL(Mdl_Module_t)) {
      Main_AppendResult("Module %s not found\n", name);
      return TCL_ERROR;        
    }

    list = lsCreate();
    lsNewEnd(list, util_strsav(name), LS_NH);

  } else {
    list = Mdl_ModuleManagerObtainModuleList(mdlManager);
  }

  if (!list)  return TCL_ERROR;
  
  lsForEachItem(list, lsgen, name){
    if(listall || listmodule || *name != '@' ){
      if (listlong) {
        Main_AppendResult("Module %s\n", name);
        MdlModulePrintVariables(interp, name, mdlManager);
        MdlModulePrintAtoms(interp, name, mdlManager, listdebug);
        Main_AppendResult("endmodule\n\n");
      }
      else {
        Main_AppendResult("%-20s", name);
        mdl = Mdl_ModuleReadFromName(mdlManager, name);
        if (listdescription){
          char * exprString =
              MdlExprConvertToString(Mdl_ModuleReadModuleExpr(mdl));
          Main_AppendResult(exprString);
          FREE(exprString);
        }
        Main_AppendResult("\n");
      }
    }
  }
  
  lsDestroy(list, free);    
  
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [List all the variables in a module of a given name.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
MdlModulePrintVariables(
  Tcl_Interp *interp,
  char* name, 
  Mdl_Manager_t* mdlmanager)
{
  lsList varlist;
  Mdl_Module_t* module;
  int flag = 0;
  lsGen lsgen;
  Var_Variable_t* var;
  char *varname, *typeString;
  int numSpacesNeeded = 15;
  char *spaces15  = ReturnWhiteSpacesNeeded(numSpacesNeeded);
  char *spacesExt = ReturnWhiteSpacesNeeded(numSpacesNeeded -
                                            strlen("external:"));
  char *spacesInt = ReturnWhiteSpacesNeeded(numSpacesNeeded -
                                            strlen("interface:"));
  char *spacesPri = ReturnWhiteSpacesNeeded(numSpacesNeeded -
                                            strlen("private:"));


  if ((module = Mdl_ModuleReadFromName(mdlmanager,name)) != NIL(Mdl_Module_t)){
    varlist = Mdl_ModuleObtainVariableList(module);

    lsForEachItem(varlist, lsgen, var){
      varname = Var_VariableReadName(var);
      typeString = Var_TypeConvertToString(Var_VariableReadType(var));
      if(Var_VariableReadPEID(var) == 1){
        /* calculation of spaces */
        char *spaces =
            ReturnWhiteSpacesNeeded(numSpacesNeeded - strlen(varname));
        if (flag == 1)
          Main_AppendResult(";\n%s%s:%s%s",
                            spaces15, varname,
                            spaces, typeString);
        else {
          Main_AppendResult("external:%s%s:%s%s",
                            spacesExt, varname,
                            spaces, typeString);
        }
        FREE(spaces);
        flag = 1;
      }
      FREE(typeString);
    }

    if (flag) Main_AppendResult("\n");

    flag = 0;
    lsForEachItem(varlist, lsgen, var){
      varname = (Var_VariableReadName(var));
      typeString = Var_TypeConvertToString(Var_VariableReadType(var));
      if(Var_VariableReadPEID(var) == 2){
        char *spaces =
            ReturnWhiteSpacesNeeded(numSpacesNeeded - strlen(varname));
        if (flag == 1)
          Main_AppendResult(";\n%s%s:%s%s",
                            spaces15, varname,
                            spaces, typeString);
        else {
          Main_AppendResult("interface:%s%s:%s%s",
                            spacesInt, varname,
                            spaces, typeString);
        }
        FREE(spaces);
	flag = 1;
      }
      FREE(typeString);
    }

    if (flag) Main_AppendResult("\n");

    flag = 0;
    lsForEachItem(varlist, lsgen, var){
      varname = (Var_VariableReadName(var));
      typeString = Var_TypeConvertToString(Var_VariableReadType(var));
      if(Var_VariableReadPEID(var) == 0){
        char *spaces =
            ReturnWhiteSpacesNeeded(numSpacesNeeded - strlen(varname));
        if (flag == 1)
          Main_AppendResult(";\n%s%s:%s%s",
                            spaces15, varname,
                            spaces, typeString);
        else {
          Main_AppendResult("private:%s%s:%s%s",
                            spacesPri, varname,
                            spaces, typeString);
        }
	flag = 1;
        FREE(spaces);
      }
      FREE(typeString);
    }
    
    if (flag) Main_AppendResult("\n");

    Main_AppendResult("\n");
    lsDestroy(varlist, (void (*) ()) 0);
  }

  FREE (spaces15);
  FREE (spacesExt);
  FREE (spacesInt);
  FREE (spacesPri);

}


/**Function********************************************************************

  Synopsis           [List all the atoms in a module of a given name.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
MdlModulePrintAtoms(
  Tcl_Interp *interp,
  char* name, 
  Mdl_Manager_t* mdlmanager,
  int mode)
{
  lsList atmlist;
  lsList varlist;
  lsList cmdlist;
  Mdl_Module_t* module;
  int flag = 0;
  int flag1 = 0;
  lsGen lsgen;
  lsGen lsgen1;
  Atm_Atom_t* atm;
  Var_Variable_t* var;
  Atm_Cmd_t* cmd;
  char* atmname;

  if ((module = Mdl_ModuleReadFromName(mdlmanager, name)) != NIL(Mdl_Module_t)){

    atmlist = Mdl_ModuleObtainSortedAtomList(module);
    lsForEachItem(atmlist, lsgen, atm){
      Atm_AtomType atype;
      atmname =  Atm_AtomReadName(atm);
      atype = Atm_AtomReadAtomType(atm);

      if (mode) {
        Main_AppendResult("(%d, %d)", Atm_AtomReadPreorder(atm),
                          Atm_AtomReadPostorder(atm));
      }

      if (atype == Atm_Event_c){
	Main_AppendResult("  event atom %s\n", atmname); 
      }
      else if (atype == Atm_Lazy_c){
	Main_AppendResult("  lazy atom %s\n", atmname);
      }
      else {
	Main_AppendResult("  atom %s\n", atmname);
      }
      
      /* print the list of control variables */
      flag1 = 0;
      varlist = Atm_AtomReadControlVarList(atm);
      lsForEachItem(varlist, lsgen1, var){
        if (flag1==1)
          Main_AppendResult(", %s", Var_VariableReadName(var));
        else
          Main_AppendResult("    controls %s", Var_VariableReadName(var));
	flag1 =1 ;
      }
      Main_AppendResult("\n");
	
      /* print the list of read variables */
      flag1 = 0;
      varlist = Atm_AtomReadReadVarList(atm);
      lsForEachItem(varlist, lsgen1, var){
        if (flag1==1)
          Main_AppendResult(", %s", Var_VariableReadName(var));
        else
          Main_AppendResult("    reads %s", Var_VariableReadName(var));
	flag1 =1 ;
      }
      if (flag1) Main_AppendResult("\n");


      /* print the list of await variables */
      flag1 = 0;
      varlist = Atm_AtomReadAwaitVarList(atm);

      flag = 1;
      lsForEachItem(varlist, lsgen1, var){
        if (flag1==1)
          Main_AppendResult(", %s", Var_VariableReadName(var));
        else
          Main_AppendResult("    awaits %s", Var_VariableReadName(var));
	flag1 =1 ;
      }
      if (flag1) Main_AppendResult("\n");

      /* print the init commands */
      cmdlist = Atm_AtomReadInitCommandList(atm);
      if (lsLength(cmdlist) != 0){
        Main_AppendResult("      init\n");
        lsForEachItem(cmdlist, lsgen1, cmd){
          MdlModulePrintGuardedCommand(interp, cmd);
          Main_AppendResult("\n");
        }
      }

      /* this is to print the default init commands */
      if (cmd = Atm_AtomReadDefaultInitCommand(atm)){
        MdlModulePrintGuardedCommand(interp, cmd);
        Main_AppendResult("\n");
      }
      
      /* print the update commands */
      Main_AppendResult("      update\n");
      cmdlist = Atm_AtomReadUpdateCommandList(atm);
      lsForEachItem(cmdlist, lsgen1, cmd){
        MdlModulePrintGuardedCommand(interp, cmd);
        Main_AppendResult("\n");
      }

      /* this is to print the default update commands */
      if (cmd = Atm_AtomReadDefaultUpdateCommand(atm)){
        MdlModulePrintGuardedCommand(interp, cmd);
        Main_AppendResult("\n");
      }
      
      Main_AppendResult("  endatom\n\n");
    }
    
    if (flag == 0)
      Main_AppendResult("    No Atoms defined. \n");
    lsDestroy(atmlist, (void (*) ()) 0);
  }
    
}


/**Function********************************************************************

  Synopsis           [Print one guarded assignment]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void 
MdlModulePrintGuardedCommand(
  Tcl_Interp* interp,
  Atm_Cmd_t* cmd
)
{
  Atm_Assign_t* assign;
  lsList assignList;
  lsGen lsgen;
  Var_Variable_t* var;
  
  assignList = Atm_CmdReadAssignList(cmd);

  Main_AppendResult("        [] ");
  {
    Atm_Expr_t * guard = Atm_CmdReadGuard(cmd);
    if (guard) 
      Atm_ExprPrintExpression(Atm_CmdReadGuard(cmd));
    else
      Main_AppendResult("default");
  }

  Main_AppendResult(" -> ");

  
  lsForEachItem(assignList, lsgen, assign){

    var = Atm_AssignReadVariable(assign);
    
    switch (Atm_AssignReadType(assign)) {
        case Atm_AssignEvent_c :
          Main_AppendResult("%s!;", Var_VariableReadName(var));
          break;
          
        case Atm_AssignStd_c :
          Main_AppendResult("%s':=", Var_VariableReadName(var));
          Atm_ExprPrintExpression(Atm_AssignReadExpr(assign));
          Main_AppendResult(";");
          break;
          
        case Atm_AssignIndex_c :
          Main_AppendResult("%s'[", Var_VariableReadName(var));
          Atm_ExprPrintExpression(Atm_AssignReadIndexExpr(assign));
          Main_AppendResult("]:=");
          Atm_ExprPrintExpression(Atm_AssignReadExpr(assign));
          Main_AppendResult(";");
          break;
          
        case Atm_AssignForall_c :
          Main_AppendResult("forall %s %s[%s]:=",
                            Var_VariableReadName(Atm_AssignReadIndexVar(assign)),
                            Var_VariableReadName(var), 
                            Var_VariableReadName(Atm_AssignReadIndexVar(assign)));
          Atm_ExprPrintExpression(Atm_AssignReadExpr(assign));
          Main_AppendResult(";");
          break;
    }
  }
}


/**Function********************************************************************

  Synopsis           [Saves the modules in an interactive session]

  Description        [Not implemented yet.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
MdlSaveCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  Main_MochaPrint("write_to_file not implemented yet\n");

/*  int shidden = 0;
  int snamemdl = 0;
  int skept = 0;
  char* outfile="";
  lsList mlist = lsCreate();
  int i=1;
  FILE* outf;

  util_opt_reset();
  while ((c=util_getopt(argc, argv, "ao:")) != -1) {
    switch (c) {
    case 'a':
      save_all = 1;
      break;
    case 'n':
      save_file = 1;
      filename = optarg;      
      break;
    default:
      Main_MochaErrorPrint("Usage: write_to_file [-a] [-o filename] [modules]\n");
      return TCL_OK;
      break;
    }
  }

  if (argc-optind != 0) { /* module names given 
    save_modules = 1;
  }

  if (save_file){ /* user defined file 
    if (util_file_search(filename, ".", "r")) {
      printf("File %s exist. Overwrite (y/n)?", filename);
      getc (c);
      if ((c!='y') || (c!='Y')) {
        return TCL_OK;
      }        
    }
    if ((fd=fopen(filename, "w") == -1)){
      Main_MochaErrorPrint("Error: File %s cannot be opened.\n");
      return TCL_ERROR;
    }
    else {
      mocha_out = fd;
    }
    
  */    
      
  return TCL_OK;
  
}

/**Function********************************************************************

  Synopsis           [Delete a module.]

  Description        [not implemented yet.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
MdlDeleteCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  /*
  Mdl_Module_t* module;
  Main_Manager_t *manager = (Main_Manager_t *) clientData;
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
    Main_ManagerReadModuleManager(manager); 
  int i = 1;
  int j = 0;
  
  if (argc < 2) {
    Main_MochaErrorPrint(Usage_delete);
    return TCL_ERROR;
  }
  else {
    while (i < argc) {
       if(Mdl_ModuleRemoveFromManager(argv[i], mdlManager))
	j++;
      }
      else {
	Main_MochaErrorPrint("Warning: Module %s not exist.\n", argv[i]);
      }
      i++;
    }
    Main_MochaErrorPrint("%d modules deleted.\n", j);

    return TCL_OK;
  
  }
  */
  Main_MochaPrint("command not implemented\n");
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Show all information about all variables in a module]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [show_vars] 	   

  CommandSynopsis    [shows the variables in side the given module.]  

  CommandArguments   [\[-vHF|HD|EV|ALL\] module]  

  CommandDescription [It prints out the variables in the given module.
  Without option, it prints a list of roughly formatted list of the variables
  of module.

  Command Options:<p>

  <dl>

  <dt> -vHF
  <dd> It returns a list of all the history free variables inside the module.

  <dt> -vHD
  <dd> It returns a list of all the history dependent variables inside the
  module.

  <dt> -vEV
  <dd> It returns a list of all the event variables inside the module.

  <dt> -vALL
  <dd> It returns a list of all variables inside the module.

  </dl>
  ]  

******************************************************************************/
static int
MdlShowVariableCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  Mdl_Module_t* module;
  Main_Manager_t *manager = (Main_Manager_t *) clientData;
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
    Main_ManagerReadModuleManager(manager); 
  st_table* vartable;
  st_generator* gen;
  char* key;
  Var_Variable_t* var;
  lsList atmlist;
  Atm_Atom_t* atm;
  lsGen lsgen;
  int flag;
  char* Tcl_Empty;
  char c;
  char* name;
  boolean opt_default = FALSE; /* default option */
  boolean opt_g = FALSE; /* debug mode -- all information are printed */
  boolean opt_va = FALSE;
  boolean opt_ve = FALSE;
  boolean opt_vhd = FALSE;
  boolean opt_vhf = FALSE;
  lsList varlist;
  
  Main_ClearResult();
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "hgv:")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_show_vars);
          return TCL_OK;
          break;
        case 'g':
          opt_g = TRUE;
          break;
        case 'v':
          if (!strcmp(util_optarg, "ALL")) {
            opt_va = TRUE;
          }
          else if (!strcmp(util_optarg, "EV")) {
            opt_ve = TRUE;
          }
          else if (!strcmp(util_optarg, "HF")) {
            opt_vhf = TRUE;
          }
          else if (!strcmp(util_optarg, "HD")) {
            opt_vhd = TRUE;
          }
          else {
            Main_MochaErrorPrint("%s\n", Usage_show_vars);
            return TCL_ERROR;
          }
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_show_vars);
          return TCL_ERROR;
          break;
    }
  }

  if (argc - util_optind != 0) {
    name = argv[util_optind];
  }
  else {
    Main_MochaErrorPrint("%s\n", Usage_show_vars);
    return TCL_ERROR;
  }
  
  if ((module = Mdl_ModuleReadFromName(mdlManager, name)) == NIL(Mdl_Module_t)){
    Main_MochaErrorPrint("Module %s not defined.\n", name);
    return TCL_ERROR;
  }
  
  if (opt_g) {
    vartable = Mdl_ModuleReadVariableTable(module);
    st_foreach_item(vartable, gen, &key, (char**) &var) {
      if (module != (Mdl_Module_t*) Var_VariableReadModule(var)) {
        Main_MochaErrorPrint("Error!! Module Name Mismatch\n");
        return TCL_ERROR;
      }
      
      flag = 0;
      
      Main_MochaErrorPrint("name:\t\t%s\n", Var_VariableReadName(var));
      Main_MochaErrorPrint("type:\t\t%s\n",
             Var_VarTypeReadName(Var_VariableReadType(var)));
      Main_MochaErrorPrint("peid:\t\t%d\n", Var_VariableReadPEID(var));
      if ((atm=(Atm_Atom_t*) Var_VariableReadControlAtom(var)) != NIL(Atm_Atom_t)) 
        Main_MochaErrorPrint("CtrlBy:\t\t%s\n", Atm_AtomReadName(atm));
      else
        Main_MochaErrorPrint("CtrlBy:\t\tnone\n");
      
      Main_MochaErrorPrint("ReadBy:");
      atmlist = Var_VariableReadReadList(var);
      lsForEachItem(atmlist, lsgen, atm) {
        Main_MochaErrorPrint("\t\t%s\n", Atm_AtomReadName(atm));
      }
      
      Main_MochaErrorPrint("AwaitBy:");
      atmlist = Var_VariableReadAwaitList(var);
      lsForEachItem(atmlist, lsgen, atm) {
        if (flag)
          Main_MochaErrorPrint("\t%s\n", Atm_AtomReadName(atm));
        else
          Main_MochaErrorPrint("\t\t%s\n", Atm_AtomReadName(atm));
        flag = 1;
      }
      Main_MochaErrorPrint("---\n");
    }
    return TCL_OK;
  }
  else if (opt_va)
    varlist = Mdl_ModuleObtainVariableList(module);
  else if (opt_ve)
    varlist = Mdl_ModuleObtainEventVariableList(module);
  else if (opt_vhd)
    varlist = Mdl_ModuleObtainHistoryDependentVariableList(module);
  else if (opt_vhf)
    varlist = Mdl_ModuleObtainHistoryFreeVariableList(module);
  else {
    /* default option */
    opt_default = TRUE;
    varlist = Mdl_ModuleObtainVariableList(module);
  }

  if (!opt_default) {
    lsForEachItem(varlist, lsgen, var) { 
      Main_AppendResult("%s\n", Var_VariableReadName(var));
    }
  }
  else {
    Main_AppendResult("\nexternal\n");
    Main_AppendResult("--------\n");
    lsForEachItem(varlist, lsgen, var) {
      if (Var_VariableReadPEID(var) == 1){ /* external */
        Main_AppendResult("%s\n",Var_VariableReadName(var));
      }
    }
    Main_AppendResult("\ninterface\n");
    Main_AppendResult("---------\n");
    lsForEachItem(varlist, lsgen, var) {
      if (Var_VariableReadPEID(var) == 2){ /* interface */
        Main_AppendResult("%s\n",Var_VariableReadName(var));
      }
    }
    Main_AppendResult("\nprivate\n");
    Main_AppendResult("-------\n");
    lsForEachItem(varlist, lsgen, var) {
      if (Var_VariableReadPEID(var) == 0){ /* private */
        Main_AppendResult("%s\n", Var_VariableReadName(var));
      }
    }
  }

  lsDestroy(varlist, (void (*) ()) 0);
  
  return TCL_OK;
  
}

/**Function********************************************************************

  Synopsis           [Compose a module on the command line]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [compose] 	   

  CommandSynopsis    [parallel composition of two modules.]  

  CommandArguments   [\[-h\] \lt;module_name\gt; \lt;module_name\gt;]  

  CommandDescription [This function creates a new module which is the
  parallel composition of the given modules. It returns the name
  of the newly composed module.

  <p>
  The name of the new module is generated internally. To designate
  a name, use let.  For example, the command

  <p>
  let P be \[compose P1 P2\]

  <p>
  creates a module P which is the parallel composition of P1 and P2.

  <p>
  Command Options:<p>

  <dl>

  <dt> -h
  <dd> Prints usage.

  </dl>

  ]  

******************************************************************************/
static int
MdlComposeCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  Mdl_Module_t* mod1;
  Mdl_Module_t* mod2;
  Mdl_Module_t* mod;
  char c;
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_compose);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_compose);
          return TCL_OK;
          break;
    }
  }

  if (argc != 3) {
    Main_MochaErrorPrint("%s", Usage_compose);
    return TCL_ERROR;
  }

  mod1 = Mdl_ModuleReadFromName(mdlManager, argv[1]);
  mod2 = Mdl_ModuleReadFromName(mdlManager, argv[2]);
  
  if (mod1 == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s not defined.\n", argv[1]);
    return TCL_ERROR;
  }
  else if (mod2 == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s not defined.\n", argv[2]);
    return TCL_ERROR;
  }
  else {
    Mdl_Expr_t * mdlExpr;
    char * name;

    mdlExpr = Mdl_ExprComposeAlloc (Mdl_ExprModuleDupAlloc(argv[1]),
                                    Mdl_ExprModuleDupAlloc(argv[2]));
    
    name = Mdl_ModuleObtainNewName();
    
    mod = Mdl_ModuleExprEvaluate(name, mdlExpr, mdlManager);
    /* only free the name, not the expression, because it
       will be stored in the new module */
    FREE(name);
    
    
    if (mod) {
      if (!Mdl_ModuleTestForAcyclicity(mod)){
        Main_MochaErrorPrint("Cycle detected in parallel composition.\n");
        Mdl_ModuleFree(mod);
        Mdl_ExprFree(mdlExpr);
        return TCL_ERROR;
      }
      else {
        Mdl_ModuleAddToManager(mod, mdlManager);
        Main_AppendResult("%s", Mdl_ModuleReadName(mod));
        return TCL_OK;
      }
    } else {
      return TCL_ERROR;
    }
  }
  
}


/**Function********************************************************************

  Synopsis           [Hide a list of variables on the command line]

  Description        [This function implements the user command hide.]

  SideEffects        []

  SeeAlso            [optional]

  CommandName        [hide] 	   

  CommandSynopsis    [hides the interface variables of the module]  

  CommandArguments   [\lt;variable_name\gt; \lt;variable_name\gt; ...   Module]  

  CommandDescription [This function creates a new module which
  hides the given variables of the specified module. It returns
  the name of the new module.

  <p>
  The name of the new module is generated internally. To designate
  a name, use let.  For example, the command

  <p>
  let P be \[hide x1 x2 Pete\]

  <p>
  creates a module P which has x1 and x2 hidden.

  <p>
  Command Options:<p>

  <dl>

  <dt> -h
  <dd> Prints usage.

  </dl>

  ]  

******************************************************************************/

static int
MdlHideCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  Mdl_Module_t* mod1;
  Mdl_Module_t* mod;
  int i;
  lsList varlist;
  char c;
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);

  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_hide);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_hide);
          return TCL_ERROR;
          break;
    }
  }

  
  if (argc < 2) {
    Main_MochaErrorPrint("%s", Usage_hide);
    return TCL_ERROR;
  }

  mod1 = Mdl_ModuleReadFromName(mdlManager, argv[argc-1]);
  
  if (mod1 == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s not defined.\n", argv[1]);
    return TCL_ERROR;
  }
  else {
    Mdl_Expr_t *mexpr;
    char * name;
    
    varlist = lsCreate();
    
    for (i=1; i < argc-1; i++) {
      lsNewEnd(varlist, argv[i], LS_NH);
    }
    
    mexpr = Mdl_ExprHideAlloc(Mdl_ExprModuleDupAlloc(argv[argc-1]),
                              varlist);
    name  = Mdl_ModuleObtainNewName();

    mod   = Mdl_ModuleExprEvaluate(name, mexpr, mdlManager);
    FREE(name);
    
    if (mod) {
      Mdl_ModuleAddToManager(mod, mdlManager);
      Main_AppendResult("%s", Mdl_ModuleReadName(mod));
      return TCL_OK;
    }
    else {
      Main_MochaErrorPrint("Error in hiding variables.\n");
      Mdl_ExprFree(mexpr);
      return TCL_ERROR;
    }
  }
}



/**Function********************************************************************

  Synopsis           [Rename a variable of  a module on the command line]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [ren] 	   

  CommandSynopsis    [renames an observable variable of a module]  

  CommandArguments   [from_var to_var Module]  

  CommandDescription [This function renames an observable (interface
  of external) variable of a module. It returns
  the name of the new module.

  <p>
  The name of the new module is generated internally. To designate
  a name, use let.  For example, the command

  <p>
  let P be \[ren x1 y1 Pete\]

  <p>
  creates a module P which has x1 renamed to y1.
]  

******************************************************************************/
static int
MdlRenameCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  Mdl_Module_t* mod1;
  Mdl_Module_t* mod;
  st_table* vartable;
  char c;
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_rename);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_rename);
          return TCL_ERROR;
          break;
    }
  }

  if (argc != 4) {
    Main_MochaErrorPrint("%s", Usage_rename);
    return TCL_ERROR;
  }

  mod1 = Mdl_ModuleReadFromName(mdlManager, argv[3]);
  
  if (mod1 == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s not defined.\n", argv[3]);
    return TCL_ERROR;
  }
  else {
    Mdl_Expr_t * mexpr;
    char * name;
    lsList varlist1 = lsCreate();
    lsList varlist2 = lsCreate();

    lsNewEnd(varlist1, util_strsav(argv[1]), LS_NH);
    lsNewEnd(varlist2, util_strsav(argv[2]), LS_NH);

    mexpr = Mdl_ExprRenameAlloc(Mdl_ExprModuleDupAlloc(argv[3]),
                             varlist1,
                             varlist2);
    
    name  = Mdl_ModuleObtainNewName();
    
    mod   = Mdl_ModuleExprEvaluate(name, mexpr, mdlManager);
    FREE(name);

    if (mod) {
      Mdl_ModuleAddToManager(mod, mdlManager);
      Main_AppendResult("%s", Mdl_ModuleReadName(mod));
      return TCL_OK;
    }
    else {
      Mdl_ExprFree(mexpr);
      Main_MochaErrorPrint("Error in renaming variables.\n");
      return TCL_ERROR;
    }
  }
}


/**Function********************************************************************

  Synopsis           [Give a name to a module.]

  Description        [Calling let will give made a copy of the module with the
                      name given on the command line.  The module will be
                      hashed into the module manager whereas using the TCL
                      command set will only attach a name to the module
                      without actually creating a new module.]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [let]

  CommandSynopsis    [creates an exact copy of a module with the given name]  

  CommandArguments   [new_module be module]  

  CommandDescription [This function creates an exact copy of a module with
  the given name. It is usually used in conjuction with compose, hide and
  rename.]  

******************************************************************************/
static int
MdlLetCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  Mdl_Module_t* mod;
  st_table* vartable;
  char c;
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);

  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_let);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_let);
          return TCL_ERROR;
          break;
    }
  }

  if (argc != 4) {
    Main_MochaErrorPrint("%s", Usage_let);
    return TCL_ERROR;
  }

  if (strcmp(argv[2],"be")) {
    Main_MochaErrorPrint("%s", Usage_let);
    return TCL_ERROR;
  }

  if (!Mdl_ModuleReadFromName(mdlManager, argv[3])) {
    Main_MochaErrorPrint("Module %s is not defined.\n", argv[3]);
    return TCL_ERROR;
  }
  else {
    mod   = Mdl_ModuleExprEvaluate(argv[1],
                                   Mdl_ExprModuleDupAlloc(argv[3]),
                                   mdlManager);
    

    Mdl_ModuleAddToManager(mod, mdlManager);
    Main_AppendResult("%s", Mdl_ModuleReadName(mod));
    return TCL_OK;
  }
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [isEventVariable] 	   

  CommandSynopsis    [Test if a variable is an event variable]  

  CommandArguments   [variable module]  

  CommandDescription [This function tests if a variable is a event
  variable of the module. If yes, 1 is returned. Otherwise, 0
  is returned.]  

******************************************************************************/
static int
MdlIsEventVariableCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  Mdl_Module_t *mod;
  Var_Variable_t *var;
  char c;
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);

  Main_ClearResult();
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_isEventVariable);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_isEventVariable);
          return TCL_ERROR;
          break;
    }
  }

  
  if (argc != 3) {
    Main_MochaErrorPrint("%s", Usage_isEventVariable);
    return TCL_ERROR;
  }
  
  mod = Mdl_ModuleReadFromName(mdlManager, argv[1]);
  if (mod == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s is not defined.\n", argv[1]);
    return TCL_ERROR;
  }
  
  var = Mdl_ModuleReadVariableFromName(argv[2], mod);
  if (var == NIL(Var_Variable_t)) {
    Main_MochaErrorPrint("Variable %s is not defined in module %s.\n",
                         argv[2], argv[1]);
  }
  

  if (Var_VariableIsEvent(var) == TRUE) {
    Main_AppendResult("1");
  } else {
    Main_AppendResult("0");
  }

  return TCL_OK;
  
}


/**Function********************************************************************

  Synopsis           [Command line function that test a private variable]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [isPrivateVariable] 	   

  CommandSynopsis    [Test if a variable is a private variable]  

  CommandArguments   [variable module]  

  CommandDescription [This function tests if a variable is a private
  variable of the module. If yes, 1 is returned. Otherwise, 0
  is returned.]  

******************************************************************************/
static int
MdlIsPrivateVariableCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  Mdl_Module_t *mod;
  Var_Variable_t *var;
  char c;
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);

  Main_ClearResult();

  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_isPrivateVariable);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_isPrivateVariable);
          return TCL_ERROR;
          break;
    }
  }
  
  if (argc != 3) {
    Main_MochaErrorPrint("%s\n", Usage_isPrivateVariable);
    return TCL_ERROR;
  }
  
  mod = Mdl_ModuleReadFromName(mdlManager, argv[1]);
  if (mod == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s is not defined.\n", argv[1]);
    return TCL_ERROR;
  }
  
  var = Mdl_ModuleReadVariableFromName(argv[2], mod);
  if (var == NIL(Var_Variable_t)) {
    Main_MochaErrorPrint("Variable %s is not defined in module %s.\n",
                         argv[2], argv[1]);
    return TCL_ERROR;
  }
  
  if (Var_VariableIsPrivate(var) == TRUE) {
    Main_AppendResult("1");
  } else {
    Main_AppendResult("0");
  }

  return TCL_OK;
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [isInterfaceVariable] 	   

  CommandSynopsis    [Test if a variable is a interface variable]  

  CommandArguments   [variable module]  

  CommandDescription [This function tests if a variable is a interface
  variable of the module. If yes, 1 is returned. Otherwise, 0
  is returned.]  

******************************************************************************/
static int
MdlIsInterfaceVariableCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  Mdl_Module_t *mod;
  Var_Variable_t *var;
  char c;
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);

  Main_ClearResult();
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_isInterfaceVariable);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_isInterfaceVariable);
          return TCL_ERROR;
          break;
    }
  }
  
  if (argc != 3) {
    Main_MochaErrorPrint("%s\n", Usage_isInterfaceVariable);
    return TCL_ERROR;
  }
  
  mod = Mdl_ModuleReadFromName(mdlManager, argv[1]);
  if (mod == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s is not defined.\n", argv[1]);
    return TCL_ERROR;
  }
  
  var = Mdl_ModuleReadVariableFromName(argv[2], mod);
  if (var == NIL(Var_Variable_t)) {
    Main_MochaErrorPrint("Variable %s is not defined in module %s.\n",
                         argv[2], argv[1]);
    return TCL_ERROR;
  }
  
  if (Var_VariableIsInterface(var) == TRUE) {
    Main_AppendResult("1");
  } else {
    Main_AppendResult("0");
  }

  return TCL_OK;
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [isExternalVariable] 	   

  CommandSynopsis    [Test if a variable is a external variable]  

  CommandArguments   [variable module]  

  CommandDescription [This function tests if a variable is a external
  variable of the module. If yes, 1 is returned. Otherwise, 0
  is returned.]  

******************************************************************************/
static int
MdlIsExternalVariableCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  Mdl_Module_t *mod;
  Var_Variable_t *var;
  char c;
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);

  Main_ClearResult();
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_isExternalVariable);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_isExternalVariable);
          return TCL_ERROR;
          break;
    }
  }

  if (argc != 3) {
    Main_MochaErrorPrint("%s\n", Usage_isExternalVariable);
    return TCL_ERROR;
  }
  
  mod = Mdl_ModuleReadFromName(mdlManager, argv[1]);
  if (mod == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s is not defined.\n", argv[1]);
    return TCL_ERROR;
  }
  
  var = Mdl_ModuleReadVariableFromName(argv[2], mod);
  if (var == NIL(Var_Variable_t)) {
    Main_MochaErrorPrint("Variable %s is not defined in module %s.\n",
                         argv[2], argv[1]);
    return TCL_ERROR;
  }
  
  if (Var_VariableIsExternal(var) == TRUE) {
    Main_AppendResult("1");
  } else {
    Main_AppendResult("0");
  }

  return TCL_OK;
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [isHistoryFreeVariable] 	   

  CommandSynopsis    [Test if a variable is a history-free variable]  

  CommandArguments   [variable module]  

  CommandDescription [This function tests if a variable is a history-free
  variable of the module. If yes, 1 is returned. Otherwise, 0
  is returned.]  

******************************************************************************/
static int
MdlIsHistoryFreeCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  Mdl_Module_t *mod;
  Var_Variable_t *var;
  char c;
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);

  Main_ClearResult();

  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_isHistoryFree);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_isHistoryFree);
          return TCL_ERROR;
          break;
    }
  }
  
  if (argc != 3) {
    Main_MochaErrorPrint("%s", Usage_isHistoryFree);
    return TCL_ERROR;
  }
  
  mod = Mdl_ModuleReadFromName(mdlManager, argv[1]);
  if (mod == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s is not defined.\n", argv[1]);
    return TCL_ERROR;
  }
  
  var = Mdl_ModuleReadVariableFromName(argv[2], mod);
  if (var == NIL(Var_Variable_t)) {
    Main_MochaErrorPrint("Variable %s is not defined in module %s.\n",
                         argv[2], argv[1]);
    return TCL_ERROR;
  }
  
  if (Var_VariableIsHistoryFree(var) == TRUE) {
    Main_AppendResult("1");
  } else {
    Main_AppendResult("0");
  }

  return TCL_OK;
  
}


/**Function********************************************************************

  Synopsis           [isModuleUpdate]

  Description        [disabled for the present release.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
MdlIsModuleUpdateCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  
  /*
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Mdl_Module_t* mod;
  
  Main_ClearResult();
  
  if (argc != 2) {
    Main_MochaErrorPrint("%s", Usage_isModuleUpdate);
    return TCL_ERROR;
  }
  
  mod = Mdl_ModuleReadFromName(mdlManager, argv[1]);
  if (mod == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s is not defined.\n", argv[1]);
    return TCL_ERROR;
  }

  if (Mdl_ModuleIsUpdate(mod, mdlManager) == TRUE) {
    Main_AppendResult("1");
  } else {
    Main_AppendResult("0");
  }

  */
  return TCL_OK;
  
}


/**Function********************************************************************

  Synopsis           [updateModule]

  Description        [disabled for the present release.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
MdlUpdateModuleCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{

  /*
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Mdl_Module_t* mod;
  
  Main_ClearResult();
  
  if (argc != 2) {
    Main_MochaErrorPrint("%s", Usage_updateModule);
    return TCL_ERROR;
  }
  
  mod = Mdl_ModuleReadFromName(mdlManager, argv[1]);
  if (mod == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s is not defined.\n", argv[1]);
    return TCL_ERROR;
  }

  if (Mdl_ModuleUpdate(mod, mdlManager) != NIL(Mdl_Module_t)) {
    Main_AppendResult("1");
  } else {
    Main_AppendResult("0");
  }

  */
  return TCL_OK;
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [show_atoms] 	   

  CommandSynopsis    [List all the atoms of the module.]  

  CommandArguments   [\[-h] \lt;module_name\lg;]
  
  CommandDescription [This command lists all the atoms of the given module.

  <p>
  Command Options:<p>

  <dl>

  <dt> -h
  <dd> Prints usage.

  </dl>
]

******************************************************************************/
static int
MdlShowAtomsCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Mdl_Module_t* mod;
  lsList atmlist;
  lsGen gen;
  Atm_Atom_t * atm;
  char c;
  
  Main_ClearResult();
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "h")) != -1) {
    switch (c) {
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_showAtoms);
          return TCL_OK;
          break;
        default:
          Main_MochaErrorPrint("%s\n", Usage_showAtoms);
          return TCL_ERROR;
          break;
    }
  }
  

  if (argc != 2) {
    Main_MochaErrorPrint("%s", Usage_showAtoms);
    return TCL_ERROR;
  }
  
  mod = Mdl_ModuleReadFromName(mdlManager, argv[1]);
  if (mod == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s is not defined.\n", argv[1]);
    return TCL_ERROR;
  }

  atmlist = Mdl_ModuleObtainSortedAtomList(mod);
  lsForEachItem(atmlist, gen, atm){
    Main_AppendElement("%s", Atm_AtomReadName(atm));
  }

  lsDestroy(atmlist, (void (*) ()) 0);

  return TCL_OK;
  
}


/**Function********************************************************************

  Synopsis           [Command that traverses the module expression tree.]

  Description        [This is a command for debugging. This command is called
  with the name of a module. It traverses the module expression tree and
  prints the leaf modules/atoms. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
MdlTraverseCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv) 
{
  
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Mdl_Module_t* mod;
  lsList atmlist;
  lsGen gen;
  Atm_Atom_t * atm;
  
  Main_ClearResult();
  
  if (argc != 2) {
    return TCL_ERROR;
  }
  
  mod = Mdl_ModuleReadFromName(mdlManager, argv[1]);
  if (mod == NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint("Module %s is not defined.\n", argv[1]);
    return TCL_ERROR;
  }
  
  traverse(mod->moduleExpr);
  
  return TCL_OK;
  
}

/**Function********************************************************************
   
  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
traverse(Mdl_Expr_t * mexpr)
{
  Mdl_ExprType type;
  
  type = Mdl_ExprReadModuleExprType(mexpr);
  
  switch (type) {
      case Mdl_ExprModuleDup_c:
        printf("module = %s\n", Mdl_ExprReadLeftChild(mexpr));
        traverse(Mdl_ExprReadRightChild(mexpr));
        break;
        
      case Mdl_ExprRename_c:
      case Mdl_ExprHide_c:
        traverse(Mdl_ExprReadLeftChild(mexpr));
        break;

      case Mdl_ExprCompose_c:
        traverse(Mdl_ExprReadLeftChild(mexpr));
        traverse(Mdl_ExprReadRightChild(mexpr));
        break;
        
      case Mdl_ExprModuleDef_c: 
      {
        lsGen gen;
        Atm_Atom_t * atom;
        lsList atomList = Mdl_ExprReadList1(mexpr);

        lsForEachItem(atomList, gen, atom){
          printf("atom = %s\n", Atm_AtomReadName(atom));
        }
        break;
      }

      default:
        fprintf(stderr, "Unexpected Mdl_ExprType in MdlExprSearchForComponent");
        break;
  }
  
}

/**Function********************************************************************

  Synopsis           [Return a string of spaces.]

  Description        [optional]

  SideEffects        [The user should free the string after use.]

  SeeAlso            [optional]

******************************************************************************/
static char*
ReturnWhiteSpacesNeeded(
  int numSpaces)
{
  char *spaces;
  int i;
  
  if (numSpaces < 0) {
    return util_strsav("");
  }

  spaces = ALLOC(char, numSpaces+1);

  for (i=0; i<numSpaces; i++) {
    *(spaces+i) = 32; /* 32 is the ascii code for space */
  }

  *(spaces+numSpaces) = '\0';

  return spaces;

}

