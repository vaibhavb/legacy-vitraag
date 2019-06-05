/**CFile***********************************************************************

  FileName    [refUtil.c]

  PackageName [ref]

  Synopsis    [Routines to check if one module is a refinement of another]

  Description [optional]

  SeeAlso     [optional]

  Author      [Sriram K. Rajamani]

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

#include  "refInt.h"

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


/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis           [check implementation compatibility between spec and impl]

  Description        [optional]

  SideEffects        [none]

  SeeAlso            [optional]

******************************************************************************/
boolean
Ref_CheckImplCompatibility(
  Mdl_Module_t *spec,
  Mdl_Module_t *impl)
{
  boolean result = FALSE;
  lsList  specAllVars = Mdl_ModuleObtainVariableList(spec);
  lsList  implAllVars = Mdl_ModuleObtainVariableList(impl);
  lsList  specExtVars = Mdl_ModuleGetExternalVariableList(spec);
  lsList  specIntfVars = Mdl_ModuleGetInterfaceVariableList(spec);
  lsList  implExtVars = Mdl_ModuleGetExternalVariableList(impl);
  lsGen gen;
  lsGeneric v;
  int countImplLatches = 0;

  lsForEachItem(specAllVars, gen, v) {
    Var_Variable_t *var = (Var_Variable_t *)v;
    Var_Variable_t *ivar;

    ivar = Ref_LookupVarList( implAllVars, Var_VariableReadName(var));
    if(!ivar){
      if(Var_VariableIsEvent(var) ||  Var_VariableIsHistoryFree(var)){
        continue;
      }
      else{
	Main_MochaErrorPrint("History Dependent specification variable %s"
			     " is not found in implementation\n",
                             Var_VariableReadName(var));
        goto done;
      }
    }
    else{
      if(Var_VariableReadDataType(var) != Var_VariableReadDataType(ivar)){
        Main_MochaErrorPrint("Variable %s has different data types in spec and impl",
                             Var_VariableReadName(var));
        goto done;
      }
      
    }
  }

  /*
    we also need to check if each interface variable in spec is an interface variable
    of the impl and an awaits dependencey relation check in the future.
    */

  lsForEachItem(specExtVars, gen, v){
    Var_Variable_t *var = (Var_Variable_t *)v;
    Var_Variable_t *ivar;

    ivar = Ref_LookupVarList( implAllVars, Var_VariableReadName(var));
    if(!ivar){
	Main_MochaErrorPrint("External specification variable %s"
			     " is not found in implementation\n",
                             Var_VariableReadName(var));
        goto done;
    }
  }

  lsForEachItem(specIntfVars, gen, v){
    Var_Variable_t *var = (Var_Variable_t *)v;
    Var_Variable_t *ivar;

    ivar = Ref_LookupVarList( implAllVars, Var_VariableReadName(var));
    if(!ivar){
	Main_MochaErrorPrint("External interface variable %s"
			     " is not found in implementation\n",
                             Var_VariableReadName(var));
        goto done;
    }
  }

  lsForEachItem(implAllVars, gen, v){
    Var_Variable_t *var = (Var_Variable_t *)v;
    if(Var_VariableIsEvent(var) ||  Var_VariableIsHistoryFree(var))
      continue;

    if(Ref_LookupVarList(implExtVars, Var_VariableReadName(var)))
       continue;

    countImplLatches++;
    Main_MochaPrint("Implementation latch : %d : %s\n", countImplLatches, Var_VariableReadName(var));
  }

  Main_MochaPrint("Number of implementation latches = %d\n", countImplLatches);

  result = TRUE;
    
  done:
  lsDestroy(specAllVars, 0);
  lsDestroy(implAllVars, 0);
  lsDestroy(specExtVars, 0);
  lsDestroy(specIntfVars, 0);
  lsDestroy(implExtVars, 0);
  return(result);
}

  


/**Function********************************************************************

  Synopsis           [Lookup variable by name from a list of variables]

  Description        [optional]

  SideEffects        [none]

  SeeAlso            [optional]

******************************************************************************/

Var_Variable_t *
Ref_LookupVarList(
  lsList varList,
  char *name)
{
  lsGeneric v;
  lsGen gen;
  Var_Variable_t *var;

  lsForEachItem(varList, gen, v){
    var = (Var_Variable_t *)v;
    if( strcmp(name, Var_VariableReadName(var)) == 0)
      return(var);
  }

  return(NIL(Var_Variable_t));
}



/**Function********************************************************************

  Synopsis           [test for membership in list of atoms]

  Description        [optional]

  SideEffects        [none]

  SeeAlso            [optional]

******************************************************************************/
boolean
Ref_IsMemberOfAtomList(
  lsList atomList,
  Atm_Atom_t *atom)
{
  lsGeneric a;
  lsGen gen;

  lsForEachItem(atomList, gen, a){
    if( atom == (Atm_Atom_t *) a)
      return(TRUE);
  }

  return(FALSE);
}

/**Function********************************************************************

  Synopsis           [test for membership in list of names]

  Description        [optional]

  SideEffects        [none]

  SeeAlso            [optional]

******************************************************************************/
boolean
Ref_IsMemberOfNameList(
  lsList nameList,
  char *name)
{
  lsGeneric a;
  lsGen gen;

  lsForEachItem(nameList, gen, a){
    if(strcmp(name,(char *) a) == 0)
      return TRUE;
  }

  return FALSE;
}

/**Function********************************************************************

  Synopsis           [free MDD array]

  Description        [Free the MDD array along with all MDDs contained in it]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
void
RefMddArrayFree(
  array_t *mddArray)
{
  int len = array_n(mddArray);
  int i;

  for ( i = 0; i < len; i++){
    mdd_t *curMdd = (mdd_t *)array_fetch(mdd_t *, mddArray, i);
    mdd_free(curMdd);
  }
  array_free(mddArray);
}
