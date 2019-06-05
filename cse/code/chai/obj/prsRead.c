/* A Bison parser, made from ./src/prs/prs.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	IDENTIFIER	257
# define	NATNUM	258
# define	BOOL_CONST	259
# define	EVENTQUERY	260
# define	EVENTSEND	261
# define	MODULE	262
# define	ENDMODULE	263
# define	ATOM	264
# define	ENDATOM	265
# define	PRIVATE	266
# define	INTERFACE	267
# define	EXTERNAL	268
# define	LAZY	269
# define	EVENT	270
# define	READS	271
# define	AWAITS	272
# define	CONTROLS	273
# define	INIT	274
# define	UPDATE	275
# define	HIDE	276
# define	ENDHIDE	277
# define	IN	278
# define	TIMER	279
# define	ARRAY	280
# define	BITVECTOR	281
# define	BITVECTOR_N	282
# define	FORALL	283
# define	IF	284
# define	THEN	285
# define	ELSE	286
# define	FI	287
# define	OF	288
# define	TYPEDEF	289
# define	DEFAULT	290
# define	NONDET	291
# define	IMPLY	292
# define	EQUIV	293
# define	NEQUIV	294
# define	CHOICE	295
# define	ARROW	296
# define	ASSIGN	297
# define	LASSIGN	298
# define	PAR	299
# define	AND	300
# define	OR	301
# define	XOR	302
# define	NOT	303
# define	GT	304
# define	LT	305
# define	GE	306
# define	LE	307
# define	EQ	308
# define	NEQ	309
# define	UMINUS	310
# define	ATOMDEF	311
# define	WAIT	312
# define	TOK_LINENO	313
# define	ELLIPSE	314
# define	ERROR	315

#line 1 "./src/prs/prs.y"

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


#line 140 "./src/prs/prs.y"
#ifndef YYSTYPE
typedef union{
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
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 1
#endif



#define	YYFINAL		280
#define	YYFLAG		-32768
#define	YYNTBASE	76

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 315 ? yytranslate[x] : 138)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    69,     2,     2,
      38,    39,    68,    66,    75,    67,     2,    70,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    72,    71,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    40,     2,    41,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    73,     2,    74,     2,     2,     2,     2,
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
      36,    37,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     6,     9,    11,    13,    17,    19,
      24,    29,    37,    43,    49,    53,    55,    57,    59,    61,
      63,    68,    70,    73,    76,    78,    79,    85,    86,    88,
      90,    91,    94,    95,    98,   102,   105,   108,   112,   113,
     117,   118,   122,   123,   127,   128,   129,   134,   135,   136,
     144,   148,   150,   153,   154,   159,   160,   163,   165,   167,
     169,   171,   175,   179,   183,   184,   185,   193,   194,   195,
     204,   208,   210,   213,   219,   223,   228,   232,   233,   238,
     242,   248,   250,   254,   256,   260,   262,   265,   271,   277,
     281,   282,   284,   285,   287,   291,   299,   300,   310,   311,
     319,   320,   328,   329,   334,   335,   340,   342,   344,   346,
     348,   350,   355,   357,   359,   363,   367,   371,   375,   379,
     383,   387,   391,   395,   399,   403,   407,   411,   414,   417,
     420,   423,   426,   430,   432,   434,   436,   440,   444,   448,
     452,   456,   460,   465,   470,   476,   478,   480,   482
};
static const short yyrhs[] =
{
      -1,    77,    78,     0,    79,     0,    78,    79,     0,    98,
       0,    80,     0,    63,     4,    71,     0,     1,     0,    35,
     136,    72,    81,     0,    35,   136,    72,   135,     0,    35,
     136,    72,    26,    82,    34,    83,     0,    35,   136,    72,
      27,   133,     0,    35,   136,    72,    28,   133,     0,    73,
     117,    74,     0,   136,     0,   135,     0,    81,     0,    16,
       0,    81,     0,    26,    82,    34,    83,     0,   135,     0,
      27,     4,     0,    28,     4,     0,   136,     0,     0,    19,
     118,    85,    87,    88,     0,     0,    15,     0,    16,     0,
       0,    17,   118,     0,     0,    18,   118,     0,    90,    92,
      94,     0,    96,    94,     0,    92,    94,     0,    63,     4,
      71,     0,     0,    20,    91,   119,     0,     0,    21,    93,
     119,     0,     0,    62,    95,   119,     0,     0,     0,    20,
      21,    97,   119,     0,     0,     0,     8,   136,    99,   101,
     100,   104,     9,     0,   136,    47,   113,     0,   102,     0,
     101,   102,     0,     0,   105,   103,   106,   121,     0,     0,
     104,   108,     0,    12,     0,    13,     0,    14,     0,   107,
       0,   106,    71,   107,     0,   117,    72,    25,     0,   117,
      72,    83,     0,     0,     0,    86,    10,   109,    84,   110,
      89,    11,     0,     0,     0,    86,    10,   136,   111,    84,
     112,    89,    11,     0,    63,     4,    71,     0,   136,     0,
     136,   114,     0,    22,   117,    24,   113,    23,     0,    38,
     113,    39,     0,    38,   113,    39,   114,     0,   113,    49,
     113,     0,     0,    40,   116,    41,   115,     0,   136,    47,
     136,     0,   136,    75,   116,    75,   136,     0,   136,     0,
     117,    75,   136,     0,   137,     0,   118,    75,   137,     0,
     120,     0,   119,   120,     0,    45,   131,    46,   122,   121,
       0,    45,    36,    46,   122,   121,     0,    63,     4,    71,
       0,     0,    71,     0,     0,   124,     0,   122,    71,   124,
       0,    30,   131,    31,   132,    32,   132,    33,     0,     0,
      29,   136,   136,    40,   136,    41,   125,    47,   130,     0,
       0,   136,    40,   136,    41,   126,    47,   130,     0,     0,
     136,    40,     4,    41,   127,    47,   130,     0,     0,   136,
     128,    47,   130,     0,     0,   136,   129,    48,     4,     0,
       7,     0,    37,     0,   132,     0,   132,     0,   136,     0,
     136,    40,   132,    41,     0,     4,     0,     5,     0,   132,
      66,   132,     0,   132,    67,   132,     0,   132,    58,   132,
       0,   132,    59,   132,     0,   132,    54,   132,     0,   132,
      55,   132,     0,   132,    56,   132,     0,   132,    57,   132,
       0,   132,    50,   132,     0,   132,    51,   132,     0,   132,
      42,   132,     0,   132,    43,   132,     0,   132,    52,   132,
       0,    50,   132,     0,    51,   132,     0,    52,   132,     0,
      67,   132,     0,    53,   132,     0,    38,   132,    39,     0,
       6,     0,   123,     0,     4,     0,   133,    67,   133,     0,
     133,    66,   133,     0,   133,    68,   133,     0,   133,    70,
     133,     0,   133,    69,   133,     0,    38,   133,    39,     0,
     136,    40,   136,    41,     0,   136,    40,     4,    41,     0,
      38,     4,    64,   133,    39,     0,     3,     0,    24,     0,
     134,     0,   136,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   283,   283,   296,   297,   302,   303,   304,   308,   318,
     322,   326,   330,   334,   340,   352,   360,   365,   371,   375,
     379,   383,   387,   391,   395,   411,   411,   439,   441,   443,
     447,   448,   457,   458,   467,   484,   498,   504,   511,   511,
     518,   518,   525,   525,   529,   534,   534,   543,   543,   543,
     621,   663,   664,   668,   668,   675,   676,   681,   683,   685,
     689,   690,   693,   720,   763,   763,   763,   804,   804,   804,
     834,   845,   856,   872,   876,   880,   886,   893,   893,   903,
     916,   930,   935,   942,   948,   958,   964,   972,  1018,  1052,
    1060,  1061,  1066,  1070,  1075,  1083,  1094,  1094,  1165,  1165,
    1253,  1253,  1328,  1328,  1357,  1357,  1393,  1447,  1453,  1459,
    1465,  1472,  1578,  1585,  1591,  1595,  1599,  1603,  1607,  1611,
    1615,  1625,  1632,  1634,  1636,  1638,  1640,  1642,  1644,  1646,
    1648,  1655,  1661,  1665,  1678,  1683,  1688,  1693,  1698,  1702,
    1706,  1710,  1717,  1721,  1726,  1736,  1746,  1752,  1758
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "IDENTIFIER", "NATNUM", "BOOL_CONST", 
  "EVENTQUERY", "EVENTSEND", "MODULE", "ENDMODULE", "ATOM", "ENDATOM", 
  "PRIVATE", "INTERFACE", "EXTERNAL", "LAZY", "EVENT", "READS", "AWAITS", 
  "CONTROLS", "INIT", "UPDATE", "HIDE", "ENDHIDE", "IN", "TIMER", "ARRAY", 
  "BITVECTOR", "BITVECTOR_N", "FORALL", "IF", "THEN", "ELSE", "FI", "OF", 
  "TYPEDEF", "DEFAULT", "NONDET", "'('", "')'", "'['", "']'", "IMPLY", 
  "EQUIV", "NEQUIV", "CHOICE", "ARROW", "ASSIGN", "LASSIGN", "PAR", "AND", 
  "OR", "XOR", "NOT", "GT", "LT", "GE", "LE", "EQ", "NEQ", "UMINUS", 
  "ATOMDEF", "WAIT", "TOK_LINENO", "ELLIPSE", "ERROR", "'+'", "'-'", 
  "'*'", "'%'", "'/'", "';'", "':'", "'{'", "'}'", "','", "program", "@1", 
  "definitions", "definition", "type_definition", "enum_const_list", 
  "arrayindex", "all_types", "atom_header", "@2", "atom_keyword", 
  "optional_reads", "optional_awaits", "commands_definition", 
  "init_guarded_commands", "@3", "update_guarded_commands", "@4", 
  "optional_wait_guarded_commands", "@5", "init_update_guarded_commands", 
  "@6", "module_definition", "@7", "@8", "ctlvars_definition", 
  "ctlvar_def", "@9", "atoms_def", "ctlvar_keyword", "declarations", 
  "declaration", "atom_def", "@10", "@11", "@12", "@13", 
  "module_expression", "variable_rename", "@14", "renames", 
  "declarator_list", "var_declarator_list", "guarded_commands", 
  "guarded_command", "opt_semicolon", "assignments", "conditional", 
  "assignment", "@15", "@16", "@17", "@18", "@19", "exp_or_nondet", 
  "predicate", "exp", "int_const", "index_declarator", "range_declarator", 
  "declarator", "var_declarator", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    77,    76,    78,    78,    79,    79,    79,    79,    80,
      80,    80,    80,    80,    81,    82,    82,    82,    83,    83,
      83,    83,    83,    83,    83,    85,    84,    86,    86,    86,
      87,    87,    88,    88,    89,    89,    89,    89,    91,    90,
      93,    92,    95,    94,    94,    97,    96,    99,   100,    98,
      98,   101,   101,   103,   102,   104,   104,   105,   105,   105,
     106,   106,   107,   107,   109,   110,   108,   111,   112,   108,
     108,   113,   113,   113,   113,   113,   113,   115,   114,   116,
     116,   117,   117,   118,   118,   119,   119,   120,   120,   120,
     121,   121,   122,   122,   122,   123,   125,   124,   126,   124,
     127,   124,   128,   124,   129,   124,   124,   130,   130,   131,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   133,   133,   133,   133,   133,
     133,   133,   134,   134,   135,   136,   136,   137,   137
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     1,     2,     1,     1,     3,     1,     4,
       4,     7,     5,     5,     3,     1,     1,     1,     1,     1,
       4,     1,     2,     2,     1,     0,     5,     0,     1,     1,
       0,     2,     0,     2,     3,     2,     2,     3,     0,     3,
       0,     3,     0,     3,     0,     0,     4,     0,     0,     7,
       3,     1,     2,     0,     4,     0,     2,     1,     1,     1,
       1,     3,     3,     3,     0,     0,     7,     0,     0,     8,
       3,     1,     2,     5,     3,     4,     3,     0,     4,     3,
       5,     1,     3,     1,     3,     1,     2,     5,     5,     3,
       0,     1,     0,     1,     3,     7,     0,     9,     0,     7,
       0,     7,     0,     4,     0,     4,     1,     1,     1,     1,
       1,     4,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
       2,     2,     3,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     4,     4,     5,     1,     1,     1,     1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     0,     8,   145,     0,   146,     0,     0,     0,     3,
       6,     5,     0,    47,     0,     0,     4,     0,     0,     0,
       7,     0,     0,    50,    71,    57,    58,    59,    48,    51,
      53,     0,     0,     0,     0,     0,     9,    10,     0,    81,
       0,     0,     0,    72,    55,    52,     0,    17,     0,    16,
      15,   135,     0,    12,    13,     0,     0,     0,     0,    74,
      76,     0,     0,    27,    90,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,    82,    75,    77,
       0,     0,    49,    28,    29,     0,     0,    56,    91,    54,
       0,    18,     0,     0,     0,    19,    11,    21,    24,   141,
     137,   136,   138,   140,   139,     0,    73,    78,    79,     0,
       0,    64,    61,    62,    63,     0,    22,    23,   144,     0,
      70,     0,    67,     0,    80,     0,    65,     0,    20,    25,
     147,   148,    83,     0,    68,     0,    30,     0,    38,    40,
       0,     0,     0,    44,    44,     0,    84,     0,    32,     0,
       0,    45,     0,     0,     0,    66,    44,    42,    36,    35,
       0,    31,     0,    26,   143,   142,     0,     0,     0,    39,
      85,    41,    37,    34,     0,    69,    33,    46,   112,   113,
     133,     0,     0,     0,     0,     0,     0,     0,     0,   134,
       0,   109,   110,     0,    86,    43,     0,    92,     0,   127,
     128,   129,   131,   130,    92,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    89,
       0,   106,     0,    90,    93,   102,   132,    90,   124,   125,
     122,   123,   126,   118,   119,   120,   121,   116,   117,   114,
     115,     0,     0,     0,    91,    88,     0,     0,     0,    87,
     111,     0,     0,    94,     0,     0,     0,     0,     0,     0,
     100,    98,   107,   103,   108,   105,    95,     0,     0,     0,
      96,     0,     0,     0,   101,    99,     0,    97,     0,     0,
       0
};

static const short yydefgoto[] =
{
     278,     1,     8,     9,    10,    95,    48,    96,   126,   136,
      86,   148,   163,   141,   142,   152,   143,   153,   158,   174,
     144,   166,    11,    18,    44,    28,    29,    46,    63,    30,
      64,    65,    87,   121,   133,   127,   145,    23,    43,   107,
      61,    66,   129,   169,   170,    89,   223,   189,   224,   273,
     269,   268,   247,   248,   263,   190,   264,    53,   130,    97,
     192,   132
};

static const short yypact[] =
{
  -32768,   147,-32768,-32768,    52,-32768,    52,    73,   132,-32768,
  -32768,-32768,    41,-32768,   -14,    10,-32768,    24,   199,    85,
  -32768,    52,    24,    65,    50,-32768,-32768,-32768,   199,-32768,
  -32768,     5,    11,    11,   145,    52,-32768,-32768,   -23,-32768,
      76,    24,    52,-32768,-32768,-32768,    52,-32768,   126,-32768,
  -32768,-32768,    11,   187,   187,   100,    34,    24,    52,    50,
  -32768,   121,   -33,    23,    95,-32768,    98,     9,   165,    11,
      11,    11,    11,    11,    11,-32768,     1,-32768,-32768,-32768,
      52,    52,-32768,-32768,-32768,   164,   177,-32768,    52,-32768,
     103,-32768,     5,   170,   189,-32768,-32768,-32768,-32768,-32768,
     171,   171,-32768,-32768,-32768,   223,-32768,-32768,-32768,   106,
     123,    52,-32768,-32768,-32768,   163,-32768,-32768,-32768,    52,
  -32768,   183,-32768,     9,-32768,    52,-32768,   183,-32768,   130,
  -32768,   168,-32768,    -3,-32768,    52,   192,    61,   193,-32768,
     212,   211,   202,   174,   174,    -3,-32768,    52,   219,   205,
     206,-32768,   -32,   -32,   159,-32768,   174,-32768,-32768,-32768,
     237,   130,    52,-32768,-32768,-32768,   -32,   139,   247,   -32,
  -32768,   -32,-32768,-32768,   -32,-32768,   130,   -32,-32768,-32768,
  -32768,   214,   203,   214,   214,   214,   214,   214,   214,-32768,
     213,   307,   231,   208,-32768,   -32,   232,   331,   266,   339,
     339,   339,   339,-32768,   331,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,-32768,
     214,-32768,    52,   209,-32768,    21,-32768,   209,   325,   325,
     339,   339,   339,    54,    54,    54,    54,   333,   333,-32768,
  -32768,   285,   218,    52,   331,-32768,   113,   235,   238,-32768,
  -32768,   214,   243,-32768,   253,   257,   148,   302,   245,    52,
  -32768,-32768,-32768,-32768,   307,-32768,-32768,   269,   260,   267,
  -32768,   148,   148,   268,-32768,-32768,   148,-32768,   313,   319,
  -32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,   321,-32768,    -9,   239,   -64,   220,-32768,
  -32768,-32768,-32768,   185,-32768,-32768,   204,-32768,   -93,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,   317,-32768,-32768,-32768,
  -32768,   265,-32768,-32768,-32768,-32768,-32768,   -13,   289,-32768,
     273,    32,  -143,   -27,   -12,  -221,   152,-32768,   124,-32768,
  -32768,-32768,-32768,-32768,   -92,   186,  -113,   155,-32768,    -8,
      -1,   234
};


#define	YYLAST		406


static const short yytable[] =
{
      12,    57,   245,    13,   161,    14,   249,    12,     3,    40,
      36,    37,     3,   167,    80,    51,    24,   138,   139,   176,
      39,    24,    47,    49,   106,    91,   114,     3,    60,     5,
      50,   168,    82,     5,    39,    92,    93,    94,    83,    84,
      24,    62,    81,    34,    76,    39,    21,    34,     5,    52,
      41,   159,    58,    38,   191,     3,    24,    77,    19,   128,
     140,   246,    22,   173,     3,   149,    98,    56,   191,  -104,
     198,   199,   200,   201,   202,   203,     5,    15,    35,   108,
      62,    20,    35,    47,    49,     5,    85,    39,    17,    98,
      42,    50,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,     3,   242,    75,    58,
     122,    31,    32,    33,    41,    59,     3,   254,   124,    91,
     216,   217,    98,    34,   131,    41,   171,     5,   113,    92,
      93,    94,    -2,     2,   131,     3,   150,     5,   258,   177,
       4,    34,     3,   178,   179,   180,   131,   195,     2,    55,
       3,     3,   178,   179,   180,     4,     5,   194,    35,   194,
      67,   131,    79,     5,    74,   194,    88,     6,   110,   181,
      90,     5,     5,    58,   116,   182,    35,   183,   181,   274,
     275,   119,     6,   194,   277,   262,   183,   111,    54,   184,
     185,   186,   187,   117,   120,     7,   225,   123,   184,   185,
     186,   187,   125,   225,    99,   135,   188,    68,   137,   147,
       7,    25,    26,    27,   151,   188,   154,     3,   178,   179,
     180,   243,   155,   139,   100,   101,   102,   103,   104,   105,
     172,    69,    70,    71,    72,    73,   157,   162,     5,    71,
      72,    73,   252,   225,   181,   255,   164,   165,   175,   197,
     251,   193,   183,    69,    70,    71,    72,    73,   267,   204,
     205,   206,   118,   220,   184,   185,   186,   187,   207,   208,
     209,   218,   210,   211,   212,   213,   214,   215,   266,   219,
     244,   188,   256,   259,   216,   217,   257,   205,   206,    69,
      70,    71,    72,    73,   260,   207,   208,   209,   261,   210,
     211,   212,   213,   214,   215,   226,   265,   271,   205,   206,
     270,   216,   217,   279,   272,   276,   207,   208,   209,   280,
     210,   211,   212,   213,   214,   215,   250,   205,   206,    16,
     160,   115,   216,   217,     3,   207,   208,   209,   221,   210,
     211,   212,   213,   214,   215,    45,   156,   134,    78,   205,
     206,   216,   217,   112,   109,     5,   227,   207,   208,   209,
     222,   210,   211,   212,   213,   214,   215,   196,   253,   146,
       0,     0,     0,   216,   217,   207,   208,   209,     0,   210,
     211,   212,   213,   214,   215,     0,     0,   210,   211,   212,
     213,   216,   217,   210,   211,   212,   213,   214,   215,   216,
     217,     0,     0,     0,     0,   216,   217
};

static const short yycheck[] =
{
       1,    24,   223,     4,   147,     6,   227,     8,     3,    22,
      19,    19,     3,    45,    47,     4,    17,    20,    21,   162,
      21,    22,    31,    31,    23,    16,    90,     3,    41,    24,
      31,    63,     9,    24,    35,    26,    27,    28,    15,    16,
      41,    42,    75,    38,    57,    46,    22,    38,    24,    38,
      49,   144,    75,    21,   167,     3,    57,    58,    72,   123,
      63,    40,    38,   156,     3,     4,    67,    35,   181,    48,
     183,   184,   185,   186,   187,   188,    24,     4,    73,    80,
      81,    71,    73,    92,    92,    24,    63,    88,    47,    90,
      40,    92,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,     3,   220,    74,    75,
     111,    26,    27,    28,    49,    39,     3,     4,   119,    16,
      66,    67,   123,    38,   125,    49,   153,    24,    25,    26,
      27,    28,     0,     1,   135,     3,   137,    24,   251,   166,
       8,    38,     3,     4,     5,     6,   147,   174,     1,     4,
       3,     3,     4,     5,     6,     8,    24,   169,    73,   171,
      34,   162,    41,    24,    64,   177,    71,    35,     4,    30,
      72,    24,    24,    75,     4,    36,    73,    38,    30,   271,
     272,    75,    35,   195,   276,    37,    38,    10,    33,    50,
      51,    52,    53,     4,    71,    63,   197,    34,    50,    51,
      52,    53,    19,   204,    39,    75,    67,    52,    40,    17,
      63,    12,    13,    14,    21,    67,     4,     3,     4,     5,
       6,   222,    11,    21,    69,    70,    71,    72,    73,    74,
      71,    66,    67,    68,    69,    70,    62,    18,    24,    68,
      69,    70,   243,   244,    30,   246,    41,    41,    11,    46,
      32,     4,    38,    66,    67,    68,    69,    70,   259,    46,
      42,    43,    39,    31,    50,    51,    52,    53,    50,    51,
      52,    40,    54,    55,    56,    57,    58,    59,    33,    71,
      71,    67,    47,    40,    66,    67,    48,    42,    43,    66,
      67,    68,    69,    70,    41,    50,    51,    52,    41,    54,
      55,    56,    57,    58,    59,    39,     4,    47,    42,    43,
      41,    66,    67,     0,    47,    47,    50,    51,    52,     0,
      54,    55,    56,    57,    58,    59,    41,    42,    43,     8,
     145,    92,    66,    67,     3,    50,    51,    52,     7,    54,
      55,    56,    57,    58,    59,    28,   142,   127,    59,    42,
      43,    66,    67,    88,    81,    24,   204,    50,    51,    52,
      29,    54,    55,    56,    57,    58,    59,   181,   244,   135,
      -1,    -1,    -1,    66,    67,    50,    51,    52,    -1,    54,
      55,    56,    57,    58,    59,    -1,    -1,    54,    55,    56,
      57,    66,    67,    54,    55,    56,    57,    58,    59,    66,
      67,    -1,    -1,    -1,    -1,    66,    67
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

case 1:
#line 284 "./src/prs/prs.y"
{
          yylineno=1;
          PrsMdlManager =
              (Mdl_Manager_t *) Main_ManagerReadModuleManager(PrsMainManager);
          PrsTypeManager =
              (Var_TypeManager_t *)
              Main_ManagerReadTypeManager(PrsMainManager);
        }
    break;
case 2:
#line 292 "./src/prs/prs.y"
{}
    break;
case 7:
#line 305 "./src/prs/prs.y"
{ 
          yylineno = yyvsp[-1].val-1;
        }
    break;
case 8:
#line 309 "./src/prs/prs.y"
{
          if (PrsModule) Mdl_ModuleFree(PrsModule);
          YYABORT;
	}
    break;
case 9:
#line 319 "./src/prs/prs.y"
{ 
	  if (!PrsTypeEnumAlloc(yyvsp[-2].id, yyvsp[0].List, PrsTypeManager)) YYERROR;
	}
    break;
case 10:
#line 323 "./src/prs/prs.y"
{
          if (!PrsTypeRangeAlloc(yyvsp[-2].id, yyvsp[0].val, PrsTypeManager)) YYERROR;
        }
    break;
case 11:
#line 327 "./src/prs/prs.y"
{
          if (!PrsTypeArrayAlloc(yyvsp[-5].id, yyvsp[-2].vartypeptr, yyvsp[0].vartypeptr, PrsTypeManager)) YYERROR;
        }
    break;
case 12:
#line 331 "./src/prs/prs.y"
{
          if (!PrsTypeBitvectorAlloc(yyvsp[-3].id, yyvsp[0].val, PrsTypeManager)) YYERROR;
        }
    break;
case 13:
#line 335 "./src/prs/prs.y"
{
          if (!PrsTypeBitvectorNAlloc(yyvsp[-3].id, yyvsp[0].val, PrsTypeManager)) YYERROR;
        }
    break;
case 14:
#line 341 "./src/prs/prs.y"
{
          /* a enum_const_list is basically a set, so can remove all
             duplicates */
          lsList constList = yyvsp[-1].List;
          lsSort(constList, strcmp);
          lsUniq(constList, strcmp, free); 

          yyval.List = constList;
        }
    break;
