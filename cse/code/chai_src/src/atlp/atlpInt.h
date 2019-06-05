/**CHeaderFile*****************************************************************

  FileName    [atlpInt.h]

  PackageName [atlp]

  Synopsis    [Declarations for internal use.]

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

  Revision    [$Id: atlpInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _ATLPINT
#define _ATLPINT

#define MAX_LENGTH_OF_VAR_NAME 512

#include  "atlp.h"


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************

  Synopsis    [Tree representing a ATL formula.]

  Description [Data structure for a ATL formula.  A formula is represented by
  a rooted tree. An internal vertex has one or two children, depending on the
  operator type: if the operator takes just one argument, then the left child
  holds the subformula, and the right child is NULL; if the operator takes two
  arguments, then both children are non-NULL. 
  A leaf is either a pair of a variable name in the network and a value, the
  tautology, or the negative tautology.  The left and right fields are used
  in leaf vertices only in the first case above. (The left child holds
  a variable name while the right child holds a value. Note that these two
  fields are used differently in internal vertices and leaves.) 
  Nothing is shared between different ATL formulas (not
  even character strings). Hence, an array of ATL formulas is a multi-rooted
  tree. DbgInfo is an uninterpreted pointer used by the ATL debugger to store
  information needed to debug a formula.]

  SeeAlso     [AtlpFormulaType]
  
******************************************************************************/
struct AtlpFormulaStruct {
  Atlp_FormulaType type;
  Atlp_Formula_t *left;   /* left child in formula */
  Atlp_Formula_t *right;  /* right child, or null if not used */
  int refCount;           /* number of formulas referring to this
                             formula as a left or right child */
  mdd_t	*states;          /* set of states satifying formula */

  /* the list of agents associated with this formula.
     Used by the formula types
     ESX, ESF, ESG, ESU, NAX, NAF, NAG, NAU */
  lsList agentList;
  
  /* if the type is an atomic formula, the following holds the atomic
     expression (in the form atm_expr_t* ) that will be freed when the mdd for
     this atomic formula is built. This will be filled during semantic
     check of this atomic formula */
  
  Atm_Expr_t * atmExpr;

  struct {
    void *data; /* used  to store information used by debugger */
    Atlp_DbgInfoFreeFn freeFn; /* free function for data */
    boolean convertedFlag;  /* was converted to existential form
                                           by non-trivial transformation */
    Atlp_Formula_t *originalFormula; /* pointer to formula from which this
                                           was converted */
  } dbgInfo;
  
};


/**Struct**********************************************************************

  Synopsis    [Atlp Formula Manager]

  Description [This is essentially a hash table that stores the name
  and the pointer to a atlp_formula.]

  SeeAlso     [optional]

******************************************************************************/
struct AtlpManagerStruct {

  st_table *nameToFormula;

};


/**Struct**********************************************************************

  Synopsis    [A structure for the agent in a Atl formula]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct AtlpAgentStruct {

  char * name;                    /* name of the agent (atom) */
  boolean isAgent;                /* is this the agent the agent specified
                                     by the user? */
  Atm_Atom_t * atom;              /* pointer to the actual atom */
  
};



/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/



/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN char * AtlpObtainNewFormulaName();
EXTERN char * AtlpFormulaAgentsConvertToString(lsList agentList);
EXTERN void AtlpFormulaSetStatesToNULL(Atlp_Formula_t *formula);
EXTERN void AtlpFormulaIncrementRefCount(Atlp_Formula_t *formula);
EXTERN void AtlpFormulaDecrementRefCount(Atlp_Formula_t *formula);
EXTERN void AtlpFormulaAddToManager(Atlp_Manager_t *manager, char * name, Atlp_Formula_t * formula);
EXTERN void AtlpFormulaFree(Atlp_Formula_t * formula);
EXTERN boolean AtlpFormulaIsWellFormed(Atlp_Formula_t * formula);

/**AutomaticEnd***************************************************************/

EXTERN lsList AtlpParse();

#endif /* _ATLPINT */
