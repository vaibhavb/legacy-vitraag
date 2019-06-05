/**CFile***********************************************************************

  FileName    [atmUtil.c]

  PackageName [atm]

  Synopsis    [Provides utility functions for manipulating the data structures 
  in the atm package.]

  SeeAlso     [atmInt.h, atm.h]

  Author      [Freddy Mang, Shaz Qadeer]

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

#include  "atmInt.h"


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define MAXNAMELEN 10


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
static int AtmCounter = 0;


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

  Synopsis    [Allocates an atom.]

  Description [The function allocates an atom with the given name. name should
  not be NULL. A copy of name is made. Hence, it is the caller's responsibility
  to free name.]  empty (e.g. empty lists)]

  SideEffects [None]

  SeeAlso     [Atm_AtomFree]

******************************************************************************/
Atm_Atom_t*
Atm_AtomAlloc(
  char * name)
{
  Atm_Atom_t   *atom    = ALLOC(Atm_Atom_t, 1);
  atom->name            = util_strsav(name);
  atom->atmType         = Atm_Normal_c;

  atom->initCmdList      = lsCreate();
  atom->updateCmdList    = lsCreate();
  atom->waitCmdList      = lsCreate();

  atom->defaultInitCmd = NIL(Atm_Cmd_t);
  atom->defaultUpdateCmd = NIL(Atm_Cmd_t);
  
  atom->ctrlVarList     = lsCreate();
  atom->readVarList     = lsCreate();
  atom->awaitVarList    = lsCreate();

  atom->preorder = -1;
  atom->postorder = -1;

  return atom;
}

    
/**Function********************************************************************

  Synopsis    [Frees an atom.]

  Description [This function frees an atom.]
  
  SeeAlso     [Atm_AtomAlloc]

******************************************************************************/
void
Atm_AtomFree(
  Atm_Atom_t *atom)
{
  FREE(atom->name);

  if (atom->defaultInitCmd)
    Atm_CmdFree(atom->defaultInitCmd);
  if (atom->defaultUpdateCmd)
    Atm_CmdFree(atom->defaultUpdateCmd);
  
  /* free the cmd list, as well as each of the commands */
  if (atom->initCmdList == atom->updateCmdList) {
    (void) lsDestroy(atom->initCmdList,       (void (*) ()) Atm_CmdFree);
  }
  else {
    (void) lsDestroy(atom->initCmdList,       (void (*) ()) Atm_CmdFree);
    (void) lsDestroy(atom->updateCmdList,     (void (*) ()) Atm_CmdFree);
    (void) lsDestroy(atom->waitCmdList,     (void (*) ()) Atm_CmdFree);
  }
  
  /* free the variable lists, but not the variables */
  (void) lsDestroy(atom->ctrlVarList,       (void (*) ()) NULL);
  (void) lsDestroy(atom->readVarList,       (void (*) ()) NULL);
  (void) lsDestroy(atom->awaitVarList,      (void (*) ()) NULL);

  /* finally free the atom */
  FREE(atom);
  
}

/**Function********************************************************************

  Synopsis           [Allocates a guarded command.]

  Description        [Given a guard and a list of assigments, the routine
                      returns a pointer to a new guarded command.]

  SideEffects        [None]

  SeeAlso            [Atm_CmdFree]

******************************************************************************/
Atm_Cmd_t*
Atm_CmdAlloc(
  Atm_Expr_t* guard,
  lsList assignList)
{
  Atm_Cmd_t* cmd;
  
  cmd = ALLOC(Atm_Cmd_t, 1);
  cmd -> guard = guard;
  cmd -> assignList = assignList;
  
  return cmd;
}

/**Function********************************************************************

  Synopsis           [Frees a guarded command.]

  Description        [The function frees a guarded command.]

  SideEffects        [None]

******************************************************************************/
void
Atm_CmdFree(
  Atm_Cmd_t* cmd)
{
  lsList assignlist;
  lsGen lsgen;
  Atm_Assign_t *assign;
  
  if (cmd == NIL(Atm_Cmd_t)) return;
  else {
    assignlist = Atm_CmdReadAssignList(cmd);
    lsForEachItem(assignlist, lsgen, assign){
      Atm_AssignFree(assign);
    }

    if (cmd->guard)
      Atm_ExprFreeExpression(cmd->guard);
    FREE(cmd);

    lsDestroy (assignlist, (void (*) ()) 0);
  }
}


/**Function********************************************************************

  Synopsis           [Allocates a standard assignment.]

  Description        [The function allocates a standard assignment.]

  SideEffects        [None]

  SeeAlso            [Atm_AssignFree]

******************************************************************************/
Atm_Assign_t*
Atm_AssignStdAlloc(
  Var_Variable_t* var,
  Atm_Expr_t* expr)
{
  Atm_Assign_t* assign;

  if (var == NIL(Var_Variable_t))
    return NIL(Atm_Assign_t);
  else {
    assign = ALLOC(Atm_Assign_t, 1);
    assign->type = Atm_AssignStd_c;
    assign -> var = var;
    assign -> expr = expr;

    return assign;
  }
    
}


/**Function********************************************************************

  Synopsis           [Allocates an event assignment.]

  Description        [The function allocates an event assignment.]

  SideEffects        [None]

  SeeAlso            [Atm_AssignFree]

******************************************************************************/
Atm_Assign_t*
Atm_AssignEventAlloc(
  Var_Variable_t* var)
{
  Atm_Assign_t* assign;

  if (var == NIL(Var_Variable_t))
    return NIL(Atm_Assign_t);
  else {
    assign = ALLOC(Atm_Assign_t, 1);
    assign->type = Atm_AssignEvent_c;
    assign -> var = var;
    assign -> expr = NIL(Atm_Expr_t);
    
    return assign;
  }
    
}