case 15:
#line 353 "./src/prs/prs.y"
{
          char *typeName = yyvsp[0].id;
          
          yyval.vartypeptr = Var_TypeManagerReadTypeFromName(PrsTypeManager, typeName);
          FREE(typeName);
        }
    break;
case 16:
#line 361 "./src/prs/prs.y"
{
          yyval.vartypeptr = PrsTypeRangeAlloc(NIL(char), yyvsp[0].val, PrsTypeManager);
        }
    break;
case 17:
#line 366 "./src/prs/prs.y"
{
          yyval.vartypeptr = PrsTypeEnumAlloc(NIL(char), yyvsp[0].List, PrsTypeManager);
        }
    break;
case 18:
#line 372 "./src/prs/prs.y"
{
          yyval.vartypeptr = Var_TypeManagerReadTypeFromName(PrsTypeManager, "event");
        }
    break;
case 19:
#line 376 "./src/prs/prs.y"
{
          yyval.vartypeptr = PrsTypeEnumAlloc(NIL(char), yyvsp[0].List, PrsTypeManager);
        }
    break;
case 20:
#line 380 "./src/prs/prs.y"
{
          yyval.vartypeptr = PrsTypeArrayAlloc(NIL(char), yyvsp[-2].vartypeptr, yyvsp[0].vartypeptr, PrsTypeManager);
        }
    break;
