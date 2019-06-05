%{
/**CFile*****************************************************************

  FileName    [Inv.y]

  PackageName [Inv]

  Synopsis    [Yacc for invariant parser.]

  SeeAlso     [Inv.h]

  Author      [Shaz Qadeer]

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

#include  "invInt.h"

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

extern int InvYylineno;
extern FILE* InvYyin;
extern char* InvScanText;

/**Variable********************************************************************

  Synopsis    [Global pointer to the invariant being created.]

  Description [This pointer is always set to the current formula
  so that the parser can free a partially constructed ATL formula
  when an error is detected. Every time the parser starts reading
  a new formula, this variable is set to NIL(Inv_Fromula_t).]

******************************************************************************/
static InvExpr_t *InvGlobalFormula;

/**Variable********************************************************************

  Synopsis    [Global pointer to the invariant manager]

  Description [This pointer is set to the atl manager where new formulas are
  added to.  This pointer is only visible in this parser file.]

******************************************************************************/
static Inv_Manager_t *InvGlobalManager;

/**Variable********************************************************************

  Synopsis    [Global flag for parsing invariant file.]

  Description [This flag is set to 0 before parsing a invariant file commences. 
  If an error is found while parsing the file, this flag is set to 1.  After the
  parser returns, if this flag is set, then the global invariant array is
  freed.]

******************************************************************************/
static int InvGlobalError;

static lsList InvGlobalListOfFormulaNames;
static char* InvGlobalFormulaID;

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
static void ParseError();
%}

/*---------------------------------------------------------------------------*/
/*      Yacc declarations                                                    */
/*---------------------------------------------------------------------------*/

%union {
  InvExpr_t *sf;	/* state formula */
  char *str;
  lsList list;
  int keyword;
}


%type <sf>   stateformula exp
%type <str>  name formula_id

%token TOK_AND
%token TOK_COMMA
%token TOK_EQ
%token TOK_EQUIV
%token TOK_FALSE
%token TOK_FORMULA
%token TOK_FORMULA_ID
%token TOK_GE
%token TOK_GT
%token TOK_ID
%token TOK_ID2
%token TOK_IMPLIES
%token TOK_LBRAC
%token TOK_LE
%token TOK_LINENO
%token TOK_LSQUARE
%token TOK_LT
%token TOK_MINUS
%token TOK_NA
%token TOK_NATNUM
%token TOK_NEXT
%token TOK_NOT
%token TOK_OR
%token TOK_PLUS
%token TOK_RBRAC
%token TOK_RSQUARE
%token TOK_SEMICOLON
%token TOK_TRUE
%token TOK_UMINUS
%token TOK_XOR

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
%nonassoc TOK_NEXT
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
          InvYylineno = atoi(InvScanText)-1;
        }
	;


formula      
	: formula_id stateformula TOK_SEMICOLON
	 {
	   Inv_Invariant_t *inv = InvInvariantAlloc($1, $2);
           InvFormulaAddToManager(InvGlobalManager, $1, inv);
           lsNewEnd(InvGlobalListOfFormulaNames,
                    util_strsav($1), LS_NH);
         }
	| stateformula TOK_SEMICOLON
	 {
           char *name = InvObtainNewFormulaName();
	   Inv_Invariant_t *inv = InvInvariantAlloc(name, $1);
           InvFormulaAddToManager(InvGlobalManager, name, inv);
           lsNewEnd(InvGlobalListOfFormulaNames,
                    util_strsav(name), LS_NH);
         }
	| error TOK_SEMICOLON 
	 {
           /* error detected. Free all the global variables */
           /* then skip to the next formula */
           ParseError();
           
	 } 
	;


/* stateformula are those exp having type bool and well-formed */
stateformula  
	: exp
	{ 
	  $$ = $1;
          InvGlobalFormula = $$;
	  if (!InvExprIsWellFormed($$)) {
            Main_MochaErrorPrint(
              "Type mismatch: line %d.\n", InvYylineno);
            YYERROR;
          }
        }
	;

