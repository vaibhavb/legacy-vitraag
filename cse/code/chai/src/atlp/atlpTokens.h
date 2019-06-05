#ifndef BISON_OBJ_ATLPREAD_H
# define BISON_OBJ_ATLPREAD_H

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


extern YYSTYPE AtlpYylval;

#endif /* not BISON_OBJ_ATLPREAD_H */
