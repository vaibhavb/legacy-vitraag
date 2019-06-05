/**CFile***********************************************************************

  FileName    [var.c]

  PackageName [var]

  Synopsis    [Routines for manipulating variables and types.]

  Description [This file contains routines for manipulating variables and
  types.]

  SeeAlso     [var.h varInt.h varMain.c]

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

static int VarTypeCmp(char *key1, char *key2);
static int VarTypeHash(char *key, int modulus);
static int VarObtainVarTypeHashValue(Var_Type_t * type, int modulus);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Returns the type table from the type manager]

  Description        [This functions returns the var type table from the type
  manager. Users should not free the table.]

  SideEffects        [required]

******************************************************************************/
st_table *
Var_TypeManagerReadVarTypeTable(
  Var_TypeManager_t *typeManager)
{
  return typeManager->VarTypeTable;
}

/**Function********************************************************************

  Synopsis           [Check if the type is compatible with number constants.]

  Description        [This function checks if the give type is compatible with
  number constants. This is used in type checking, where, for instance, a
  variable of type Range can add to a number constant. Other compatible types
  are Integer, Natural, Bitvector and NumConst.]

  SideEffects        [required]

******************************************************************************/
boolean
Var_TypeIsCompatibleWithNumConst(
  Var_Type_t *type)
{
  Var_DataType dataType = Var_VarTypeReadDataType(type);
  
  switch (dataType) {
      case Var_Array_c:
      case Var_Boolean_c:
      case Var_Enumerated_c:
      case Var_Event_c:
        return FALSE;
        break;
      case Var_Integer_c:
      case Var_Natural_c:
      case Var_Range_c:
      case Var_Bitvector_c:
      case Var_BitvectorN_c:
      case Var_NumConst_c:
      case Var_Timer_c:
      case Var_TimeIncrement_c:
        return TRUE;
        break;
      default:
        fprintf(stderr, "unexpected type in Var_TypeIsCompatibleWithNumConst.");
        exit(1);
  }
}


/**Function********************************************************************

  Synopsis    [Allocates a new variable type.]

  Description [Allocates a new variable type.  The name string
  is copied for internal use. The user is responsible for freeing
  the original string. Returns a pointer to
  the variable if success. Otherwise, NIL(Var_Type_t) is returned. ]

  SideEffects []

  SeeAlso     [Var_VariableAddRangeInfo Var_VariableFree]

******************************************************************************/
Var_Type_t *
Var_VarTypeBasicAlloc(
  char *name,
  Var_DataType datatype)
{
  Var_Type_t *type;

  type = ALLOC(Var_Type_t, 1);
  type->creationTime = Main_ReturnCreationTime();
  if (name)
    type->name = util_strsav(name);
  else
    type->name = NIL(char);
  type->dataType = datatype;

  return type;
}
/**Function********************************************************************

  Synopsis    [Allocates a new variable type for a timer variable.]

  Description [Allocates a new variable type for a timer variable.   Returns a pointer to
  the variable if success. Otherwise, NIL(Var_Type_t) is returned. ]

  SideEffects []

  SeeAlso     [ Var_VariableFree]

******************************************************************************/

Var_Type_t *
Var_VarNewTimerTypeAlloc(
)
{
  Var_Type_t *type;
  char *timerName;
  static int timerCounter = 1;

  
  type = ALLOC(Var_Type_t,1);
  type->creationTime = Main_ReturnCreationTime();
  type->name = ALLOC(char, 8 + (int) ( log10((double)(timerCounter)))   );
  strcpy(type->name,"");
  sprintf(type->name,"timer_%d",timerCounter);
  type->dataType = Var_Timer_c;

  type->typeData.range = 0;

  timerCounter++;
  return type;
}
/**Function********************************************************************

  Synopsis    [Allocates a variable type for the time increment variable.]

  Description [Allocates a variable type for the time increment variable.]

  SideEffects []

  SeeAlso     []

******************************************************************************/

Var_Type_t *
Var_VarTimeIncrementTypeAlloc(
long maxIncrement
)
{
  Var_Type_t *type;
  
  type = ALLOC(Var_Type_t,1);
  type->name = util_strsav("timeIncrementType");
  type->dataType = Var_TimeIncrement_c;

  type->typeData.range = maxIncrement;

  return type;
}

