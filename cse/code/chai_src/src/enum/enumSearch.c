/**CFile***********************************************************************

  FileName    [enumSearch.c]

  PackageName [enum]

  Synopsis    [This file contains routines for doing state space search.]

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

******************************************************************************/

#include  "enumInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/
#define LOAD_FACTOR 2

#if SIZEOF_LONG == 8
#define MULT_FACTOR 0x9e3779b97f4a8000l
#elif SIZEOF_LONG == 4
#define MULT_FACTOR 0x9e3779b9l
#endif

#define BYTE_SIZE 8
#define WORD_SIZE BYTE_SIZE * sizeof(unsigned long)
#define DEFAULT_SIZE 9
/* 2^DEFAULT_SIZE is the default initial size of the hash table. */

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
EXTERN int MainTkEnabled;


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

  Synopsis           [Allocates a state table structure.]

  Description        [The function allocates and initializes a state table
                      data structure with 2^power number of buckets. The
                      number of buckets in the state table is always a power
                      of 2.]  

  SideEffects        [None]

  SeeAlso            [Enum_StateTableFree]

******************************************************************************/
Enum_StateTable_t *
Enum_StateTableAlloc(
  int power,
  Enum_VarInfo_t *varInfo)
{
  unsigned long i;
  Enum_StateTable_t *stateTable = ALLOC(Enum_StateTable_t, 1);

  assert(power >= 0);

  stateTable->power = power;
  stateTable->numBuckets = 1;
  stateTable->numBuckets <<= power;

  stateTable->table = ALLOC(Enum_StateTableEntry_t *, stateTable->numBuckets);

  for(i = 0; i < stateTable->numBuckets; i++) {
    stateTable->table[i] = NIL(Enum_StateTableEntry_t);
  }
  stateTable->size = 0;
  stateTable->numStoredVars = varInfo->numHistDependVars;

  return stateTable;
}

/**Function********************************************************************

  Synopsis           [Frees a state table.]

  Description        [This function frees a state table and all the states in
                      it.]

  SideEffects        [None]

  SeeAlso            [Enum_StateTableAlloc]

******************************************************************************/
void
Enum_StateTableFree(
  Enum_StateTable_t *stateTable)
{
  int i;
  Enum_StateTableEntry_t *entry, *temp;
  
  for(i = 0; i < stateTable->numBuckets; i++) {
    for(entry = stateTable->table[i]; entry != NIL(Enum_StateTableEntry_t); ) {
      Enum_StateFree(entry->state);
      temp = entry->next;
      Enum_StateTableEntryFree(entry);
      entry = temp;
    }
  }
  
  FREE(stateTable->table);
  FREE(stateTable);
}

/**Function********************************************************************

  Synopsis           [Allocates a state table entry data structure.]

  SeeAlso            [Enum_StateTableEntryFree]

  SideEffects        [None]

******************************************************************************/
Enum_StateTableEntry_t *
Enum_StateTableEntryAlloc(
  Enum_State_t *state,
  Enum_StateTableEntry_t *next)
{
   Enum_StateTableEntry_t *entry = ALLOC(Enum_StateTableEntry_t, 1);
   entry->state = state;
   entry->next = next;
   return entry;
}

/**Function********************************************************************

  Synopsis           [Frees a state table entry.]

  Description        [This function frees a state table entry. Note that it
                      does not free the state in the entry.]

  SideEffects        [None]

  SeeAlso            [Enum_StateTableEntryAlloc]

******************************************************************************/
void
Enum_StateTableEntryFree(
  Enum_StateTableEntry_t *entry)
{
  FREE(entry);
}

/**Function********************************************************************

  Synopsis           [Sets the parentState field of state.]

  Description        [The function sets the parentState field of state. This
                      field is used in producing a counterexample in case an
                      invariant check fails.] 
  
  SideEffects        [None]

******************************************************************************/
void
Enum_StateSetParentState(
  Enum_State_t *state,
  Enum_State_t *parentState)
{
  state->parentState = parentState;
}

