/**CFile***********************************************************************

  FileName    [symUtil.c]

  PackageName [sym]

  Synopsis    [utilities provided by sym package]

  Description [This file contains utilities that are used both by other files
               of the sym package, as well as by users of sym package]

  SeeAlso     [sym.c]

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

#include "symInt.h" 

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

  Synopsis           [Allocate and return a symInfo struct]

  Description        [Allocate and return a symInfo struct]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoFree]

******************************************************************************/
Sym_Info_t *
Sym_SymInfoAlloc(
  Tcl_Interp *interp,
  Mdl_Module_t *module,
  mdd_manager *manager)
{
  Sym_Info_t *symInfo = ALLOC(Sym_Info_t, 1);

  symInfo->transRelation = ALLOC(Sym_Trans_t, 1);
  symInfo->transRelation->module = module;
  symInfo->transRelation->manager = manager;
  symInfo->transRelation->conjuncts = array_alloc( mdd_t *, 0);
  symInfo->transRelation->atmToConjunct = st_init_table(st_ptrcmp, st_ptrhash);

  symInfo->interp = interp;
  symInfo->manager = manager;

  symInfo->varToUnprimedId = st_init_table(st_ptrcmp, st_ptrhash);
  symInfo->varToPrimedId = st_init_table(st_ptrcmp, st_ptrhash);
  symInfo->primedIdArray = array_alloc(int, 0);
  symInfo->unprimedIdArray = array_alloc(int, 0);
  symInfo->quantifyIdArray = array_alloc(int, 0);
  symInfo->idToVar = st_init_table(st_ptrcmp, st_ptrhash);

  symInfo->imgInfo = NIL(Img_ImageInfo_t);

  symInfo->rangeToDummyId1 = st_init_table(st_ptrcmp, st_ptrhash);
  symInfo->rangeToDummyId2 = st_init_table(st_ptrcmp, st_ptrhash);
  
  return symInfo;
}


  
/**Function********************************************************************

  Synopsis           [Utility to access a dummyId for a range]

  Description        [We create twp dummy MDD ids for each value of range in a
                      multi valued expression. These MDD ids are used while
		      manipluating mvfs. This function looks up a dummy id
		      given the range value]

  SideEffects        [If the desired dummy id is not found, it is created 
                      and inserted into the rangeToDummyId table]

  SeeAlso            []

******************************************************************************/
int
SymRangeReadDummyId(
  Sym_Info_t *symInfo,
  int range,
  int num)
{
  long l;
  st_table *rangeToId = (num == 1) ? symInfo->rangeToDummyId1: symInfo->rangeToDummyId2;
  boolean result = st_lookup(rangeToId, (char *)(long)range, (char **)&l);
  if(result)
    return ( (int) l);

  l = (long) SymCreateMddVariable(symInfo->manager, range);
  st_insert(rangeToId, (char *)(long)range, (char *)l);
  
  return( (int) l);
}

/**Function********************************************************************

  Synopsis           [Utility to free symInfo]

  Description        [Utility to free symInfo]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoAlloc]

******************************************************************************/
void
Sym_SymInfoFree(
  Sym_Info_t *symInfo)
{
  Sym_Trans_t  *t = symInfo->transRelation;
  int i;

  if(t){
    for ( i =0; i < array_n(t->conjuncts); i++){
      mdd_free((mdd_t *) array_fetch(mdd_t *, t->conjuncts,  i));
    }
    array_free(t->conjuncts);
    st_free_table(t->atmToConjunct);
    FREE(t);
  }

  if (symInfo->imgInfo)
    Img_ImageInfoFree(symInfo->imgInfo);
  st_free_table(symInfo->varToPrimedId);
  array_free(symInfo->primedIdArray);
  array_free(symInfo->quantifyIdArray);
  st_free_table(symInfo->varToUnprimedId);
  array_free(symInfo->unprimedIdArray);
  st_free_table(symInfo->idToVar);

  st_free_table(symInfo->rangeToDummyId1); 
  st_free_table(symInfo->rangeToDummyId2); 

  FREE(symInfo);
}

/**Function********************************************************************

  Synopsis           [Utility to dump mdds]

  Description        [Dump the given MDD as a sum of cubes.
                      Using symInfo, the MDD ids are translated to variable
		      names in the reactive module description]

  SideEffects        [none]

  SeeAlso            [Sym_MddPrintRawCubes]

******************************************************************************/
void
Sym_MddPrintCubes(
  Sym_Info_t *symInfo,
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
    id = SymGetTopMddId(mdd, symInfo);

    if(SymIsUnprimedId(symInfo, id)){
      pOrUnp[0] = '\0';
    }
    
    if( !st_lookup(symInfo->idToVar, (char *)(long)(id), (char **)&var)){
      Main_MochaErrorPrint("Error: No var for id : %d\n", id);
      return;
    }

    varName = Var_VariableReadName(var);
    dataType = Var_VariableReadDataType(var);

    nvals = SymGetNvals(symInfo, id);
    for ( i =0; i < nvals; i++){
      literal = SymGetMddLiteral(symInfo->manager,id,i);
      s = mdd_cofactor(symInfo->manager, mdd, literal);

      if( !mdd_is_tautology(s, 0)){
        if(ife)
          Main_MochaPrint( " + ");

        if(dataType == Var_Enumerated_c){
          Main_MochaPrint( "(%s%s = %s)", varName, pOrUnp,
                           Var_EnumElementReadName(
                             Var_VariableObtainEnumElementFromIndex(var, i)));
        }
        else
          Main_MochaPrint( "(%s%s = %d)", varName,  pOrUnp, i);
        
        ife = TRUE;
        Sym_MddPrintCubes(symInfo, s , level);
      }
    }
  }
  
  
  level--;

  Main_MochaPrint(") ");
  
  if(level == 0)
    Main_MochaPrint("\n");
}



