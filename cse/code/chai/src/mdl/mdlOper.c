/**CFile***********************************************************************

  FileName    [mdlOper.c]

  PackageName [mdl]

  Synopsis    [routines for performing mdl operations.]

  Description [routines for performing mdl operations including hiding,
               parallel composition and renaming are found here.]

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


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static boolean CheckVarIsInListRecursively(Var_Variable_t *var, lsList varList);
static void VarObtainComponentsRecursively(Var_Variable_t *var, lsList varList);
static void VarNameObtainComponents(char *name, char **name1, char **name2);
static int MdlDFSOnAtoms(Atm_Atom_t* atom, int* preorder, int* postorder);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
      
/**Function********************************************************************

  Synopsis           [Change the name of a variable, and update the vartable.]

  Description        [Given a vartable, a variable and a new name, this function
                      remove the entry from the vartable, updates the name, and
		      insert it again into the vartable.  It returns 1 if
		      successful, and 0 otherwise.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
Mdl_VariableRename(
  st_table* vartable,
  char* oldname,
  char* newname)
{
  Var_Variable_t* var;
  if(!st_lookup(vartable, newname, (char**) &var)){
    if(st_delete(vartable, &oldname, (char**) &var)){
      Var_VariableChangeName(var, newname);
      FREE(oldname);
      if(!st_insert(vartable, util_strsav(newname), (char*) var)){
	return 1;
      }
    }
  }
  
  return 0;
    
}




/**Function********************************************************************

  Synopsis    [Acyclicity Test.]

  Description [This function is called when never a module is composed:
               whether it is defined by enumerating the atoms, or by parallel
	       composition.  This routine returns 1 if there is no
	       interdependence of variables within a module, and 0 otherwise.

	       Acyclicity test is primarily done by looking at the dependence
	       of atoms and variables in a DFS-like manner. ]

  SideEffects []

  SeeAlso     [MdlDFSOnAtoms]
******************************************************************************/

int
Mdl_ModuleTestForAcyclicity(
  Mdl_Module_t* module)
{
  st_generator* gen;
  Atm_Atom_t* atm;
  char* key;
  int preorder = 0;
  int postorder = 0;
  st_table* atmtable;

  /* a NIL module would not violate the acyclicity criteria */
  if (module == NIL(Mdl_Module_t)) return 1;
  
  /* reset pre/postorder of all atoms */
  atmtable = Mdl_ModuleReadAtomTable(module);
  st_foreach_item(atmtable, gen, &key, (char**) &atm) {
    Atm_AtomSetPreorder(atm, -1);
    Atm_AtomSetPostorder(atm, -1);    
  }
  
  /* Ok, let's get down to business */
  
  st_foreach_item(atmtable, gen, &key, (char**) &atm){
    if(Atm_AtomReadPreorder(atm) < 0){
      if(MdlDFSOnAtoms(atm, &preorder, &postorder) == 0) return 0; 
    };
  };

  return 1;
}

/**Function********************************************************************

  Synopsis           [Returns a list of topologically sorted atoms.]

  Description        [This function, when given module, will return a
                      list of topologically sorted atoms.  The atoms
		      at the head of the list are those that should
		      be updated first.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
lsList
Mdl_ModuleObtainSortedAtomList(
  Mdl_Module_t* module)
{
  lsList list = lsCreate();

  if (module == NIL(Mdl_Module_t)) return list;
  else {
    
    lsGen lsgen;
    Atm_Atom_t* atm;
    Atm_Atom_t* atm1;
    char* key;
    st_table* atmtable = Mdl_ModuleReadAtomTable(module);
    lsHandle* dummyhandle = NIL(lsHandle);
    int done = 0;
    st_generator* gen;
    int postorder;
    
    st_foreach_item (atmtable, gen, &key, (char**) &atm){
      lsgen = lsStart(list);
      postorder = Atm_AtomReadPostorder(atm);
      while ((lsNext(lsgen, (lsGeneric*) &atm1, dummyhandle) == LS_OK) &&
             !done){
        if (Atm_AtomReadPostorder(atm1) > postorder) {
          lsPrev(lsgen, (lsGeneric*) &atm1, dummyhandle);
          lsInBefore(lsgen, (lsGeneric) atm, NIL(lsHandle));
	  done = 1;
	}
      }
      if (!done)
	lsNewEnd(list, (lsGeneric) atm, LS_NH);
      
      lsFinish(lsgen);
      done = 0;
    }
  }
  
  return list;
  
}

/**Function********************************************************************
   
  Synopsis           [Returns a list of external variables given a module.]

  Description        [User should free the list with the function:
                      (void) lsDestroy(list, (void (*) ()) NULL);
		      so as NOT to free the variables themselves.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
lsList
Mdl_ModuleGetExternalVariableList(
  Mdl_Module_t* module
  )
{
  st_generator* gen;
  char* key;
  Var_Variable_t* var;
  lsList list = lsCreate();
  Var_Type_t * type;
  
  if (module != NIL(Mdl_Module_t)) {
    st_foreach_item(module->varTable, gen, &key, (char**) &var) {
      type = Var_VariableReadType(var);
      
      if (!(Var_TypeIsArray(type) || Var_TypeIsBV(type))) {
        if(Var_VariableIsExternal(var)) /* external variable? */
          lsNewEnd(list, (lsGeneric) var, NIL(lsHandle));
      }
    }
  }
  
  return list;
}

