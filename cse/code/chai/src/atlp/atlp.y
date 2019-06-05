%{
/**CFile*****************************************************************

  FileName    [atlp.y]

  PackageName [atlp]

  Synopsis    [Yacc for ATL formula parser.]

  SeeAlso     [atlp.h]

  Author      [Freddy Mang]

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

#include  "atlpInt.h"

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

extern int AtlpYylineno;
extern FILE* AtlpYyin;
extern char* AtlpScanText;

static lsList AtlpGlobalAgentList;

/**Variable********************************************************************

  Synopsis    [Global pointer to the ATL formulas being created.]

  Description [This pointer is always set to the current formula
  so that the parser can free a partially constructed ATL formula
  when an error is detected. Every time the parser starts reading
  a new formula, this variable is set to NIL(Atlp_Fromula_t).]

******************************************************************************/
static Atlp_Formula_t *AtlpGlobalFormula;

/**Variable********************************************************************

  Synopsis    [Global pointer to the ATL Manager]

  Description [This pointer is set to the atl manager where new formulas are
  added to.  This pointer is only visible in this parser file.]

******************************************************************************/
static Atlp_Manager_t *AtlpGlobalManager;

/**Variable********************************************************************

  Synopsis    [Global flag for parsing ATL file.]

  Description [This flag is set to 0 before parsing a ATL file commences.  If
  an error is found while parsing the file, this flag is set to 1.  After the
  parser returns, if this flag is set, then the global ATL formula array is
  freed.]

******************************************************************************/
static int AtlpGlobalError;

static lsList AtlpGlobalListOfAgentLists;
static lsList AtlpGlobalListOfFormulaNames;
static char* AtlpGlobalFormulaID;

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
static Atlp_Formula_t *AtlpParsePathFormula(int,int,Atlp_Formula_t*,Atlp_Formula_t*);
static void AtlpParseError();
static int AtlpAgentCmp (Atlp_Agent_t *, Atlp_Agent_t *);
%}

/*---------------------------------------------------------------------------*/
/*      Grammar declarations                                                 */
/*---------------------------------------------------------------------------*/

%union {
  Atlp_Formula_t *sf;	/* state formula */
  char *str;
  lsList list;
  int keyword;
}


%type <sf>   stateformula exp
%type <str>  name formula_id
%type <keyword> path_qtf

%token TOK_AND
%token TOK_COMMA
%token TOK_EQ
%token TOK_EQUIV
%token TOK_ES
%token TOK_EVENTUALLY
%token TOK_EXISTS
%token TOK_FALSE
%token TOK_FINALLY
%token TOK_FORALL
%token TOK_FORMULA
%token TOK_FORMULA_ID
%token TOK_GE
%token TOK_GLOBALLY
%token TOK_GT
%token TOK_ID
%token TOK_ID2
%token TOK_IMPLIES
%token TOK_INVARIANT
%token TOK_LBRAC
%token TOK_LE
%token TOK_LINENO
%token TOK_LLANGLE
%token TOK_LLSQUARE
%token TOK_LSQUARE
%token TOK_LT
%token TOK_MINUS
%token TOK_NA
%token TOK_NATNUM
%token TOK_NOT
%token TOK_ONEXT
%token TOK_OR
%token TOK_PLUS
%token TOK_RBRAC
%token TOK_RRANGLE
%token TOK_RRSQUARE
%token TOK_RSQUARE
%token TOK_SEMICOLON
%token TOK_TRUE
%token TOK_UMINUS
%token TOK_UNTIL
%token TOK_WAIT
%token TOK_XOR
%token TOK_XNEXT

/* precedence is specified here from lowest to highest */
%nonassoc TOK_UNTIL
%nonassoc TOK_GT TOK_GE TOK_LT TOK_LE
%left TOK_IMPLIES
%left TOK_EQUIV
%left TOK_XOR
%left TOK_OR
%left TOK_AND
%nonassoc TOK_GLOBALLY
%nonassoc TOK_FINALLY
%nonassoc TOK_ONEXT
%nonassoc TOK_XNEXT
%nonassoc TOK_WAIT
%nonassoc TOK_NOT
%left TOK_EQ
%left TOK_PLUS TOK_MINUS
%nonassoc TOK_UMINUS

%%

/*---------------------------------------------------------------------------*/
/*      Grammar rules                                                        */
/*---------------------------------------------------------------------------*/
descriptions
	: /* nothing */
	| descriptions description
	;

description
	: formula {}
	| TOK_LINENO TOK_NATNUM TOK_SEMICOLON
	{ 
          AtlpYylineno = atoi(AtlpScanText)-1;
        }
	;


