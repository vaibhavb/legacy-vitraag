%{
/**CFile***********************************************************************

  FileName    [prs.y]

  PackageName [prs]

  Synopsis    [The parser file in Reactive Modules.]

  Description [This file contains the grammer for reactive modules as
  well as the routines for parsing the inputs.]

  SeeAlso     [optional]

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


#include <stdio.h>
#include "prsInt.h"
  

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

extern int yydebug;
extern FILE *yyin;

int PrsParseValid;     /* whether the Module is valid or not */
static Mdl_Module_t *PrsModule;
static Mdl_Manager_t *PrsMdlManager;
static Atm_Atom_t *PrsAtom;
static Var_TypeManager_t *PrsTypeManager;
static Main_Manager_t *PrsMainManager;
static int PrsDummyVarCount;
static int PrsParsingGuardedCmd;
static Var_Type_t *PrsAssignedVarType;
static Var_Variable_t *PrsAssignedVar;
 
/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
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


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
static Atm_Expr_t *ExprParseDeclarator(char *name);
static Atm_Expr_t *ExprParseEventQuery(char *eventname);
static boolean ParseStatusIsValid(char *moduleName);
static Var_Variable_t *DesignatorCheckIsCorrect(char *, Mdl_Module_t *);
static unsigned int NumberObtainSize(int);
static Var_Variable_t *NodeIsCorrectVarDeclaration(PrsNode_t *);
void ModuleVariablesCreateRecursively(Mdl_Module_t *module, char *name,
                                      Var_Type_t *type, int peid);
void AtomAddControlReadAwaitVariableRecursively(Mdl_Module_t *module,
                                                Atm_Atom_t *atom,
                                                Var_Variable_t *var,
                                                int code);
void AtomCheckControlReadAwaitVariableRecursively(Mdl_Module_t *module,
                                                  Atm_Atom_t *atom,
                                                  Var_Variable_t *var,
                                                  int code);
static int PrsNodeCompare(PrsNode_t * node1,  PrsNode_t * node2);
static void PrsDeclaratorListRemoveDuplication(lsList declaratorList);
static PrsAtomAddVariables(lsList varList, int code);
static boolean PrsAssignmentTypeCheck(Atm_Assign_t*);
static boolean PrsForallAssignmentCheckDesignatorIsArrayOrBitvector(Var_Variable_t *var); 
static void PrsForallAssignmentCreateDummyVariable(
  char *dummyName, Var_Variable_t *arrayOrBitvectorVariable, Mdl_Module_t
  *module, Mdl_Manager_t *manager, Var_TypeManager_t *typeManager);
static boolean PrsForallAssignmentCheckIndexVariable(char * dummyVar1,
                                                     char * dummyVar2);
 
static Atm_Expr_t * PrsAtmExprCreate (Atm_ExprType exprtype, Atm_Expr_t *left,
                                      Atm_Expr_t *right);

static void PrsTimerExpressionCheckBound(Atm_Expr_t *left, Atm_Expr_t *right);

%}

%union{
  int keyword;
  int val;
  char *id;
  struct {
    char *ptr;
    int length;
  } name;
  PrsNode_t *nodepointer;
  Var_Type_t *vartypeptr;
  Atm_Expr_t *atmexprptr;
  Atm_Assign_t *atmassignptr;
  Atm_Cmd_t *atmcmdptr;
  Mdl_Expr_t *mdlexprptr;
  lsList List;
  int bool;
}

%start program

/*
 * token declaratons
 */

%token <name>   IDENTIFIER	/* data */
%token <val>  NATNUM
%token <bool> BOOL_CONST	/* Boolean constant */
%token <name>   EVENTQUERY	/* event */
%token <name>   EVENTSEND
%token <keyword> MODULE		/* keywords */
%token <keyword> ENDMODULE
%token <keyword> ATOM
%token <keyword> ENDATOM
%token <keyword> PRIVATE
%token <keyword> INTERFACE
%token <keyword> EXTERNAL
%token <keyword> LAZY
%token <keyword> EVENT
%token <keyword> READS
%token <keyword> AWAITS
%token <keyword> CONTROLS
%token <keyword> INIT
%token <keyword> UPDATE
%token <keyword> HIDE
%token <keyword> ENDHIDE
%token <keyword> IN
%token <keyword> TIMER    	/* types allowed */
%token <keyword> ARRAY
%token <keyword> BITVECTOR
%token <keyword> BITVECTOR_N
%token <keyword> FORALL
%token <keyword> IF		/* program reserved words */
%token <keyword> THEN
%token <keyword> ELSE
%token <keyword> FI
%token <keyword> OF
%token <keyword> TYPEDEF	/* type definition */
%token <keyword> DEFAULT        /* default guarded command */
%token <keyword> NONDET         /* nondeterministic assignment */
%token <keyword> '('		/* misc tokens */
%token <keyword> ')'
%token <keyword> '['
%token <keyword> ']'
%token <keyword> IMPLY
%token <keyword> EQUIV
%token <keyword> NEQUIV
%token <keyword> CHOICE		/* guarded statements */
%token <keyword> ARROW		
%token <keyword> ASSIGN		/* := */ 
%token <keyword> LASSIGN        /* :<= */
%token <keyword> PAR		/* || */
%token <keyword> AND
%token <keyword> OR
%token <keyword> XOR
%token <keyword> NOT
%token <keyword> GT
%token <keyword> LT
%token <keyword> GE
%token <keyword> LE
%token <keyword> EQ
%token <keyword> NEQ
%token <keyword> UMINUS

%token <keyword> ATOMDEF
%token <keyword> WAIT
%token <keyword> TOK_LINENO
%token <keyword> ELLIPSE

%token <keyword> ERROR

/*
 * precedence table
 */

%right ASSIGN
%left PAR
%left ARROW
%left EQUIV IMPLY NEQUIV
%left AND OR XOR
%left NOT
%left EQ NEQ
%left GT LT GE LE
%left '+' '-'
%left '*' '%' '/'
%left UMINUS


%type <val> int_const
%type <val> range_declarator
%type <val> atom_keyword
%type <vartypeptr> arrayindex
%type <vartypeptr> all_types
%type <nodepointer> commands_definition
%type <val> ctlvar_keyword
%type <nodepointer> ctlvars_definition
%type <nodepointer> ctlvar_def
%type <id> declarator
%type <nodepointer> index_declarator
%type <nodepointer> var_declarator
%type <nodepointer> declarations
%type <nodepointer> declaration
%type <nodepointer> atom_def
%type <mdlexprptr> module_expression
%type <nodepointer> variable_rename
%type <nodepointer> renames
%type <List> enum_const_list
%type <List> var_declarator_list
%type <List> declarator_list
%type <List> guarded_commands
%type <atmcmdptr> guarded_command
%type <atmexprptr> conditional
%type <List> assignments
%type <atmassignptr> assignment
%type <atmexprptr> predicate
%type <atmexprptr> exp
%type <atmexprptr> exp_or_nondet
%type <List> init_guarded_commands
%type <List> update_guarded_commands
%type <List> init_update_guarded_commands
%type <List> optional_wait_guarded_commands

%%

program	: 	
	{
          yylineno=1;
          PrsMdlManager =
              (Mdl_Manager_t *) Main_ManagerReadModuleManager(PrsMainManager);
          PrsTypeManager =
              (Var_TypeManager_t *)
              Main_ManagerReadTypeManager(PrsMainManager);
        }  
	  definitions {}


definitions
	: definition
	| definitions definition

/* Any definition is a type definition, module definition, or a LINENO */

definition
	: module_definition
	| type_definition
	| TOK_LINENO NATNUM ';'
	{ 
          yylineno = $2-1;
        }
	| error
	{
          if (PrsModule) Mdl_ModuleFree(PrsModule);
          YYABORT;
	}
	;

/* definition of type starts here */

type_definition
	: TYPEDEF declarator ':' enum_const_list
	{ 
	  if (!PrsTypeEnumAlloc($2, $4, PrsTypeManager)) YYERROR;
	}
	| TYPEDEF declarator ':' range_declarator
	{
          if (!PrsTypeRangeAlloc($2, $4, PrsTypeManager)) YYERROR;
        }
	| TYPEDEF declarator ':' ARRAY arrayindex OF all_types
	{
          if (!PrsTypeArrayAlloc($2, $5, $7, PrsTypeManager)) YYERROR;
        }
	| TYPEDEF declarator ':' BITVECTOR int_const
	{
          if (!PrsTypeBitvectorAlloc($2, $5, PrsTypeManager)) YYERROR;
        }
	| TYPEDEF declarator ':' BITVECTOR_N int_const
	{
          if (!PrsTypeBitvectorNAlloc($2, $5, PrsTypeManager)) YYERROR;
        }

enum_const_list
	: '{' declarator_list '}'
	{
          /* a enum_const_list is basically a set, so can remove all
             duplicates */
          lsList constList = $2;
          lsSort(constList, strcmp);
          lsUniq(constList, strcmp, free); 

          $$ = constList;
        }

arrayindex
        : declarator
        {
          char *typeName = $1;
          
          $$ = Var_TypeManagerReadTypeFromName(PrsTypeManager, typeName);
          FREE(typeName);
        }

        | range_declarator 
        {
          $$ = PrsTypeRangeAlloc(NIL(char), $1, PrsTypeManager);
        }

        | enum_const_list
        {
          $$ = PrsTypeEnumAlloc(NIL(char), $1, PrsTypeManager);
        }
  
all_types
	:EVENT
	{
          $$ = Var_TypeManagerReadTypeFromName(PrsTypeManager, "event");
        }
        | enum_const_list
        {
          $$ = PrsTypeEnumAlloc(NIL(char), $1, PrsTypeManager);
        }
        | ARRAY arrayindex OF all_types
        {
          $$ = PrsTypeArrayAlloc(NIL(char), $2, $4, PrsTypeManager);
        }
	| range_declarator
	{
          $$ = PrsTypeRangeAlloc(NIL(char), $1, PrsTypeManager);
        }
	| BITVECTOR NATNUM
	{
          $$ = PrsTypeBitvectorAlloc(NIL(char), $2, PrsTypeManager);
	}
	| BITVECTOR_N NATNUM
	{
          $$ = PrsTypeBitvectorNAlloc(NIL(char), $2, PrsTypeManager);
	}
	| declarator
	{
          char *typename = $1;
          
          $$ = Var_TypeManagerReadTypeFromName(PrsTypeManager,
                                              typename);
          if ($$ == NIL(Var_Type_t))
            Main_MochaErrorPrint("Error at line %d: type %s not defined.\n", 
                                 yylineno, typename);
          FREE(typename);
        }


