/* A Bison parser, made from ./src/atlp/atlp.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse AtlpYyparse
#define yylex AtlpYylex
#define yyerror AtlpYyerror
#define yylval AtlpYylval
#define yychar AtlpYychar
#define yydebug AtlpYydebug
#define yynerrs AtlpYynerrs
# define	TOK_AND	257
# define	TOK_COMMA	258
# define	TOK_EQ	259
# define	TOK_EQUIV	260
# define	TOK_ES	261
# define	TOK_EVENTUALLY	262
# define	TOK_EXISTS	263
# define	TOK_FALSE	264
# define	TOK_FINALLY	265
# define	TOK_FORALL	266
# define	TOK_FORMULA	267
# define	TOK_FORMULA_ID	268
# define	TOK_GE	269
# define	TOK_GLOBALLY	270
# define	TOK_GT	271
# define	TOK_ID	272
# define	TOK_ID2	273
# define	TOK_IMPLIES	274
# define	TOK_INVARIANT	275
# define	TOK_LBRAC	276
# define	TOK_LE	277
# define	TOK_LINENO	278
# define	TOK_LLANGLE	279
# define	TOK_LLSQUARE	280
# define	TOK_LSQUARE	281
# define	TOK_LT	282
# define	TOK_MINUS	283
# define	TOK_NA	284
# define	TOK_NATNUM	285
# define	TOK_NOT	286
# define	TOK_ONEXT	287
# define	TOK_OR	288
# define	TOK_PLUS	289
# define	TOK_RBRAC	290
# define	TOK_RRANGLE	291
# define	TOK_RRSQUARE	292
# define	TOK_RSQUARE	293
# define	TOK_SEMICOLON	294
# define	TOK_TRUE	295
# define	TOK_UMINUS	296
# define	TOK_UNTIL	297
# define	TOK_WAIT	298
# define	TOK_XOR	299
# define	TOK_XNEXT	300

#line 1 "./src/atlp/atlp.y"

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

#line 97 "./src/atlp/atlp.y"
#ifndef YYSTYPE
typedef union {
  Atlp_Formula_t *sf;	/* state formula */
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



#define	YYFINAL		93
#define	YYFLAG		-32768
#define	YYNTBASE	47

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 300 ? yytranslate[x] : 56)

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
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     6,    10,    14,    17,    20,    22,
      24,    28,    30,    32,    34,    39,    43,    47,    51,    55,
      59,    63,    67,    71,    75,    79,    82,    86,    90,    94,
      98,   105,   112,   116,   120,   123,   125,   127,   131,   135,
     137,   139,   141,   143,   145,   147,   149,   151,   153,   155,
     157,   158,   160
};
static const short yyrhs[] =
{
      -1,    47,    48,     0,    49,     0,    24,    31,    40,     0,
      53,    50,    40,     0,    50,    40,     0,     1,    40,     0,
      51,     0,    54,     0,    22,    51,    36,     0,    41,     0,
      10,     0,    31,     0,    54,    27,    51,    39,     0,    51,
      15,    51,     0,    51,    23,    51,     0,    51,    17,    51,
       0,    51,    28,    51,     0,    51,     3,    51,     0,    51,
      34,    51,     0,    51,    45,    51,     0,    51,     6,    51,
       0,    51,    20,    51,     0,    51,     5,    51,     0,    32,
      51,     0,    52,    16,    51,     0,    52,    11,    51,     0,
      52,    33,    51,     0,    52,    46,    51,     0,    52,    22,
      51,    43,    51,    36,     0,    52,    22,    51,    44,    51,
      36,     0,    51,    35,    51,     0,    51,    29,    51,     0,
      29,    51,     0,    12,     0,     9,     0,    25,    55,    37,
       0,    26,    55,    38,     0,    14,     0,    18,     0,    19,
       0,    12,     0,     9,     0,    16,     0,    11,     0,    33,
       0,    46,     0,    43,     0,    44,     0,     0,    54,     0,
      55,     4,    54,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   178,   179,   183,   184,   192,   199,   206,   218,   231,
     239,   244,   252,   260,   268,   277,   282,   287,   292,   297,
     302,   307,   312,   317,   322,   327,   332,   338,   344,   350,
     356,   362,   368,   373,   378,   387,   394,   403,   413,   427,
     446,   450,   454,   458,   462,   466,   470,   474,   478,   482,
     493,   498,   507
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "TOK_AND", "TOK_COMMA", "TOK_EQ", 
  "TOK_EQUIV", "TOK_ES", "TOK_EVENTUALLY", "TOK_EXISTS", "TOK_FALSE", 
  "TOK_FINALLY", "TOK_FORALL", "TOK_FORMULA", "TOK_FORMULA_ID", "TOK_GE", 
  "TOK_GLOBALLY", "TOK_GT", "TOK_ID", "TOK_ID2", "TOK_IMPLIES", 
  "TOK_INVARIANT", "TOK_LBRAC", "TOK_LE", "TOK_LINENO", "TOK_LLANGLE", 
  "TOK_LLSQUARE", "TOK_LSQUARE", "TOK_LT", "TOK_MINUS", "TOK_NA", 
  "TOK_NATNUM", "TOK_NOT", "TOK_ONEXT", "TOK_OR", "TOK_PLUS", "TOK_RBRAC", 
  "TOK_RRANGLE", "TOK_RRSQUARE", "TOK_RSQUARE", "TOK_SEMICOLON", 
  "TOK_TRUE", "TOK_UMINUS", "TOK_UNTIL", "TOK_WAIT", "TOK_XOR", 
  "TOK_XNEXT", "descriptions", "description", "formula", "stateformula", 
  "exp", "path_qtf", "formula_id", "name", "name_or", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    47,    47,    48,    48,    49,    49,    49,    50,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    52,    52,    52,    52,    53,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      55,    55,    55
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     1,     3,     3,     2,     2,     1,     1,
       3,     1,     1,     1,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     3,     3,     3,
       6,     6,     3,     3,     2,     1,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     1,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     0,     0,    43,    12,    45,    42,    39,    44,    40,
      41,     0,     0,    50,    50,     0,    13,     0,    46,    11,
      48,    49,    47,     2,     3,     0,     8,     0,     0,     9,
       7,     0,     0,    43,    42,    51,     0,     0,    34,    25,
       6,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      10,     4,     0,    37,    38,    19,    24,    22,    15,    17,
      23,    16,    18,    33,    20,    32,    21,    27,    26,     0,
      28,    29,     5,     0,    52,     0,     0,    14,     0,     0,
      30,    31,     0,     0
};

