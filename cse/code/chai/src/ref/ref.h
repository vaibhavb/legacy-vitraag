/**CHeaderFile*****************************************************************

  FileName    [ref.h]

  PackageName [ref]

  Synopsis    [routines for refinement checking between modules]

  Description [This package has routines that check if one module refines
                another.
		1. refinement check is provided via the check_refine
		   command. It requires that all history dependent 
		   variables of the specification be witnessed in the 
		   implementation
		2. simulation check is provided via the check_simulate
		   command.
		3. compositonal refinement facility is provided by the
		   check_refine_atom command. This command helps the 
		   user carry out one proof oligation in a compositional 
		   proof. ]

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

  Revision    [$Id: ref.h,v 1.1.1.1 2001/09/22 20:42:57 luca Exp $]

******************************************************************************/

#ifndef _REF
#define _REF

#include  "sym.h"
#include  "img.h"

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

EXTERN int Ref_Init(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Ref_Reinit(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN int Ref_End(Tcl_Interp *interp, Main_Manager_t *manager);
EXTERN boolean Ref_CheckAtomRefinement(Mdl_Manager_t *mdlManager, Mdl_Module_t * spec, Atm_Atom_t *specAtom, Mdl_Module_t *impl, lsList varNameList, lsList forceVarNameList, Tcl_Interp *interp, boolean checkRef, boolean verbose, boolean keepIntermediate, int impBoundary, boolean checkAtEnd, char *orderFileName);
EXTERN boolean Ref_CheckNohiddenRefinement(Mdl_Module_t * implModule, Sym_Info_t *implSymInfo, Mdl_Module_t *specModule, Sym_Info_t *specSymInfo, boolean verbose, boolean checkAtEnd);
EXTERN Atm_Atom_t * Ref_VariableReadControlAtom(Var_Variable_t *var);
EXTERN boolean Ref_CheckSimulationUsingPre(Mdl_Module_t * implModule, Sym_Info_t *implSymInfo, Mdl_Module_t *specModule, Sym_Info_t *specSymInfo, boolean verbose);
EXTERN boolean Ref_CheckImplCompatibility(Mdl_Module_t *spec, Mdl_Module_t *impl);
EXTERN Var_Variable_t * Ref_LookupVarList(lsList varList, char *name);
EXTERN boolean Ref_IsMemberOfAtomList(lsList atomList, Atm_Atom_t *atom);
EXTERN boolean Ref_IsMemberOfNameList(lsList nameList, char *name);

/**AutomaticEnd***************************************************************/

#endif /* _ */