/**Function********************************************************************

  Synopsis           [Performs a breadth-first search on the state space of
                      module.]

  Description [Currently, there could be a bug in this routine if transition
  invariants are being checked. No next states are generated if the histrory
  dependent part of the state has been encountered earlier in some other state
  and hence no transition invariant is checked also. For state invariants,
  this is fine but for transition invariants, satisfaction of an invariant by
  the transition <ps, ns> might depend on the history-free variables of
  ps. This needs to be worked out.]

  SideEffects        [If checkInvariant is TRUE, then the function changes
                      the status of the unchecked invariants to either passed
                      or failed.]

  SeeAlso            [Enum_ModulePerformDFS]

******************************************************************************/
void
Enum_ModulePerformBFS(
  Tcl_Interp *interp,
  Main_Manager_t *mainManager,
  Mdl_Module_t *module,
  array_t *invNameArray,
  array_t *typedExprArray)
{
  Enum_StateTableEntry_t *queueHead, *queueTail;
  Enum_StateTableEntry_t *temp, *entry, **prevEntry;
  Enum_StateManager_t *stateManager = Main_ManagerReadStateManager(mainManager);
  Enum_VarInfo_t *varInfo =
    Enum_StateManagerObtainVarInfoFromModule(stateManager, module);
  lsList initialStateList = Enum_ModuleComputeSetOfInitialStates(module, varInfo);
  lsList postStateList;
  lsGen gen;
  lsGeneric state;
  Enum_StateTable_t *stateTable = Enum_StateTableAlloc(DEFAULT_SIZE, varInfo);
  Atm_Expr_t *expr;
  int numStateInvRemain = 0;
  char *invName;
  boolean checkInvariant = (typedExprArray != NIL(array_t));
  boolean *statusArray;
  int i;
  
  
  if(checkInvariant) {

     assert(array_n(invNameArray) == array_n(typedExprArray));

     numStateInvRemain = array_n(typedExprArray);
     statusArray = ALLOC(boolean, array_n(typedExprArray));
     for (i = 0; i < array_n(typedExprArray); i++)
	statusArray[i] = TRUE;
  }

  /* To save space, I am freeing all memory for the history free and event */
  /* variables in any state put in the hash table. */
    
  prevEntry = &queueHead;
  
  lsForEachItem(initialStateList, gen, state) {
    entry = Enum_StateTableEntryAlloc((Enum_State_t *) state, NIL(Enum_StateTableEntry_t));
    *prevEntry = entry;
    prevEntry = &(entry->next);
  }

  lsDestroy(initialStateList, NULL);
  
  queueTail = entry;

  while(queueHead != NIL(Enum_StateTableEntry_t)) {

    /* First, check if there are any more invariants that have not been
       checked. If there are no more to be checked then return. */
    
    if(checkInvariant && (numStateInvRemain == 0)) {
      while(queueHead != NIL(Enum_StateTableEntry_t)) {
        temp = queueHead->next;
        Enum_StateTableEntryFree(queueHead);
        queueHead = temp;
      }

      FREE(statusArray);
      EnumStateTablePrint(stateTable, varInfo);
      Enum_StateTableFree(stateTable);
      return;
    }

    /* Take the element at the front of the queue. Check if it violates any
       state invariant. */

    if(checkInvariant && (numStateInvRemain > 0)) {
      arrayForEachItem(Atm_Expr_t *, typedExprArray, i, expr) {
	 if (statusArray[i]) {
            if(!EnumExprIsTrue(expr, varInfo, queueHead->state, NIL(Enum_State_t))) {
               statusArray[i] = FALSE;
               numStateInvRemain--;
	       invName = array_fetch(char *, invNameArray, i);
               Main_MochaPrint("State invariant %s failed\n", invName);
               EnumCounterExamplePrint(interp, stateManager, module,
                                       invName, queueHead->state);
            }
         }
      }
    }

    /* Compute the set of post states of the first element in the queue. For
       each post state, check if it is in the hash table. If it is 
       not, put it in and add it to the rear of the queue. */

    if (EnumStateAddToTable(stateTable, queueHead->state, varInfo)) {

      postStateList = Enum_ModuleComputeSetOfPostStates(module, varInfo, queueHead->state);
      
      lsForEachItem(postStateList, gen, state) {
        Enum_StateSetParentState((Enum_State_t *) state, queueHead->state);
        
        entry = Enum_StateTableEntryAlloc((Enum_State_t *) state, NIL(Enum_StateTableEntry_t));
        queueTail->next = entry;
        queueTail = queueTail->next;
      }

      lsDestroy(postStateList, NULL);
      Enum_StateFreeHistFreeVars(queueHead->state);
      Enum_StateFreeEventVars(queueHead->state);
    }
    else {
      Enum_StateFree(queueHead->state);
    }
    
    temp = queueHead->next;
    Enum_StateTableEntryFree(queueHead);
    queueHead = temp;
  }

  if(checkInvariant) {
     arrayForEachItem(char *, invNameArray, i, invName) {
	if (statusArray[i]) 
	   Main_MochaPrint("State invariant %s passed\n", invName);
     }
     
     FREE(statusArray);
  }

  EnumStateTablePrint(stateTable, varInfo);
  Enum_StateTableFree(stateTable);
}

