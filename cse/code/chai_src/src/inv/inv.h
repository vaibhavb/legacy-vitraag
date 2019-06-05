/**CHeaderFile*****************************************************************

  FileName    [Inv.h]

  PackageName [Inv]

  Synopsis    [Routines for parsing, writing and accessing invariants.]

  Description [This package implements invariant checking. It provides routines 
               for parsing invariants and for interfacing with the symbolic 
               and enumerative reachability engines.]

  Author      [Freddy Mang and Shaz Qadeer]

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

******************************************************************************/

#ifndef _INV
#define _INV

#include  "main.h"
#include  "atm.h"
#include  "mdl.h"
#include  "enum.h"
#include  "sym.h"

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct InvInvariantStruct Inv_Invariant_t;
typedef struct InvManagerStruct Inv_Manager_t;


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN int Inv_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Inv_Reinit(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Inv_End(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN Inv_Manager_t * Inv_ReadInvManager();
EXTERN Inv_Manager_t * Inv_ManagerAlloc();
EXTERN void Inv_ManagerFree(Inv_Manager_t * manager);
EXTERN lsList Inv_FileParseFormula(Inv_Manager_t *manager, FILE * fp);
EXTERN array_t * Inv_ManagerReadInvariants(Inv_Manager_t *manager);
EXTERN array_t * Inv_ManagerReadInvariantNames(Inv_Manager_t *manager);
EXTERN Inv_Invariant_t * Inv_ManagerReadInvariantFromName(Inv_Manager_t * manager, char * name);
EXTERN void Inv_InvariantArrayFree(array_t * formulaArray);
EXTERN Atm_Expr_t * Inv_InvariantReadTypedExpr(Inv_Invariant_t *invariant);
EXTERN char * Inv_InvariantReadName(Inv_Invariant_t *invariant);

/**AutomaticEnd***************************************************************/

#endif /* _INV */








