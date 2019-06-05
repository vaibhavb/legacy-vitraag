/**CFile***********************************************************************

  FileName    [refSym1.c]

  PackageName [ref]

  Synopsis    [Routines to check if one module simulates another using naive approach]

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

  Synopsis           [Check if specModule is a simulates of implModule]

  Description        []

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Ref_CheckSimulationUsingPre(
  Mdl_Module_t * implModule,
  Sym_Info_t *implSymInfo,
  Mdl_Module_t *specModule,
  Sym_Info_t *specSymInfo,
  boolean verbose)
{
  mdd_manager *manager = Sym_SymInfoReadManager(implSymInfo);
  mdd_t *temp1, *temp2;
  boolean result = FALSE;

  mdd_t *implInitSet      =   Sym_ModuleBuildInitialRegion(implSymInfo, implModule);
  mdd_t *specInitSet      =   Sym_ModuleBuildInitialRegion(specSymInfo, specModule);
  
  array_t *allUnprimedIds =   RefUnprimedIdsMerge(implSymInfo, specSymInfo);
  array_t *allPrimedIds   =   RefPrimedIdsMerge(implSymInfo, specSymInfo);
  array_t *quantifyIds    =   array_alloc(int, 0);

  mdd_t *implNoChange =  RefMddBuildNoChange(implSymInfo);
  mdd_t *specNoChange =  RefMddBuildNoChange(specSymInfo);
  mdd_t *obsEq        =  RefMddBuildObseq(implModule,implSymInfo,specModule,specSymInfo);

  array_t *newImplConjuncts = RefConjunctsAddExtra(implSymInfo, specNoChange);
  array_t *newSpecConjuncts = RefConjunctsAddExtra(specSymInfo, implNoChange);

  Img_ImageInfo_t *specImgInfo = Img_ImageInfoInitialize(0, manager,
						     allUnprimedIds,
						     allPrimedIds,
						     quantifyIds,
						     Img_Iwls95_c,
						     newSpecConjuncts,
						     Img_Both_c);

  Img_ImageInfo_t *implImgInfo = Img_ImageInfoInitialize(0, manager,
						     allUnprimedIds,
						     allPrimedIds,
						     quantifyIds,
						     Img_Iwls95_c,
						     newImplConjuncts,
						     Img_Both_c);

  mdd_t *symRelation = mdd_one(manager);
  mdd_t *oneMdd      = mdd_one(manager);
  mdd_t *newSymRelation;
  int step = 0;
  while(1){
    mdd_t * firstImage = Img_ImageInfoComputeBwdWithDomainVars(specImgInfo, 
						       symRelation, 
						       symRelation,
						       oneMdd);
    mdd_t *notFirstImage = mdd_not(firstImage);
    
   
     mdd_t *secondImage = Img_ImageInfoComputeBwdWithDomainVars(implImgInfo, 
						       notFirstImage, 
						       notFirstImage,
						       oneMdd);

    mdd_free(firstImage);
    mdd_free(notFirstImage);
    
    newSymRelation = mdd_and(secondImage, obsEq, 0, 1);
    mdd_free(secondImage);

    step++;
    if(verbose){
      Main_MochaPrint("Simulation relation after step %d\n", step);
      Sym_MddPrintCubesImplSpec(implSymInfo, specSymInfo, newSymRelation, 0);
    }

    if(mdd_equal(symRelation, newSymRelation))
       break;
       
    mdd_free(symRelation);
    symRelation = newSymRelation;
  }

  Main_MochaPrint(" \n ******************************************************************"
		  " \n Reached fixpoint after  %d steps\n", step);

  temp1 = mdd_and(symRelation,specInitSet,1,1);
  if(verbose){
    Main_MochaPrint("Intersection of sym relation with spec init set:\n");
    Sym_MddPrintCubesImplSpec(implSymInfo, specSymInfo, temp1, 0);
  }
  temp2 = mdd_smooth(manager, temp1, Sym_SymInfoReadUnprimedIdArray(specSymInfo));
  if(verbose){
    Main_MochaPrint("After quantifying all spec variables:\n");
    Sym_MddPrintCubesImplSpec(implSymInfo, specSymInfo, temp2, 0);
  }
  mdd_free(temp1);

  temp1 = mdd_and(implInitSet, temp2, 1, 0);
  if(mdd_is_tautology(temp1, 0)){
    Main_MochaPrint("Yes: There is a simulation from %s to %s\n",
		    Mdl_ModuleReadName(implModule),
		    Mdl_ModuleReadName(specModule));
    result = TRUE;
  }
  else{
    Main_MochaPrint("No: There is NO simulation from %s to %s\n",
		    Mdl_ModuleReadName(implModule),
		    Mdl_ModuleReadName(specModule));
    result = FALSE;
  }

  mdd_free(temp2);
  mdd_free(symRelation);


  array_free(allUnprimedIds);
  array_free(allPrimedIds);
  array_free(quantifyIds);
  mdd_free(implInitSet);
  mdd_free(specInitSet);
  mdd_free(implNoChange);
  mdd_free(specNoChange);
  mdd_free(obsEq);
  
  Img_ImageInfoFree(specImgInfo);
  Img_ImageInfoFree(implImgInfo);
  RefMddArrayFree(newSpecConjuncts);
  RefMddArrayFree(newImplConjuncts);

  return(result);
}

/**Function********************************************************************

  Synopsis           [merge unprimed ids of two modules]

  Description        [merge unprimed ids of two modules and return the 
                      result in a new array. It is the caller's responsibility
		      to free this array]

  SideEffects        [none]

  SeeAlso            [RefPrimedIdsMerge]

******************************************************************************/
array_t *
RefUnprimedIdsMerge(
  Sym_Info_t *symInfo1,
  Sym_Info_t *symInfo2)
{
  array_t *result = array_alloc(int, 0);
  int i;
  array_t *u1     = Sym_SymInfoReadUnprimedIdArray(symInfo1);
  array_t *u2     = Sym_SymInfoReadUnprimedIdArray(symInfo2);

  for (i = 0; i < array_n(u1); i++){
    int id = (int) array_fetch( int, u1, i);
    array_insert_last(int, result, id);
  }

  for (i = 0; i < array_n(u2); i++){
    int id = (int) array_fetch( int, u2, i);
    array_insert_last(int, result, id);
  }

  return(result);
}

