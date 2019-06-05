/**CFile***********************************************************************

  FileName    [main.c]

  PackageName [main]

  Synopsis    [Initialization and utility routines]

  Description [This file contains routines that initializes different
  packages and Tcl/Tk. It also contains utility routines for printing and
  error reporting.]

  SeeAlso     []

  Author      [Freddy Mang, Shaz Qadeer]

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

#include "mainInt.h" 
#include <sys/types.h>
#include <unistd.h>

extern int matherr();
int *tclDummyMathPtr = (int *) matherr;

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
static char * MainMochaSrcDir;
static char * MainMochaHelpDir;
static char * MainMochaTmpDir;
Tcl_Interp* mainInterp;
int MainTkEnabled = 0;
int MainCreationTime=0;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int MochaReinitialize(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int MochaEnd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
static int MainInitMochaPackages(Tcl_Interp *interp, Main_Manager_t *manager);
static void MainTclGlobalVariablesInitialize(Tcl_Interp* interp);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Initializes the main package.]

  SideEffects        [Registers commands by modifying the TCL variable
                      mocha_commands.]

******************************************************************************/
int
Main_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  
  Tcl_CreateCommand(interp, "reinit", MochaReinitialize,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp, "_mocha_end", MochaEnd,
                    (ClientData) manager, (Tcl_CmdDeleteProc *) NULL);
  
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Reinitializes the main package.]

  SideEffects        [None]

******************************************************************************/
int
Main_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Ends the main package.]

  SideEffects        [None]

******************************************************************************/
int
Main_End(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  return TCL_OK;
}


/**Function********************************************************************

  Synopsis           [Returns the module manager.]

  Description        [This function returns a pointer to the current module
                      manager. The pointer should be cast to Mdl_Manager_t *
                      before use.]

  SideEffects        [None]

******************************************************************************/
Main_MdlManagerGeneric
Main_ManagerReadModuleManager(
  Main_Manager_t *manager)
{
  return manager->mdlManager;
}

/**Function********************************************************************

  Synopsis           [Returns the type manager.]

  Description        [This function returns a pointer to the current type
                      manager. The pointer should be cast to Var_TypeManager_t
                      * before use.]

  SideEffects        [None]

******************************************************************************/
Main_TypeManagerGeneric
Main_ManagerReadTypeManager(
  Main_Manager_t *manager)
{
  return manager->typeManager;
}

/**Function********************************************************************

  Synopsis           [Returns the state manager.]

  Description        [This function returns the state manager.]

  SideEffects        [None]

******************************************************************************/
Main_StateManagerGeneric
Main_ManagerReadStateManager(
  Main_Manager_t *manager)
{
  return manager->stateManager;
}

/**Function********************************************************************

  Synopsis           [Returns the region manager.]

  Description        [This function returns the region manager.]

  SideEffects        [None]

******************************************************************************/
Main_RegionManagerGeneric
Main_ManagerReadRegionManager(
  Main_Manager_t *manager)
{
  return manager->rgnManager;
}


/**Function********************************************************************

  Synopsis           [Returns the mdd manager.]

  Description        [This function returns the mdd manager.]

  SideEffects        [None]

******************************************************************************/
Main_MddManagerGeneric
Main_ManagerReadMddManager(
  Main_Manager_t *manager)
{
  return manager->mddManager;
}



/**Function********************************************************************

  Synopsis           [Sets the module manager.]

  Description        [This function sets the value of the module manager in
                      the main manager.]

  SideEffects        [The value of the module manager is changed.]

******************************************************************************/
void
Main_ManagerSetModuleManager(
  Main_Manager_t *manager,
  Main_MdlManagerGeneric mdlManager)
{
  manager->mdlManager = mdlManager;
}

/**Function********************************************************************

  Synopsis           [Sets the variable type manager.]

  Description        [This function sets the value of the variable type
                      manager in the main manager.]

  SideEffects        [The value of the variable type manager is changed.]

******************************************************************************/
void
Main_ManagerSetTypeManager(
  Main_Manager_t *manager,
  Main_TypeManagerGeneric typeManager)
{
  manager->typeManager = typeManager;
}

/**Function********************************************************************

  Synopsis           [Sets the state manager.]

  Description        [This function sets the value of the state manager in
                      the main manager.]

  SideEffects        [The value of the state manager is changed.]

******************************************************************************/
void
Main_ManagerSetStateManager(
  Main_Manager_t *manager,
  Main_StateManagerGeneric stateManager)
{
  manager->stateManager = stateManager;
}

