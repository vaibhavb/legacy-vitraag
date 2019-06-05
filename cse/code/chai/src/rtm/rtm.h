/**CHeaderFile*****************************************************************

  FileName    [rtm.h]

  PackageName [rtm]

  Synopsis    [Timing verification]

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

  Revision    [$Id: rtm.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _RTM
#define _RTM

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#include "main.h"
#include "mdl.h"
#include "var.h"
#include "atm.h"
#include "img.h"
#include "sym.h"
#include "mdd.h"

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef struct RtmInfoStruct Rtm_Info_t;
typedef struct RtmRegionManagerStruct Rtm_RegionManager_t;
typedef struct RtmRegionStruct Rtm_Region_t;
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

EXTERN int Rtm_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Rtm_Reinit(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Rtm_End(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN void Rtm_ModulePerformBFS(Tcl_Interp *interp, Main_Manager_t *mainManager, Mdl_Module_t *module, array_t *invNameArray, array_t *typedExprArray);
EXTERN void Rtm_ModuleBuildTransitionRelation(Rtm_Info_t *rtmInfo, Mdl_Module_t *module);
EXTERN mdd_t * Rtm_AtomBuildTransitionRelation(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo);
EXTERN void Rtm_AtomBuildUpdateCmdListMdds(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo, mdd_t **atomMddPtr, mdd_t **guardOrMddPtr);
EXTERN void Rtm_AtomBuildWaitCmdListMdds(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo, mdd_t **atomMddPtr, mdd_t **timedGuardOrMddPtr);
EXTERN mdd_t * Rtm_GuardBuildMdd(Rtm_Info_t *rtmInfo, Atm_Expr_t *guard);
EXTERN mdd_t * Rtm_AssignmentsBuildMdd(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo, Atm_Cmd_t *cmd);
EXTERN mdd_t * Rtm_InitAssignmentsBuildMdd(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo, Atm_Cmd_t *cmd);
EXTERN void Rtm_AtomProcessDefaultUpdateCommand(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo, mdd_t **atomMddPtr, mdd_t **guardOrMddPtr);
EXTERN void Rtm_AtomProcessImplicitUpdateCommand(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo, mdd_t **atomMddPtr, mdd_t **guardOrMddPtr);
EXTERN mdd_t * Rtm_WaitAssignmentsBuildMdd(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo, Atm_Cmd_t *cmd);
EXTERN mdd_t * Rtm_BuildOneWaitAssignmentMdd(Atm_Expr_t *expr, Rtm_Info_t *rtmInfo, int pid, int uid);
EXTERN mdd_t * Rtm_BuildUnconstrainedTimerIncMdd(int pid, int uid, Var_Variable_t *var, Rtm_Info_t *rtmInfo);
EXTERN Tcl_HashTable * Rtm_RegionManagerReadRegionTable(Rtm_RegionManager_t *rtmRegionManager);
EXTERN Rtm_Info_t * Rtm_RegionManagerObtainRtmInfoFromModule(Rtm_RegionManager_t *rtmRegionManager, Mdl_Module_t *module);
EXTERN Rtm_RegionManager_t * Rtm_RegionManagerAlloc();
EXTERN void Rtm_RegionManagerAddModuleToRtmInfo(Rtm_RegionManager_t *rtmRegionManager, Mdl_Module_t *module, Rtm_Info_t *rtmInfo);
EXTERN mdd_t * Rtm_ModuleBuildInitialRegion(Rtm_Info_t *rtmInfo, Mdl_Module_t *module);
EXTERN mdd_t * Rtm_AtomBuildInitialRegion(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo);
EXTERN void Rtm_AtomBuildInitCmdListMdds(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo, mdd_t **atomMddPtr, mdd_t **guardOrMddPtr);
EXTERN void Rtm_AtomProcessDefaultInitCommand(Atm_Atom_t *atom, Rtm_Info_t *rtmInfo, mdd_t **atomMddPtr, mdd_t **guardOrMddPtr);
EXTERN Rtm_Region_t * Rtm_AllocateRegionStruct(Mdl_Module_t *module, mdd_t *regionSet);
EXTERN unsigned long Rtm_RtmRegionManagerReadCounter(Rtm_RegionManager_t *rtmRegionManager);
EXTERN unsigned long Rtm_RtmRegionManagerIncrementCounter(Rtm_RegionManager_t *rtmRegionManager);
EXTERN void Rtm_MddPrintCubes(mdd_t *mdd, Rtm_Info_t *rtmInfo, int level);
EXTERN void Rtm_DebugTracePrint(Rtm_Info_t *rtmInfo, array_t *savedOnions, int step, mdd_t *notInv, array_t *eventIdArray, array_t *histDependIdArray, char *invName, char *moduleName, Tcl_Interp *interp);
EXTERN mdd_t  * Rtm_MddPickMinterm(mdd_t *mdd, Rtm_Info_t *rtmInfo);

/**AutomaticEnd***************************************************************/

#endif /* _ */










