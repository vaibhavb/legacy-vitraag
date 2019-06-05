/* A Bison parser, made from ./src/inv/inv.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse InvYyparse
#define yylex InvYylex
#define yyerror InvYyerror
#define yylval InvYylval
#define yychar InvYychar
#define yydebug InvYydebug
#define yynerrs InvYynerrs
# define	TOK_AND	257
# define	TOK_COMMA	258
# define	TOK_EQ	259
# define	TOK_EQUIV	260
# define	TOK_FALSE	261
# define	TOK_FORMULA	262
# define	TOK_FORMULA_ID	263
# define	TOK_GE	264
# define	TOK_GT	265
# define	TOK_ID	266
# define	TOK_ID2	267
# define	TOK_IMPLIES	268
# define	TOK_LBRAC	269
# define	TOK_LE	270
# define	TOK_LINENO	271
# define	TOK_LSQUARE	272
# define	TOK_LT	273
# define	TOK_MINUS	274
# define	TOK_NA	275
# define	TOK_NATNUM	276
# define	TOK_NEXT	277
# define	TOK_NOT	278
# define	TOK_OR	279
# define	TOK_PLUS	280
# define	TOK_RBRAC	281
# define	TOK_RSQUARE	282
# define	TOK_SEMICOLON	283
# define	TOK_TRUE	284
# define	TOK_UMINUS	285
# define	TOK_XOR	286
# define	TOK_UNTIL	287
# define	TOK_GLOBALLY	288
# define	TOK_FINALLY	289

#line 1 "./src/inv/inv.y"

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

#line 92 "./src/inv/inv.y"
#ifndef YYSTYPE
typedef union {
  InvExpr_t *sf;	/* state formula */
  char *str;
  lsList list;
  int keyword;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 1
#endif



#define	YYFINAL		56
#define	YYFLAG		-32768
#define	YYNTBASE	36

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 289 ? yytranslate[x] : 43)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     6,    10,    14,    17,    20,    22,
      24,    28,    30,    32,    34,    39,    43,    47,    51,    55,
      59,    63,    67,    71,    75,    79,    82,    86,    90,    93,
      95
};
static const short yyrhs[] =
{
      -1,    36,    37,     0,    38,     0,    17,    22,    29,     0,
      41,    39,    29,     0,    39,    29,     0,     1,    29,     0,
      40,     0,    42,     0,    15,    40,    27,     0,    30,     0,
       7,     0,    22,     0,    42,    18,    40,    28,     0,    40,
      10,    40,     0,    40,    16,    40,     0,    40,    11,    40,
       0,    40,    19,    40,     0,    40,     3,    40,     0,    40,
      25,    40,     0,    40,     6,    40,     0,    40,    14,    40,
       0,    40,    32,    40,     0,    40,     5,    40,     0,    24,
      40,     0,    40,    26,    40,     0,    40,    20,    40,     0,
      20,    40,     0,     9,     0,    12,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   156,   157,   161,   162,   170,   177,   185,   197,   210,
     215,   220,   226,   232,   238,   244,   249,   254,   259,   264,
     269,   274,   279,   284,   290,   295,   300,   305,   310,   319,
     337
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "TOK_AND", "TOK_COMMA", "TOK_EQ", 
  "TOK_EQUIV", "TOK_FALSE", "TOK_FORMULA", "TOK_FORMULA_ID", "TOK_GE", 
  "TOK_GT", "TOK_ID", "TOK_ID2", "TOK_IMPLIES", "TOK_LBRAC", "TOK_LE", 
  "TOK_LINENO", "TOK_LSQUARE", "TOK_LT", "TOK_MINUS", "TOK_NA", 
  "TOK_NATNUM", "TOK_NEXT", "TOK_NOT", "TOK_OR", "TOK_PLUS", "TOK_RBRAC", 
  "TOK_RSQUARE", "TOK_SEMICOLON", "TOK_TRUE", "TOK_UMINUS", "TOK_XOR", 
  "TOK_UNTIL", "TOK_GLOBALLY", "TOK_FINALLY", "descriptions", 
  "description", "formula", "stateformula", "exp", "formula_id", "name", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    36,    36,    37,    37,    38,    38,    38,    39,    40,
      40,    40,    40,    40,    40,    40,    40,    40,    40,    40,
      40,    40,    40,    40,    40,    40,    40,    40,    40,    41,
      42
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     1,     3,     3,     2,     2,     1,     1,
       3,     1,     1,     1,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     3,     2,     1,
       1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     0,     0,    12,    29,    30,     0,     0,     0,    13,
       0,    11,     2,     3,     0,     8,     0,     9,     7,     0,
       0,    28,    25,     6,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    10,     4,
      19,    24,    21,    15,    17,    22,    16,    18,    27,    20,
      26,    23,     5,     0,    14,     0,     0
};

