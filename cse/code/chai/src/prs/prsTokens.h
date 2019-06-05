#ifndef BISON_OBJ_PRSREAD_H
# define BISON_OBJ_PRSREAD_H

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


extern YYSTYPE yylval;

#endif /* not BISON_OBJ_PRSREAD_H */
