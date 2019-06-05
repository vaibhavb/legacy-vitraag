/**CHeaderFile*****************************************************************

 
  FileName    [varInt.h]

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

  Revision    [$Id: varInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _VARINT
#define _VARINT

#include "var.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************

  Synopsis    [A multi-valued variable.]

  Description [A multi-valued variable.]

******************************************************************************/
struct VarVariableStruct {
  char  *name;	               /* name of variable */
  Var_Type_t *varType;         /* type identifier associated with variable */
  int peid;                    /* 0 = private, 1 = external, 2 = interface, */
                               /* 3 = dummy (used in forall assignment) */
  VarAtomGeneric ctrlByAtom;   /* pointer to the atom that controls this var*/
  lsList readByAtomList;       /* list of ptrs to the atoms that read this var*/
  lsList awaitByAtomList;      /* list of ptrs to the atoms that awaits this var*/
  VarModuleGeneric module;     /* module to which variable belongs  */
};


/**Struct**********************************************************************

  Synopsis    [A variable type.]

  Description [A variable type.]

******************************************************************************/
struct VarTypeStruct {
  char *name;
  int creationTime;
  Var_DataType dataType;  /* int/nat/bool/enum/range/event/array/bitvector/timer/timeincrement */

  union {
    int range;            /* range type, timer, time increment. (0..range-1) */
    
    struct {              /* enumerated type */
      array_t  *indexToValue;   /* int -> VarEnumElementStruct * */
      st_table *valueToIndex;   /* VarEnumElementStruct * -> int */
    } enumData;

    struct {              /* array type */
      Var_Type_t *indexType;  
      Var_Type_t *entryType;  
    } arrayData;
    
    int size;             /* bitvector type */
  } typeData;

};


/**Struct**********************************************************************

  Synopsis    [An element type.]

  Description [An element type, with a name and a pointer to the data type.]

  SeeAlso     [struct VarTypeManagerStruct]

******************************************************************************/
struct VarEnumElementStruct{
  char* name;
  Var_Type_t* dataType;
};


/**Struct**********************************************************************

  Synopsis    [A structure with a table of types and elements.]

  Description [This structure contains a hash table of types and elements.
               All the data types available in the reactive module will
	       be stored in the VarTypeTable, while all the enumerative
	       elements will be stored in the VarTypeElementsTable.]

  SeeAlso     [optional]

******************************************************************************/
struct VarTypeManagerStruct {
  st_table* VarTypeTable;
  st_table* VarEnumElementsTable;
};


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/


/**AutomaticEnd***************************************************************/

#endif /* _VARINT */