static const short yydefgoto[] =
{
       1,    12,    13,    14,    15,    16,    17
};

static const short yypact[] =
{
  -32768,    32,   -26,-32768,-32768,-32768,    31,   -17,    31,-32768,
      31,-32768,-32768,-32768,   -23,   103,    31,   -10,-32768,    54,
     -20,-32768,    43,-32768,    31,    31,    31,    31,    31,    31,
      31,    31,    31,    31,    31,    31,   -18,    31,-32768,-32768,
      43,   -19,   131,   127,   127,    10,   127,   127,-32768,    98,
  -32768,   145,-32768,    79,-32768,    12,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,    -2,    -6,-32768,-32768
};


#define	YYLAST		171


static const short yytable[] =
{
      19,    32,    21,    18,    22,    20,    23,    34,    37,    39,
       0,    52,    56,    24,    36,    25,    26,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      32,    53,    55,     2,     0,    33,    34,     0,     3,     3,
       0,     4,    35,     5,     5,     0,     6,     6,    25,     7,
       0,     8,     8,     9,     9,    10,    10,    24,     0,    25,
      26,    11,    11,    32,    27,    28,     0,     0,    29,    34,
      30,     0,     0,    31,    32,     0,     0,     0,     0,    33,
      34,    38,    24,     0,    25,    26,    35,     0,     0,    27,
      28,     0,     0,    29,     0,    30,     0,     0,    31,    32,
       0,    24,     0,    25,    33,    34,    24,    54,    25,    26,
       0,    35,     0,    27,    28,     0,     0,    29,    32,    30,
       0,     0,    31,    32,    34,     0,     0,     0,    33,    34,
      24,     0,    25,    26,    24,    35,    25,-32768,-32768,     0,
       0,    29,     0,-32768,     0,     0,-32768,    32,    24,     0,
      25,    32,    33,    34,     0,     0,    33,    34,     0,    35,
       0,     0,     0,    35,     0,    32,     0,     0,     0,     0,
      33,    34
};

static const short yycheck[] =
{
       6,    20,     8,    29,    10,    22,    29,    26,    18,    29,
      -1,    29,     0,     3,    16,     5,     6,    -1,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      20,    37,     0,     1,    -1,    25,    26,    -1,     7,     7,
      -1,     9,    32,    12,    12,    -1,    15,    15,     5,    17,
      -1,    20,    20,    22,    22,    24,    24,     3,    -1,     5,
       6,    30,    30,    20,    10,    11,    -1,    -1,    14,    26,
      16,    -1,    -1,    19,    20,    -1,    -1,    -1,    -1,    25,
      26,    27,     3,    -1,     5,     6,    32,    -1,    -1,    10,
      11,    -1,    -1,    14,    -1,    16,    -1,    -1,    19,    20,
      -1,     3,    -1,     5,    25,    26,     3,    28,     5,     6,
      -1,    32,    -1,    10,    11,    -1,    -1,    14,    20,    16,
      -1,    -1,    19,    20,    26,    -1,    -1,    -1,    25,    26,
       3,    -1,     5,     6,     3,    32,     5,    10,    11,    -1,
      -1,    14,    -1,    16,    -1,    -1,    19,    20,     3,    -1,
       5,    20,    25,    26,    -1,    -1,    25,    26,    -1,    32,
      -1,    -1,    -1,    32,    -1,    20,    -1,    -1,    -1,    -1,
      25,    26
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 3:
#line 161 "./src/inv/inv.y"
{}
    break;
case 4:
#line 163 "./src/inv/inv.y"
{ 
          InvYylineno = atoi(InvScanText)-1;
        }
    break;
case 5:
#line 171 "./src/inv/inv.y"
{
	   Inv_Invariant_t *inv = InvInvariantAlloc(yyvsp[-2].str, yyvsp[-1].sf);
           InvFormulaAddToManager(InvGlobalManager, yyvsp[-2].str, inv);
           lsNewEnd(InvGlobalListOfFormulaNames,
                    util_strsav(yyvsp[-2].str), LS_NH);
         }
    break;
case 6:
#line 178 "./src/inv/inv.y"
{
           char *name = InvObtainNewFormulaName();
	   Inv_Invariant_t *inv = InvInvariantAlloc(name, yyvsp[-1].sf);
           InvFormulaAddToManager(InvGlobalManager, name, inv);
           lsNewEnd(InvGlobalListOfFormulaNames,
                    util_strsav(name), LS_NH);
         }
    break;
case 7:
#line 186 "./src/inv/inv.y"
{
           /* error detected. Free all the global variables */
           /* then skip to the next formula */
           ParseError();
           
	 }
    break;
case 8:
#line 198 "./src/inv/inv.y"
{ 
	  yyval.sf = yyvsp[0].sf;
          InvGlobalFormula = yyval.sf;
	  if (!InvExprIsWellFormed(yyval.sf)) {
            Main_MochaErrorPrint(
              "Type mismatch: line %d.\n", InvYylineno);
            YYERROR;
          }
        }
    break;
case 9:
#line 211 "./src/inv/inv.y"
{
	     yyval.sf = InvExprAlloc(InvId_c, (InvExpr_t *) yyvsp[0].str, NIL(InvExpr_t));
	     InvGlobalFormula = yyval.sf;
          }
    break;
case 10:
#line 216 "./src/inv/inv.y"
{
            yyval.sf = yyvsp[-1].sf;
            InvGlobalFormula = yyval.sf;
	  }
    break;
case 11:
#line 221 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvBoolConst_c, (InvExpr_t *) 1,
			       NIL(InvExpr_t));
            InvGlobalFormula = yyval.sf;
          }
    break;