/**Function********************************************************************

  Synopsis           [Sets the region manager.]

  Description        [This function sets the value of the region manager in
                      the main manager.]

  SideEffects        [The value of the region manager is changed.]

******************************************************************************/
void
Main_ManagerSetRegionManager(
  Main_Manager_t *manager,
  Main_RegionManagerGeneric rgnManager)
{
  manager->rgnManager = rgnManager;
}

/**Function********************************************************************

  Synopsis           [Sets the mdd manager.]

  Description        [This function sets the value of the mdd  manager in
                      the main manager.]

  SideEffects        [The value of the mdd manager is changed.]

******************************************************************************/
void
Main_ManagerSetMddManager(
  Main_Manager_t *manager,
  Main_MddManagerGeneric mddManager)
{
  manager->mddManager = mddManager;
}
/**Function********************************************************************

  Synopsis           [Returns the manager for the rtm package.]

  Description        [This function returns manager containing the info that
                      the rtm package uses.]

  SideEffects        [None]

******************************************************************************/
Main_RtmRegionManagerGeneric
Main_ManagerReadRtmRegionManager(
  Main_Manager_t *manager)
{
  return manager->rtmManager;
}

/**Function********************************************************************

  Synopsis           [Sets the rtm region manager.]

  Description        [This function sets the value of the rtm region manager in
                      the main manager.]

  SideEffects        [The value of the rtm region manager is changed.]

******************************************************************************/
void
Main_ManagerSetRtmRegionManager(
  Main_Manager_t *manager,
  Main_RtmRegionManagerGeneric rtmRegionManager)
{
  manager->rtmManager = rtmRegionManager;
}

/**Function********************************************************************

  Synopsis           [Prints appropriately to either the Tcl or Tk interface.]

  Description        [This function simulates the printf() command. If Tk is
  invoked, the message will be printed to the Mocha Command Window instead of
  the terminal. The maximum length of the message that can be printed is
  512. Users should use this command instead of printf().]

  SideEffects        [None]

  SeeAlso            [Main_MochaErrorPrint]

******************************************************************************/
void
Main_MochaPrint(
  char* format,
  ...)
{
  va_list args;

  if (MainTkEnabled){
    /* Print to the Tk message window */

    char *msg = ALLOC(char, 512); /* Allocate 512 bytes; hopefully they
                                      would be enough. */
    char *cmd = ALLOC(char, 512);

    va_start(args, format);
    vsprintf(msg, format, args);
    sprintf(cmd, "IntfWriteStdout {%s}", msg);
    Tcl_Eval(mainInterp, cmd);
    FREE(msg);
    FREE(cmd);
    va_end(args);
  }
  else {
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
  }
  
}

/**Function********************************************************************

  Synopsis           [Prints error messages appropriately to either the Tcl or
                      the Tk interface.]

  Description        [This function simulates the fprintf(stderr, ...)
  command. If Tk is invoked, the message will be printed to the Mocha Command
  Window instead of the terminal. The maximum length of the message that can
  be printed is 512. Users should use this command instead of fprintf(stderr,
  ...) ]

  SideEffects        [None]

  SeeAlso            [Main_MochaPrint]

******************************************************************************/
void
Main_MochaErrorPrint(
  char* format,
  ...)
{
  va_list args;

  if (MainTkEnabled){
    /* Print to the Tk message window */

    char *msg = ALLOC(char, 512); /* Allocate 512 bytes; hopefully they
                                      would be enough. */
    char *cmd = ALLOC(char, 512);

    va_start(args, format);
    vsprintf(msg, format, args);
    sprintf(cmd, "IntfWriteStderr {%s}", msg);
    Tcl_Eval(mainInterp, cmd);
    FREE(msg);
    FREE(cmd);
    va_end(args);
  }
  else {
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fflush(stderr);
  }
}

/**Function********************************************************************

  Synopsis           [Prints to file pointer fp]

  Description        [This function simulates the fprintf(fp, ...)
  command. If Tk is invoked, the message will be printed to the Mocha Command
  Window instead of the terminal. The maximum length of the message that can
  be printed is 512. Users should use this command instead of fprintf(stderr,
  ...) ]

  SideEffects        [None]

  SeeAlso            [Main_MochaPrint]

******************************************************************************/
void
Main_MochaFilePrint(
  FILE * fp,
  char* format,
  ...)
{
  va_list args;
  
  if (MainTkEnabled & (fp == stdout || fp == stderr)) {
    /* Print to the Tk message window */
    
    char *msg = ALLOC(char, 512); /* Allocate 512 bytes; hopefully they
                                     would be enough. */
    char *cmd = ALLOC(char, 512);
    
    va_start(args, format);
    vsprintf(msg, format, args);
    if (fp == stdout)
      sprintf(cmd, "IntfWriteStdout {%s}", msg);
    else
      sprintf(cmd, "IntfWriteStderr {%s}", msg);
    Tcl_Eval(mainInterp, cmd);
    FREE(msg);
    FREE(cmd);
    va_end(args);
  } else {
    va_start(args, format);
    vfprintf(fp, format, args);
    va_end(args);
    fflush(fp);
  }

}

