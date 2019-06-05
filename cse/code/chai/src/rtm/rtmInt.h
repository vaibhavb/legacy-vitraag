/**CHeaderFile*****************************************************************

  FileName    [rtmInt.h]

  PackageName [rtm]

  Synopsis    []

  Description []

  SeeAlso     []

  Author      [Serdar Tasiran]

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

  Revision    [$Id: rtmInt.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _RTMINT
#define _RTMINT

#include "rtm.h"

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

  Synopsis    [Info needed for rtm package ]

  Description [Each RtmInfoStruct corresponds to a module ]

  SeeAlso     [optional]

******************************************************************************/
struct RtmInfoStruct {            

  Sym_Info_t   *symInfo;            /* Info related to mdd manipulations.
                                       See SymInt.h */
  int          timeIncId;           /* The mdd Id of the variable representing the
                                       time increment */
/*
  These are not used right now but may become handy for other traversal methods

  array_t      *timedconjuncts;        array of conjuncts, one for each atom
                                       Consists of transitions where one time unit
                                       elapses. The mdd variable with ID
                                       rtmInfo->timeIncId is assigned to 1 
  array_t      *untimedconjuncts;      array of conjuncts, one for each atom
                                       Consists of transitions where the
                                       mdd variable with ID rtmInfo->timeIncId
                                       is assigned to 0, i.e., control transitions
                                       If the "reads" variables haven't changed
                                       a self-loop must be included in each conjunct
*/
};
/**Struct**********************************************************************

  Synopsis    [required]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct RtmRegionStruct {
  Mdl_Module_t *module;            /* a region of this module */
  mdd_t        *mdd;               /* MDD associated with region */
};

/**Struct**********************************************************************

  Synopsis    [RtmRegionManagerStruct]

  Description [optional]

  SeeAlso     [optional]

******************************************************************************/
struct RtmRegionManagerStruct {
  st_table *moduleToRtmInfo;
  Tcl_HashTable *rtmRegionTable;
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

EXTERN array_t * RtmReadOrder(FILE *fp, Mdl_Module_t *module);
EXTERN void RtmCreateTimeIncVar(Rtm_Info_t *rtmInfo, Mdl_Module_t *module);
EXTERN int RtmGetNvals(Rtm_Info_t *rtmInfo, int id);
EXTERN int RtmGetTopMddId(mdd_t *m, Rtm_Info_t *rtmInfo);
EXTERN mdd_t * RtmModuleSearch(Mdl_Module_t *module, Rtm_Info_t *rtmInfo, boolean verbose, boolean checkInvariants, array_t *invNameArray, array_t *typedExprArray, Tcl_Interp *interp);
EXTERN boolean RtmMddInTrueSupport(mdd_t *m, int id, Rtm_Info_t *rtmInfo);
EXTERN mdd_t * RtmGetMddLiteral(mdd_manager *manager, int id, int value);

/**AutomaticEnd***************************************************************/

#endif /* _ */
