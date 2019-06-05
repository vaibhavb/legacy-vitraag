/**CFile***********************************************************************

  FileName    [rtmUtil.c]

  PackageName [rtm]

  Synopsis    []

  Description []

  SeeAlso     []

  Author      [Serdar Tasiran]

  Copyright   [Copyright (c) 1994-1997 The Regents of the Univ. of California.
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

#include "rtmInt.h" 

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
#define Main_MochaPrint printf

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
Rtm_Info_t *
Rtm_RtmInfoAlloc(
  Tcl_Interp *interp,
  Mdl_Module_t *module,
  mdd_manager *manager)
{
  Rtm_Info_t *rtmInfo = ALLOC(Rtm_Info_t, 1);

  rtmInfo->symInfo = Sym_SymInfoAlloc(interp, module, manager);
  
  return rtmInfo;
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

  
/**Function********************************************************************

  Synopsis           [Check if the given variable is in the true support of
                      the given mdd ]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]
 
******************************************************************************/
boolean
RtmMddInTrueSupport(
  mdd_t *m,
  int id,
  Rtm_Info_t *rtmInfo)
{
  array_t * varidArray;
  int i;
  
  varidArray = mdd_get_support(Sym_SymInfoReadManager(rtmInfo->symInfo), m);
  for ( i = 0; i < array_n(varidArray); i++){
    if (id ==  (int)array_fetch(int, varidArray, i))
      return(TRUE);
  }

  return(FALSE);
}

/**Function********************************************************************

  Synopsis           [utility to get and MDD literal]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
mdd_t *
RtmGetMddLiteral(
  mdd_manager *manager,
  int id,
  int value)
{
  array_t *values = array_alloc(int, 0);
  mdd_t *mdd;
  
  array_insert(int, values, 0, value);
  mdd = mdd_literal(manager, id, values);

  if(!mdd){
    Main_MochaErrorPrint("RtmGetMddLiteral: got NULL mdd for variable id %d val %d\n",
                         id, value);
    exit(1);
  }
  
  array_free(values);
  return(mdd);
}
  
/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/













