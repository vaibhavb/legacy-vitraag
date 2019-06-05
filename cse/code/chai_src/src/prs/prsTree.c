/**CFile***********************************************************************

  FileName    [prsTree.c]

  PackageName [prs]

  Synopsis    [Routines for building parse tree.]

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

#include  "prsInt.h"
#include  "prsTokens.h"

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
int PrsUnnamedTypeCount = 0;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static Var_Type_t* PrsTypeCheckName(char *typeName, Var_TypeManager_t* typeManager);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Allocates a new PrsNode_t]

  Description        [Allocates a new PrsNode_t]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
PrsNode_t *
PrsNodeAlloc (
  int id,
  char* field1,
  char* field2)
{
  PrsNode_t * node;

  node = ALLOC(PrsNode_t, 1);
  node->id = id;
  node->field1 = field1;
  node->field2 = field2;

  return node;

}

/**Function********************************************************************

  Synopsis           [Frees a PrsNode_t]

  Description        [Frees a PrsNode_t. It is the programmers responsibility
  to free field1 and field2.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
PrsNodeFree (
  PrsNode_t * node)
{
  if (node != NIL(PrsNode_t)) {
    FREE(node);
    
  }
}

/**Function********************************************************************

  Synopsis           [Returns the id of the PrsNode_t]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
PrsNodeReadId(
  PrsNode_t* node)
{
  return node->id;
}

/**Function********************************************************************

  Synopsis           [Returns the field1 of the PrsNode_t]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
char *
PrsNodeReadField1(
  PrsNode_t* node)
{
  return node->field1;
}

/**Function********************************************************************

  Synopsis           [Returns the field2 of the PrsNode_t]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
char *
PrsNodeReadField2(
  PrsNode_t* node)
{
  return node->field2;
}


/**Function********************************************************************

  Synopsis    [Check whether the variable is primed, and strip the prime if so.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
int
PrsVariableCheckPrimed(
  char *name)
{
  int len = strlen(name);
  if (*(name+len-1) != '\'') return 0;
  else
    { 
      *(name+len-1) = '\0'; /* strip the last ' */
      return 1;
    }
}


/**Function********************************************************************

  Synopsis           [Check if a item is in a list.]

  Description        [return TRUE if found, FALSE else.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
PrsIsInList (
  lsList list,
  lsGeneric item)
{
  lsGen lsgen;
  lsGeneric item1;
  
  lsForEachItem(list, lsgen, item1){
    if (item1 == item) {
      lsFinish(lsgen);
      return TRUE;
    }
  }
  return FALSE;    
}


/**Function********************************************************************

  Synopsis           [Define new enumerative data types]

  Description        [This function creates new data types. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Type_t *
PrsTypeEnumAlloc(
  char* name,
  lsList enumConstList,
  Var_TypeManager_t* typeManager)
{
  Var_Type_t *type;
  char *elementName;
  lsGen lsgen;
  
  type = PrsTypeCheckName(name, typeManager);
  
  if (type){
    assert(name);
    FREE(name);
    return type;
  }
  else {
    Var_EnumElement_t *element;
    array_t *indexToValue;
    st_table *valueToIndex;
    int flag = 0;
    int count = 0;

    /* Check if the name of any element clashes with some enum
       element of another enumerated type. */
    lsForEachItem(enumConstList, lsgen, elementName){
      if (Var_TypeManagerReadEnumElementFromName(typeManager, elementName) !=
          NIL(Var_EnumElement_t)) {
        if (flag != -1) flag = 1;
        else flag = 0;
      } else {
        if (flag != 1) flag = -1;
        else flag = 0;
      }
      
      if (flag == 0) {
        Main_MochaErrorPrint(
          "Error at line %d:  Name \"%s\" used before.\n", yylineno, elementName);
        lsFinish(lsgen);
        break;
      }
    }
    
    if ((name && (flag==1)) || (flag == 0)) {
      lsDestroy(enumConstList, free);
      if (name)
        FREE(name);
      return NIL(Var_Type_t);
    }
    
    indexToValue = array_alloc(Var_EnumElement_t *, 0);
    valueToIndex = st_init_table(st_ptrcmp, st_ptrhash);

    type = Var_VarTypeEnumeratedAlloc(name, indexToValue, valueToIndex);
    
    lsForEachItem(enumConstList, lsgen, elementName){
      if (flag == -1){
        element = Var_EnumElementAlloc(elementName, type);
        Var_TypeManagerAddEnumElement(typeManager, element);
      } else {
        element = Var_TypeManagerReadEnumElementFromName(typeManager,
                                                      elementName);
      }
      
      array_insert_last(Var_EnumElement_t *, indexToValue, element);
      st_insert(valueToIndex, (char *) element, (char *) (long) count);
      count++;
    }


    if (flag == -1) {
      type = Var_TypeManagerAddType(typeManager, type);    
    } else {
      Var_Type_t *type1;
      if (!(type1 = Var_TypeManagerObtainType(typeManager, type))) {
        lsFirstItem(enumConstList, &elementName, LS_NH);
        Main_MochaErrorPrint(
          "Error at line %d:  Name \"%s\" used before.\n", yylineno, elementName);
      }
      Var_VarTypeFree(type);
      type = type1;
    }

    lsDestroy(enumConstList, free);
    
    if (name)
      FREE(name);
    return type;
  }
  
}

