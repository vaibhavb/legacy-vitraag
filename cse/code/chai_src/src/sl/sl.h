/**CHeaderFile*****************************************************************

  FileName    [sl.h]

  PackageName [sl]

  Synopsis    [A small script secretful language]

  Description [optional]

  SeeAlso     [optional]

  Author      [Luca de Alfaro & Freddy Mang]

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

  Revision    [$Id: sl.h,v 1.2 2001/12/27 09:54:01 luca Exp $]

******************************************************************************/

#ifndef _SL
#define _SL

#include "main.h"
#include "mdl.h"
#include "sym.h"
#include "list.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef enum {
  Sl_FSM_c,
  Sl_BDD_c, 
  Sl_INTF_c, 
  Sl_IdArray_c,
  Sl_IntArray_c,
  Sl_StringArray_c
} Sl_GSTEntryKind_t;

typedef struct SlFSMStruct Sl_FSM_t;
typedef struct SlIntfStruct Sl_INTF_t; 
typedef struct SlGlobalSymbolTableEntry Sl_GSTEntry_t;
typedef Sl_GSTEntry_t Mocha_SlObject_t;


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

EXTERN int Sl_Init(Tcl_Interp *interp, Main_Manager_t *manager);

/**AutomaticEnd***************************************************************/

#endif /* _SL */