/**Function********************************************************************

  Synopsis           [Allocates an index assignment.]

  Description        [The function allocates an index assignment.]

  SideEffects        [None]

  SeeAlso            [Atm_AssignFree]

******************************************************************************/
Atm_Assign_t*
Atm_AssignIndexAlloc(
  Var_Variable_t* var,
  Atm_Expr_t *indexExpr,
  Atm_Expr_t* expr)
{
  Atm_Assign_t* assign;

  if (var == NIL(Var_Variable_t))
    return NIL(Atm_Assign_t);
  else {
    assign = ALLOC(Atm_Assign_t, 1);
    assign->type = Atm_AssignIndex_c;
    assign -> var = var;
    assign -> expr = expr;
    assign->index.indexExpr = indexExpr;
    
    return assign;
  }
    
}


/**Function********************************************************************

  Synopsis           [Allocates a forall assignment.]

  Description        [The function allocates a forall assignment.]

  SideEffects        [None]

  SeeAlso            [Atm_AssignFree]

******************************************************************************/
Atm_Assign_t*
Atm_AssignForallAlloc(
  Var_Variable_t* var,
  Var_Variable_t *indexVar,
  Atm_Expr_t* expr)
{
  Atm_Assign_t* assign;

  if (var == NIL(Var_Variable_t))
    return NIL(Atm_Assign_t);
  else {
    assign = ALLOC(Atm_Assign_t, 1);
    assign->type = Atm_AssignForall_c;
    assign -> var = var;
    assign -> expr = expr;
    assign->index.indexVar = indexVar;
    
    return assign;
  }
    
}

/**Function********************************************************************

  Synopsis           [Frees an assignment.]

  Description        [This function frees an assignment.]

  SideEffects        [None]

******************************************************************************/
void
Atm_AssignFree(
  Atm_Assign_t* assign)
{
  Atm_AssignType assignType;
  
  if (assign == NIL(Atm_Assign_t))
    return;

  assignType = Atm_AssignReadType(assign);
  
  switch (assignType) {
      case Atm_AssignStd_c :
        Atm_ExprFreeExpression(assign->expr);
        break;
        
      case Atm_AssignForall_c :
        Atm_ExprFreeExpression(assign->expr);
        break;
        
      case Atm_AssignIndex_c :
        Atm_ExprFreeExpression(assign->expr);
        Atm_ExprFreeExpression(assign->index.indexExpr);
        break;
        
      case Atm_AssignEvent_c :
        break;
  }
  
  FREE(assign);
}	       

/**Function********************************************************************

  Synopsis    [Add the given init command to the atom]

  Description [Add the given init command to the atom. If an init command list
              is already present it is freed and cmdList is put in its place.]

  SideEffects [The old init command list will be freed. The content of
  the original init command list will not be freed.  The user should free
  them before calling this function.]

  SeeAlso     [Atm_AtomReadInitCmdList]

******************************************************************************/
void 
Atm_AtomAddInitCommandList(
  Atm_Atom_t *atom,
  lsList cmdlist)
{
  if (atom->initCmdList != NULL){
    lsDestroy(atom->initCmdList, 0);
  }
  atom->initCmdList = cmdlist;
}


    
/**Function********************************************************************

  Synopsis    [Returns the list of init commands for the atom]

  Description [Returns the list of init commands for the atom.
               The user should not free or modfy this list in any
               way.]

  SideEffects [None]

  SeeAlso     [Atm_AtomAddInitCommand]

******************************************************************************/
lsList
Atm_AtomReadInitCommandList(
  Atm_Atom_t *atom)
{
  return(atom->initCmdList);
}


/**Function********************************************************************

  Synopsis    [Returns the default init guarded command of the atom.]

  Description [Returns the default init guarded command of the atom.]
               The user should not free or modfy it.]

  SideEffects [None]

  SeeAlso     [Atm_AtomReadDefaultUpdateCommand]

******************************************************************************/
Atm_Cmd_t *
Atm_AtomReadDefaultInitCommand(
  Atm_Atom_t *atom)
{
  return atom->defaultInitCmd;
}



/**Function********************************************************************

  Synopsis    [Add the given update command list to the atom]

  Description [Add the given update command list to the atom.
              The update command now becomes a property of the atom.
              The user should not free it]

  SideEffects [The old update command list will be freed. The content of
  the original update command list will not be freed.  The user should free
  them before calling this function.]

  SeeAlso     [Atm_AtomReadUpdateCmdList]

******************************************************************************/
void 
Atm_AtomAddUpdateCommandList(
  Atm_Atom_t *atom,
  lsList cmdList)
{
  if (atom->updateCmdList != NULL){
    lsDestroy(atom->updateCmdList, 0);
  }
  atom->updateCmdList = cmdList;
}


    
/**Function********************************************************************

  Synopsis    [Returns the list of update commands for the atom]

  Description [Returns the list of update commands for the atom.
               The user should not free or modfy this list in any
               way.]

  SideEffects [None]

  SeeAlso     [Atm_AtomAddUpdateCommand]

******************************************************************************/
lsList
Atm_AtomReadUpdateCommandList(
  Atm_Atom_t *atom)
{
  return(atom->updateCmdList);
}

/**Function********************************************************************

  Synopsis    [Returns the default update guarded command of the atom.]

  Description [Returns the default update guarded command of the atom.]
               The user should not free or modfy it.]

  SideEffects [None]

  SeeAlso     [Atm_AtomReadDefaultInitCommand]

******************************************************************************/
Atm_Cmd_t *
Atm_AtomReadDefaultUpdateCommand(
  Atm_Atom_t *atom)
{
  return atom->defaultUpdateCmd;
}

