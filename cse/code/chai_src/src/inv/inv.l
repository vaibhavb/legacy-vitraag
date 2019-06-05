%{
/**CFile***********************************************************************

  FileName    [Inv.l]

  PackageName [Inv]

  Synopsis    [Lexical analyzer for invariant parser. See Inv.h for syntax.]

  Author      [Freddy Mang and Shaz Qadeer]

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

  Revision    [$Id $]

******************************************************************************/
#include  "invInt.h"
#include  "invTokens.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
char* InvScanText;

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/**AutomaticEnd***************************************************************/



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
void InvSetScanText();

%}

/*---------------------------------------------------------------------------*/
/*      Lexical analyzer rules                                               */
/*---------------------------------------------------------------------------*/

%option noyywrap
%option lex-compat


DIGIT		[0-9]
ALPHA		[a-zA-Z&]
NATNUM		{DIGIT}+
ID		{ALPHA}({ALPHA}|{DIGIT})*(_({ALPHA}|{DIGIT})+)*
ID1		({ALPHA}|{DIGIT}|_|\.|\/|\[|\])+
WHITESPACE	[ \t\f\b\r\v]
NEWLINE		[\n]
BLANKLINE	{WHITESPACE}*{NEWLINE}
FormulaID	\"([^\"])*\"

%x COMMENT COMMENT1

%%

"--"		{BEGIN (COMMENT);}
<COMMENT>[^\n]*	
<COMMENT>\n	{BEGIN (INITIAL);}

"/*"              {BEGIN (COMMENT1);}
<COMMENT1>"*"+"/" {BEGIN (INITIAL);}
<COMMENT1>\n
<COMMENT1>.

{WHITESPACE}	|
{BLANKLINE}	{}

\#LINENO {return (TOK_LINENO);}


\[      { return(TOK_LSQUARE); }
\]      { return(TOK_RSQUARE); }

TRUE    |
true	{ return(TOK_TRUE); }

FALSE   |
false	{ return(TOK_FALSE); }

\|	{ return(TOK_OR); }
\&	{ return(TOK_AND); }
"~"	{ return(TOK_NOT); }
"^"	{ return(TOK_XOR); }
"=>"	{ return(TOK_IMPLIES); }
"<=>"	{ return(TOK_EQUIV); }
"="     { return(TOK_EQ); }


"("	{ return(TOK_LBRAC); }
")"	{ return(TOK_RBRAC); }
">"	{ return (TOK_GT);}
"<"	{ return (TOK_LT);}
">="	{ return (TOK_GE);}	
"<="	{ return (TOK_LE);}
"+"	{ return (TOK_PLUS);}
"-"	{ return (TOK_MINUS);}

\,      { return(TOK_COMMA); }


";"	{ return (TOK_SEMICOLON); }

{NATNUM}	        { InvSetScanText(); return(TOK_NATNUM);}
{FormulaID}		{ InvSetScanText(); return(TOK_FORMULA_ID);}
{ID1}"'"?               { InvSetScanText(); return(TOK_ID);
			   /* Ashwini: Implement primed ID's */} 
{ID}+\[[0-9]+\]         { InvSetScanText(); return(TOK_ID2); }

.       {}
%%

void InvSetScanText() {

  if (InvScanText != NIL(char)) {
    FREE(InvScanText);
  }

  InvScanText = util_strsav(InvYytext);
    
}