/**Function********************************************************************

  Synopsis           [Performs a depth-first search on the state space of
                      module.]

  Description        [optional]

  SideEffects        [If checkInvariant is TRUE, then the function changes
                      the status of the unchecked invariants to either passed
                      or failed.]

  SeeAlso            [Enum_ModulePerformBFS]

******************************************************************************/
void
Enum_ModulePerformDFS(
  Tcl_Interp *interp,
  Main_Manager_t *mainManager,
  Mdl_Module_t *module,
  array_t *invNameArray,
  array_t *typedExprArray)
{
  Enum_StateTableEntry_t *queue, *temp, *entry, *nextEntry;
  Enum_StateManager_t *stateManager = Main_ManagerReadStateManager(mainManager);
  Enum_VarInfo_t *varInfo =
     Enum_StateManagerObtainVarInfoFromModule(stateManager, module);
  lsList initialStateList = Enum_ModuleComputeSetOfInitialStates(module, varInfo);
  lsList postStateList;
  lsGen gen;
  lsGeneric state;
  Enum_StateTable_t *stateTable = Enum_StateTableAlloc(DEFAULT_SIZE, varInfo);
  Atm_Expr_t *expr;
  int numStateInvRemain = 0;
  char *invName;
  boolean checkInvariant = (typedExprArray != NIL(array_t));
  boolean *statusArray;
  int i;

  if(checkInvariant) {

     assert(array_n(typedExprArray) == array_n(invNameArray));

     numStateInvRemain = array_n(typedExprArray);
     statusArray = ALLOC(boolean, array_n(typedExprArray));
     for (i = 0; i < array_n(typedExprArray); i++)
	statusArray[i] = TRUE;
  }

  /* To save space, I am freeing all memory for the history free and event */
  /* variables in any state put in the hash table. */
  
  nextEntry = NIL(Enum_StateTableEntry_t);
  
  lsForEachItem(initialStateList, gen, state) {
      entry = Enum_StateTableEntryAlloc((Enum_State_t *) state, nextEntry);
      nextEntry = entry;
  }

  lsDestroy(initialStateList, NULL);

  queue = entry;

  while (queue != NIL(Enum_StateTableEntry_t)) {
    
    /* First, check if there are any more invariants that have not been
       checked. If there are no more to be checked, then return. */
    
    if (checkInvariant && (numStateInvRemain == 0)) {
      while (queue != NIL(Enum_StateTableEntry_t)) {
        temp = queue->next;
        Enum_StateTableEntryFree(queue);
        queue = temp;
      }

      FREE(statusArray);
      EnumStateTablePrint(stateTable, varInfo);
      Enum_StateTableFree(stateTable);
      return;
    }
    
    /* Take the element at the front of the queue. Check if it violates any
       state invariant. */
    
    if (checkInvariant && (numStateInvRemain > 0)) {
      arrayForEachItem(Atm_Expr_t *, typedExprArray, i, expr) {
         if (statusArray[i]) {
            if(!EnumExprIsTrue(expr, varInfo, queue->state, NIL(Enum_State_t))) {
               statusArray[i] = FALSE;
               numStateInvRemain--;
	       invName = array_fetch(char *, invNameArray, i);
               Main_MochaPrint("State invariant %s failed\n", invName);
               EnumCounterExamplePrint(interp, stateManager, module,
                                       invName, queue->state);
            }
         }
      }
    }
    

    /* Compute the set of post states of the first element in the queue. For
       each post state, check if it is in the hash table. If it is 
       not, put it in and add it to the rear of the queue. */

    nextEntry = queue->next;

    if (EnumStateAddToTable(stateTable, queue->state, varInfo)) {

      postStateList = Enum_ModuleComputeSetOfPostStates(module, varInfo, queue->state);
    
      lsForEachItem(postStateList, gen, state) {
        Enum_StateSetParentState((Enum_State_t *) state, queue->state);
              
        entry = Enum_StateTableEntryAlloc((Enum_State_t *) state, nextEntry);
        nextEntry = entry;
      }

      lsDestroy(postStateList, NULL);
      Enum_StateFreeHistFreeVars(queue->state);
      Enum_StateFreeEventVars(queue->state);
    }
    else {
      Enum_StateFree(queue->state);
    }
    
    Enum_StateTableEntryFree(queue);
    queue = nextEntry;
  }
  

  if(checkInvariant) {
     arrayForEachItem(char *, invNameArray, i, invName) {
	if (statusArray[i]) 
	   Main_MochaPrint("State invariant %s passed\n", invName);
     }

     FREE(statusArray);
  }
  
  EnumStateTablePrint(stateTable, varInfo);
  Enum_StateTableFree(stateTable);
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Prints a counterexample to an invariant.]

  Description        [This function prints the sequence of states, starting
                      from an initial state to badState, that led to the
                      violation of some invariant.]

  SideEffects        [None]

