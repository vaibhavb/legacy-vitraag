#ifndef BISON_OBJ_INVREAD_H
# define BISON_OBJ_INVREAD_H

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


extern YYSTYPE InvYylval;

#endif /* not BISON_OBJ_INVREAD_H */