case 21:
#line 384 "./src/prs/prs.y"
{
          yyval.vartypeptr = PrsTypeRangeAlloc(NIL(char), yyvsp[0].val, PrsTypeManager);
        }
    break;
case 22:
#line 388 "./src/prs/prs.y"
{
          yyval.vartypeptr = PrsTypeBitvectorAlloc(NIL(char), yyvsp[0].val, PrsTypeManager);
	}
    break;
case 23:
#line 392 "./src/prs/prs.y"
{
          yyval.vartypeptr = PrsTypeBitvectorNAlloc(NIL(char), yyvsp[0].val, PrsTypeManager);
	}
    break;
case 24:
#line 396 "./src/prs/prs.y"
{
          char *typename = yyvsp[0].id;
          
          yyval.vartypeptr = Var_TypeManagerReadTypeFromName(PrsTypeManager,
                                              typename);
          if (yyval.vartypeptr == NIL(Var_Type_t))
            Main_MochaErrorPrint("Error at line %d: type %s not defined.\n", 
                                 yylineno, typename);
          FREE(typename);
        }
    break;
case 25:
#line 412 "./src/prs/prs.y"
{
          PrsDeclaratorListRemoveDuplication(yyvsp[0].List);
	  /* need to check if the control variables 
	     are declared to be interface or private */
          PrsAtomAddVariables(yyvsp[0].List, 0);
          lsDestroy(yyvsp[0].List, PrsNodeFree);
        }
    break;