******************************************************************************/
void
EnumCounterExamplePrint(
  Tcl_Interp *interp,
  Enum_StateManager_t *stateManager,
  Mdl_Module_t *module,
  char *invName,
  Enum_State_t *badState)
{
  lsList counterExample = lsCreate();
  Enum_State_t *state, *fullState;
  lsGen gen, initGen, postGen;
  Enum_State_t *initState, *postState;
  lsList initialSet, postSet;
  Tcl_HashTable *stateNameTable =
        Enum_StateManagerReadStateNameTable(stateManager);
  char *cmd;
  char *moduleName = Mdl_ModuleReadName(module);
  Enum_VarInfo_t *varInfo =
    Enum_StateManagerObtainVarInfoFromModule(stateManager, module);
  lsList fullCounterExample;
  
  state = badState;
  while(state->parentState != NIL(Enum_State_t)) {
    lsNewBegin(counterExample, (lsGeneric) state, LS_NH);
    state = state->parentState;
  }

  /* At this point, state is pointing to an initial state. Construct
     the set of initial states and find one that matches it. Do a post on that
     state and find one that matches the first state in the list, and so on. */
  
  initialSet = Enum_ModuleComputeSetOfInitialStates(module, varInfo);

  lsForEachItem(initialSet, initGen, initState) {
    if(StateCompare(state, initState, varInfo)) {
      fullState = Enum_StateDup(initState, varInfo);
      break;
    }
  }
  lsFinish(initGen);
  
  lsForEachItem(initialSet, initGen, initState) {
    Enum_StateFree(initState);
  }

  lsDestroy(initialSet, NULL);

  fullCounterExample = lsCreate();
  lsNewEnd(fullCounterExample, (lsGeneric) fullState, LS_NH);
  

  lsForEachItem(counterExample, gen, state) {
    postSet = Enum_ModuleComputeSetOfPostStates(module, varInfo, fullState);
    lsForEachItem(postSet, postGen, postState) {
      if(StateCompare(state, postState, varInfo)) {
        fullState = Enum_StateDup(postState, varInfo);
        lsNewEnd(fullCounterExample, (lsGeneric) fullState, LS_NH);
        break;
      }
    }

    lsFinish(postGen);
    
    lsForEachItem(postSet, postGen, postState) {
      Enum_StateFree(postState);
    }
  }

  lsDestroy(counterExample, NULL);
  
  if (!MainTkEnabled) {
    fprintf(stdout, "Counterexample for invariant %s\n", invName);
    lsForEachItem(fullCounterExample, gen, state) {
      EnumStatePrint(state, varInfo);
    }
  }
  else {
    char *cmd = ALLOC(char, 150);     /* Allocate a reasonably big string to
                                         create Tcl commands. */

    sprintf(cmd, "set EnumErrorTrace [trc_errorTraceInit %s \"Error Trace\"]",
            moduleName);
    Tcl_Eval(interp, cmd);
    
    sprintf(cmd, "trc_searchMethodSet $EnumErrorTrace Enumerative");
    Tcl_Eval(interp, cmd);
    
    sprintf(cmd, "trc_propertyFailedSet $EnumErrorTrace %s", invName);
    Tcl_Eval(interp, cmd);


    lsForEachItem(fullCounterExample, gen, state) {

      char *stateString = ALLOC(char, 1);
      int i, n;
      Var_Variable_t **varArray;
      
      *stateString = '\0';

      /* First, print the history dependent variables. */
      n = Enum_VarInfoReadNumHistDependVars(varInfo);
      varArray = Enum_VarInfoReadHistDependVars(varInfo);
      for(i = 0; i < n; i++) {
        char *name = Var_VariableReadName(varArray[i]);
        long value = state->histDependVars[i];
        Var_DataType dataType = Var_VariableReadDataType(varArray[i]);
        char *valueString, *tempString;

        if(dataType == Var_Enumerated_c)
          valueString = util_strcat3(name, "=",
                                     Var_EnumElementReadName(Var_VariableObtainEnumElementFromIndex(varArray[i], value)));   
        
        else
          if(dataType == Var_Boolean_c) {
            if(value == 0)
              valueString = util_strcat3(name, "=", "0");
            if(value == 1)
              valueString = util_strcat3(name, "=", "1");
          }
          else {
            unsigned long modValue, len;
            char *s;
            
            if (value == 0)
              len = 1;
            else {
              if (value < 0) {
                len = 1;
                modValue = -value;
              }
              else {
                len = 0;
                modValue = value;
              }
              len += floor(log10(modValue)) + 1;
            }
            
            s = ALLOC(char, len+1);
            sprintf(s, "%d", value);
            valueString = util_strcat3(name, "=", s);
            FREE(s);
          }
        tempString = util_strcat3(stateString, valueString, " ");
        FREE(stateString);
        FREE(valueString);
        stateString = tempString;
      }

      /* Second, print history free variables. */
      n = Enum_VarInfoReadNumHistFreeVars(varInfo);
      varArray = Enum_VarInfoReadHistFreeVars(varInfo);
      for(i = 0; i < n; i++) {
        char *name = Var_VariableReadName(varArray[i]);
        long value = state->histDependVars[i];
        Var_DataType dataType = Var_VariableReadDataType(varArray[i]);
        char *valueString, *tempString;
        
        if(dataType == Var_Enumerated_c)
          valueString = util_strcat3(name, "=",
                                     Var_EnumElementReadName(Var_VariableObtainEnumElementFromIndex(varArray[i], value)));   
        
        else
          if(dataType == Var_Boolean_c) {
            if(value == 0)
              valueString = util_strcat3(name, "=", "0");
            if(value == 1)
              valueString = util_strcat3(name, "=", "1");
          }
          else {
            unsigned long modValue, len;
            char *s;
            
            if (value == 0)
              len = 1;
            else {
              if (value < 0) {
                len = 1;
                modValue = -value;
              }
              else {
                len = 0;
                modValue = value;
              }
              len += floor(log10(modValue)) + 1;
            }
            
            s = ALLOC(char, len+1);
            sprintf(s, "%d", value);
            valueString = util_strcat3(name, "=", s);
            FREE(s);
          }
        tempString = util_strcat3(stateString, valueString, " ");
        FREE(stateString);
        FREE(valueString);
        stateString = tempString;
      }

      /* Third, print event variables. The convention is that only those event */
      /* variable names are present that actually happened. */
      n = Enum_VarInfoReadNumEventVars(varInfo);
      varArray = Enum_VarInfoReadEventVars(varInfo);
      for(i = 0; i < n; i++) {
        char *name = Var_VariableReadName(varArray[i]);
        long value = state->eventVars[i];
        char *tempString;
        
        if(value == 1) {
          tempString = util_strcat3(stateString, name, " ");
          FREE(stateString);
          stateString = tempString;
        }
      }

      printf("%s\n", stateString);
      sprintf(cmd, "trc_errorTraceAppend $EnumErrorTrace {%s}", stateString);
      Tcl_Eval(interp, cmd);
      FREE(stateString);
    }
    
    FREE(cmd);
  }

  lsForEachItem(fullCounterExample, gen, state) {
     Enum_StateFree(state);
  }
  lsDestroy(fullCounterExample, NULL);
}
    