formula      
	:formula_id stateformula TOK_SEMICOLON
	 {
           AtlpFormulaAddToManager(AtlpGlobalManager, $1, $2);
           lsNewEnd(AtlpGlobalListOfFormulaNames,
                    util_strsav($1), LS_NH);
           
         }
	| stateformula TOK_SEMICOLON
	 {
           char *name = AtlpObtainNewFormulaName();
           AtlpFormulaAddToManager(AtlpGlobalManager, name, $1);
           lsNewEnd(AtlpGlobalListOfFormulaNames, util_strsav(name), LS_NH);
           
         }
	| error TOK_SEMICOLON 
	 {
           /* error detected. Free all the global variables */
           /* then skip to the next formula */
           AtlpParseError();
           
	 } 
	;


/* stateformula are those exp having type bool and well-formed */
stateformula  
	: exp
	{ 
	  $$ = $1;
          AtlpGlobalFormula = $$;
	  if (!AtlpFormulaIsWellFormed($$)) {
            Main_MochaErrorPrint(
              "Type mismatch: line %d.\n", AtlpYylineno);
            YYERROR;
          }
        }
	;

exp	
	: name
	  {
            $$ = Atlp_FormulaCreate(Atlp_ID_c,
                                    $1,
                                    NIL(Atlp_Formula_t)
                                    );
            AtlpGlobalFormula = $$;
          } 
	| TOK_LBRAC exp TOK_RBRAC
	  {
            $$ = $2;
            AtlpGlobalFormula = $$;
	  }
	| TOK_TRUE
          {
            $$ = Atlp_FormulaCreate(Atlp_TRUE_c,
                                    NIL(Atlp_Formula_t),
                                    NIL(Atlp_Formula_t)
                                    );
            AtlpGlobalFormula = $$;
          }
	| TOK_FALSE
          {
            $$ = Atlp_FormulaCreate(Atlp_FALSE_c,
                                    NIL(Atlp_Formula_t),
                                    NIL(Atlp_Formula_t)
                                    );
            AtlpGlobalFormula = $$;
          }
	| TOK_NATNUM
          {
            $$ = Atlp_FormulaCreate(Atlp_NATNUM_c,
                                    (Atlp_Formula_t *) util_strsav(AtlpScanText),
                                    NIL(Atlp_Formula_t)
                                    );
            AtlpGlobalFormula = $$;
          }
        | name TOK_LSQUARE exp TOK_RSQUARE
          {
            $$ = Atlp_FormulaCreate(Atlp_INDEX_c,
                                    Atlp_FormulaCreate(Atlp_ID_c,
                                                       (Atlp_Formula_t *) $1,
                                                       NIL(Atlp_Formula_t)),
                                    $3);
            AtlpGlobalFormula = $$;
          }
	| exp TOK_GE exp
          {
            $$ = Atlp_FormulaCreate(Atlp_GE_c, $1, $3);
            AtlpGlobalFormula = $$;
          }
	| exp TOK_LE exp
          {
            $$ = Atlp_FormulaCreate(Atlp_LE_c, $1, $3);
            AtlpGlobalFormula = $$;
          }
	| exp TOK_GT exp
          {
            $$ = Atlp_FormulaCreate(Atlp_GT_c, $1, $3);
            AtlpGlobalFormula = $$;
          }
	| exp TOK_LT exp
          {
            $$ = Atlp_FormulaCreate(Atlp_LT_c, $1, $3);
            AtlpGlobalFormula = $$;
          }
	| exp TOK_AND exp
	  {
            $$ = Atlp_FormulaCreate(Atlp_AND_c, $1, $3);
            AtlpGlobalFormula = $$;
	  }
	| exp TOK_OR exp
	  {
            $$ = Atlp_FormulaCreate(Atlp_OR_c, $1, $3);
            AtlpGlobalFormula = $$;
	  }
	| exp TOK_XOR exp
	  {
            $$ = Atlp_FormulaCreate(Atlp_XOR_c, $1, $3);
            AtlpGlobalFormula = $$;
	  }
	| exp TOK_EQUIV exp
	  {
            $$ = Atlp_FormulaCreate(Atlp_EQUIV_c, $1, $3);
            AtlpGlobalFormula = $$;
	  }
	| exp TOK_IMPLIES exp
	  {
            $$ = Atlp_FormulaCreate(Atlp_IMPLIES_c, $1, $3);
            AtlpGlobalFormula = $$;
	  }
	| exp TOK_EQ exp
	  {
            $$ = Atlp_FormulaCreate(Atlp_EQ_c, $1, $3);
            AtlpGlobalFormula = $$;
	  }
	| TOK_NOT exp
	  {
            $$ = Atlp_FormulaCreate(Atlp_NOT_c, $2, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = $$;
	  }
	| path_qtf TOK_GLOBALLY exp
	  {
            $$ = AtlpParsePathFormula (
              $1, TOK_GLOBALLY, $3, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = $$;
	  }
	| path_qtf TOK_FINALLY exp
	  {
            $$ = AtlpParsePathFormula (
              $1, TOK_FINALLY, $3, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = $$;
	  }
        | path_qtf TOK_ONEXT exp
	  {
            $$ = AtlpParsePathFormula (
              $1, TOK_ONEXT, $3, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = $$;
	  }
        | path_qtf TOK_XNEXT exp
	  {
            $$ = AtlpParsePathFormula (
              $1, TOK_XNEXT, $3, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = $$;
	  }
	| path_qtf TOK_LBRAC exp TOK_UNTIL exp TOK_RBRAC
	  {
            $$ = AtlpParsePathFormula (
              $1, TOK_UNTIL, $3, $5);
            AtlpGlobalFormula = $$;
	  }
	| path_qtf TOK_LBRAC exp TOK_WAIT exp TOK_RBRAC
	  {
            $$ = AtlpParsePathFormula (
              $1, TOK_WAIT, $3, $5);
            AtlpGlobalFormula = $$;
	  }
	| exp TOK_PLUS exp
          {
            $$ = Atlp_FormulaCreate(Atlp_PLUS_c, $1, $3);
            AtlpGlobalFormula = $$;
          }        
	| exp TOK_MINUS exp
          {
            $$ = Atlp_FormulaCreate(Atlp_MINUS_c, $1, $3);
            AtlpGlobalFormula = $$;
          }
	| TOK_MINUS exp %prec TOK_UMINUS
          {
            $$ = Atlp_FormulaCreate(Atlp_UMINUS_c, $2, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = $$;
          }	
	;


path_qtf
	: TOK_FORALL
	  {
            lsNewBegin(AtlpGlobalListOfAgentLists,
                       (lsGeneric) lsCreate(), LS_NH);
	    $$ = TOK_ES; /* the exists a strategy for an empty set of agents,
                            meaning whether or not the agents cooperate */
	  }
	| TOK_EXISTS
	  {
            lsNewBegin(AtlpGlobalListOfAgentLists,
                       (lsGeneric) lsCreate(), LS_NH);
	    $$ = TOK_NA; /* the empty set of agents cannot avoid, equiv to
                            the full set of agents can cooperate to  */
                            
            
	  }
	| TOK_LLANGLE name_or TOK_RRANGLE
	  {
            lsList agentList;

            lsFirstItem(AtlpGlobalListOfAgentLists,
                        (lsGeneric *) &agentList, LS_NH);
	    lsSort(agentList, AtlpAgentCmp);
            lsUniq(agentList, AtlpAgentCmp, Atlp_AgentFree); 
	    $$ = TOK_ES; /* there Exists a Strategy */
	  }
	| TOK_LLSQUARE name_or TOK_RRSQUARE
	  {
            lsList agentList;

            lsFirstItem(AtlpGlobalListOfAgentLists,
                        (lsGeneric *) &agentList, LS_NH);
	    lsSort(agentList, AtlpAgentCmp);
            lsUniq(agentList, AtlpAgentCmp, Atlp_AgentFree); 
	    $$ = TOK_NA; /* agents canNot Avoid */ 
	  }
	;


formula_id
	: TOK_FORMULA_ID
	 {
	   char *tmp1 = util_strsav(AtlpScanText);
	   int   len = strlen(tmp1);
	   char *tmp;

	   tmp = ALLOC(char, len-1);
	   memcpy(tmp, tmp1+1, len-2);
	   *(tmp+len-2) = '\0';

	   FREE(tmp1);
	   $$ = tmp;

           AtlpGlobalFormulaID = $$;
         }


/* all possible names */
name
	: TOK_ID
	  { 
	    $$ = util_strsav(AtlpScanText);
	  }
     	| TOK_ID2
	  { 
	    $$ = util_strsav(AtlpScanText);
	  }
	| TOK_FORALL
	  {
            $$ = util_strsav("A");
          }
	| TOK_EXISTS
	  {
            $$ = util_strsav("E");
          }
	| TOK_GLOBALLY
	  {
            $$ = util_strsav("G");
          }
	| TOK_FINALLY
	  {
            $$ = util_strsav("F");
          }
	| TOK_ONEXT
	  {
            $$ = util_strsav("O");
          }
	| TOK_XNEXT
	  {
            $$ = util_strsav("X");
          }
	| TOK_UNTIL
	  {
            $$ = util_strsav("U");
          }
	| TOK_WAIT
	  {
            $$ = util_strsav("W");
          }
	;


/* name_or is only used in specifying the agents, so 
/* it is fine to create the agents here */

name_or
	: /* nothing */
	{
          lsNewBegin(AtlpGlobalListOfAgentLists,
                     (lsGeneric) lsCreate(), LS_NH);
	}
	|  name
	 {
           lsList tmp = lsCreate();
           Atlp_Agent_t * agent = Atlp_AgentAllocWithName($1);
           lsNewEnd(tmp, (lsGeneric) agent, LS_NH);
           lsNewBegin(AtlpGlobalListOfAgentLists,
                      (lsGeneric) tmp, LS_NH);
           FREE($1);
         }  
	|  name_or TOK_COMMA name
         {
           lsList tmp;
           Atlp_Agent_t * agent = Atlp_AgentAllocWithName($3);
           lsFirstItem(AtlpGlobalListOfAgentLists,
                       (lsGeneric *) &tmp, LS_NH);
           
           lsNewEnd(tmp, (lsGeneric) agent, LS_NH);
           FREE($3);
	 }
	;


	

%%
static Atlp_Formula_t *AtlpParsePathFormula(
  int typeOfPathQuantifier,
  int typeOfTemporalOperator,
  Atlp_Formula_t *formula1,
  Atlp_Formula_t *formula2
  )
{
  
  Atlp_Formula_t *formula;
  Atlp_FormulaType formulaType;
  lsList agentList;
  
  switch (typeOfTemporalOperator) {
      case TOK_GLOBALLY:
        if (typeOfPathQuantifier == TOK_ES) formulaType = Atlp_ESG_c;
        else if (typeOfPathQuantifier == TOK_NA) formulaType = Atlp_NAG_c;
        else {
          fprintf (stderr, "unknown type in parsing."); exit(1);
        }
        break;
      case TOK_FINALLY:
        if (typeOfPathQuantifier == TOK_ES) formulaType = Atlp_ESF_c;
        else if (typeOfPathQuantifier == TOK_NA) formulaType = Atlp_NAF_c;
        else {
          fprintf (stderr, "unknown type in parsing."); exit(1);
        }
        break;
      case TOK_ONEXT:
        if (typeOfPathQuantifier == TOK_ES) formulaType = Atlp_ESX_c;
        else if (typeOfPathQuantifier == TOK_NA) formulaType = Atlp_NAX_c;
        else {
          fprintf (stderr, "unknown type in parsing."); exit(1);
        }
        break;
      case TOK_XNEXT:
        if (typeOfPathQuantifier == TOK_ES) formulaType = Atlp_ESX_c;
        else if (typeOfPathQuantifier == TOK_NA) formulaType = Atlp_NAX_c;
        else {
          fprintf (stderr, "unknown type in parsing."); exit(1);
        }
        break;
      case TOK_UNTIL:
        if (typeOfPathQuantifier == TOK_ES) formulaType = Atlp_ESU_c;
        else if (typeOfPathQuantifier == TOK_NA) formulaType = Atlp_NAU_c;
        else {
          fprintf (stderr, "unknown type in parsing."); exit(1);
        }
        break;
      case TOK_WAIT:
        if (typeOfPathQuantifier == TOK_ES) formulaType = Atlp_ESW_c;
        else if (typeOfPathQuantifier == TOK_NA) formulaType = Atlp_NAW_c;
        else {
          fprintf (stderr, "unknown type in parsing."); exit(1);
        }
        break;
      default:
        fprintf (stderr, "unknown type in parsing."); exit(1);
  }
  
  lsDelBegin(AtlpGlobalListOfAgentLists, (lsGeneric *) &agentList);
  
  formula =  Atlp_FormulaCreateWithAgents(formulaType,
                                          formula1, formula2,
                                          agentList
                                          );

  return formula;
  
}
  
lsList AtlpParse(
  Atlp_Manager_t* manager)
{
  
  AtlpGlobalFormula = NIL(Atlp_Formula_t);
  AtlpGlobalError = 0;
  AtlpGlobalManager = manager;
  AtlpGlobalListOfAgentLists = lsCreate();
  AtlpGlobalListOfFormulaNames = lsCreate();
  AtlpYylineno = 1;
  
  AtlpYyrestart(AtlpYyin);

  AtlpYyparse();

  lsDestroy(AtlpGlobalListOfAgentLists, (void (*) () ) 0);
  return AtlpGlobalListOfFormulaNames;

}


int yyerror () {
  Main_MochaErrorPrint("Error at %d\n", AtlpYylineno);
}

static void AtlpParseError () 
{
  lsGen gen;
  lsList agentList;
  

  Atlp_FormulaFree(AtlpGlobalFormula);
  lsForEachItem(AtlpGlobalListOfAgentLists, gen, agentList) {
    lsDestroy (agentList, Atlp_AgentFree);
  }
  FREE(AtlpGlobalFormulaID);
  Main_MochaErrorPrint("Invalid ATL formula, line %d\n\n",
                       AtlpYylineno);

  AtlpGlobalFormula = NIL(Atlp_Formula_t);

}

static int
AtlpAgentCmp (
  Atlp_Agent_t * agent1,
  Atlp_Agent_t * agent2
  ) 
{
  return strcmp (agent1->name, agent2->name);
}