/* defintion of Atom starts here */

atom_header 
	: CONTROLS var_declarator_list
	{
          PrsDeclaratorListRemoveDuplication($2);
	  /* need to check if the control variables 
	     are declared to be interface or private */
          PrsAtomAddVariables($2, 0);
          lsDestroy($2, PrsNodeFree);
        }
	optional_reads optional_awaits
	{
          /* for event atom, make sure that the intersection
             between the awaited variables and read variables
             is non-empty */
          if (Atm_AtomReadAtomType(PrsAtom) ==  Atm_Event_c) {
            lsList tmplist = Atm_AtomObtainReadAwaitVarList(PrsAtom);
            if (lsLength (tmplist) == 0) {
              Main_MochaErrorPrint(
                "Error at %d: event atom has no read and await variable in common.",
                yylineno);
              PrsParseValid = 0;
            }
            
            lsDestroy(tmplist, (void (*) ()) 0);
            
          }
        }

atom_keyword
        : { $$ = 0; /* no keyword */ }

	| LAZY
		{ $$ = 1; }
	| EVENT
		{ $$ = 2; }

optional_reads
	: { /* no reads */ }
        | READS var_declarator_list
	{
          PrsDeclaratorListRemoveDuplication($2);
          PrsAtomAddVariables($2, 1);
          lsDestroy($2, PrsNodeFree);
        }


optional_awaits
	: { /* no awaits */ }
        | AWAITS var_declarator_list
        {
          PrsDeclaratorListRemoveDuplication($2);
          PrsAtomAddVariables($2, 2);
          lsDestroy($2, PrsNodeFree);
        }


commands_definition
	: init_guarded_commands update_guarded_commands optional_wait_guarded_commands
	{ 
          lsGen lsgen;
          Atm_Cmd_t *guarded_cmd;
          
          lsForEachItem($1, lsgen, guarded_cmd){
            if (lsLength(Atm_CmdReadAssignList(guarded_cmd)) == 0 ) {
              Main_MochaErrorPrint(
                "Error at %d: Init commands must have non-empty assignments.\n",
                yylineno);
              PrsParseValid = 0;
            }
          }
          Atm_AtomAddInitCommandList (PrsAtom, $1);
          Atm_AtomAddUpdateCommandList (PrsAtom, $2);
          Atm_AtomAddWaitCommandList (PrsAtom, $3);
        }
	| init_update_guarded_commands optional_wait_guarded_commands
	{
          lsList readVarList = Atm_AtomReadReadVarList(PrsAtom);
          
          if (lsLength(readVarList) != 0) {
            Main_MochaErrorPrint(
              "Error at %d: Read variables not allowed in init guarded commands.\n", yylineno);
            PrsParseValid = 0;
          }
          
	  Atm_AtomAddInitCommandList (PrsAtom, $1);
	  Atm_AtomAddUpdateCommandList (PrsAtom, $1);
	  Atm_AtomAddWaitCommandList (PrsAtom, $2);
        }
	| update_guarded_commands optional_wait_guarded_commands
	{
          Atm_AtomAddInitCommandList (PrsAtom, lsCreate());
	  Atm_AtomAddUpdateCommandList (PrsAtom, $1);
	  Atm_AtomAddWaitCommandList (PrsAtom, $2);
        }
	| TOK_LINENO NATNUM ';'
	{ 
          yylineno = $2-1;
        }
	;

init_guarded_commands
	: INIT 
	{ PrsParsingGuardedCmd = 0; }	
	guarded_commands
	{ $$ = $3; }
	;

update_guarded_commands 
	: UPDATE 
	{ PrsParsingGuardedCmd = 1; }	
	guarded_commands
	{ $$ = $3; }
	;

optional_wait_guarded_commands
	: WAIT 
	{ PrsParsingGuardedCmd = 3; }	
	guarded_commands
	{ $$ = $3; }
	| 
	{ $$ = lsCreate(); }
	;

init_update_guarded_commands
	: INIT UPDATE 
	{ PrsParsingGuardedCmd = 2; }
	guarded_commands
	{ $$ = $4; }
	;

/* Module definition starts here */

module_definition 
	: MODULE declarator
	{
          char *moduleName = $2;
          Atm_AtomCounterReset(); 
          PrsParseValid = 1;
          PrsDummyVarCount = 1;
          
          if (Mdl_ModuleReadFromName(PrsMdlManager, moduleName)) {
            Main_MochaErrorPrint(
              "Error at line %d: Module \"%s\" defined before\n", yylineno,
              moduleName);

            FREE(moduleName);
            YYERROR;
          }
              
          PrsModule = Mdl_ModuleAlloc(moduleName);
          
        }		
          ctlvars_definition
          {
            if (!PrsParseValid) YYERROR;
            Atm_ReinitTypeCheck(PrsTypeManager);
            
	  }
	  atoms_def ENDMODULE
	  {
            /* PrsVariableCheckUnused */
            /* PrsAcyclicityTest */
            /* PrsModuleSetModuleExpr */
            
            char *moduleName = $2;
            st_generator *gen;
            Var_Variable_t *var;
            char *varName;
            Var_Type_t*  type;
            
            /* Checking the variables, eg. unused variables */
            Mdl_ModuleForEachVariable(PrsModule, gen, varName, var) {
              type = Var_VariableReadType(var);
              /* array variables and external or dummy variables are not
                 supposed to be controlled by atoms */
              if (!Var_TypeIsArray(type) &&
                  !Var_TypeIsBV(type)
                  && (Var_VariableIsPrivate(var) ||
                      Var_VariableIsInterface(var)))
                
                if ((Atm_Atom_t *) Var_VariableReadControlAtom(var) == NIL(Atm_Atom_t)) {
                  PrsParseValid = 0;
                  Main_MochaErrorPrint(
                    "Error at line %d: Variable %s in module %s not controlled by any atom.\n",
                    yylineno, varName, moduleName);
                }
            }
            
            /* Acyclicity test */
            if (!Mdl_ModuleTestForAcyclicity(PrsModule)) {
              PrsParseValid = 0;
              Main_MochaErrorPrint("Error at line %d: Cycle detected in Module %s.\n", 
                                   yylineno, moduleName);
            }
            
            /* test for validity */
            if (!ParseStatusIsValid(moduleName)) {
              FREE(moduleName);
              YYERROR;
            }
            
            /* set the module expression */
            Mdl_ModuleSetModuleExprForTypeModuleDef(PrsModule);
            
            Mdl_ModuleAddToManager(PrsModule, PrsMdlManager);
            PrsModule = NIL(Mdl_Module_t);
            
            Main_MochaPrint(
              "Module %s is composed and checked in.\n", moduleName);
            FREE(moduleName);
          }
	| declarator ASSIGN module_expression
		{
                  char * moduleName = $1;

                  if (!$3) {
                    FREE(moduleName);
                    YYERROR;
                  }

                  if (Mdl_ModuleReadFromName(PrsMdlManager, moduleName)) {
                    Main_MochaErrorPrint(
                      "Error at line %d: Module \"%s\" defined before\n", yylineno,
                      moduleName);
                    
                    FREE(moduleName);
                    YYERROR;
                  }
                  
                  PrsModule = Mdl_ModuleExprEvaluate(moduleName, $3, PrsMdlManager);

                  if (PrsModule) {
		    /*
                    Mdl_ModuleFree(
                    Mdl_ModuleRemoveFromManager(moduleName, PrsMdlManager));
		    */
                    Mdl_ModuleAddToManager(PrsModule, PrsMdlManager);
                    Main_MochaPrint("Module %s is composed and checked in.\n",
                                    moduleName);
                    PrsModule = NIL(Mdl_Module_t);
                    FREE(moduleName);
		  } else {
                    PrsParseValid = 0;
                    if (!ParseStatusIsValid(moduleName)) {
                      Mdl_ExprFree($3);
                      FREE(moduleName);
                      YYERROR;
                    }
                  }
                }

/* I am prohibiting an empty module. A module has to have some variables. */
ctlvars_definition
	: ctlvar_def
	| ctlvars_definition ctlvar_def


ctlvar_def
	: ctlvar_keyword 
	  	{
		  _varclass = $1; 
		}
	  declarations opt_semicolon

atoms_def
	: {/* no atom defined within the module */}
	| atoms_def atom_def



ctlvar_keyword
	: PRIVATE
		{ $$=PRIVATE;}
	| INTERFACE
		{ $$=INTERFACE;}
	| EXTERNAL
		{ $$=EXTERNAL;}

declarations
	: declaration { }
        | declarations ';' declaration {}

declaration 
	: declarator_list ':' TIMER
		{
		  int peid;
		  char *name, *tmpName;
                  lsGen lsgen;
		  Var_Type_t *type;

                  if (_varclass == PRIVATE) peid = 0; 
                  else if (_varclass == EXTERNAL) peid = 1;
                  else if (_varclass == INTERFACE) peid = 2;

                  if (PrsParseValid) {
                      lsForEachItem($1, lsgen, tmpName){
                        if (peid != 0)  /* not private */
                          name = tmpName;
                        else {
                          name = util_strcat3(Mdl_ModuleReadName(PrsModule),
                                              "/", tmpName);
                          FREE(tmpName);
                        }
			type = Var_VarNewTimerTypeAlloc();
                        ModuleVariablesCreateRecursively(PrsModule, name, type, peid);
                        FREE(name);
                      }
                  }
                  lsDestroy($1, NULL);
		}
	| declarator_list ':' all_types
		{
		  int peid;
		  char *name, *tmpName;
		  Var_Type_t *type = $3;	
                  lsGen lsgen;
                  
                  if (type == NIL(Var_Type_t)){
                    PrsParseValid = 0;
                  }
                  else {
                    if (_varclass == PRIVATE) peid = 0; 
                    else if (_varclass == EXTERNAL) peid = 1;
                    else if (_varclass == INTERFACE) peid = 2;

                    lsForEachItem($1, lsgen, tmpName){
                      if (Var_TypeManagerReadEnumElementFromName(
                        PrsTypeManager, tmpName)) {
                        Main_MochaErrorPrint(
                          "Error at line %d: an enumerated type element of name %s already exists.\n", yylineno, tmpName);
                        PrsParseValid = 0;
                      }
                    }

                    if (PrsParseValid)
                      lsForEachItem($1, lsgen, tmpName){
                        if (peid != 0)  /* not private */
                          name = tmpName;
                        else {
                          name = util_strcat3(Mdl_ModuleReadName(PrsModule),
                                              "/", tmpName);
                          FREE(tmpName);
                        }
                        ModuleVariablesCreateRecursively(PrsModule, name, type, peid);
                        FREE(name);
                      }
                  }
                  lsDestroy($1, NULL);
		}
	