/**Function********************************************************************

  Synopsis           [Utility to dump mdds]

  Description        [Dump the given MDD as a sum of cubes.
                      Using symInfo, the MDD ids are translated to variable
		      names in the reactive module description]

  SideEffects        [none]

  SeeAlso            [Sym_MddPrintRawCubes]

******************************************************************************/
void
Sym_MddPrintCubesImplSpec(
  Sym_Info_t *implSymInfo,
  Sym_Info_t *specSymInfo,
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
  boolean implVar;

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
    id = SymGetTopMddId(mdd, implSymInfo);

    if(SymIsUnprimedId(implSymInfo, id)){
      pOrUnp[0] = '\0';
    }
    
    if(st_lookup(implSymInfo->idToVar, (char *)(long)(id), (char **)&var)){
      implVar = TRUE;
      nvals = SymGetNvals(implSymInfo, id);
    }
    else if(st_lookup(specSymInfo->idToVar, (char *)(long)(id), (char **)&var)){
      implVar = FALSE;
      nvals = SymGetNvals(specSymInfo, id);
    }
    else{
      Main_MochaErrorPrint("Error: No var for id : %d\n", id);
      return;
    }

    varName = Var_VariableReadName(var);
    dataType = Var_VariableReadDataType(var);

    for ( i =0; i < nvals; i++){
      literal = SymGetMddLiteral(implSymInfo->manager,id,i);
      s = mdd_cofactor(implSymInfo->manager, mdd, literal);

      if( !mdd_is_tautology(s, 0)){
        if(ife)
          Main_MochaPrint( " + ");

        if(dataType == Var_Enumerated_c){
          Main_MochaPrint( "(%s%s%s = %s)", (implVar? "I:" : "S:"), varName, pOrUnp,
                           Var_EnumElementReadName(
                             Var_VariableObtainEnumElementFromIndex(var, i)));
        }
        else
          Main_MochaPrint( "(%s%s%s = %d)", (implVar? "I:" : "S:"), varName,  pOrUnp, i);
        
        ife = TRUE;
        Sym_MddPrintCubesImplSpec(implSymInfo, specSymInfo, s , level);
      }

      mdd_free(literal);      
      mdd_free(s);
    }
  }
  
  
  level--;

  Main_MochaPrint(") ");
  
  if(level == 0)
    Main_MochaPrint("\n");
}

  
/**Function********************************************************************

  Synopsis           [Utility to generate a state string for Tcl error
                      trace display]

  Description        [Given an mdd representinga state, generate a string
                      in the form x1=v1 x2=v2 ...]

  SideEffects        [desired sting is padded to the end of stateString]

  SeeAlso            [Sym_MddPrintCubes]

******************************************************************************/
void
Sym_MddPrintStateForErrorTrace(
  Sym_Info_t *symInfo,
  mdd_t *mdd,
  char **stateString
 )
{
  mdd_t *s;
  int  id;
  char *varName;
  int nvals, i;
  mdd_t *literal;
  char *newString;
  char tempstr[256];
  Var_Variable_t *var;
  Var_DataType dataType;  
  
  if(mdd_is_tautology(mdd,1)){
    return;
  }

  id = SymGetTopMddId(mdd, symInfo);
          
  if( !st_lookup(symInfo->idToVar, (char *)(long)(id), (char **)&var)){
    Main_MochaErrorPrint("Error: No var for id : %d\n", id);
    return;
  }

  varName = Var_VariableReadName(var);
  dataType = Var_VariableReadDataType(var);

  nvals = SymGetNvals(symInfo, id);
  for ( i =0; i < nvals; i++){
    literal = SymGetMddLiteral(symInfo->manager,id,i);
    s = mdd_cofactor(symInfo->manager, mdd, literal);
    mdd_free(literal);
    
    if( !mdd_is_tautology(s, 0)){
      if(dataType == Var_Enumerated_c){
        sprintf(tempstr, "=%s ",
                Var_EnumElementReadName(Var_VariableObtainEnumElementFromIndex(var, i)));
      }
      else
        sprintf(tempstr  , "=%d ", i);
      newString = util_strcat3( *stateString, varName, tempstr);
      FREE(*stateString);
      *stateString = newString;
      Sym_MddPrintStateForErrorTrace(symInfo, s, stateString);
    }
    
    mdd_free(s);
  }
}