static const short yydefgoto[] =
{
       1,    23,    24,    25,    26,    27,    28,    29,    36
};

static const short yypact[] =
{
  -32768,    54,   -35,    36,-32768,-32768,    77,-32768,-32768,-32768,
  -32768,   270,   -24,   306,   306,   270,-32768,   270,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,   -32,   221,   172,   270,   -17,
  -32768,   119,   -29,-32768,-32768,-32768,    16,    11,-32768,    55,
  -32768,   270,   270,   270,   270,   270,   270,   270,   270,   270,
     270,   270,   270,   270,   270,   270,   270,   270,   -28,   270,
  -32768,-32768,   306,-32768,-32768,    55,   -26,   229,   242,   242,
      22,   242,   242,-32768,   108,-32768,   249,    55,    55,    86,
      55,    55,-32768,   146,-32768,   270,   270,-32768,   167,   194,
  -32768,-32768,    13,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,   -14,   -11,-32768,-32768,   -12,     2
};


#define	YYLAST		352


static const short yytable[] =
{
      31,    35,    35,    49,    38,    30,    39,    32,    40,    51,
      59,    61,    82,    93,    58,    62,    37,     0,     0,     0,
      62,     0,     0,     0,     0,    41,     0,    42,    43,     0,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,   -36,    83,    64,
      84,    49,   -36,    63,    92,     2,    50,    51,   -36,     0,
      42,     0,     0,     3,     4,     5,     6,    52,     7,   -36,
       8,     0,     9,    10,    88,    89,    11,     0,    12,    13,
      14,     0,   -36,    15,    49,    16,    17,    18,   -35,    41,
      51,    42,    43,   -35,     0,    19,     0,    20,    21,   -35,
      22,    44,     0,    45,     0,     0,    46,     0,     0,    47,
     -35,    41,     0,    42,    48,    49,     0,     0,     0,     0,
      50,    51,    41,   -35,    42,    43,     0,     0,     0,    85,
      86,    52,     0,     0,    44,     0,    45,    49,     0,    46,
       0,     0,    47,    51,     0,     0,     0,    48,    49,    41,
       0,    42,    43,    50,    51,    60,     0,     0,     0,     0,
       0,    44,     0,    45,    52,     0,    46,     0,     0,    47,
      41,     0,    42,    43,    48,    49,     0,     0,     0,     0,
      50,    51,    44,    53,    45,    87,     0,    46,    54,     0,
      47,    52,     0,     0,    55,    48,    49,    41,     0,    42,
      43,    50,    51,    90,     0,    56,     0,     0,     0,    44,
       0,    45,    52,     0,    46,     0,     0,    47,    57,     0,
       0,     0,    48,    49,    41,     0,    42,    43,    50,    51,
      91,     0,    41,     0,    42,     0,    44,     0,    45,    52,
       0,    46,     0,     0,    47,    41,     0,    42,    43,    48,
      49,     0,    41,     0,    42,    50,    51,-32768,    49,-32768,
       0,     0,    46,    50,    51,-32768,    52,     0,     0,     0,
  -32768,    49,     0,     0,    52,     0,    50,    51,    49,     3,
       4,     5,     6,    50,    51,     0,     8,    52,     9,    10,
       0,     0,    11,     0,     0,    13,    14,     0,     0,    15,
       0,    16,    17,    18,     0,     0,     0,     0,     0,     0,
       0,    19,     0,    20,    21,    33,    22,     5,    34,     0,
       0,     0,     8,     0,     9,    10,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    18,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    20,
      21,     0,    22
};