atom_def
	:atom_keyword ATOM
                {
                  Atm_Atom_t *atm;
                  char *atmname, *tmpname;

                  /* Create a new atom name, because atom name is not
                     supplied */
                  tmpname = Atm_AtomNewName();
                  
                  atmname = Mdl_NamePrefixWithString(
                    Mdl_ModuleReadName(PrsModule), tmpname);
                  atm = Atm_AtomAlloc(atmname);
                  if ($1 == 1)
                    Atm_AtomSetAtomType(atm, Atm_Lazy_c);
                  if ($1 == 2)
                    Atm_AtomSetAtomType(atm, Atm_Event_c);
                  Mdl_ModuleAddAtom(PrsModule, atmname, atm);
                  
                  PrsAtom = atm;
                  FREE(tmpname);
                  FREE(atmname);
		}

   	  atom_header
	  {
            if (!PrsParseValid) YYERROR;
          }
          commands_definition ENDATOM 
	  {
            PrsAtom = NIL(Atm_Atom_t);
            if (!PrsParseValid) YYERROR;
          }
          


	/* the following is not allowed for the moment */
	/* | declarator ':' atom_header
		{ $$=PrsNewNode(ATOMNODE, ATOMDEF);
		  PrsSetAtomDefNode($$, $1, $3, NULL);
		}
	*/
	| atom_keyword ATOM declarator 
		{ Atm_Atom_t *atm;
		  char *atmName;
                                    
		  atmName = Mdl_NamePrefixWithString(
                    Mdl_ModuleReadName(PrsModule), $3);
		  atm = Atm_AtomAlloc(atmName);
                  if ($1 == 1)
                    Atm_AtomSetAtomType(atm, Atm_Lazy_c);
                  if ($1 == 2)
                    Atm_AtomSetAtomType(atm, Atm_Event_c);
		  Mdl_ModuleAddAtom(PrsModule, atmName, atm);
		  PrsAtom = atm;
                  FREE($3);
                  FREE(atmName);
		}

	atom_header
	{
          if (!PrsParseValid) YYERROR;
        }

        commands_definition ENDATOM
	{
          /* Check unused variables.  Flag warning and remove redundant variables. */
          /* Atm_AtomCheckUnusedVariables(PrsAtom); */
          PrsAtom = NIL(Atm_Atom_t);
          if (!PrsParseValid) YYERROR;
        }

	| TOK_LINENO NATNUM ';'
	{ 
          yylineno = $2-1;
        }


/* The following is the definiton of module obtained by various
   module operations.
 */

module_expression
	: declarator
	{
          if (!Mdl_ModuleReadFromName(PrsMdlManager, $1)){
            Main_MochaErrorPrint("Error at line %d: Module %s not defined.\n",
                                 yylineno, $1);
            $$ = NIL(Mdl_Expr_t);
          } else {
            $$ = Mdl_ExprModuleDupAlloc($1);
          }
          FREE($1);
	}
	| declarator variable_rename
	{
          
          if (!Mdl_ModuleReadFromName(PrsMdlManager, $1)){
            Main_MochaErrorPrint("Error at line %d: Module %s not defined.\n",
                                 yylineno, $1);
            $$ = NIL(Mdl_Expr_t);
          } else {
            Mdl_Expr_t * tmp = Mdl_ExprModuleDupAlloc($1);
            lsList list1 = (lsList) PrsNodeReadField1($2);
            lsList list2 = (lsList) PrsNodeReadField2($2);
            $$ = Mdl_ExprRenameAlloc(tmp, list1, list2);
          }
          FREE($1);
          PrsNodeFree($2);
	}
	| HIDE declarator_list IN module_expression ENDHIDE
	{
          $$ = Mdl_ExprHideAlloc($4, $2);
	}
	| '(' module_expression ')'
	{
          $$ = $2;
        }
	| '(' module_expression ')' variable_rename 
	{
          lsList list1 = (lsList) PrsNodeReadField1($4);
          lsList list2 = (lsList) PrsNodeReadField2($4);
          $$ = Mdl_ExprRenameAlloc($2, list1, list2);
        }
	| module_expression PAR module_expression 
	{
          $$ = Mdl_ExprComposeAlloc ($1, $3);
        }
	;

variable_rename 
	: '[' renames ']' {}
	{
	  $$= $2;
        }
	;



/************************************/
renames
	: declarator ASSIGN declarator
		{

                  lsList designatorList  = lsCreate();
                  lsList destinationList = lsCreate();
                  PrsNode_t *node =
                      PrsNodeAlloc(0, (char *) designatorList, 
                                   (char *) destinationList);
                  lsNewBegin (designatorList,  $1, LS_NH);
                  lsNewEnd   (destinationList, $3, LS_NH);
                  $$ = node;
		}
		  	
	| declarator ',' renames ',' declarator
		{ 

                  lsList designatorList  = (lsList) PrsNodeReadField1($3);
                  lsList destinationList = (lsList) PrsNodeReadField2($3);
                  
                  lsNewBegin (designatorList,  $1, LS_NH);
                  lsNewEnd   (destinationList, $5, LS_NH);

                  $$  = $3;
		}

		
declarator_list
	: declarator
		{
                  $$ = lsCreate();
                  lsNewEnd($$, $1, LS_NH);
		}
	| declarator_list ',' declarator
                {
                  $$ = $1;
                  lsNewEnd($$, $3, LS_NH);
		}

var_declarator_list
        : var_declarator 
           {
             $$ = lsCreate();
             lsNewEnd($$, (lsGeneric) $1, LS_NH);
           }

        | var_declarator_list ',' var_declarator
	{
          $$ = $1;
          lsNewEnd($$, (lsGeneric) $3, LS_NH);
        }

  
/* The following are the definitions of general data manipulations */

guarded_commands
	: guarded_command
          {
	    $$ = lsCreate();
            if ($1 != NIL(Atm_Cmd_t))
              lsNewEnd($$, (lsGeneric) $1, LS_NH);
	  }	
	| guarded_commands guarded_command
	  {
            if ($2 != NIL(Atm_Cmd_t))
              lsNewEnd($$, (lsGeneric) $2, LS_NH);
	  }
 

guarded_command
	: CHOICE predicate ARROW assignments opt_semicolon
            {
              Var_Type_t *predtype;
              Var_Type_t *booltype;
	      lsGen lsgen;
	      Atm_Assign_t *assgt;
              long OldBound, NewBound;
	      Var_Variable_t *timerVar;      
	      Var_Type_t *timerType;
	      Atm_Expr_t *expr;
              
              if (!ParseStatusIsValid(NIL(char)) || !$2) YYERROR;
              
              /* check if predicate is of boolean type */
              /* predtype = Atm_ExprObtainDataType($2); */
              booltype = Var_TypeManagerReadTypeFromName(PrsTypeManager,
                                                         "bool");

              /* if (!Atm_TypeCheck (booltype, $2)) { */
              if( !Atm_TypeCheckNew(booltype, &$2)){ 

                Main_MochaErrorPrint("Error at line %d: Type mismatch in guard\n", yylineno);
                PrsParseValid = 0;
              }

              /* should check the assignments */

              if (!ParseStatusIsValid(NIL(char))) YYERROR;
              lsForEachItem($4, lsgen, assgt){
                timerVar = Atm_AssignReadVariable(assgt);
                expr = Atm_AssignReadExpr(assgt);
                if (Var_VariableReadDataType(timerVar) == Var_Timer_c) {
                  if ((Atm_ExprReadType(expr) != Atm_TimerUpperBound_c) &&
                      (Atm_ExprReadType(expr) != Atm_NumConst_c)) {
                         Main_MochaErrorPrint("Disallowed assignment to timer variable \n",yylineno);
                  }
                  timerType = Var_VariableReadType(timerVar);
                  OldBound = Var_VariableTypeReadNumVals(timerType);
                  NewBound = (long) Atm_ExprReadLeftChild(expr) + 1;
                  if (NewBound > OldBound) {
                    Var_VariableTypeSetNumVals(timerType,NewBound);
                  }
                }
              }
              $$ = Atm_CmdAlloc($2, $4);				
            }
	| CHOICE DEFAULT ARROW assignments opt_semicolon
            {
              Atm_Cmd_t *cmd;
              
              if (!ParseStatusIsValid(NIL(char))) YYABORT;

              if (PrsParsingGuardedCmd == 0 || PrsParsingGuardedCmd == 2)
                if (Atm_AtomReadDefaultInitCommand(PrsAtom)) {
                  Main_MochaErrorPrint(
                    "Error at line %d: Only one default guarded command allowed.\n", yylineno);
                  PrsParseValid = 0;
                }
                else {
                  cmd = Atm_CmdAlloc(NIL(Atm_Expr_t), $4);
                  Atm_AtomSetDefaultInitCommand(PrsAtom, cmd);
                }
              
              if (PrsParsingGuardedCmd == 1 || PrsParsingGuardedCmd == 2)
                if (Atm_AtomReadDefaultUpdateCommand(PrsAtom)) {
                  Main_MochaErrorPrint(
                    "Error at line %d: Only one default guarded command allowed.\n", yylineno);
                  PrsParseValid = 0;
                }
                else {
                  cmd = Atm_CmdAlloc(NIL(Atm_Expr_t), $4);
                  Atm_AtomSetDefaultUpdateCommand(PrsAtom, cmd);
                }

              /* should check the assignments */
              /* return a NULL guarded command, so that the rule
                 "guarded_commands" can detect it and remove it from the
                 list of guarded assignments */
              $$ =  NIL(Atm_Cmd_t);
            }
	| TOK_LINENO NATNUM ';'
	{ 
          yylineno = $2-1;
          $$ =  NIL(Atm_Cmd_t);
        }