exp	
	: name
          {
	     $$ = InvExprAlloc(InvId_c, (InvExpr_t *) $1, NIL(InvExpr_t));
	     InvGlobalFormula = $$;
          } 
	| TOK_LBRAC exp TOK_RBRAC
	  {
            $$ = $2;
            InvGlobalFormula = $$;
	  }
	| TOK_TRUE
          {
            $$ = InvExprAlloc(InvBoolConst_c, (InvExpr_t *) 1,
			       NIL(InvExpr_t));
            InvGlobalFormula = $$;
          }
	| TOK_FALSE
          {
            $$ = InvExprAlloc(InvBoolConst_c, (InvExpr_t *) 0,
			       NIL(InvExpr_t));
            InvGlobalFormula = $$;
          }
	| TOK_NATNUM
          {
            $$ = InvExprAlloc(InvNumConst_c, (InvExpr_t *) (long) atoi(InvScanText),
			       NIL(InvExpr_t));
            InvGlobalFormula = $$;
          }
        | name TOK_LSQUARE exp TOK_RSQUARE
          {
            $$ = InvExprAlloc(InvIndex_c, InvExprAlloc(InvId_c, (InvExpr_t *)
                                                       $1, NIL(InvExpr_t)), $3);
            InvGlobalFormula = $$;
          }
        | exp TOK_GE exp
          {
            $$ = InvExprAlloc(InvGreaterEqual_c, $1, $3);
            InvGlobalFormula = $$;
          }
	| exp TOK_LE exp
          {
            $$ = InvExprAlloc(InvLessEqual_c, $1, $3);
            InvGlobalFormula = $$;
          }
	| exp TOK_GT exp
          {
            $$ = InvExprAlloc(InvGreater_c, $1, $3);
            InvGlobalFormula = $$;
          }
	| exp TOK_LT exp
          {
            $$ = InvExprAlloc(InvLess_c, $1, $3);
            InvGlobalFormula = $$;
          }
	| exp TOK_AND exp
	  {
            $$ = InvExprAlloc(InvAnd_c, $1, $3);
            InvGlobalFormula = $$;
	  }
	| exp TOK_OR exp
	  {
            $$ = InvExprAlloc(InvOr_c, $1, $3);
            InvGlobalFormula = $$;
	  }
	| exp TOK_EQUIV exp
	  {
            $$ = InvExprAlloc(InvEquiv_c, $1, $3);
            InvGlobalFormula = $$;
	  }
	| exp TOK_IMPLIES exp
	  {
            $$ = InvExprAlloc(InvImplies_c, $1, $3);
            InvGlobalFormula = $$;
	  }
	| exp TOK_XOR exp
	  {
	    /* Ashwini: Added rule for TOK_XOR */
            $$ = InvExprAlloc(InvXor_c, $1, $3);
            InvGlobalFormula = $$;
	  }
	| exp TOK_EQ exp
	  {
            $$ = InvExprAlloc(InvEqual_c, $1, $3);
            InvGlobalFormula = $$;
	  }
	| TOK_NOT exp
	  {
            $$ = InvExprAlloc(InvNot_c, $2, NIL(InvExpr_t));
            InvGlobalFormula = $$;
	  }
	| exp TOK_PLUS exp
          {
            $$ = InvExprAlloc(InvPlus_c, $1, $3);
            InvGlobalFormula = $$;
          }        
	| exp TOK_MINUS exp
          {
            $$ = InvExprAlloc(InvMinus_c, $1, $3);
            InvGlobalFormula = $$;
          }
	| TOK_MINUS exp %prec TOK_UMINUS
          {
            $$ = InvExprAlloc(InvUnaryMinus_c, $2, NIL(InvExpr_t));
            InvGlobalFormula = $$;
          }	
	;


formula_id
	: TOK_FORMULA_ID
	 {
	   char *tmp1 = util_strsav(InvScanText);
	   int   len = strlen(tmp1);
	   char *tmp;

	   tmp = ALLOC(char, len-1);
	   memcpy(tmp, tmp1+1, len-2);
	   *(tmp+len-2) = '\0';

	   FREE(tmp1);
	   $$ = tmp;

           InvGlobalFormulaID = $$;
         }
        ;

name
	: TOK_ID
	  { 
	    $$ = util_strsav(InvScanText);
	  }
	;
/*
     	| TOK_ID2
	  { 
	    $$ = util_strsav(InvScanText);
	  }
	;
*/

%%
	  
lsList InvParse(
  Inv_Manager_t* manager)
{
  
  InvGlobalFormula = NIL(InvExpr_t);
  InvGlobalError = 0;
  InvGlobalManager = manager;
  InvGlobalListOfFormulaNames = lsCreate();
  InvYylineno = 1;
  
  InvYyrestart(InvYyin);

  InvYyparse();

  return InvGlobalListOfFormulaNames;

}


int yyerror () {
  fprintf(stderr, "error at %d\n", InvYylineno);
}

static void ParseError () 
{
  lsGen gen;

  InvExprFree(InvGlobalFormula);
  FREE(InvGlobalFormulaID);
  Main_MochaErrorPrint("Invalid Invariant, line %d\n\n",
                       InvYylineno);

  InvGlobalFormula = NIL(InvExpr_t);

}