/**Function********************************************************************

  Synopsis           [merge primed ids of two modules]

  Description        [merge primed ids of two modules and return the 
                      result in a new array. It is the caller's responsibility
		      to free this array]

  SideEffects        [none]

  SeeAlso            [RefUnprimedIdsMerge]

******************************************************************************/
array_t *
RefPrimedIdsMerge(
  Sym_Info_t *symInfo1,
  Sym_Info_t *symInfo2)
{
  array_t *result = array_alloc(int, 0);
  int i;
  array_t *u1     = Sym_SymInfoReadPrimedIdArray(symInfo1);
  array_t *u2     = Sym_SymInfoReadPrimedIdArray(symInfo2);

  for (i = 0; i < array_n(u1); i++){
    int id = (int) array_fetch( int, u1, i);
    array_insert_last(int, result, id);
  }

  for (i = 0; i < array_n(u2); i++){
    int id = (int) array_fetch( int, u2, i);
    array_insert_last(int, result, id);
  }

  return(result);
}


/**Function********************************************************************

  Synopsis           [Build MDD for no variables changing]

  Description        [Build one conjunct for each variable x,
                      saying that (x' = x), and take conjunction
		      of all these]

  SideEffects        [none]

  SeeAlso            [RefMddBuildObseq]

******************************************************************************/
mdd_t *
RefMddBuildNoChange(
   Sym_Info_t *symInfo)
{
  mdd_t *result;
  mdd_manager *manager = Sym_SymInfoReadManager(symInfo);

  int i;
  array_t *u     = Sym_SymInfoReadUnprimedIdArray(symInfo);
  array_t *p     = Sym_SymInfoReadPrimedIdArray(symInfo);

  result = mdd_one(manager);

  for (i = 0; i < array_n(u); i++){
    int uid = (int) array_fetch(int, u, i);
    int pid = (int) array_fetch(int, p, i);
    
    mdd_t *conjunct = mdd_eq(manager, uid, pid);
    mdd_t *temp     = mdd_and(conjunct, result, 1, 1);

    mdd_free(conjunct);
    mdd_free(result);
    result = temp;
  }

  return(result);
}
  