/**Function********************************************************************

  Synopsis           [Append result to the Tcl interpreter.]

  Description        [This function appends result to the Tcl Interpreter.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Main_AppendResult(
  char *format,
  ...
  )
{
  va_list args;
  char *msg = ALLOC(char, 512); /* Allocate 512 bytes; hopefully they
                                   would be enough. */
  
  va_start(args, format);
  vsprintf(msg, format, args);
  Tcl_AppendResult(mainInterp, msg, NIL(char));
  FREE(msg);
  va_end(args);
  
}


/**Function********************************************************************

  Synopsis           [Append element to the Tcl interpreter.]

  Description        [This function appends result to the Tcl Interpreter.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Main_AppendElement(
  char *format,
  ...
  )
{
  va_list args;
  char *msg = ALLOC(char, 512); /* Allocate 512 bytes; hopefully they
                                   would be enough. */
  
  va_start(args, format);
  vsprintf(msg, format, args);
  Tcl_AppendElement(mainInterp, msg);
  FREE(msg);
  va_end(args);

}

/**Function********************************************************************

  Synopsis           [Clears the result in the Tcl interpreter.]

  Description        [It clears the result field in the Tcl interpreter.]

  SideEffects        [required]

  SeeAlso            [Tcl_ResetResult in the Tcl Reference Menu]

******************************************************************************/
void 
Main_ClearResult()
{
  Tcl_ResetResult(mainInterp);
}