opt_semicolon
	: {}
	| ';' {} 
	;
	

assignments
	: /* no assignment  */
         { 
	   $$ = lsCreate();
	 }
	| assignment
         { 
	   $$ = lsCreate();
           lsNewEnd($$, (lsGeneric) $1, LS_NH);
	 }
	| assignments ';' assignment 
	{
           lsNewEnd($1, (lsGeneric) $3, LS_NH);
	   $$ = $1;
	}
	

conditional
	: IF predicate THEN exp ELSE exp FI 
	{
          Atm_Expr_t * exp = PrsAtmExprCreate(Atm_IfThenElseFi_c, $4, $6);

          $$ = PrsAtmExprCreate(Atm_IfThenElseFi_c, $2, exp);
        }




assignment
            : FORALL declarator declarator '[' declarator ']' 
             {
               Var_Variable_t *var, *actualVar, *indexVar;
             
               if ((var = DesignatorCheckIsCorrect ($3, PrsModule)) ==
                   NIL(Var_Variable_t))
                 PrsParseValid = 0;
               else {
                 Var_Type_t * type = Var_VariableReadType(var);
                 PrsAssignedVar = var;

                 if (Var_TypeIsBV(type))
                   PrsAssignedVarType = BoolType;
                 else 
                   PrsAssignedVarType = Var_VarTypeReadEntryType(type);
	       }
               
               /* check the index variable */
               if (PrsParseValid)
                 PrsParseValid = PrsForallAssignmentCheckIndexVariable($2, $5);

               /* check if variable is array or bitvector */
               if (PrsParseValid) 
                 PrsParseValid = PrsForallAssignmentCheckDesignatorIsArrayOrBitvector(var);

               /* Check here that the array variable assigned to here is wholly */
               /* controlled by the current atom. */
               if (PrsParseValid) {
                 AtomCheckControlReadAwaitVariableRecursively(PrsModule,
                                                              PrsAtom, var,
                                                              0);
               }

               /* create (dummy) index variable */
               if (PrsParseValid)
                 PrsForallAssignmentCreateDummyVariable($2, var,
                                                        PrsModule,
                                                        PrsMdlManager,
                                                        PrsTypeManager);
             FREE($3);
             FREE($5);

             }
            ASSIGN exp_or_nondet 
           {
             Var_Variable_t *indexVar;
             char *indexVarName;
             char *intString, *tmp;

             if (PrsParseValid) {
               intString = Main_ConvertIntToString(PrsDummyVarCount-1);
               tmp = util_strcat3($2, "-", intString);
               FREE(intString);             
               indexVarName = Mdl_NamePrefixWithString(
                 Mdl_ModuleReadName(PrsModule), tmp);
               FREE(tmp);
               
               indexVar = Mdl_ModuleReadVariableFromName(indexVarName, PrsModule);
               FREE(indexVarName);
               
               $$ = Atm_AssignForallAlloc (PrsAssignedVar, indexVar, $9);
               
               if (!PrsAssignmentTypeCheck($$)) {
                 Atm_AssignFree($$);
                 $$ = NIL(Atm_Assign_t);
                 PrsParseValid = FALSE;
               }
             }
             FREE($2);
           }

           | declarator '[' declarator ']'
           {
             Atm_Expr_t *indexExpr;
             Atm_ExprType indexExprType;
             Var_Type_t *varType, *indexType;
             Var_EnumElement_t *enumElement;
             Var_DataType dataType;
             char *varName, *actualVarName;
             Var_Variable_t *var, *actualVar;

             if ((var = DesignatorCheckIsCorrect ($1, PrsModule)) == NIL(Var_Variable_t)) 
               PrsParseValid = 0;
             
             if (PrsParseValid) {
               varType = Var_VariableReadType(var);
               varName = Var_VariableReadName(var);
                            
               if (Var_TypeIsArray(varType)) {
                 indexType = Var_VarTypeReadIndexType(varType);
                 if (Var_TypeIsEnum(indexType)) {
                   PrsAssignedVarType = Var_VarTypeReadEntryType(varType);
		 }
                 else {
                   Main_MochaErrorPrint(
                     "Error at line %d: variable index not of enumerated type.\n", yylineno);
                   PrsParseValid = 0;
                 }   
               }
               else {
                 Main_MochaErrorPrint(
                   "Error at line %d: variable not an array.\n", yylineno);
                 PrsParseValid = 0;
               }   
             }

             if (PrsParseValid) {
	       
	       /* check whether index types are correct and whether the
		  variable is controlled by the current atom */
                 
	       indexExpr = ExprParseDeclarator($3);
	       indexExprType = Atm_ExprReadType(indexExpr);
	       
	       if (Atm_ExprReadType(indexExpr) == Atm_EnumConst_c) {
		 enumElement = (Var_EnumElement_t *)
		   Atm_ExprReadLeftChild(indexExpr);
		 if (Var_EnumElementReadType(enumElement) != indexType) {
		   Main_MochaErrorPrint(
					"Error at %d: index is of incorrect type\n", yylineno);
		   PrsParseValid = 0;
		 }
		 else {
		   actualVarName = util_strcat4(varName, "[",
						Var_EnumElementReadName(enumElement), "]"); 
		   actualVar = Mdl_ModuleReadVariableFromName(actualVarName,
                                                            PrsModule);
                   PrsAssignedVar = actualVar;
		   AtomCheckControlReadAwaitVariableRecursively(PrsModule,
								PrsAtom, actualVar,
								0);
		   FREE(actualVarName);
		 }
	       }
	       else {
		 Main_MochaErrorPrint(
				      "Error at %d: incorrect index.\n", yylineno);
		 PrsParseValid = 0;
	       } 
                 
	       Atm_ExprFreeExpression(indexExpr);
	     }

             FREE($1);
             FREE($3);
           }
           ASSIGN exp_or_nondet
	   {
             if (PrsParseValid) {
               $$ = Atm_AssignStdAlloc(PrsAssignedVar, $7);
               
               if (!PrsAssignmentTypeCheck($$)) {
                 Atm_AssignFree($$);
                 $$ = NIL(Atm_Assign_t);
                 PrsParseValid = FALSE;
               }
             }
           }

	   | declarator '[' NATNUM ']'
           {
             Var_Variable_t *var;
             Var_Type_t *varType;
             Var_DataType dataType; 
             char *varName, *actualVarName;
             Var_Variable_t *actualVar;
             int arraySize;

             if ((var = DesignatorCheckIsCorrect ($1, PrsModule)) == NIL(Var_Variable_t)) 
               PrsParseValid = 0;
             
             if (PrsParseValid) {
               varName = Var_VariableReadName(var);
               varType = Var_VariableReadType(var);

               if (!Var_TypeIsArray(varType) && !Var_TypeIsBV(varType)) {
                 Main_MochaErrorPrint("Error at line %d: variable \"%s\" not  an array or bitvector.\n",
                                      yylineno,
                                      varName);
		  PrsParseValid = 0;
               }
	       
	       if (Var_TypeIsBV(varType)) {
		  PrsAssignedVarType = Var_TypeManagerReadTypeFromName(PrsTypeManager, "bool");
	       }

               if (Var_TypeIsArray(varType)) 
                 if (Var_TypeIsRange(Var_VarTypeReadIndexType(varType))) {
                   PrsAssignedVarType = Var_VarTypeReadEntryType(varType);
		 }
                 else {
                   Main_MochaErrorPrint(
                     "Error at line %d: index of variable not a range.\n", yylineno);
                   PrsParseValid = 0;
                 }
             }

             if (PrsParseValid) {
	       arraySize = Var_VarTypeArrayOrBitvectorReadSize(varType);
	       if ($3 >= arraySize) {
		 Main_MochaErrorPrint(
                   "Error at line %d: index out of bound.\n", yylineno);
		 PrsParseValid = 0;
	       }
               else {
		 actualVarName = ALLOC(char, strlen(varName) + 1 +
				       NumberObtainSize($3) + 1 + 1);
		 sprintf(actualVarName, "%s[%d]", varName, $3);
		 actualVar = Mdl_ModuleReadVariableFromName(actualVarName,
                                                          PrsModule);
                 PrsAssignedVar = actualVar;
		 FREE(actualVarName);
		 AtomCheckControlReadAwaitVariableRecursively(PrsModule,
							      PrsAtom, actualVar,
							      0);
	       }
	     }

             FREE($1);
           }

           ASSIGN exp_or_nondet
	   {
             if (PrsParseValid) {
               $$ = Atm_AssignStdAlloc(PrsAssignedVar, $7);
               
               if (!PrsAssignmentTypeCheck($$)) {
                 Atm_AssignFree($$);
                 $$ = NIL(Atm_Assign_t);
                 PrsParseValid = FALSE;
               }
             }
           }

           | declarator
           {
             Var_Variable_t *var;
             
             if ((var = DesignatorCheckIsCorrect ($1, PrsModule)) == NIL(Var_Variable_t))
               PrsParseValid = 0;
             else 
               AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, var, 0);

             if (PrsParseValid) {
               PrsAssignedVar = var;
	       PrsAssignedVarType = Var_VariableReadType(var);
             }
             
	     FREE($1);
           }

           ASSIGN exp_or_nondet
	   {
             if (PrsParseValid) {
               $$ = Atm_AssignStdAlloc(PrsAssignedVar, $4);
               
               if (!PrsAssignmentTypeCheck($$)) {
                 Atm_AssignFree($$);
                 $$ = NIL(Atm_Assign_t);
                 PrsParseValid = FALSE;
               }
             }
           }
          | declarator 
           {
             Var_Variable_t *var;
             
             if ((var = DesignatorCheckIsCorrect ($1, PrsModule)) == NIL(Var_Variable_t))
               PrsParseValid = 0;
             else 
               AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, var, 0);

             if (PrsParseValid) {
               PrsAssignedVar = var;
	       PrsAssignedVarType = Var_VariableReadType(var);
             }
             
	     FREE($1);
           }
         LASSIGN NATNUM
           {
             Atm_Expr_t *upBoundExpr;

	     if (Var_VariableReadDataType(PrsAssignedVar) != Var_Timer_c) {
		PrsParseValid = 0;
	     }

	     upBoundExpr = Atm_ExprAlloc(Atm_TimerUpperBound_c,
				(Atm_Expr_t *) (long) $4,
				NIL(Atm_Expr_t));

             if (PrsParseValid) {
                $$ = Atm_AssignStdAlloc(PrsAssignedVar, upBoundExpr);
                PrsAssignmentTypeCheck($$);
	     } 
	     else {
                $$ = NIL(Atm_Assign_t);
             }
           }
           | EVENTSEND  /* ! */ 
		{
                  Var_Variable_t *var;
		  Atm_Expr_t *expr;
		  Atm_Expr_t *varexpr;
                  lsList ctrlList, readList;
                  char *privateName, *name;

                  name = ALLOC(char, $1.length + 1);
                  memcpy(name, $1.ptr, $1.length);
                  *(name + $1.length) = '\0';

       		  ctrlList = Atm_AtomReadControlVarList(PrsAtom);
                  readList = Atm_AtomReadReadVarList(PrsAtom);
                  
                  privateName = util_strcat3(Mdl_ModuleReadName(PrsModule),
                                             "/", name);

                  var = Mdl_ModuleReadVariableFromName(name, PrsModule);
                  if (var == NIL(Var_Variable_t))
                    var = Mdl_ModuleReadVariableFromName(privateName, PrsModule);
                  
		  if ( var == NIL(Var_Variable_t) ) {
                    Main_MochaErrorPrint("Error at line %d: Variable %s not defined.\n",
                                         yylineno, name);
                    PrsParseValid = FALSE;
                  }

                  if (PrsParseValid && !PrsIsInList(ctrlList, (lsGeneric)
                                                    var)) {
                    Main_MochaErrorPrint("Error at line %d: Variable %s not controlled by atom.\n",
                                         yylineno, name);
                    PrsParseValid = FALSE;
                  }

                  if (PrsParseValid && !PrsIsInList(readList, (lsGeneric)
                                                    var)) {
                    Main_MochaErrorPrint("Error at line %d: Variable %s not read by atom.\n",
                                         yylineno, name);
                    PrsParseValid = FALSE;
                  }
                  
                  if (PrsParseValid) {
                    $$ = Atm_AssignEventAlloc(var);
                    PrsParseValid = PrsAssignmentTypeCheck($$);
                    
                    if (!PrsParseValid) {
                      Atm_AssignFree($$);
                      $$ = NIL(Atm_Assign_t);
                    }
                  }
                }


