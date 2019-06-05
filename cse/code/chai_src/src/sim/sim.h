/**CHeaderFile*****************************************************************

  FileName    [sim.h]

  PackageName [sim]

  Synopsis    [External header file for simulation.]

  Description [This file contains the exported function prototype and data
  structure declarations. Currently, there are three different modes of
  simulation: random simulation (the simulator executes all the atoms),
  manual/step simulation (user guides the execution of all the atoms), and
  game simulation (the simulator executes some of the atom, and the user
  controls the others).]

  SeeAlso     [simInt.h]

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

  Revision    [$Id: sim.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _SIM
#define _SIM

#include  "enum.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct SimManagerStruct Sim_Manager_t;
typedef struct SimInfoStruct Sim_Info_t;
typedef struct SimAtomStruct Sim_Atom_t;

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

EXTERN char* Sim_SimAtomReadName(Sim_Atom_t* simAtom);
EXTERN boolean Sim_SimAtomReadIsUserAtom(Sim_Atom_t* simAtom);
EXTERN Atm_Atom_t* Sim_SimAtomReadAtom(Sim_Atom_t* simAtom);
EXTERN array_t * Sim_SimAtomReturnSortedArray(Mdl_Module_t * module, array_t * atomArray, int isUserAtom);
EXTERN void Sim_SimAtomArrayFree(array_t *simAtomArray);
EXTERN int Sim_Init(Tcl_Interp *interp, Main_Manager_t *manager);

/**AutomaticEnd***************************************************************/

#endif /* _SIM */