case 26:
#line 420 "./src/prs/prs.y"
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
    break;
case 27:
#line 439 "./src/prs/prs.y"
{ yyval.val = 0; /* no keyword */ }
    break;
case 28:
#line 442 "./src/prs/prs.y"
{ yyval.val = 1; }
    break;
case 29:
#line 444 "./src/prs/prs.y"
{ yyval.val = 2; }
    break;
case 30:
#line 447 "./src/prs/prs.y"
{ /* no reads */ }
    break;
case 31:
#line 449 "./src/prs/prs.y"
{
          PrsDeclaratorListRemoveDuplication(yyvsp[0].List);
          PrsAtomAddVariables(yyvsp[0].List, 1);
          lsDestroy(yyvsp[0].List, PrsNodeFree);
        }
    break;
case 32:
#line 457 "./src/prs/prs.y"
{ /* no awaits */ }
    break;
case 33:
#line 459 "./src/prs/prs.y"
{
          PrsDeclaratorListRemoveDuplication(yyvsp[0].List);
          PrsAtomAddVariables(yyvsp[0].List, 2);
          lsDestroy(yyvsp[0].List, PrsNodeFree);
        }
    break;
case 34:
#line 468 "./src/prs/prs.y"
{ 
          lsGen lsgen;
          Atm_Cmd_t *guarded_cmd;
          
          lsForEachItem(yyvsp[-2].List, lsgen, guarded_cmd){
            if (lsLength(Atm_CmdReadAssignList(guarded_cmd)) == 0 ) {
              Main_MochaErrorPrint(
                "Error at %d: Init commands must have non-empty assignments.\n",
                yylineno);
              PrsParseValid = 0;
            }
          }
          Atm_AtomAddInitCommandList (PrsAtom, yyvsp[-2].List);
          Atm_AtomAddUpdateCommandList (PrsAtom, yyvsp[-1].List);
          Atm_AtomAddWaitCommandList (PrsAtom, yyvsp[0].List);
        }
    break;
case 35:
#line 485 "./src/prs/prs.y"
{
          lsList readVarList = Atm_AtomReadReadVarList(PrsAtom);
          
          if (lsLength(readVarList) != 0) {
            Main_MochaErrorPrint(
              "Error at %d: Read variables not allowed in init guarded commands.\n", yylineno);
            PrsParseValid = 0;
          }
          
	  Atm_AtomAddInitCommandList (PrsAtom, yyvsp[-1].List);
	  Atm_AtomAddUpdateCommandList (PrsAtom, yyvsp[-1].List);
	  Atm_AtomAddWaitCommandList (PrsAtom, yyvsp[0].List);
        }
    break;
case 36:
#line 499 "./src/prs/prs.y"
{
          Atm_AtomAddInitCommandList (PrsAtom, lsCreate());
	  Atm_AtomAddUpdateCommandList (PrsAtom, yyvsp[-1].List);
	  Atm_AtomAddWaitCommandList (PrsAtom, yyvsp[0].List);
        }
    break;
case 37:
#line 505 "./src/prs/prs.y"
{ 
          yylineno = yyvsp[-1].val-1;
        }
    break;
case 38:
#line 512 "./src/prs/prs.y"
{ PrsParsingGuardedCmd = 0; }
    break;
case 39:
#line 514 "./src/prs/prs.y"
{ yyval.List = yyvsp[0].List; }
    break;
case 40:
#line 519 "./src/prs/prs.y"
{ PrsParsingGuardedCmd = 1; }
    break;
case 41:
#line 521 "./src/prs/prs.y"
{ yyval.List = yyvsp[0].List; }
    break;
case 42:
#line 526 "./src/prs/prs.y"
{ PrsParsingGuardedCmd = 3; }
    break;
case 43:
#line 528 "./src/prs/prs.y"
{ yyval.List = yyvsp[0].List; }
    break;
case 44:
#line 530 "./src/prs/prs.y"
{ yyval.List = lsCreate(); }
    break;
case 45:
#line 535 "./src/prs/prs.y"
{ PrsParsingGuardedCmd = 2; }
    break;
case 46:
#line 537 "./src/prs/prs.y"
{ yyval.List = yyvsp[0].List; }
    break;