/**Function********************************************************************

  Synopsis    [Add the given wait command list to the atom]

  Description [Add the given wait command list to the atom.
              The wait command now becomes a property of the atom.
              The user should not free it]

  SideEffects [None]

  SeeAlso     [Atm_AtomReadWaitCmdList]

******************************************************************************/
void 
Atm_AtomAddWaitCommandList(
  Atm_Atom_t *atom,
  lsList cmdList)
{

  if (atom->waitCmdList != NULL){
    lsDestroy(atom->waitCmdList, 0);
  }

  atom->waitCmdList = cmdList;
}


    
/**Function********************************************************************

  Synopsis    [Returns the list of wait commands for the atom]

  Description [Returns the list of wait commands for the atom.
               The user should not free or modfy this list in any
               way]

  SideEffects [None]

  SeeAlso     [Atm_AtomAddWaitCommandList]

******************************************************************************/
lsList
Atm_AtomReadWaitCommandList(
  Atm_Atom_t *atom)
{
  return(atom->waitCmdList);
}


/**Function********************************************************************

  Synopsis    [Add the given variable to the controlled variable list]

  Description [[Add the given variable to the controlled variable list.
              The variable now becomes a property of the atom.
              The user should not free it]

  SideEffects [ctrlVarList in atom is modified.]

  SeeAlso     [Atm_AtomReadControlVarList]

******************************************************************************/
void 
Atm_AtomAddControlVar(
  Atm_Atom_t *atom,
  Var_Variable_t*  var)
{
  lsNewEnd(atom->ctrlVarList, (lsGeneric) var, LS_NH);
}


    
/**Function********************************************************************

  Synopsis    [Returns the list of control variables for the atom]

  Description [Returns the list of control variables for the atom.
               The user should not free or modfy this list in any
               way]

  SideEffects [None]

  SeeAlso     [Atm_AtomAddControlVar]

******************************************************************************/
lsList 
Atm_AtomReadControlVarList(
  Atm_Atom_t *atom)
{
  return(atom->ctrlVarList);
}



/**Function********************************************************************

  Synopsis    [Add the given variable to the read variable list]

  Description [[Add the given variable to the read variable list.
              The variable now becomes a property of the atom.
              The user should not free it]

  SideEffects [None]

  SeeAlso     [Atm_AtomReadReadVarList]

******************************************************************************/
void 
Atm_AtomAddReadVar(
  Atm_Atom_t *atom,
  Var_Variable_t* var)
{
  lsNewEnd(atom->readVarList, (lsGeneric) var, LS_NH);
}


    
/**Function********************************************************************

  Synopsis    [Returns the list of read variables for the atom]

  Description [Returns the list of read variables for the atom.
               The user should not free or modfy this list in any
               way]

  SideEffects [None]

  SeeAlso     [Atm_AtomAddReadVar]

******************************************************************************/
lsList 
Atm_AtomReadReadVarList(
  Atm_Atom_t *atom)
{
  return(atom->readVarList);
}


/**Function********************************************************************

  Synopsis    [Add the given variable to the await variable list]

  Description [[Add the given variable to the await variable list.
              The variable now becomes a property of the atom.
              The user should not free it]

  SideEffects [awaitVarList is modified.]

  SeeAlso     [Atm_AtomReadAwaitVarList]

******************************************************************************/
void 
Atm_AtomAddAwaitVar(
  Atm_Atom_t *atom,
  Var_Variable_t* var)
{
  lsNewEnd(atom->awaitVarList, (lsGeneric) var, LS_NH);
}


    
/**Function********************************************************************

  Synopsis    [Returns the list of await variables for the atom]

  Description [Returns the list of await variables for the atom.
               The user should not free or modfy this list in any
               way]

  SideEffects [None]

  SeeAlso     [Atm_AtomAddAwaitVar]

******************************************************************************/
lsList 
Atm_AtomReadAwaitVarList(
  Atm_Atom_t *atom)
{
  return atom->awaitVarList;
}

/**Function********************************************************************

  Synopsis           [Computes the set of variables that are both read and
                      awaited by an atom.]

  Description        [The function returns a list of variables that are both
                      read and awaited by atom.]

  SideEffects        [None]

  SeeAlso            [Atm_AtomReadReadVarList, Atm_AtomReadAwaitVarList]

******************************************************************************/
lsList
Atm_AtomObtainReadAwaitVarList(
  Atm_Atom_t *atom)
{
  lsGeneric var1, var2;
  lsGen gen1, gen2;
  lsList readVarList = Atm_AtomReadReadVarList(atom);
  lsList awaitVarList = Atm_AtomReadAwaitVarList(atom);
  lsList newList = lsCreate();
  
  lsForEachItem(readVarList, gen1, var1) {
    lsForEachItem(awaitVarList, gen2, var2) {
      if(var1 == var2)
        lsNewEnd(newList, var1, LS_NH);
    }
  }
  
  return newList;
}


/**Function********************************************************************

  Synopsis           [Computes the set of variables that are either read or
                      awaited by an atom.]

  Description        [The function returns a list of variables that are both
                      read and awaited by atom.]

  SideEffects        [None]

  SeeAlso            [Atm_AtomReadReadVarList, Atm_AtomReadAwaitVarList]

******************************************************************************/
lsList
Atm_AtomObtainReadOrAwaitVarList(
  Atm_Atom_t *atom)
{
  lsGeneric var1, var2;
  lsGen gen1, gen2;
  lsList readVarList = Atm_AtomReadReadVarList(atom);
  lsList awaitVarList = Atm_AtomReadAwaitVarList(atom);
  lsList newList = lsCopy(readVarList, 0);
  
    
  lsForEachItem(awaitVarList, gen1, var1) {
    boolean addVar = TRUE;
    lsForEachItem(newList, gen2, var2) {
      if(var1 == var2){
        addVar = FALSE;
        break;
      }
    }
    if(addVar)
      lsNewEnd(newList,var1, LS_NH);
  }
  return newList;
}