exp_or_nondet : NONDET
                {
                  $$ = Atm_ExprAlloc(Atm_Nondet_c,
                                     (Atm_Expr_t *) PrsAssignedVarType,
                                     NIL(Atm_Expr_t));
                }
              | exp
                {
                  $$ = $1;
                }


predicate  : exp
		{ $$ = $1;
		}


exp	
	: declarator /* a variable (primed/unprimed, or a enumerative constant
                      */
		{
                  $$ = ExprParseDeclarator($1);

                  FREE($1);   
                }
	| declarator '[' exp ']' /*indexing of an array or bitvector */
		{
                  /* $$ = PrsAtmExprCreate(Atm_Equal_c, $1, $3); */
                  lsList readList, awaitList;
                  Var_Variable_t *var, *actualVar;
                  Atm_ExprType exprType;
                  char *varName = $1;
                  int isPrimed = PrsVariableCheckPrimed(varName);
                  int size, index;
                  char *privateName, *actualVarName, *realName;
                  Var_Type_t *varType, *indexType;
                  Var_DataType dataType;

                  if (PrsParsingGuardedCmd == 0 || PrsParsingGuardedCmd == 2) 
                    if (!isPrimed) {
                      Main_MochaErrorPrint(
                        "Error at line %d: variable %s cannot be read in init guarded commands.\n", yylineno, varName);
                      PrsParseValid = 0;
                    }
                    
                  if ((var = Mdl_ModuleReadVariableFromName(varName, PrsModule)) ==
                      NIL(Var_Variable_t)) {
                    privateName = util_strcat3(Mdl_ModuleReadName(PrsModule),
                                               "/", varName);
                    if ((var = Mdl_ModuleReadVariableFromName(privateName, PrsModule)) ==
                        NIL(Var_Variable_t)) 
                      PrsParseValid = 0;
                    realName = privateName;
                  }
                  else
                    realName = util_strsav(varName);
                  
                  if ($3 != NIL(Atm_Expr_t))
                    exprType = Atm_ExprReadType($3);
                  else
                    PrsParseValid = 0;
                  
                  if (PrsParseValid) {
                    if (exprType == Atm_NumConst_c) {
                      index = (int) (long) Atm_ExprReadLeftChild($3);
                      /* FIXME: check if index is within range of index of
                         variable */
                      
                      actualVarName = ALLOC(char, strlen(realName) + 1 +
                                            NumberObtainSize(index) + 1 + 1);
                      
                      sprintf(actualVarName, "%s[%d]", realName, index);
                      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, PrsModule);

                      if (isPrimed) 
                        AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, actualVar, 2); 
                      else
                        AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, actualVar, 1);
                      
                      FREE(actualVarName);
                    }
                    else if (exprType == Atm_EnumConst_c) {
                      actualVarName = util_strcat4(realName, "[",
                                                   Var_EnumElementReadName(
                                                     (Var_EnumElement_t *) 
                                                     Atm_ExprReadLeftChild($3)), "]"); 
                      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, PrsModule);

                      if (isPrimed) 
                        AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, actualVar, 2); 
                      else
                        AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, actualVar, 1);
                      
                      FREE(actualVarName);
                    }
                    else {
                      if (isPrimed) 
                        AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, var, 2); 
                      else
                        AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, var, 1);
                    }
                  }

                  if (PrsParseValid) {
                    if (exprType == Atm_NumConst_c || exprType == Atm_EnumConst_c) {
                      if (isPrimed)
                        $$ = Atm_ExprAlloc(Atm_PrimedVar_c, (Atm_Expr_t *)
                                           actualVar, NIL(Atm_Expr_t));
                      else
                        $$ = Atm_ExprAlloc(Atm_UnPrimedVar_c, (Atm_Expr_t *)
                                           actualVar, NIL(Atm_Expr_t));
                      Atm_ExprFreeExpression($3);
                    }
                    else {
                      Atm_Expr_t *leftExpr;
                      if (isPrimed)
                        leftExpr = Atm_ExprAlloc(Atm_PrimedVar_c,
                                                 (Atm_Expr_t *) var, NIL(Atm_Expr_t));
                      else
                        leftExpr = Atm_ExprAlloc(Atm_UnPrimedVar_c,
                                                 (Atm_Expr_t *) var, NIL(Atm_Expr_t));
                      
                      $$ = Atm_ExprAlloc(Atm_Index_c, leftExpr, $3);
                    }
                  }
                  else
                      $$ = NIL(Atm_Expr_t);

                  FREE(realName);
                  FREE(varName);
                }
	| NATNUM 
		{
                  /* $$ = PrsAtmExprCreate(Atm_Equal_c, $1, $3); */
                  $$=Atm_ExprAlloc(Atm_NumConst_c, 
				   (Atm_Expr_t *) (long) $1, 
			           NIL(Atm_Expr_t));
                }
	| BOOL_CONST
		{ 
                  $$ = Atm_ExprAlloc(Atm_BoolConst_c,
                                     (Atm_Expr_t *) (long) $1,
                                     NIL(Atm_Expr_t));
                }
	| exp '+' exp
		{
                  $$ = PrsAtmExprCreate(Atm_Plus_c, $1, $3);
                }
	| exp '-' exp
        	{
                  $$ = PrsAtmExprCreate(Atm_Minus_c, $1, $3);
                }
	| exp EQ exp
		{
                  $$ = PrsAtmExprCreate(Atm_Equal_c, $1, $3);
                }
	| exp NEQ exp
		{
                  $$ = PrsAtmExprCreate(Atm_NotEqual_c, $1, $3);
                }
	| exp GT exp
		{
                  $$ = PrsAtmExprCreate(Atm_Greater_c, $1, $3);
                }
	| exp LT exp
		{
                  $$ = PrsAtmExprCreate(Atm_Less_c, $1, $3);
                }
	| exp GE exp
	{
          Var_Variable_t *TimerVar;
          Var_Type_t *timerType;
	  long NewBound, OldBound;

          $$ = PrsAtmExprCreate(Atm_GreaterEqual_c, $1, $3);

          PrsTimerExpressionCheckBound($1, $3);
        }
	| exp LE exp
	{ 
	  $$ = PrsAtmExprCreate(Atm_LessEqual_c, $1, $3); 

          PrsTimerExpressionCheckBound($1, $3);
          
	}
	| exp AND exp
		{ $$ = PrsAtmExprCreate(Atm_And_c, $1, $3); }
   	| exp OR exp
		{ $$ = PrsAtmExprCreate(Atm_Or_c, $1, $3); }
   	| exp IMPLY exp
		{ $$ = PrsAtmExprCreate(Atm_Implies_c, $1, $3); }
   	| exp EQUIV exp
		{ $$ = PrsAtmExprCreate(Atm_Equiv_c, $1, $3); }
   	| exp XOR exp
		{ $$ = PrsAtmExprCreate(Atm_Xor_c, $1, $3); }
	| AND exp
		{ $$ = Atm_ExprAlloc(Atm_RedAnd_c, $2, NIL(Atm_Expr_t)); }
	| OR exp
		{ $$ = Atm_ExprAlloc(Atm_RedOr_c, $2, NIL(Atm_Expr_t)); }
	| XOR exp
		{ $$ = Atm_ExprAlloc(Atm_RedXor_c, $2, NIL(Atm_Expr_t)); }
        | '-' exp %prec UMINUS
		{
                  if (! $2)
                    $$ = NIL(Atm_Expr_t);
                  else 
                    $$ = Atm_ExprAlloc(Atm_UnaryMinus_c, $2, NIL(Atm_Expr_t));
                }
	| NOT exp
		{
                  if (! $2)
                    $$ = NIL(Atm_Expr_t);
                  else
                    $$=Atm_ExprAlloc(Atm_Not_c, $2,  NIL(Atm_Expr_t));}
	| '(' exp ')'
		{
                  $$=$2;
                }
	| EVENTQUERY  /* ? */ 
		{

                  /* $$ = PrsAtmExprCreate(Atm_EventQuery_c, $1, $3); */
                 char *name;
                 
                 name = ALLOC(char, $1.length + 1);
                 memcpy(name, $1.ptr, $1.length);
                 *(name + $1.length) = '\0';
                 
                 $$ = ExprParseEventQuery(name);
                 FREE(name);
                }
	| conditional
        ;


