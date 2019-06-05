%{
/**CFile***********************************************************************

  FileName    [atlp.l]

  PackageName [ctlp]

  Synopsis    [Lexical analyzer for ATL formula parser. See atlp.h for syntax.]

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

  Revision    [$Id $]

******************************************************************************/
#include  "atlpInt.h"
#include  "atlpTokens.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/
char* AtlpScanText;

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/**AutomaticEnd***************************************************************/



/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/
void AtlpSetScanText();

%}

/*---------------------------------------------------------------------------*/
/*      Lexical analyzer rules                                               */
/*---------------------------------------------------------------------------*/

%option noyywrap
%option lex-compat


DIGIT		[0-9]
ALPHA		[a-zA-Z]
NATNUM		{DIGIT}+
ID		{ALPHA}({ALPHA}|{DIGIT})*(_({ALPHA}|{DIGIT})+)*'?
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

[F|f]ormula { return (TOK_FORMULA);}
[I|i]nvariant {return (TOK_INVARIANT);}
\#LINENO {return (TOK_LINENO);}

A	{ return(TOK_FORALL); }
E	{ return(TOK_EXISTS); }
\<\<	{ return(TOK_LLANGLE); }
\>\>	{ return(TOK_RRANGLE); }
\[\[    { return(TOK_LLSQUARE); }
\]\]    { return(TOK_RRSQUARE); }

\<\>    |
F	{ return (TOK_FINALLY); }

\[\]    |
G	{ return (TOK_GLOBALLY); }

O	{ return (TOK_ONEXT);}
X	{ return (TOK_XNEXT);}

U	{ return(TOK_UNTIL); }

W       { return(TOK_WAIT); }


TRUE    |
true	{ return(TOK_TRUE); }

FALSE   |
false	{ return(TOK_FALSE); }

\[      { return(TOK_LSQUARE); }
\]      { return(TOK_RSQUARE); }


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

{NATNUM}	        { AtlpSetScanText(); return(TOK_NATNUM);}
{FormulaID}		{ AtlpSetScanText(); return(TOK_FORMULA_ID);}
{ID}+                   { AtlpSetScanText(); return(TOK_ID); }
{ID}+\[[0-9]+\]         { AtlpSetScanText(); return(TOK_ID2); }

.       {}
%%

void AtlpSetScanText() {

  if (AtlpScanText != NIL(char)) {
    FREE(AtlpScanText);
  }

  AtlpScanText = util_strsav(AtlpYytext);
    
}