/**Function********************************************************************

  Synopsis    [Allocates an array type.]

  Description [Allocates a new array type.  The name string is copied for
  internal use. The user is responsible for freeing the original
  string. Returns a pointer to the type if success. Otherwise, NIL(Var_Type_t)
  is returned. ]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Var_Type_t *
Var_VarTypeArrayAlloc(
  char *name,
  Var_Type_t *indexType,
  Var_Type_t *entryType)
{
  Var_Type_t *type = Var_VarTypeBasicAlloc(name, Var_Array_c);

  type->typeData.arrayData.indexType = indexType;
  type->typeData.arrayData.entryType = entryType;

  return type;
}

/**Function********************************************************************

  Synopsis    [Allocates a bitvector type.]

  Description [Allocates a new bitvector type.  The name string
  is copied for internal use. The user is responsible for freeing
  the original string. Returns a pointer to
  the type if success. Otherwise, NIL(Var_Type_t) is returned. ]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Var_Type_t *
Var_VarTypeBitvectorAlloc(
  char *name,
  int size)
{
  Var_Type_t *type = Var_VarTypeBasicAlloc(name, Var_Bitvector_c);
  
  type->typeData.size = size;

  return type;
}

/**Function********************************************************************

  Synopsis    [Allocates a bitvectorN type.]

  Description [Allocates a new bitvector type.  The name string
  is copied for internal use. The user is responsible for freeing
  the original string. Returns a pointer to
  the type if success. Otherwise, NIL(Var_Type_t) is returned. ]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Var_Type_t *
Var_VarTypeBitvectorNAlloc(
  char *name,
  int size)
{
  Var_Type_t *type = Var_VarTypeBasicAlloc(name, Var_BitvectorN_c);
  
  type->typeData.size = size;

  return type;
}

/**Function********************************************************************

  Synopsis    [Allocates an enumerated type.]

  Description [Allocates a new enumerated type.  The name string
  is copied for internal use. The user is responsible for freeing
  the original string. Returns a pointer to
  the type if success. Otherwise, NIL(Var_Type_t) is returned. ]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Var_Type_t *
Var_VarTypeEnumeratedAlloc(
  char *name,
  array_t *indexToValue,
  st_table *valueToIndex)
{
  Var_Type_t *type = Var_VarTypeBasicAlloc(name, Var_Enumerated_c);
  
  type->typeData.enumData.indexToValue = indexToValue;
  type->typeData.enumData.valueToIndex = valueToIndex;

  return type;
}
  

/**Function********************************************************************

  Synopsis    [Allocates a range type.]

  Description [Allocates a new rnage type.  The name string
  is copied for internal use. The user is responsible for freeing
  the original string. Returns a pointer to
  the type if success. Otherwise, NIL(Var_Type_t) is returned. ]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Var_Type_t *
Var_VarTypeRangeAlloc(
  char *name,
  int range)
{
  Var_Type_t *type = Var_VarTypeBasicAlloc(name, Var_Range_c);

  type->typeData.range = range;

  return type;
}
  
/**Function********************************************************************

  Synopsis    [Makes a copy of a variable type.]

  Description [Makes a copy of a variable type, which is then assigned a new
  name. Typically, the new type will be put into the typetable afterwards. The
  name can be NIL(char).]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Var_Type_t *
Var_VarTypeDup(
  Var_Type_t *type,
  char *name)
{
  switch (type->dataType) {
      case Var_Array_c :
        return Var_VarTypeArrayAlloc(name, type->typeData.arrayData.indexType,
                                     type->typeData.arrayData.entryType);

      case Var_Bitvector_c :
        return Var_VarTypeBitvectorAlloc(name, type->typeData.size);

      case Var_BitvectorN_c :
        return Var_VarTypeBitvectorNAlloc(name, type->typeData.size);
        
      case Var_Boolean_c :
        return Var_VarTypeBasicAlloc(name, Var_Boolean_c);

      case Var_Event_c :
        return Var_VarTypeBasicAlloc(name, Var_Event_c);

      case Var_Integer_c :
        return Var_VarTypeBasicAlloc(name, Var_Integer_c);

      case Var_Natural_c :
        return Var_VarTypeBasicAlloc(name, Var_Natural_c);

      case Var_Enumerated_c :
        return Var_VarTypeEnumeratedAlloc(name,
                                          array_dup(type->typeData.enumData.indexToValue),
                                          st_copy(type->typeData.enumData.valueToIndex));
    
      case Var_Range_c :
        return Var_VarTypeRangeAlloc(name, type->typeData.range);
  }
}

/**Function********************************************************************

  Synopsis    [Frees a variable type]

  Description [It frees a variable type. For enumerative types, it is the
  users responsbility to free the enum constants before freeing this type.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void
Var_VarTypeFree(
  Var_Type_t *type)
{
  if (type->name)
    FREE(type->name);

  if (type->dataType == Var_Enumerated_c) {
    
    if (type->typeData.enumData.indexToValue != NIL(array_t))
     array_free(type->typeData.enumData.indexToValue);
    
    if (type->typeData.enumData.valueToIndex != NIL(st_table))
      st_free_table(type->typeData.enumData.valueToIndex);
    
  }
  
  FREE(type);
}


/**Function********************************************************************

  Synopsis    [Allocates a new variable.]

  Description [Allocates a new variable.  The arguments of this function are a
  pointer to the module to which a new variable belongs, name of the variable,
  and a pointer to the type of the variable. The name string is copied for
  internal use. The user is responsible for freeing the original string.
  Returns a pointer to the variable if success. Otherwise, NIL(Var_Variable_t)
  is returned. ]

  SideEffects []

  SeeAlso     [Var_VariableFree]

******************************************************************************/
Var_Variable_t *
Var_VariableAlloc(
  VarModuleGeneric module, 
  char *name,
  Var_Type_t *type)
{
  Var_Variable_t *var;

  var = ALLOC(Var_Variable_t,1);
  if (name)
    var->name = util_strsav(name);
  else
    var->name = NIL(char);
  var->module = module;
  
  var->varType = type;
  var->peid  = 0; /* assume private variable */
  var->ctrlByAtom = (VarAtomGeneric) 0;
  var->readByAtomList = lsCreate();
  var->awaitByAtomList = lsCreate();
  return var;

}


/**Function********************************************************************

  Synopsis    [Frees a variable.]

  Description [It frees a variable structure.]

  SideEffects []

  SeeAlso     [Var_VariableAlloc]

******************************************************************************/
int
Var_VariableFree(Var_Variable_t *var)
{
  
  if (var != NIL(Var_Variable_t)){
    FREE(var->name);
    lsDestroy (var->readByAtomList, (void (*) ()) 0);
    lsDestroy (var->awaitByAtomList, (void (*) ()) 0);
    FREE(var);

    return 1;
  }
  else
    return 0;

}

/**Function********************************************************************

  Synopsis    [Returns the name of a variable.]

  Description [It reads the name of a variable. User should not free the
  name. ]

  SideEffects [None]

******************************************************************************/
char *
Var_VariableReadName(
  Var_Variable_t *var)
{
  return var->name;
}


/**Function********************************************************************

  Synopsis    [Returns the index type of an array data type.]

  Description [The function returns the index type of an array data type.]

  SideEffects [None]

  SeeAlso     [Var_VarTypeReadEntryType]

******************************************************************************/
Var_Type_t *
Var_VarTypeReadIndexType(
  Var_Type_t *varType)
{
  if (Var_TypeIsBV(varType)) {
    fprintf(stderr,
            "Error: argument is not array in Var_VarTypeReadIndexType.\n");
    exit(1);
    /* FIXME: */
    /* return Var_RangeTypeReadFromRange(typeManager, varType -> size); */
  } else if (Var_TypeIsArray(varType)) {
    return varType->typeData.arrayData.indexType;
  } else {
    fprintf(stderr,
            "Error: argument is not array nor bitvector in Var_VarTypeReadIndexType.\n");
    exit(1);
  }
  
}