/**Function********************************************************************

  Synopsis           [Utility to dump mdds - in terms of raw mdd ids]

  Description        [Utility to dump mdds - in terms of raw mdd ids]

  SideEffects        [none]

  SeeAlso            [Sym_MddPrintCubes]

******************************************************************************/
void
Sym_MddPrintRawCubes(
  Sym_Info_t *symInfo,
  mdd_t *mdd,
  int level
 )
{
  mdd_t *s;
  int  id;
  char varName[100];
  boolean ife = FALSE;
  int nvals, i;
  mdd_t *literal;
  
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
    id = SymGetTopMddId(mdd, symInfo);
    sprintf(varName, "V_%d", id);
    nvals = SymGetNvals(symInfo, id);
    for ( i =0; i < nvals; i++){
      literal = SymGetMddLiteral(symInfo->manager,id,i);
      s = mdd_cofactor(symInfo->manager, mdd, literal);

      if( !mdd_is_tautology(s, 0)){
        if(ife)
          Main_MochaPrint( " + ");
        Main_MochaPrint( "(%s = %d)", varName, i);
        ife = TRUE;
        Sym_MddPrintRawCubes(symInfo, s, level);
      }
    }
  }
  
  
  level--;

  Main_MochaPrint(") ");
  
  if(level == 0)
    Main_MochaPrint("\n");
}


/**Function********************************************************************

  Synopsis            [ match mdd ids of two modules ]

  Description         [ returns arrays of matching mdd ids for unprimed vars of
                        two modules (through the arguments). The return value is
			true if every variable of module2 is also present
			in module 1 (this is the no-hidden variable case)]
 
  SideEffects        [ matching ids are returned in corresponding indices of
                       idArray1 and idArray2]

  SeeAlso            []

******************************************************************************/
boolean
Sym_ModuleMatchMddIds(
  Mdl_Module_t *module1,
  Mdl_Module_t *module2,
  Sym_Info_t *symInfo1,
  Sym_Info_t *symInfo2,
  array_t *idArray1,
  array_t *idArray2)
{
  boolean result = TRUE;
  lsList m2VarList;
  lsGen varGen;
  lsGeneric var;
  Var_Variable_t *var1, *var2;
  int unprimedMddId1, unprimedMddId2;
  int primedMddId1, primedMddId2;
  char *varname;
  
  m2VarList = Mdl_ModuleObtainVariableList(module2);
  lsForEachItem(m2VarList, varGen, var){
    var2 = (Var_Variable_t *)var;
    if(!Sym_SymInfoLookupUnprimedVariableId(symInfo2, var2, &unprimedMddId2 )){
      Main_MochaErrorPrint( "Error: Sym_ModuleMatchMddIds cant find mdd id for var\n");
      exit(1);
    }

    if(!Sym_SymInfoLookupPrimedVariableId(symInfo2, var2, &primedMddId2 )){
      Main_MochaErrorPrint( "Error: Sym_ModuleMatchMddIds cant find mdd id for var\n");
      exit(1);
    }
     
    varname = Var_VariableReadName(var2);
    var1 = Mdl_ModuleReadVariableFromName(varname, module1);
    if(!var1)
      result = FALSE;
    else {
      if(!Sym_SymInfoLookupUnprimedVariableId(symInfo1, var1, &unprimedMddId1 )){
        Main_MochaErrorPrint( "Error: Sym_ModuleMatchMddIds cant find mdd id for var\n");
        exit(1);
      }

      if(!Sym_SymInfoLookupPrimedVariableId(symInfo1, var1, &primedMddId1 )){
        Main_MochaErrorPrint( "Error: Sym_ModuleMatchMddIds cant find mdd id for var\n");
        exit(1);
      }

      /* now just insert the mdd ids */
      array_insert_last(int, idArray1, unprimedMddId1);
      array_insert_last(int, idArray2, unprimedMddId2);

      array_insert_last(int, idArray1, primedMddId1);
      array_insert_last(int, idArray2, primedMddId2);
    }
  }

  return(result);
}

/**Function********************************************************************

  Synopsis           [Utility to build a flat transition relation]

  Description        [symInfo maintains a transition relation for a module
                      as an array of conjuncts.  This utility takes "and"
		      of all the conjuncts and builds a flat
		      transition relation]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
mdd_t *
Sym_SymInfoBuildFlatTrans(
  Sym_Info_t *syminfo)
{
  mdd_manager *manager = syminfo->manager;
  mdd_t * result, *conjunct, *temp1;
  array_t *conjuncts = syminfo->transRelation->conjuncts;
  int i;

  result = mdd_one(manager);
  
  for ( i = 0; i < array_n(conjuncts); i++){
    conjunct = array_fetch(mdd_t *, conjuncts, i);
    temp1 = mdd_and(conjunct, result, 1, 1);
    mdd_free(result);
    result = temp1;
  }

  return(result);
}

/**Function********************************************************************

  Synopsis           [read the varToUnprimedId st_table]

  Description        [read the varToUnprimedId st_table]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoReadVarToPrimedId]


******************************************************************************/
st_table *
Sym_SymInfoReadVarToUnprimedId(
  Sym_Info_t *symInfo)
{
  return(symInfo->varToUnprimedId);
}
/**Function********************************************************************

  Synopsis           [read the varToPrimedId st_table]

  Description        [read the varToPrimedId st_table]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoReadVarToUnprimedId]


******************************************************************************/
st_table *
Sym_SymInfoReadVarToPrimedId(
  Sym_Info_t *symInfo)
{
  return(symInfo->varToPrimedId);
}



