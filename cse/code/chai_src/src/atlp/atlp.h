/**CHeaderFile*****************************************************************

  FileName    [atlp.h]

  PackageName [atlp]

  Synopsis    [Routines for parsing, writing and accessing ATL formulas.]

  Description [This package implements a parser for ATL (Alternating-time
  Temporal Logic) formulas.  ATL is a language used to describe properties 
  of systems. For the syntax of ATL formulas, refer to 
  <A HREF="../atlp/atlp/atlp.html"> the ATL syntax manual</A>.]

  SeeAlso     [mc]

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

  Revision    [$Id: atlp.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _ATLP
#define _ATLP

#include  "main.h"
#include  "sym.h"

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/**Enum************************************************************************

  Synopsis    [Types of operators allowed in ATL formulas.]

  Description [The types of nodes in a ATL formula parse tree. EQ, TRUE, and
  FALSE are leaves, and all others are internal nodes.]

  SeeAlso     [Atlp_FormulaReadType]

******************************************************************************/
typedef enum {
  /* Agents have a strategy to (there Exists a Strategy) */
  Atlp_ESX_c,     /* << agents >> O  */
  Atlp_ESG_c,     /* << agents >> [] */
  Atlp_ESF_c,     /* << agents >> <> */
  Atlp_ESU_c,     /* << agents >> U  */ 
  Atlp_ESW_c,     /* << agents >> W  */ 

  /* Agents cannot avoid (agents canNot Avoid */
  Atlp_NAX_c,     /* [[ agents ]] O  */
  Atlp_NAG_c,     /* [[ agents ]] [] */
  Atlp_NAF_c,     /* [[ agents ]] <> */
  Atlp_NAU_c,     /* [[ agents ]] U  */
  Atlp_NAW_c,     /* [[ agents ]] W  */

  /* boolean connectives */
  Atlp_OR_c,      /* Boolean disjunction */
  Atlp_AND_c,     /* Boolean conjunction */
  Atlp_NOT_c,     /* Boolean negation*/
  Atlp_IMPLIES_c, /* Boolean implies (not symmetric) */
  Atlp_XOR_c,     /* Boolean not equal */
  Atlp_EQUIV_c,   /* Boolean equal */

  /* leaves */
  Atlp_TRUE_c,   /* tautology (used only for translation to exist. form) */
  Atlp_FALSE_c,  /* empty (used only for translation to exist. form) */
  Atlp_ID_c,     /* variables, enumerative constants */
  Atlp_INDEX_c,  /* index type */
  
  /* operators that return boolean while the arguments are not boolean */
  Atlp_GE_c,     /* >= */
  Atlp_LE_c,     /* <= */
  Atlp_GT_c,     /* >  */
  Atlp_LT_c,     /* <  */
  Atlp_EQ_c,     /* =  */
  
  /* operators that do not return boolean */
  Atlp_PLUS_c,   /* +  */
  Atlp_MINUS_c,  /* -  */
  Atlp_UMINUS_c, /* -x */
  Atlp_NATNUM_c  /* natural number */

} Atlp_FormulaType;

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct AtlpFormulaStruct Atlp_Formula_t;
typedef void   (*Atlp_DbgInfoFreeFn) (Atlp_Formula_t *);
typedef struct AtlpManagerStruct Atlp_Manager_t;
typedef struct AtlpAgentStruct Atlp_Agent_t;

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN int Atlp_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN Atlp_Manager_t * Atlp_ReadAtlpManager();
EXTERN Atlp_Agent_t * Atlp_AgentAllocWithName(char * name);
EXTERN Atlp_Agent_t * Atlp_AgentAlloc(char *name, boolean isAgent, Atm_Atom_t * atom);
EXTERN void Atlp_AgentFree(Atlp_Agent_t * agent);
EXTERN char * Atlp_AgentReadName(Atlp_Agent_t * agent);
EXTERN boolean Atlp_AgentIsAgent(Atlp_Agent_t * agent);
EXTERN Atm_Atom_t * Atlp_AgentReadAtom(Atlp_Agent_t * agent);
EXTERN Atlp_Formula_t * Atlp_FormulaReadLeft(Atlp_Formula_t * formula);
EXTERN Atlp_Formula_t * Atlp_FormulaReadRight(Atlp_Formula_t * formula);
EXTERN Atlp_Manager_t * Atlp_ManagerAlloc();
EXTERN void Atlp_ManagerFree(Atlp_Manager_t * manager);
EXTERN lsList Atlp_FileParseFormula(Atlp_Manager_t *manager, FILE * fp);
EXTERN array_t * Atlp_ManagerReadFormulas(Atlp_Manager_t *manager);
EXTERN array_t * Atlp_ManagerReadFormulaNames(Atlp_Manager_t *manager);
EXTERN Atlp_Formula_t * Atlp_FormulaReadByName(char * name, Atlp_Manager_t * manager);
EXTERN char * Atlp_FormulaConvertToString(Atlp_Formula_t * formula);
EXTERN void Atlp_FormulaPrint(FILE * fp, Atlp_Formula_t * formula);
EXTERN Atlp_FormulaType Atlp_FormulaReadType(Atlp_Formula_t * formula);
EXTERN char * Atlp_FormulaReadVariableName(Atlp_Formula_t * formula);
EXTERN Atlp_Formula_t * Atlp_FormulaReadLeftChild(Atlp_Formula_t * formula);
EXTERN Atlp_Formula_t * Atlp_FormulaReadRightChild(Atlp_Formula_t * formula);
EXTERN Atm_Expr_t * Atlp_FormulaReadAtmExpr(Atlp_Formula_t * formula);
EXTERN mdd_t * Atlp_FormulaObtainStates(Atlp_Formula_t * formula);
EXTERN lsList Atlp_FormulaReadAgentList(Atlp_Formula_t * formula);
EXTERN void Atlp_FormulaSetStates(Atlp_Formula_t * formula, mdd_t * states);
EXTERN void Atlp_FormulaSetAtmExpr(Atlp_Formula_t * formula, Atm_Expr_t * atmExpr);
EXTERN void Atlp_FormulaSetAgentList(Atlp_Formula_t * formula, lsList agentList);
EXTERN void Atlp_FormulaSetDbgInfo(Atlp_Formula_t * formula, void *data, Atlp_DbgInfoFreeFn freeFn);
EXTERN void * Atlp_FormulaReadDebugData(Atlp_Formula_t * formula);
EXTERN boolean Atlp_FormulaTestIsConverted(Atlp_Formula_t * formula);
EXTERN boolean Atlp_FormulaTestIsQuantifierFree(Atlp_Formula_t *formula);
EXTERN Atlp_Formula_t * Atlp_FormulaReadOriginalFormula(Atlp_Formula_t * formula);
EXTERN void Atlp_FormulaFree(Atlp_Formula_t *formula);
EXTERN void Atlp_FlushStates(Atlp_Formula_t * formula);
EXTERN Atlp_Formula_t * Atlp_FormulaDup(Atlp_Formula_t * formula);
EXTERN void Atlp_FormulaArrayFree(array_t * formulaArray);
EXTERN Atlp_Formula_t * Atlp_FormulaConvertToExistentialForm(Atlp_Formula_t * formula);
EXTERN array_t * Atlp_FormulaArrayConvertToExistentialFormTree(array_t * formulaArray);
EXTERN array_t * Atlp_FormulaArrayConvertToDAG(array_t *formulaArray);
EXTERN array_t * Atlp_FormulaDAGConvertToExistentialFormDAG(array_t *formulaDAG);
EXTERN Atlp_Formula_t * Atlp_FormulaCreateWithAgents(Atlp_FormulaType type, void * left, void * right, lsList agentList);
EXTERN Atlp_Formula_t * Atlp_FormulaCreate(Atlp_FormulaType type, void * left, void * right);
EXTERN void Atlp_AgentIsAgentSet(Atlp_Agent_t * agent, boolean value);

/**AutomaticEnd***************************************************************/

#endif /* _ATLP */



