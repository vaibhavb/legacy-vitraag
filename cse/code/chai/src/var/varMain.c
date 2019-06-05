/**CFile***********************************************************************

  FileName    [varMain.c]

  PackageName [var]

  Synopsis    [Routines for the var package to interface with the main package.]

  Description [This file contains all routines that are required to interact
  with the Main package, including routine for initialization of package by
  tcl/tk and those for registering new commands.]

  SeeAlso     [atm]

  Author      [Freddy Mang, Shaz Qadeer, Sriram Rajamani]

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

#include "varInt.h" 
#include "tcl.h"

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

static int VarShowTypesCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Initializes the var package.]

  SideEffects        [The command, show_types is registered.]

  SeeAlso            [Var_Reinit, Var_End]

******************************************************************************/
int
Var_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Var_TypeManager_t *typeManager = Var_TypeManagerInit();

  Main_ManagerSetTypeManager(manager, (Main_TypeManagerGeneric) typeManager);
  
  Tcl_CreateCommand(interp, "show_types", VarShowTypesCmd, (ClientData) manager,
                    (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Reinitializes the var package.]

  SideEffects        [None]

  SeeAlso            [Var_Init, Var_End]

******************************************************************************/
int
Var_Reinit(
  Tcl_Interp *interp,
  Main_Manager_t *manager) 
{
  Var_TypeManager_t *typemanager = (Var_TypeManager_t *)
      Main_ManagerReadTypeManager(manager);

  Var_TypeManagerFree(typemanager);
  typemanager = Var_TypeManagerInit();
  Main_ManagerSetTypeManager(manager, (Main_TypeManagerGeneric)
                             typemanager);

  return TCL_OK;
}

/**Function********************************************************************

  Synopsis           [Ends the var package.]

  SideEffects        [None]

  SeeAlso            [Var_Init, Var_Reinit]

******************************************************************************/
int
Var_End(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{
  Var_TypeManager_t *typemanager = (Var_TypeManager_t *)
      Main_ManagerReadTypeManager(manager);

  Var_TypeManagerFree(typemanager);

  return TCL_OK;
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Tcl command "show_types"]

  Description        [Implements the tcl command "show_types"]

  SideEffects        [required]

  SeeAlso            [optional]

  CommandName        [show_types] 	   

  CommandSynopsis    [List all types defined in the current Mocha session.]  

  CommandArguments   [\[-lg\]]  

  CommandDescription [

  Command Options:<p>

  <dl>

  <dt> -l
  <dd> List the names of all the types, together with their content.

  <dt> -g
  <dd> Debug mode. Complete information of the types are listed. For
  enumerative types, the index of the enumerative constants are listed.

  </dl>]
]  

******************************************************************************/
static int
VarShowTypesCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv)
{
  st_table* typetable;
  st_generator* gen;
  Var_Type_t* vartype;
  int opt_l, opt_g, i;
  char c;
  Var_EnumElement_t* typeconst;
  char* name;
  
  int flag = 0;
  
  Var_TypeManager_t *typeManager = (Var_TypeManager_t *)
      Main_ManagerReadTypeManager((Main_Manager_t *) clientData);
  
  opt_l = opt_g = 0 ;
  
  util_getopt_reset();
  while ((c=util_getopt(argc, argv, "lg")) != -1) {
    switch (c) {
        case 'l':
          opt_l = 1;
          break;
        case 'g':
          opt_g = 1;
          break;
        default:
          Main_MochaErrorPrint("Usage: show_type [-lg] <module>\n");
          return TCL_OK;
          break;
    }
  }

  Main_MochaPrint("Built-in    : bool, int, nat, event\n\n");

  Main_MochaPrint("Enumerative : ");
  Var_TypeManagerForEachVarType(typeManager, gen, vartype) {
    if (!vartype -> name)
      name = "unnamed";
    else
      name = vartype -> name;
    
    if (Var_VarTypeReadDataType(vartype) == Var_Enumerated_c){
      if (opt_l || opt_g) {
        int count = 1;
        int size = Var_VarTypeReadDomainSize(vartype);
        
        if (opt_g)
          Main_MochaPrint("\n\t%s (%d values) : {", name, size);
        else
          Main_MochaPrint("\n\t%s : {", name);
        
        flag = 1;
        
        Var_VarTypeForEachSymbolicValue(vartype, i, typeconst){
          char * constName = Var_EnumElementReadName(typeconst);
          Main_MochaPrint("%s", constName);
          if (count < size)
            if (opt_g) 
              Main_MochaPrint("(%d), ", Var_EnumElementObtainIndex(typeconst));
            else
              Main_MochaPrint(", ", Var_EnumElementObtainIndex(typeconst));
          else
            if (opt_g) 
              Main_MochaPrint("(%d)", Var_EnumElementObtainIndex(typeconst));
          count++;
        }
        Main_MochaPrint("}");
      }
      else {
        Main_MochaPrint("\n\t%s", name);
        flag = 1;
      }
    }  
  }
  
  if (flag==0) Main_MochaPrint("no enumerative type defined.");
  Main_MochaPrint("\n\n");

  flag = 0;
  Main_MochaPrint("Range       : ");
  Var_TypeManagerForEachVarType(typeManager, gen, vartype) {
    if (!vartype -> name)
      name = "unnamed";
    else
      name = vartype -> name;
    
    if (Var_VarTypeReadDataType(vartype) == Var_Range_c){
      if (opt_l || opt_g) {
        Main_MochaPrint("\n\t%s: (0..%d)", name,
                        Var_VarTypeReadDomainSize(vartype) - 1);
        flag = 1;
      }
      else{
        Main_MochaPrint("\n\t%s", name);
        flag = 1;
      }
    }
  }
  
  if (flag==0) Main_MochaPrint("no range type defined.");
  Main_MochaPrint("\n\n");

  flag = 0;
  Main_MochaPrint("Bitvector   : ");
  Var_TypeManagerForEachVarType(typeManager, gen,  vartype) {
    if (!vartype -> name)
      name = "unnamed";
    else
      name = vartype -> name;
    
    if (Var_TypeIsBV(vartype)){
      if (opt_l || opt_g) {
        if (Var_TypeIsBitvector(vartype)) {
          Main_MochaPrint("\n\t%s: bitvector %d", name,
                          Var_VarTypeArrayOrBitvectorReadSize(vartype));
          flag = 1;
        } else {
          Main_MochaPrint("\n\t%s: bitvector_n %d", name,
                          Var_VarTypeArrayOrBitvectorReadSize(vartype));
        }
      }
      else{
        Main_MochaPrint("\n\t%s ", name);
        flag = 1;
      }
    }
  }
  if (flag==0) Main_MochaPrint("no bitvector type defined.");
  Main_MochaPrint("\n\n");
  
  flag = 0;
  Main_MochaPrint("Array       : ");
  Var_TypeManagerForEachVarType(typeManager, gen, vartype) {
    if (!vartype -> name)
      name = "unnamed";
    else
      name = vartype -> name;
    
    if (Var_VarTypeReadDataType(vartype) == Var_Array_c){
      if (opt_l || opt_g) {
        Main_MochaPrint("\n\t%s: array %s of %s", name,
                        Var_VarTypeReadName(Var_VarTypeReadIndexType(vartype)),
                        Var_VarTypeReadName(Var_VarTypeReadEntryType(vartype))); 
        flag = 1;
      }
      else{
        Main_MochaPrint("\n\t%s ", name);
        flag = 1;
      }
    }
  }
  
  if (flag==0) Main_MochaPrint("no array type defined.");
  Main_MochaPrint("\n");

  return TCL_OK;
}