/**Function********************************************************************

  Synopsis           [Returns a list of interface variables given a module.]

  Description        [User should free the list with the function:
                      (void) lsDestroy(list, (void (*) ()) NULL);
		      so as NOT to free the variables themselves.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
lsList
Mdl_ModuleGetInterfaceVariableList(
  Mdl_Module_t* module)
{
  st_generator* gen;
  char* key;
  Var_Variable_t* var;
  lsList list = lsCreate();
  Var_Type_t * type;
  
  if (module != NIL(Mdl_Module_t)) {
    st_foreach_item(module->varTable, gen, &key, (char**) &var) {
      type = Var_VariableReadType(var);
      
      if (!(Var_TypeIsArray(type) || Var_TypeIsBV(type))) {
        if(Var_VariableIsInterface(var)) /* interface variable? */
          lsNewEnd(list, (lsGeneric) var, NIL(lsHandle));
      }
    }
  }
  
  return list;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mdl_Module_t *
Mdl_ModuleCreateFromAtoms(
  lsList atomList,
  lsList privateVarNameList)
{
  lsList varList;
  lsGen atomGen, varGen;
  char *moduleName, *varName, *dupVarName, *atmName;
  Atm_Atom_t *atom, *dupAtom;
  Var_Variable_t *var, *dupVar;
  st_table *dupVarTable;
  Mdl_Module_t *module, *oldModule, *composedModule, *newModule;
  int i, j;
  array_t *moduleArray = array_alloc(Mdl_Module_t *, 0);
  st_generator *stGen;
  char cmd[1024];
  int peid;
  
  lsForEachItem(atomList, atomGen, atom) {
    /* First create duplicate variables for the atom variables. */
    moduleName = Mdl_ModuleObtainNewName();
    module = Mdl_ModuleAlloc(moduleName);
    dupVarTable = st_init_table(st_ptrcmp, st_ptrhash);
    
    varList = Atm_AtomReadControlVarList(atom);

    /* I can get at the module only through the variable. */
    lsFirstItem(varList, (lsGeneric *) &var, LS_NH);
    oldModule = (Mdl_Module_t *) Var_VariableReadModule(var);
    
    lsForEachItem(varList, varGen, var) {
      dupVarName = util_strsav(Var_VariableReadName(var));
      dupVar = Var_VariableAlloc((VarModuleGeneric) module, dupVarName,
                                 Var_VariableReadType(var));
      Var_VariableSetPEID(dupVar, 2);
      Mdl_ModuleAddVariable(module, dupVarName, dupVar);
      st_insert(dupVarTable, (char *) var, (char *) dupVar);
      FREE(dupVarName);
    }
    
    varList = Atm_AtomReadAwaitVarList(atom);
    lsForEachItem(varList, varGen, var) {
      dupVarName = util_strsav(Var_VariableReadName(var));
      dupVar = Var_VariableAlloc((VarModuleGeneric) module, dupVarName,
                                 Var_VariableReadType(var));
      Var_VariableSetPEID(dupVar, 1);
      Mdl_ModuleAddVariable(module, dupVarName, dupVar);
      st_insert(dupVarTable, (char *) var, (char *) dupVar);
      FREE(dupVarName);
    }
    
    varList = Atm_AtomReadReadVarList(atom);
    lsForEachItem(varList, varGen, var) {
      if (!st_lookup(dupVarTable, (char *) var, NIL(char *))) {
        dupVarName = util_strsav(Var_VariableReadName(var));
        dupVar = Var_VariableAlloc((VarModuleGeneric) module, dupVarName,
                                   Var_VariableReadType(var));
        Var_VariableSetPEID(dupVar, 1);
        Mdl_ModuleAddVariable(module, dupVarName, dupVar);
        st_insert(dupVarTable, (char *) var, (char *) dupVar);
        FREE(dupVarName);
      }
    }

    /* Now I want to check for each array or bitvector variable in the module
       containing the original atom, whether that variable as a whole is
       controlled, read or awaited by the atom. If yes, then create a new
       variable and add the variable to the module and also to the
       dupVarTable.
       */

    Mdl_ModuleForEachVariable(oldModule, stGen, varName, var) {
      if (Var_VariableReadPEID(var) == 3) {
        /* I am copying all dummy variables of the module to save the work of */
        /* going into the expression trees of the guarded commands. */
          dupVar = Var_VariableAlloc((VarModuleGeneric) module,
                                     Var_VariableReadName(var),
                                     Var_VariableReadType(var));
          Mdl_ModuleAddVariable(module, Var_VariableReadName(var), dupVar);
          Var_VariableSetPEID(dupVar, 3);
          st_insert(dupVarTable, (char *) var, (char *) dupVar);
      }
      else {
        Var_Type_t * type = Var_VariableReadType(var);
        
        if (Var_TypeIsArray(type) || Var_TypeIsBV(type)) {
          peid = -1;
          if (CheckVarIsInListRecursively(var, Atm_AtomReadControlVarList(atom)))
            peid = 2;
          else if (CheckVarIsInListRecursively(var, Atm_AtomReadAwaitVarList(atom)) ||
                   CheckVarIsInListRecursively(var, Atm_AtomReadReadVarList(atom)))
            peid = 1;
          if (peid != -1) {
            dupVar = Var_VariableAlloc((VarModuleGeneric) module,
                                       Var_VariableReadName(var),
                                       Var_VariableReadType(var));
            Mdl_ModuleAddVariable(module, Var_VariableReadName(var), dupVar);
            Var_VariableSetPEID(dupVar, peid);
            st_insert(dupVarTable, (char *) var, (char *) dupVar);
          }
        }
      }
    }
    
    
    atmName = Atm_AtomNewName();
    dupAtom = Atm_AtomDup(atom, atmName, dupVarTable);
    Mdl_ModuleAddAtom(module, atmName, dupAtom);
    array_insert_last(Mdl_Module_t *, moduleArray, module);
    st_free_table(dupVarTable);
  }

  composedModule = array_fetch(Mdl_Module_t *, moduleArray, 0);
  
  for (i = 1; i < array_n(moduleArray); i++) {
    st_table * atomMappingTable  = NIL(st_table);
    st_table * varMappingTable  = NIL(st_table);
    
    module = array_fetch(Mdl_Module_t *, moduleArray, i);
    newModule = MdlModuleCompose(composedModule, module, &atomMappingTable, &varMappingTable);

    if (atomMappingTable) st_free_table(atomMappingTable);
    if (varMappingTable) st_free_table(varMappingTable);
    
    Mdl_ModuleFree(composedModule);
    Mdl_ModuleFree(module);

    if (newModule == NIL(Mdl_Module_t)) {
      for (j = i+1; j < array_n(moduleArray); j++) {
        module = array_fetch(Mdl_Module_t *, moduleArray, j);
        Mdl_ModuleFree(module);
      }
      array_free(moduleArray);
      return NIL(Mdl_Module_t);
    }
    
    composedModule = newModule;
  }

  array_free(moduleArray);

  /* Now make each variable in the privateVarList private. */
  {
    st_table * atomMappingTable  = NIL(st_table);
    st_table * varMappingTable  = NIL(st_table);
    
    newModule = composedModule;
    
    composedModule =
        MdlModuleHideVariables(composedModule, privateVarNameList,
                                &atomMappingTable,
                                &varMappingTable);
    
    if (atomMappingTable) st_free_table(atomMappingTable);
    if (varMappingTable) st_free_table(varMappingTable);
    
    Mdl_ModuleFree(newModule);
  }

  return composedModule;
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis    [Performs parallel composition.]

  Description [This function performs parallel composition of two modules 
               A and B.  The procedure goes on like this:
	       1. Create a copy of A.
	       2. Copy the variables/atoms of B to the copy of A.
	      ]

  SideEffects []

******************************************************************************/

Mdl_Module_t*
MdlModuleCompose(
  Mdl_Module_t* module1,
  Mdl_Module_t* module2,
  st_table ** atomMappingTable,
  st_table ** varMappingTable
)
{
  Mdl_Module_t* newmodule;

  *atomMappingTable = NIL(st_table);
  *varMappingTable  = NIL(st_table);

  if (module1 == NIL(Mdl_Module_t) || module2 == NIL(Mdl_Module_t)){
    return NIL(Mdl_Module_t);
  }
  else {
    char * name = Mdl_ModuleObtainNewName();
    st_table *varMappingTable1;
    st_table *atomMappingTable1;
    
    newmodule = Mdl_ModuleDup(name, module1,
                              atomMappingTable, varMappingTable);

    FREE(name);
    varMappingTable1 = MdlVariablesCopyToNewModule(newmodule, module2);
    
    if(!varMappingTable1){
      st_free_table(*atomMappingTable);
      st_free_table(*varMappingTable);
      
      *atomMappingTable = NIL(st_table);
      *varMappingTable  = NIL(st_table);

      Mdl_ModuleFree(newmodule);
      return NIL(Mdl_Module_t);
    }
    
    atomMappingTable1 = MdlAtomsCopyToNewModule(newmodule, module2, varMappingTable1);
    MdlVariablesWrapUp(newmodule); 

    /* detect if there is any cycle */
    if (!Mdl_ModuleTestForAcyclicity(newmodule)){
      Main_MochaErrorPrint(
        "Error:  Cycle detected in parallel composition.\n");
      st_free_table(*atomMappingTable);
      st_free_table(*varMappingTable);
      st_free_table(varMappingTable1);
      st_free_table(atomMappingTable1);
      *atomMappingTable = NIL(st_table);
      *varMappingTable  = NIL(st_table);
      Mdl_ModuleFree(newmodule);
      return NIL(Mdl_Module_t);
    }

    /* merge the varMappingTables and atmMappingTables */
    {
      Var_Variable_t *oldvar, *newvar;
      Atm_Atom_t *oldatom, *newatom;
      st_generator *gen;

      st_foreach_item(varMappingTable1, gen, (char **) &oldvar,
                      (char **) &newvar){
        assert(!st_is_member(*varMappingTable, (char *) oldvar));
        st_insert(*varMappingTable, (char *) oldvar, (char *) newvar);
      }
      
      st_foreach_item(atomMappingTable1, gen, (char **) &oldatom,
                      (char **) &newatom){
        assert(!st_is_member(*atomMappingTable, (char *) oldatom));
        st_insert(*atomMappingTable, (char *) oldatom, (char *) newatom);
      }
    }
      
    st_free_table(varMappingTable1);
    st_free_table(atomMappingTable1);
      
    return newmodule;
  }
}

/**Function********************************************************************

  Synopsis           [Hides a list of variables.]

  Description        [Given a module and a list of variable names, this function 
                      "hides" the variables by turning them into private
		      variables.  Warnings will be given if any of the variables 
		      are not defined in the module, or they are already
		      private.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mdl_Module_t *
MdlModuleHideVariables(
  Mdl_Module_t* module,
  lsList varlist,
  st_table **atomMappingTable,
  st_table **varMappingTable)
{
  lsGen lsgen;
  char* name;
  st_table* vartable;
  Var_Variable_t* var;
  char* newname;
  char* newname1;
  Var_Variable_t* tmpitem;  
  int counter = 0;
  boolean ok = FALSE;
  char *newModuleName = Mdl_ModuleObtainNewName();
  Mdl_Module_t * newModule;
  
    
  if(module == NIL(Mdl_Module_t)) {
    *atomMappingTable = NIL(st_table);
    *varMappingTable = NIL(st_table);
    return NIL(Mdl_Module_t);
  }
  

  newModule = Mdl_ModuleDup(newModuleName, module, atomMappingTable, varMappingTable);

  vartable = Mdl_ModuleReadVariableTable(newModule);
  lsForEachItem(varlist, lsgen, name){
    lsList compVarList;
    lsGen varGen;
    Var_Variable_t *compVar;
    char *compVarName;
    
    if (st_lookup(vartable, name, (char**) &var)){
      if(Var_VariableReadPEID(var) == 1) 
	Main_MochaErrorPrint("Warning: %s is external and cannot be hidden.\n", name);
      else {
        compVarList = lsCreate();
        VarObtainComponentsRecursively(var, compVarList);
        lsForEachItem(compVarList, varGen, compVar) {          
          compVarName = Var_VariableReadName(compVar);
          newname1 = util_strcat3(newModuleName, "/",
                                  compVarName);

          Var_VariableSetPEID(compVar, 0);
          Mdl_VariableRename(vartable, compVarName, newname1);
          FREE(newname1);
        }
        lsDestroy(compVarList, 0);
      }
    }
    else
      Main_MochaErrorPrint("Warning: variable %s is either private or not defined.\n", name);
  }

  FREE(newModuleName);
  return newModule;
  
}

/**Function********************************************************************

  Synopsis           [Rename the variables of a module]

  Description        [This function renames the variables in the
  designatorList to the names in the destinationList.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mdl_Module_t *
MdlModuleRenameVariables(
  Mdl_Module_t *module,
  lsList designatorList,
  lsList destinationList,
  st_table ** atomMappingTable,
  st_table ** varMappingTable)
{
  Var_Variable_t* var;
  int len, i;
  lsList varList, newnameList;
  char *designator, *newname;
  Mdl_Module_t * newModule;
  char *newModuleName;
  boolean flag = TRUE;

  if (module == NIL(Mdl_Module_t)){
    return NIL(Mdl_Module_t);
  }
  
  newModuleName = Mdl_ModuleObtainNewName();
  newModule = Mdl_ModuleDup(newModuleName, module, atomMappingTable, varMappingTable);
  FREE(newModuleName);

  /* First extract the variables which are to be renamed */
  
  varList     = lsCreate(); /* stores the variables to be renamed */
  newnameList = lsCreate(); /* stores the new names */
  
  {
    int i, len;
    lsGen desigGen, destGen, varGen;
    st_table *varTable;
    lsList compVarList;
    char *oldCompName, *newCompName;
    Var_Variable_t *compVar;
    
    len = lsLength (destinationList);
  
    desigGen = lsStart(designatorList);
    destGen  = lsStart(destinationList);
    
    varTable    = Mdl_ModuleReadVariableTable(newModule);
    for(i=0; i < len; i++) {    
      lsNext(desigGen, (lsGeneric *) &designator, LS_NH);
      lsNext(destGen,  (lsGeneric *) &newname, LS_NH);
      if (!st_lookup(varTable, designator, (char **) &var)){
        Main_MochaErrorPrint(
          "Warning in variable rename: Variable %s is either private or not defined.\n",
          designator);
      }
      else {
	 compVarList = lsCreate();
	 VarObtainComponentsRecursively(var, compVarList);
	 lsForEachItem(compVarList, varGen, compVar) {
           char *name1, *name2;
           
	    oldCompName = Var_VariableReadName(compVar);
	    st_delete(varTable, &oldCompName, NIL(char*));
            VarNameObtainComponents(oldCompName, &name1, &name2);
	    newCompName = util_strcat3(newname, name2, "");
            FREE(name1);
            FREE(name2);
	    lsNewEnd(varList, (lsGeneric) compVar,  LS_NH);
	    lsNewEnd(newnameList, (lsGeneric) newCompName,  LS_NH);
	 }
	 
	 lsDestroy(compVarList, NULL);
      }
    }
    
    /* free the list generators */
    lsFinish(desigGen);
    lsFinish(destGen);
  }

  
  /* then, do the actual renaming */
  {
    int len, i;
    lsGen varGen, newnameGen;
    
    len = lsLength(newnameList);
    
    varGen  = lsStart(varList);
    newnameGen = lsStart(newnameList);
    
    for (i=0; i<len; i++) {
      lsNext(newnameGen, (lsGeneric *) &newname, LS_NH);
      lsNext(varGen,  (lsGeneric *) &var, LS_NH);
      
      if (Mdl_ModuleReadVariableFromName(newname, newModule)) {                      
        Main_MochaErrorPrint(
          "Error in variable rename: Variable of name %s already exists.\n",
          newname);
      flag = FALSE;
      break;
      } else {
        Var_VariableChangeName(var, newname);
        Mdl_ModuleAddVariable(newModule, newname, var);
      }
    }
  
    /* free the list generators */
    lsFinish(varGen);
    lsFinish(newnameGen);
  }
  
  /* free all the auxillary data structures */
  lsDestroy(newnameList, free);
  lsDestroy(varList, (void (*) ()) 0);

  if (!flag){
    Mdl_ModuleFree(newModule);
    st_free_table(*atomMappingTable);
    st_free_table(*varMappingTable);
    *atomMappingTable  = NIL(st_table);
    *varMappingTable = NIL(st_table);
    newModule = NIL(Mdl_Module_t);
  }

  return (newModule);
  
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
void
VarNameObtainComponents(
  char *name,
  char **name1,
  char **name2)
{
  int i, j;
  
  i = 0;
  while (!(name[i] == '[' || name[i] == '\0')) {
    i++;
  }
  
  *name1 = ALLOC(char, i+1);
  strncpy(*name1, name, i);
  (*name1)[i] = '\0';
  
  j = strlen(name) - i;   
  *name2 = ALLOC(char,  j+1);
  strncpy(*name2, name+i, j);
  (*name2)[j] = '\0';   
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
VarObtainComponentsRecursively(
  Var_Variable_t *var,
  lsList varList)
{
  Var_Type_t *varType = Var_VariableReadType(var);
  char *varName = Var_VariableReadName(var);
  char *actualVarName, *tmpString;
  lsGen lsgen;
  lsGeneric item;
  Mdl_Module_t *module = (Mdl_Module_t *) Var_VariableReadModule(var);
  Var_Type_t *indexType;
  int i, indexIsEnum, size;
  Var_Variable_t *actualVar;
  
  if (!(Var_TypeIsArray(varType) || Var_TypeIsBV(varType))) {
     lsNewEnd(varList, (lsGeneric) var, LS_NH);
     return;
  }

  if (Var_TypeIsBV(varType)) {

     lsNewEnd(varList, (lsGeneric) var, LS_NH);
 
     size = Var_VarTypeArrayOrBitvectorReadSize(varType);
    
    for (i = 0; i < size; i++) {
      tmpString = ALLOC(char, INDEX_STRING_LENGTH);
      sprintf(tmpString, "%d", i);
      actualVarName = util_strcat4(varName, "[", tmpString, "]");
      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
      
      FREE(tmpString);
      FREE(actualVarName);

      VarObtainComponentsRecursively(actualVar, varList);
    }    
    return;
  }

  if (Var_TypeIsArray(varType)) {

    lsNewEnd(varList, (lsGeneric) var, LS_NH);
	  
    size = Var_VarTypeArrayOrBitvectorReadSize(varType);
    indexType = Var_VarTypeReadIndexType(varType);

    indexIsEnum = Var_TypeIsEnum(indexType);
    
    for (i = 0; i < size; i++) {
      if (indexIsEnum)
        actualVarName = util_strcat4(varName, "[", Var_EnumElementReadName(
          Var_VarTypeReadSymbolicValueFromIndex(indexType, i)), "]");
      else {
	tmpString = ALLOC(char, INDEX_STRING_LENGTH);
	sprintf(tmpString, "%d", i);
        actualVarName = util_strcat4(varName, "[", tmpString, "]");
        FREE(tmpString);
      }
      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
      FREE(actualVarName);
      
      VarObtainComponentsRecursively(actualVar, varList);
    }
    return;
  }   
   
}


/**Function********************************************************************

  Synopsis           [Checks if all components of a variable are present in a
  list of variables.]

  Description        [This function checks if all components of a variable are
  present in a list of variables. For compound variables like arrays or
  bitvectors, this check is done recursively for all entries of the array or
  bitvector.]

  SideEffects        [None]

  SeeAlso            [VariableObtainComponentsRecursively]

******************************************************************************/
static boolean
CheckVarIsInListRecursively(
  Var_Variable_t *var,
  lsList varList) 
{
  Var_Type_t *varType = Var_VariableReadType(var);
  char *varName = Var_VariableReadName(var);
  char *actualVarName, *tmpString;
  lsGen lsgen;
  lsGeneric item;
  Mdl_Module_t *module = (Mdl_Module_t *) Var_VariableReadModule(var);
  Var_Type_t *indexType;
  int i, indexIsEnum, size;
  Var_Variable_t *actualVar;
  
  
  if (!(Var_TypeIsArray(varType) || Var_TypeIsBV(varType))) {
    lsForEachItem(varList, lsgen, item) {
      if (item == (lsGeneric) var)
        return TRUE;
    }
    return FALSE;    
  }

  if (Var_TypeIsBV(varType)) {
    size = Var_VarTypeArrayOrBitvectorReadSize(varType);
    
    for (i = 0; i < size; i++) {
      tmpString = ALLOC(char, INDEX_STRING_LENGTH);
      sprintf(tmpString, "%d", i);
      actualVarName = util_strcat4(varName, "[", tmpString, "]");
      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
      
      FREE(tmpString);
      FREE(actualVarName);
      
      if (!CheckVarIsInListRecursively(actualVar, varList))
        return FALSE;
    }
    return TRUE;
  }

  if (Var_TypeIsArray(varType)) {
    size = Var_VarTypeArrayOrBitvectorReadSize(varType);
    indexType = Var_VarTypeReadIndexType(varType);
    
    indexIsEnum = Var_TypeIsEnum(indexType);
    
    for (i = 0; i < size; i++) {
      if (indexIsEnum)
        actualVarName = util_strcat4(varName, "[", Var_EnumElementReadName(
          Var_VarTypeReadSymbolicValueFromIndex(indexType, i)), "]");
      else {
	tmpString = ALLOC(char, INDEX_STRING_LENGTH);
	sprintf(tmpString, "%d", i);
        actualVarName = util_strcat4(varName, "[", tmpString, "]");
        FREE(tmpString);
      }
      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
      FREE(actualVarName);
      
      if (!CheckVarIsInListRecursively(actualVar, varList))
        return FALSE;
    }
    return TRUE;
  }
}

/**Function********************************************************************

  Synopsis    [running DFS to do reachability test]

  Description [This function is called by Mdl_ModuleTestForAcyclicity.]

  SideEffects []

******************************************************************************/
static int
MdlDFSOnAtoms(
  Atm_Atom_t* atom,
  int* preorder,
  int* postorder
)
{ Var_Variable_t* var;
  Atm_Atom_t* atom1;
  lsList awaitvarlist;
  lsGen  lsgen;
  int status=1;

  /* NIL atom does not constitute cycles */
  if (atom == NIL(Atm_Atom_t)) return 1;

  /* preorder the atom first */
  Atm_AtomSetPreorder(atom, *preorder);
  (*preorder)++;
  
  /* Read the list of variables the atom is awaiting */
  awaitvarlist = Atm_AtomReadAwaitVarList(atom);
  
  lsForEachItem(awaitvarlist, lsgen, var){
    atom1 = (Atm_Atom_t*) Var_VariableReadControlAtom(var);
    if (atom1 != NIL(Atm_Atom_t)){
      
      /* cycle if only the preorder is assigned. */
      if (Atm_AtomReadPreorder(atom1) >= 0 && Atm_AtomReadPostorder(atom1) < 0)
        return 0;
      
      if ( Atm_AtomReadPostorder(atom1) < 0) 
        /* cycle if detected here or in any of the children subtrees */
        if ( MdlDFSOnAtoms(atom1, preorder, postorder) == 0) return 0;
    }
  }
  
  /* now assign postorder to this atom */
  Atm_AtomSetPostorder(atom, *postorder);

  /* And then append this atom into the Topological list. */
  (*postorder) ++;
    
  return 1;
}