int_const
        : NATNUM
          {
            $$ = $1;
          }

        | int_const '-' int_const
          {
            $$ = $1 - $3;
          }

        | int_const '+' int_const
          {
            $$ = $1 + $3;
          }

        | int_const '*' int_const
          {
            $$ = $1 * $3;
          }
        | int_const '/' int_const
          {
            $$ = $1 / $3;
          }
        | int_const '%' int_const
          {
            $$ = $1 % $3;
          }
        | '(' int_const ')'
          {
            $$ = $2;
          }


index_declarator
               : declarator '[' declarator ']'
                 {
                   $$ = PrsNodeAlloc(2, $1, $3);
                 }
               | declarator '[' NATNUM ']'
               {
                 $$ = PrsNodeAlloc(1, $1, (char *) (long) $3);
               }

range_declarator :  '(' NATNUM ELLIPSE int_const ')'
                {
		  if ($2 != 0) {
                    Main_MochaErrorPrint(
                      "Error at line %d: range has to start at zero.\n");
                    YYERROR;
                  }
                  $$ = $4;
                }

declarator : IDENTIFIER
               {
                 char *name;
                 
                 name = ALLOC(char, $1.length + 1);
                 memcpy(name, $1.ptr, $1.length);
                 *(name + $1.length) = '\0';

                 $$ = name;
		}
	     | IN  /* seems like "in" is always used as a variable */
		{
                  $$ = util_strsav("in");
		}

var_declarator
           : index_declarator
           {
             $$ = $1;
           }


           | declarator 
           {
             $$ = PrsNodeAlloc(0, $1, NIL(char));
           }

%%


int PrsParse()
{
  int flag;
  
  yydebug = 0;
	
  yyrestart(yyin);
  
  flag = yyparse();

  return (!flag);
}

yyerror(char *s){
  Main_MochaErrorPrint("Error at line %d:  %s \n",  yylineno, s);
}

void
PrsGlobalPointersInitialize(Main_Manager_t *manager)
{
  PrsModule = NIL(Mdl_Module_t);
  PrsParseValid = 1;
  PrsMainManager = manager;
}

static Atm_Expr_t *
ExprParseDeclarator(
  char *name)
{
  Var_Type_t *type;
  Var_DataType varDataType;
  Var_Variable_t *var;
  Var_EnumElement_t *enumconst;
  int isPrimed;
  char *realName = NIL(char), *privateName=NIL(char), *dummyName=NIL(char);
  lsList readList;
  lsList awaitList;
  Atm_Expr_t *resultExpr;
  int varCode = -1;
  
  isPrimed = PrsVariableCheckPrimed(name);

  /* an enumerative constant? */
  if (!isPrimed) 
    if ((enumconst = Var_TypeManagerReadEnumElementFromName(PrsTypeManager, name)) !=
        NIL(Var_EnumElement_t))
      return Atm_ExprAlloc(Atm_EnumConst_c, (Atm_Expr_t*) enumconst,
                           NIL(Atm_Expr_t));
  
  if (PrsParseValid) {
    char * intString;

    privateName = util_strcat3(Mdl_ModuleReadName(PrsModule),
                               "/", name);
    intString = Main_ConvertIntToString(PrsDummyVarCount-1);
    dummyName = util_strcat3(privateName, "-", intString);
    FREE(intString);
    
    if ((var = Mdl_ModuleReadVariableFromName(name, PrsModule)) !=
        NIL(Var_Variable_t)) {
      if (!isPrimed && (PrsParsingGuardedCmd == 0 || PrsParsingGuardedCmd == 2)) {
        Main_MochaErrorPrint(
          "Error at line %d: variable %s cannot be read in init guarded commands.\n", yylineno, name);
        PrsParseValid = 0;
      }
      varCode = 0;
      realName = util_strsav(name);
    }
    else if ((var = Mdl_ModuleReadVariableFromName(privateName, PrsModule)) != NIL(Var_Variable_t)) {
      if (!isPrimed && (PrsParsingGuardedCmd == 0 || PrsParsingGuardedCmd == 2)) {
        Main_MochaErrorPrint(
          "Error at line %d: variable %s cannot be read in init guarded commands.\n", yylineno, name);
        PrsParseValid = 0;
      }
      varCode = 1;
      realName = util_strsav(privateName);
    }
    else if ((var = Mdl_ModuleReadVariableFromName(dummyName, PrsModule)) != NIL(Var_Variable_t)) {
      if (isPrimed) {
        Main_MochaErrorPrint("Error at line %d: Variable %s is a dummy variable, cannot be awaited.\n",
                             yylineno, name);
        PrsParseValid = 0;
      }
      varCode = 2;
      realName = util_strsav(dummyName);
    }
    
    FREE(privateName);
    FREE(dummyName);
    
    switch (varCode) {
        case 0 :
        case 1 : 
        {
          if (isPrimed) 
            AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, var, 2); 
          else
            AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, var, 1);
          
          if (PrsParseValid) 
            if (isPrimed) 
              resultExpr = Atm_ExprAlloc(Atm_PrimedVar_c, (Atm_Expr_t*) var, 
                                         NIL(Atm_Expr_t));
            else 
              resultExpr = Atm_ExprAlloc(Atm_UnPrimedVar_c, (Atm_Expr_t*) var, 
                                         NIL(Atm_Expr_t));
          break;
        }
        
        case 2 : 
        {
          resultExpr = Atm_ExprAlloc(Atm_UnPrimedVar_c, (Atm_Expr_t *) var,
                                     NIL(Atm_Expr_t));
          break;
        }
        
        default : 
        {
          PrsParseValid = 0;
          if (isPrimed)
            Main_MochaErrorPrint("Error at line %d: Variable %s' not defined.\n", yylineno, name);
          else 
            Main_MochaErrorPrint("Error at line %d: Variable %s not defined.\n", yylineno, name);
        }
    }
    
    if (realName)
      FREE(realName);
  }
  
  if (PrsParseValid)
    return resultExpr;
  else
    return NIL(Atm_Expr_t);
}


static Atm_Expr_t *
ExprParseEventQuery(
  char *eventname)
{
  Var_Variable_t *var;
  lsList readlist;
  lsList awaitlist;
  char *private_name;
  Atm_Expr_t *resultexpr;
  
  readlist = Atm_AtomReadReadVarList(PrsAtom);
  awaitlist = Atm_AtomReadAwaitVarList(PrsAtom);
  private_name = util_strcat3(Mdl_ModuleReadName(PrsModule),
                              "/", eventname);
  
  var = Mdl_ModuleReadVariableFromName(eventname, PrsModule);
  if (var == NIL(Var_Variable_t))
    var = Mdl_ModuleReadVariableFromName(private_name, PrsModule);
  
  if (var != NIL(Var_Variable_t) &&
      PrsIsInList(readlist, (lsGeneric) var) &&
      PrsIsInList(awaitlist, (lsGeneric) var)) {
    resultexpr=Atm_ExprAlloc(
      Atm_EventQuery_c, 
      Atm_ExprAlloc(Atm_UnPrimedVar_c, (Atm_Expr_t*) var, NIL(Atm_Expr_t)),
      NIL(Atm_Expr_t));
  }
  else {
    if (var == NIL(Var_Variable_t)) /* variable not defined */                    
      Main_MochaErrorPrint("Error at line %d: Variable %s not defined.\n",
                           yylineno, eventname);
    else 
      Main_MochaErrorPrint(
        "Error at line %d: Event variable %s either not read or awaited by atom.\n",
        yylineno, eventname);
    
    resultexpr=NIL(Atm_Expr_t);
    PrsParseValid = 0;
  }

  FREE(private_name);
  
  return resultexpr;
  
}

static boolean
ParseStatusIsValid(
  char * moduleName
)
{
  /* test for validity */
  if (!PrsParseValid) {
    if (moduleName == NIL(char)){
      moduleName = Mdl_ModuleReadName(PrsModule);
    }
    Main_MochaErrorPrint("Warning at line %d: %s is not composed.\n", 
                         yylineno, moduleName);
    
    Mdl_ModuleFree(PrsModule);
    PrsModule=NIL(Mdl_Module_t);
    return FALSE;
  }
  else 
    return TRUE;
}

static Var_Variable_t*
DesignatorCheckIsCorrect(
  char *name,
  Mdl_Module_t *module) 
{
  boolean primed;
  Var_Variable_t *var;
  int num;
  
  primed  = PrsVariableCheckPrimed(name);
  
  if (!primed) {
    Main_MochaErrorPrint("Warning at line %d: unprimed variable %s on left hand side of assignment.\n",
                         yylineno, name);
    return NIL(Var_Variable_t);
  }
  
  var = Mdl_ModuleReadVariableFromName(name, module);
  
  if (var==NIL(Var_Variable_t)) {
    char * private_name = util_strcat3(Mdl_ModuleReadName(PrsModule),
                                       "/", name);
    var = Mdl_ModuleReadVariableFromName(private_name, module);
    FREE(private_name);
  }
  
  if (var==NIL(Var_Variable_t)) {
    Main_MochaErrorPrint("Error at line %d: Variable %s not defined.\n",
                         yylineno, name);
  }
  
  return var;
}

static unsigned int
NumberObtainSize(
  int value)
{
  
  unsigned int modValue, len;
  
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
  return len;
}

