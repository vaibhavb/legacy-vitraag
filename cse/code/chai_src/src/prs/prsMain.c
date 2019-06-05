/**CFile***********************************************************************

  FileName    [prsMain.c]

  PackageName [prs]

  Synopsis    [the main file for this package.  The tcl init command is
               defined here.]

  Description [optional]

  SeeAlso     [prs.y]

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
/*vaibhav*/
#include "prsIntf.h"
/*vaibhav*/
#include  "prsInt.h"
#include  "tcl.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define Usage_readModule "Usage: read_module [-hp] <filename>"
/*vaibhav*/
#define Usage_readIntf "Usage: read_intf [-hp] <filename>"
/*vaibhav*/
/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
extern FILE * yyin;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int PrsReadModuleCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
/*vaibhav*/
static int PrsReadIntfCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
/*
static int readIntfModule(char* name, boolean pureOption,Tcl_Interp *interp);
static char* inName(char* name, char* tail);
*/
/*vaibhav*/
static char * FileTail(char *name);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Initializes the prs package.]

  SideEffects        [None]

******************************************************************************/
int
Prs_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  char cmd[512];
  Tcl_CreateCommand(interp, "read_module", PrsReadModuleCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  /*vaibhav*/
  Tcl_CreateCommand(interp, "read_intf", PrsReadIntfCmd,
                    (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);
  /*vaibhav*/
  sprintf(cmd, "%s", "source $MOCHASRCDIR/prs/prsPreproc.tcl");
  Tcl_Eval(interp, cmd);
    
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Reinitializes the prs package.]

  SideEffects        [None]

******************************************************************************/
int
Prs_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Ends the prs package.]

  SideEffects        [None]

******************************************************************************/
int
Prs_End(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  return TCL_OK;
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [Definition of read_module]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [read_module] 	   

  CommandSynopsis    [Reads a file containing Reactive Module descriptions.]  

  CommandArguments   [\[-hp\] \lt;filename\gt;]  

  CommandDescription [This commands reads in a file containing Reactive Module
  descriptions, as well as global type definitions.

  <p>
  Command Options:<p>

  <dl>
  <dt> -h
  <dd> Prints usage.

  <dt> -p
  <dd> Invoke the pure parser. By default, the input file is preprocessed to
  handle #define and #foreach...#endforeach constructs. If -p is specified,
  these will not be handled.

  </dl>]


******************************************************************************/
static int
PrsReadModuleCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char *s,  *name;
  char c;
  boolean pureOption = FALSE; /* by default, preprocessor is invoked before
                                 parsing */
  boolean parseOk = FALSE;
  Main_Manager_t *manager = (Main_Manager_t *) clientData;
  
  PrsGlobalPointersInitialize(manager);

  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "hp")) != EOF) {
    switch (c) {
        case 'p':
          pureOption = TRUE;
          break;
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_readModule);
          return TCL_OK;
        default:
          Main_MochaErrorPrint("%s\n", Usage_readModule);
          return TCL_ERROR;
          break;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("%s\n", Usage_readModule);
    return TCL_ERROR;
  }
  
  name = util_strsav(argv[util_optind]);
  if (!pureOption) { /* use the preprocessor */
    if (Tcl_VarEval(interp, "prs_preproc ", name, NIL(char)) == TCL_ERROR) {
      FREE(name);
      return TCL_ERROR;
    } else {
      char *newname, *tmpname, *tmpdir;

      tmpdir = Tcl_GetVar(interp, "MOCHATMPDIR", TCL_GLOBAL_ONLY);

      assert (tmpdir);

      tmpname = FileTail(name);

      assert (tmpname);

      newname = util_strcat4(tmpdir, "/", tmpname, ".p");
      FREE(name);
      FREE(tmpname);
      name = newname;
    }
  }
  
  
  yyin = fopen (name, "r");
  
  if (yyin == NIL(FILE)) {
    Main_MochaErrorPrint("Error: Cannot read file %s.\n", name);
    FREE(name);
    return TCL_ERROR;
  }
  /* should put everything in a new module manager, and if parse not ok,
     remove the new module manager.  If parse ok, add the modules to the
     master manager. */

  parseOk = PrsParse();
  fclose (yyin);

/* not needed, because the preprocessed file will be removed
   together with the mocha tmp directory after use */
/*
  if (!pureOption & remove(name)) {
    Main_MochaPrint("Warning: cannot remove %s.\n", name);
  }
*/
  
  FREE(name);
  if (parseOk){      
    Main_MochaPrint ("parse successful.\n");
    return (TCL_OK);
  }
  else {
    Main_MochaErrorPrint ("parse not successful.\n");
    return TCL_ERROR;
  }
}



/**Function********************************************************************

  Synopsis           [Returns the string after the last /]

  Description        [This command returns all the characters after the last
  /.This is used for, for instance, getting the filename without the path name
  from an absolute filename.]

  SideEffects        [The user should free the string after use.]

  SeeAlso            [optional]

******************************************************************************/
static char *
FileTail(char *name)
{
  int len, i;
  char *s;

  if (!name)
    return NIL(char);
  
  len = strlen(name);
  
  i = len-1;
  while (i>=0) {
    if (*(name+i) == '/')
      break;
    i--;
  }

  len = len-i-1;
  
  s = ALLOC(char, len+1);
  memcpy(s, (name+i+1), len);
  *(s+len) = '\0';

  return s;
  
}


