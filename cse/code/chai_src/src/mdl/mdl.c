/**CFile***********************************************************************

  FileName    [mdl.c]

  PackageName [mdl]

  Synopsis    [Contains routines for manipulating the module manager.]

  Description [optional]

  SeeAlso     [optional]

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


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
static int MdlCounter = 0;


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void MdlExprReadComponentsRecursively(Mdl_Expr_t * mexpr, array_t ** leafArray);
static void MdlVariableCopyVariableReadbyAtomList(char* modname, Var_Variable_t* newvar, Var_Variable_t* oldvar, st_table* newatmtable);
static void MdlVariableCopyVariableAwaitedbyAtomList(char* modname, Var_Variable_t* newvar, Var_Variable_t* oldvar, st_table* newatmtable);
static boolean MdlNameSplit(char *name, char **firstName, char **middleName, char **lastName, int *ctr);
static boolean MdlAtomNameCheckConflict(Mdl_Module_t *newmodule, char ** name1, char ** name2);
static lsList MdlExprListDup(lsList list);
static boolean MdlExprSearchForComponent(char *name, Mdl_Expr_t *mexpr, array_t ** matchArray);
static void MdlExprReadAtomIntoArray(Mdl_Expr_t* mexpr, array_t **atomArray);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Allocate memory for a new module.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mdl_Module_t*
Mdl_ModuleAlloc(
  char* name)
{
  Mdl_Module_t* module = ALLOC(Mdl_Module_t, 1);
  module -> name = util_strsav(name);
  module -> moduleExpr = NIL(Mdl_Expr_t);
  module -> atmTable = st_init_table (strcmp, st_strhash);
  module -> varTable = st_init_table (strcmp, st_strhash);
  module -> stateInvTable = st_init_table (strcmp, st_strhash);
  module -> tranInvTable = st_init_table (strcmp, st_strhash);
  module -> creationTime = Main_ReturnCreationTime();

  return module;
}

/**Function********************************************************************

  Synopsis           [Add information to the module.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Mdl_ModuleAddValueInfo(
  Mdl_Module_t* module,  
  st_table* atmTable,
  st_table* varTable)
{
  module -> atmTable = atmTable;
  module -> varTable = varTable;
  
  return 1;
}

/**Function********************************************************************

  Synopsis           [It frees a module.]

  Description        [It is the user's responsibility to free the atoms and
                      variables in the vartable and atomtable first, before
		      freeing the module.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Mdl_ModuleFree(
  Mdl_Module_t* module)
{
  Atm_Atom_t* atom;
  Var_Variable_t* var;
  Mdl_Invariant_t* inv;
  st_generator* gen;
  char* key;
  

  if (module != NIL(Mdl_Module_t)){
    FREE(module->name);
    Mdl_ExprFree(module->moduleExpr);
    
    /* free the entries */
    st_foreach_item(module->atmTable, gen, &key, (char**) &atom){
      FREE(key);
      Atm_AtomFree(atom);
    }
    
    st_foreach_item(module->varTable, gen, &key, (char**) &var){
      FREE(key);
      Var_VariableFree(var);
    }

    st_foreach_item(module->stateInvTable, gen, &key, (char**) &inv){
      FREE(key);
      Mdl_InvariantFree(inv);
    }

    st_foreach_item(module->tranInvTable, gen, &key, (char**) &inv){
      FREE(key);
      Mdl_InvariantFree(inv);
    }
    
    /* then free the tables */
    st_free_table (module -> atmTable);
    st_free_table (module -> varTable);
    st_free_table (module -> stateInvTable);
    st_free_table (module -> tranInvTable);
    
    /* finally, free the module itself */
    FREE(module);
  }
}


/**Function********************************************************************

  Synopsis           [Free the module manager.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Mdl_ModuleManagerFree(
  Mdl_Manager_t* mdlmanager)
{
  char* key, *value;
  st_generator *gen;
  
  st_foreach_item(mdlmanager->nameToModule, gen, &key, &value){
     FREE(key);
     Mdl_ModuleFree((Mdl_Module_t*) value);
  }

  st_free_table(mdlmanager->nameToModule);
  
  FREE(mdlmanager);
}


/**Function********************************************************************

  Synopsis           [Add a module to the module manager.]

  Description        [When adding a module, the name of the module and its
  creation time are hashed into the module dependency table.  Also, it
  maintains a list of pointers pointing to the its component modules.
  It feturns 1 if successful, 0 otherwise.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Mdl_ModuleAddToManager(
  Mdl_Module_t* module,
  Mdl_Manager_t* mdlmanager)
{
  st_table* table = mdlmanager->nameToModule;
  char* name;
  Mdl_Module_t* tmpmdl;

  if(module != NIL(Mdl_Module_t)){
    name = Mdl_ModuleReadName(module);
    Mdl_ModuleRemoveFromManager(name, mdlmanager);
    st_insert(table, util_strsav(name), (char*) module);
    return 1;
  }
  else
    return 0;
}

/**Function********************************************************************

  Synopsis           [Returns a pointer to  a module.]

  Description        [When called with a name of a module, this function
                      looks up the name in the module manager.  If found,
                      this function returns a pointer to this module.
		      Otherwise, it returns NIL(Mdl_Module_t).

		      The user should NOT free or modify this module.
		      ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mdl_Module_t*
Mdl_ModuleReadFromName(
  Mdl_Manager_t* mdlmanager,
  char* name)
{
  Mdl_Module_t* module;

  if(st_lookup(mdlmanager->nameToModule, name, (char**) &module))
    return module;
  else
    return NIL(Mdl_Module_t);
}

/**Function********************************************************************

  Synopsis           [Returns the name of a module.]

  Description        [This function returns a pointer to the name of the
                      module.  User should not free or modify the string.]
  
  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
char*
Mdl_ModuleReadName(
  Mdl_Module_t* module)
{
  if (module!=NIL(Mdl_Module_t))
    return module->name;
  return NIL(char);
}

/**Function********************************************************************

  Synopsis           [Returns a pointer to the variable table.]

  Description        [Given a pointer to a module, this function
                      returns a pointer to the variable table.

		      The user should not free or modify the table.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
st_table*
Mdl_ModuleReadVariableTable(
  Mdl_Module_t* module)
{
  if (module != NIL(Mdl_Module_t))
    return (module->varTable);
  else
    return (NIL(st_table));
}


/**Function********************************************************************

  Synopsis           [Returns a pointer to the atom table.]

  Description        [Given a pointer to a module, this function
                      returns a pointer to the atom table.

		      The user should not free or modify the table.]

  SideEffects        [required]

  SeeAlso            [optional]


******************************************************************************/
st_table*
Mdl_ModuleReadAtomTable(
  Mdl_Module_t* module)
{
  if (module != NIL(Mdl_Module_t))
    return (module->atmTable);
  else
    return NIL(st_table);
}

/**Function********************************************************************

  Synopsis           [Returns the creation time of the module.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Mdl_ModuleReadCreationTime(
  Mdl_Module_t * module)
{
  if (module != NIL(Mdl_Module_t))
    return (module->creationTime);
  else
    return -1;

}



/**Function********************************************************************

  Synopsis           [Returns a pointer to the atom specified by its name]

  Description        [This function returns a pointer to the atom, given
  the atom name and the module.]

  SideEffects        [The user should not free the atom.]

  SeeAlso            [optional]

******************************************************************************/
Atm_Atom_t *
Mdl_ModuleReadAtomFromName(
  char * name,
  Mdl_Module_t * module)
{
  st_table *atmTable;
  Atm_Atom_t *atom;

  if (module == NIL(Mdl_Module_t)) {
    return NIL(Atm_Atom_t);
  }

  if (name == NIL(char)) {
    return NIL(Atm_Atom_t);
  }

  atmTable = Mdl_ModuleReadAtomTable(module);
  if (st_lookup(atmTable, name, (char**) &atom)) /* atom found */
    return atom;
  else
    return NIL(Atm_Atom_t);

}



/**Function********************************************************************

  Synopsis           [Returns a list of  module names.]

  Description        [This function returns a list of module names. 
                      Will later add the definition of the modules.
		      It is the user's responsibility to free the list.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
lsList
Mdl_ModuleManagerObtainModuleList(
  Mdl_Manager_t* mdlmanager)
{
  char* key, *value;
  st_generator* gen;
  lsList list = lsCreate();
  
  st_foreach_item(mdlmanager->nameToModule, gen, &key, &value){
    lsNewEnd(list, (lsGeneric) util_strsav(key), LS_NH);
  }

  return list;
}

/**Function********************************************************************

  Synopsis           [Returns a list of atoms given a module.]

  Description        [Returns a list of atoms given a module.  Each
                      element of the list is a pointer to the atoms
		      in the atom table. 
		      
		      In later implementation, this should return a 
		      {\em copy} of the {\em linearized partial ordering}
		      of the atoms.

		      The user should NOT free or modify the list.]

  SideEffects        [required]

  SeeAlso            [optional]


******************************************************************************/
lsList
Mdl_ModuleObtainAtomList(
  Mdl_Module_t* module)
{
  st_generator* gen;
  char* key;
  char* atom;
  lsList list = lsCreate();
  
  if (module != NIL(Mdl_Module_t))
    st_foreach_item(module->atmTable, gen, &key, &atom){
      lsNewEnd(list, (lsGeneric) atom, LS_NH);
    }
  
  return list;
}