/**Function********************************************************************

  Synopsis           [Returns the atom type.]

  Description        [The function returns the atom type for the atom
                      (Atm_Event_c, Atm_Lazy_c, Atm_Normal_c).]

  SideEffects        [None]

******************************************************************************/
Atm_AtomType
Atm_AtomReadAtomType(
  Atm_Atom_t* atom)
{
  return atom->atmType;
}

/**Function********************************************************************

  Synopsis           [Set the atom type.]

  Description        [Given a pointer to an atom_atom_t, an integer indicating
                      the type of the atom (0 for unspecified, 1 for event atom,
		      and 2 for lazy atom), the functions sets the the
		      atomtype field.]

  SideEffects        [None]

******************************************************************************/
void
Atm_AtomSetAtomType(
  Atm_Atom_t* atom,
  Atm_AtomType t)
{
  atom->atmType = t;
}

/**Function********************************************************************

  Synopsis           [Sets the default init command of atom.]

  Description        [This function sets the default init command of atom.]

  SideEffects        [None]

  SeeAlso            [Atm_AtomSetDefaultUpdateCommand]

******************************************************************************/
void
Atm_AtomSetDefaultInitCommand(
  Atm_Atom_t *atom,
  Atm_Cmd_t *cmd)
{
  atom->defaultInitCmd = cmd;
}


/**Function********************************************************************

  Synopsis           [Sets the default update command of atom.]

  Description        [This function sets the default update command of atom.]

  SideEffects        [None]

  SeeAlso            [Atm_AtomSetDefaultInitCommand]

******************************************************************************/
void
Atm_AtomSetDefaultUpdateCommand(
  Atm_Atom_t *atom,
  Atm_Cmd_t *cmd)
{
  atom->defaultUpdateCmd = cmd;
}

/**Function********************************************************************

  Synopsis           [Returns the name of the atom.]

  Description        [Returns a copy of the name of the given atom. The user
                      should not free the returned string.]

  SideEffects        [None]

******************************************************************************/
char*
Atm_AtomReadName(
  Atm_Atom_t* atom)
{
  return atom->name;
}

/**Function********************************************************************

  Synopsis           [Generates a new atom name.]

  Description        [The function controls a global variable which is a counter
                      that counts the number of intermediate
                      module generated.  The counter is also used to generate a
		      name for the modules.  For example, if the counter
		      counts 14, the generated module name will be ATM14.
		      It should be used in conjuction with Mdl_NamePrefixWithString
		      to prefix the module name before the atm name.]

  SideEffects        [None]

******************************************************************************/
char*
Atm_AtomNewName()
{
  char* name = ALLOC(char, MAXNAMELEN);
  sprintf(name, "ATM%d", AtmCounter);
  AtmCounter ++;
  return name;
}

/**Function********************************************************************

  Synopsis           [Reset the global atom counter.]

  SideEffects        [None]

  SeeAlso            [Atm_AtomNewName]

******************************************************************************/
void
Atm_AtomCounterReset()
{
  AtmCounter = 0;
}

/**Function********************************************************************

  Synopsis           [Returns the guard expression of a guarded command.]

  Description        [This function returns the guard expression of a guarded
  command.]

  SideEffects        [None]

  SeeAlso            [Atm_CmdReadAssignList]

******************************************************************************/
Atm_Expr_t *
Atm_CmdReadGuard(
  Atm_Cmd_t *command)
{
  return command->guard;
}

/**Function********************************************************************

  Synopsis           [Returns the list of assignments of a guarded command.]

  Description        [The function returns the list of assignments of a guarded
  command.]

  SideEffects        [None]

  SeeAlso            [Atm_CmdReadGuard]

******************************************************************************/
lsList
Atm_CmdReadAssignList(
  Atm_Cmd_t *command)
{
  return command->assignList;
}
  
/**Function********************************************************************

  Synopsis           [Returns the type of an expression.]

  SideEffects        [None]

******************************************************************************/
Atm_ExprType
Atm_ExprReadType(
  Atm_Expr_t *expr) 
{
  return expr->type;
}


/**Function********************************************************************

  Synopsis           [Reads the left child of an expression.]

  SideEffects        [None]

  SeeAlso            [Atm_ExprReadRightChild]

******************************************************************************/
Atm_Expr_t *
Atm_ExprReadLeftChild(
  Atm_Expr_t *expr) 
{
  return expr->leftExpr;
}

/**Function********************************************************************

  Synopsis           [Reads the right child of an expression.]

  SideEffects        [None]

  SeeAlso            [Atm_ExprReadLeftChild]

******************************************************************************/
Atm_Expr_t *
Atm_ExprReadRightChild(
  Atm_Expr_t *expr) 
{
  return expr->rightExpr;
}

/**Function********************************************************************

  Synopsis           [Returns the type of an assignment.]

  SideEffects        [None]

  SeeAlso            [Atm_AssignReadVariable, Atm_AssignReadExpr]

******************************************************************************/
Atm_AssignType
Atm_AssignReadType(
  Atm_Assign_t *assign)
{
  return assign->type;
}

/**Function********************************************************************

  Synopsis           [Returns the variable on the left hand side of the
                      assignment.]

  SideEffects        [None]

  SeeAlso            [Atm_AssignReadType, Atm_AssignReadExpr]

******************************************************************************/
Var_Variable_t*
Atm_AssignReadVariable(
  Atm_Assign_t *assign)
{
  return assign->var;
}

/**Function********************************************************************

  Synopsis           [Returns the expr on the right hand side of assignment.]

  SideEffects        [None]

  SeeAlso            [Atm_AssignReadType, Atm_AssignReadVariable]

******************************************************************************/
Atm_Expr_t *
Atm_AssignReadExpr(
  Atm_Assign_t *assign)
{
  return assign->expr;
}