static Var_Variable_t *
NodeIsCorrectVarDeclaration(
  PrsNode_t *node)
{
  int isIndexVariable;
  char *varname, *actualVarName, *realName;
  char *private_varname;
  Var_Variable_t *var, *actualVar;
  Var_Type_t *varType;
  Var_EnumElement_t *enumElement;
  
  if (node->id == 0) 
    isIndexVariable = FALSE;
  else
    isIndexVariable = TRUE;
  varname  = PrsNodeReadField1(node);                    
  if(PrsVariableCheckPrimed(varname)) {
    Main_MochaErrorPrint(
      "Warning at line %d: variable should not be primed in declaration.\n",
                         yylineno, varname);
    return NIL(Var_Variable_t);
  }
  

  var = Mdl_ModuleReadVariableFromName(varname, PrsModule);
  
  if (var == NIL(Var_Variable_t)){
    private_varname = util_strcat3(Mdl_ModuleReadName(PrsModule),
                                   "/", varname);
    var = Mdl_ModuleReadVariableFromName(private_varname, PrsModule);
    if (var == NIL(Var_Variable_t)) {
      Main_MochaErrorPrint(
        "Error at line %d: Variable %s not defined.\n", yylineno, varname);
      FREE(varname);
      FREE(private_varname);
      return NIL(Var_Variable_t);
    }
    else
      realName = private_varname;
  }
  else
    realName = util_strsav(varname);
  
  
  if (!isIndexVariable) {
    FREE(varname);
    FREE(realName);
    return var;
  }  
  else {
    varType = Var_VariableReadType(var);
    if (Var_VariableReadDataType(var) != Var_Array_c) {
      Main_MochaErrorPrint(
        "Error at line %d: variable %s not an array variable\n", yylineno, varname);
      FREE(varname);
      FREE(realName);
      return NIL(Var_Variable_t);
    }
    
    if (node->id == 2) {
      char *indexName = PrsNodeReadField2(node);
      if ((enumElement =
           Var_TypeManagerReadEnumElementFromName(PrsTypeManager, indexName))
          == NIL(Var_EnumElement_t)) {
        Main_MochaErrorPrint(
          "Error at line %d: Incorrect index name %s.\n",
          yylineno, indexName);
        FREE(realName);
        FREE(varname);
        FREE(indexName);
        return NIL(Var_Variable_t);
      }
      else if (Var_EnumElementReadType(enumElement) != Var_VarTypeReadIndexType(varType)) {
        Main_MochaErrorPrint(
          "Error at line %d: Incorrect index name %s.\n",
          yylineno, indexName);
        FREE(realName);
        FREE(varname);
        FREE(indexName);
        return NIL(Var_Variable_t);
      }

      actualVarName = util_strcat4(realName, "[",
                                   Var_EnumElementReadName(enumElement), "]");
      FREE(indexName);
    }
    else if (node->id == 1) {
      int numConst = (int) (long) PrsNodeReadField2(node);
      
      if (Var_VarTypeReadDataType(Var_VarTypeReadIndexType(varType)) != Var_Range_c) {
        Main_MochaErrorPrint(
          "Error at line %d: Mismatch in index.\n", yylineno);
        FREE(realName);
        FREE(varname);
        return NIL(Var_Variable_t);
      }

      if (numConst >= Var_VarTypeArrayOrBitvectorReadSize(varType)) {
        Main_MochaErrorPrint(
          "Error at line %d: Index out of bound.\n", yylineno);
        FREE(realName);
        FREE(varname);
        return NIL(Var_Variable_t);
      }
      
      actualVarName = ALLOC(char, strlen(realName) + 1 + NumberObtainSize(numConst)
                            + 1 + 1);
      sprintf(actualVarName, "%s[%d]", realName, numConst);
    }
    
    actualVar = Mdl_ModuleReadVariableFromName(actualVarName, PrsModule);
    FREE(realName);
    FREE(varname);
    FREE(actualVarName);
    return actualVar;
  }
}


void
ModuleVariablesCreateRecursively(
  Mdl_Module_t *module,
  char *name,
  Var_Type_t *type,
  int peid)
{
  Var_Type_t *entryType, *indexType;
  int size, i;
  Var_Variable_t *var;
  char *varName;
  
  if(Var_TypeIsArray(type)) {
    int indexIsEnum = 0;  

    var = Var_VariableAlloc((VarModuleGeneric) module, name, type);
    Var_VariableSetPEID(var, peid);
    Mdl_ModuleAddVariable(module, name, var);
    
    entryType = Var_VarTypeReadEntryType(type);
    indexType = Var_VarTypeReadIndexType(type);
    size = Var_VarTypeArrayOrBitvectorReadSize(type);
    
    indexIsEnum = Var_TypeIsEnum(indexType);
    
    if (!indexIsEnum)
      varName = ALLOC(char, strlen(name) + 1 + NumberObtainSize(size) + 1 + 1);

    for (i = 0; i < size; i++) {
      if (indexIsEnum)
        varName = util_strcat4(name, "[", Var_EnumElementReadName(
          Var_VarTypeReadSymbolicValueFromIndex(indexType, i)), "]");
      else 
        sprintf(varName, "%s[%d]", name, i);
      
      ModuleVariablesCreateRecursively(module, varName, entryType, peid);
      if (indexIsEnum)
        FREE(varName);
    }

    if (!indexIsEnum)
      FREE(varName);
    return;
  }

  if (Var_TypeIsBV(type)){
    var = Var_VariableAlloc((VarModuleGeneric) module, name, type);
    Var_VariableSetPEID(var, peid);
    Mdl_ModuleAddVariable(module, name, var);
    
    entryType = Var_TypeManagerReadTypeFromName(PrsTypeManager, "bool");
    size = Var_VarTypeArrayOrBitvectorReadSize(type);
    
    varName = ALLOC(char, strlen(name) + 1 +
                    NumberObtainSize(size) + 1 + 1);
    
    for (i = 0; i < size; i++) {
      sprintf(varName, "%s[%d]", name, i);
      ModuleVariablesCreateRecursively(module, varName, entryType, peid);
    }
    
    FREE(varName);
    return;
  }

  var = Var_VariableAlloc((VarModuleGeneric) module, name, type);
  Var_VariableSetPEID(var, peid);
  Mdl_ModuleAddVariable(module, name, var);
  return;
}


void
AtomAddControlReadAwaitVariableRecursively(
  Mdl_Module_t *module,
  Atm_Atom_t *atom,
  Var_Variable_t *var,
  int code)
{
  char *varName = Var_VariableReadName(var);
  Var_Type_t *varType = Var_VariableReadType(var);
  int i, size;
  char *actualVarName;
  Var_Variable_t *actualVar;

  if (Var_TypeIsArray(varType)) {
    int indexIsEnum = 0;
    Var_Type_t *indexType;
    
    size = Var_VarTypeArrayOrBitvectorReadSize(varType);
    indexType = Var_VarTypeReadIndexType(varType);
    
    indexIsEnum = Var_TypeIsEnum(indexType);

    if (!indexIsEnum)
      actualVarName = ALLOC(char, strlen(varName) + 1 +
                            NumberObtainSize(size) + 1 +
                            1);
    for (i = 0; i < size; i++) {
      if (indexIsEnum)
        actualVarName = util_strcat4(varName, "[", Var_EnumElementReadName(
          Var_VarTypeReadSymbolicValueFromIndex(indexType, i)), "]");
      else
        sprintf(actualVarName, "%s[%d]", varName, i);
      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
      AtomAddControlReadAwaitVariableRecursively(module, atom, actualVar, code);
      if (indexIsEnum)
        FREE(actualVarName);
    }

    if (!indexIsEnum)
      FREE(actualVarName);
    return;
  }

  if (Var_TypeIsBV(varType)) {
    size = Var_VarTypeArrayOrBitvectorReadSize(varType);
    actualVarName = ALLOC(char, strlen(varName) + 1 +
                          NumberObtainSize(size) + 1 +
                          1);
    for (i = 0; i < size; i++) {
      sprintf(actualVarName, "%s[%d]", varName, i);
      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
      AtomAddControlReadAwaitVariableRecursively(module, atom, actualVar, code);
    }

    FREE(actualVarName);
    return;
  }

  switch (code) {
      case 0 : 
      {
        if ((Atm_Atom_t *) Var_VariableReadControlAtom(var) == NIL(Atm_Atom_t)) {
          Atm_AtomAddControlVar(atom,  var);
          Var_VariableSetControlAtom(var, (VarAtomGeneric) atom);
        }
        else {
          PrsParseValid = 0;
          Main_MochaErrorPrint(
            "Error at %d: %s is controlled by another atom", yylineno, varName);
        }
        break;
      }

      case 1 :
      {
        Atm_AtomAddReadVar(atom,  var); 
        Var_VariableAddToReadByAtomList(var, (VarAtomGeneric) atom);
        break;
      }

      case 2 :
      {
        Atm_AtomAddAwaitVar(atom,  var); 
        Var_VariableAddToAwaitByAtomList(var, (VarAtomGeneric) atom);
        break;
      }
  }
  
  return;
}