/**Function********************************************************************

  Synopsis           [Creates a copy of module, with a given name.]

  Description        [This function creates a copy of the module, with a given
                      name.  It also creates a new copy of the module table
		      and atom table, with atom names prefixed with the new 
		      module name, and the private variables prefixed with
		      the new module name too.

		      The function returns a pointer to the new module.]

  SideEffects        [required]

******************************************************************************/
Mdl_Module_t*
Mdl_ModuleDup(
  char* name,                       /* name of the new module */
  Mdl_Module_t* module,             /* the module to be copied */
  st_table **atomMappingTable,      /* mapping between original atoms to new
                                       atoms */
  st_table **variableMappingTable   /* mapping between original vars to new
                                       vars */
  )
{
  Mdl_Module_t* newmodule=Mdl_ModuleAlloc(name);

  *variableMappingTable = NIL(st_table);
  *atomMappingTable = NIL(st_table);
    
  Atm_AtomCounterReset();
  if(module != NIL(Mdl_Module_t)){
    *variableMappingTable = MdlVariablesCopyToNewModule(newmodule, module);

    if (*variableMappingTable != NIL(st_table)){

      *atomMappingTable = MdlAtomsCopyToNewModule(newmodule, module, *variableMappingTable);

      MdlVariablesWrapUp(newmodule);
      MdlAtomsWrapUp(newmodule);
      
      return newmodule;
    } else {
      
      Mdl_ModuleFree(newmodule);
      return NIL(Mdl_Module_t);
    }
  }
  
  return NIL(Mdl_Module_t);
  
}
  

/**Function********************************************************************

  Synopsis           [Set the moduleExpr of a module.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Mdl_ModuleSetModuleExpr(
  Mdl_Module_t * module,
  Mdl_Expr_t * moduleExpr)
{
  module->moduleExpr = moduleExpr;
  
}

/**Function********************************************************************

  Synopsis           [Read the moduleExpr of a module]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mdl_Expr_t *
Mdl_ModuleReadModuleExpr(
  Mdl_Module_t * module)
{
  return module->moduleExpr;
}


/**Function********************************************************************

  Synopsis           [Generates a new module name.]

  Description        [It keeps a counter that counts the number of intermediate
                      module generated.  The counter is also used to generate a
		      name for the modules.  For example, if the counter
		      counts 14, the generated module name will be @M14.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
char*
Mdl_ModuleObtainNewName()
{
  char *counterString = Main_ConvertIntToString(MdlCounter);
  char *name;

  name = util_strcat3("@M", counterString, "");
  
  MdlCounter ++;
  FREE(counterString);

  return name;
  
}

/**Function********************************************************************

  Synopsis           [Returns a name with a new prefix.]

  Description        [This function takes in a prefix string and a name.]

  SideEffects        [User should free the returned string.]

******************************************************************************/

char* 
Mdl_NamePrefixWithString(
  char* prefix,
  char* name)
{
  return util_strcat3(prefix, "/", name);
}


/**Function********************************************************************

  Synopsis           [Delete a variable.]

  Description        [Delete a variable from the module. This routine only
                      deletes the entry of the variable from the varTable
		      of the module.  It is the user's responsibility to
		      free the memory allocated to the variable.

		      The routine returns 0 if the variable is not found
		      in the module, 1 if deletion is successful.]

  SideEffects        [required]

******************************************************************************/
int
Mdl_ModuleDeleteVariable(
  Mdl_Module_t* module,
  char* varname)
{
  st_table* vartable = Mdl_ModuleReadVariableTable(module);
  int flag = 0;
  Var_Variable_t* var1;

  if (st_lookup(vartable, varname, (char**) &var1)){ /* variable found */
    st_delete(vartable, &varname, (char**) &var1);
    FREE(varname);
    flag = 1;
  }

  return flag;

}

/**Function********************************************************************

  Synopsis           [Add a variable to the module.]

  Description        [returns 1 if addition is successful, 0 if variable
                      of the same name already exists in the module.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Mdl_ModuleAddVariable(
  Mdl_Module_t* module,
  char* varname,
  Var_Variable_t* var
)
{
  st_table* vartable = Mdl_ModuleReadVariableTable(module);
  int flag = 0;
  Var_Variable_t* var1;

  if (!st_lookup(vartable, varname, (char**) &var1)){
    st_insert(vartable, util_strsav(varname), (char*) var);
    flag = 1;
  }

  return flag;
}


/**Function********************************************************************

  Synopsis           [Add an atom to the module.]

  Description        [returns 1 if addition is successful, 0 if variable
                      of the same name already exists in the module.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Mdl_ModuleAddAtom(
  Mdl_Module_t* module,
  char* atmname,
  Atm_Atom_t* atm
)
{
  st_table* atmtable = Mdl_ModuleReadAtomTable(module);
  int flag = 0;
  Atm_Atom_t* atm1;

  if (!st_lookup(atmtable, atmname, (char**) &atm1)){
    st_insert(atmtable, util_strsav(atmname), (char*) atm);
    flag = 1;
  }

  return flag;
}

/**Function********************************************************************

  Synopsis           [Remove a module of a given name from a manager.]

  Description        [This remove a module of a given name from a module
                      manager.  If the module is found, this routine
		      returns a pointer to the module. If not, NIL(Mdl_Module_t)
		      is returned instead.]

  SideEffects        [required]

******************************************************************************/
boolean
Mdl_ModuleRemoveFromManager(
  char* name,
  Mdl_Manager_t* mdlmanager
)
{
  Mdl_Module_t* module = 0;

  if (st_delete(mdlmanager->nameToModule, &name, (char**) &module)){
    FREE(name);
    if(module){
      Mdl_ModuleFree(module);
      return(TRUE);
    }
  }
  
  return(FALSE);
}

/**Function********************************************************************

  Synopsis           [Return a pointer to the variable given a name and a module]

  Description        [When given a name of a variable, and a pointer to a module,
                      this function returns a pointer to the variable of the given
		      name.  If the desired variable is not found, a 
		      NIL(Var_Variable_t) will be returned.  User should not free
		      of modifiy the pointer.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Variable_t*
Mdl_ModuleReadVariableFromName(
  char* varname,
  Mdl_Module_t* module)
{
  char* modname;
  st_table* vartable;
  Var_Variable_t* var;

  if (module == NIL(Mdl_Module_t)) return NIL(Var_Variable_t);
  
  modname = Mdl_ModuleReadName(module);
  vartable = Mdl_ModuleReadVariableTable(module);

  if(!st_lookup(vartable, varname, (char**) &var)){
    char *tmpname = Mdl_NamePrefixWithString(modname, varname);
    if(!st_lookup(vartable, tmpname , (char**) &var))
      var = NIL(Var_Variable_t);
    FREE(tmpname);
  }
  
  return var;
}

/**Function********************************************************************

  Synopsis           [returns a list of all variables.]

  Description        [Given a module (pointer), this function returns
                      a list of all variables.  The
		      variables are arranged according to the topological
		      order of the atoms that controls the variables.
		      External variables come first. It is the user's
		      responsibility to free the list (but not the list
		      elements).]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
lsList
Mdl_ModuleObtainVariableList(
  Mdl_Module_t* module)
{
  lsList all_list;
  lsList list;
  lsList varlist;
  lsGen gen;
  lsGen gen1;
  Atm_Atom_t* atom;
  Var_Variable_t* var;
  Var_Type_t*  type;
  
  all_list = lsCreate();

  list = Mdl_ModuleGetExternalVariableList(module);
  lsForEachItem(list, gen, var) {
    type = Var_VariableReadType(var);
    if (!(Var_TypeIsArray(type) || Var_TypeIsBV(type)))
      lsNewEnd(all_list, (lsGeneric) var, LS_NH);
  }

  lsDestroy (list, (void (*) ()) 0);
  
  list = Mdl_ModuleObtainSortedAtomList(module);
  lsForEachItem(list, gen, atom) {
    varlist = Atm_AtomReadControlVarList(atom); 
    lsForEachItem(varlist, gen1, var) {
      type = Var_VariableReadType(var);
      if (!(Var_TypeIsArray(type) || Var_TypeIsBV(type)))
        lsNewEnd(all_list, (lsGeneric) var, LS_NH);
    }
  }

  lsDestroy (list, (void (*) ()) 0); 

  return all_list;
}


  


/**Function********************************************************************

  Synopsis           [returns a list of history_dependent variables.]

  Description        [Given a module (pointer), this function returns
                      a list of history_dependent variables.  The
		      variables are arranged according to the topological
		      order of the atoms that controls the variables.
		      External variables come first. It is the user's
		      responsibility to free the list (but not the list
		      elements).]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
lsList
Mdl_ModuleObtainHistoryDependentVariableList(
  Mdl_Module_t* module
)
{
  lsList hdlist;
  lsList list;
  lsList varlist;
  lsGen gen;
  lsGen gen1;
  Atm_Atom_t* atom;
  Var_Variable_t* var;

  hdlist = lsCreate();
  list = Mdl_ModuleGetExternalVariableList(module);
  lsForEachItem(list, gen, var) {
    if (Var_VariableIsHistoryDependent(var))
      lsNewEnd(hdlist, (lsGeneric) var, LS_NH);
  }
  lsDestroy(list, NULL);
  
  list = Mdl_ModuleObtainSortedAtomList(module);
  lsForEachItem(list, gen, atom) {
    varlist = Atm_AtomReadControlVarList(atom); 
    lsForEachItem(varlist, gen1, var) {
      if (Var_VariableIsHistoryDependent(var))
	lsNewEnd(hdlist, (lsGeneric) var, LS_NH);
    }
  }
  lsDestroy(list, NULL);
  
  return hdlist;
}

/**Function********************************************************************

  Synopsis           [Returns a list of history free variables.]

  Description        [Given a module (pointer), this function returns
                      a list of history free variables.  The
		      variables are arranged according to the topological
		      order of the atoms that controls the variables.
		      External variables come first. It is the user's
		      responsibility to free the list (but not the list
		      elements).]

  SideEffects        [None]

******************************************************************************/
lsList
Mdl_ModuleObtainHistoryFreeVariableList(
  Mdl_Module_t* module)
{
  lsList hdlist;
  lsList list;
  lsList varlist;
  lsGen gen;
  lsGen gen1;
  Atm_Atom_t* atom;
  Var_Variable_t* var;

  hdlist = lsCreate();
  list = Mdl_ModuleGetExternalVariableList(module);
  lsForEachItem(list, gen, var) {
    if (Var_VariableIsHistoryFree(var))
      lsNewEnd(hdlist, (lsGeneric) var, LS_NH);
  }
  lsDestroy(list, NULL);
  
  list = Mdl_ModuleObtainSortedAtomList(module);
  lsForEachItem(list, gen, atom) {
    varlist = Atm_AtomReadControlVarList(atom); 
    lsForEachItem(varlist, gen1, var) {
      if (Var_VariableIsHistoryFree(var))
	lsNewEnd(hdlist, (lsGeneric) var, LS_NH);
    }
  }
  lsDestroy(list, NULL);
  
  return hdlist;
}



