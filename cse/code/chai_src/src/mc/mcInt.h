/**CHeaderFile*****************************************************************

  FileName    [mcInt.h]

  PackageName [mc]

  Synopsis    [Internal declarations.]

  Author      [Adnan Aziz and Tom Shiple]

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

  Revision    [$Id: mcInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _MCINT
#define _MCINT

/*---------------------------------------------------------------------------*/
/* Nested includes                                                           */
/*---------------------------------------------------------------------------*/
#include  "mc.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

#define McMaxStringLength_c 1000


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/**Enum************************************************************************

  Synopsis    [Level to which debugging of CTL formulae is performed.]

******************************************************************************/

typedef enum {
  McDbgLevelNone_c,
  McDbgLevelMin_c,
  McDbgLevelMax_c,
  McDbgLevelInteractive_c
} McDbgLevel;


/**Struct**********************************************************************

  Synopsis    [A struct for representing a paths.]

  Description [Structure consists of two arrays of mdd's - the stem,
  and the cycle (possibley empty if we are dealing with finite paths)]

******************************************************************************/

struct McPathStruct {
  array_t *stemArray; /* represents a sequence of mdd's each corresponding to a state,
                       * for the finite segment of the path
                       */

  array_t *cycleArray; /* represents a sequence of mdd's each corresponding to a state,
                        * for the cycle of the path
                        */
};

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef struct McPathStruct McPath_t;


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN array_t * McAgentsCheckValid(lsList agentList, Mdl_Module_t* module);
EXTERN mdd_t * McEvaluateEXFormula(Sym_Info_t *symInfo, lsList agentList, mdd_t * target, Mc_VerbosityLevel verbosity);
EXTERN mdd_t * McEvaluateEGFormula(Sym_Info_t * symInfo, lsList agentList, mdd_t *invariantMdd, array_t *onionRingsArrayForDbg, Mc_VerbosityLevel verbosity);
EXTERN mdd_t * McEvaluateEUFormula(Sym_Info_t * symInfo, lsList agentList, mdd_t *invariantMdd, mdd_t *targetMdd, array_t *onionRings, Mc_VerbosityLevel verbosity);
EXTERN mdd_t * McEvaluateEWFormula(Sym_Info_t * symInfo, lsList agentList, mdd_t *invariantMdd, mdd_t *targetMdd, array_t *onionRings, Mc_VerbosityLevel verbosity);

/**AutomaticEnd***************************************************************/

#endif /* _MCINT */



































































