/**CHeaderFile*****************************************************************

  FileName    [refInt.h]

  PackageName [ref]

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

  Author      [Sriram Rajamani]

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

  Revision    [$Id: refInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _REFINT
#define _REFINT

#include  "ref.h"

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
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN boolean RefCheckTransitionInvariant(Mdl_Module_t *module, Sym_Info_t *symInfo, mdd_t *transInv, mdd_t *initSet, array_t *eventIdArray, array_t *histFreeIdArray, array_t *histDependIdArray, array_t *transQuanIdArr, boolean verbose, boolean checkAtEnd);
EXTERN int RefCheckNohiddenRefinement(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
EXTERN int RefCheckCompositionalRefinementOneAtom(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
EXTERN void RefDumpIds(array_t *a);
EXTERN mdd_t * RefGetFlatSpecTransRelation(Mdl_Module_t *specModule, Sym_Info_t *specSymInfo, Mdl_Module_t * implModule, Sym_Info_t *implSymInfo, array_t *specIdArray, array_t *implIdArray);
EXTERN array_t * RefUnprimedIdsMerge(Sym_Info_t *symInfo1, Sym_Info_t *symInfo2);
EXTERN array_t * RefPrimedIdsMerge(Sym_Info_t *symInfo1, Sym_Info_t *symInfo2);
EXTERN mdd_t * RefMddBuildNoChange(Sym_Info_t *symInfo);
EXTERN mdd_t * RefMddBuildObseq(Mdl_Module_t *impl, Sym_Info_t *implSymInfo, Mdl_Module_t *spec, Sym_Info_t *specSymInfo);
EXTERN array_t * RefConjunctsAddExtra(Sym_Info_t *symInfo, mdd_t *extra);
EXTERN int RefCheckSimulation(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
EXTERN void RefMddArrayFree(array_t *mddArray);

/**AutomaticEnd***************************************************************/

#endif /* _ */