/**Function********************************************************************

  Synopsis           [returns a list of event variables.]

  Description        [Given a module (pointer), this function returns
                      a list of event variables.  The
		      variables are arranged according to the topological
		      order of the atoms that controls the variables.
		      External variables come first. It is the user's
		      responsibility to free the list (but not the list
		      elements).]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
lsList
Mdl_ModuleObtainEventVariableList(
  Mdl_Module_t* module)
{
  lsList hdlist;
  lsList list;
  lsList varlist;
  lsGen gen;
  lsGen gen1;
  Atm_Atom_t* atom;
  Var_Variable_t* var;

  hdlist = lsCreate();
  list = Mdl_ModuleGetExternalVariableList(module);
  lsForEachItem(list, gen, var) {
    if (Var_VariableIsEvent(var))
      lsNewEnd(hdlist, (lsGeneric) var, LS_NH);
  }
  lsDestroy(list, NULL);
  
  list = Mdl_ModuleObtainSortedAtomList(module);
  lsForEachItem(list, gen, atom) {
    varlist = Atm_AtomReadControlVarList(atom); 
    lsForEachItem(varlist, gen1, var) {
      if (Var_VariableIsEvent(var))
	lsNewEnd(hdlist, (lsGeneric) var, LS_NH);
    }
  }
  lsDestroy(list, NULL);
  
  return hdlist;
}


/**Function********************************************************************

  Synopsis           [Allocate an invariant structure.]

  Description        [Allocate an invariant structure and return a pointer to
                      it.]

  SideEffects        [required]

  SeeAlso            [Mdl_InvariantFree]

******************************************************************************/
Mdl_Invariant_t *
Mdl_InvariantAlloc(
  char *name,
  Mdl_Module_t *module)
{
  Mdl_Invariant_t *invariant;

  invariant = ALLOC(Mdl_Invariant_t, 1);
  invariant -> name = util_strsav(name);
  invariant -> module = module;
  invariant -> status = Mdl_InvUnchecked_c;
  invariant -> inv_expr = NIL(Atm_Expr_t);

  return invariant;

}

/**Function********************************************************************

  Synopsis           [Free an invariant structure.]

  Description        [Free an invariant structure.]

  SideEffects        [required]

  SeeAlso            [Mdl_InvariantAlloc]

******************************************************************************/
void
Mdl_InvariantFree(
  Mdl_Invariant_t *invariant)
{
  if (invariant != NIL(Mdl_Invariant_t)){
    if (invariant -> name)
      FREE (invariant -> name);
    if (invariant -> inv_expr)
      Atm_ExprFreeExpression(invariant->inv_expr);

    FREE(invariant);
  }  
}

/**Function********************************************************************

  Synopsis           [Set the invariant expression of an invariant struct. ]

  Description        [Set the invariant expression of an invariant struct. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Mdl_InvariantSetExpression(
  Mdl_Invariant_t *invariant,
  Atm_Expr_t *exptree)
{
  if (invariant) {
    invariant -> inv_expr = exptree;
  }
}

/**Function********************************************************************

  Synopsis           [Set the status of the invariant.]

  Description        [Set the status of the invariant.  Can set it
                      to one of these three values: Mdl_InvPassed_c,
                      Mdl_InvFailed_c, Mdl_InvUnchecked_c.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Mdl_InvariantSetStatus(
  Mdl_Invariant_t *invariant,
  Mdl_InvStatus_t status)
{
  if (invariant)
    invariant -> status = status;
}

/**Function********************************************************************

  Synopsis           [Return the status of an invariant]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mdl_InvStatus_t
Mdl_InvariantReadStatus(
  Mdl_Invariant_t *invariant)
{
  return (invariant -> status);
}


/**Function********************************************************************

  Synopsis           [Return the expression of an invariant]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Atm_Expr_t *
Mdl_InvariantReadExpression(
  Mdl_Invariant_t *invariant)
{
  return (invariant -> inv_expr);
}



/**Function********************************************************************

  Synopsis           [Add an state invariant to the module.]

  Description        [Add an invariant to a module. Returns TRUE if
                      successful, FALSE else.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Mdl_StateInvAddToModule(
  Mdl_Module_t *module,
  Mdl_Invariant_t *invariant)
{
  char *name;
  st_table *invtable;
  Mdl_Invariant_t *tmpinv;
  
  if (module && invariant){
    invtable = Mdl_ModuleReadStateInvTable(module);
    name = Mdl_InvariantReadName(invariant);
    if (st_lookup(invtable, name, (char **) &tmpinv)){
      FREE(name);
      return FALSE;
    }
    else {
      st_insert (invtable, util_strsav(name), (char *) invariant);
      return TRUE;
    }
  }
}


/**Function********************************************************************

  Synopsis           [Add an transition invariant to the module.]

  Description        [Add an invariant to a module. Returns TRUE if
                      successful, FALSE else.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Mdl_TranInvAddToModule(
  Mdl_Module_t *module,
  Mdl_Invariant_t *invariant)
{
  char *name;
  st_table *invtable;
  Mdl_Invariant_t *tmpinv;
  
  if (module && invariant){
    invtable = Mdl_ModuleReadTranInvTable(module);
    name = Mdl_InvariantReadName(invariant);
    if (st_lookup(invtable, name, (char **) &tmpinv)){
      FREE(name);
      return FALSE;
    }
    else {
      st_insert (invtable, util_strsav(name), (char *) invariant);
      return TRUE;
    }
  }
}

/**Function********************************************************************

  Synopsis           [Return the name of an invariant]

  Description        [The user should free the string after use.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
char *
Mdl_InvariantReadName(
  Mdl_Invariant_t * invariant)
{
  if (invariant)
    return invariant -> name;
  else
    return NIL(char);
}

/**Function********************************************************************

  Synopsis           [Obtain the invariant table from a module.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
st_table *
Mdl_ModuleReadStateInvTable(
  Mdl_Module_t *module)
{
  if (module)
    return (module -> stateInvTable);
  else
    return NIL(st_table);
}


/**Function********************************************************************

  Synopsis           [Read the transition invariant table from a module.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
st_table *
Mdl_ModuleReadTranInvTable(
  Mdl_Module_t *module)
{
  if (module)
    return (module -> tranInvTable);
  else
    return NIL(st_table);
}

/**Function********************************************************************

  Synopsis           [Allocates an module expression with Compose type.]

  Description        [This function allocates a module expression with
  the compose type.  The two composing modules are set as the left
  and right child of the module expression.]

  SideEffects        [validity of module1 and module2 are not checked. So the
  user should make sure that they are not NULL.]

  SeeAlso            [MdlExprAlloc MdlExprFree]

******************************************************************************/
Mdl_Expr_t *
Mdl_ExprComposeAlloc (
  Mdl_Expr_t *module1,
  Mdl_Expr_t *module2)
{
  Mdl_Expr_t *mexpr =  MdlExprAlloc(Mdl_ExprCompose_c);

  MdlExprSetLeftChild(mexpr, module1);
  MdlExprSetRightChild(mexpr, module2);

  return mexpr;
  
}

/**Function********************************************************************

  Synopsis           [Allocates an module expression with Rename type.]

  Description        [This function allocates a module expression with
  the Rename type. The oldVariableNameList is set to be the auxilliary
  list1, and the newVariableNameList is set to be the auxilliary list2.]
  
  SideEffects        [validity of module1 is not checked. The user should
  make sure that they are not NULL.]

  SeeAlso            [MdlExprAlloc MdlExprFree]

******************************************************************************/
Mdl_Expr_t *
Mdl_ExprRenameAlloc (
  Mdl_Expr_t *module,
  lsList oldVariableNameList,
  lsList newVariableNameList)
{
  Mdl_Expr_t *mexpr =  MdlExprAlloc(Mdl_ExprRename_c);

  MdlExprSetLeftChild(mexpr, module);
  MdlExprSetList1(mexpr, oldVariableNameList);
  MdlExprSetList2(mexpr, newVariableNameList);
  
  return mexpr;
  
}

