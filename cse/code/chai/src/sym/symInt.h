/**CHeaderFile*****************************************************************

  FileName    [symInt.h]

  PackageName [sym]

  Synopsis    []

  Description []

  SeeAlso     []

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

  Revision    [$Id: symInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _SYMINT
#define _SYMINT

#include "sym.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

struct SymTransStruct{
  Mdl_Module_t *module;             /* pointer to the module */
  mdd_manager *manager;          /* mdd manager - required for every mdd
                                     operation */
  array_t      *conjuncts;          /* array of conjuncts, one for each atom */
  st_table     *atmToConjunct;
};
                                      
/**Struct**********************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct SymInfoStruct {
  mdd_manager *manager;
  Sym_Trans_t * transRelation;
  Img_ImageInfo_t *imgInfo;
  st_table *varToPrimedId;
  array_t  *primedIdArray;
  array_t  *quantifyIdArray;
  st_table *varToUnprimedId;
  array_t  *unprimedIdArray;
  st_table  *idToVar;
  Tcl_Interp *interp;
  st_table *rangeToDummyId1;
  st_table *rangeToDummyId2; 
};


/**Struct**********************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct SymRegionStruct {
  Mdl_Module_t *module;            /* a region of this module */
  mdd_t        *mdd;               /* MDD associated with region */
};


/**Struct**********************************************************************

  Synopsis    [SymRegionManagerStruct]
  
  Description [A region manager manintains:
               1. mapping from modules to their symInfos and 
	       2. region tables mapping region names to regions
	       3. region counter used to generate unique region names]

  SeeAlso     [optional]

******************************************************************************/
struct SymRegionManagerStruct{
  st_table *moduleToSymInfo;
  Tcl_HashTable *regionTable;
  unsigned long regionCounter;
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

EXTERN mdd_t * SymModuleSearch(Mdl_Module_t *module, Sym_Info_t *symInfo, boolean verbose, boolean checkInvariants, array_t *invNameArray, array_t *typedExprArray, Tcl_Interp *interp, int maxNumSteps);
EXTERN void MddGroupVariables(mdd_manager *mddMgr, int initMddId, int blockSize);
EXTERN mdd_t * SymAssignmentBuildMddStandard(Sym_Info_t *symInfo, Var_Variable_t *var, Atm_Expr_t *expr);
EXTERN mdd_t * SymAssignmentBuildMddBoolean(Sym_Info_t *symInfo, Var_Variable_t *var, Atm_Expr_t *expr);
EXTERN mdd_t * SymBitVectorBuildAssignmentMdd(Sym_Info_t *symInfo, Var_Variable_t *var, Atm_Expr_t *expr);
EXTERN mdd_t * SymBitVectorBuildBooleanExpressionMdd(Sym_Info_t *symInfo, Atm_Expr_t *expr);
EXTERN array_t * SymBitVectorBuildMddArray(Sym_Info_t *symInfo, Atm_Expr_t *expr);
EXTERN array_t * SymBitVectorBuildMddArrayForVar(Sym_Info_t *symInfo, Atm_ExprType varType, Var_Variable_t *var);
EXTERN mdd_t * SymExprBuildExprConstMdd(Sym_Info_t *symInfo, Atm_Expr_t *expr, int c, int nVals);
EXTERN void SymWriteOrder(FILE *fp, Sym_Info_t *symInfo);
EXTERN array_t * SymReadOrder(FILE *fp, Mdl_Module_t *module);
EXTERN Mvf_Function_t * SymExprBuildMvf(Sym_Info_t *symInfo, Atm_Expr_t *expr);
EXTERN mdd_t * SymMVBuildBooleanExpressionMdd(Sym_Info_t *symInfo, Atm_Expr_t *expr);
EXTERN mdd_t * SymMVBuildAssignmentMdd(Sym_Info_t *symInfo, Var_Variable_t *var, Atm_Expr_t *expr);
EXTERN array_t * SymMddArrayObtainConstant(Sym_Info_t *symInfo, int n, boolean c);
EXTERN array_t * SymMddArrayOrMdd(Sym_Info_t *symInfo, array_t *array1, mdd_t *m);
EXTERN array_t * SymMddArrayAndMdd(Sym_Info_t *symInfo, array_t *array1, mdd_t *m);
EXTERN array_t * SymMddArrayImpliesMddArray(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN array_t * SymMddArrayOrMddArray(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN array_t * SymMddArrayAndMddArray(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN array_t * SymMddArrayXnorMddArray(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN array_t * SymMddArrayXorMddArray(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN array_t * SymMddArrayPlusMddArray(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN array_t * SymMddArrayMinusMddArray(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN array_t * SymMddArrayNot(Sym_Info_t *symInfo, array_t *mddArray);
EXTERN mdd_t * SymMddArrayAndComponents(Sym_Info_t *symInfo, array_t *mddArray);
EXTERN mdd_t * SymMddArrayCompareEqual(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN mdd_t * SymMddArrayRedAnd(Sym_Info_t *symInfo, array_t *array1);
EXTERN mdd_t *SymMddArrayRedOr(Sym_Info_t *symInfo,  array_t *array1);
EXTERN mdd_t * SymMddArrayCompareGreater(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN mdd_t * SymMddArrayCompareLesser(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN mdd_t * SymMddArrayCompareGreaterEqual(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN mdd_t * SymMddArrayCompareLesserEqual(Sym_Info_t *symInfo, array_t *array1, array_t *array2);
EXTERN void SymMddArrayFree(array_t *mddArray);
EXTERN void SymMddArrayPrint(Sym_Info_t *symInfo, array_t *array1);
EXTERN int SymRangeReadDummyId(Sym_Info_t *symInfo, int range, int num);
EXTERN boolean SymIsUnprimedId(Sym_Info_t *symInfo, int id);
EXTERN int SymCreateMddVariable(mdd_manager *manager, int range);
EXTERN int SymGetNvals(Sym_Info_t *symInfo, int id);
EXTERN boolean SymMddInTrueSupport(mdd_t *m, int id, Sym_Info_t *symInfo);
EXTERN int SymGetTopMddId(mdd_t *m, Sym_Info_t *symInfo);
EXTERN mdd_t * SymGetMddLiteral(mdd_manager *manager, int id, int value);
EXTERN int SymGetConstant(Atm_Expr_t *expr);
EXTERN boolean SymIsNumEnumRangeConstant(Atm_Expr_t *expr);
EXTERN int SymGetVariableId(Atm_Expr_t *expr, Sym_Info_t *symInfo);
EXTERN void SymGetVariableIdConstantAndPlus(Atm_Expr_t *expr, int *varId, int *c, boolean *isPlus, Sym_Info_t *symInfo);
EXTERN boolean SymIsMultiValuedVariable(Atm_Expr_t *expr);
EXTERN Var_Variable_t * SymGetArrayElementVar(Sym_Info_t *symInfo, Var_Variable_t *var, int value);
EXTERN int SymGetArrayId(Sym_Info_t *symInfo, Var_Variable_t *var, Atm_ExprType pOrUnp, int value);
EXTERN int SymGetArrayNvals(Var_Variable_t *var);
EXTERN Mdl_Module_t * SymInfoReadModule(Sym_Info_t *symInfo);
EXTERN mdd_t * SymGetRelExprMdd(Sym_Info_t *symInfo, Atm_ExprType exprType, int lvarid, int rvarid);
EXTERN mdd_t * SymMddArrayRedXor(Sym_Info_t *symInfo,  array_t *array1);

/**AutomaticEnd***************************************************************/

#endif /* _ */
