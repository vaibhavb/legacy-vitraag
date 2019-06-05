/**CHeaderFile*****************************************************************

  FileName    [var.h]

  PackageName [var]

  Synopsis    [Data structures and routines to manipulate variables and
               variable types]

  Description [optional]

  SeeAlso     [optional]

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

  Revision    [$Id: var.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _VAR
#define _VAR

#include "main.h"
#include "array.h"
#include "st.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct VarVariableStruct Var_Variable_t;
typedef struct VarTypeManagerStruct Var_TypeManager_t;
typedef struct VarEnumElementStruct Var_EnumElement_t;
typedef struct VarTypeStruct Var_Type_t;
typedef char* VarModuleGeneric;
typedef char* VarAtomGeneric;

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
/**Enum************************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
typedef enum {
  Var_Array_c,
  Var_Bitvector_c,
  Var_BitvectorN_c,
  Var_Boolean_c,
  Var_Enumerated_c,
  Var_Event_c,
  Var_Integer_c,
  Var_Natural_c,
  Var_Range_c,
  Var_NumConst_c,
  Var_Timer_c,
  Var_TimeIncrement_c /* used for type checking */
} Var_DataType;


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
Var_Type_t* BoolType;
Var_Type_t* EventType;
Var_Type_t* IntType;
Var_Type_t* NatType;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
/**Macro***********************************************************************

  Synopsis     [Iterates over the symbolic values of an enumerated type
                variable.]

  Description  [The macro iterates over the symbolic values of an enumerated
                type variable. Note that the macro should be used only for an
                enumerated type variable.]

  SideEffects  [None]

  SeeAlso      [Var_VarTypeForEachSymbolicValue]

******************************************************************************/
#define Var_VariableForEachSymbolicValue(                            \
  var,  	/* pointer to a variable (Var_Variable_t *) */       \
  i,		/* integer for iterating over the entries */         \
  value		/* variable for data (Var_EnumElement_t *) */        \
)                                                                    \
  arrayForEachItem(Var_EnumElement_t *,                              \
                   Var_VariableReadIndexToValue(var), i, value) 

  
/**Macro***********************************************************************

  Synopsis     [Iterates over the symbolic values of an enumerated data type.]

  Description  [The macro iterates over the symbolic values of an enumerated
                data type. Note that the macro should be used only for an
                enumerated type.]

  SideEffects  [None]

  SeeAlso      [Var_VariableForEachSymbolicValue]

******************************************************************************/
#define Var_VarTypeForEachSymbolicValue(                       \
  varType,  	/* pointer to a variable type (Var_Type_t *) */     \
  i,		/* integer for iterating over the entries */        \
  value		/* variable for data (Var_EnumElement_t *) */       \
)                                                                   \
  arrayForEachItem(Var_EnumElement_t *,                             \
                   Var_VarTypeReadIndexToValue(varType), i, value)

/**Macro***********************************************************************

  Synopsis     [required]

  Description  [optional]

  SideEffects  [required]

  SeeAlso      [optional]

******************************************************************************/
#define Var_TypeManagerForEachVarType(                                      \
typeManager,      /* pointer to type manager */                             \
gen,              /* local variable of type st_generator* */                \
type              /* returned data of type Var_Type_t* */                   \
)                                                                           \
  st_foreach_item(Var_TypeManagerReadVarTypeTable(typeManager), (gen),      \
                  (char **) &(type), (char **) &(type))                                 