case 47:
#line 544 "./src/prs/prs.y"
{
          char *moduleName = yyvsp[0].id;
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
    break;
case 48:
#line 563 "./src/prs/prs.y"
{
            if (!PrsParseValid) YYERROR;
            Atm_ReinitTypeCheck(PrsTypeManager);
            
	  }
    break;
case 49:
#line 569 "./src/prs/prs.y"
{
            /* PrsVariableCheckUnused */
            /* PrsAcyclicityTest */
            /* PrsModuleSetModuleExpr */
            
            char *moduleName = yyvsp[-5].id;
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
    break;
case 50:
#line 622 "./src/prs/prs.y"
{
                  char * moduleName = yyvsp[-2].id;

                  if (!yyvsp[0].mdlexprptr) {
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
                  
                  PrsModule = Mdl_ModuleExprEvaluate(moduleName, yyvsp[0].mdlexprptr, PrsMdlManager);

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
                      Mdl_ExprFree(yyvsp[0].mdlexprptr);
                      FREE(moduleName);
                      YYERROR;
                    }
                  }
                }
    break;
case 53:
#line 669 "./src/prs/prs.y"
{
		  _varclass = yyvsp[0].val; 
		}
    break;
case 55:
#line 675 "./src/prs/prs.y"
{/* no atom defined within the module */}
    break;
case 57:
#line 682 "./src/prs/prs.y"
{ yyval.val=PRIVATE;}
    break;
case 58:
#line 684 "./src/prs/prs.y"
{ yyval.val=INTERFACE;}
    break;
case 59:
#line 686 "./src/prs/prs.y"
{ yyval.val=EXTERNAL;}
    break;
case 60:
#line 689 "./src/prs/prs.y"
{ }
    break;
case 61:
#line 690 "./src/prs/prs.y"
{}
    break;
case 62:
#line 694 "./src/prs/prs.y"
{
		  int peid;
		  char *name, *tmpName;
                  lsGen lsgen;
		  Var_Type_t *type;

                  if (_varclass == PRIVATE) peid = 0; 
                  else if (_varclass == EXTERNAL) peid = 1;
                  else if (_varclass == INTERFACE) peid = 2;

                  if (PrsParseValid) {
                      lsForEachItem(yyvsp[-2].List, lsgen, tmpName){
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
                  lsDestroy(yyvsp[-2].List, NULL);
		}
    break;
case 63:
#line 721 "./src/prs/prs.y"
{
		  int peid;
		  char *name, *tmpName;
		  Var_Type_t *type = yyvsp[0].vartypeptr;	
                  lsGen lsgen;
                  
                  if (type == NIL(Var_Type_t)){
                    PrsParseValid = 0;
                  }
                  else {
                    if (_varclass == PRIVATE) peid = 0; 
                    else if (_varclass == EXTERNAL) peid = 1;
                    else if (_varclass == INTERFACE) peid = 2;

                    lsForEachItem(yyvsp[-2].List, lsgen, tmpName){
                      if (Var_TypeManagerReadEnumElementFromName(
                        PrsTypeManager, tmpName)) {
                        Main_MochaErrorPrint(
                          "Error at line %d: an enumerated type element of name %s already exists.\n", yylineno, tmpName);
                        PrsParseValid = 0;
                      }
                    }

                    if (PrsParseValid)
                      lsForEachItem(yyvsp[-2].List, lsgen, tmpName){
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
                  lsDestroy(yyvsp[-2].List, NULL);
		}
    break;
case 64:
#line 764 "./src/prs/prs.y"
{
                  Atm_Atom_t *atm;
                  char *atmname, *tmpname;

                  /* Create a new atom name, because atom name is not
                     supplied */
                  tmpname = Atm_AtomNewName();
                  
                  atmname = Mdl_NamePrefixWithString(
                    Mdl_ModuleReadName(PrsModule), tmpname);
                  atm = Atm_AtomAlloc(atmname);
                  if (yyvsp[-1].val == 1)
                    Atm_AtomSetAtomType(atm, Atm_Lazy_c);
                  if (yyvsp[-1].val == 2)
                    Atm_AtomSetAtomType(atm, Atm_Event_c);
                  Mdl_ModuleAddAtom(PrsModule, atmname, atm);
                  
                  PrsAtom = atm;
                  FREE(tmpname);
                  FREE(atmname);
		}
    break;
case 65:
#line 787 "./src/prs/prs.y"
{
            if (!PrsParseValid) YYERROR;
          }
    break;
case 66:
#line 791 "./src/prs/prs.y"
{
            PrsAtom = NIL(Atm_Atom_t);
            if (!PrsParseValid) YYERROR;
          }
    break;
case 67:
#line 805 "./src/prs/prs.y"
{ Atm_Atom_t *atm;
		  char *atmName;
                                    
		  atmName = Mdl_NamePrefixWithString(
                    Mdl_ModuleReadName(PrsModule), yyvsp[0].id);
		  atm = Atm_AtomAlloc(atmName);
                  if (yyvsp[-2].val == 1)
                    Atm_AtomSetAtomType(atm, Atm_Lazy_c);
                  if (yyvsp[-2].val == 2)
                    Atm_AtomSetAtomType(atm, Atm_Event_c);
		  Mdl_ModuleAddAtom(PrsModule, atmName, atm);
		  PrsAtom = atm;
                  FREE(yyvsp[0].id);
                  FREE(atmName);
		}
    break;
case 68:
#line 822 "./src/prs/prs.y"
{
          if (!PrsParseValid) YYERROR;
        }
    break;
case 69:
#line 827 "./src/prs/prs.y"
{
          /* Check unused variables.  Flag warning and remove redundant variables. */
          /* Atm_AtomCheckUnusedVariables(PrsAtom); */
          PrsAtom = NIL(Atm_Atom_t);
          if (!PrsParseValid) YYERROR;
        }
    break;
case 70:
#line 835 "./src/prs/prs.y"
{ 
          yylineno = yyvsp[-1].val-1;
        }
    break;
case 71:
#line 846 "./src/prs/prs.y"
{
          if (!Mdl_ModuleReadFromName(PrsMdlManager, yyvsp[0].id)){
            Main_MochaErrorPrint("Error at line %d: Module %s not defined.\n",
                                 yylineno, yyvsp[0].id);
            yyval.mdlexprptr = NIL(Mdl_Expr_t);
          } else {
            yyval.mdlexprptr = Mdl_ExprModuleDupAlloc(yyvsp[0].id);
          }
          FREE(yyvsp[0].id);
	}
    break;
case 72:
#line 857 "./src/prs/prs.y"
{
          
          if (!Mdl_ModuleReadFromName(PrsMdlManager, yyvsp[-1].id)){
            Main_MochaErrorPrint("Error at line %d: Module %s not defined.\n",
                                 yylineno, yyvsp[-1].id);
            yyval.mdlexprptr = NIL(Mdl_Expr_t);
          } else {
            Mdl_Expr_t * tmp = Mdl_ExprModuleDupAlloc(yyvsp[-1].id);
            lsList list1 = (lsList) PrsNodeReadField1(yyvsp[0].nodepointer);
            lsList list2 = (lsList) PrsNodeReadField2(yyvsp[0].nodepointer);
            yyval.mdlexprptr = Mdl_ExprRenameAlloc(tmp, list1, list2);
          }
          FREE(yyvsp[-1].id);
          PrsNodeFree(yyvsp[0].nodepointer);
	}
    break;
case 73:
#line 873 "./src/prs/prs.y"
{
          yyval.mdlexprptr = Mdl_ExprHideAlloc(yyvsp[-1].mdlexprptr, yyvsp[-3].List);
	}
    break;
case 74:
#line 877 "./src/prs/prs.y"
{
          yyval.mdlexprptr = yyvsp[-1].mdlexprptr;
        }
    break;
case 75:
#line 881 "./src/prs/prs.y"
{
          lsList list1 = (lsList) PrsNodeReadField1(yyvsp[0].nodepointer);
          lsList list2 = (lsList) PrsNodeReadField2(yyvsp[0].nodepointer);
          yyval.mdlexprptr = Mdl_ExprRenameAlloc(yyvsp[-2].mdlexprptr, list1, list2);
        }
    break;
case 76:
#line 887 "./src/prs/prs.y"
{
          yyval.mdlexprptr = Mdl_ExprComposeAlloc (yyvsp[-2].mdlexprptr, yyvsp[0].mdlexprptr);
        }
    break;
case 77:
#line 893 "./src/prs/prs.y"
{}
    break;
case 78:
#line 894 "./src/prs/prs.y"
{
	  yyval.nodepointer= yyvsp[-2].nodepointer;
        }
    break;
case 79:
#line 904 "./src/prs/prs.y"
{

                  lsList designatorList  = lsCreate();
                  lsList destinationList = lsCreate();
                  PrsNode_t *node =
                      PrsNodeAlloc(0, (char *) designatorList, 
                                   (char *) destinationList);
                  lsNewBegin (designatorList,  yyvsp[-2].id, LS_NH);
                  lsNewEnd   (destinationList, yyvsp[0].id, LS_NH);
                  yyval.nodepointer = node;
		}
    break;
case 80:
#line 917 "./src/prs/prs.y"
{ 

                  lsList designatorList  = (lsList) PrsNodeReadField1(yyvsp[-2].nodepointer);
                  lsList destinationList = (lsList) PrsNodeReadField2(yyvsp[-2].nodepointer);
                  
                  lsNewBegin (designatorList,  yyvsp[-4].id, LS_NH);
                  lsNewEnd   (destinationList, yyvsp[0].id, LS_NH);

                  yyval.nodepointer  = yyvsp[-2].nodepointer;
		}
    break;
case 81:
#line 931 "./src/prs/prs.y"
{
                  yyval.List = lsCreate();
                  lsNewEnd(yyval.List, yyvsp[0].id, LS_NH);
		}
    break;
case 82:
#line 936 "./src/prs/prs.y"
{
                  yyval.List = yyvsp[-2].List;
                  lsNewEnd(yyval.List, yyvsp[0].id, LS_NH);
		}
    break;
case 83:
#line 943 "./src/prs/prs.y"
{
             yyval.List = lsCreate();
             lsNewEnd(yyval.List, (lsGeneric) yyvsp[0].nodepointer, LS_NH);
           }
    break;
case 84:
#line 949 "./src/prs/prs.y"
{
          yyval.List = yyvsp[-2].List;
          lsNewEnd(yyval.List, (lsGeneric) yyvsp[0].nodepointer, LS_NH);
        }
    break;
case 85:
#line 959 "./src/prs/prs.y"
{
	    yyval.List = lsCreate();
            if (yyvsp[0].atmcmdptr != NIL(Atm_Cmd_t))
              lsNewEnd(yyval.List, (lsGeneric) yyvsp[0].atmcmdptr, LS_NH);
	  }
    break;
case 86:
#line 965 "./src/prs/prs.y"
{
            if (yyvsp[0].atmcmdptr != NIL(Atm_Cmd_t))
              lsNewEnd(yyval.List, (lsGeneric) yyvsp[0].atmcmdptr, LS_NH);
	  }
    break;
case 87:
#line 973 "./src/prs/prs.y"
{
              Var_Type_t *predtype;
              Var_Type_t *booltype;
	      lsGen lsgen;
	      Atm_Assign_t *assgt;
              long OldBound, NewBound;
	      Var_Variable_t *timerVar;      
	      Var_Type_t *timerType;
	      Atm_Expr_t *expr;
              
              if (!ParseStatusIsValid(NIL(char)) || !yyvsp[-3].atmexprptr) YYERROR;
              
              /* check if predicate is of boolean type */
              /* predtype = Atm_ExprObtainDataType($2); */
              booltype = Var_TypeManagerReadTypeFromName(PrsTypeManager,
                                                         "bool");

              /* if (!Atm_TypeCheck (booltype, $2)) { */
              if( !Atm_TypeCheckNew(booltype, &yyvsp[-3].atmexprptr)){ 

                Main_MochaErrorPrint("Error at line %d: Type mismatch in guard\n", yylineno);
                PrsParseValid = 0;
              }

              /* should check the assignments */

              if (!ParseStatusIsValid(NIL(char))) YYERROR;
              lsForEachItem(yyvsp[-1].List, lsgen, assgt){
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
              yyval.atmcmdptr = Atm_CmdAlloc(yyvsp[-3].atmexprptr, yyvsp[-1].List);				
            }
    break;
case 88:
#line 1019 "./src/prs/prs.y"
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
                  cmd = Atm_CmdAlloc(NIL(Atm_Expr_t), yyvsp[-1].List);
                  Atm_AtomSetDefaultInitCommand(PrsAtom, cmd);
                }
              
              if (PrsParsingGuardedCmd == 1 || PrsParsingGuardedCmd == 2)
                if (Atm_AtomReadDefaultUpdateCommand(PrsAtom)) {
                  Main_MochaErrorPrint(
                    "Error at line %d: Only one default guarded command allowed.\n", yylineno);
                  PrsParseValid = 0;
                }
                else {
                  cmd = Atm_CmdAlloc(NIL(Atm_Expr_t), yyvsp[-1].List);
                  Atm_AtomSetDefaultUpdateCommand(PrsAtom, cmd);
                }

              /* should check the assignments */
              /* return a NULL guarded command, so that the rule
                 "guarded_commands" can detect it and remove it from the
                 list of guarded assignments */
              yyval.atmcmdptr =  NIL(Atm_Cmd_t);
            }
    break;
case 89:
#line 1053 "./src/prs/prs.y"
{ 
          yylineno = yyvsp[-1].val-1;
          yyval.atmcmdptr =  NIL(Atm_Cmd_t);
        }
    break;
case 90:
#line 1060 "./src/prs/prs.y"
{}
    break;
case 91:
#line 1061 "./src/prs/prs.y"
{}
    break;
case 92:
#line 1067 "./src/prs/prs.y"
{ 
	   yyval.List = lsCreate();
	 }
    break;
case 93:
#line 1071 "./src/prs/prs.y"
{ 
	   yyval.List = lsCreate();
           lsNewEnd(yyval.List, (lsGeneric) yyvsp[0].atmassignptr, LS_NH);
	 }
    break;
case 94:
#line 1076 "./src/prs/prs.y"
{
           lsNewEnd(yyvsp[-2].List, (lsGeneric) yyvsp[0].atmassignptr, LS_NH);
	   yyval.List = yyvsp[-2].List;
	}
    break;
case 95:
#line 1084 "./src/prs/prs.y"
{
          Atm_Expr_t * exp = PrsAtmExprCreate(Atm_IfThenElseFi_c, yyvsp[-3].atmexprptr, yyvsp[-1].atmexprptr);

          yyval.atmexprptr = PrsAtmExprCreate(Atm_IfThenElseFi_c, yyvsp[-5].atmexprptr, exp);
        }
    break;
case 96:
#line 1095 "./src/prs/prs.y"
{
               Var_Variable_t *var, *actualVar, *indexVar;
             
               if ((var = DesignatorCheckIsCorrect (yyvsp[-3].id, PrsModule)) ==
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
                 PrsParseValid = PrsForallAssignmentCheckIndexVariable(yyvsp[-4].id, yyvsp[-1].id);

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
                 PrsForallAssignmentCreateDummyVariable(yyvsp[-4].id, var,
                                                        PrsModule,
                                                        PrsMdlManager,
                                                        PrsTypeManager);
             FREE(yyvsp[-3].id);
             FREE(yyvsp[-1].id);

             }
    break;
case 97:
#line 1138 "./src/prs/prs.y"
{
             Var_Variable_t *indexVar;
             char *indexVarName;
             char *intString, *tmp;

             if (PrsParseValid) {
               intString = Main_ConvertIntToString(PrsDummyVarCount-1);
               tmp = util_strcat3(yyvsp[-7].id, "-", intString);
               FREE(intString);             
               indexVarName = Mdl_NamePrefixWithString(
                 Mdl_ModuleReadName(PrsModule), tmp);
               FREE(tmp);
               
               indexVar = Mdl_ModuleReadVariableFromName(indexVarName, PrsModule);
               FREE(indexVarName);
               
               yyval.atmassignptr = Atm_AssignForallAlloc (PrsAssignedVar, indexVar, yyvsp[0].atmexprptr);
               
               if (!PrsAssignmentTypeCheck(yyval.atmassignptr)) {
                 Atm_AssignFree(yyval.atmassignptr);
                 yyval.atmassignptr = NIL(Atm_Assign_t);
                 PrsParseValid = FALSE;
               }
             }
             FREE(yyvsp[-7].id);
           }
    break;
case 98:
#line 1166 "./src/prs/prs.y"
{
             Atm_Expr_t *indexExpr;
             Atm_ExprType indexExprType;
             Var_Type_t *varType, *indexType;
             Var_EnumElement_t *enumElement;
             Var_DataType dataType;
             char *varName, *actualVarName;
             Var_Variable_t *var, *actualVar;

             if ((var = DesignatorCheckIsCorrect (yyvsp[-3].id, PrsModule)) == NIL(Var_Variable_t)) 
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
                 
	       indexExpr = ExprParseDeclarator(yyvsp[-1].id);
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

             FREE(yyvsp[-3].id);
             FREE(yyvsp[-1].id);
           }
    break;
case 99:
#line 1241 "./src/prs/prs.y"
{
             if (PrsParseValid) {
               yyval.atmassignptr = Atm_AssignStdAlloc(PrsAssignedVar, yyvsp[0].atmexprptr);
               
               if (!PrsAssignmentTypeCheck(yyval.atmassignptr)) {
                 Atm_AssignFree(yyval.atmassignptr);
                 yyval.atmassignptr = NIL(Atm_Assign_t);
                 PrsParseValid = FALSE;
               }
             }
           }
    break;
case 100:
#line 1254 "./src/prs/prs.y"
{
             Var_Variable_t *var;
             Var_Type_t *varType;
             Var_DataType dataType; 
             char *varName, *actualVarName;
             Var_Variable_t *actualVar;
             int arraySize;

             if ((var = DesignatorCheckIsCorrect (yyvsp[-3].id, PrsModule)) == NIL(Var_Variable_t)) 
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
	       if (yyvsp[-1].val >= arraySize) {
		 Main_MochaErrorPrint(
                   "Error at line %d: index out of bound.\n", yylineno);
		 PrsParseValid = 0;
	       }
               else {
		 actualVarName = ALLOC(char, strlen(varName) + 1 +
				       NumberObtainSize(yyvsp[-1].val) + 1 + 1);
		 sprintf(actualVarName, "%s[%d]", varName, yyvsp[-1].val);
		 actualVar = Mdl_ModuleReadVariableFromName(actualVarName,
                                                          PrsModule);
                 PrsAssignedVar = actualVar;
		 FREE(actualVarName);
		 AtomCheckControlReadAwaitVariableRecursively(PrsModule,
							      PrsAtom, actualVar,
							      0);
	       }
	     }

             FREE(yyvsp[-3].id);
           }
    break;
case 101:
#line 1316 "./src/prs/prs.y"
{
             if (PrsParseValid) {
               yyval.atmassignptr = Atm_AssignStdAlloc(PrsAssignedVar, yyvsp[0].atmexprptr);
               
               if (!PrsAssignmentTypeCheck(yyval.atmassignptr)) {
                 Atm_AssignFree(yyval.atmassignptr);
                 yyval.atmassignptr = NIL(Atm_Assign_t);
                 PrsParseValid = FALSE;
               }
             }
           }
    break;
case 102:
#line 1329 "./src/prs/prs.y"
{
             Var_Variable_t *var;
             
             if ((var = DesignatorCheckIsCorrect (yyvsp[0].id, PrsModule)) == NIL(Var_Variable_t))
               PrsParseValid = 0;
             else 
               AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, var, 0);

             if (PrsParseValid) {
               PrsAssignedVar = var;
	       PrsAssignedVarType = Var_VariableReadType(var);
             }
             
	     FREE(yyvsp[0].id);
           }
    break;
case 103:
#line 1346 "./src/prs/prs.y"
{
             if (PrsParseValid) {
               yyval.atmassignptr = Atm_AssignStdAlloc(PrsAssignedVar, yyvsp[0].atmexprptr);
               
               if (!PrsAssignmentTypeCheck(yyval.atmassignptr)) {
                 Atm_AssignFree(yyval.atmassignptr);
                 yyval.atmassignptr = NIL(Atm_Assign_t);
                 PrsParseValid = FALSE;
               }
             }
           }
    break;
case 104:
#line 1358 "./src/prs/prs.y"
{
             Var_Variable_t *var;
             
             if ((var = DesignatorCheckIsCorrect (yyvsp[0].id, PrsModule)) == NIL(Var_Variable_t))
               PrsParseValid = 0;
             else 
               AtomCheckControlReadAwaitVariableRecursively(PrsModule, PrsAtom, var, 0);

             if (PrsParseValid) {
               PrsAssignedVar = var;
	       PrsAssignedVarType = Var_VariableReadType(var);
             }
             
	     FREE(yyvsp[0].id);
           }
    break;
case 105:
#line 1374 "./src/prs/prs.y"
{
             Atm_Expr_t *upBoundExpr;

	     if (Var_VariableReadDataType(PrsAssignedVar) != Var_Timer_c) {
		PrsParseValid = 0;
	     }

	     upBoundExpr = Atm_ExprAlloc(Atm_TimerUpperBound_c,
				(Atm_Expr_t *) (long) yyvsp[0].val,
				NIL(Atm_Expr_t));

             if (PrsParseValid) {
                yyval.atmassignptr = Atm_AssignStdAlloc(PrsAssignedVar, upBoundExpr);
                PrsAssignmentTypeCheck(yyval.atmassignptr);
	     } 
	     else {
                yyval.atmassignptr = NIL(Atm_Assign_t);
             }
           }
    break;
case 106:
#line 1394 "./src/prs/prs.y"
{
                  Var_Variable_t *var;
		  Atm_Expr_t *expr;
		  Atm_Expr_t *varexpr;
                  lsList ctrlList, readList;
                  char *privateName, *name;

                  name = ALLOC(char, yyvsp[0].name.length + 1);
                  memcpy(name, yyvsp[0].name.ptr, yyvsp[0].name.length);
                  *(name + yyvsp[0].name.length) = '\0';

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
                    yyval.atmassignptr = Atm_AssignEventAlloc(var);
                    PrsParseValid = PrsAssignmentTypeCheck(yyval.atmassignptr);
                    
                    if (!PrsParseValid) {
                      Atm_AssignFree(yyval.atmassignptr);
                      yyval.atmassignptr = NIL(Atm_Assign_t);
                    }
                  }
                }
    break;
case 107:
#line 1448 "./src/prs/prs.y"
{
                  yyval.atmexprptr = Atm_ExprAlloc(Atm_Nondet_c,
                                     (Atm_Expr_t *) PrsAssignedVarType,
                                     NIL(Atm_Expr_t));
                }
    break;
case 108:
#line 1454 "./src/prs/prs.y"
{
                  yyval.atmexprptr = yyvsp[0].atmexprptr;
                }
    break;
case 109:
#line 1460 "./src/prs/prs.y"
{ yyval.atmexprptr = yyvsp[0].atmexprptr;
		}
    break;
case 110:
#line 1467 "./src/prs/prs.y"
{
                  yyval.atmexprptr = ExprParseDeclarator(yyvsp[0].id);

                  FREE(yyvsp[0].id);   
                }
    break;
case 111:
#line 1473 "./src/prs/prs.y"
{
                  /* $$ = PrsAtmExprCreate(Atm_Equal_c, $1, $3); */
                  lsList readList, awaitList;
                  Var_Variable_t *var, *actualVar;
                  Atm_ExprType exprType;
                  char *varName = yyvsp[-3].id;
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
                  
                  if (yyvsp[-1].atmexprptr != NIL(Atm_Expr_t))
                    exprType = Atm_ExprReadType(yyvsp[-1].atmexprptr);
                  else
                    PrsParseValid = 0;
                  
                  if (PrsParseValid) {
                    if (exprType == Atm_NumConst_c) {
                      index = (int) (long) Atm_ExprReadLeftChild(yyvsp[-1].atmexprptr);
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
                                                     Atm_ExprReadLeftChild(yyvsp[-1].atmexprptr)), "]"); 
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
                        yyval.atmexprptr = Atm_ExprAlloc(Atm_PrimedVar_c, (Atm_Expr_t *)
                                           actualVar, NIL(Atm_Expr_t));
                      else
                        yyval.atmexprptr = Atm_ExprAlloc(Atm_UnPrimedVar_c, (Atm_Expr_t *)
                                           actualVar, NIL(Atm_Expr_t));
                      Atm_ExprFreeExpression(yyvsp[-1].atmexprptr);
                    }
                    else {
                      Atm_Expr_t *leftExpr;
                      if (isPrimed)
                        leftExpr = Atm_ExprAlloc(Atm_PrimedVar_c,
                                                 (Atm_Expr_t *) var, NIL(Atm_Expr_t));
                      else
                        leftExpr = Atm_ExprAlloc(Atm_UnPrimedVar_c,
                                                 (Atm_Expr_t *) var, NIL(Atm_Expr_t));
                      
                      yyval.atmexprptr = Atm_ExprAlloc(Atm_Index_c, leftExpr, yyvsp[-1].atmexprptr);
                    }
                  }
                  else
                      yyval.atmexprptr = NIL(Atm_Expr_t);

                  FREE(realName);
                  FREE(varName);
                }
    break;