/**Function********************************************************************

  Synopsis    [Returns the entry type of an array data type.]

  Description [The function returns the entry type of an array data type.]

  SideEffects [None]

  SeeAlso     [Var_VarTypeReadIndexType]

******************************************************************************/
Var_Type_t *
Var_VarTypeReadEntryType(
  Var_Type_t *varType)
{
  if (Var_TypeIsBV(varType)) {
    return BoolType;
  } else if (Var_TypeIsArray(varType)) {
    return varType->typeData.arrayData.entryType;
  } else {
    fprintf(stderr,
            "Error: argument is not array nor bitvector in Var_VarTypeReadEntryType.\n");
    exit(1);
  }
  
}


/**Function********************************************************************

  Synopsis    [Returns the name of the symbolic constant associated with an
               integer index for an enumerated data type.] 

  Description [The function returns the name of the symbolic constant
               associated with an integer index for an enumerated data
               type. For other types, it returns NIL(Var_EnumElement_t).]

  SideEffects [None]

  See Also    [Var_VarTypeReadIndexFromSymbolicValue]

******************************************************************************/
Var_EnumElement_t *
Var_VarTypeReadSymbolicValueFromIndex(
  Var_Type_t *type,
  int i)
{
  if (type->dataType == Var_Enumerated_c)
    return array_fetch(Var_EnumElement_t *,
                       type->typeData.enumData.indexToValue, i);
  return NIL(Var_EnumElement_t);
}


/**Function********************************************************************

  Synopsis    [Returns the integer index associated with a symbolic constant
               for an enumerated data type.] 

  Description [The function returns the integer index associated with a
               symbolic constant for and enumerated data type. If the data
               type is not enumerated or the symbolic name is not valid, the
               routine returns -1.] 

  SideEffects [None]

  SeeAlso     [Var_VarTypeReadSymbolicValueFromIndex]

******************************************************************************/
int
Var_VarTypeReadIndexFromSymbolicValue(
  Var_Type_t *type,
  Var_EnumElement_t *symValue)
{
  long  index;

  if (type->dataType == Var_Enumerated_c) 
    if (st_lookup(type->typeData.enumData.valueToIndex, (char *) symValue,
                  (char **) &index)) 
      return (int) index;

  return -1;
}


/**Function********************************************************************

  Synopsis    [Returns the module to which a variable belongs.]

  Description [It returns a pointer to the module to which this variable
  belongs. Since the the module structure is defined in the mdl package, which
  is "higher" than the var package, the pointer to the module is casted to
  VarModuleGeneric. User should cast the pointer to Mdl_Module_t* when this
  function is used.]

  SideEffects [User should not free the module.]

  SeeAlso     []

******************************************************************************/
VarModuleGeneric
Var_VariableReadModule(
  Var_Variable_t *var)
{
  return var->module;
}

/**Function********************************************************************

  Synopsis    [Returns a pointer to the type of a variable.]

  Description [Returns a pointer to the type of a variable.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Var_Type_t*
Var_VariableReadType(
  Var_Variable_t *var)
{
  return var->varType;
}

/**Function********************************************************************

  Synopsis    [Returns a string containing the type name of a variable.]

  Description [Returns a string containing the type name of a variable.]

  SideEffects [The string should not be freed.]

  SeeAlso     []

******************************************************************************/
char*
Var_VariableReadTypeName(
  Var_Variable_t *var)
{
  return var->varType->name;
}

/**Function********************************************************************

  Synopsis           [returns an integer indicating the class of the variable.]

  Description        [returns 0 if private, 1 if external, 2 if interface, and
  3 if it is a dummy variable.]

  SideEffects        [required]

******************************************************************************/
int
Var_VariableReadPEID(
  Var_Variable_t* var)
{
  return (var->peid);
}

/**Function********************************************************************

  Synopsis           [set the class type of the variable.]

  Description        [set the class(private/external/interface/dummy) of the 
                      variable.]

  SideEffects        [required]

******************************************************************************/
void
Var_VariableSetPEID(
  Var_Variable_t* var,
  int peid)
{
  var->peid = peid;
}

/**Function********************************************************************

  Synopsis           [Set a pointer to the atom which controls this atom.]

  Description        [The pointer, ctrlByAtom, is pointing to the atom that
                      controls this atom.  This routine set this pointer
		      to the appropriate atom.]

  SideEffects        [required]

******************************************************************************/
void
Var_VariableSetControlAtom(
  Var_Variable_t* var,
  VarAtomGeneric atm)
{
  var->ctrlByAtom = atm;
}
/**Function********************************************************************

  Synopsis           [Returns the number of values a variable of this type can take] 

  Description        [The function returns the number of values a variable of
  this type can take - this makes sensse only for timer and time increment data
  types ]

  SideEffects        [None]

  SeeAlso            [optional]

******************************************************************************/
int 
Var_VariableTypeReadNumVals(
  Var_Type_t* type		    
)
{
  if ((type->dataType != Var_Timer_c) &&
      (type->dataType != Var_TimeIncrement_c)) {  
    Main_MochaErrorPrint( "var.c: This variable type does not have the numValues field\n");
  }
  else {
    return (type->typeData.range);
  }
}
/**Function********************************************************************

  Synopsis           [Modifies the number of values a variable of this type can take] 

  Description        [The function modifies the number of values a variable of
  this type can take - this makes sensse only for timer and range data
  types ]

  SideEffects        [None]

  SeeAlso            [optional]

******************************************************************************/
int 
Var_VariableTypeSetNumVals(
  Var_Type_t* type,
  int         numValues
)
{
  if ((type->dataType != Var_Timer_c) &&
      (type->dataType != Var_TimeIncrement_c)) {  
    Main_MochaErrorPrint( "var.c: This variable type does not have the numValues field\n");
  }
  else {
    type->typeData.range = numValues;
  }
}

/**Function********************************************************************

  Synopsis           [Return a pointer to the list of atoms reading this var.]

  Description        [It returns the pointer pointing to the list of
                      atoms reading the variable.  User should not free this
		      pointer.]

  SideEffects        [required]

******************************************************************************/
lsList
Var_VariableReadReadList(
  Var_Variable_t* var)
{
  return(var->readByAtomList);
}