/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN st_table * Var_TypeManagerReadVarTypeTable(Var_TypeManager_t *typeManager);
EXTERN boolean Var_TypeIsCompatibleWithNumConst(Var_Type_t *type);
EXTERN Var_Type_t * Var_VarTypeBasicAlloc(char *name, Var_DataType datatype);
EXTERN Var_Type_t * Var_VarNewTimerTypeAlloc();
EXTERN Var_Type_t * Var_VarTimeIncrementTypeAlloc(long maxIncrement);
EXTERN Var_Type_t * Var_VarTypeArrayAlloc(char *name, Var_Type_t *indexType, Var_Type_t *entryType);
EXTERN Var_Type_t * Var_VarTypeBitvectorAlloc(char *name, int size);
EXTERN Var_Type_t * Var_VarTypeBitvectorNAlloc(char *name, int size);
EXTERN Var_Type_t * Var_VarTypeEnumeratedAlloc(char *name, array_t *indexToValue, st_table *valueToIndex);
EXTERN Var_Type_t * Var_VarTypeRangeAlloc(char *name, int range);
EXTERN Var_Type_t * Var_VarTypeDup(Var_Type_t *type, char *name);
EXTERN void Var_VarTypeFree(Var_Type_t *type);
EXTERN Var_Variable_t * Var_VariableAlloc(VarModuleGeneric module, char *name, Var_Type_t *type);
EXTERN int Var_VariableFree(Var_Variable_t *var);
EXTERN char * Var_VariableReadName(Var_Variable_t *var);
EXTERN Var_Type_t * Var_VarTypeReadIndexType(Var_Type_t *varType);
EXTERN Var_Type_t * Var_VarTypeReadEntryType(Var_Type_t *varType);
EXTERN Var_EnumElement_t * Var_VarTypeReadSymbolicValueFromIndex(Var_Type_t *type, int i);
EXTERN int Var_VarTypeReadIndexFromSymbolicValue(Var_Type_t *type, Var_EnumElement_t *symValue);
EXTERN VarModuleGeneric Var_VariableReadModule(Var_Variable_t *var);
EXTERN Var_Type_t* Var_VariableReadType(Var_Variable_t *var);
EXTERN char* Var_VariableReadTypeName(Var_Variable_t *var);
EXTERN int Var_VariableReadPEID(Var_Variable_t* var);
EXTERN void Var_VariableSetPEID(Var_Variable_t* var, int peid);
EXTERN void Var_VariableSetControlAtom(Var_Variable_t* var, VarAtomGeneric atm);
EXTERN int Var_VariableTypeReadNumVals(Var_Type_t* type);
EXTERN int Var_VariableTypeSetNumVals(Var_Type_t* type, int numValues);
EXTERN lsList Var_VariableReadReadList(Var_Variable_t* var);
EXTERN lsList Var_VariableReadAwaitList(Var_Variable_t* var);
EXTERN void Var_VariableAddToReadByAtomList(Var_Variable_t* var, VarAtomGeneric atom);
EXTERN VarAtomGeneric Var_VariableReadControlAtom(Var_Variable_t* var);
EXTERN void Var_VariableAddToAwaitByAtomList(Var_Variable_t* var, VarAtomGeneric atom);
EXTERN void Var_VariableChangeName(Var_Variable_t *var, char *name);
EXTERN boolean Var_VariableIsBoolean(Var_Variable_t *var);
EXTERN Var_TypeManager_t* Var_TypeManagerInit();
EXTERN void Var_TypeManagerFree(Var_TypeManager_t *typemanager);
EXTERN Var_Type_t * Var_TypeManagerAddType(Var_TypeManager_t* typeManager, Var_Type_t* type);
EXTERN char* Var_VarTypeReadName(Var_Type_t* type);
EXTERN int Var_VarTypeReadDomainSize(Var_Type_t* type);
EXTERN int Var_VarTypeArrayOrBitvectorReadSize(Var_Type_t* type);
EXTERN Var_EnumElement_t * Var_EnumElementAlloc(char *name, Var_Type_t *dataType);
EXTERN boolean Var_TypeManagerAddEnumElement(Var_TypeManager_t *typeManager, Var_EnumElement_t *element);
EXTERN void Var_EnumElementFree(Var_EnumElement_t *typeconst);
EXTERN Var_EnumElement_t* Var_TypeManagerReadEnumElementFromName(Var_TypeManager_t *typeManager, char *name);
EXTERN Var_Type_t* Var_EnumElementReadType(Var_EnumElement_t* element);
EXTERN char* Var_EnumElementReadName(Var_EnumElement_t* element);
EXTERN Var_Type_t* Var_TypeManagerReadTypeFromName(Var_TypeManager_t* typeManager, char* name);
EXTERN Var_Type_t* Var_TypeManagerObtainType(Var_TypeManager_t* typeManager, Var_Type_t* type);
EXTERN Var_DataType Var_VarTypeReadDataType(Var_Type_t* type);
EXTERN Var_DataType Var_VariableReadDataType(Var_Variable_t* var);
EXTERN Var_EnumElement_t * Var_VariableObtainEnumElementFromIndex(Var_Variable_t *var, int index);
EXTERN int Var_EnumElementObtainIndex(Var_EnumElement_t *element);
EXTERN boolean Var_VariableIsHistoryFree(Var_Variable_t* var);
EXTERN boolean Var_VariableIsHistoryDependent(Var_Variable_t *var);
EXTERN boolean Var_VariableIsEvent(Var_Variable_t* var);
EXTERN boolean Var_VariableIsDummy(Var_Variable_t* var);
EXTERN boolean Var_VariableIsPrivate(Var_Variable_t* var);
EXTERN boolean Var_VariableIsInterface(Var_Variable_t* var);
EXTERN boolean Var_VariableIsExternal(Var_Variable_t* var);
EXTERN array_t * Var_VariableReadIndexToValue(Var_Variable_t* var);
EXTERN array_t * Var_VarTypeReadIndexToValue(Var_Type_t* varType);
EXTERN boolean Var_TypesAreCompatible(Var_Type_t * type1, Var_Type_t * type2);
EXTERN boolean Var_TypeIsEnum(Var_Type_t * type);
EXTERN boolean Var_TypeIsEvent(Var_Type_t * type);
EXTERN boolean Var_TypeIsRange(Var_Type_t * type);
EXTERN boolean Var_TypeIsArray(Var_Type_t * type);
EXTERN boolean Var_TypeIsBV(Var_Type_t * type);
EXTERN boolean Var_TypeIsBitvector(Var_Type_t * type);
EXTERN boolean Var_TypeIsBitvectorN(Var_Type_t * type);
EXTERN boolean Var_TypeIsBoolean(Var_Type_t * type);
EXTERN boolean Var_TypeIsFinite(Var_Type_t * type);
EXTERN Var_Type_t * Var_RangeTypeReadFromRange(Var_TypeManager_t * typeManager, int num);
EXTERN char * Var_TypeConvertToString(Var_Type_t * type);
EXTERN int Var_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Var_Reinit(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Var_End(Tcl_Interp *interp, Main_Manager_t *manager);

/**AutomaticEnd***************************************************************/

#endif /* _VAR */