/**Function********************************************************************

  Synopsis           [read the unprimedIdArray array]

  Description        [read the unprimedIdArray array]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoReadPrimedIdArray]

******************************************************************************/
array_t *
Sym_SymInfoReadUnprimedIdArray(
  Sym_Info_t *symInfo)
{
  return(symInfo->unprimedIdArray);
}

/**Function********************************************************************

  Synopsis           [read the primedIdArray array]

  Description        [read the primedIdArray array]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoReadUnprimedIdArray]

******************************************************************************/
array_t *
Sym_SymInfoReadPrimedIdArray(
  Sym_Info_t *symInfo)
{
  return(symInfo->primedIdArray);
}

/**Function********************************************************************

  Synopsis           [read the quantifyIdArray array]

  Description        [read the quantifyIdArray array]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoReadUnprimedIdArray]

******************************************************************************/
array_t *
Sym_SymInfoReadQuantifyIdArray(
  Sym_Info_t *symInfo)
{
  return(symInfo->quantifyIdArray);
}
 

/**Function********************************************************************

  Synopsis           [read the image info from symInfo]

  Description        [read the image info from symInfo]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoReadUnprimedArray]

******************************************************************************/
Img_ImageInfo_t *
Sym_SymInfoReadImgInfo(
  Sym_Info_t *symInfo)
{
  return(symInfo->imgInfo);
}
/**Function********************************************************************

  Synopsis           [read the idToVar table from symInfo]

  Description        [read the idToVar table from symInfo]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
st_table *
Sym_SymInfoReadIdToVar(
  Sym_Info_t *symInfo)
{
  return(symInfo->idToVar);
}

/**Function********************************************************************

  Synopsis           [read the image info from symInfo]

  Description        [read the image info from symInfo]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoReadUnprimedArray]

******************************************************************************/
void
Sym_SymInfoSetImgInfo(
  Sym_Info_t *symInfo,
  Img_ImageInfo_t *imgInfo
  )
{
  symInfo->imgInfo = imgInfo;
}

/**Function********************************************************************

  Synopsis           [read the conjuncts from symInfo]

  Description        [read the conjuncts from symInfo]

  SideEffects        [none]

  SeeAlso            [Sum_SymInfoReadImgInfo]

******************************************************************************/
array_t *
Sym_SymInfoReadConjuncts(
  Sym_Info_t *symInfo
)
{
  return(symInfo->transRelation->conjuncts);
}



/**Function********************************************************************

  Synopsis           [read the mdd manager from symInfo]

  Description        [read the mdd manager from symInfo]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoReadConjuncts]

******************************************************************************/
mdd_manager *
Sym_SymInfoReadManager(
  Sym_Info_t *symInfo)
{
  return(symInfo->manager);
}


/**Function********************************************************************

  Synopsis           [read the conjunct  (transtion relation) for the given atom ]

  Description        [read the conjunct  (transtion relation) for the given atom ]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoReadManager]

******************************************************************************/
mdd_t *
Sym_SymInfoReadConjunctForAtom(
  Sym_Info_t *symInfo,
  Atm_Atom_t *atom)
{
  mdd_t *retVal;
  boolean result = st_lookup(symInfo->transRelation->atmToConjunct,
                             (char *)atom, (char **)&retVal);

  return retVal;
  
}


/**Function********************************************************************

  Synopsis           [set the conjunct  (transtion relation) for the given atom ]

  Description        [set the conjunct  (transtion relation) for the given atom ]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoReadManager]

******************************************************************************/
void
Sym_SymInfoSetConjunctForAtom(
  Sym_Info_t *symInfo,
  Atm_Atom_t *atom,
  mdd_t *atomMdd)
{
  st_insert(symInfo->transRelation->atmToConjunct, (char *) atom, (char *) atomMdd);  
}


/**Function********************************************************************

  Synopsis           [lookup primed mdd id for a variable ]

  Description        [lookup primed mdd id for a variable ]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoLookupUnprimedVariableId]

******************************************************************************/
boolean
Sym_SymInfoLookupPrimedVariableId(
  Sym_Info_t *symInfo,
  Var_Variable_t *var,
  int *id)
{
  long l;
  st_table *table = symInfo->varToPrimedId;
  boolean result = st_lookup(table, (char *) var, (char **)&l);
  *id = (int) l;
  return(result);
}

/**Function********************************************************************

  Synopsis           [lookup unprimed mdd id for a variable ]

  Description        [lookup unprimed mdd id for a variable ]

  SideEffects        [none]

  SeeAlso            [Sym_SymInfoLookupPrimedVariableId]

******************************************************************************/
boolean
Sym_SymInfoLookupUnprimedVariableId(
  Sym_Info_t *symInfo,
  Var_Variable_t *var,
  int *id)
{
  long l;
  st_table *table = symInfo->varToUnprimedId;
  boolean result = st_lookup(table, (char *) var, (char **)&l);
  *id = (int) l;
  return(result);
}