static const short yycheck[] =
{
      11,    13,    14,    29,    15,    40,    17,    31,    40,    35,
      27,    40,    40,     0,    28,     4,    14,    -1,    -1,    -1,
       4,    -1,    -1,    -1,    -1,     3,    -1,     5,     6,    -1,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    11,    59,    38,
      62,    29,    16,    37,     0,     1,    34,    35,    22,    -1,
       5,    -1,    -1,     9,    10,    11,    12,    45,    14,    33,
      16,    -1,    18,    19,    85,    86,    22,    -1,    24,    25,
      26,    -1,    46,    29,    29,    31,    32,    33,    11,     3,
      35,     5,     6,    16,    -1,    41,    -1,    43,    44,    22,
      46,    15,    -1,    17,    -1,    -1,    20,    -1,    -1,    23,
      33,     3,    -1,     5,    28,    29,    -1,    -1,    -1,    -1,
      34,    35,     3,    46,     5,     6,    -1,    -1,    -1,    43,
      44,    45,    -1,    -1,    15,    -1,    17,    29,    -1,    20,
      -1,    -1,    23,    35,    -1,    -1,    -1,    28,    29,     3,
      -1,     5,     6,    34,    35,    36,    -1,    -1,    -1,    -1,
      -1,    15,    -1,    17,    45,    -1,    20,    -1,    -1,    23,
       3,    -1,     5,     6,    28,    29,    -1,    -1,    -1,    -1,
      34,    35,    15,    11,    17,    39,    -1,    20,    16,    -1,
      23,    45,    -1,    -1,    22,    28,    29,     3,    -1,     5,
       6,    34,    35,    36,    -1,    33,    -1,    -1,    -1,    15,
      -1,    17,    45,    -1,    20,    -1,    -1,    23,    46,    -1,
      -1,    -1,    28,    29,     3,    -1,     5,     6,    34,    35,
      36,    -1,     3,    -1,     5,    -1,    15,    -1,    17,    45,
      -1,    20,    -1,    -1,    23,     3,    -1,     5,     6,    28,
      29,    -1,     3,    -1,     5,    34,    35,    15,    29,    17,
      -1,    -1,    20,    34,    35,    23,    45,    -1,    -1,    -1,
      28,    29,    -1,    -1,    45,    -1,    34,    35,    29,     9,
      10,    11,    12,    34,    35,    -1,    16,    45,    18,    19,
      -1,    -1,    22,    -1,    -1,    25,    26,    -1,    -1,    29,
      -1,    31,    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    -1,    43,    44,     9,    46,    11,    12,    -1,
      -1,    -1,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    -1,    46
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
#line 183 "./src/atlp/atlp.y"
{}
    break;
case 4:
#line 185 "./src/atlp/atlp.y"
{ 
          AtlpYylineno = atoi(AtlpScanText)-1;
        }
    break;
case 5:
#line 193 "./src/atlp/atlp.y"
{
           AtlpFormulaAddToManager(AtlpGlobalManager, yyvsp[-2].str, yyvsp[-1].sf);
           lsNewEnd(AtlpGlobalListOfFormulaNames,
                    util_strsav(yyvsp[-2].str), LS_NH);
           
         }
    break;
case 6:
#line 200 "./src/atlp/atlp.y"
{
           char *name = AtlpObtainNewFormulaName();
           AtlpFormulaAddToManager(AtlpGlobalManager, name, yyvsp[-1].sf);
           lsNewEnd(AtlpGlobalListOfFormulaNames, util_strsav(name), LS_NH);
           
         }
    break;
case 7:
#line 207 "./src/atlp/atlp.y"
{
           /* error detected. Free all the global variables */
           /* then skip to the next formula */
           AtlpParseError();
           
	 }
    break;
case 8:
#line 219 "./src/atlp/atlp.y"
{ 
	  yyval.sf = yyvsp[0].sf;
          AtlpGlobalFormula = yyval.sf;
	  if (!AtlpFormulaIsWellFormed(yyval.sf)) {
            Main_MochaErrorPrint(
              "Type mismatch: line %d.\n", AtlpYylineno);
            YYERROR;
          }
        }
    break;
case 9:
#line 232 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_ID_c,
                                    yyvsp[0].str,
                                    NIL(Atlp_Formula_t)
                                    );
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 10:
#line 240 "./src/atlp/atlp.y"
{
            yyval.sf = yyvsp[-1].sf;
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 11:
#line 245 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_TRUE_c,
                                    NIL(Atlp_Formula_t),
                                    NIL(Atlp_Formula_t)
                                    );
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 12:
#line 253 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_FALSE_c,
                                    NIL(Atlp_Formula_t),
                                    NIL(Atlp_Formula_t)
                                    );
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 13:
#line 261 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_NATNUM_c,
                                    (Atlp_Formula_t *) util_strsav(AtlpScanText),
                                    NIL(Atlp_Formula_t)
                                    );
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 14:
#line 269 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_INDEX_c,
                                    Atlp_FormulaCreate(Atlp_ID_c,
                                                       (Atlp_Formula_t *) yyvsp[-3].str,
                                                       NIL(Atlp_Formula_t)),
                                    yyvsp[-1].sf);
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 15:
#line 278 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_GE_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 16:
#line 283 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_LE_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 17:
#line 288 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_GT_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 18:
#line 293 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_LT_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 19:
#line 298 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_AND_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 20:
#line 303 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_OR_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 21:
#line 308 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_XOR_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 22:
#line 313 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_EQUIV_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 23:
#line 318 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_IMPLIES_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 24:
#line 323 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_EQ_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 25:
#line 328 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_NOT_c, yyvsp[0].sf, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 26:
#line 333 "./src/atlp/atlp.y"
{
            yyval.sf = AtlpParsePathFormula (
              yyvsp[-2].keyword, TOK_GLOBALLY, yyvsp[0].sf, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 27:
#line 339 "./src/atlp/atlp.y"
{
            yyval.sf = AtlpParsePathFormula (
              yyvsp[-2].keyword, TOK_FINALLY, yyvsp[0].sf, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 28:
#line 345 "./src/atlp/atlp.y"
{
            yyval.sf = AtlpParsePathFormula (
              yyvsp[-2].keyword, TOK_ONEXT, yyvsp[0].sf, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 29:
#line 351 "./src/atlp/atlp.y"
{
            yyval.sf = AtlpParsePathFormula (
              yyvsp[-2].keyword, TOK_XNEXT, yyvsp[0].sf, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 30:
#line 357 "./src/atlp/atlp.y"
{
            yyval.sf = AtlpParsePathFormula (
              yyvsp[-5].keyword, TOK_UNTIL, yyvsp[-3].sf, yyvsp[-1].sf);
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 31:
#line 363 "./src/atlp/atlp.y"
{
            yyval.sf = AtlpParsePathFormula (
              yyvsp[-5].keyword, TOK_WAIT, yyvsp[-3].sf, yyvsp[-1].sf);
            AtlpGlobalFormula = yyval.sf;
	  }
    break;
case 32:
#line 369 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_PLUS_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 33:
#line 374 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_MINUS_c, yyvsp[-2].sf, yyvsp[0].sf);
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 34:
#line 379 "./src/atlp/atlp.y"
{
            yyval.sf = Atlp_FormulaCreate(Atlp_UMINUS_c, yyvsp[0].sf, NIL(Atlp_Formula_t));
            AtlpGlobalFormula = yyval.sf;
          }
    break;
case 35:
#line 388 "./src/atlp/atlp.y"
{
            lsNewBegin(AtlpGlobalListOfAgentLists,
                       (lsGeneric) lsCreate(), LS_NH);
	    yyval.keyword = TOK_ES; /* the exists a strategy for an empty set of agents,
                            meaning whether or not the agents cooperate */
	  }
    break;
case 36:
#line 395 "./src/atlp/atlp.y"
{
            lsNewBegin(AtlpGlobalListOfAgentLists,
                       (lsGeneric) lsCreate(), LS_NH);
	    yyval.keyword = TOK_NA; /* the empty set of agents cannot avoid, equiv to
                            the full set of agents can cooperate to  */
                            
            
	  }
    break;
case 37:
#line 404 "./src/atlp/atlp.y"
{
            lsList agentList;

            lsFirstItem(AtlpGlobalListOfAgentLists,
                        (lsGeneric *) &agentList, LS_NH);
	    lsSort(agentList, AtlpAgentCmp);
            lsUniq(agentList, AtlpAgentCmp, Atlp_AgentFree); 
	    yyval.keyword = TOK_ES; /* there Exists a Strategy */
	  }
    break;
case 38:
#line 414 "./src/atlp/atlp.y"
{
            lsList agentList;

            lsFirstItem(AtlpGlobalListOfAgentLists,
                        (lsGeneric *) &agentList, LS_NH);
	    lsSort(agentList, AtlpAgentCmp);
            lsUniq(agentList, AtlpAgentCmp, Atlp_AgentFree); 
	    yyval.keyword = TOK_NA; /* agents canNot Avoid */ 
	  }
    break;
case 39:
#line 428 "./src/atlp/atlp.y"
{
	   char *tmp1 = util_strsav(AtlpScanText);
	   int   len = strlen(tmp1);
	   char *tmp;

	   tmp = ALLOC(char, len-1);
	   memcpy(tmp, tmp1+1, len-2);
	   *(tmp+len-2) = '\0';

	   FREE(tmp1);
	   yyval.str = tmp;

           AtlpGlobalFormulaID = yyval.str;
         }
    break;
case 40:
#line 447 "./src/atlp/atlp.y"
{ 
	    yyval.str = util_strsav(AtlpScanText);
	  }
    break;
case 41:
#line 451 "./src/atlp/atlp.y"
{ 
	    yyval.str = util_strsav(AtlpScanText);
	  }
    break;
case 42:
#line 455 "./src/atlp/atlp.y"
{
            yyval.str = util_strsav("A");
          }
    break;
case 43:
#line 459 "./src/atlp/atlp.y"
{
            yyval.str = util_strsav("E");
          }
    break;
case 44:
#line 463 "./src/atlp/atlp.y"
{
            yyval.str = util_strsav("G");
          }
    break;
case 45:
#line 467 "./src/atlp/atlp.y"
{
            yyval.str = util_strsav("F");
          }
    break;
case 46:
#line 471 "./src/atlp/atlp.y"
{
            yyval.str = util_strsav("O");
          }
    break;
case 47:
#line 475 "./src/atlp/atlp.y"
{
            yyval.str = util_strsav("X");
          }
    break;
case 48:
#line 479 "./src/atlp/atlp.y"
{
            yyval.str = util_strsav("U");
          }
    break;
case 49:
#line 483 "./src/atlp/atlp.y"
{
            yyval.str = util_strsav("W");
          }
    break;
case 50:
#line 494 "./src/atlp/atlp.y"
{
          lsNewBegin(AtlpGlobalListOfAgentLists,
                     (lsGeneric) lsCreate(), LS_NH);
	}
    break;
case 51:
#line 499 "./src/atlp/atlp.y"
{
           lsList tmp = lsCreate();
           Atlp_Agent_t * agent = Atlp_AgentAllocWithName(yyvsp[0].str);
           lsNewEnd(tmp, (lsGeneric) agent, LS_NH);
           lsNewBegin(AtlpGlobalListOfAgentLists,
                      (lsGeneric) tmp, LS_NH);
           FREE(yyvsp[0].str);
         }
    break;
case 52:
#line 508 "./src/atlp/atlp.y"
{
           lsList tmp;
           Atlp_Agent_t * agent = Atlp_AgentAllocWithName(yyvsp[0].str);
           lsFirstItem(AtlpGlobalListOfAgentLists,
                       (lsGeneric *) &tmp, LS_NH);
           
           lsNewEnd(tmp, (lsGeneric) agent, LS_NH);
           FREE(yyvsp[0].str);
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
#line 522 "./src/atlp/atlp.y"

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