/**Function********************************************************************

  Synopsis           [Returns the address of expr on the right hand side of assignment.]

  SideEffects        [None]

  SeeAlso            [Atm_AssignReadType, Atm_AssignReadVariable]

******************************************************************************/
Atm_Expr_t **
Atm_AssignReadExprAddress(
  Atm_Assign_t *assign)
{
  return &(assign->expr);
}

/**Function********************************************************************

  Synopsis           [Returns the index variable of a forall assignment.]

  SideEffects        [None]

  SeeAlso            [Atm_AssignReadIndexExpr]

******************************************************************************/
Var_Variable_t*
Atm_AssignReadIndexVar(
  Atm_Assign_t *assign)
{
  return assign->index.indexVar;
}

/**Function********************************************************************

  Synopsis           [Returns the index expr of an index assignment.]

  SideEffects        [None]

  SeeAlso            [Atm_AssignReadIndexVar]

******************************************************************************/
Atm_Expr_t*
Atm_AssignReadIndexExpr(
  Atm_Assign_t *assign)
{
  return assign->index.indexExpr;
}


/**Function********************************************************************

  Synopsis           [Build an expression from two expression and an operator.]

  Description        [This routine returns a pointer to a new Atm_Expr_t tree,
                      which is built from the two child expressions,
		      leftchild and rightchild.  The expr_type is set to the
		      desired type.  It retuns NIL(Atm_Expr_t) if fails. It is 
		      the user's responsibility to free the tree after use.
		      Note that the following expr_type only has one child.]

  SideEffects        [None]

  SeeAlso            [Atm_ExprFreeExpression]

******************************************************************************/
Atm_Expr_t*
Atm_ExprAlloc(
  Atm_ExprType exprtype,
  Atm_Expr_t* leftchild,
  Atm_Expr_t* rightchild
)
{
  Atm_Expr_t* newexpr = ALLOC(Atm_Expr_t, 1);

  
  newexpr -> type = exprtype;
  newexpr -> leftExpr = leftchild;
  newexpr -> rightExpr = rightchild;

  return newexpr;
}



/**Function********************************************************************

  Synopsis           [Frees an expression recursively.]

  Description        [This function frees an expression recursively.]

  SideEffects        [None]

******************************************************************************/
void
Atm_ExprFreeExpression(
  Atm_Expr_t* expr)
{
  Atm_ExprType type;

  if (expr != NIL(Atm_Expr_t)) {
    type = expr->type;
    if ((type != Atm_PrimedVar_c) &&
        (type != Atm_BitwisePrimedVar_c) &&
	(type != Atm_UnPrimedVar_c) &&
        (type != Atm_BitwiseUnPrimedVar_c) &&
	(type != Atm_EnumConst_c) &&
        (type != Atm_BoolConst_c) &&
        (type != Atm_NumConst_c) &&
	(type != Atm_RangeConst_c) &&
        (type != Atm_BitwiseNumConst_c) &&
        (type != Atm_Nondet_c) &&
        (type != Atm_TimerUpperBound_c)) {
      
      /* free the children recursively. */
      Atm_ExprFreeExpression (expr->leftExpr);
      Atm_ExprFreeExpression (expr->rightExpr);
    }

    if (type == Atm_BitwiseNumConst_c)
      /* FREE((char *) expr->leftExpr); */
       FREE(expr->leftExpr); 
    
    FREE(expr);
  }
}

/**Function********************************************************************

  Synopsis           [Read the preorder of the given atom.]

  Description        [This function returns the preorder of an given atom.
                      The first atom in the preordering is 0.  By default,
		      every atom has a preorder value -1.]

  SideEffects        [None]

  SeeAlso            [Atm_AtomReadPostorder, Atm_AtomSetPreorder,
                      Atm_AtomSetPostorder]

******************************************************************************/
int
Atm_AtomReadPreorder(
  Atm_Atom_t* atom)
{
  if (atom == NIL(Atm_Atom_t)) return -1;
  
  return (atom->preorder);
}		     


/**Function********************************************************************

  Synopsis           [Read the postorder of the given atom.]

  Description        [This function returns the preorder of an given atom.
                      The first atom in the preordering is 0.  By default,
		      every atom has a preorder value -1.]

  SideEffects        [None]

  SeeAlso            [Atm_AtomReadPreorder, Atm_AtomSetPreorder, 
                      Atm_AtomSetPostorder]

******************************************************************************/
int
Atm_AtomReadPostorder(
  Atm_Atom_t* atom)
{
  if (atom == NIL(Atm_Atom_t)) return -1;
  
  return (atom->postorder);
}		     


/**Function********************************************************************

  Synopsis           [Set the preorder of the given atom.]

  Description        [This function sets the preorder of an given atom. ]

  SideEffects        [None]

  SeeAlso            [Atm_AtomReadPostorder, Atm_AtomReadPreorder,
                      Atm_AtomSetPostorder]

******************************************************************************/
void
Atm_AtomSetPreorder(
  Atm_Atom_t* atom,
  int preorder)
{
  if (atom != NIL(Atm_Atom_t))
    atom->preorder = preorder;
}		     


/**Function********************************************************************

  Synopsis           [Set the postorder of the given atom.]

  Description        [This function sets the preorder of an given atom. ]

  SideEffects        [None]

  SeeAlso            [Atm_AtomReadPostorder, Atm_AtomSetPreorder,
                      Atm_AtomReadPreorder]

******************************************************************************/
void
Atm_AtomSetPostorder(
  Atm_Atom_t* atom,
  int postorder)
{
  if (atom != NIL(Atm_Atom_t))
    atom->postorder = postorder;
}		     