/**Function********************************************************************

  Synopsis           [Build MDD for observational equivalence between two modules]

  Description        [Preconditions:
                      1. Each interface variable of the specification needs to be
                      an interface variable of the implementation.
                      2. Each external variable of the specification needs to be
                      an interface or external variable of the implementation.

		      If these conditions are met, this routine returns an MDD that 
		      forces equality for corresponding UNPRIMED variables of
		      specification and implementation.  ]

  SideEffects        []

  SeeAlso            [RefMddBuildNoChange]
******************************************************************************/
mdd_t *
RefMddBuildObseq(
  Mdl_Module_t *impl,
  Sym_Info_t *implSymInfo,
  Mdl_Module_t *spec,
  Sym_Info_t *specSymInfo)
{
  lsList  specExtVars = Mdl_ModuleGetExternalVariableList(spec);
  lsList  specIntfVars = Mdl_ModuleGetInterfaceVariableList(spec);
  lsList  implExtVars = Mdl_ModuleGetExternalVariableList(impl);
  lsList  implIntfVars = Mdl_ModuleGetInterfaceVariableList(impl);
  
  mdd_manager *manager = Sym_SymInfoReadManager(implSymInfo);
  mdd_t * result = mdd_one(manager);
  lsGeneric v;
  lsGen gen;
  
  lsForEachItem(specIntfVars, gen, v) {
    Var_Variable_t *var = (Var_Variable_t *)v;
    Var_Variable_t *ivar;
    mdd_t *conjunct, *temp;
    int uid1, uid2;
    ivar = Ref_LookupVarList( implIntfVars, Var_VariableReadName(var));
    if(!ivar){
	Main_MochaErrorPrint("Specification interface variable %s"
			     " is not found an implementation interface variable\n",
                             Var_VariableReadName(var)); 
	mdd_free(result);
	return(0);
    }
    else if(Var_VariableReadDataType(var) != Var_VariableReadDataType(ivar)){
        Main_MochaErrorPrint("Variable %s has different data types in spec and impl",
                             Var_VariableReadName(var));
	mdd_free(result);
	return(0);
    }

    
    if(!Sym_SymInfoLookupUnprimedVariableId(specSymInfo, var, &uid1)){
        Main_MochaErrorPrint("Cant get unprimed variable id for variable %s in specification",
                             Var_VariableReadName(var));
	mdd_free(result);
	return(0);
    }
    
    if (!Sym_SymInfoLookupUnprimedVariableId(implSymInfo, ivar, &uid2)){
        Main_MochaErrorPrint("Cant get unprimed variable id for variable %s in implementation",
                             Var_VariableReadName(var));
	mdd_free(result);
	return(0);
    }

    conjunct = mdd_eq(manager, uid1, uid2);
    temp     = mdd_and(conjunct, result, 1, 1);

    mdd_free(conjunct);
    mdd_free(result);
    result = temp;
  }


  lsForEachItem(specExtVars, gen, v) {
    Var_Variable_t *var = (Var_Variable_t *)v;
    Var_Variable_t *ivar;
    mdd_t *conjunct, *temp;
    int uid1, uid2;
    ivar = Ref_LookupVarList( implExtVars, Var_VariableReadName(var));
    if (!ivar)
      ivar = Ref_LookupVarList( implIntfVars, Var_VariableReadName(var));

    if(!ivar){
	Main_MochaErrorPrint("Specification interface variable %s"
			     " is not found an implementation interface variable\n",
                             Var_VariableReadName(var)); 
	mdd_free(result);
	return(0);
    }
    else if(Var_VariableReadDataType(var) != Var_VariableReadDataType(ivar)){
        Main_MochaErrorPrint("Variable %s has different data types in spec and impl",
                             Var_VariableReadName(var));
	mdd_free(result);
	return(0);
    }


    if(!Sym_SymInfoLookupUnprimedVariableId(specSymInfo, var, &uid1)){
        Main_MochaErrorPrint("Cant get unprimed variable id for variable %s in specification",
                             Var_VariableReadName(var));
	mdd_free(result);
	return(0);
    }
    
    if (!Sym_SymInfoLookupUnprimedVariableId(implSymInfo, ivar, &uid2)){
        Main_MochaErrorPrint("Cant get unprimed variable id for variable %s in implementation",
                             Var_VariableReadName(var));
	mdd_free(result);
	return(0);
    }
    conjunct = mdd_eq(manager, uid1, uid2);
    temp     = mdd_and(conjunct, result, 1, 1);

    mdd_free(conjunct);
    mdd_free(result);
    result = temp;
  }

  return(result);
}
   
