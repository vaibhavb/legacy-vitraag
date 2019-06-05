/**CHeaderFile*****************************************************************

  FileName    [prsInt.h]

  PackageName [prs]

  Synopsis    [parser for reactive modules]

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

  Revision    [$Id: prsInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _PRSINT
#define _PRSINT

#include  "prs.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define MAXINTLEN 5
#define _MAXUNNAMED 20

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct PrsNodeStruct PrsNode_t;


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
/**Struct**********************************************************************

  Synopsis    [Definition of nodes for building the parse-tree.]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct PrsNodeStruct {
  /* use of id for var_declarator: 0 = field1 has name, 1 = field1 has name
     and field2 has number, 2 = field1 and field2 have names */
  int id; 
  char * field1;
  char * field2;
};


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
static int     _varclass;
extern int PrsUnnamedTypeCount;
extern int yylineno;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN PrsNode_t * PrsNodeAlloc(int id, char* field1, char* field2);
EXTERN void PrsNodeFree(PrsNode_t * node);
EXTERN int PrsNodeReadId(PrsNode_t* node);
EXTERN char * PrsNodeReadField1(PrsNode_t* node);
EXTERN char * PrsNodeReadField2(PrsNode_t* node);
EXTERN int PrsVariableCheckPrimed(char *name);
EXTERN boolean PrsIsInList(lsList list, lsGeneric item);
EXTERN Var_Type_t * PrsTypeEnumAlloc(char* name, lsList enumConstList, Var_TypeManager_t* typeManager);
EXTERN Var_Type_t* PrsTypeRangeAlloc(char* name, int range, Var_TypeManager_t* typeManager);
EXTERN Var_Type_t* PrsTypeArrayAlloc(char* name, Var_Type_t *indexType, Var_Type_t *entryType, Var_TypeManager_t* typeManager);
EXTERN Var_Type_t* PrsTypeBitvectorAlloc(char* name, int size,
                                         Var_TypeManager_t* typeManager);
EXTERN Var_Type_t* PrsTypeBitvectorNAlloc(char* name, int size, Var_TypeManager_t* typeManager);


/**AutomaticEnd***************************************************************/

/* these functions are in prs.y, so not automatically generated */
EXTERN int yyerror(char* s);
EXTERN int PrsParse();
EXTERN void PrsGlobalPointersInitialize(Main_Manager_t *manager);

#endif /* _PRSINT */


