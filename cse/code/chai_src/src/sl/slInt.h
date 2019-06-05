/**CHeaderFile*****************************************************************

  FileName    [slInt.h]

  PackageName [sl]

  Synopsis    [Internal Header File for SL]

  Description [optional]

  SeeAlso     [optional]

  Author      [Luca de Alfaro & Freddy Mang]

  Copyright   [Copyright (c) 1994-2002 The Regents of the Univ. of California.
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

  Revision    [$Id: slInt.h,v 1.30 2003/04/02 23:57:20 ashwini Exp $]

******************************************************************************/

#ifndef _SLINT
#define _SLINT

#include "sl.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/
struct SlFSMStruct {
  array_t * initBdd;
  array_t * updateBdd;
  array_t * contrvarL;
  array_t * extvarL;
  array_t * erasedvarL;
  array_t * dependency;
};

struct SlIntfStruct { 
  array_t * levels;
};
    

struct SlGlobalSymbolTableEntry {
  char * name;
  Sl_GSTEntryKind_t kind;

  union {
    Sl_FSM_t fsm;
    Sl_INTF_t intf; 
    array_t * bddArray;
    array_t * idArray;
    array_t * intArray;
    array_t * stringArray;
  } obj;
};


typedef struct {
  char * info; /* used to store dependency relation */ 
  int first; /* unprimed variable id */ 
  int second; /* primed variable id */ 
  int flag; /* this is used for various algorithms, such as cycle detection */ 
} Pair_t;

/*Ashwini added TauStruct */
typedef struct {
    mdd_t * tau;
    array_t * cntrlvars; /* the vars controlled by tau */
    array_t * dependArray; /* array of TauStructs that depend on 
			      * this tauStruct */
    int flag;
} TauStruct;

typedef struct { 
    array_t * Ivars; 
    array_t * Ovars; 
    array_t * IinitBdds; 
    array_t * OinitBdds;
    array_t * IupdateBdds;
    array_t * OupdateBdds;
} Level_t;

