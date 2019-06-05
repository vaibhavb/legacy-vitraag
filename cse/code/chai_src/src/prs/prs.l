%{
#include <stdio.h> 
#include <math.h> 
#include  "prsInt.h"
#include  "prsTokens.h"

extern YYSTYPE yylval;

%}

%option yylineno


DIGIT		[0-9]
ALPHA		[a-zA-Z&]
NATNUM		{DIGIT}+
ID		{ALPHA}({ALPHA}|{DIGIT})*((_({ALPHA}|{DIGIT})|"."{ALPHA})({ALPHA}|{DIGIT})*)*
WHITESPACE	[ \t\f\b\r\v]
NEWLINE		[\n]
BLANKLINE	{WHITESPACE}*{NEWLINE}

%x COMMENT COMMENT1

%%


"--"		{BEGIN (COMMENT);}
<COMMENT>[^\n]*	
<COMMENT>\n	{BEGIN (INITIAL);}

"/*"              {BEGIN (COMMENT1);}
<COMMENT1>"*"+"/" {BEGIN (INITIAL);}
<COMMENT1>\n
<COMMENT1>.


\#LINENO        { return (TOK_LINENO);}
atom		{ return (ATOM);}
endatom		{ return (ENDATOM);}
lazy		{ return (LAZY);}
event		{ return (EVENT);}
controls        { return (CONTROLS);}
reads		{ return (READS);}
awaits		{ return (AWAITS);}
init		{ return (INIT);}
update		{ return (UPDATE);}
wait            { return (WAIT);}
timer           { return (TIMER);}
clock           { return (TIMER);}

module		{ return (MODULE);}
endmodule	{ return (ENDMODULE);}
private		{ return (PRIVATE);}
interface	{ return (INTERFACE);}
external	{ return (EXTERNAL);}
"||"		{ return (PAR);}
hide		{ return (HIDE);}
in		{ return (IN);}
endhide		{ return (ENDHIDE);}

event		{ return (EVENT); }
array           { return (ARRAY); }
bitvector       { return (BITVECTOR);}
bitvector_n     { return (BITVECTOR_N);}
forall		{ return (FORALL);}

"<=>"		{ return (EQUIV); }
"=>"		{ return (IMPLY); }

"[]"		{ return (CHOICE); }
"->"		{ return (ARROW); }


true		{yylval.bool = 1; return (BOOL_CONST);}
false		{yylval.bool = 0; return (BOOL_CONST);}
if		{return (IF);}
then		{return (THEN);}
else		{return (ELSE);}	
fi		{return (FI);}
of              {return (OF);}
type		{return (TYPEDEF);}
default         {return (DEFAULT);}
nondet          {return (NONDET);}
"!"		{return ('!');}
"?"		{return ('?');}
"("		{return ('(');}
")"		{return (')');}
"{"		{return ('{');}
"}"		{return ('}');}
"["		{return ('[');}
"]"		{return (']');}
":="		{return (ASSIGN);}
":<="           {return (LASSIGN);}
"&"		{return (AND);}
"|"		{return (OR);}
"~"		{return (NOT);}
"^"             {return (XOR);}
">"		{return (GT);}
"<"		{return (LT);}
">="		{return (GE);}	
"<="		{return (LE);}
"="		{return (EQ);}
"!="            {return (NEQ);}
"+"		{return ('+');}
"-"		{return ('-');}
"%"             {return ('%');}
";"		{return (';');}
":"		{return (':');}
","		{return (',');}
".."		{return (ELLIPSE);}
"."             {return ('.');}



{ID}"\'"?	{yylval.name.ptr = yytext; yylval.name.length = yyleng; return (IDENTIFIER);}
{ID}"?"		{yylval.name.ptr = yytext; yylval.name.length = yyleng - 1; return(EVENTQUERY);}
{ID}"!"		{yylval.name.ptr = yytext; yylval.name.length = yyleng - 1; return(EVENTSEND);}
{NATNUM}	{yylval.val = atoi(yytext); return (NATNUM);}
{WHITESPACE}	|
{BLANKLINE}	

.               {return (ERROR);}

%%