/**Function********************************************************************

  Synopsis           [Changes the name of an atom.]

  Description        [This function changes the name of an atom. It is an
  error to call this function with a NULL atom name.]

  SideEffects        [The old name will be freed automatically.]

******************************************************************************/
void
Atm_AtomChangeName(
  Atm_Atom_t * atom,
  char * newname)
{

  FREE(atom->name);
  atom->name = util_strsav(newname);
  
}


/**Function********************************************************************

  Synopsis           [Duplicate an atom.]

  Description        [This function duplicates an atom, with a new atom name,
  and put it in the atom table of the newModule.  This function assumes that
  the newModule has been created, and that all the variables associated with
  the atom (ie. controlled, read and awaited variables) are already present in
  the newModule.  This function also updates the pointer from the variables to
  the atoms. It is an error to call this function with an NULL
  newAtmName, NULL atom or NULL newModule.]

  SideEffects        [None]

  SeeAlso            [Var_VariableDup, Mdl_ModuleDup, Mdl_ParComp]

******************************************************************************/
Atm_Atom_t *
Atm_AtomDup(
  Atm_Atom_t * atom,
  char * newAtmName,
  st_table *variableMappingTable)
{
  Atm_Atom_t* newatom;
  Atm_Cmd_t *cmd, *cmdDup;
  lsList newAssignList;
  
  newatom = Atm_AtomAlloc(newAtmName);

  Atm_AtomSetAtomType(newatom, Atm_AtomReadAtomType(atom));
  Atm_AtomSetPreorder(newatom, Atm_AtomReadPreorder(atom));
  Atm_AtomSetPostorder(newatom, Atm_AtomReadPostorder(atom));

  if ((cmd = Atm_AtomReadDefaultInitCommand(atom)) != NIL(Atm_Cmd_t)) {
    newAssignList = AtmAssignListDup(Atm_CmdReadAssignList(cmd),
                                     variableMappingTable);
    cmdDup = Atm_CmdAlloc(NIL(Atm_Expr_t), newAssignList);
    Atm_AtomSetDefaultInitCommand(newatom, cmdDup);
  }

  if ((cmd = Atm_AtomReadDefaultUpdateCommand(atom)) != NIL(Atm_Cmd_t)) {
    newAssignList = AtmAssignListDup(Atm_CmdReadAssignList(cmd),
                                     variableMappingTable);
    cmdDup = Atm_CmdAlloc(NIL(Atm_Expr_t), newAssignList);
    Atm_AtomSetDefaultUpdateCommand(newatom, cmdDup);
  }

  AtmControlVarsDup  (newatom, atom, variableMappingTable);
  AtmReadVarsDup (newatom, atom, variableMappingTable);
  AtmAwaitVarsDup(newatom, atom, variableMappingTable);
  AtmGuardedCommandsDup (newatom, atom, variableMappingTable);

  return newatom;

}

/**Function********************************************************************

  Synopsis           [Substitute a constant for the dummy variable everywhere
  in an expression.]

  Description        [This function is useful for manipulating forall
  assignments. It substitutes a constant c for the dummy variable var everywhere
  in the expression on the right hand side of the assignment. The function
  returns the new expression.]

  SideEffects        [None]

******************************************************************************/
Atm_Expr_t *
Atm_ExprSubstituteConstantForDummyVar(
  Atm_Expr_t *e,
  Var_Variable_t *var,
  int c,
  boolean isEnum)
{
  Atm_Expr_t *result, *lresult, *rresult;
  Atm_ExprType eType;
  Atm_Expr_t *lexpr;
  Atm_Expr_t *rexpr; 

  if (!e)
    return NIL(Atm_Expr_t);
  
  eType = Atm_ExprReadType(e);
  lexpr = Atm_ExprReadLeftChild(e);
  rexpr = Atm_ExprReadRightChild(e);
  
  switch (eType) {
      case Atm_UnPrimedVar_c : 
      {
        Var_Type_t *varType = Var_VariableReadType(var);
         
        if (var == (Var_Variable_t *) lexpr) {
          result =  Atm_ExprAlloc(Atm_RangeConst_c, (Atm_Expr_t *) (long) c,
                                  (Atm_Expr_t *) (long) Var_VarTypeReadDomainSize(varType)) ;
          return(result);
        } else {

          result = Atm_ExprAlloc(eType, lexpr, rexpr);
          return result;
        }
        break;        
      }

      case Atm_Nondet_c :
      case Atm_EventQuery_c :
      case Atm_PrimedVar_c :
      case Atm_BitwisePrimedVar_c :
      case Atm_BitwiseUnPrimedVar_c :
      case Atm_EnumConst_c :
      case Atm_BoolConst_c :
      case Atm_NumConst_c :
      case Atm_RangeConst_c :
      {
        result = Atm_ExprAlloc(eType, lexpr, rexpr);
        return result;
      }
      case Atm_BitwiseNumConst_c : 
      {
        int numBits = (int) (long) rexpr;
        char *bitArray = (char *) lexpr;
        char *dupBitArray = ALLOC(char, numBits);
        int i;
        
        for (i = 0; i < numBits; i++) 
          dupBitArray[i] = bitArray[i];

        result = Atm_ExprAlloc(eType, (Atm_Expr_t *) dupBitArray, (Atm_Expr_t *) (long) numBits);
        return result;
      }
      
      case Atm_IfThenElseFi_c :
      case Atm_BitwiseIfThenElseFi_c :
      {
        Atm_Expr_t *thenresult, *elseresult;

        lresult = Atm_ExprSubstituteConstantForDummyVar(lexpr, var, c, isEnum);
        thenresult = Atm_ExprSubstituteConstantForDummyVar(Atm_ExprReadLeftChild(rexpr), var, c, isEnum);
        elseresult = Atm_ExprSubstituteConstantForDummyVar(Atm_ExprReadRightChild(rexpr), var, c, isEnum);

        result = Atm_ExprAlloc(eType, lresult, Atm_ExprAlloc(eType,
                                                             thenresult,
                                                             elseresult));
        return result;
      }

      default :
      {
        lresult = Atm_ExprSubstituteConstantForDummyVar(lexpr, var, c, isEnum);
        rresult = Atm_ExprSubstituteConstantForDummyVar(rexpr, var, c, isEnum);
        result = Atm_ExprAlloc(eType, lresult, rresult);
        return result;
      }
  }
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Copies the ctl variables]

  Description        [Given two "identical" atoms, newatom and oldatom, 
	              as well as the variableMappingTable (from the
                      oldvariable to the new variable), this function identifies
		      the ctl variables in the old atom, and set the
		      CtrlVarList in the new atom to point to the same 
		      variables.]

  SideEffects        [None]