/**Function********************************************************************

  Synopsis           [Return a pointer to the list of atoms awaiting this var.]

  Description        [It returns the pointer pointing to the list of
                      atoms awaiting the variable.  User should not free this
		      pointer.]

  SideEffects        [required]

******************************************************************************/
lsList
Var_VariableReadAwaitList(
  Var_Variable_t* var)
{
  return(var->awaitByAtomList);
}

/**Function********************************************************************

  Synopsis           [Add an atom to the readByAtomList.]

  Description        [Add an atom to the readByAtomList.  Note since atm
  package is not included in this package, user should cast the pointer to the
  atom to VarAtomGeneric before calling this function.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Var_VariableAddToReadByAtomList(
  Var_Variable_t* var,
  VarAtomGeneric atom)
{
  lsNewEnd(var->readByAtomList, (lsGeneric) atom, NIL(lsHandle));
}


/**Function********************************************************************

  Synopsis           [Return a pointer to the atom controlling this var.]

  Description        [This function returns the pointer to the atom that
  controls this variable. If the atom is an external variable, the pointer is
  NIL.]

  SideEffects        [required]

******************************************************************************/
VarAtomGeneric
Var_VariableReadControlAtom(
  Var_Variable_t* var)
{
  if (var == NIL(Var_Variable_t)) return NIL(char);
  
  return (var->ctrlByAtom);

}


/**Function********************************************************************

  Synopsis           [Add an atom to the awaitByAtomList.]

  Description        [Add an atom to the awaitByAtomList.  Note since atm
  package is not included in this package, user should cast the pointer to the
  atom to VarAtomGeneric before calling this function.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
Var_VariableAddToAwaitByAtomList(
  Var_Variable_t* var,
  VarAtomGeneric atom
)
{
  lsNewEnd(var->awaitByAtomList, (lsGeneric) atom, NIL(lsHandle));
}


/**Function********************************************************************

  Synopsis    [Changes the name of a variable.]

  Description [Changes the name of a variable.]

  SideEffects [The original name is freed. A copy of the new name is
  stored.]

  SeeAlso     []

******************************************************************************/
void
Var_VariableChangeName(
  Var_Variable_t *var,
  char *name)
{
  FREE(var->name);
  var->name = util_strsav(name); 
}

/**Function********************************************************************

  Synopsis           [Checks if the variable is a boolean variable.]

  Description        [Returns TRUE if the variable is boolean, FALSE
  otherwise. ]

  SideEffects        [required]

******************************************************************************/
boolean
Var_VariableIsBoolean(
  Var_Variable_t *var)
{
  if(!strcmp(var->varType->name,"bool"))
    return TRUE;
  else
    return FALSE;
}

/**Function********************************************************************

  Synopsis           [Init the type manager]

  Description        [This function initializes the type manager. The variable
  type table and the enumerative elements table are initialized. Built in
  variable types, namely bool, int, nat and event are created and hashed in
  the variable type table. It returns the type manager.]

  SideEffects        [required]

******************************************************************************/
Var_TypeManager_t*
Var_TypeManagerInit()
{
  Var_TypeManager_t* typemanager;
  Var_Type_t* type;

  typemanager = ALLOC(Var_TypeManager_t, 1);
  typemanager -> VarTypeTable =  st_init_table (VarTypeCmp, VarTypeHash);
  typemanager -> VarEnumElementsTable = st_init_table (strcmp, st_strhash);

  /* initialize built-in types */
  BoolType = Var_VarTypeBasicAlloc("bool", Var_Boolean_c);
  Var_TypeManagerAddType(typemanager, BoolType);
  
  IntType = Var_VarTypeBasicAlloc("int", Var_Integer_c);
  Var_TypeManagerAddType(typemanager, IntType);

  NatType = Var_VarTypeBasicAlloc("nat", Var_Natural_c);
  Var_TypeManagerAddType(typemanager, NatType);

  EventType = Var_VarTypeBasicAlloc("event", Var_Event_c);
  Var_TypeManagerAddType(typemanager, EventType);
  
  return typemanager;
}

/**Function********************************************************************

  Synopsis           [Free the type mananager]

  Description        [This frees all the variable types first, then the type
  table and the enum constant table, as well as the typemanager itself.]

  SideEffects        [required]


******************************************************************************/
void
Var_TypeManagerFree(
  Var_TypeManager_t *typemanager)
{
  st_table* varTypeTable;
  st_table* typeConstTable;
  Var_Type_t* vartype;
  Var_EnumElement_t* typeconst;
  st_generator* gen;
  char* key;
    
  varTypeTable = typemanager->VarTypeTable;
  st_foreach_item(varTypeTable, gen, &key, (char**) &vartype){
    Var_VarTypeFree(vartype);
/*    FREE(key); */  
  }
  st_free_table(varTypeTable);

  typeConstTable = typemanager -> VarEnumElementsTable;
  st_foreach_item(typeConstTable, gen, &key, (char**) &typeconst){
    Var_EnumElementFree(typeconst);
    FREE(key);
  }
  
  st_free_table(typeConstTable);

  FREE(typemanager);
  
}

/**Function********************************************************************

  Synopsis           [Add a variable type into the type manager.]

  Description        [This function adds a type into the type manager. First
  it looks up if the type is already defined. If so, return the pointer to the
  defined one, and frees the given one. If not, add the type into the type
  manager. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Type_t *
Var_TypeManagerAddType(
  Var_TypeManager_t* typeManager,
  Var_Type_t* type)
{
  Var_Type_t * type1;

  if (type1 = Var_TypeManagerObtainType(typeManager, type)) {
    Var_VarTypeFree(type);
    type = type1;
  }
  else {
    st_insert(typeManager->VarTypeTable, (char*) type, (char*) type);  
  }

  return type;
}

/**Function********************************************************************

  Synopsis           [Return the name of the variable type.]

  Description        [The user should not free the returned string.]

  SideEffects        [required]

******************************************************************************/
char*
Var_VarTypeReadName(
  Var_Type_t* type)
{
  return type->name;
} 

 
/**Function********************************************************************

  Synopsis           [Returns the size of the domain of the variable type.]

  Description        [The function returns the size of the domain of a variable
                      type for enumerated and range data types. For other
                      variable types it returns -1.]

  SideEffects        [None]

******************************************************************************/
int 
Var_VarTypeReadDomainSize(
  Var_Type_t* type)
{
  if (type->dataType == Var_Enumerated_c)
    return array_n(type->typeData.enumData.indexToValue);
  if ((type->dataType == Var_Range_c) || (type->dataType == Var_Timer_c))
    return type->typeData.range;
  return -1;
}
 
 
/**Function********************************************************************

  Synopsis           [Returns the size of the domain of the variable type.]

  Description        [The function returns the size of the domain of a variable
                      type for enumerated and range data types. For other
                      variable types it returns -1.]

  SideEffects        [None]

******************************************************************************/
int 
Var_VarTypeArrayOrBitvectorReadSize(
  Var_Type_t* type)
{
  if (type->dataType == Var_Bitvector_c)
    return type->typeData.size;

  if (type->dataType == Var_BitvectorN_c)
    return type->typeData.size;

  if (type->dataType == Var_Array_c)
    return Var_VarTypeReadDomainSize(type->typeData.arrayData.indexType);
  return -1;
}