/*vaibhav*/

/**Function********************************************************************

  Synopsis           [Definition of read_intf]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [read_intf] 	   

  CommandSynopsis    [Reads a file containing Interface descriptions.]  

  CommandArguments   [\[-hp\] \lt;filename\gt;]  

  CommandDescription [This commands reads in a file containing interface description.

  <p>
  Command Options:<p>

  <dl>
  <dt> -h
  <dd> Prints usage.

  <dt> -p
  <dd> Invoke the interface parser.

  </dl>]


******************************************************************************/
static int
PrsReadIntfCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  char newname[512], module_name[256];
  char *s,  *name;
  char c;
  boolean pureOption = FALSE; /* by default, preprocessor is invoked before
                                 parsing */
  boolean parseOk = FALSE;
  Main_Manager_t *manager = (Main_Manager_t *) clientData;
  
  PrsGlobalPointersInitialize(manager);

  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "hp")) != EOF) {
    switch (c) {
        case 'p':
          pureOption = TRUE;
          break;
        case 'h':
          Main_MochaErrorPrint("%s\n", Usage_readIntf);
          return TCL_OK;
        default:
          Main_MochaErrorPrint("%s\n", Usage_readIntf);
          return TCL_ERROR;
          break;
    }
  }

  if (argc - util_optind == 0) {
    Main_MochaErrorPrint("%s\n", Usage_readIntf);
    return TCL_ERROR;
  }
  name = util_strsav(argv[util_optind]);
  strcpy(module_name,splitIntf(name));
  if (pureOption) {
    sprintf(newname," -p %s.I",name);
  }
  else sprintf(newname," %s.I",name);   
  printf("DEBUG %s : %s\n",__FUNCTION__, newname); 
  if (Tcl_VarEval(interp, "read_module ", newname, \
		  NIL(char)) == TCL_ERROR){
    FREE(name);
    return TCL_ERROR;
  } 
  else {
    if (pureOption) {
      sprintf(newname," -p %s.I", name);
    }
    else sprintf(newname," %s.O", name);    
    if (Tcl_VarEval(interp, "read_module ", newname, \
		    NIL(char)) == TCL_ERROR) {
      FREE(name);
      return (TCL_ERROR);
    }
    else {
      //sl_fsm gateI gateO
      sprintf(newname,"%sI %sO", module_name, module_name);
      if (Tcl_VarEval(interp, "sl_fsm ", newname, \
		      NIL(char)) == TCL_ERROR) {
	FREE(name);
	return (TCL_ERROR);
      }
      //sl_make_intf G fsm_gateI    fsm_gateO
      sprintf(newname,"%s fsm_%sI fsm_%sO", module_name, \
	      module_name, module_name);
      if (Tcl_VarEval(interp, "sl_make_intf ", newname, \
		      NIL(char)) == TCL_ERROR) {
	FREE(name);
	return (TCL_ERROR);
      }
      printf("DEBUG Interface Created: %s\n", module_name); 
      FREE(name);
      return (TCL_OK);
    }
  }
}

/*
  readIntfModule(inName(name, ".I"),pureOption, interp);
  readIntfModule(inName(name,".O"),pureOption, interp);
*/

static int
readIntfModule(char* name, boolean pureOption, Tcl_Interp *interp)
{
  boolean parseOk = FALSE;
  if (!pureOption) { /* use the preprocessor */
    if (Tcl_VarEval(interp, "prs_preproc ", name, NIL(char)) == TCL_ERROR) {
      FREE(name);
      return TCL_ERROR;
    } else {
      char *newname, *tmpname, *tmpdir;
      tmpdir = Tcl_GetVar(interp, "MOCHATMPDIR", TCL_GLOBAL_ONLY);
      assert (tmpdir);
      tmpname = FileTail(name);
      assert (tmpname);
      newname = util_strcat4(tmpdir, "/", tmpname, ".p");
      FREE(name);
      FREE(tmpname);
      name = newname;
    }
  }
 
  yyin = fopen (name, "r");
  
  if (yyin == NIL(FILE)) {
    Main_MochaErrorPrint("Error: Cannot read file %s.\n", name);
    FREE(name);
    return TCL_ERROR;
  }
  /* should put everything in a new module manager, and if parse not ok,
     remove the new module manager.  If parse ok, add the modules to the
     master manager. */

  parseOk = PrsParse();
  fclose (yyin);
  FREE(name);
  if (parseOk){      
    Main_MochaPrint ("parse successful.\n");
    return (TCL_OK);
  }
  else {
    Main_MochaErrorPrint ("parse not successful.\n");
    return TCL_ERROR;
  }
}

/*
  static char* inName(char* name, char* tail)
  {
  char newname[60];
  strcpy(newname,name);
  strcpy(newname,tail);
  return (newname);
  }
*/

/*vaibhav*/