void
AtomCheckControlReadAwaitVariableRecursively(
  Mdl_Module_t *module,
  Atm_Atom_t *atom,
  Var_Variable_t *var,
  int code)
{

  Var_Type_t *varType = Var_VariableReadType(var);
  char *varName = Var_VariableReadName(var);
  int i, size;
  char *actualVarName;
  Var_Variable_t *actualVar;
  
  if (Var_TypeIsArray(varType)) {

    Var_Type_t *indexType = Var_VarTypeReadIndexType(varType);
    int indexIsEnum = 0;
    
    size = Var_VarTypeArrayOrBitvectorReadSize(varType);
    indexIsEnum = Var_TypeIsEnum(indexType)
;
    
    if (!indexIsEnum)
      actualVarName = ALLOC(char, strlen(varName) + 1 +
                            NumberObtainSize(size) + 1 + 1);
                
    for (i = 0; i < size; i++) {
      if (indexIsEnum)
        actualVarName = util_strcat4(varName, "[", Var_EnumElementReadName(
          Var_VarTypeReadSymbolicValueFromIndex(indexType, i)), "]");
      else
        sprintf(actualVarName, "%s[%d]", varName, i);

      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
      AtomCheckControlReadAwaitVariableRecursively(module, atom, actualVar,
                                                   code);
      if (indexIsEnum)
        FREE(actualVarName);
    }
    
    if (!indexIsEnum)
      FREE(actualVarName);
    return;
  }

  if (Var_TypeIsBV(varType)) {
    size = Var_VarTypeArrayOrBitvectorReadSize(varType);
    actualVarName = ALLOC(char, strlen(varName) + 1 +
                          NumberObtainSize(size) + 1 + 1);
                
    for (i = 0; i < size; i++) {
      sprintf(actualVarName, "%s[%d]", varName, i);
      actualVar = Mdl_ModuleReadVariableFromName(actualVarName, module);
      AtomCheckControlReadAwaitVariableRecursively(module, atom, actualVar,
                                                   code);
    }

    FREE(actualVarName);
    return;
  }

  switch (code) {
      case 0 :
      {
        if (!PrsIsInList(Atm_AtomReadControlVarList(atom), (lsGeneric) var)) {
          Main_MochaErrorPrint(
            "Error at line %d: variable %s not controlled by atom.\n", yylineno, varName);
          PrsParseValid = 0;
        }
        break;
      }

      case 1 :
      {
        if (!PrsIsInList(Atm_AtomReadReadVarList(atom), (lsGeneric) var)) {
          Main_MochaErrorPrint(
            "Error at line %d: variable %s not read by atom.\n", yylineno, varName);
          PrsParseValid = 0;
        }
        break;
      }
      
      case 2 :
      {
        if (!PrsIsInList(Atm_AtomReadAwaitVarList(atom), (lsGeneric) var)) {
          Main_MochaErrorPrint(
            "Error at line %d: variable %s not awaited by atom.\n", yylineno, varName);
          PrsParseValid = 0;
        }
        break;
      }
  }
  return;
}



/**Function********************************************************************

  Synopsis           [This function removes the duplication in the list.]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
PrsDeclaratorListRemoveDuplication(
  lsList declaratorList)
{

  lsSort(declaratorList, PrsNodeCompare);
  lsUniq(declaratorList, PrsNodeCompare, PrsNodeFree);
  
}

/**Function********************************************************************

  Synopsis           [compares two prsNode]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int
PrsNodeCompare(
  PrsNode_t * node1,
  PrsNode_t * node2)
{ int flag;

  /* nodes are compared in lexicographical order */

  /* first node id */
  if (node1->id < node2->id)
    return -1;

  if (node1->id > node2->id)
    return 1;

  /* then field1 */
  flag = strcmp (node1->field1, node2->field1);

  if (flag !=0) {    return flag;
  }
  
  /* then field2 */
  if (node1->id == 0){
    return 0;
  }

  if (node1->id == 1) {
    int numConst1 = (int) (long) PrsNodeReadField2(node1);
    int numConst2 = (int) (long) PrsNodeReadField2(node2);

    if (numConst1 < numConst2)
      return -1;

    return (numConst1 > numConst2);
  }
  
  if (node1->id == 2) {
    return strcmp (node1->field2, node2->field2);
  }
  
  Main_MochaErrorPrint("unknown PrsNode id.\n");
  exit(1);
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static PrsAtomAddVariables(
  lsList varList,
  int code)
{
  
  PrsNode_t *node;
  Var_Variable_t *var;
  lsGen lsgen;
  
  lsForEachItem(varList, lsgen, node){
    if ((var = NodeIsCorrectVarDeclaration(node)) !=
        NIL(Var_Variable_t)) {
      AtomAddControlReadAwaitVariableRecursively(PrsModule,
                                                 PrsAtom, var,
                                                 code);    
    }
    else {
      PrsParseValid = FALSE;
    }
  }
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
PrsAssignmentTypeCheck(
  Atm_Assign_t * assign)
{
  Var_Type_t *expType, *varType;
  Atm_AssignType assignType;
  Var_Variable_t *var;
  Atm_Expr_t **expr;
  boolean flag = TRUE;

  if (!assign) {
    return FALSE;
  }

  assignType = Atm_AssignReadType(assign);
  var = Atm_AssignReadVariable(assign);
  expr = Atm_AssignReadExprAddress(assign);
  varType = Var_VariableReadType(var);
	  
  switch (assignType) {
      case Atm_AssignStd_c : 
        flag =  (Atm_TypeCheckNew(varType, expr));
        break;
        
      case Atm_AssignForall_c: 
        if (Var_TypeIsArray(varType))
          varType = Var_VarTypeReadEntryType(Var_VariableReadType(var));
        else if (Var_TypeIsBV(varType))
          varType = Var_TypeManagerReadTypeFromName(PrsTypeManager, "bool");
        flag =  Atm_TypeCheckNew(varType, expr);
        
        break;

      case Atm_AssignIndex_c:
        Main_MochaErrorPrint("Error:Atm_AssignIndex_c used. \n");
        flag = FALSE;
        break;
        
      case Atm_AssignEvent_c:
        if (!Var_TypeIsEvent(varType)) {
          flag = FALSE;
        }
        break;

      default:
        fprintf(stderr, "assignType not implemented\n");
        assert(0);
        break;
  }
  
  if (!flag) {
    Main_MochaErrorPrint(
      "Error at line %d: type mismatch in assignment to variable \"%s\".\n",
      yylineno,
      Var_VariableReadName(var));
    return FALSE;
  } 

  return TRUE;
}

               
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
PrsForallAssignmentCheckIndexVariable(
  char * dummyVar1,
  char * dummyVar2)
{
  
  if (PrsVariableCheckPrimed(dummyVar1) || PrsVariableCheckPrimed(dummyVar2)) {
    Main_MochaErrorPrint(
      "Error at %d: index variable primed\n", yylineno);
    return FALSE;
  }

  if (strcmp(dummyVar1, dummyVar2)) {
    Main_MochaErrorPrint(
      "Error at %d: index variable mismatch\n", yylineno);
    return FALSE;
  }

  return TRUE;
}


/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static boolean
PrsForallAssignmentCheckDesignatorIsArrayOrBitvector(
  Var_Variable_t * var)
{
  Var_Type_t *type = Var_VariableReadType(var);
  if (!(Var_TypeIsArray(type) || Var_TypeIsBV(type))){
    Main_MochaErrorPrint(
      "Error at %d: variable not an array or bitvector.\n", yylineno);
    return FALSE;
  }
  return TRUE;
}
             

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void 
PrsForallAssignmentCreateDummyVariable(
  char *dummyName,
  Var_Variable_t *arrayOrBitvectorVariable,
  Mdl_Module_t *module,
  Mdl_Manager_t *manager,
  Var_TypeManager_t *typeManager)
{
  char *intString;
  char *indexVarName, *tmp;
  Var_Type_t *varType = Var_VariableReadType(arrayOrBitvectorVariable);
  Var_Type_t *indexType;
  Var_Variable_t *indexVar;
  int peid = 3;

  intString = Main_ConvertIntToString(PrsDummyVarCount);

  tmp = util_strcat3(dummyName, "-", intString);
  indexVarName = Mdl_NamePrefixWithString(Mdl_ModuleReadName(PrsModule),
                                          tmp);
  
  FREE(intString);
  FREE(tmp);

  PrsDummyVarCount++;
                 
  if (Var_TypeIsArray(varType)){
    indexType = Var_VarTypeReadIndexType(varType);
  } else {
    int size = Var_VarTypeArrayOrBitvectorReadSize(varType);

    indexType = PrsTypeRangeAlloc(NIL(char), size, typeManager);
  } 
  
  indexVar = Var_VariableAlloc((VarModuleGeneric) module,
                               indexVarName,
                               indexType);
                 
  Var_VariableSetPEID(indexVar, peid);
  Mdl_ModuleAddVariable(PrsModule, indexVarName, indexVar);
  FREE(indexVarName);
}
               
/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static Atm_Expr_t *
PrsAtmExprCreate (
  Atm_ExprType exprtype,
  Atm_Expr_t *left,
  Atm_Expr_t *right
  )
{

  if (!left || !right){
    Atm_ExprFreeExpression(left);
    Atm_ExprFreeExpression(right);
    return NIL(Atm_Expr_t);
  }
    
  return Atm_ExprAlloc(exprtype, left, right);  
}



/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static void
PrsTimerExpressionCheckBound(
  Atm_Expr_t *left,
  Atm_Expr_t *right)
{
  long NewBound, OldBound;
  Var_Variable_t *TimerVar;
  Var_Type_t *timerType;

  if (left && right) {
    if ( (Atm_ExprReadType((Atm_Expr_t *) left)==Atm_PrimedVar_c) ||
         (Atm_ExprReadType((Atm_Expr_t *) left)==Atm_UnPrimedVar_c) ) {
      TimerVar = (Var_Variable_t *) Atm_ExprReadLeftChild((Atm_Expr_t *)left);
      timerType = Var_VariableReadType(TimerVar);
      if (Var_VariableReadDataType(TimerVar) == Var_Timer_c) {
        if (Atm_ExprReadType((Atm_Expr_t *)right) != Atm_NumConst_c) {
          Main_MochaErrorPrint(
            "Error at line %d: Timers can be compared with integers only \n", yylineno);
        }             
        NewBound = (long) Atm_ExprReadLeftChild((Atm_Expr_t *)right) + 1;
        OldBound = Var_VariableTypeReadNumVals(timerType);
        if (NewBound > OldBound) {
          Var_VariableTypeSetNumVals(timerType,NewBound);
        }
      }
    }
    if ( (Atm_ExprReadType((Atm_Expr_t *) right)==Atm_PrimedVar_c) ||
         (Atm_ExprReadType((Atm_Expr_t *) right)==Atm_UnPrimedVar_c) ) {
      TimerVar = (Var_Variable_t *) Atm_ExprReadLeftChild((Atm_Expr_t *)right);
      timerType = Var_VariableReadType(TimerVar);
      if (Var_VariableReadDataType(TimerVar) == Var_Timer_c) {
        if (Atm_ExprReadType((Atm_Expr_t *)left) != Atm_NumConst_c) {
          Main_MochaErrorPrint("Error at line %d: Timers can be compared with integers only \n", yylineno);
        }             
        NewBound = (long) Atm_ExprReadLeftChild((Atm_Expr_t *)left) + 1;
        OldBound = Var_VariableTypeReadNumVals(timerType);
        if (NewBound > OldBound) {
          Var_VariableTypeSetNumVals(timerType,NewBound);
        }
      }
    }
  }
}
