/**Function********************************************************************

  Synopsis           [utility to pick a minterm form a MDD]

  Description        [utility to pick a minterm form a MDD]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
mdd_t  *
Sym_MddPickMinterm(
  Sym_Info_t *symInfo,
  mdd_t *mdd )
{
  int id, nvals, i;
  mdd_t *literal;
  mdd_t *rest, *result, *temp;
  
  if(mdd_is_tautology(mdd, 1)){
    return mdd_one(symInfo->manager);
  }

  if(mdd_is_tautology(mdd, 0)){
    Main_MochaErrorPrint( "Error in Sym_MddPickMinterm\n");
  }

  id = SymGetTopMddId(mdd, symInfo);
  nvals = SymGetNvals(symInfo, id); 

  for ( i = 0; i < nvals; i++){
      literal = SymGetMddLiteral(symInfo->manager,id,i);
      temp = mdd_cofactor(symInfo->manager, mdd, literal);
      if(!mdd_is_tautology(temp, 0)){
        rest = Sym_MddPickMinterm(symInfo, temp);
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

  Synopsis           [Compute partial image with respect to a set of atoms]

  Description        [Compute partial image with respect to a set of atoms]

  SideEffects        [none]

  SeeAlso            [Sym_AtomsBuildPartialInitialRegion]
******************************************************************************/
mdd_t * Sym_AtomsComputePartialImage(
   Sym_Info_t *symInfo,
   array_t *atomArray,
   mdd_t *prevState,
   mdd_t *partialState)
{

  array_t *andSmoothArray = array_alloc(mdd_t *, 0);
  int i;
  array_t *smoothIdArray, *idArray;
  mdd_t *result;
  array_t *nullArray = array_alloc(mdd_t *, 0);

  array_insert_last(mdd_t *, andSmoothArray, prevState);
  array_insert_last(mdd_t *, andSmoothArray, partialState);
  
  for (i=0; i < array_n(atomArray); i++){
    Atm_Atom_t *atom = array_fetch(Atm_Atom_t *, atomArray, i);
    mdd_t *c = Sym_SymInfoReadConjunctForAtom(symInfo, atom);
    if(!c){
      Main_MochaErrorPrint("Could not find trans. relation for atom\n");
      exit(1);
    }
    array_insert_last(mdd_t *, andSmoothArray, c);
  }

  smoothIdArray = mdd_get_support(symInfo->manager, prevState);
  idArray = mdd_get_support(symInfo->manager, partialState);
  array_append(smoothIdArray, idArray);
  

  result = Img_MultiwayLinearAndSmooth(symInfo->manager,
				       andSmoothArray,
				       smoothIdArray,
				       nullArray);
  
  array_free(smoothIdArray);
  array_free(idArray);
  array_free(andSmoothArray);
  array_free(nullArray);
  return(result);
}