/**Function********************************************************************

  Synopsis           [Define new range data types]

  Description        [This function creates new data types. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Type_t*
PrsTypeRangeAlloc(
  char* name,
  int range,
  Var_TypeManager_t* typeManager)
{
  Var_Type_t * type, *type1;
  
  type = PrsTypeCheckName(name, typeManager);
  
  if (type){
    if (name)
      FREE(name);
    return type;
  }

  type = Var_VarTypeRangeAlloc(name, range+1);
  type = Var_TypeManagerAddType(typeManager, type);

  if (name)
    FREE(name);
  return type;
}

/**Function********************************************************************

  Synopsis           [Define new array data types]

  Description        [This function creates new data types. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Type_t*
PrsTypeArrayAlloc(
  char* name,
  Var_Type_t *indexType,
  Var_Type_t *entryType,
  Var_TypeManager_t* typeManager)
{
  Var_Type_t *type;

  type = PrsTypeCheckName(name, typeManager);

  if (type){
    if (name)
      FREE(name);
    return type;
  }

  if (indexType == NIL(Var_Type_t) || entryType == NIL(Var_Type_t)) {
    Main_MochaErrorPrint("Error at line %d: type defined incorrectly.\n", yylineno);
    if (name)
      FREE(name);
    return NIL(Var_Type_t);
  }
  
  type = Var_VarTypeArrayAlloc(name, indexType, entryType);
  type = Var_TypeManagerAddType(typeManager, type);
  
  if (name)
    FREE(name);
  return type;
}

/**Function********************************************************************

  Synopsis           [Define new bitvector data types]

  Description        [This function creates new data types. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Type_t*
PrsTypeBitvectorAlloc(
  char* name,
  int size,
  Var_TypeManager_t* typeManager)
{
  Var_Type_t * type, *type1;
  
  type = PrsTypeCheckName(name, typeManager);
  
  if (type) {
    if (name)
      FREE(name);
    return type;
  }
  
  type = Var_VarTypeBitvectorAlloc(name, size);
  type = Var_TypeManagerAddType(typeManager, type);

  if (name)
    FREE(name);
  return type;
}

/**Function********************************************************************

  Synopsis           [Define new bitvector data types]

  Description        [This function creates new data types. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Type_t*
PrsTypeBitvectorNAlloc(
  char* name,
  int size,
  Var_TypeManager_t* typeManager)
{
  Var_Type_t * type, *type1;
  
  type = PrsTypeCheckName(name, typeManager);
  
  if (type) {
    if (name)
      FREE(name);
    return type;
  }
  
  type = Var_VarTypeBitvectorNAlloc(name, size);
  type = Var_TypeManagerAddType(typeManager, type);

  if (name)
    FREE(name);
  return type;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Check the typename when defining a new data type]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static Var_Type_t*
PrsTypeCheckName(
  char *typeName,
  Var_TypeManager_t* typeManager)
{
  Var_Type_t* type;

  type = Var_TypeManagerReadTypeFromName(typeManager, typeName);
  
  if (type != NIL(Var_Type_t)) {
    Main_MochaErrorPrint("Warning at line %d: type \"%s\" defined. Ignored.\n",
                         yylineno, typeName);
  }
  
  return type;
}