/**Function********************************************************************

  Synopsis           [Allocates a new symbolic constant for an enumerated data
                      type.]

  Description        [Allocates a new symbolic constant for an enumerated data
                      type. It makes a copy of name. Hence, it is the user's
                      responsibility to free his copy.]

  SideEffects        [None]

******************************************************************************/
Var_EnumElement_t *
Var_EnumElementAlloc(
  char *name,
  Var_Type_t *dataType)
{
  Var_EnumElement_t *element = ALLOC(Var_EnumElement_t, 1);

  element->name = util_strsav(name);
  element->dataType = dataType;

  return element;
}


/**Function********************************************************************

  Synopsis           [For enumerative type, add a new type element.]

  Description        [If successful, return a pointer to the new element.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeManagerAddEnumElement(
  Var_TypeManager_t *typeManager,
  Var_EnumElement_t *element)
{
  
  if(Var_TypeManagerReadEnumElementFromName(typeManager, Var_EnumElementReadName(element)) != 
     NIL(Var_EnumElement_t))  
    return FALSE;
  
  st_insert(typeManager->VarEnumElementsTable, 
	    util_strsav(Var_EnumElementReadName(element)), (char*) element);
  
  return TRUE;
}

/**Function********************************************************************

  Synopsis           [Frees an eumerative type constant.]

  Description        [It frees an enumerative constant.]

  SideEffects        [required]

  SeeAlso            [optional]
  
******************************************************************************/
void
Var_EnumElementFree(
  Var_EnumElement_t *typeconst
  )
{
  FREE(typeconst->name);
  FREE(typeconst);
}


/**Function********************************************************************

  Synopsis           [Returns a pointer to the element given a name.]

  Description        [for enumerative type, given a name and the typemanager,
                      this function first lookup the type_element_table, and
		      return a pointer the the element if found.

		      Returns NIL(Var_EnumElement_t) if the element is not
		      found.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_EnumElement_t*
Var_TypeManagerReadEnumElementFromName(
  Var_TypeManager_t *typeManager,
  char *name)
{
  Var_EnumElement_t* typeconst;
  
  if (typeManager == NIL(Var_TypeManager_t)) 
    return NIL(Var_EnumElement_t);

  if(st_lookup(typeManager->VarEnumElementsTable, name,
	       (char**) &typeconst)) 
    return (typeconst);
  else
    return NIL(Var_EnumElement_t);
    
}
			       

/**Function********************************************************************

  Synopsis           [Returns a pointer to the type of an given element.]

  Description        [Should not free the pointer.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Type_t*
Var_EnumElementReadType(
  Var_EnumElement_t* element
)
{
  if (element != NIL(Var_EnumElement_t))
    return (element -> dataType);
  else
    return NIL(Var_Type_t);
}


/**Function********************************************************************

  Synopsis           [Returns the name of an element.]

  Description        [This function returns a pointer to the name of the
                      enumerative type constant.  User should not free
                      or modify the string. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
char*
Var_EnumElementReadName(
  Var_EnumElement_t* element)
{
    return element->name;
}


/**Function********************************************************************

  Synopsis           [Returns a pointer to the datatype struct given the name.]

  Description        [Should not free the pointer.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Type_t*
Var_TypeManagerReadTypeFromName(
  Var_TypeManager_t* typeManager,
  char* name)
{
  Var_Type_t* type, *ttype;

  if (!name) {
    type = NIL(Var_Type_t);
  } else {
    /* Var_Boolean_c is just dummy */
    ttype = Var_VarTypeBasicAlloc(name, Var_Boolean_c);
    
    if(!st_lookup(typeManager->VarTypeTable, (char *) ttype, (char**) &type))
      type = NIL(Var_Type_t);
    
    Var_VarTypeFree(ttype);
  }
  
  return type;
}

/**Function********************************************************************

  Synopsis           [Returns a pointer to the datatype struct.]

  Description        [Should not free the pointer.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Type_t*
Var_TypeManagerObtainType(
  Var_TypeManager_t* typeManager,
  Var_Type_t* type)
{
  Var_Type_t * ttype;

  if (!st_lookup(typeManager->VarTypeTable, (char *) type, (char**) &ttype)) {
    return (NIL(Var_Type_t));
  } else {
    return ttype;
  }
}


/**Function********************************************************************

  Synopsis           [Returns the data type of a varable type.]

  Description        [The function returns the data type of a varable type.]

  SideEffects        [None]

  SeeAlso            [optional]

******************************************************************************/
Var_DataType
Var_VarTypeReadDataType(
  Var_Type_t* type		    
)
{
  return (type->dataType);

}

/**Function********************************************************************

  Synopsis           [Returns the data type of a variable.]

  Description        [The function returns the data type of the variable.]

  SideEffects        [None]

  SeeAlso            [optional]

******************************************************************************/
Var_DataType
Var_VariableReadDataType(
  Var_Variable_t* var)
{
  return (var->varType->dataType);
}


