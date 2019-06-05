/**CHeaderFile*****************************************************************

  FileName    [mc.h]

  PackageName [mc]

  Synopsis    [Fair CTL model checker and debugger.]

  Description [Fair CTL model checker and debugger.  Works on a flattened
  network.]

  Author      [Adnan Aziz, Tom Shiple]

  Comment     [Documentation under construction]
  
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

  Revision    [$Id: mc.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _MC
#define _MC

/*---------------------------------------------------------------------------*/
/* Nested includes                                                           */
/*---------------------------------------------------------------------------*/
#include  "atlp.h"
#include  "sim.h"

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/**Enum************************************************************************

  Synopsis    [Use forward or backward analysis]

******************************************************************************/

typedef enum {
  McFwd_c,
  McBwd_c /* someday there may be a mixed method */
} Mc_FwdBwdAnalysis;

/**Enum************************************************************************

  Synopsis    [Extent to which Mc functions provide feedback/stats.]

******************************************************************************/

typedef enum {
  McVerbosityNone_c,
  McVerbositySome_c,
  McVerbosityMax_c
} Mc_VerbosityLevel;

/**Enum************************************************************************

  Synopsis    [Extent to which don't cares are used in Mc functions.]

******************************************************************************/

typedef enum {
  McDcLevelNone_c,
  McDcLevelRch_c,
  McDcLevelMax_c
} Mc_DcLevel;


/**Enum************************************************************************

  Synopsis [When creating a path from aState to bState, make it non trivial
  (ie a cycle) when aState == bState and McPathLengthType is McGreaterZero_c.]

******************************************************************************/

typedef enum {
  McGreaterZero_c,
  McGreaterOrEqualZero_c
} Mc_PathLengthType;


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN int Mc_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN boolean Mc_FormulaSemanticCheck(Atlp_Formula_t * formula, Mdl_Module_t * module, Var_TypeManager_t * typeManager);
EXTERN mdd_t * Mc_FormulaEvaluate(Sym_Info_t *symInfo, Atlp_Formula_t *formula, Mc_VerbosityLevel verbosity);

/**AutomaticEnd***************************************************************/

#endif /* _MC */































































