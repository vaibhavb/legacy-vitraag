/**CHeaderFile*****************************************************************

  FileName    [mdlInt.h]

  PackageName [mdl]

  Synopsis    [Structures defined in this module]

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

  Revision    [$Id: mdlInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _MDLINT
#define _MDLINT

#include  "mdl.h"
#include  "error.h"

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

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct MdlManagerStruct {
  st_table *nameToModule;
};

/**Struct**********************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct MdlModuleStruct {
  char *name;			/* name of the module */
  Mdl_Expr_t *moduleExpr;	/* module expression: how the module
                                   is built */
  st_table *atmTable;
  st_table *varTable;
  st_table *stateInvTable; 
  st_table *tranInvTable;
  int creationTime;		/* creation time of this module */
};

/**Struct**********************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct MdlInvariantStruct {
  char *name;
  Atm_Expr_t *inv_expr;
  Mdl_Module_t *module;  
  Mdl_InvStatus_t status;
};


/**Struct**********************************************************************

  Synopsis    [A structure for the module expression]

  Description [This structure is used in building the module expression tree.
  A module is either obtained by explicit enumeration of its atoms, or by some
  operation on other existing modules. These operations include module
  duplication, variable renaming, variable hiding and parallel composition. In
  module duplication, the name of the module copied from will be stored in
  left.  In variable renaming the set of variables that are renamed and their
  new names are specified, and they are stored in list1 and list2. In
  variable hiding, the set of variables to be hidden are specified in
  list1. In parallel composition the two component modules will be stored in
  left and right.]

  SeeAlso     [Mdl_ExprType Mdl_Expr_t]

******************************************************************************/
struct MdlExpressionStruct {
  Mdl_ExprType moduleExprType; /* the type of the module expression */
  Mdl_Expr_t * left;           /* the left child */
  Mdl_Expr_t * right;          /* the right child */
  lsList list1;                /* auxilliary list 1 */
  lsList list2;                /* auxilliary list 2 */
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

EXTERN char * MdlExprConvertToString(Mdl_Expr_t * mexpr);
EXTERN Mdl_Module_t * MdlExprEvaluate(Mdl_Expr_t *mexpr, Mdl_Manager_t *moduleManager);
EXTERN void MdlExprSetAtomPointers(Mdl_Expr_t *mexpr, st_table *atomMappingTable);
EXTERN Mdl_Expr_t * MdlExprAlloc(Mdl_ExprType moduleExprType);
EXTERN void MdlExprSetLeftChild(Mdl_Expr_t * mexpr, Mdl_Expr_t * child);
EXTERN void MdlExprSetRightChild(Mdl_Expr_t * mexpr, Mdl_Expr_t * child);
EXTERN void MdlExprSetList1(Mdl_Expr_t * mexpr, lsList list);
EXTERN void MdlExprSetList2(Mdl_Expr_t * mexpr, lsList list);
EXTERN st_table * MdlVariablesCopyToNewModule(Mdl_Module_t* newmodule, Mdl_Module_t* module2);
EXTERN st_table * MdlAtomsCopyToNewModule(Mdl_Module_t* newmodule, Mdl_Module_t* oldmodule, st_table* variableMappingTable);
EXTERN void MdlVariablesWrapUp(Mdl_Module_t* newmodule);
EXTERN void MdlAtomsWrapUp(Mdl_Module_t *module);
EXTERN void MdlModuleCounterReset();
EXTERN char * MdlExprListConvertToString(lsList list);
EXTERN Mdl_Module_t* MdlModuleCompose(Mdl_Module_t* module1, Mdl_Module_t* module2, st_table ** atomMappingTable, st_table ** varMappingTable);
EXTERN Mdl_Module_t * MdlModuleHideVariables(Mdl_Module_t* module, lsList varlist, st_table **atomMappingTable, st_table **varMappingTable);
EXTERN Mdl_Module_t * MdlModuleRenameVariables(Mdl_Module_t *module, lsList designatorList, lsList destinationList, st_table ** atomMappingTable, st_table ** varMappingTable);

/**AutomaticEnd***************************************************************/

#endif /* _MDLINT */