/**Function********************************************************************

  Synopsis           [Returns the enumerated element of a symbolic variable
                      from its index.]

  Description        [The function returns the enumerated element of a
                      symbolic variable from its index if the index is within
                      the the domain size of the variable. If the variable is
                      not symbolic or the index is not within the domain size
                      NIL(Var_EnumElemant_t) is returned.]

  SideEffects        [None]

  See Also           [Var_EnumElementObtainIndex]

******************************************************************************/
Var_EnumElement_t *
Var_VariableObtainEnumElementFromIndex(
  Var_Variable_t *var,
  int index)
{
  if (var->varType->dataType != Var_Enumerated_c)
    return NIL(Var_EnumElement_t);
  if (index >= Var_VarTypeReadDomainSize(Var_VariableReadType(var)))
    return NIL(Var_EnumElement_t);
  
  return array_fetch(Var_EnumElement_t *,
                     var->varType->typeData.enumData.indexToValue, index);
}

/**Function********************************************************************

  Synopsis           [Returns the index of the enumerated element of a
                      symbolic variable.]

  Description        [The function returns the index of the enumerated element
                      of a symbolic variable, if successful. Otherwise, it
                      returns -1.] 

  SideEffects        [None]

  See Also           [Var_VariableObtainEnumElementFromIndex]

******************************************************************************/
int
Var_EnumElementObtainIndex(
  Var_EnumElement_t *element)
{
  int index;

  if(st_lookup(element->dataType->typeData.enumData.valueToIndex, (char *)
               element, (char **) &index))
    return index;
  else
    return -1;
}

/**Function********************************************************************

  Synopsis           [Checks if the variable is history free.]

  Description        [The function returns TRUE if the variable is history
                      free, otherwise it returns FALSE. There are three
                      categories in which the set of variables is partitioned
                      - event variables, history free variables and history
                      dependent variables.] 

  SideEffects        [None]

******************************************************************************/
boolean
Var_VariableIsHistoryFree(
  Var_Variable_t* var)
{
  if (var->varType->dataType == Var_Timer_c)
    return FALSE;
  if (var->varType->dataType == Var_TimeIncrement_c)
    return TRUE;
  if(lsLength(var->readByAtomList))
    return FALSE;
  else
    return TRUE;
}

/**Function********************************************************************

  Synopsis           [Checks if the variable is history dependent.]

  Description        [The function returns TRUE if the variable is history
                      dependent, otherwise it returns FALSE. There are three
                      categories in which the set of variables is partitioned
                      - event variables, history free variables and history
                      dependent variables.] 

  SideEffects        [None]

******************************************************************************/
boolean
Var_VariableIsHistoryDependent(
  Var_Variable_t *var)
{
  if((var->varType->dataType == Var_Event_c) ||
     (var->varType->dataType == Var_TimeIncrement_c) )
    return FALSE;
  if(var->varType->dataType == Var_Timer_c)
    return TRUE;
  if(lsLength(var->readByAtomList))
    return TRUE;
  else
    return FALSE;
}
  

/**Function********************************************************************

  Synopsis           [Checks if a variable is an event variable.]

  Description        [The function returns TRUE if the variable is an event
                      variable, otherwise it returns FALSE. There are three
                      categories in which the set of variables is partitioned
                      - event variables, history free variables and history
                      dependent variables.] 

  SideEffects        [None]

******************************************************************************/
boolean
Var_VariableIsEvent(
  Var_Variable_t* var)
{
  return (var->varType->dataType == Var_Event_c);
}  


/**Function********************************************************************

  Synopsis           [Checks if a variable is a dummy variable.]

  Description        [The function returns TRUE if the variable is an external
                      variable, otherwise it returns FALSE. ]

  SideEffects        [None]

******************************************************************************/
boolean
Var_VariableIsDummy(
  Var_Variable_t* var)
{
  if (var->peid == 3)
    return TRUE;
  else 
    return FALSE;
}  

/**Function********************************************************************

  Synopsis           [Checks if a variable is a private variable.]

  Description        [The function returns TRUE if the variable is an private
                      variable, otherwise it returns FALSE. ]

  SideEffects        [None]

******************************************************************************/
boolean
Var_VariableIsPrivate(
  Var_Variable_t* var)
{
  if (var->peid == 0)
    return TRUE;
  else 
    return FALSE;
}  


/**Function********************************************************************

  Synopsis           [Checks if a variable is an interface variable.]

  Description        [The function returns TRUE if the variable is an interface
                      variable, otherwise it returns FALSE. ]

  SideEffects        [None]

******************************************************************************/
boolean
Var_VariableIsInterface(
  Var_Variable_t* var)
{
  if (var->peid == 2)
    return TRUE;
  else 
    return FALSE;
}  


/**Function********************************************************************

  Synopsis           [Checks if a variable is an external variable.]

  Description        [The function returns TRUE if the variable is an external
                      variable, otherwise it returns FALSE. ]

  SideEffects        [None]

******************************************************************************/
boolean
Var_VariableIsExternal(
  Var_Variable_t* var)
{
  if (var->peid == 1)
    return TRUE;
  else 
    return FALSE;
}  

/**Function********************************************************************

  Synopsis           [Checks if a variable is an external variable.]

  Description        [The function returns TRUE if the variable is an external
                      variable, otherwise it returns FALSE. ]

  SideEffects        [None]

******************************************************************************/
array_t *
Var_VariableReadIndexToValue(
  Var_Variable_t* var)
{
  return var->varType->typeData.enumData.indexToValue;
}  

/**Function********************************************************************

  Synopsis           [Checks if a variable is an external variable.]

  Description        [The function returns TRUE if the variable is an external
                      variable, otherwise it returns FALSE. ]

  SideEffects        [None]

******************************************************************************/
array_t *
Var_VarTypeReadIndexToValue(
  Var_Type_t* varType)
{
  return varType->typeData.enumData.indexToValue;
}  


/**Function********************************************************************

  Synopsis           [Checks if two types are compatible.]

  Description        [This function returns true if the given two types are
  compatible. It checks if any of the two type is a num_const. If so, check if
  the other one is compatible with a num_const. Otherwise, check if the two
  types are equal. ]

  SideEffects        [required]

  SeeAlso            [Var_TypeIsCompatibleWithNumConst]

******************************************************************************/
boolean
Var_TypesAreCompatible(
  Var_Type_t * type1,
  Var_Type_t * type2
  )
{
  if (type1 == NIL(Var_Type_t) || type2 == NIL(Var_Type_t))
    return FALSE;
    
  if (type1->dataType == Var_NumConst_c)
    return Var_TypeIsCompatibleWithNumConst(type2);

  if (type2->dataType == Var_NumConst_c)
    return Var_TypeIsCompatibleWithNumConst(type1);

  return (type1 == type2);
}