/**Function********************************************************************

  Synopsis           [build mdd for partial initial region]

  Description        [build mdd for partial initial region]

  SideEffects        [none]

  SeeAlso            [Sym_AtomsComputePartialImage]

******************************************************************************/
mdd_t *
Sym_AtomsBuildPartialInitialRegion(
  Sym_Info_t *symInfo,
  array_t *atomArray,
  mdd_t *partialState)
{
  Atm_Atom_t * atom;
  int i = 0;
  int varid;
  int numAtoms = array_n(atomArray);
  mdd_t *pState = mdd_dup(partialState);
  
  /* Now loop through atoms and build initial states for each atom */
  for( i=0; i<numAtoms; i++){
    mdd_t *atomInitState, *temp;

    atom = array_fetch(Atm_Atom_t*, atomArray, i);

    atomInitState = Sym_AtomBuildInitialRegion(symInfo,(Atm_Atom_t*)atom);
    temp = mdd_and (atomInitState, pState, 1, 1);

    mdd_free(partialState);
    mdd_free(atomInitState);
    pState = temp;
  }

  /* make sure that initial state is independent of unprimed vars    */
  for ( i = 0 ; i < array_n(symInfo->unprimedIdArray); i++){
    varid = array_fetch(int, symInfo->unprimedIdArray, i);

    if(SymMddInTrueSupport(pState, varid, symInfo)){
      Main_MochaErrorPrint(
              "Error: Initial region depends on unprimed var: %d \n",
              varid);
      exit(1);
    }

  }

  if(array_n(symInfo->primedIdArray) != array_n(symInfo->unprimedIdArray)){
    Main_MochaErrorPrint(
            "Error: Unprimed and Primed MDD id arrays have unequal sizes\n");
    exit(1);
  }
  
 
  return (pState);
  
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [does this id belong to a primed or unprimed variable]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
******************************************************************************/
boolean
SymIsUnprimedId(
  Sym_Info_t *symInfo,
  int id)
{
  return( (id % 2) == 0);
}



/**Function********************************************************************

  Synopsis           [utility to create an MDD variable]

  Description        [given a range create an MDD variable for that range]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SymCreateMddVariable(
  mdd_manager *manager,
  int range
  )
{
  array_t *mvarValues = array_alloc(unsigned int, 1);
  int  mddId;

  array_insert(unsigned int, mvarValues, 0, range);
  mddId = mdd_create_variables(manager, mvarValues, NIL(array_t), NIL(array_t));
  array_free(mvarValues);

  return((int)mddId);
}



/**Function********************************************************************

  Synopsis           [utility to get the number of possible values of a multi
  valued variable]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SymGetNvals(
  Sym_Info_t *symInfo,
  int id)
{
  mvar_type x;
  array_t *mvar_list = mdd_ret_mvar_list(symInfo->manager);
  x = array_fetch(mvar_type, mvar_list, id);
  return(x.values);
}


/**Function********************************************************************

  Synopsis           [Check if the given variable is in the true support of
                      the given mdd ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
SymMddInTrueSupport(
  mdd_t *m,
  int id,
  Sym_Info_t *symInfo)
{
  array_t * varidArray;
  int i;
  
  varidArray = mdd_get_support(symInfo->manager, m);
  for ( i = 0; i < array_n(varidArray); i++){
    if (id ==  (int)array_fetch(int, varidArray, i))
      return(TRUE);
  }

  array_free(varidArray);
  return(FALSE);
}

/**Function********************************************************************

  Synopsis           [get top mdd id ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SymGetTopMddId(
  mdd_t *m,
  Sym_Info_t *symInfo)
{
  array_t * varidArray;
  int firstId;
  
  varidArray = mdd_get_support(symInfo->manager, m);
  if (array_n(varidArray) == 0){
    Main_MochaErrorPrint("Error: SymGetTopMddId : mdd has empty support\n");
    exit(1);
  }

  firstId = (int)array_fetch( int, varidArray, 0);
  array_free(varidArray);
  return(firstId);
}

/**Function********************************************************************

  Synopsis           [utility to get and MDD literal]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
mdd_t *
SymGetMddLiteral(
  mdd_manager *manager,
  int id,
  int value)
{
  array_t *values = array_alloc(int, 0);
  mdd_t *mdd;
  
  array_insert(int, values, 0, value);
  mdd = mdd_literal(manager, id, values);

  if(!mdd){
    Main_MochaErrorPrint("SymGetMddLiteral: got NULL mdd for variable id %d val %d\n",
                         id, value);
    exit(1);
  }
  
  array_free(values);
  return(mdd);

}



/**Function********************************************************************

  Synopsis           [utility to get a constant value from an expression]

  Description        [The key here is to return the "index" of the constant
  value in the case of enumerated data types]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SymGetConstant(
  Atm_Expr_t *expr)
{
  Atm_ExprType exprType = Atm_ExprReadType(expr);
  switch(exprType){
      case Atm_BoolConst_c:
        return( Atm_ExprReadLeftChild(expr) ? 1 : 0 );

      case Atm_EnumConst_c:
      {
        Var_EnumElement_t *elem =
            (Var_EnumElement_t *) Atm_ExprReadLeftChild(expr);
        int c =  Var_EnumElementObtainIndex(elem);
        return(c);
      }

      case Atm_RangeConst_c:
      case Atm_NumConst_c:
        return((int)(long)Atm_ExprReadLeftChild(expr));

      default:
        Main_MochaErrorPrint("SymGetConstant: Expression is not a valid constant\n");
        exit(1);
  }
}




/**Function********************************************************************

  Synopsis           [utility to check if a given expression is a num or enum or range
                      constant]

  Description        []

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
SymIsNumEnumRangeConstant(
  Atm_Expr_t *expr)
{
  Atm_ExprType exprType = Atm_ExprReadType(expr);
  switch(exprType){
      case Atm_EnumConst_c:
      case Atm_NumConst_c:
      case Atm_RangeConst_c:
        return(TRUE);

      default:
        return(FALSE);
  }
}



/**Function********************************************************************

  Synopsis           [utility to get the mdd variable id corresponding to the expression]

  Description        []

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SymGetVariableId(
  Atm_Expr_t *expr,
  Sym_Info_t *symInfo)
{
  Atm_ExprType exprType = Atm_ExprReadType(expr);  
  Var_Variable_t *var;
  int varid;
  
  switch(exprType){
      case Atm_UnPrimedVar_c:
      {

        var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        
        if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, var, &varid )){
          Main_MochaErrorPrint( "Error: SymGetVariableId, cant find mdd id for var\n");
          exit(1);
        }
        break;
      }
      
      case Atm_PrimedVar_c:
      {
        var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        
        if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, &varid )){
          Main_MochaErrorPrint( "Error: SymGetVariableId, cant find mdd id for var\n");
          exit(1);
        }
        break;
      }

      default:
        Main_MochaErrorPrint( "Error: SymGetVariableId, expression is not a veriable\n");
        exit(1);
  }

  return(varid);
}






/**Function********************************************************************

  Synopsis           [utility to get the components of an expression of the
  form v + c or v - c]

  Description        []

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SymGetVariableIdConstantAndPlus(
  Atm_Expr_t *expr,
  int *varId,
  int *c,
  boolean *isPlus,
  Sym_Info_t *symInfo)
{
  Atm_ExprType exprType = Atm_ExprReadType(expr);  
  Atm_Expr_t *lexpr = Atm_ExprReadLeftChild(expr);
  Atm_Expr_t *rexpr = Atm_ExprReadRightChild(expr);
  Atm_ExprType lexprType, rexprType;
  Var_Variable_t *var;

  /* first get if it is a plus or a minus */
  switch(exprType){
    case  Atm_Plus_c:
    {
      *isPlus = TRUE;
      break;
    }

    case Atm_Minus_c:
    {
      *isPlus = FALSE;
      break;
    }

   case Atm_UnPrimedVar_c:
   {
     *isPlus = TRUE;
     *c = 0;
     var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        
     if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, var, varId )){
       Main_MochaErrorPrint( "Error: SymGetVariableIdConstantAndPlus, cant find mdd id for var\n");
       exit(1);
     }
     return;
   }
   
   case Atm_PrimedVar_c:
   {
     *isPlus = TRUE;
     *c = 0;
     var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        
     if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, varId )){
       Main_MochaErrorPrint( "Error: SymGetVariableIdConstantAndPlus, cant find mdd id for var\n");
       exit(1);
     }
     return;
   }

   default:
     Main_MochaErrorPrint(
       "SymGetVariableIdConstantAndPlus : Bad Expression type : only + or - are expected\n");
     exit(1);
  }
  

  /* get the variable id */
  lexprType = Atm_ExprReadType(lexpr);
  switch(lexprType){
      case Atm_UnPrimedVar_c:
      {

        var = (Var_Variable_t *)Atm_ExprReadLeftChild(lexpr);
        
        if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, var, varId )){
          Main_MochaErrorPrint( "Error: SymGetVariableIdConstantAndPlus, cant find mdd id for var\n");
          exit(1);
        }
        break;
      }
      
      case Atm_PrimedVar_c:
      {
        var = (Var_Variable_t *)Atm_ExprReadLeftChild(lexpr);
        
        if(!Sym_SymInfoLookupPrimedVariableId(symInfo, var, varId )){
          Main_MochaErrorPrint( "Error: SymGetVariableIdConstantAndPlus, cant find mdd id for var\n");
          exit(1);
        }
        break;
      }
  }

  /* now get the constant */
  *c = SymGetConstant(rexpr);
  return;
}

    