/**Function********************************************************************

  Synopsis           [Allocates an module expression with Hide type.]

  Description        [This function allocates a module expression with
  the Hide type. The variableList is set to be the auxilliary list2.]

  SideEffects        [validity of module is not checked. The user should
  make sure that they are not NULL.]

  SeeAlso            [MdlExprAlloc MdlExprFree]

******************************************************************************/
Mdl_Expr_t *
Mdl_ExprHideAlloc (
  Mdl_Expr_t *module,
  lsList variableNameList)
{
  Mdl_Expr_t *mexpr =  MdlExprAlloc(Mdl_ExprHide_c);

  MdlExprSetLeftChild(mexpr, module);
  MdlExprSetList1(mexpr, variableNameList);

  return mexpr;
  
}


/**Function********************************************************************

  Synopsis           [Allocates an module expression with ModuleDup type.]

  Description        [This function allocates a module expression with the
  ModuleDup type.  The name of the module will be set as the left child.]

  SideEffects        [validity of module is not checked. The user should
  make sure that it is not NULL.]

  SeeAlso            [MdlExprAlloc MdlExprFree]

******************************************************************************/
Mdl_Expr_t *
Mdl_ExprModuleDupAlloc (
  char * name)
{
  Mdl_Expr_t *mexpr =  MdlExprAlloc(Mdl_ExprModuleDup_c);
  
  MdlExprSetLeftChild(mexpr, (Mdl_Expr_t *) util_strsav(name));

  return mexpr;
  
}

/**Function********************************************************************

  Synopsis           [Allocates an module expression with ModuleDef type.]

  Description        [This function allocates a module expression with the
  ModuleDef type.]

  SideEffects        []

  SeeAlso            [MdlExprAlloc MdlExprFree]

******************************************************************************/
Mdl_Expr_t *
Mdl_ExprModuleDefAlloc ()
{
  Mdl_Expr_t *mexpr =  MdlExprAlloc(Mdl_ExprModuleDef_c);

  return mexpr;
  
}

/**Function********************************************************************

  Synopsis           [Frees a module expression.]

  Description        [This function recursively frees a module expression.]

  SideEffects        [required]

  SeeAlso            [Mdl_ExprComposeAlloc Mdl_ExprRenameAlloc
  Mdl_ExprHideAlloc Mdl_ExprModuleDupAlloc MdlExprAlloc]

******************************************************************************/
void
Mdl_ExprFree(
  Mdl_Expr_t * mexpr)
{
  if (mexpr != NIL(Mdl_Expr_t)){
    Mdl_ExprType type = Mdl_ExprReadModuleExprType(mexpr);
    
    Mdl_ExprFree(mexpr-> right);
    if (type == Mdl_ExprModuleDup_c){
      free((char *) mexpr->left);
    } else if (type == Mdl_ExprModuleDef_c) {
      if (mexpr->list1) 
        lsDestroy(mexpr->list1, 0);
    } else {
      Mdl_ExprFree(mexpr-> left);
      if (mexpr-> list1) {
        lsDestroy (mexpr->list1, free);
      }
      if (mexpr-> list2) {
        lsDestroy (mexpr->list2, free);
      }
    }
    FREE(mexpr);
  }
  
}

/**Function********************************************************************

  Synopsis           [Read the moduleExprType of the module expression]

  Description        [This function returns a pointer to the moduleExprType of
  the module expression. ]

  SideEffects        [It is an error to call this function with a NULL module
  expression.]

  SeeAlso            [Mdl_ExprReadList1]

******************************************************************************/
Mdl_ExprType
Mdl_ExprReadModuleExprType(
  Mdl_Expr_t *mexpr)
{
  return mexpr->moduleExprType;
}


/**Function********************************************************************

  Synopsis           [Read the left child of the module expression]

  Description        [This function returns a pointer to the left child of the
  module expression. ]

  SideEffects        [It is an error to call this function with a NULL module
  expression.]

  SeeAlso            [Mdl_ExprReadRightChild]

******************************************************************************/
Mdl_Expr_t *
Mdl_ExprReadLeftChild(
  Mdl_Expr_t *mexpr)
{
  return mexpr->left;
}

/**Function********************************************************************

  Synopsis           [Read the right child of the module expression]

  Description        [This function returns a pointer to the right child of the
  module expression. ]

  SideEffects        [It is an error to call this function with a NULL module
  expression.]

  SeeAlso            [Mdl_ExprReadLeftChild]

******************************************************************************/
Mdl_Expr_t *
Mdl_ExprReadRightChild(
  Mdl_Expr_t *mexpr)
{
  return mexpr->right;
}

/**Function********************************************************************

  Synopsis           [Read the list1 of the module expression]

  Description        [This function returns a pointer to the list1 of the
  module expression. ]

  SideEffects        [It is an error to call this function with a NULL module
  expression.]

  SeeAlso            [Mdl_ExprReadList2]

******************************************************************************/
lsList
Mdl_ExprReadList1(
  Mdl_Expr_t *mexpr)
{
  return mexpr->list1;
}

/**Function********************************************************************

  Synopsis           [Read the list2 of the module expression]

  Description        [This function returns a pointer to the list2 of the
  module expression. ]

  SideEffects        [It is an error to call this function with a NULL module
  expression.]

  SeeAlso            [Mdl_ExprReadList1]

******************************************************************************/
lsList
Mdl_ExprReadList2(
  Mdl_Expr_t *mexpr)
{
  return mexpr->list2;
}


/* the following two functions should later be moved to mdlOper.c */
/**Function********************************************************************

  Synopsis           [Creates a new module by module operations.]

  Description        [This function evaluates a module expression and the
  result module will have the given name.]

  SideEffects        [During MdlExprEvaluate, list1 of the leaves of mexpr
  will become the list of pointers pointer to the atoms.]

  SeeAlso            [MdlExprEvaluate]

******************************************************************************/
Mdl_Module_t*
Mdl_ModuleExprEvaluate(
  char *name,
  Mdl_Expr_t *mexpr,
  Mdl_Manager_t *moduleManager)
{
  Mdl_Module_t * newmodule;
  Mdl_Module_t * module = MdlExprEvaluate(mexpr, moduleManager);
  st_table *atomMappingTable = NIL(st_table);
  st_table *varMappingTable  = NIL(st_table);

  if (module != NIL(Mdl_Module_t)) {
    newmodule = Mdl_ModuleDup(name, module, &atomMappingTable, &varMappingTable);
    MdlExprSetAtomPointers(mexpr, atomMappingTable);
    Mdl_ModuleSetModuleExpr(newmodule, mexpr);
    Mdl_ModuleFree(module);
  } else {
    newmodule = NIL(Mdl_Module_t);
  }
  
  if (atomMappingTable) st_free_table(atomMappingTable);
  if (varMappingTable) st_free_table(varMappingTable);
  
  return newmodule;
  
}



/**Function********************************************************************

  Synopsis           [Reads the leaves of an expr tree.]

  Description        [This function returns an array of pointers pointing to
  the leaves of the given expr tree. The leaves must be of types either
  ExprModuleDup of ExprModuleDef.]

  SideEffects        [The arrays should be freed, but not its content.]

  SeeAlso            [optional]

******************************************************************************/
array_t *
Mdl_ExprTreeReadComponents(
  Mdl_Expr_t * mexpr)
{
  array_t * leafArray = array_alloc(Mdl_Expr_t *, 0);

  MdlExprReadComponentsRecursively(mexpr, &leafArray);

  return leafArray;
  
}


/**Function********************************************************************

  Synopsis           [Update a module, in case of its components have changed.]

  Description        [This function first checks if all the components of the
  module are created before it.  If not, then it means one of the components
  have been updated, and the module will be composed once again, replacing the
  old module.]

  SideEffects        [The original module will be deleted and freed if any
  of the components are newer than the original module.]

  SeeAlso            [optional]

******************************************************************************/
Mdl_Module_t *
Mdl_ModuleUpdate(
  Mdl_Module_t *module,
  Mdl_Manager_t *manager)
{

  if (module == NIL(Mdl_Module_t)){
    return NIL(Mdl_Module_t);
  }
  
  if (Mdl_ModuleIsUpdate(module, manager)){
    return module;
  } else  {
    Mdl_Expr_t * mexpr = Mdl_ExprDup(Mdl_ModuleReadModuleExpr(module));
    char * name = Mdl_ModuleReadName(module);
    array_t *components = Mdl_ExprTreeReadComponents(mexpr);
    Mdl_Module_t * newModule;
    int i, numComponents = array_n (components);
    
    /* update each component */

    for (i=0; i<numComponents; i++){
      Mdl_Expr_t *componentExpr = array_fetch(Mdl_Expr_t*, components, i);
      char *componentName = (char *) Mdl_ExprReadLeftChild(componentExpr);
      Mdl_Module_t * component = Mdl_ModuleReadFromName(manager, componentName);

      (void) !Mdl_ModuleUpdate(component, manager);
    }

    array_free(components);
    newModule =  Mdl_ModuleExprEvaluate(name, mexpr, manager);
    Mdl_ModuleAddToManager(newModule, manager);
    return newModule;
  }
  

}