case 112:
#line 1579 "./src/prs/prs.y"
{
                  /* $$ = PrsAtmExprCreate(Atm_Equal_c, $1, $3); */
                  yyval.atmexprptr=Atm_ExprAlloc(Atm_NumConst_c, 
				   (Atm_Expr_t *) (long) yyvsp[0].val, 
			           NIL(Atm_Expr_t));
                }
    break;
case 113:
#line 1586 "./src/prs/prs.y"
{ 
                  yyval.atmexprptr = Atm_ExprAlloc(Atm_BoolConst_c,
                                     (Atm_Expr_t *) (long) yyvsp[0].bool,
                                     NIL(Atm_Expr_t));
                }
    break;
case 114:
#line 1592 "./src/prs/prs.y"
{
                  yyval.atmexprptr = PrsAtmExprCreate(Atm_Plus_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr);
                }
    break;
case 115:
#line 1596 "./src/prs/prs.y"
{
                  yyval.atmexprptr = PrsAtmExprCreate(Atm_Minus_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr);
                }
    break;
case 116:
#line 1600 "./src/prs/prs.y"
{
                  yyval.atmexprptr = PrsAtmExprCreate(Atm_Equal_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr);
                }
    break;
case 117:
#line 1604 "./src/prs/prs.y"
{
                  yyval.atmexprptr = PrsAtmExprCreate(Atm_NotEqual_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr);
                }
    break;