/**Function********************************************************************

  Synopsis           [is the given expression a multi-valued variable?]

  Description        []

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
SymIsMultiValuedVariable(
  Atm_Expr_t *expr)
{
  Var_Variable_t *var;
  Var_DataType dType;
  Atm_ExprType exprType = Atm_ExprReadType(expr);
  
  switch(exprType){
      case Atm_UnPrimedVar_c:
      case Atm_PrimedVar_c:
        var = (Var_Variable_t *)Atm_ExprReadLeftChild(expr);
        dType = Var_VariableReadDataType(var);
        if ((dType == Var_Enumerated_c)  || (dType == Var_Range_c) || (dType == Var_Timer_c))
          return(TRUE);
        else
          return(FALSE);
        break;

      default:
        return(FALSE);
  }
}



/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Variable_t * 
SymGetArrayElementVar(
  Sym_Info_t *symInfo,
  Var_Variable_t *var,
  int value)
{
  char *varName, *actualVarName;
  Var_Type_t *varType, *indexType;
  boolean indexIsEnum = FALSE; 
  int id;
  Var_Variable_t *actualVar;
  
  
  varName = Var_VariableReadName(var);
  varType = Var_VariableReadType(var);

  if (!Var_TypeIsArray(varType) && !Var_TypeIsBV(varType)) {
    Main_MochaErrorPrint("SymGetArrayId: variable is not array or bv.\n");
    exit(1);
  }
  

  indexType = Var_TypeIsArray(varType)?
      Var_VarTypeReadIndexType(varType):  BoolType;
      
  if (Var_TypeIsEnum(indexType)) {
    
    actualVarName = Main_strConcat(4,
                                   varName,
                                   "[",
                                   Var_EnumElementReadName(Var_VarTypeReadSymbolicValueFromIndex(indexType, value)),
                                   "]");
  } else {
    char * vString = Main_ConvertIntToString(value);
    actualVarName = Main_strConcat(4,
                                varName,
                                "[",
                                vString,
                                "]");
    FREE(vString);
  }

  actualVar = Mdl_ModuleReadVariableFromName(actualVarName, symInfo->transRelation->module);
  if(!actualVar){
    Main_MochaErrorPrint("SymGetArrayId: Cant find variable %s in module",
                         actualVarName);
    exit(1);
  }
  FREE(actualVarName);

  return(actualVar);
  
}



/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SymGetArrayId(
  Sym_Info_t *symInfo,
  Var_Variable_t *var,
  Atm_ExprType   pOrUnp,
  int value)
{
  int id;
  Var_Variable_t *actualVar;
  
  actualVar = SymGetArrayElementVar(symInfo, var, value);

  switch(pOrUnp){
      case Atm_BitwisePrimedVar_c:
      case Atm_PrimedVar_c:
        if(!Sym_SymInfoLookupPrimedVariableId(symInfo, actualVar, &id )){
          Main_MochaErrorPrint( "Error: SymGetArrayId, cant find mddid for var\n");
          exit(1);
        }
        break;

      case Atm_BitwiseUnPrimedVar_c:
      case Atm_UnPrimedVar_c:
        if(!Sym_SymInfoLookupUnprimedVariableId(symInfo, actualVar, &id )){
          Main_MochaErrorPrint( "Error: SymGetArrayId, cant find mddid for var\n");
          exit(1);
        }
        break;

      default:
        Main_MochaErrorPrint("Error: SymGetArrayId: bad Atm_ExprType\n");
        exit(1);
  }

  return(id);
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SymGetArrayNvals(
  Var_Variable_t *var)
{
  char *varName;
  Var_Type_t *varType, *indexType;
  int retVal;
  

  varName = Var_VariableReadName(var);
  varType = Var_VariableReadType(var);

  if (Var_VarTypeReadDataType(varType) != Var_Array_c) {
    Main_MochaErrorPrint("SymGetArrayNvals: variable doenst have type Var_Array_c\n");
    exit(1);
  }

  indexType = Var_VarTypeReadIndexType(varType);
  retVal = Var_VarTypeReadDomainSize(indexType);

  if(retVal < 0){
    Main_MochaPrint("SymGetArrayNvals:bas indextype \n");
    exit(1);
  }

  return(retVal);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mdl_Module_t *
SymInfoReadModule(
  Sym_Info_t *symInfo)
{
  return symInfo->transRelation->module;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
mdd_t *
SymGetRelExprMdd(
  Sym_Info_t *symInfo,
  Atm_ExprType exprType,
  int lvarid,
  int rvarid)
{
  
  mdd_t *result;
        
  switch(exprType){
      case Atm_Equal_c:
        result = mdd_eq_s(symInfo->manager, lvarid, rvarid);
        break;

      case Atm_NotEqual_c:
        result = mdd_neq_s(symInfo->manager, lvarid, rvarid);
        break;

      case Atm_Greater_c:
        result = mdd_gt_s(symInfo->manager, lvarid, rvarid);
        break;
        
      case Atm_GreaterEqual_c:
        result = mdd_geq_s(symInfo->manager, lvarid, rvarid);
        break;
        
      case Atm_Less_c:
        result = mdd_lt_s(symInfo->manager, lvarid, rvarid);
        break;
        
      case Atm_LessEqual_c:
        result = mdd_leq_s(symInfo->manager, lvarid, rvarid);
        break;
        
      default:
        Main_MochaErrorPrint("Error: SymGetRelExprMdd : unsupported operator\n");
        exit(1);
        break;
  }

  return(result);
}

/**Function********************************************************************

  Synopsis           [Check if a state is in the mdd.]

  Description        [A state is given as an array of varids (corresponding to
                      the unprimed variables) and an array of values
                      (corresponding to the values of the variables). This
                      function checks if the state is in the onset of the mdd.
                      This function assumes that the values are within the
                      domain size of the corresponding variables. (For
                      example, the value of a boolean variable can only be
                      0 or 1, and the value of a enumerative variable with
                      3 values can only be 0, 1 or 2).

		      It returns 0 if the state is not in the mdd, 1 if it is,
		      or 2 if it is not determined.  This happens when
		      the state is a partial state and there is not enough
		      information to check if it is in the mdd.                       

		      Note: this implementation is based on cofactoring and 
		      in theory is inefficient. It should be
		      replaced by a more efficient one which is based on traversal
		      of the mdd.]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
int
Sym_StateInMdd(
  mdd_manager * mddManager,
  mdd_t * mdd,
  array_t * varIdArray,
  array_t * valueArray)
{
  mdd_t * s = NIL(mdd_t);
  int flag;
  

  flag = Sym_StateInMddWithResidueMdd(mddManager, mdd, varIdArray, valueArray, &s);

  if (s)
    mdd_free(s);

  return flag;
  
}


/**Function********************************************************************

  Synopsis           [Check if a state is in the mdd.]

  Description        [A state is given as an array of varids (corresponding to
                      the unprimed variables) and an array of values
                      (corresponding to the values of the variables). This
                      function checks if the state is in the onset of the mdd.
                      This function assumes that the values are within the
                      domain size of the corresponding variables. (For
                      example, the value of a boolean variable can only be
                      0 or 1, and the value of a enumerative variable with
                      3 values can only be 0, 1 or 2).

		      It returns 0 if the state is not in the mdd, 1 if it is,
		      or 2 if it is not determined.  This happens when
		      the state is a partial state and there is not enough
		      information to check if it is in the mdd.  In this case,
                      the returnMdd pointer will point to the "residue mdd",
                      i.e., the remaining mdd that is not resolved.
                     

		      Note: this implementation is based on cofactoring and 
		      in theory is inefficient. It should be
		      replaced by a more efficient one which is based on traversal
		      of the mdd.]

  SideEffects        [none]

  SeeAlso            []

******************************************************************************/
int
Sym_StateInMddWithResidueMdd(
  mdd_manager * mddManager,
  mdd_t * mdd,
  array_t * varIdArray,
  array_t * valueArray,
  mdd_t ** returnMdd)
{
  mdd_t * s, * t;
  int id;
  int i;
  mdd_t *literal;
  
  if(mdd_is_tautology(mdd, 0)){
    return 0;
  }  
  
  if (mdd_is_tautology(mdd, 1)) {
    return 1;
  }
  
  s = mdd_dup(mdd);
  arrayForEachItem(int, varIdArray, i, id) {

    literal = SymGetMddLiteral(mddManager ,id, 
			       array_fetch(int, valueArray, i));
    t = mdd_cofactor(mddManager, s, literal);

    mdd_free(literal);
    if (mdd_is_tautology(t, 0)){
      mdd_free(t);
      mdd_free(s);
      return 0;
    }  
    
    if (mdd_is_tautology(t, 1)) {
      mdd_free(t);
      mdd_free(s);
      return 1;
    }
    
    mdd_free(s);
    s = t;
  }

  *returnMdd = s;

  return 2;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/