/**Function********************************************************************

  Synopsis           [Checks if a module is update]

  Description        [This function takes in a module and the module manager,
  and check if all the components of the module are created before this
  module.  It returns TRUE if this is the case, else FALSE.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Mdl_ModuleIsUpdate (
  Mdl_Module_t * module,
  Mdl_Manager_t *manager)
{

  Mdl_Expr_t * mexpr = Mdl_ModuleReadModuleExpr(module);
  boolean flag = TRUE;

  if (module == NIL(Mdl_Module_t))
    return FALSE;
  
  if (Mdl_ExprReadModuleExprType(mexpr) != Mdl_ExprModuleDef_c) {
    int i, numComponents;
    array_t *components = Mdl_ExprTreeReadComponents(mexpr);
    int creationTime = Mdl_ModuleReadCreationTime(module);
    
    numComponents = array_n (components);
    for (i=0; i<numComponents; i++){
      Mdl_Expr_t *componentExpr = array_fetch(Mdl_Expr_t*, components, i);
      char *componentName = (char *) Mdl_ExprReadLeftChild(componentExpr);
      Mdl_Module_t * component = Mdl_ModuleReadFromName(manager, componentName);

      if (component == NIL(Mdl_Module_t)){
        flag = FALSE;
        break;
      }
      
      if (creationTime < Mdl_ModuleReadCreationTime(component)) {
        flag = FALSE;
        break;
      }

      if (!Mdl_ModuleIsUpdate (component, manager)){
        flag = FALSE;
        break;
      }
    }

    array_free(components);
    
  }

  return flag;
  
}

/**Function********************************************************************
   
  Synopsis           [Duplicate a module expression tree.]
  
  Description        [optional]
  
  SideEffects        [required]
  
  SeeAlso            [optional]
  
******************************************************************************/
Mdl_Expr_t *
Mdl_ExprDup(
  Mdl_Expr_t *mexpr)
{
  Mdl_ExprType exprType;
  Mdl_Expr_t *newMexpr, *lexpr;
  
  
  if (mexpr == NIL(Mdl_Expr_t)){
    return NIL(Mdl_Expr_t);
  }

  exprType = Mdl_ExprReadModuleExprType(mexpr);

  if (exprType == Mdl_ExprModuleDup_c){

    newMexpr = Mdl_ExprModuleDupAlloc((char *) Mdl_ExprReadLeftChild(mexpr));
    MdlExprSetRightChild(newMexpr, Mdl_ExprDup(Mdl_ExprReadRightChild(mexpr)));
    return newMexpr;
    
  }

  if (exprType == Mdl_ExprModuleDef_c){
    lsGen lsgen;
    lsList newList = lsCreate();
    lsList list = Mdl_ExprReadList1(mexpr);
    Atm_Atom_t* atom;

    lsForEachItem(list, lsgen, atom){
      lsNewEnd(newList, (lsGeneric) atom, LS_NH);    
    }
    newMexpr = Mdl_ExprModuleDefAlloc();
    MdlExprSetList1 (newMexpr, newList);
    return newMexpr;
  }

  lexpr = Mdl_ExprDup(Mdl_ExprReadLeftChild(mexpr));
  switch(exprType) {
      case Mdl_ExprRename_c:
      {
        lsList list1 = MdlExprListDup(Mdl_ExprReadList1(mexpr));
        lsList list2 = MdlExprListDup(Mdl_ExprReadList2(mexpr));

        newMexpr = Mdl_ExprRenameAlloc(lexpr, list1, list2);
        break;
      }

      case Mdl_ExprHide_c:
      {
        lsList list1 = MdlExprListDup(Mdl_ExprReadList1(mexpr));
        newMexpr = Mdl_ExprHideAlloc(lexpr, list1);
        break;
      }

      case Mdl_ExprCompose_c:
      {
        Mdl_Expr_t * rexpr = Mdl_ExprDup(Mdl_ExprReadRightChild(mexpr));
        newMexpr = Mdl_ExprComposeAlloc (lexpr, rexpr);
        break;
      }

      default:
        fprintf(stderr,
                "Unexpected Mdl_ExprType in MdlExprConvertToString!\n");
        exit(1);
        break;
  }
  
  return newMexpr;
  
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
array_t *
Mdl_ModuleObtainComponentAtomArray(
  Mdl_Module_t * module,
  array_t * nameArray)
{
  int i;
  int numComponents = array_n(nameArray);
  array_t *componentAtomArray;
  
  if (!module || !nameArray)
    return NIL(array_t);

  componentAtomArray = array_alloc(Atm_Atom_t *, 0);
  for(i=0; i<numComponents; i++){
    char *name = array_fetch(char *, nameArray, i);
    array_t * matchArray = Mdl_ModuleSearchForComponent(name, module);
    int numMatch = array_n(matchArray);
    boolean ok = FALSE;
    
    if (numMatch == 0) {
      /* no match */
      Main_MochaErrorPrint("%s is not a component of module.\n", name);
    } else if (numMatch > 1) {
      /* more than one component with the same name */
      Main_MochaErrorPrint("Ambiguous component name: %s\n", name);
    } else {
      Mdl_Expr_t * mexpr = array_fetch(Mdl_Expr_t*, matchArray, 0);
      array_t * atomArray = Mdl_ExprReturnAtomArray(mexpr);
      int flag = array_append(componentAtomArray, atomArray);
      assert (flag);

      ok = TRUE;
      array_free(atomArray);
    }

    array_free(matchArray);

    if (!ok) {
      array_free(componentAtomArray);
      componentAtomArray=NIL(array_t);
      break;
    }
  }

  return componentAtomArray;
  
}

  
  
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
array_t *
Mdl_ModuleSearchForComponent(
  char *name,
  Mdl_Module_t * module)
{
  Mdl_Expr_t *mexpr;
  assert (name);
  assert (module);

  {
    array_t * matchArray = array_alloc(Mdl_Expr_t*, 0);
    char * moduleName = Mdl_ModuleReadName(module);

    mexpr = Mdl_ModuleReadModuleExpr(module);
    if (!strcmp(name, moduleName)) {
      array_insert_last(Mdl_Expr_t*, matchArray, mexpr);
      return matchArray;
    }
    
    if (MdlExprSearchForComponent(name, mexpr, &matchArray)) {
      return matchArray;
    } else {
      array_free(matchArray);
      return NIL(array_t);
    }
  }

  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
array_t* 
Mdl_ExprReturnAtomArray(
  Mdl_Expr_t* mexpr)
{
  array_t * atomArray = array_alloc(Atm_Atom_t*, 0);

  MdlExprReadAtomIntoArray(mexpr, &atomArray);

  return atomArray;
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Mdl_ModuleSetModuleExprForTypeModuleDef(
  Mdl_Module_t *module)
{

  if (module == NIL(Mdl_Module_t)){
    return FALSE;
  } else {
    Mdl_Expr_t * mexpr = Mdl_ExprModuleDefAlloc();
    lsList list = Mdl_ModuleObtainSortedAtomList(module);

    MdlExprSetList1(mexpr, list);
    Mdl_ModuleSetModuleExpr(module, mexpr);

    return TRUE;
  }

}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Convert a module expression to a string]

  Description        [This function converts a module expression to a string.]

  SideEffects        [The user should free the returned string.]

  SeeAlso            [optional]

******************************************************************************/
char *
MdlExprConvertToString(
  Mdl_Expr_t * mexpr)
{

  char *s, *s1;
  Mdl_ExprType exprType;

  if (mexpr == NIL(Mdl_Expr_t)){
    return util_strsav("");
  }

  exprType = Mdl_ExprReadModuleExprType(mexpr);

  if (exprType == Mdl_ExprModuleDup_c){
    return util_strsav((char *) Mdl_ExprReadLeftChild(mexpr));
  }

  if (exprType == Mdl_ExprModuleDef_c){
    return util_strsav("");
  }
  
  s1 = MdlExprConvertToString(Mdl_ExprReadLeftChild(mexpr));
  switch(exprType) {
      case Mdl_ExprRename_c:
      {
        char * tmp = util_strcat3("(", s1, ")");
        char * tmp1= MdlExprListConvertToString(Mdl_ExprReadList1(mexpr));        
        char * tmp2= MdlExprListConvertToString(Mdl_ExprReadList2(mexpr));
        char * tmp3= util_strcat3(tmp, "[", tmp1);
        s    = util_strcat4(tmp3, ":=", tmp2, "]");
        FREE(tmp);
        FREE(tmp1);
        FREE(tmp2);
        FREE(tmp3);
        break;
      }

      case Mdl_ExprHide_c:
      {
        char * tmp = MdlExprListConvertToString(Mdl_ExprReadList1(mexpr));
        char * tmp1 = util_strcat3("hide ", tmp, " in (");
        s = util_strcat3(tmp1 , s1, ") endhide");
        FREE(tmp);
        FREE(tmp1);
        break;
      }

      case Mdl_ExprCompose_c:
      {
        char * tmp = MdlExprConvertToString(Mdl_ExprReadRightChild(mexpr));
        s = util_strcat3(s1, " || ", tmp);
        FREE(tmp);
        break;
      }

      default:
        fprintf(stderr,
                "Unexpected Mdl_ExprType in MdlExprConvertToString!\n");
        exit(1);
        break;
  }

  FREE(s1);
  
  return s;
}



/**Function********************************************************************

  Synopsis           [Evaluate an module expression tree.]

  Description        [This function evaluates a module expression and
  returns the resultant module.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mdl_Module_t *
MdlExprEvaluate(
  Mdl_Expr_t *mexpr,
  Mdl_Manager_t *moduleManager)
{
  Mdl_Module_t * newmodule;
  st_table * atomMappingTable = NIL(st_table);
  st_table * varMappingTable = NIL(st_table);
  
  if (mexpr == NIL(Mdl_Expr_t)) {
    return NIL(Mdl_Module_t);
  }
  
  
  switch (Mdl_ExprReadModuleExprType(mexpr)) {
      case Mdl_ExprModuleDup_c:
      {
        char * name = (char *) Mdl_ExprReadLeftChild(mexpr);
        Mdl_Module_t * module = Mdl_ModuleReadFromName(moduleManager, name);
        
        if (module==NIL(Mdl_Module_t)){
          Main_MochaErrorPrint("Error: Module %s not defined.\n", name);
          newmodule = NIL(Mdl_Module_t);
        } else {
          char * newModuleName = Mdl_ModuleObtainNewName();
          newmodule = Mdl_ModuleDup(newModuleName, module,
                                        &atomMappingTable,
                                        &varMappingTable
                                        );

          Mdl_ExprFree(Mdl_ExprReadRightChild(mexpr));
          MdlExprSetRightChild(mexpr, Mdl_ExprDup(Mdl_ModuleReadModuleExpr(module)));
          MdlExprSetAtomPointers(mexpr, atomMappingTable);
          FREE(newModuleName);
        }
        break;
      }        

      case Mdl_ExprRename_c:
      {
        Mdl_Module_t * module = MdlExprEvaluate(Mdl_ExprReadLeftChild(mexpr), moduleManager);
        lsList designatorList = Mdl_ExprReadList1(mexpr);
        lsList destinationList = Mdl_ExprReadList2(mexpr);

        newmodule = MdlModuleRenameVariables(module, designatorList,
                                              destinationList,
                                              &atomMappingTable,
                                              &varMappingTable);

        MdlExprSetAtomPointers(mexpr, atomMappingTable);
        Mdl_ModuleFree(module);
        break;
      }
      
      case Mdl_ExprHide_c:
      {
        Mdl_Module_t * module = MdlExprEvaluate(Mdl_ExprReadLeftChild(mexpr), moduleManager);
        lsList varlist = Mdl_ExprReadList1(mexpr);
        newmodule = MdlModuleHideVariables(module, varlist, &atomMappingTable,
                                           &varMappingTable);
        MdlExprSetAtomPointers(mexpr, atomMappingTable);
        Mdl_ModuleFree(module);
        break;
      }
      
      case Mdl_ExprCompose_c:
      {
        Mdl_Module_t * module1 = MdlExprEvaluate(Mdl_ExprReadLeftChild (mexpr), moduleManager);
        Mdl_Module_t * module2 = MdlExprEvaluate(Mdl_ExprReadRightChild(mexpr), moduleManager);

        newmodule = MdlModuleCompose(module1, module2, &atomMappingTable,
                                     &varMappingTable);
        MdlExprSetAtomPointers(mexpr, atomMappingTable);
        Mdl_ModuleFree(module1);
        Mdl_ModuleFree(module2);
        break;
      }
        
      case Mdl_ExprModuleDef_c:
        fprintf(stderr, "Warning: evaluating illegal module expression\n");
        newmodule = NIL(Mdl_Module_t);
        break;

      default:
        fprintf(stderr,
                "unknown module expression type in MdlExprEvaluate!");
        exit(1);
        break;
  }

  if (atomMappingTable) st_free_table(atomMappingTable);
  if (varMappingTable)  st_free_table(varMappingTable);

  return newmodule;
  
}

/**Function********************************************************************

  Synopsis           [Update/Allocate the atom pointers in the module expr]

  Description        [This function allocates/updates the atom pointers in the
  module expression, depending on the module expression type. The atom
  pointers will be stored as a list in list1 of the structure.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
MdlExprSetAtomPointers(
  Mdl_Expr_t *mexpr,
  st_table *atomMappingTable)
{

  if (atomMappingTable!=NIL(st_table)){
    switch (Mdl_ExprReadModuleExprType(mexpr)){
        case Mdl_ExprModuleDef_c:
        {
          Atm_Atom_t * atom, *oldAtom;
          lsList atomList = Mdl_ExprReadList1(mexpr);
          lsList newAtomList = lsCreate();
          lsGen lsgen;
          
          assert(atomList);

          lsForEachItem(atomList, lsgen, oldAtom){
            boolean flag;
            
            flag = st_lookup(atomMappingTable,
                             (char *) oldAtom, (char **) &atom);
            assert(flag);
            lsNewEnd(newAtomList, (lsGeneric) atom, LS_NH);
          }
          lsDestroy(atomList, 0);
          
          MdlExprSetList1(mexpr, newAtomList);
          break;
        }
        
        case Mdl_ExprRename_c:
          MdlExprSetAtomPointers(Mdl_ExprReadLeftChild(mexpr), atomMappingTable);
          break;

        case Mdl_ExprCompose_c:
          MdlExprSetAtomPointers(Mdl_ExprReadLeftChild (mexpr), atomMappingTable);
          MdlExprSetAtomPointers(Mdl_ExprReadRightChild(mexpr), atomMappingTable);
          break;

        case Mdl_ExprModuleDup_c:
          MdlExprSetAtomPointers(Mdl_ExprReadRightChild(mexpr), atomMappingTable);
          break;

        case Mdl_ExprHide_c:
          MdlExprSetAtomPointers(Mdl_ExprReadLeftChild(mexpr), atomMappingTable);
          break;
          
        default:
          fprintf(stderr, "Unknown type in MdlExprSetAtomPointers!\n");
          exit(1);
    }
  }

}


/**Function********************************************************************

  Synopsis           [Allocating a module expression node.]

  Description        [This functions allocates a module expression node
  with the moduleType specified.  It is an internal function and is
  not exported to other packages.  Hence this function can only be used
  indirectly. ]

  SideEffects        [The user should free the allocated module expression.]

  SeeAlso            [Mdl_ExprComposeAlloc Mdl_ExprRenameAlloc
  Mdl_ExprHideAlloc Mdl_ExprModuleDupAlloc Mdl_ExpFree]

******************************************************************************/
Mdl_Expr_t *
MdlExprAlloc(
  Mdl_ExprType moduleExprType)
{
  Mdl_Expr_t *mexpr;

  mexpr = ALLOC(Mdl_Expr_t, 1);
  mexpr-> moduleExprType = moduleExprType;
  mexpr-> left           = NIL(Mdl_Expr_t);
  mexpr-> right          = NIL(Mdl_Expr_t);
  mexpr-> list1          = NULL;
  mexpr-> list2          = NULL;

  return mexpr;
}

/**Function********************************************************************

  Synopsis           [Setting the left child of the module expression]

  Description        [This function sets the left child of the module.]

  SideEffects        [It is an error to call this function with a NULL module
  expression.]

  SeeAlso            [MdlExprSetRightChild]

******************************************************************************/
void
MdlExprSetLeftChild(
  Mdl_Expr_t * mexpr,
  Mdl_Expr_t * child)
{
  mexpr -> left = child;
}

/**Function********************************************************************

  Synopsis           [Setting the right child of the module expression]

  Description        [This function sets the right child of the module.]

  SideEffects        [It is an error to call this function with a NULL module
  expression.]

  SeeAlso            [MdlExprSetLeftChild]

******************************************************************************/
void
MdlExprSetRightChild(
  Mdl_Expr_t * mexpr,
  Mdl_Expr_t * child)
{
  mexpr -> right = child;
}

/**Function********************************************************************

  Synopsis           [Setting the auxillary list1]

  Description        [This function sets the auxillary list1 of the module.]

  SideEffects        [It is an error to call this function with a NULL module
  expression.]

  SeeAlso            [MdlExprSetList2]

******************************************************************************/
void
MdlExprSetList1(
  Mdl_Expr_t * mexpr,
  lsList list)
{
  mexpr -> list1 = list;
}

/**Function********************************************************************

  Synopsis           [Setting the auxillary list2]

  Description        [This function sets the auxillary list2 of the module.]

  SideEffects        [It is an error to call this function with a NULL module
  expression.]

  SeeAlso            [MdlExprSetList1]

******************************************************************************/
void
MdlExprSetList2(
  Mdl_Expr_t * mexpr,
  lsList list)
{
  mexpr -> list2 = list;
}



/**Function********************************************************************

  Synopsis    [Copy the variables of a module.]

  Description [When called, this]

  SideEffects []

******************************************************************************/

st_table * 
MdlVariablesCopyToNewModule(
  Mdl_Module_t* newmodule,
  Mdl_Module_t* module2)
{ 
  Var_Variable_t* oldvar;
  st_generator* gen;
  st_table * variableMappingTable = st_init_table(st_ptrcmp, st_ptrhash);
  st_table* oldvartable = Mdl_ModuleReadVariableTable(module2);
  st_table* newvartable = Mdl_ModuleReadVariableTable(newmodule);
  char *name;
  boolean flag = TRUE;
  
  st_foreach_item(oldvartable, gen, &name, (char**) &oldvar){
    int peid = Var_VariableReadPEID(oldvar);
    Var_Variable_t * newvar;
        
    if(!st_lookup(newvartable, name, (char**) &newvar)
       || peid == 0 || peid == 3){
      char *newname;
      
      if (peid == 0 || peid == 3){
        newname = util_strcat3(Mdl_ModuleReadName(newmodule), "/", name);
      }
      else {
        newname = util_strsav(name);
      }
      
      newvar = Var_VariableAlloc((VarModuleGeneric) newmodule, 
                                 newname, Var_VariableReadType(oldvar));
      
      Var_VariableSetPEID(newvar, peid);

      assert(!st_is_member(newvartable, newname));
      
      st_insert(newvartable, newname, (char*) newvar);
            
    }
    
    else {
      int oldpeid = Var_VariableReadPEID(oldvar);
      int newpeid = Var_VariableReadPEID(newvar);

      /* first check if the variables are of the same type */
      if (Var_VariableReadType(oldvar) != Var_VariableReadType(newvar)) {
        Main_MochaErrorPrint(
          "Ambiguous type for variable %s. Type mismatch in composition.\n", name);
        flag = FALSE;
      }
      else if(oldpeid == 2 && newpeid == 1){ /*interface vs external -> interface */
        Var_VariableSetPEID(newvar, 2);
      }
      else if (oldpeid == 1 && newpeid == 2){/*external vs interface-> interface */
        Var_VariableSetPEID(newvar, 2);
      }
      else if (oldpeid == 1 && newpeid == 1){/*external vs external-> external */
        Var_VariableSetPEID(newvar, 1);
      }
      else if (oldpeid == 2 && newpeid == 2){/*interface vs interface -> error */
        Main_MochaErrorPrint(
          "Error: %s is interface variable to more than one modules.\n",
          name);
        flag = FALSE;
      }
    }
    st_insert(variableMappingTable, (char *) oldvar, (char *) newvar);
  }

  if (!flag) {
    st_free_table(variableMappingTable);
    variableMappingTable = NIL(st_table);
  }

  return variableMappingTable;
}
 
 

/**Function********************************************************************

  Synopsis    [Copy the atoms of a module.]

  Description []
  
  SideEffects []

******************************************************************************/
      
st_table *
MdlAtomsCopyToNewModule(
  Mdl_Module_t* newmodule,
  Mdl_Module_t* oldmodule,
  st_table* variableMappingTable)
{
  char* key;
  Atm_Atom_t* oldatom, *newatom;
  st_generator* gen;
  char* name, * newModName;
  st_table* newatmtable = Mdl_ModuleReadAtomTable(newmodule);
  st_table* oldatmtable = Mdl_ModuleReadAtomTable(oldmodule);
  st_table *atomMappingTable = st_init_table(st_ptrcmp, st_ptrhash);
  
  newModName = Mdl_ModuleReadName(newmodule);
  
  st_foreach_item(oldatmtable, gen, &key, (char**) &oldatom){
    name = Mdl_NamePrefixWithString(newModName, key);
      
    newatom = Atm_AtomDup(oldatom, name, variableMappingTable);
    st_insert(newatmtable, name, (char*) newatom);
    st_insert(atomMappingTable, (char *) oldatom, (char *) newatom);
  }

  return atomMappingTable;
  
}


/**Function********************************************************************

  Synopsis    [Renames the private variables to the standard.]

  Description [All private variable names are prefixed with a $ sign, followed
  by an underscore and a counter. The counter is used to distinguish private
  variables of the same name.  For example, if x is declared to be the
  private variables in a module, it will be automatically renamed to be
  $x_0.  In subsequent compositions, the variable will be renamed to be
  $x_0, or $x_1 etc.]

  SideEffects []

******************************************************************************/
void
MdlVariablesWrapUp(
  Mdl_Module_t* module)
{ 

  Var_Variable_t * var;
  st_generator * gen;
  char *name, *firstName, *middleName, *lastName, *counterString;
  char *newName, *newVarName;
  int counter, i;
  st_table * varTable = Mdl_ModuleReadVariableTable(module);
  st_table * counterTable = st_init_table(strcmp, st_strhash);
  lsList varList = lsCreate();
  lsGen lsgen;
  
  /* collapse variable names */
  /* obtain all variables */
  st_foreach_item(varTable, gen, &name, (char **) &var) {
    i = Var_VariableReadPEID(var);
    
    if (i == 0 || i == 3) {
      lsNewEnd(varList, (lsGeneric) var, 0);
    }
  }
  
  lsForEachItem(varList, lsgen, var) {
    char * tmpName;

    name = Var_VariableReadName(var);
    
    MdlNameSplit(name,
                 &firstName, &middleName, &lastName, &counter);
    newName = util_strcat3(firstName, lastName, "");
    tmpName = newName;
    if (!st_delete(counterTable, &tmpName,  (char **) (long *) &counter)){
      counter = 0;
    } else {
      FREE(tmpName);
    }
    
    st_insert (counterTable, newName, (char *) (long) (counter+1));

    if (counter != 0) {
      counterString = Main_ConvertIntToString(counter);
      newVarName = util_strcat3(newName, "-", counterString);
      FREE(counterString);
    } else {
      newVarName = util_strsav(newName);
    }
    
    Mdl_VariableRename(varTable, name, newVarName);

    FREE(newVarName);
    FREE(firstName);
    FREE(middleName);
    FREE(lastName);
  }
  
  

  st_foreach_item(counterTable, gen, &name, NIL(char*)) {
    FREE(name);
  }

  st_free_table(counterTable);
  lsDestroy(varList, 0);

}

/**Function********************************************************************

  Synopsis           [adjust the atom names.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
MdlAtomsWrapUp(
  Mdl_Module_t *module)
{
  Atm_Atom_t * atom;
  st_generator* gen;
  char *name;
  st_table * atmTable = Mdl_ModuleReadAtomTable(module);
  st_table * counterTable = st_init_table(strcmp, st_strhash);
  st_table * tmpAtmTable = st_init_table (strcmp, st_strhash);
  
  /* collapse atom names */
  st_foreach_item(atmTable, gen, &name, (char**) &atom) {
    char *firstName, *middleName, *lastName, *counterString;
    char *newName, *newName1, *newAtmName;
    int counter;
    
    MdlNameSplit(name, &firstName, &middleName, &lastName, &counter);
    
    newName = util_strcat3(firstName, lastName, "");
    newName1 = newName;
    if (!st_delete(counterTable, &newName1,  (char **) (long *) &counter)){
      counter = 0;
    } else {
      FREE(newName1);
    }

    st_insert (counterTable, newName, (char *) (long) (counter+1));    

    if (counter != 0) {
      counterString = Main_ConvertIntToString(counter);
      newAtmName = util_strcat3(newName, "-", counterString);
      FREE(counterString);
    } else {
      newAtmName = util_strsav(newName);
    }
    
    st_delete(atmTable, &name, (char**) &atom);
    FREE(name);

    Atm_AtomChangeName(atom, newAtmName);

    st_insert(tmpAtmTable, newAtmName, (char*) atom);

    FREE(firstName);
    FREE(middleName);
    FREE(lastName);

  }

  assert(st_count(atmTable) == 0);
  
  st_foreach_item(tmpAtmTable, gen, &name, (char**) &atom) {
    st_insert(atmTable, name, (char*) atom);
  }

  st_free_table(tmpAtmTable);

  st_foreach_item(counterTable, gen, &name, NIL(char*)) {
    FREE(name);
  }

  st_free_table(counterTable);

}


/**Function********************************************************************

  Synopsis           [Reset Module Counter.]

  Description        [optional]

  SideEffects        [required]

******************************************************************************/
void
MdlModuleCounterReset()
{
  MdlCounter = 0;
}
  				   

/**Function********************************************************************

  Synopsis           [Concatenates a list of strings to a single string.]

  Description        [This function concatenates a list of strings to a single
  string. The user should free the returned string.]

  SideEffects        [User has to make sure that the elements of the list are
  proper character strings.]

  SeeAlso            [optional]

******************************************************************************/
char *
MdlExprListConvertToString(
  lsList list)
{
  lsGen lsgen;
  boolean flag = FALSE;
  char * name;
  char * tmp = util_strsav("");
  char * s = tmp;
  
  lsForEachItem(list, lsgen, name){
    if (flag) {
      s = util_strcat3(tmp, ", ", name);
    } else {
      s = util_strsav(name);
      flag = TRUE;
    }
    
    FREE(tmp);
    tmp = s;
  }

  return s;
  
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
MdlExprReadComponentsRecursively(
  Mdl_Expr_t * mexpr,
  array_t ** leafArray)
{
  switch (Mdl_ExprReadModuleExprType(mexpr)){
    
      case Mdl_ExprModuleDup_c:    /* Duplication of an existing module */
      case Mdl_ExprModuleDef_c:    /* Explicit definition of the module */
        array_insert_last(Mdl_Expr_t *,
                          *leafArray,
                          mexpr);
        break;
        
      case Mdl_ExprRename_c:      /* variable renaming */
      case Mdl_ExprHide_c:        /* variable hiding */
        MdlExprReadComponentsRecursively(Mdl_ExprReadLeftChild(mexpr),
                                         leafArray);
        break;

      case Mdl_ExprCompose_c:     /* parallel composition of modules */
        MdlExprReadComponentsRecursively(Mdl_ExprReadLeftChild(mexpr),
                                         leafArray);
        MdlExprReadComponentsRecursively(Mdl_ExprReadRightChild(mexpr),
                                         leafArray);
        break;

      default:
        fprintf(stderr, "unknown type in MdlExprReadComponentsRecursively\n");
        exit(1);
  }
  
}

/**Function********************************************************************

  Synopsis           [Updates the readByAtomList of the variables.]

  Description        [This function is called when making a copy of the modules.
                      When given two varaibles, newvar and oldvar, as well
		      as the atom table (newatmtable) corresponding to newvar,
		      this function identifies the list of atoms in the
		      readByAtomList inn oldvar, and correspondingly 
		      updates the readByAtomList in newvar.]

  SideEffects        [required]

******************************************************************************/
static void
MdlVariableCopyVariableReadbyAtomList(
  char* modname,
  Var_Variable_t* newvar, 
  Var_Variable_t* oldvar, 
  st_table* newatmtable)
{
  lsList newlist = Var_VariableReadReadList(newvar);
  lsList oldlist = Var_VariableReadReadList(oldvar);
  lsGen  gen;
  Atm_Atom_t* oldatom;
  Atm_Atom_t* newatom;
  
  lsForEachItem(oldlist, gen, oldatom){
    char* sname;
    char* tmpname;
    char* oldatomname;

    oldatomname = Atm_AtomReadName(oldatom);
    if (oldatomname == NULL){
      tmpname = Atm_AtomNewName();
      sname = Mdl_NamePrefixWithString(modname, tmpname);
      FREE(tmpname);
    }
    else{
      sname = util_strcat3(modname, oldatomname, "");
    }
    
    if(st_lookup(newatmtable, sname, (char**) &newatom))
      lsNewEnd(newlist, (lsGeneric) newatom, LS_NH);
    
    FREE(sname);
  }
  
}


/**Function********************************************************************

  Synopsis           [Updates the awaitByAtomList of the variables.]

  Description        [This function is called when making a copy of the modules.
                      When given two varaibles, newvar and oldvar, as well
		      as the atom table (newatmtable) corresponding to newvar,
		      this function identifies the list of atoms in the
		      awaitByAtomList inn oldvar, and correspondingly 
		      updates the awaitByAtomList in newvar.]

  SideEffects        [required]

******************************************************************************/
static void
MdlVariableCopyVariableAwaitedbyAtomList(
  char* modname,
  Var_Variable_t* newvar,
  Var_Variable_t* oldvar,
  st_table* newatmtable)
{
  lsList newlist = Var_VariableReadAwaitList(newvar);
  lsList oldlist = Var_VariableReadAwaitList(oldvar);
  lsGen  gen;
  VarAtomGeneric oldatom;
  VarAtomGeneric newatom;
  
  lsForEachItem(oldlist, gen, oldatom){
    char* tmpname;
    char* sname;
    char* oldatomname;

    oldatomname = Atm_AtomReadName((Atm_Atom_t*) oldatom);
    
    if (oldatomname == NULL){
      tmpname = Atm_AtomNewName();
      sname = Mdl_NamePrefixWithString(modname, tmpname);
      FREE(tmpname);
    }
    else
      sname = util_strcat3(modname, oldatomname, "");

    if(st_lookup(newatmtable, sname, (char**) &newatom))
      lsNewEnd(newlist, (lsGeneric) newatom, LS_NH);
    
    FREE(sname);
  }
  
}


/**Function********************************************************************

  Synopsis           [Split a name into four parts]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
MdlNameSplit(
  char *name,
  char **firstName,
  char **middleName,
  char **lastName,
  int *ctr)
{
  int i = 0;
  int start, middle, end, firstRange, secondRange, len;
  char *tmpname;
  char *fname;
  char *mname;
  char *lname;
  boolean flag = TRUE;

  if (name == NIL(char)) {
    return FALSE;
  }

  tmpname = util_strsav(name);
  len = strlen(tmpname);
  
  i = len - 1;
  while(*(tmpname+i) != '@' && i> 0) i--;
  while(*(tmpname+i) != '/' && i<len) i++;
  assert (i<len);

  middle = i+1;

  i=0;
  while(*(tmpname+i) != '@' && i<len) i++;
  if (i==len) {
    start=middle;
    flag = FALSE;
  } else {
    start = i;
  }
  
  i = len - 1;
  while(*(tmpname+i) != '-' && i > 0) i--;
  end = i;
  if (end == 0)
    end = len;

  firstRange  = middle - start;
  secondRange = end-middle;
  
  fname = ALLOC(char, start + 1);
  memcpy (fname, tmpname, start);
  *(fname+start) = '\0';

  mname= ALLOC(char, firstRange+1);
  memcpy (mname, tmpname+start, firstRange);
  *(mname+firstRange) = '\0';
  
  lname = ALLOC(char, secondRange+1);
  memcpy (lname, tmpname+middle, secondRange);
  *(lname+secondRange) = '\0';
  

  if (end < len-2)
    *ctr = atoi (tmpname +end+1);
  else
    *ctr = 0;
 
  
  FREE(tmpname);

  *firstName = fname;
  *middleName = mname;
  *lastName = lname;

  return flag;
  
}



/**Function********************************************************************

  Synopsis           [Check if there is any name conflict in the atoms.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
MdlAtomNameCheckConflict(
  Mdl_Module_t *newmodule,
  char ** name1,
  char ** name2)
{
  st_table * atmTable = Mdl_ModuleReadAtomTable(newmodule);
  boolean flag = FALSE;
  char *name, *firstName, *middleName, *lastName;
  char *middleName1 = NIL(char), *middleName2=NIL(char);
  int ctr;
  Atm_Atom_t * atom;
  st_generator * gen;
    
  st_foreach_item(atmTable, gen, &name, (char**) &atom) {

    if(!MdlNameSplit(name, &firstName, &middleName, &lastName, &ctr)){
      flag = FALSE;
      st_free_gen(gen);
      break;
    }
    
    
    if (middleName1 == NIL(char)){
      middleName1 = util_strsav(middleName);
    } else if (strcmp (middleName1, middleName)){
      middleName2 = util_strsav(middleName);
    }
    

    if (middleName1 != NIL(char) && middleName2 != NIL(char)){
      char * intString = ALLOC(char, INDEX_STRING_LENGTH);
      char * tmpName1, * tmpName2;
      Atm_Atom_t * dummyatm;

      sprintf(intString, "%d", ctr);
      tmpName1 = util_strcat3(firstName, middleName1, lastName);
      tmpName2 = util_strcat3(tmpName1, "-", intString);
      *name1=middleName1;
      *name2=middleName2;
            
      if (st_lookup(atmTable, tmpName2, (char **) &dummyatm)) {
        flag = TRUE;
      }
      
      if (ctr == 0){
        if (st_lookup(atmTable, tmpName1, (char **) &dummyatm)) {
          flag = TRUE;
        }
      }
      
      FREE(firstName);
      FREE(middleName);
      FREE(lastName);
      FREE(tmpName1);
      FREE(tmpName2);
      FREE(intString);
      st_free_gen(gen);
      break;
    }
      
    FREE(middleName);
    FREE(firstName);
    FREE(lastName);
  }
  
  return flag;

}



/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static lsList
MdlExprListDup(
  lsList list)
{
  lsGen lsgen;
  char *name;
  lsList newList = lsCreate();

  lsForEachItem(list, lsgen, name){

    lsNewEnd(newList, (lsGeneric) util_strsav(name), LS_NH);    
  }

  return newList;

}

/**Function********************************************************************

  Synopsis           [Returns an array of matched modules.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean 
MdlExprSearchForComponent(
  char *name,
  Mdl_Expr_t *mexpr,
  array_t ** matchArray)
{
  Mdl_ExprType type;

  type = Mdl_ExprReadModuleExprType(mexpr);

  switch (type) {
      case Mdl_ExprModuleDup_c:
        if(!strcmp(name, (char *) Mdl_ExprReadLeftChild(mexpr))) {
          array_insert_last(Mdl_Expr_t *, *matchArray, mexpr);
        } else {
          MdlExprSearchForComponent(name, Mdl_ExprReadRightChild(mexpr), matchArray);
        }
        break;
        
      case Mdl_ExprRename_c:
      case Mdl_ExprHide_c:
        MdlExprSearchForComponent(name,  Mdl_ExprReadLeftChild(mexpr), matchArray);
        break;

      case Mdl_ExprCompose_c:
        MdlExprSearchForComponent(name,  Mdl_ExprReadLeftChild(mexpr), matchArray);
        MdlExprSearchForComponent(name, Mdl_ExprReadRightChild(mexpr), matchArray);
        break;
        
      case Mdl_ExprModuleDef_c:
        /* check if the agent is an atom */
        break;

      default:
        fprintf(stderr, "Unexpected Mdl_ExprType in MdlExprSearchForComponent");
        break;
  }

  return TRUE;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
MdlExprReadAtomIntoArray(
  Mdl_Expr_t* mexpr,
  array_t **atomArray)
{

  Mdl_ExprType type;

  type = Mdl_ExprReadModuleExprType(mexpr);
  
  switch (type) {
      case Mdl_ExprModuleDup_c:
        MdlExprReadAtomIntoArray(Mdl_ExprReadRightChild(mexpr), atomArray);
        break;
        
      case Mdl_ExprRename_c:
      case Mdl_ExprHide_c:
        MdlExprReadAtomIntoArray(Mdl_ExprReadLeftChild(mexpr), atomArray);
        break;

      case Mdl_ExprCompose_c:
        MdlExprReadAtomIntoArray(Mdl_ExprReadLeftChild(mexpr), atomArray);
        MdlExprReadAtomIntoArray(Mdl_ExprReadRightChild(mexpr), atomArray);
        break;
        
      case Mdl_ExprModuleDef_c: 
      {
        lsGen gen;
        Atm_Atom_t * atom;
        lsList atomList = Mdl_ExprReadList1(mexpr);

        lsForEachItem(atomList, gen, atom){
          array_insert_last(Atm_Atom_t*, *atomArray, atom);

        }
        break;
      }

      default:
        fprintf(stderr, "Unexpected Mdl_ExprType in MdlExprSearchForComponent");
        break;
  }
  
}