case 118:
#line 1608 "./src/prs/prs.y"
{
                  yyval.atmexprptr = PrsAtmExprCreate(Atm_Greater_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr);
                }
    break;
case 119:
#line 1612 "./src/prs/prs.y"
{
                  yyval.atmexprptr = PrsAtmExprCreate(Atm_Less_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr);
                }
    break;
case 120:
#line 1616 "./src/prs/prs.y"
{
          Var_Variable_t *TimerVar;
          Var_Type_t *timerType;
	  long NewBound, OldBound;

          yyval.atmexprptr = PrsAtmExprCreate(Atm_GreaterEqual_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr);

          PrsTimerExpressionCheckBound(yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr);
        }
    break;
case 121:
#line 1626 "./src/prs/prs.y"
{ 
	  yyval.atmexprptr = PrsAtmExprCreate(Atm_LessEqual_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr); 

          PrsTimerExpressionCheckBound(yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr);
          
	}
    break;
case 122:
#line 1633 "./src/prs/prs.y"
{ yyval.atmexprptr = PrsAtmExprCreate(Atm_And_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr); }
    break;
case 123:
#line 1635 "./src/prs/prs.y"
{ yyval.atmexprptr = PrsAtmExprCreate(Atm_Or_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr); }
    break;
case 124:
#line 1637 "./src/prs/prs.y"
{ yyval.atmexprptr = PrsAtmExprCreate(Atm_Implies_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr); }
    break;