/**Function********************************************************************

  Synopsis           [Returns the creation time]

  Description        [It returns the system-generated creation time. The
  system keeps an internal counter which increment once each time when this
  function is called. The creation time can be used to keep track of when the
  modules/atoms etc. are generated and it can be used for comparison.]

  SideEffects        [MainCreationTime is incremented.]

  SeeAlso            [optional]

******************************************************************************/
int
Main_ReturnCreationTime()
{
  return (MainCreationTime++);
}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Tk application initialization routine]

  Description        [Required by Tcl/Tk for initialization of the
  packages. It initializes the Tcl/Tk and Tix, and also all the
  packages. See in-line documentation for how to initialize a new package.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
MainTkAppInit(
  Tcl_Interp *interp)
{
  Main_Manager_t *manager;
  Tk_Window tkwindow;
  char cmd[512];
  
  tkwindow = Tk_MainWindow(interp);
  
  if (Tcl_Init(interp) == TCL_ERROR) 
    return TCL_ERROR;

  manager = ALLOC(Main_Manager_t, 1);

  /* must initialize everyting here */
  manager->mddManager = 0;
  
  if (Tk_Init(interp) == TCL_ERROR)
    return TCL_ERROR;

  Tcl_StaticPackage(interp, "Tk", Tk_Init, (Tcl_PackageInitProc *) NULL);

  if (Tix_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }

  /* set some tcl global variables */
  MainTclGlobalVariablesInitialize(interp);

  /* the following is to initialize different core tcl/tk/tix packages */
  
  sprintf(cmd, "%s", "source $MOCHASRCDIR/intf/intfMain.tcl");
  Tcl_Eval (interp, cmd);
  
  sprintf(cmd, "%s", "source $MOCHASRCDIR/trc/trcMain.tcl");
  Tcl_Eval (interp, cmd);

  if (MainInitMochaPackages (interp, manager) == TCL_ERROR)
    return TCL_ERROR;

  mainInterp = interp;

  /* Don't bother to return. Enter an infinite loop */
  Tk_MainLoop();
  Tcl_Eval(interp, "exit");

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Tcl application initialization routine]

  Description        [see TKAppInit]

  SideEffects        [required]

  SeeAlso            [TkAppInit]

******************************************************************************/
int MainTclAppInit(
  Tcl_Interp *interp)
{
  Main_Manager_t *manager;
  char cmd[512];
  
  if (Tcl_Init(interp) == TCL_ERROR)
    return TCL_ERROR;


  /* Call the initialization routine for each package */
  manager = ALLOC(Main_Manager_t, 1);


  /* set some global variables */
  MainTclGlobalVariablesInitialize(interp);
  
  /* the following is to initialize different core tcl/tk/tix packages */

  sprintf(cmd, "%s", "source $MOCHASRCDIR/intf/intfMain.tcl");
  Tcl_Eval (interp, cmd);

  if (MainInitMochaPackages (interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  
  manager->mddManager = 0;  
  mainInterp = interp;

  return TCL_OK;

}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Implements the reinit command.]

  SideEffects        [Mocha is reinitialized.]
  
  CommandName        [reinit] 	   

  CommandSynopsis    [Reinitializes mocha.]  

  CommandDescription [The command reinitializes mocha to the state in which it
                      starts operation when mocha is typed at the shell.]  

******************************************************************************/
static int
MochaReinitialize(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Main_Manager_t *manager = (Main_Manager_t *) clientData;

  if (Var_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Atm_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Enum_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Img_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR; 
  if (Inv_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR; 
  if (Main_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Mdl_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Prs_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Ref_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Rtm_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Sym_Reinit(interp, manager) == TCL_ERROR)
    return TCL_ERROR;

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Implemants the mocha_end command.]

  SideEffects        [None]
  
  CommandName        [_mocha_end] 	   

  CommandSynopsis    [Frees all mocha data structures.]  

  CommandDescription [The command frees all allocated resources in mocha. This
                      command is not visible to the user. It is used to
                      implement the command quit, which is visible.]  

******************************************************************************/
static int
MochaEnd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Main_Manager_t *manager = (Main_Manager_t *) clientData;
/*  
  if (Atm_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Enum_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Img_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR; 
  if (Inv_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR; 
  if (Main_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Mdl_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Prs_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Ref_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Rtm_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Sym_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR; 
  if (Var_End(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
    */
  return TCL_OK;
}



/**Function********************************************************************

  Synopsis           [Initialize all mocha packages.]

  Description        [This function is called by TclAppInit and TkAppInit to
  initialize the mocha packages. It serves as a single point where the
  user initialize their own packages.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
MainInitMochaPackages (
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{

  /* Call the initialization routine for each package.
   * Each call should like this:
   *
   * if (Pkg_Init(interp) == TCL_ERROR)
   *    return TCL_ERROR;
   *
   * where Pkg is the name of the package.
   * All the commands created by Pkg should be registered by Pkg_Init
   * using Tcl_CreateCommand. */


  if (Var_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Atm_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Enum_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Img_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Inv_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Main_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Mdl_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Prs_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;

  if (Ref_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;

  if (Sym_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Mc_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Sim_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Rtm_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;
  if (Sl_Init(interp, manager) == TCL_ERROR)
    return TCL_ERROR;


  return TCL_OK;


}



/**Function********************************************************************

  Synopsis           [Initialize some Tcl global variables.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
MainTclGlobalVariablesInitialize(
  Tcl_Interp* interp)
{


  /* where is the MOCHASRCDIR? */
  MainMochaSrcDir = getenv ("MOCHASRCDIR");
  if (MainMochaSrcDir == NIL(char)) {
    fprintf(stderr, "Please set the MOCHASRCDIR environment variable.\n");
    exit(1);
  }

  /* where is the MOCHAHELPDIR? */
  MainMochaHelpDir = getenv ("MOCHAHELPDIR");
  if (MainMochaHelpDir == NIL(char)) {
    fprintf(stderr, "The MOCHAHELPDIR environment variable is not set:\n");
    fprintf(stderr, "  no help is available for this section");
  }
  
  /* get the process id and create a new tmp directory */
  /* where is the tmp directory */
  {
    int pid = (int) getpid ();
    char *pidString = ALLOC(char, 10);
    char *tmpDir = getenv ("TMPDIR");
    
    if (tmpDir == NIL(char)) {
      tmpDir = getenv ("MOCHATMPDIR");
      if (tmpDir == NIL(char))
        tmpDir = "/tmp";
    }

    sprintf(pidString, "%d", pid);
    MainMochaTmpDir = util_strcat3(tmpDir, "/mocha", pidString);

    FREE(pidString);

  }

  /* set some global variables */
  Tcl_SetVar(interp, "MOCHASRCDIR", MainMochaSrcDir,  TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, "MOCHAHELPDIR", MainMochaHelpDir,  TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, "MOCHATMPDIR", MainMochaTmpDir,  TCL_GLOBAL_ONLY);

  if (MainTkEnabled) {
    Tcl_SetVar(interp, "MOCHATKENABLED", "1",  TCL_GLOBAL_ONLY);
  } else {
    Tcl_SetVar(interp, "MOCHATKENABLED", "0",  TCL_GLOBAL_ONLY);
  }
  
  
}


