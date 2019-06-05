/**CHeaderFile*****************************************************************

  FileName    [main.h]

  PackageName [main]

  Synopsis    [External header file for the main package.]

  Description []

  SeeAlso     []

  Author      [Shaz Qadeer]

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

  Revision    [$Id: main.h,v 1.2 2003/05/24 07:06:33 vaibhav Exp $]

******************************************************************************/

#ifndef _MAIN
#define _MAIN

#include <stdlib.h>
#include <stdarg.h>
#include "tcl.h"
#include "util.h"
#include "array.h"
#include "list.h"
#include "st.h"
#include "bdd.h"
#include "mdd.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define Main_MAXMSGLEN 255
#define INDEX_STRING_LENGTH 50

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct MainManagerStruct Main_Manager_t;
typedef void * Main_MdlManagerGeneric;
typedef void * Main_TypeManagerGeneric;
typedef void * Main_StateManagerGeneric;
typedef void * Main_RegionManagerGeneric;
typedef void * Main_RtmRegionManagerGeneric;
typedef void * Main_MddManagerGeneric;


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

EXTERN int Main_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Main_Reinit(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Main_End(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN Main_MdlManagerGeneric Main_ManagerReadModuleManager(Main_Manager_t *manager);
EXTERN Main_TypeManagerGeneric Main_ManagerReadTypeManager(Main_Manager_t *manager);
EXTERN Main_StateManagerGeneric Main_ManagerReadStateManager(Main_Manager_t *manager);
EXTERN Main_RegionManagerGeneric Main_ManagerReadRegionManager(Main_Manager_t *manager);
EXTERN Main_MddManagerGeneric Main_ManagerReadMddManager(Main_Manager_t *manager);
EXTERN void Main_ManagerSetModuleManager(Main_Manager_t *manager, Main_MdlManagerGeneric mdlManager);
EXTERN void Main_ManagerSetTypeManager(Main_Manager_t *manager, Main_TypeManagerGeneric typeManager);
EXTERN void Main_ManagerSetStateManager(Main_Manager_t *manager, Main_StateManagerGeneric stateManager);
EXTERN void Main_ManagerSetRegionManager(Main_Manager_t *manager, Main_RegionManagerGeneric rgnManager);
EXTERN void Main_ManagerSetMddManager(Main_Manager_t *manager, Main_MddManagerGeneric mddManager);
EXTERN Main_RtmRegionManagerGeneric Main_ManagerReadRtmRegionManager(Main_Manager_t *manager);
EXTERN void Main_ManagerSetRtmRegionManager(Main_Manager_t *manager, Main_RtmRegionManagerGeneric rtmRegionManager);
EXTERN void Main_MochaPrint(char* format, ...);
EXTERN void Main_MochaErrorPrint(char* format, ...);
EXTERN void Main_MochaFilePrint(FILE * fp, char* format, ...);
EXTERN void Main_AppendResult(char *format, ...);
EXTERN void Main_AppendElement(char *format, ...);
EXTERN void Main_ClearResult();
EXTERN int Main_ReturnCreationTime();
EXTERN char * Main_ConvertIntToString(int i);
EXTERN char * Main_strConcat(int numArgs, ...);
/**AutomaticEnd***************************************************************/

#endif /* _MAIN */