/**Function********************************************************************

  Synopsis           [Checks if a type is an enum type]

  Description        [This function returns true if the type is an enum
  type. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeIsEnum(
  Var_Type_t * type
  )
{
  if (!type) return FALSE;

  return (type->dataType == Var_Enumerated_c);
}

/**Function********************************************************************

  Synopsis           [Checks if a type is an timer type]

  Description        [This function returns true if the type is a timer
  type. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeIsTimer(
  Var_Type_t * type
  )
{
  if (!type) return FALSE;

  return (type->dataType == Var_Timer_c);
}

/**Function********************************************************************

  Synopsis           [Checks if a type is an event type]

  Description        [This function returns true if the type is an event
  type. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeIsEvent(
  Var_Type_t * type
  )
{
  if (!type) return FALSE;

  return (type->dataType == Var_Event_c);
}



/**Function********************************************************************

  Synopsis           [Checks if a type is an range type]

  Description        [This function returns true if the type is an range
  type. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeIsRange(
  Var_Type_t * type
  )
{
  if (!type) return FALSE;

  return (type->dataType == Var_Range_c);
}



/**Function********************************************************************

  Synopsis           [Checks if a type is an array type]

  Description        [This function returns true if the type is an array
  type. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeIsArray(
  Var_Type_t * type
  )
{
  if (!type) return FALSE;

  return (type->dataType == Var_Array_c);
}

/**Function********************************************************************

  Synopsis           [Checks if a type is a bitvector or a bitvector_n.]

  Description        [This function returns true if the type is a bitvector
  type. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeIsBV(
  Var_Type_t * type
  )
{
  if (!type) return FALSE;

  return ((type->dataType == Var_Bitvector_c) ||
          (type->dataType == Var_BitvectorN_c));

}



/**Function********************************************************************

  Synopsis           [Checks if a type is a bitvector or a bitvector_n.]

  Description        [This function returns true if the type is a bitvector
  type. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeIsBitvector(
  Var_Type_t * type
  )
{
  if (!type) return FALSE;

  return (type->dataType == Var_Bitvector_c);
}

/**Function********************************************************************

  Synopsis           [Checks if a type is a bitvector_n.]

  Description        [This function returns true if the type is a bitvector
  type. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeIsBitvectorN(
  Var_Type_t * type
  )
{
  if (!type) return FALSE;

  return (type->dataType == Var_BitvectorN_c);
}

/**Function********************************************************************

  Synopsis           [Checks if a type is a boolean.]

  Description        [This function returns true if the type is a boolean
  type. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeIsBoolean(
  Var_Type_t * type
  )
{
  if (!type) return FALSE;

  return (type->dataType == Var_Boolean_c);
}


/**Function********************************************************************

  Synopsis           [Checks if a type is finite.]

  Description        [This function returns true if the type is a finite
  type. ]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
boolean
Var_TypeIsFinite(
  Var_Type_t * type
  )
{

  if (!type)
    return FALSE;

  switch (type -> dataType) {
      case Var_Boolean_c:
      case Var_Bitvector_c:
      case Var_BitvectorN_c:
      case Var_Event_c:
      case Var_Range_c:
      case Var_Enumerated_c:
        return TRUE;

      case Var_Integer_c:
      case Var_Natural_c:
      case Var_NumConst_c:
      case Var_Timer_c:
      case Var_TimeIncrement_c:
        return FALSE;
        
      case Var_Array_c:
        return Var_TypeIsFinite(Var_VarTypeReadEntryType(type));

      default:
        fprintf(stderr, "Unknown Type encountered in Var_TypeIsFinite\n");
        exit (0);
  }
}




/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Type_t *
Var_RangeTypeReadFromRange (
  Var_TypeManager_t * typeManager,
  int num) 
{
  Var_Type_t * type = Var_VarTypeRangeAlloc(NIL(char), num);

  return Var_TypeManagerObtainType(typeManager, type);
  
}

/**Function********************************************************************

  Synopsis           [It converts a variable type to a string.]

  Description        [This routine prints a type (without name) to a
  string. For example, a range type is printed as "(0..n)", and a bitvector type
  is printed as "bitvector n", where n in both cases refer to the size of the
  data type.  This routine should not be called when the type is one of the
  built-in type, namely, event, bool, int and nat. ]

  SideEffects        [It returns a string. User should free the string after
  use.]

  SeeAlso            []

******************************************************************************/
char *
Var_TypeConvertToString(
  Var_Type_t * type)
{
  char *s, *typename;

  typename = Var_VarTypeReadName(type);
  if (typename) 
    return util_strsav(typename);
  
  switch (Var_VarTypeReadDataType(type)) {
      case Var_Array_c:
      {
        char *indexTypeString = Var_TypeConvertToString(Var_VarTypeReadIndexType(type));
        char *entryTypeString = Var_TypeConvertToString(Var_VarTypeReadEntryType(type));
        
        s = util_strcat4("array ", indexTypeString, " of ", entryTypeString);
        FREE(indexTypeString);
        FREE(entryTypeString);
        return s;
        break;
      }
      case Var_Bitvector_c: 
      {
        char *intString = ALLOC(char, INDEX_STRING_LENGTH);
	sprintf(intString, "%d", Var_VarTypeArrayOrBitvectorReadSize(type));
        s = strcat("bitvector ", intString);
        FREE(intString);
        return s;
        break;
      }

      case Var_BitvectorN_c: 
      {
        char *intString = ALLOC(char, INDEX_STRING_LENGTH);
	sprintf(intString, "%d", Var_VarTypeArrayOrBitvectorReadSize(type));
        s = strcat("bitvector_n ", intString);
        FREE(intString);
        return s;
        break;
      }

      case Var_Range_c: 
      {
        char *intString = ALLOC(char, INDEX_STRING_LENGTH);
	sprintf(intString, "%d", Var_VarTypeReadDomainSize(type)-1);
        s = util_strcat3("(0..", intString, ")");
        FREE(intString);
        return s;
        break;
      }

      case Var_Enumerated_c:
      {
        int i, numValue = Var_VarTypeReadDomainSize(type);
        Var_EnumElement_t *value;
        char *name, *tmpname;

        value = Var_VarTypeReadSymbolicValueFromIndex(type, 0);
        name  = util_strsav(Var_EnumElementReadName(value));
        
        for(i=1; i<numValue; i++) {
          value = Var_VarTypeReadSymbolicValueFromIndex(type, i);
          tmpname  = util_strcat3(name, "," , Var_EnumElementReadName(value));
          FREE(name);
          name = tmpname;
        }
        s = util_strcat3("{",name,"}");
        FREE(name);
        return s;
        break;
      }

      
      case Var_Event_c:
      case Var_Integer_c:
      case Var_Natural_c:
      case Var_Boolean_c:
      case Var_NumConst_c:
      default:
        fprintf(stderr, "Unexpected var datatype in VarTypeHash\n");
        exit(1);
        break;
  }

}



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
VarTypeCmp(
  char *key1,
  char *key2)
{
  Var_Type_t* type1 = (Var_Type_t*) key1;
  Var_Type_t* type2 = (Var_Type_t*) key2;
  Var_DataType dataType1, dataType2;
  int flag;
    
  assert(type1);
  assert(type2);

  
  if (!(type1->name) && (type2->name)) {
    return 1;
  }

  if ((type1->name) && !(type2->name)) {
    return -1;
  }

  if ((type1->name) && (type2->name)){
    return strcmp(type1->name, type2->name);
  }

  dataType1 = type1->dataType;
  dataType2 = type2->dataType;
  
  if ((dataType2 > dataType1)) {
    return 1;
  } else if (dataType2 < dataType1) {
    return -1;
  }
  

  switch (dataType1){
      case Var_Array_c:
        if ((type1->typeData.arrayData.entryType ==
             type2->typeData.arrayData.entryType) &&
            (type1->typeData.arrayData.indexType ==
             type2->typeData.arrayData.indexType))
          return 0;
        else return -1;
        break;

      case Var_Bitvector_c:
        if (type2->typeData.size == type1->typeData.size) return 0;
        else return -1;
        break;

      case Var_BitvectorN_c:
        if (type2->typeData.size == type1->typeData.size) return 0;
        else return -1;
        break;

      case Var_Enumerated_c:
      {
        array_t *valueArray1 = type1->typeData.enumData.indexToValue;
        array_t *valueArray2 = type2->typeData.enumData.indexToValue;
        int numValue1 = array_n(valueArray1);
        int numValue2 = array_n(valueArray2);
        int i;
        
        if (numValue2 > numValue1) return 1;
        if (numValue2 < numValue1) return -1;

        for (i=0; i<numValue1; i++) {
          int flag;
          Var_EnumElement_t* value1 = array_fetch(Var_EnumElement_t *,
                                                  valueArray1, i);
          Var_EnumElement_t* value2 = array_fetch(Var_EnumElement_t *,
                                                  valueArray2, i);
          
          flag = st_ptrcmp((char *) value1, (char *) value2);
          if (flag !=0 ) return flag;
        }
        return 0;
        break;
      }
 
      case Var_Range_c:
        if (type2->typeData.range == type1->typeData.range) return 0;
        else return -1;
        break;

      case Var_Event_c:
      case Var_Integer_c:
      case Var_Natural_c:
      case Var_Boolean_c:
      case Var_NumConst_c:
      default:
        fprintf(stderr, "Unexpected var datatype in VarTypeCmp\n");
        exit(1);
        break;
        
  }
  
}