******************************************************************************/
void
AtmControlVarsDup(
  Atm_Atom_t* newatom,
  Atm_Atom_t* oldatom,
  st_table* variableMappingTable)
{
  lsList oldlist = Atm_AtomReadControlVarList(oldatom);
  lsList newlist = Atm_AtomReadControlVarList(newatom);
  lsGen  gen;
  Var_Variable_t* oldvar;
  Var_Variable_t* newvar;
  
  lsForEachItem(oldlist, gen, oldvar){
    st_lookup(variableMappingTable, (char *) oldvar, (char **) &newvar);
    Var_VariableSetControlAtom((Var_Variable_t*) newvar, (VarAtomGeneric) newatom);
    lsNewEnd(newlist, (lsGeneric) newvar, LS_NH);    
  }
}

/**Function********************************************************************

  Synopsis    [Copy the variables read by a given atom to the new atom.]

  Description [This function is called by CopyAtoms.  When a new atom is
               created, it has to update the list of variables that it
	       is reading.]

  SideEffects [None]

******************************************************************************/
void
AtmReadVarsDup(
  Atm_Atom_t* newatom,
  Atm_Atom_t* oldatom,
  st_table * variableMappingTable)
{
  lsList oldlist = Atm_AtomReadReadVarList(oldatom);
  lsList newlist = Atm_AtomReadReadVarList(newatom);
  Var_Variable_t* oldvar;
  Var_Variable_t* newvar;
  lsGen  gen;
  
  lsForEachItem(oldlist, gen, oldvar){
    st_lookup(variableMappingTable, (char *) oldvar, (char **) &newvar);
    lsNewEnd(newlist, (lsGeneric) newvar, LS_NH);
    Var_VariableAddToReadByAtomList(newvar, (VarAtomGeneric) newatom);
  }
}


/**Function********************************************************************

  Synopsis    [Copy the variables awaited by a given atom to the new atom.]

  Description [This function is called by CopyAtoms.  When a new atom is
               created, it has to update the list of variables that it
	       is reading.]

  SideEffects [None]

******************************************************************************/
  	

void
AtmAwaitVarsDup(
  Atm_Atom_t* newatom,
  Atm_Atom_t* oldatom,
  st_table * variableMappingTable)
{
  lsList oldlist = Atm_AtomReadAwaitVarList(oldatom);
  lsList newlist = Atm_AtomReadAwaitVarList(newatom);
  Var_Variable_t* oldvar;
  Var_Variable_t* newvar;
  lsGen  gen;
  
  lsForEachItem(oldlist, gen, oldvar){
    st_lookup(variableMappingTable, (char *) oldvar, (char **) &newvar);
    lsNewEnd(newlist, (lsGeneric) newvar, LS_NH);
    Var_VariableAddToAwaitByAtomList(newvar, (VarAtomGeneric) newatom);
  }
}

/**Function********************************************************************

  Synopsis           [Copy the guarded commands from one atom to another.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [AtmCopySelectedCommands]

******************************************************************************/
void
AtmGuardedCommandsDup(
  Atm_Atom_t* newatom,
  Atm_Atom_t* oldatom,
  st_table* variableMappingTable)
{
  AtmSelectedCommandsDup(newatom, oldatom, variableMappingTable, 0);
  if (Atm_AtomReadInitCommandList(oldatom)==Atm_AtomReadUpdateCommandList(oldatom))
    Atm_AtomAddUpdateCommandList(newatom, Atm_AtomReadInitCommandList(newatom));
  else
    AtmSelectedCommandsDup(newatom, oldatom, variableMappingTable, 1);
  AtmSelectedCommandsDup(newatom, oldatom, variableMappingTable, 2);  
}

/**Function********************************************************************

  Synopsis    [Copy the guarded command to the new atom.]

  Description [To save code, there is a parameter to select either the
               init command or the update commands to be copied.
	       0: init, 1: update.]

  SideEffects []

******************************************************************************/
void
AtmSelectedCommandsDup(
  Atm_Atom_t* newatom,
  Atm_Atom_t* oldatom,
  st_table* variableMappingTable,
  int which)
{
  lsList oldcmdlist;
  lsList newcmdlist;
  Atm_Cmd_t* oldcmd;
  Atm_Cmd_t* newcmd;
  lsGen  gen;
  Atm_Expr_t* newguard;
  lsList oldassignlist;
  lsList newassignlist;
  
  if (which == 0){ /* init command to be copied */
    oldcmdlist = Atm_AtomReadInitCommandList(oldatom);
    newcmdlist = Atm_AtomReadInitCommandList(newatom);
  }
  else if (which == 1) { /* update command to be copied */
    oldcmdlist = Atm_AtomReadUpdateCommandList(oldatom);
    newcmdlist = Atm_AtomReadUpdateCommandList(newatom);
  }
  else if (which == 2) { /* wait commands to be copied */
    oldcmdlist = Atm_AtomReadWaitCommandList(oldatom);
    newcmdlist = Atm_AtomReadWaitCommandList(newatom);
  }


  lsForEachItem(oldcmdlist, gen, oldcmd){
    newguard = AtmExprDup(Atm_CmdReadGuard(oldcmd), variableMappingTable);
    oldassignlist = Atm_CmdReadAssignList(oldcmd);
    newassignlist = AtmAssignListDup(oldassignlist, variableMappingTable);
    newcmd = Atm_CmdAlloc(newguard, newassignlist);
    lsNewEnd(newcmdlist, (lsGeneric) newcmd, LS_NH);
    
  }
}