case 125:
#line 1639 "./src/prs/prs.y"
{ yyval.atmexprptr = PrsAtmExprCreate(Atm_Equiv_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr); }
    break;
case 126:
#line 1641 "./src/prs/prs.y"
{ yyval.atmexprptr = PrsAtmExprCreate(Atm_Xor_c, yyvsp[-2].atmexprptr, yyvsp[0].atmexprptr); }
    break;
case 127:
#line 1643 "./src/prs/prs.y"
{ yyval.atmexprptr = Atm_ExprAlloc(Atm_RedAnd_c, yyvsp[0].atmexprptr, NIL(Atm_Expr_t)); }
    break;
case 128:
#line 1645 "./src/prs/prs.y"
{ yyval.atmexprptr = Atm_ExprAlloc(Atm_RedOr_c, yyvsp[0].atmexprptr, NIL(Atm_Expr_t)); }
    break;
case 129:
#line 1647 "./src/prs/prs.y"
{ yyval.atmexprptr = Atm_ExprAlloc(Atm_RedXor_c, yyvsp[0].atmexprptr, NIL(Atm_Expr_t)); }
    break;
case 130:
#line 1649 "./src/prs/prs.y"
{
                  if (! yyvsp[0].atmexprptr)
                    yyval.atmexprptr = NIL(Atm_Expr_t);
                  else 
                    yyval.atmexprptr = Atm_ExprAlloc(Atm_UnaryMinus_c, yyvsp[0].atmexprptr, NIL(Atm_Expr_t));
                }
    break;
case 131:
#line 1656 "./src/prs/prs.y"
{
                  if (! yyvsp[0].atmexprptr)
                    yyval.atmexprptr = NIL(Atm_Expr_t);
                  else
                    yyval.atmexprptr=Atm_ExprAlloc(Atm_Not_c, yyvsp[0].atmexprptr,  NIL(Atm_Expr_t));}
    break;
case 132:
#line 1662 "./src/prs/prs.y"
{
                  yyval.atmexprptr=yyvsp[-1].atmexprptr;
                }
    break;
case 133:
#line 1666 "./src/prs/prs.y"
{

                  /* $$ = PrsAtmExprCreate(Atm_EventQuery_c, $1, $3); */
                 char *name;
                 
                 name = ALLOC(char, yyvsp[0].name.length + 1);
                 memcpy(name, yyvsp[0].name.ptr, yyvsp[0].name.length);
                 *(name + yyvsp[0].name.length) = '\0';
                 
                 yyval.atmexprptr = ExprParseEventQuery(name);
                 FREE(name);
                }
    break;
case 135:
#line 1684 "./src/prs/prs.y"
{
            yyval.val = yyvsp[0].val;
          }
    break;
case 136:
#line 1689 "./src/prs/prs.y"
{
            yyval.val = yyvsp[-2].val - yyvsp[0].val;
          }
    break;
case 137:
#line 1694 "./src/prs/prs.y"
{
            yyval.val = yyvsp[-2].val + yyvsp[0].val;
          }
    break;
case 138:
#line 1699 "./src/prs/prs.y"
{
            yyval.val = yyvsp[-2].val * yyvsp[0].val;
          }
    break;
case 139:
#line 1703 "./src/prs/prs.y"
{
            yyval.val = yyvsp[-2].val / yyvsp[0].val;
          }
    break;
case 140:
#line 1707 "./src/prs/prs.y"
{
            yyval.val = yyvsp[-2].val % yyvsp[0].val;
          }
    break;
case 141:
#line 1711 "./src/prs/prs.y"
{
            yyval.val = yyvsp[-1].val;
          }
    break;
case 142:
#line 1718 "./src/prs/prs.y"
{
                   yyval.nodepointer = PrsNodeAlloc(2, yyvsp[-3].id, yyvsp[-1].id);
                 }
    break;
case 143:
#line 1722 "./src/prs/prs.y"
{
                 yyval.nodepointer = PrsNodeAlloc(1, yyvsp[-3].id, (char *) (long) yyvsp[-1].val);
               }
    break;
case 144:
#line 1727 "./src/prs/prs.y"
{
		  if (yyvsp[-3].val != 0) {
                    Main_MochaErrorPrint(
                      "Error at line %d: range has to start at zero.\n");
                    YYERROR;
                  }
                  yyval.val = yyvsp[-1].val;
                }
    break;
case 145:
#line 1737 "./src/prs/prs.y"
{
                 char *name;
                 
                 name = ALLOC(char, yyvsp[0].name.length + 1);
                 memcpy(name, yyvsp[0].name.ptr, yyvsp[0].name.length);
                 *(name + yyvsp[0].name.length) = '\0';

                 yyval.id = name;
		}
    break;
case 146:
#line 1747 "./src/prs/prs.y"
{
                  yyval.id = util_strsav("in");
		}
    break;
case 147:
#line 1753 "./src/prs/prs.y"
{
             yyval.nodepointer = yyvsp[0].nodepointer;
           }
    break;
case 148:
#line 1759 "./src/prs/prs.y"
{
             yyval.nodepointer = PrsNodeAlloc(0, yyvsp[0].id, NIL(char));
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
#line 1763 "./src/prs/prs.y"



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
