/**Function********************************************************************

  Synopsis    [The hash function for the formula unique table.]

  Description [The function takes as parameter a ATL formula. If the formula
  type is Atlp_ID_c, st_strhash is used with the left child as the key string.
  If type is GT, LT, EQ, GE, LE, then the formula is flattened (by calling
  Atlp_FormulaConvertToSting) and the flattened string is the key string.
  Otherwise, something very similar to st_ptrhash is done.]

  SideEffects []

  SeeAlso     [VarTypeCmp]

******************************************************************************/
static int
VarTypeHash(
  char *key,
  int modulus)
{
  Var_Type_t *type = (Var_Type_t *) key;
  
  if (type->name) {
    return st_strhash(type->name, modulus);
  } else {
    return VarObtainVarTypeHashValue(type, modulus);
  }
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
VarObtainVarTypeHashValue(
  Var_Type_t * type,
  int modulus)
{
  int hashValue;
  char *s;

  switch (type -> dataType){
      case Var_Array_c:
        return (int) ((((unsigned long) type->typeData.arrayData.indexType >>2) +
                       ((unsigned long) type->typeData.arrayData.entryType >>2)) % modulus);

        break;
        
      case Var_Bitvector_c: 
      {
        s = ALLOC(char, INDEX_STRING_LENGTH);

        sprintf(s, "bitvector %d", type -> typeData.size);

        hashValue = st_strhash(s, modulus) ;
        
        FREE(s);
        return hashValue;
        break;
      }

      case Var_BitvectorN_c: 
      {
        s = ALLOC(char, INDEX_STRING_LENGTH);

        sprintf(s, "bitvector_n %d", type -> typeData.size);

        hashValue = st_strhash(s, modulus) ;
        
        FREE(s);
        return hashValue;
        break;
      }

      case Var_Enumerated_c:
      {
        array_t *valueArray = type->typeData.enumData.indexToValue;
        int i = 0;
        
        Var_EnumElement_t* value = array_fetch(Var_EnumElement_t *,
                                               valueArray, i);
        return st_strhash(value->name, modulus);
        break;
      }
      
      case Var_Range_c:
      {
        s = ALLOC(char, INDEX_STRING_LENGTH);

        sprintf(s, "range %d", type -> typeData.size);
        
        hashValue = st_strhash(s, modulus);
        
        FREE(s);
        return hashValue;
        break;
      }
      
      case Var_Event_c:
      case Var_Integer_c:
      case Var_Natural_c:
      case Var_Boolean_c:
      case Var_NumConst_c:
      default:
        fprintf(stderr, "Unexpected var datatype in VarTypeHash\n");
        exit(1);
        break;
  }
}