case 12:
#line 227 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvBoolConst_c, (InvExpr_t *) 0,
			       NIL(InvExpr_t));
            InvGlobalFormula = yyval.sf;
          }
    break;
case 13:
#line 233 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvNumConst_c, (InvExpr_t *) (long) atoi(InvScanText),
			       NIL(InvExpr_t));
            InvGlobalFormula = yyval.sf;
          }
    break;
case 14:
#line 239 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvIndex_c, InvExprAlloc(InvId_c, (InvExpr_t *)
                                                       yyvsp[-3].str, NIL(InvExpr_t)), yyvsp[-1].sf);
            InvGlobalFormula = yyval.sf;
          }
    break;
case 15:
#line 245 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvGreaterEqual_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
          }
    break;
case 16:
#line 250 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvLessEqual_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
          }
    break;
case 17:
#line 255 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvGreater_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
          }
    break;
case 18:
#line 260 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvLess_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
          }
    break;
case 19:
#line 265 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvAnd_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
	  }
    break;
case 20:
#line 270 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvOr_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
	  }
    break;
case 21:
#line 275 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvEquiv_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
	  }
    break;
case 22:
#line 280 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvImplies_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
	  }
    break;
case 23:
#line 285 "./src/inv/inv.y"
{
	    /* Ashwini: Added rule for TOK_XOR */
            yyval.sf = InvExprAlloc(InvXor_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
	  }
    break;
case 24:
#line 291 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvEqual_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
	  }
    break;
case 25:
#line 296 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvNot_c, yyvsp[0].sf, NIL(InvExpr_t));
            InvGlobalFormula = yyval.sf;
	  }
    break;
case 26:
#line 301 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvPlus_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
          }
    break;
case 27:
#line 306 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvMinus_c, yyvsp[-2].sf, yyvsp[0].sf);
            InvGlobalFormula = yyval.sf;
          }
    break;
case 28:
#line 311 "./src/inv/inv.y"
{
            yyval.sf = InvExprAlloc(InvUnaryMinus_c, yyvsp[0].sf, NIL(InvExpr_t));
            InvGlobalFormula = yyval.sf;
          }
    break;
case 29:
#line 320 "./src/inv/inv.y"
{
	   char *tmp1 = util_strsav(InvScanText);
	   int   len = strlen(tmp1);
	   char *tmp;

	   tmp = ALLOC(char, len-1);
	   memcpy(tmp, tmp1+1, len-2);
	   *(tmp+len-2) = '\0';

	   FREE(tmp1);
	   yyval.str = tmp;

           InvGlobalFormulaID = yyval.str;
         }
    break;
case 30:
#line 338 "./src/inv/inv.y"
{ 
	    yyval.str = util_strsav(InvScanText);
	  }
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 350 "./src/inv/inv.y"

	  
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