/**Function********************************************************************

  Synopsis           [Returns a copy of the assignment list.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
lsList
AtmAssignListDup(
  lsList oldassignlist,
  st_table *variableMappingTable)
{
  lsList newassignlist = lsCreate();
  Atm_Expr_t *expr, *indexExpr;
  lsGen gen;
  Atm_Assign_t* oldassign;
  Atm_Assign_t* newassign;
  Atm_AssignType assignType;
  Var_Variable_t* oldvar;
  Var_Variable_t* newvar;

  lsForEachItem(oldassignlist, gen, oldassign){
    assignType = Atm_AssignReadType(oldassign);
    oldvar = Atm_AssignReadVariable(oldassign);
    st_lookup(variableMappingTable, (char *) oldvar, (char**) &newvar);
    
    switch (assignType) {
        case Atm_AssignStd_c : 
        {
          expr = Atm_AssignReadExpr(oldassign);
          newassign =
              Atm_AssignStdAlloc(newvar,
                                 AtmExprDup(expr,variableMappingTable));
          
          break;
        }
        
        case Atm_AssignEvent_c : 
        {
          newassign = Atm_AssignEventAlloc(newvar);
          break;
        }
        
        case Atm_AssignIndex_c: 
        {
          expr = Atm_AssignReadExpr(oldassign);
          indexExpr = Atm_AssignReadIndexExpr(oldassign);
          newassign =
              Atm_AssignIndexAlloc(newvar,
                                   AtmExprDup(indexExpr, variableMappingTable),
                                   AtmExprDup(expr, variableMappingTable));
          
          break;
        }
        
        case Atm_AssignForall_c: 
        {
          Var_Variable_t *oldIndexVar, *newIndexVar;
          
          expr = Atm_AssignReadExpr(oldassign);
          oldIndexVar = Atm_AssignReadIndexVar(oldassign);
          st_lookup(variableMappingTable, (char *) oldIndexVar, (char**) &newIndexVar);
          newassign = Atm_AssignForallAlloc(newvar, newIndexVar,
                                            AtmExprDup(expr,
                                                           variableMappingTable));
          break;
        }
    }
    
    lsNewEnd(newassignlist, (lsGeneric) newassign, LS_NH);
  }
  
  return newassignlist;
}


/**Function********************************************************************

  Synopsis           [Returns an exact duplicate of the expression tree.]

  Description        [This function returns a pointer to a duplicated
                      expression tree, given an old tree and a valid
		      variable table associated with the new expression 
		      tree.  Valid means the variable table should have
		      already contained all the variables appearing in the
		      expression tree.  This function will return 
		      NIL(Atm_Expr_t) if unsuccessful.]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
Atm_Expr_t*
AtmExprDup(
  Atm_Expr_t* expr,
  st_table* variableMappingTable)
{
  Atm_Expr_t* newexpr;
  Atm_ExprType type;
  Atm_Expr_t* left;
  Atm_Expr_t* right;
  Var_Variable_t* var;
  Var_Variable_t* newvar;
  char* newname;


  if (expr == NIL(Atm_Expr_t)) return NIL(Atm_Expr_t);
  
  type = Atm_ExprReadType(expr);

  switch(type) {
      case Atm_BitwisePrimedVar_c:
      case Atm_BitwiseUnPrimedVar_c: 
      case Atm_PrimedVar_c:
      case Atm_UnPrimedVar_c: 
      {
        var = (Var_Variable_t*)  Atm_ExprReadLeftChild(expr);
        st_lookup(variableMappingTable, (char *) var, (char**) &newvar);
        left = (Atm_Expr_t*) newvar;
        right = NIL(Atm_Expr_t);    
        break;
      }
      
      case Atm_BitwiseNumConst_c: 
      {
        int numBits = (int) (long) Atm_ExprReadRightChild(expr);
        char *bitArray = (char *) Atm_ExprReadLeftChild(expr);
        char *dupBitArray = ALLOC(char, numBits);
        int i;
        
        for (i = 0; i < numBits; i++) 
          dupBitArray[i] = bitArray[i];

        left = (Atm_Expr_t *) dupBitArray;
        right = (Atm_Expr_t *) (long) numBits;
        break;
      }

      case Atm_TimerUpperBound_c:
      case Atm_EnumConst_c:
      case Atm_BoolConst_c:
      case Atm_NumConst_c: 
      {
        left = Atm_ExprReadLeftChild(expr);
        right = NIL(Atm_Expr_t);    
        break;
      }

	case Atm_RangeConst_c : {
	left = Atm_ExprReadLeftChild(expr);
	right = Atm_ExprReadRightChild(expr);
	break;
	}

      case Atm_Nondet_c:
      {
        left = Atm_ExprReadLeftChild(expr);
        right = NIL(Atm_Expr_t);
        break;
      }
      
      default: 
      {
        left  = AtmExprDup(
          Atm_ExprReadLeftChild(expr), variableMappingTable);
        right = AtmExprDup(
          Atm_ExprReadRightChild(expr), variableMappingTable);    
        break;
      }
  }
  
  newexpr=
    Atm_ExprAlloc(type, left, right);

  return newexpr;
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
