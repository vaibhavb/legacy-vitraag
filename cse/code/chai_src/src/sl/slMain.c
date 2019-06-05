/**CFile***********************************************************************

  FileName    [slMain.c]

  PackageName [sl]

  Synopsis    []

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

******************************************************************************/

#include "slInt.h" 

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
Mdl_Manager_t * SlModuleManager;
mdd_manager * SlMddManager;
Sym_RegionManager_t * SlRegionManager;
Tcl_Interp * SlInterp;
array_t * SlIdPairArray;
st_table * SlNameToIdTable;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Initializes the var package.]

  SideEffects        [The command, show_types is registered.]

  SeeAlso            [Var_Reinit, Var_End]

******************************************************************************/
int
Sl_Init(
  Tcl_Interp *interp,
  Main_Manager_t *manager)
{

  
  SlGSTInit(interp, manager);
  SlCreateCommandFreddy(interp, manager);
  SlCreateCommandLuca(interp, manager); 
  SlCreateCommandIntf(interp, manager); 
  SlCreateCommandSim(interp, manager); 

  return TCL_OK;
  
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlGSTInit(
  Tcl_Interp *interp,
  Main_Manager_t * manager)
{
  Sl_GST = st_init_table(strcmp, st_strhash);
  
  SlInterp = interp;
  SlModuleManager = (Mdl_Manager_t *)
      Main_ManagerReadModuleManager(manager);
  SlRegionManager =   (Sym_RegionManager_t *)
      Main_ManagerReadRegionManager(manager);

  SlMddManager = mdd_init_empty();
  Main_ManagerSetMddManager((Main_Manager_t *) manager,
                            (Main_MddManagerGeneric) SlMddManager);

  SlIdPairArray = array_alloc(Pair_t * , 0);
  SlNameToIdTable = st_init_table(strcmp, st_strhash);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Tcl_Interp * 
SlGetTclInterp()
{
  return SlInterp;
  
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Mdl_Manager_t *
SlGetModuleManager()
{
  return SlModuleManager;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
mdd_manager *
SlGetMddManager()
{

  return SlMddManager;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Sym_RegionManager_t *
SlGetRegionManager()
{
  return SlRegionManager;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
st_table *
SlGetNameToIdTable()
{
  return SlNameToIdTable;
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

  
******************************************************************************/
array_t*
SlGetIdPairArray()
{
  return SlIdPairArray;

}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
void
SlAddIdPair(
  Pair_t * pair)
{
  int max;
  int len = array_n(SlIdPairArray);
  int unprimedId = pair -> first;
  int primedId = pair -> second;

  max = (primedId > unprimedId)? primedId: unprimedId;
  
  while (len <= max) {
    array_insert_last(Pair_t * , SlIdPairArray, 0);
    len ++;
  }
  
  array_insert(Pair_t *, SlIdPairArray, primedId, pair);
  array_insert(Pair_t *, SlIdPairArray, unprimedId, pair);
  
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlReturnPrimedId(
  int id)
{
  Pair_t * pair;

  if (id >= array_n(SlIdPairArray))
    return -1;
  
  pair = array_fetch(Pair_t *, SlIdPairArray, id);

  return pair -> second;

}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
int
SlReturnUnprimedId(
  int id)
{
  Pair_t * pair;

  if (id >= array_n(SlIdPairArray))
    return -1;
  
  pair = array_fetch(Pair_t *, SlIdPairArray, id);
  
  return pair -> first;
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Var_Variable_t *
SlReturnVariableFromId(
  int id)
{
  Pair_t * pair;

  if ((id >= array_n(SlIdPairArray)) || (id < 0))
    return 0;
  
  pair = array_fetch(Pair_t *, SlIdPairArray, id);

  return ((Var_Variable_t *) pair -> info);
  
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/