/**Function********************************************************************

  Synopsis           [Adds a state to the state table.]

  Description        [The function adds a state to the state table if it is
                      not present in the state table already. In this case, it
                      returns TRUE, otherwise it returns FALSE. The function
                      first computes index of the bucket where the state has
                      to be put and then puts the state in the front of the
                      linked list in the bucket.]
                      
  SideEffects        [size field of state table is incremented by 1.]

******************************************************************************/
boolean
EnumStateAddToTable(
  Enum_StateTable_t *stateTable,
  Enum_State_t *state,
  Enum_VarInfo_t *varInfo)
{
  
  unsigned long index;
  Enum_StateTableEntry_t *entry, *newEntry;

  if(stateTable->size == stateTable->numBuckets*LOAD_FACTOR) {
    StateTableResize(stateTable);
  }
  index = StateHashFunction(stateTable, state);
  
  for(entry = stateTable->table[index]; entry != NIL(Enum_StateTableEntry_t); entry = entry->next) {
    if(StateCompare(state, entry->state, varInfo))
      return FALSE;
  }
  ++(stateTable->size);
  newEntry = Enum_StateTableEntryAlloc(state, stateTable->table[index]);
  stateTable->table[index] = newEntry;
  return TRUE;
}

/**Function********************************************************************

  Synopsis           [Prints all the states in the hash table.]

  SideEffects        [None]

******************************************************************************/
void
EnumStateTablePrint(
  Enum_StateTable_t *stateTable,
  Enum_VarInfo_t *varInfo)
{
  int i;
  Enum_StateTableEntry_t *entry;
  int count = 0;
  
  for(i = 0; i < stateTable->numBuckets; i++) {
    
    for(entry = stateTable->table[i]; entry != NIL(Enum_StateTableEntry_t);
        entry = entry->next) {
      count++;
      /*
      EnumStatePrint(entry->state, varInfo);
      */
    }
  }
  Main_MochaPrint("No. of explored states = %d\n", count);
  
}