/**Function********************************************************************

  Synopsis           [Add one mdd to the conjuncts array and return a new
                      conjuncts array]

  Description        [Add one mdd to the conjuncts array and return a new
                      conjuncts array. The user is responsible for freeing
		      the new array and all the mdds in the new array]

  SideEffects        [none]

  SeeAlso            [RefUnprimedIdsMerge]

******************************************************************************/
array_t *
RefConjunctsAddExtra(
 Sym_Info_t *symInfo,
 mdd_t *extra)
{
  array_t *result = array_alloc(mdd_t *, 0);
  array_t *conjuncts = Sym_SymInfoReadConjuncts(symInfo);
  int i;
  
  for ( i = 0; i < array_n(conjuncts); i++){
    mdd_t *c = (mdd_t *) array_fetch( mdd_t *, conjuncts, i);
    array_insert_last(mdd_t *, result, mdd_dup(c));
  }

  array_insert_last(mdd_t *, result, mdd_dup(extra));
  return(result);
}


/**Function********************************************************************

  Synopsis           [driver for checking simulation]

  Description        [optional]

  SideEffects        []

  SeeAlso            [optional]

  CommandName        [check_simulation] 	   

  CommandSynopsis    [Performs symbolic (mdd based) simulation check]

  CommandArguments   [ \[-h\] \[-v\]  &lt;implementation_module&gt; &lt;specification_module&gt;]    

  CommandDescription [The command performs MDD based simulation check. 

  Command Options:<p>

  <dl>

  <dt> -h
  <dd> print usage <p>

  <dt> -v
  <dd> verbose mode - prints MDDs associated with each intermediate step<p>

  </dl>]
******************************************************************************/
int
RefCheckSimulation(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char **argv)
{
  Mdl_Manager_t *mdlManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager((Main_Manager_t *) clientData);
  Sym_RegionManager_t *regionManager = (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager((Main_Manager_t *) clientData); 
  Tcl_HashTable *regionTable = Sym_RegionManagerReadRegionTable(regionManager);
  Mdl_Module_t *implModule, *specModule;
  Sym_Info_t *implSymInfo, *specSymInfo;
  boolean result;
  int retVal;
  boolean verbose = FALSE;
  boolean checkAtEnd = FALSE;
  char *fileName = NIL(char);
  int c;
  
  util_getopt_reset();

  while((c = util_getopt(argc, argv, "o:vhe")) != EOF) {
    switch(c) {
        case 'v' :
          verbose = TRUE;
          break;
       case 'e':
	 checkAtEnd = TRUE;
	 break;
       case 'o':
	 fileName = util_optarg;
	 break;
       case 'h' :
          goto usage;
        default :
          goto usage;
    }
  }

  if (argc - util_optind == 0) {
    goto usage;
  }
  
  if((implModule = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }

  if (argc - util_optind == 0) {
    goto usage;
  }

  util_optind++;
  if((specModule = Mdl_ModuleReadFromName(mdlManager, argv[util_optind])) ==
     NIL(Mdl_Module_t)) {
    Main_MochaErrorPrint( "Module %s not found", argv[util_optind]);
    return TCL_ERROR;
  }
  
  
  implSymInfo = Sym_RegionManagerReadSymInfo(regionManager, implModule);
  if(!implSymInfo){
      char cmd[512];

      if(fileName){
	Main_MochaPrint("Ordering variables from %s\n", fileName);
	sprintf(cmd, "sym_static_order -f %s %s", fileName, Mdl_ModuleReadName(implModule));
	Tcl_Eval(interp, cmd);
      }

      Main_MochaPrint("Building transition relation for module %s\n",
                      Mdl_ModuleReadName(implModule));
      sprintf(cmd, "sym_trans %s", Mdl_ModuleReadName(implModule));
      Tcl_Eval(interp, cmd);
      implSymInfo = Sym_RegionManagerReadSymInfo(regionManager, implModule);  
      if(!implSymInfo){
	Main_MochaErrorPrint( "Command: %s unsuccessful\n", cmd);
	return(TCL_ERROR);
      }

      Main_MochaPrint("Writing order into imporder.dat\n");
      sprintf(cmd, "sym_write_order -f /tmp/imporder.dat %s", Mdl_ModuleReadName(implModule));
      Tcl_Eval(interp, cmd);
  }

  specSymInfo = Sym_RegionManagerReadSymInfo(regionManager, specModule);
  if(!specSymInfo){
      char cmd[512];
      Main_MochaPrint("Building transition relation for module %s\n", Mdl_ModuleReadName(specModule));
      sprintf(cmd, "sym_trans %s", Mdl_ModuleReadName(specModule));
      Tcl_Eval(interp, cmd);
      specSymInfo = Sym_RegionManagerReadSymInfo(regionManager, specModule);  
      if(!specSymInfo){
	Main_MochaErrorPrint( "Command: %s unsuccessful\n", cmd);
	return(TCL_ERROR);
      }
  }

  {
    /* first turn on dynamic variable ordering */
    char cmd[512];
    sprintf(cmd, "sym_dynamic_var_ordering -e sift");
    Tcl_Eval(interp, cmd);
  }

  result = Ref_CheckSimulationUsingPre(implModule, implSymInfo, 
				       specModule, specSymInfo, verbose);


  if(result){
    Main_MochaPrint("%s is simulated by %s\n",
                    Mdl_ModuleReadName(implModule), Mdl_ModuleReadName(specModule));
    retVal = TCL_OK;
  }
  else{
    Main_MochaPrint("%s is NOT simulated by %s\n",
                    Mdl_ModuleReadName(implModule), Mdl_ModuleReadName(specModule));
    retVal = TCL_ERROR;
  }


  Main_MochaPrint("Freeing syminfo for both specification and implementation\n");
  Sym_RegionManagerDeleteModuleToSymInfo(regionManager, implModule);
  Sym_RegionManagerDeleteModuleToSymInfo(regionManager, specModule);
  Sym_SymInfoFree(implSymInfo);
  Sym_SymInfoFree(specSymInfo);

  return(retVal);

usage:
  Main_MochaErrorPrint( "Usage: check_simulate  [-h] [-v] <implModule> <specModule>\n");
  return TCL_ERROR;
}


  

			     