typedef struct Int_Stack_Struct Int_Stack_t; 
struct Int_Stack_Struct { 
    int n; 
    Int_Stack_t * next;
};

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
st_table * Sl_GST;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/
#define GSTEntryIsBdd(entry)  ((entry) && ((entry)->kind == Sl_BDD_c))
#define GSTEntryIsFsm(entry)  ((entry) && ((entry)->kind == Sl_FSM_c))
#define GSTEntryIsIdArray(entry)  ((entry) && ((entry)->kind == Sl_IdArray_c))
#define GSTEntryIsIntArray(entry)  ((entry) && ((entry)->kind == Sl_IntArray_c))
#define GSTEntryIsStringArray(entry)  ((entry) && ((entry)->kind == Sl_StringArray_c))

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN int SlDumpBddCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlCreateCommandFreddy(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int SlCreateCommandLuca(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int SlCreateCommandIntf(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int SlCreateCommandSim(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int SlFSMCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPrintBddCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPickMintermCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlBDDCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPrintVarCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPrintSupportCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN mdd_t  * SlPickMddMinterm(mdd_t *mdd);
EXTERN void SlAssignMddIds(array_t * moduleArray, array_t * varArray);
EXTERN void SlAssignMddIdsForEachModule(array_t * moduleArray);
EXTERN void SlAssignMddIdsForVariable(Sym_Info_t * symInfo, Var_Variable_t * var);
EXTERN void SlDetermineControllabilityOfVariables(Mdl_Module_t * module, Sl_GSTEntry_t * gstEntry);
EXTERN int SlObtainUnprimedBddId(char * name);
EXTERN int SlObtainPrimedBddId(char * name);
EXTERN int SlObtainBddId(char * name);
EXTERN void SlMddPrintCubes(mdd_t *mdd, int level);
EXTERN array_t* SlPrimeVars(array_t *unprimedIdArray);
EXTERN array_t* SlUnprimeVars(array_t *primedIdArray);
EXTERN mdd_t* SlPrimeMdd(mdd_t* m);
EXTERN mdd_t* SlUnprimeMdd(mdd_t* m);
EXTERN void SlAddAwaitDependencyToFSM(Mdl_Module_t * module, Sl_GSTEntry_t * gstEntry);
EXTERN array_t * SlDependencyMerge(array_t * dep1, array_t * dep2);
EXTERN array_t * SlReturnSortedIds(Sl_FSM_t fsm);
EXTERN int SlFreeBddCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPrimeCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlGetCtrlVarIdsCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlEQuantifyIdsCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN mdd_t * SlRandomSliceBdd(int id);
EXTERN array_t * SlGetArrayOfCubes(int id);
EXTERN int SlGetMddSupportCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPrintAwaitDepCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPrintAwaitVarCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlWriteOrderCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlCopyCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlCheckImplCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlComposeCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlLcontrCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlLcontrNCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlWcontrCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlModWcontrCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlReachCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlReachHistoryCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlCheckInvCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlCheckTrInvCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlRestrictCmd (ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlCheckSimCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlInitRandCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlCheckRandInvCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlTrInvCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlContrReachSpecCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlContrReachFsmCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlEraseCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlAndCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlOrCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlNotCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlTrueCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlRetrictCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlGetInitCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlGetTRCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlBddSizeCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPredSizeCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlMakeTrInvCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlControlCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlGetFsmVarsCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlMakeVarListCmd(ClientData clientData, Tcl_Interp *interp, int  argc, char** argv);
EXTERN int SlCreateIntArrayCmd (ClientData clientData, Tcl_Interp *interp, int
                                argc,  char** argv);
EXTERN int SlAddToArrayCmd(ClientData clientData,  Tcl_Interp *interp, int
                           argc,  char** argv);
EXTERN int SlPrintObjCmd (ClientData clientData,  Tcl_Interp *interp, int
                          argc, char** argv);
EXTERN int SlGetVarNamesFromIdsCmd(  ClientData clientData, Tcl_Interp
                                     *interp,  int argc,  char** argv);
EXTERN int SlPreNCmd (ClientData clientData,Tcl_Interp *interp, int argc,
                      char** argv);
EXTERN int
SlPrintTCAwaitDepCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int SlFreeObjCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlMakeStringArrayCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlIsStateInMddCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlMakeIntArrayCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlGetPrimedIdFromVarNameCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlGetIdFromVarNameCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlMakeIdArrayCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlToTclCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlGetVarNameFromIdCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlGetPartitionedTRCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlIsVVStateInMddCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlConjoinAndCopyCmd (
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlGetPrimedIdFromIdCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);
EXTERN int
SlGetUnprimedIdFromIdCmd(
  ClientData clientData,
  Tcl_Interp *interp,
  int argc,
  char** argv);

EXTERN int SlPrimeIdArrayCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlUnprimeIdArrayCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);

EXTERN int SlGetTopMddId(mdd_t * mdd);
EXTERN int SlReturnRandomValue(int range); 
EXTERN mdd_t * SlGetLiteral(int id, int pos);

/* Interface commands */ 

EXTERN int SlMakeIntfCmd (ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlMakeIntfOutCmd (ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPrintLevelsCmd (ClientData clientData, Tcl_Interp *interp, int argc, char** argv); 
EXTERN int SlComposeIntfCmd (ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPrintIntfCmd (ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlPrintIntfSupportCmd (ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlCheckIntfRefCmd (ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int SlStrengthenIntfCmd (ClientData clientData, Tcl_Interp *interp, int argc, char** argv);


/* Simulation+Symbolic commands */ 
EXTERN void SlRandomcubeInvChk (Sl_GSTEntry_t *inFsm, int task, array_t *invariant, int n_steps, int mode, unsigned int seed);
EXTERN mdd_t * SlGetRandomCubeInMdd ( mdd_t *mdd, unsigned int seed );
EXTERN int SlRandomcubeInvChkCmd(ClientData clientData, Tcl_Interp *interp, int argc, char** argv);
EXTERN int  SlInvBuildMdd (ClientData clientdata, Tcl_Interp *interp,
	int argc, char ** argv);
EXTERN int  SlMddEqual (ClientData clientdata, Tcl_Interp *interp,
	int argc, char ** argv);
EXTERN int  SlMddSubstitute (ClientData clientdata, Tcl_Interp *interp,
	int argc, char ** argv);
EXTERN int  SlTauPlusMinus (ClientData clientdata, Tcl_Interp *interp,
	int argc, char ** argv);
EXTERN int  SlBetterUPreComb (ClientData clientdata, Tcl_Interp *interp,
	int argc, char ** argv);
EXTERN int  SlBetterUPre (ClientData clientdata, Tcl_Interp *interp,
	int argc, char ** argv);
EXTERN int SlAtomSimulateCmd(ClientData clientData, Tcl_Interp *interp,
	int argc, char** argv);
/* made the findPair function of slFreddy.c public -Ashwini*/ 
EXTERN Pair_t * SlfindPair(array_t * dependency, int unprimedId);


/* === */ 

EXTERN void SlGSTInit(Tcl_Interp *interp, Main_Manager_t * manager);
EXTERN Tcl_Interp * SlGetTclInterp();
EXTERN Mdl_Manager_t * SlGetModuleManager();
EXTERN mdd_manager * SlGetMddManager();
EXTERN Sym_RegionManager_t * SlGetRegionManager();
EXTERN st_table * SlGetNameToIdTable();
EXTERN array_t* SlGetIdPairArray();
EXTERN void SlAddIdPair(Pair_t * pair);
EXTERN int SlReturnPrimedId(int id);
EXTERN int SlReturnUnprimedId(int id);
EXTERN Var_Variable_t * SlReturnVariableFromId(int id);
EXTERN Mocha_SlObject_t * Mocha_SlGetObjectByName(char * objName);
EXTERN array_t * Mocha_SlGetBDDArray(Mocha_SlObject_t * slObj);
EXTERN array_t * Mocha_SlGetIdArray(Mocha_SlObject_t * slObj);
EXTERN array_t* Mocha_SlGetIntArray(Mocha_SlObject_t * slObj);
EXTERN array_t * Mocha_SlGetStringArray(Mocha_SlObject_t * slObj);
EXTERN int Mocha_StateInMdd(mdd_t * mdd, array_t * idArray,  array_t *
                            valueArray) ;
EXTERN void SlDeleteGstEntry(char * name);
EXTERN void SlInsertGstEntry(Sl_GSTEntry_t * gstEntry);
EXTERN void SlGstEntryFree(Sl_GSTEntry_t * gstEntry);
EXTERN Sl_GSTEntry_t * SlGstGetEntry(char * name);
EXTERN void SlFreeArrayOfBdd(array_t * bddArray);
EXTERN Pair_t * SlPairAlloc();
EXTERN int SlCompareInt(char *obj1, char *obj2);

/* Useful also elsewhere */ 
EXTERN array_t * SlIdPairArray;

/* in SlUtil.c */ 

EXTERN int SlImplicationCheck(array_t *bddArray1, array_t *bddArray2);
EXTERN array_t* SlArrayUnion(array_t *inArray1, array_t *inArray2);
EXTERN array_t * SlArrayDifference(array_t *lhs, array_t *rhs);
EXTERN array_t * SlArrayIntersection (array_t *lhs, array_t *rhs); 
EXTERN int SlCheckSupportSubset(mdd_t *formula, array_t *allowedVars, char *errorMsg);
EXTERN array_t* SlBddArrayDupe(array_t *dupeme);
EXTERN array_t * SlIntArrayDupe(array_t *dupeme);
EXTERN mdd_t * SlConjoin(array_t *bddArray);
EXTERN mdd_t * SlSmoothConjoin(array_t *bddArray, array_t *variables);
EXTERN array_t * SlSingleMddArray(mdd_t *inputMdd);
EXTERN mdd_t* SlComputeReach(Sl_GSTEntry_t *inFsm, int task, array_t *invariant, int mode);
EXTERN int SlPrintBddStats(mdd_t *inBdd, array_t* support);
EXTERN void SlPrintVarArray(array_t *a);
EXTERN mdd_t * SlGetInit(Sl_GSTEntry_t *fsmEntry);
EXTERN int SlPrintVarNames (array_t *idArray);
EXTERN int SlTclPrintVarNames (array_t *idArray);
EXTERN int SlAreArrayEqual (array_t *lhs, array_t *rhs);
EXTERN int SlIsArraySubset (array_t *lhs, array_t *rhs); 
EXTERN int SlDetectCycles (array_t *dependency);
EXTERN Int_Stack_t * SlStackAlloc (); 
EXTERN Int_Stack_t * SlStackPush (Int_Stack_t *stack, int i);
EXTERN Int_Stack_t * SlStackPop (Int_Stack_t *stack, int *ip);
EXTERN int SlStackIsEmpty (Int_Stack_t *stack);
EXTERN Level_t * SlLevelAlloc(); 
EXTERN void SlFreeDependency (array_t * dependency);
EXTERN void SlFreeLevels (array_t * levels); 
EXTERN void SlFreeMddList (array_t * list); 
EXTERN void SlMddArrayAppend (array_t *a, array_t *b); 
EXTERN array_t * SlVarsOfAllLevels (array_t *levels); 
EXTERN array_t * SlIVarsOfAllLevels (array_t *levels); 
EXTERN array_t * SlOVarsOfAllLevels (array_t *levels); 
EXTERN array_t * SlEArrayQuantify (array_t * conjuncts, array_t * variables);
EXTERN mdd_t *   SlEMddQuantify   (array_t * conjuncts, array_t * variables);

EXTERN void SlPrintMddArray (array_t * a);
EXTERN array_t * SlHistoryFree (array_t * allvars, array_t * trans_rel);

/**AutomaticEnd***************************************************************/

#endif /* _SLINT */