/**Function********************************************************************

  Synopsis           [Prints a state.]

  SideEffects        [None]

******************************************************************************/
void
EnumStatePrint(
  Enum_State_t *state,
  Enum_VarInfo_t *varInfo)
{
  int n, i;
  Var_Variable_t **varArray;
  
  /* First, print the history dependent variables. */
  n = Enum_VarInfoReadNumHistDependVars(varInfo);
  varArray = Enum_VarInfoReadHistDependVars(varInfo);
  for(i = 0; i < n; i++) {
    char *name = Var_VariableReadName(varArray[i]);
    long value = state->histDependVars[i];
    Var_DataType dataType = Var_VariableReadDataType(varArray[i]);
    char *valueString;
    
    if(dataType == Var_Enumerated_c)
      Main_MochaPrint("%s=%s ", name,
                       Var_EnumElementReadName(Var_VariableObtainEnumElementFromIndex(varArray[i], value)));    
    else
      if(dataType == Var_Boolean_c) {
        if((long) value == 0)
          Main_MochaPrint("%s=%s ", name, "0");
        if((long) value == 1)
          Main_MochaPrint("%s=%s ", name, "1");
      }
      else {
        Main_MochaPrint("%s=%ld ", name, (long) value);
      }
  }

  /* Second, print history free variables. */
  
  n = Enum_VarInfoReadNumHistFreeVars(varInfo);
  varArray = Enum_VarInfoReadHistFreeVars(varInfo);
  for(i = 0; i < n; i++) {
    char *name = Var_VariableReadName(varArray[i]);
    long value = state->histFreeVars[i];
    Var_DataType dataType = Var_VariableReadDataType(varArray[i]);
    char *valueString;
    
    if(dataType == Var_Enumerated_c)
      Main_MochaPrint("%s=%s ", name,
                       Var_EnumElementReadName(Var_VariableObtainEnumElementFromIndex(varArray[i], value)));    
    else
      if(dataType == Var_Boolean_c) {
        if((long) value == 0)
          Main_MochaPrint("%s=%s ", name, "0");
        if((long) value == 1)
          Main_MochaPrint("%s=%s ", name, "1");
      }
      else {
        Main_MochaPrint("%s=%ld " , name, (long) value);
      }
  }
  
  /* Third, print event variables. The convention is that the names of only
     those events that actually happened, are printed. */
  
  n = Enum_VarInfoReadNumEventVars(varInfo);
  varArray = Enum_VarInfoReadEventVars(varInfo);
  for(i = 0; i < n; i++) {
    char *name = Var_VariableReadName(varArray[i]);
    long value = state->eventVars[i];

    if(value == 1)
      Main_MochaPrint("%s ", name); 
  }
  Main_MochaPrint("\n");
  
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis           [Compares if two states are the same.]

  Description        [The function returns TRUE if the history dependent part
                      of state1 and state2 is the same, otherwise it returns
                      FALSE.]

  SideEffects        [None]

******************************************************************************/
boolean
StateCompare(
  Enum_State_t *state1,
  Enum_State_t *state2,
  Enum_VarInfo_t *varInfo)
{
  int n = varInfo->numHistDependVars;
  int i;

  for(i = 0; i < n; i ++) 
    if(state1->histDependVars[i] != state2->histDependVars[i])
      return FALSE;

  return TRUE;
}

/**Function********************************************************************

  Synopsis           [Resizes a state table to twice its size.]

  Description        [The function resizes a state table to twice the current
                      number of buckets. The number of buckets in the state
                      table is always a power of 2. The function rehashes the
                      states in stateTable into the new table.]

  SideEffects        [None]

  SeeAlso            [Enum_StateAddToTable]

******************************************************************************/
void
StateTableResize(
  Enum_StateTable_t *stateTable)
{
  unsigned long oldNumBuckets = stateTable->numBuckets;
  Enum_StateTableEntry_t **oldTable = stateTable->table;
  unsigned long i;
  
  stateTable->numBuckets <<= 1;
  stateTable->power += 1;
  
  stateTable->table = ALLOC(Enum_StateTableEntry_t *, stateTable->numBuckets);
  for(i = 0; i < stateTable->numBuckets; i++)
    stateTable->table[i] = NIL(Enum_StateTableEntry_t);

  /* Hash each state in the old hash table into the new table. To avoid memory */
  /* allocations and frees, I have written the code below so that */
  /* Enum_StateTableEntry_t elements are reused. Notice that the order of the */
  /* elements in the collision chain is reversed. Does this have an effect on */
  /* the performance? */
  
  for(i = 0; i < oldNumBuckets; i++) {
    Enum_StateTableEntry_t *entry, *temp;
    unsigned long index;
    
    entry = oldTable[i];
    while(entry != NIL(Enum_StateTableEntry_t)) {
      index = StateHashFunction(stateTable, entry->state);
      temp = entry;
      entry = entry->next;
      temp->next = stateTable->table[index];
      stateTable->table[index] = temp;
    } 
  }
  
  FREE(oldTable);
}

/**Function********************************************************************

  Synopsis           [Hashes a state into the state table.]

  Description        [The function first converts the state into a natural
                      number. This natural number is then converted into an
                      index into the state hash table by using the
                      multiplication method given on page 228 of the book
                      "Introduction to Algorithms" by Cormen, Leiserson and
                      Rivest. It returns the index into the hash table.]

  SideEffects        [None]

  SeeAlso            [StateCompactToNumber]

******************************************************************************/
unsigned long
StateHashFunction(
  Enum_StateTable_t *stateTable,
  Enum_State_t *state)
{
  unsigned long n = StateCompactToNumber(state, stateTable->numStoredVars);
  unsigned long product = n * MULT_FACTOR;

  product >>= (WORD_SIZE - stateTable->power);
  return product;
}

/**Function********************************************************************
    

  Synopsis           [Converts a state into a natural number.]

  Description        [The function converts the history dependent part of a
                      state to a natural number. Each history dependent
                      variable contributes a bit to the number. The bit is set
                      to 1 if the variable value is odd and to 0 if the
                      variable value is even.] 

  SideEffects        [None]

  SeeAlso            [StateHashFunction]

******************************************************************************/
unsigned long
StateCompactToNumber(
  Enum_State_t *state,
  int numStoredVars)
{
  long num=0; /* this variable contains the state compacted into a natural */
              /* number */
  long one = 1;
  int i, n, index;
  long *varArray = state->histDependVars;
  unsigned int sizeOfLong = 8 * sizeof(long);
  unsigned int loopCount = (numStoredVars <= sizeOfLong) ? numStoredVars :
      sizeOfLong;
  
  
  /* In this scheme, odd variable values are mapped to 1 and even values are
     mapped to 0. Let val be the array of 0 and 1 corresponding to the array
     of state variables generated by this mapping. The whole state is
     compacted into a natural number of size long by the following method.
     Let the length of long be l (32 or 64) depending on the machine. Then,
     num[i] = val[i] \xor val[i+l] \xor val[i+2l]......\xor val[i+kl], where
     i+kl is the least index greater than the number of state variables. */

  n = 0;
  index = 0;
  while(index < numStoredVars) {
    if((varArray[index] & one) != 0)
      n ^= one;
    index += sizeOfLong;
  }
  n &= one;
  num += n;
  
  for(i = 1; i < loopCount; i++) {
    num <<= 1;

    n = 0;
    index = i;
    while(index < numStoredVars) {
      if((varArray[index] & one) != 0)
        n ^= one;
      index += sizeOfLong;
    }
    n &= one;
    num += n;
  }

  return num;
}

