/**CFile***********************************************************************

  FileName    [mcDependency.c]

  PackageName [mc]

  Synopsis    [Routines manipulating the atom dependency table.]

  Description [In ATL, the order of execution of the agents is of vital
  importance, since it effects the type and order of quantification of the
  variables. In this file, there is a structure, called McAtom, for each atom
  of the module. The variable, McGlobalAtomDependencyInfo, stores a directed
  acyclic graph whose arc goes from the McAtom that is being awaited to the
  one that is awaiting this McAtom.]

  SeeAlso     [mcMc.c]

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

******************************************************************************/

#include  "mcInt.h"


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
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Check the validity of the agents]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
array_t *
McAgentsCheckValid(
  lsList agentList,
  Mdl_Module_t* module)
{
  
  lsGen gen;
  Atlp_Agent_t * agent;
  array_t * componentAtomArray, *agentNameArray;
  Mdl_Expr_t *mexpr = Mdl_ModuleReadModuleExpr(module);
  
  if (mexpr == NIL(Mdl_Expr_t) || (agentList == NULL))
    return FALSE;

  /* prepare an array of agent names */
  agentNameArray = array_alloc(char *, 0);
  lsForEachItem(agentList, gen, agent) {
    char * name = Atlp_AgentReadName(agent);
    array_insert_last(char*, agentNameArray, name);
  }
        
  /* read the atoms */
  componentAtomArray =
      Mdl_ModuleObtainComponentAtomArray(module, agentNameArray);

  array_free(agentNameArray);
  return componentAtomArray;
}
