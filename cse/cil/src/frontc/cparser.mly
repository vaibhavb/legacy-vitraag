/*(*
 *
 * Copyright (c) 2001-2003,
 *  George C. Necula    <necula@cs.berkeley.edu>
 *  Scott McPeak        <smcpeak@cs.berkeley.edu>
 *  Wes Weimer          <weimer@cs.berkeley.edu>
 *  Ben Liblit          <liblit@cs.berkeley.edu>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The names of the contributors may not be used to endorse or promote
 * products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **)/
/*(* NOTE: This parser is based on a parser written by Hugues Casse. Since
   * then I have changed it in numerous ways to the point where it probably
   * does not resemble Hugues's original one at all  *)*/
%{
open Cabs
module E = Errormsg

let parse_error msg : unit =       (* sm: c++-mode highlight hack: -> ' <- *)
  E.hadErrors := true;
  E.parse_error
    msg
    (Parsing.symbol_start ()) (Parsing.symbol_end ())

let print = print_string


let currentLoc () = 
  let l, f, c = E.getPosition () in
  { lineno   = l; filename = f; byteno   = c; }

let cabslu = {lineno = -10; filename = "cabs loc unknown"; byteno = -10;}

(*
** Expression building
*)
let smooth_expression lst =
  match lst with
    [] -> NOTHING
  | [expr] -> expr
  | _ -> COMMA (lst)


let currentFunctionName = ref "<outside any function>"
    
let announceFunctionName ((n, decl, _, _):name) =
  !E.add_identifier n;
  (* Start a context that includes the parameter names and the whole body. 
   * Will pop when we finish parsing the function body *)
  !E.push_context ();
  (* Go through all the parameter names and mark them as identifiers *)
  let rec findProto = function
      PROTO (d, args, _) when isJUSTBASE d -> 
        List.iter (fun (_, (an, _, _, _)) -> !E.add_identifier an) args

    | PROTO (d, _, _) -> findProto d
    | PARENTYPE (_, d, _) -> findProto d
    | PTR (_, d) -> findProto d
    | ARRAY (d, _, _) -> findProto d
    | _ -> parse_error "Cannot find the prototype in a function definition";
           raise Parsing.Parse_error 

  and isJUSTBASE = function
      JUSTBASE -> true
    | PARENTYPE (_, d, _) -> isJUSTBASE d
    | _ -> false
  in
  findProto decl;
  currentFunctionName := n



let applyPointer (ptspecs: attribute list list) (dt: decl_type)  
       : decl_type = 
  (* Outer specification first *)
  let rec loop = function
      [] -> dt
    | attrs :: rest -> PTR(attrs, loop rest)
  in
  loop ptspecs

let doDeclaration (loc: cabsloc) (specs: spec_elem list) (nl: init_name list) : definition = 
  if isTypedef specs then begin
    (* Tell the lexer about the new type names *)
    List.iter (fun ((n, _, _, _), _) -> !E.add_type n) nl;
    TYPEDEF ((specs, List.map (fun (n, _) -> n) nl), loc)
  end else
    if nl = [] then
      ONLYTYPEDEF (specs, loc)
    else begin
      (* Tell the lexer about the new variable names *)
      List.iter (fun ((n, _, _, _), _) -> !E.add_identifier n) nl;
      DECDEF ((specs, nl), loc)  
    end


let doFunctionDef (loc: cabsloc)
                  (lend: cabsloc)
                  (specs: spec_elem list) 
                  (n: name) 
                  (b: block) : definition = 
  let fname = (specs, n) in
  FUNDEF (fname, b, loc, lend)


let doOldParDecl (names: string list)
                 ((pardefs: name_group list), (isva: bool)) 
    : single_name list * bool =
  let findOneName n =
    (* Search in pardefs for the definition for this parameter *)
    let rec loopGroups = function
        [] -> ([SpecType Tint], (n, JUSTBASE, [], cabslu))
      | (specs, names) :: restgroups ->
          let rec loopNames = function
              [] -> loopGroups restgroups
            | ((n',_, _, _) as sn) :: _ when n' = n -> (specs, sn)
            | _ :: restnames -> loopNames restnames
          in
          loopNames names
    in
    loopGroups pardefs
  in
  let args = List.map findOneName names in
  (args, isva)

let checkConnective (s : string) : unit =
begin
  (* checking this means I could possibly have more connectives, with *)
  (* different meaning *)
  if (s <> "to") then (
    parse_error "transformer connective must be 'to'";
    raise Parsing.Parse_error
  )
  else ()
end

(* takes a not-nul-terminated list, and converts it to a string. *)
let rec intlist_to_string (str: int64 list):string =
  match str with
    [] -> ""  (* add nul-termination *)
  | value::rest ->
      let this_char = 
	if (compare value (Int64.of_int 255) > 0) 
           || (compare value Int64.zero < 0)
	then begin
	  let msg = Printf.sprintf "character 0x%Lx too big" value in
	  parse_error msg;
	  raise Parsing.Parse_error
	end 
	else 
	  String.make 1 (Char.chr (Int64.to_int value))
      in
      this_char ^ (intlist_to_string rest)

let fst3 (result, _, _) = result
let snd3 (_, result, _) = result
let trd3 (_, _, result) = result

(* for collecting declarations and statements uniformly *)
type blockElement =
  BE_Decl of definition
| BE_Stmt of statement

(* extract the declarations *)
let rec filterBEDecls (elts : blockElement list) : definition list =
  match elts with
  | [] -> []
  | BE_Decl(d) :: rest -> d :: (filterBEDecls rest)
  | BE_Stmt(_) :: rest -> (filterBEDecls rest)

(* extract the statments *)
let rec filterBEStmts (elts : blockElement list) : statement list =
  match elts with
  | [] -> []
  | BE_Decl(_) :: rest -> (filterBEStmts rest)
  | BE_Stmt(s) :: rest -> s :: (filterBEStmts rest)

%}

%token <string * Cabs.cabsloc> IDENT
%token <int64 list * Cabs.cabsloc> CST_CHAR
%token <int64 list * Cabs.cabsloc> CST_WCHAR
%token <string * Cabs.cabsloc> CST_INT
%token <string * Cabs.cabsloc> CST_FLOAT
%token <string * Cabs.cabsloc> NAMED_TYPE

/* Each character is its own list element, and the terminating nul is not
   included in this list. */
%token <int64 list * Cabs.cabsloc> CST_STRING   
%token <int64 list * Cabs.cabsloc> CST_WSTRING

%token EOF
%token<Cabs.cabsloc> CHAR INT DOUBLE FLOAT VOID INT64 INT32
%token<Cabs.cabsloc> ENUM STRUCT TYPEDEF UNION
%token<Cabs.cabsloc> SIGNED UNSIGNED LONG SHORT
%token<Cabs.cabsloc> VOLATILE EXTERN STATIC CONST RESTRICT AUTO REGISTER
%token<Cabs.cabsloc> THREAD

%token<Cabs.cabsloc> SIZEOF ALIGNOF

%token EQ PLUS_EQ MINUS_EQ STAR_EQ SLASH_EQ PERCENT_EQ
%token AND_EQ PIPE_EQ CIRC_EQ INF_INF_EQ SUP_SUP_EQ
%token ARROW DOT

%token EQ_EQ EXCLAM_EQ INF SUP INF_EQ SUP_EQ
%token<Cabs.cabsloc> PLUS MINUS STAR
%token SLASH PERCENT
%token<Cabs.cabsloc> TILDE AND
%token PIPE CIRC
%token<Cabs.cabsloc> EXCLAM AND_AND
%token PIPE_PIPE
%token INF_INF SUP_SUP
%token<Cabs.cabsloc> PLUS_PLUS MINUS_MINUS

%token RPAREN 
%token<Cabs.cabsloc> LPAREN RBRACE
%token<Cabs.cabsloc> LBRACE
%token LBRACKET RBRACKET
%token COLON
%token<Cabs.cabsloc> SEMICOLON
%token COMMA ELLIPSIS QUEST

%token<Cabs.cabsloc> BREAK CONTINUE GOTO RETURN
%token<Cabs.cabsloc> SWITCH CASE DEFAULT
%token<Cabs.cabsloc> WHILE DO FOR
%token<Cabs.cabsloc> IF
%token ELSE

%token<Cabs.cabsloc> ATTRIBUTE INLINE ASM TYPEOF FUNCTION__ PRETTY_FUNCTION__
%token LABEL__
%token<Cabs.cabsloc> BUILTIN_VA_ARG
%token BUILTIN_VA_LIST
%token BLOCKATTRIBUTE
%token<Cabs.cabsloc> DECLSPEC
%token<string * Cabs.cabsloc> MSASM MSATTR
%token<Cabs.cabsloc> PRAGMA

/* sm: cabs tree transformation specification keywords */
%token<Cabs.cabsloc> AT_TRANSFORM AT_TRANSFORMEXPR AT_SPECIFIER AT_EXPR
%token AT_NAME

/* operator precedence */
%nonassoc 	IF
%nonassoc 	ELSE


%left	COMMA
%right	EQ PLUS_EQ MINUS_EQ STAR_EQ SLASH_EQ PERCENT_EQ
                AND_EQ PIPE_EQ CIRC_EQ INF_INF_EQ SUP_SUP_EQ
%right	QUEST COLON
%left	PIPE_PIPE
%left	AND_AND
%left	PIPE
%left 	CIRC
%left	AND
%left	EQ_EQ EXCLAM_EQ
%left	INF SUP INF_EQ SUP_EQ
%left	INF_INF SUP_SUP
%left	PLUS MINUS
%left	STAR SLASH PERCENT CONST RESTRICT VOLATILE
%right	EXCLAM TILDE PLUS_PLUS MINUS_MINUS CAST RPAREN ADDROF SIZEOF ALIGNOF
%left 	LBRACKET
%left	DOT ARROW LPAREN LBRACE
%right  NAMED_TYPE     /* We'll use this to handle redefinitions of
                        * NAMED_TYPE as variables */
%left   IDENT

/* Non-terminals informations */
%start interpret file
%type <Cabs.definition list> file interpret globals

%type <Cabs.definition> global


%type <Cabs.attribute list> attributes attributes_with_asm asmattr
%type <Cabs.statement> statement
%type <Cabs.constant * cabsloc> constant
%type <string * cabsloc> string_constant
%type <Cabs.expression * cabsloc> expression
%type <Cabs.expression> opt_expression
%type <Cabs.init_expression> init_expression
%type <Cabs.expression list * cabsloc> comma_expression
%type <Cabs.expression list * cabsloc> paren_comma_expression
%type <Cabs.expression list> arguments
%type <Cabs.expression list> bracket_comma_expression
%type <int64 list * cabsloc> string_list 
%type <int64 list * cabsloc> wstring_list

%type <Cabs.initwhat * Cabs.init_expression> initializer
%type <(Cabs.initwhat * Cabs.init_expression) list> initializer_list
%type <Cabs.initwhat> init_designators init_designators_opt

%type <spec_elem list * cabsloc> decl_spec_list
%type <typeSpecifier * cabsloc> type_spec
%type <Cabs.field_group list> struct_decl_list


%type <Cabs.name> old_proto_decl
%type <Cabs.single_name> parameter_decl
%type <Cabs.enum_item> enumerator
%type <Cabs.enum_item list> enum_list
%type <Cabs.definition> declaration function_def
%type <cabsloc * spec_elem list * name> function_def_start
%type <Cabs.spec_elem list * Cabs.decl_type> type_name
%type <Cabs.block * cabsloc * cabsloc> block
%type <blockElement list> block_element_list
%type <string list> local_labels local_label_names
%type <string list> old_parameter_list_ne

%type <Cabs.init_name> init_declarator
%type <Cabs.init_name list> init_declarator_list
%type <Cabs.name> declarator
%type <Cabs.name * expression option> field_decl
%type <(Cabs.name * expression option) list> field_decl_list
%type <string * Cabs.decl_type> direct_decl
%type <Cabs.decl_type> abs_direct_decl abs_direct_decl_opt
%type <Cabs.decl_type * Cabs.attribute list> abstract_decl
%type <attribute list list * cabsloc> pointer pointer_opt /* Each element is a "* <type_quals_opt>" */
%type <Cabs.cabsloc> location
%type <Cabs.spec_elem * cabsloc> cvspec
%%

interpret:
  file EOF				{$1}
;
file: globals				{$1}
;
globals:
  /* empty */                           { [] }
| global globals                        { $1 :: $2 }
| SEMICOLON globals                     { $2 }
;

location:
   /* empty */                	{ currentLoc () }  %prec IDENT


/*** Global Definition ***/
global:
| declaration                           { $1 }
| function_def                          { $1 } 
| ASM LPAREN string_constant RPAREN SEMICOLON
                                        { GLOBASM (fst $3, $1) }
| PRAGMA attr				{ PRAGMA ($2, $1) }
/* (* Old-style function prototype. This should be somewhere else, like in
    * "declaration". For now we keep it at global scope only because in local
    * scope it looks too much like a function call  *) */
| IDENT LPAREN old_parameter_list_ne RPAREN old_pardef_list SEMICOLON
                           { (* Convert pardecl to new style *)
                             let pardecl, isva = doOldParDecl $3 $5 in
                             (* Make the function declarator *)
                             doDeclaration (snd $1) []
                               [((fst $1, PROTO(JUSTBASE, pardecl,isva), [], cabslu),
                                 NO_INIT)]
                            }
/* (* Old style function prototype, but without any arguments *) */
| IDENT LPAREN RPAREN  SEMICOLON
                           { (* Make the function declarator *)
                             doDeclaration (snd $1) []
                               [((fst $1, PROTO(JUSTBASE,[],false), [], cabslu),
                                 NO_INIT)]
                            }
/* transformer for a toplevel construct */
| AT_TRANSFORM LBRACE global RBRACE  IDENT/*to*/  LBRACE globals RBRACE {
    checkConnective(fst $5);
    TRANSFORMER($3, $7, $1)
  }
/* transformer for an expression */
| AT_TRANSFORMEXPR LBRACE expression RBRACE  IDENT/*to*/  LBRACE expression RBRACE {
    checkConnective(fst $5);
    EXPRTRANSFORMER(fst $3, fst $7, $1)
  }
| location error SEMICOLON { PRAGMA (VARIABLE "parse_error", $1) }
;

id_or_typename:
    IDENT				{fst $1}
|   NAMED_TYPE				{fst $1}
|   AT_NAME LPAREN IDENT RPAREN         { "@name(" ^ fst $3 ^ ")" }     /* pattern variable name */
;

maybecomma:
   /* empty */                          { () }
|  COMMA                                { () }
;

/* *** Expressions *** */


expression:
        	constant
		        {CONSTANT (fst $1), snd $1}
|		IDENT
		        {VARIABLE (fst $1), snd $1}
|		SIZEOF expression
		        {EXPR_SIZEOF (fst $2), $1}
|	 	SIZEOF LPAREN type_name RPAREN
		        {let b, d = $3 in TYPE_SIZEOF (b, d), $1}
|		ALIGNOF expression
		        {EXPR_ALIGNOF (fst $2), $1}
|	 	ALIGNOF LPAREN type_name RPAREN
		        {let b, d = $3 in TYPE_ALIGNOF (b, d), $1}
|		PLUS expression
		        {UNARY (PLUS, fst $2), $1}
|		MINUS expression
		        {UNARY (MINUS, fst $2), $1}
|		STAR expression
		        {UNARY (MEMOF, fst $2), $1}
|		AND expression				%prec ADDROF
		        {UNARY (ADDROF, fst $2), $1}
|		EXCLAM expression
		        {UNARY (NOT, fst $2), $1}
|		TILDE expression
		        {UNARY (BNOT, fst $2), $1}
|		PLUS_PLUS expression                    %prec CAST
		        {UNARY (PREINCR, fst $2), $1}
|		expression PLUS_PLUS
		        {UNARY (POSINCR, fst $1), snd $1}
|		MINUS_MINUS expression                  %prec CAST
		        {UNARY (PREDECR, fst $2), $1}
|		expression MINUS_MINUS
		        {UNARY (POSDECR, fst $1), snd $1}
|		expression ARROW id_or_typename
		        {MEMBEROFPTR (fst $1, $3), snd $1}
|		expression DOT id_or_typename
		        {MEMBEROF (fst $1, $3), snd $1}
|		LPAREN block RPAREN
		        { GNU_BODY (fst3 $2), $1 }
|		paren_comma_expression
		        {smooth_expression (fst $1), snd $1}
|		expression LPAREN arguments RPAREN
			{CALL (fst $1, $3), snd $1}
|               BUILTIN_VA_ARG LPAREN expression COMMA type_name RPAREN
                        { let b, d = $5 in
                          CALL (VARIABLE "__builtin_va_arg", 
                                [fst $3; TYPE_SIZEOF (b, d)]), $1 }
|		expression bracket_comma_expression
			{INDEX (fst $1, smooth_expression $2), snd $1}
|		expression QUEST opt_expression COLON expression
			{QUESTION (fst $1, $3, fst $5), snd $1}
|		expression PLUS expression
			{BINARY(ADD, fst $1, fst $3), snd $1}
|		expression MINUS expression
			{BINARY(SUB, fst $1, fst $3), snd $1}
|		expression STAR expression
			{BINARY(MUL, fst $1, fst $3), snd $1}
|		expression SLASH expression
			{BINARY(DIV, fst $1, fst $3), snd $1}
|		expression PERCENT expression
			{BINARY(MOD, fst $1, fst $3), snd $1}
|		expression AND_AND expression
			{BINARY(AND, fst $1, fst $3), snd $1}
|		expression PIPE_PIPE expression
			{BINARY(OR, fst $1, fst $3), snd $1}
|		expression AND expression
			{BINARY(BAND, fst $1, fst $3), snd $1}
|		expression PIPE expression
			{BINARY(BOR, fst $1, fst $3), snd $1}
|		expression CIRC expression
			{BINARY(XOR, fst $1, fst $3), snd $1}
|		expression EQ_EQ expression
			{BINARY(EQ, fst $1, fst $3), snd $1}
|		expression EXCLAM_EQ expression
			{BINARY(NE, fst $1, fst $3), snd $1}
|		expression INF expression
			{BINARY(LT, fst $1, fst $3), snd $1}
|		expression SUP expression
			{BINARY(GT, fst $1, fst $3), snd $1}
|		expression INF_EQ expression
			{BINARY(LE, fst $1, fst $3), snd $1}
|		expression SUP_EQ expression
			{BINARY(GE, fst $1, fst $3), snd $1}
|		expression  INF_INF expression
			{BINARY(SHL, fst $1, fst $3), snd $1}
|		expression  SUP_SUP expression
			{BINARY(SHR, fst $1, fst $3), snd $1}
|		expression EQ expression
			{BINARY(ASSIGN, fst $1, fst $3), snd $1}
|		expression PLUS_EQ expression
			{BINARY(ADD_ASSIGN, fst $1, fst $3), snd $1}
|		expression MINUS_EQ expression
			{BINARY(SUB_ASSIGN, fst $1, fst $3), snd $1}
|		expression STAR_EQ expression
			{BINARY(MUL_ASSIGN, fst $1, fst $3), snd $1}
|		expression SLASH_EQ expression
			{BINARY(DIV_ASSIGN, fst $1, fst $3), snd $1}
|		expression PERCENT_EQ expression
			{BINARY(MOD_ASSIGN, fst $1, fst $3), snd $1}
|		expression AND_EQ expression
			{BINARY(BAND_ASSIGN, fst $1, fst $3), snd $1}
|		expression PIPE_EQ expression
			{BINARY(BOR_ASSIGN, fst $1, fst $3), snd $1}
|		expression CIRC_EQ expression
			{BINARY(XOR_ASSIGN, fst $1, fst $3), snd $1}
|		expression INF_INF_EQ expression	
			{BINARY(SHL_ASSIGN, fst $1, fst $3), snd $1}
|		expression SUP_SUP_EQ expression
			{BINARY(SHR_ASSIGN, fst $1, fst $3), snd $1}
|		LPAREN type_name RPAREN expression
		         { CAST($2, SINGLE_INIT (fst $4)), $1 }
/* (* We handle GCC constructor expressions *) */
|		LPAREN type_name RPAREN LBRACE initializer_list_opt RBRACE
		         { CAST($2, COMPOUND_INIT $5), $1 }
/* (* GCC's address of labels *)  */
|               AND_AND IDENT  { LABELADDR (fst $2), $1 }
|               AT_EXPR LPAREN IDENT RPAREN         /* expression pattern variable */
                         { EXPR_PATTERN(fst $3), $1 }
;

constant:
    CST_INT				{CONST_INT (fst $1), snd $1}
|   CST_FLOAT				{CONST_FLOAT (fst $1), snd $1}
|   CST_CHAR				{CONST_CHAR (fst $1), snd $1}
|   CST_WCHAR				{CONST_WCHAR (fst $1), snd $1}
|   string_constant			{CONST_STRING (fst $1), snd $1}
/*add a nul to strings.  We do this here (rather than in the lexer) to make
  concatenation easy below.*/
|   wstring_list			{CONST_WSTRING (fst $1 @ [Int64.zero]), snd $1}
;

string_constant:
/* Now that we know this constant isn't part of a wstring, convert it
   back to a string for easy viewing. */
    string_list                         {intlist_to_string (fst $1), snd $1 }
;
one_string_constant:
/* Don't concat multiple strings.  For asm templates. */
    CST_STRING                          {intlist_to_string (fst $1) }
;
string_list:
    one_string                          { $1 }
|   string_list one_string              { (fst $1) @ (fst $2), snd $1 }
;

wstring_list:
    CST_WSTRING                         { $1 }
|   wstring_list one_string             { (fst $1) @ (fst $2), snd $1 }
|   wstring_list CST_WSTRING            { (fst $1) @ (fst $2), snd $1 }
/* Only the first string in the list needs an L, so L"a" "b" is the same
 * as L"ab" or L"a" L"b". */

one_string: 
    CST_STRING				{$1}
|   FUNCTION__                          {(Cabs.explodeStringToInts 
					    !currentFunctionName), $1}
|   PRETTY_FUNCTION__                   {(Cabs.explodeStringToInts 
					    !currentFunctionName), $1}
;    

init_expression:
     expression         { SINGLE_INIT (fst $1) }
|    LBRACE initializer_list_opt RBRACE
			{ COMPOUND_INIT $2}

initializer_list:    /* ISO 6.7.8. Allow a trailing COMMA */
    initializer                             { [$1] }
|   initializer COMMA initializer_list_opt  { $1 :: $3 }
;
initializer_list_opt:
    /* empty */                             { [] }
|   initializer_list                        { $1 }
;
initializer: 
    init_designators eq_opt init_expression { ($1, $3) }
|   gcc_init_designators init_expression { ($1, $2) }
|                       init_expression { (NEXT_INIT, $1) }
;
eq_opt: 
   EQ                        { () }
   /*(* GCC allows missing = *)*/
|  /*(* empty *)*/               { () }
;
init_designators: 
    DOT id_or_typename init_designators_opt      { INFIELD_INIT($2, $3) }
|   LBRACKET  expression RBRACKET init_designators_opt
                                        { ATINDEX_INIT(fst $2, $4) }
|   LBRACKET  expression ELLIPSIS expression RBRACKET
                                        { ATINDEXRANGE_INIT(fst $2, fst $4) }
;         
init_designators_opt:
   /* empty */                          { NEXT_INIT }
|  init_designators                     { $1 }
;

gcc_init_designators:  /*(* GCC supports these strange things *)*/
   id_or_typename COLON                 { INFIELD_INIT($1, NEXT_INIT) }
;

arguments: 
                /* empty */         { [] }
|               comma_expression    { fst $1 }
;

opt_expression:
	        /* empty */
	        	{NOTHING}
|	        comma_expression
	        	{smooth_expression (fst $1)}
;

comma_expression:
	        expression                        {[fst $1], snd $1}
|               expression COMMA comma_expression { fst $1 :: fst $3, snd $1 }
|               error COMMA comma_expression      { $3 }
;

comma_expression_opt:
                /* empty */         { NOTHING }
|               comma_expression    { smooth_expression (fst $1) }
;

paren_comma_expression:
  LPAREN comma_expression RPAREN                   { $2 }
| LPAREN error RPAREN                              { [], $1 }
;

bracket_comma_expression:
  LBRACKET comma_expression RBRACKET                   { fst $2 }
| LBRACKET error RBRACKET                              { [] }
;


/*** statements ***/
block: /* ISO 6.8.2 */
    block_begin local_labels block_attrs block_element_list RBRACE
                                         {!E.pop_context();
                                          { blabels = $2;
                                            battrs = $3;
                                            bdefs = filterBEDecls($4);
                                            bstmts = filterBEStmts($4); },
					    $1, $5
                                         } 
|   error location RBRACE                { { blabels = [];
                                             battrs  = [];
                                             bdefs   = [];
                                             bstmts  = [] },
					     $2, $3
                                         }
;
block_begin:
    LBRACE      		         {!E.push_context (); $1}
;

block_attrs:
   /* empty */                                              { [] }
|  BLOCKATTRIBUTE paren_attr_list_ne
                                        { [("__blockattribute__", $2)] }
;

/* statements and declarations in a block, in any order (for C99 support) */
block_element_list:
    /* empty */                          { [] }
|   declaration block_element_list       { BE_Decl($1) :: $2 }
|   statement block_element_list         { BE_Stmt($1) :: $2 }
/*(* GCC accepts a label at the end of a block *)*/
|   IDENT COLON	                         { [ BE_Stmt(LABEL (fst $1, NOP (snd $1), snd $1))] }
;

local_labels:
   /* empty */                                       { [] }
|  LABEL__ local_label_names SEMICOLON local_labels  { $2 @ $4 }
;
local_label_names: 
   IDENT                                 { [ fst $1 ] }
|  IDENT COMMA local_label_names         { fst $1 :: $3 }
;



statement:
    SEMICOLON		{NOP $1 }
|   comma_expression SEMICOLON
	        	{COMPUTATION (smooth_expression (fst $1), snd $1)}
|   block               {BLOCK (fst3 $1, snd3 $1)}
|   IF paren_comma_expression statement                    %prec IF
                	{IF (smooth_expression (fst $2), $3, NOP $1, $1)}
|   IF paren_comma_expression statement ELSE statement
	                {IF (smooth_expression (fst $2), $3, $5, $1)}
|   SWITCH paren_comma_expression statement
                        {SWITCH (smooth_expression (fst $2), $3, $1)}
|   WHILE paren_comma_expression statement
	        	{WHILE (smooth_expression (fst $2), $3, $1)}
|   DO statement WHILE paren_comma_expression SEMICOLON
	        	         {DOWHILE (smooth_expression (fst $4), $2, $1)}
|   FOR LPAREN for_clause opt_expression
	        SEMICOLON opt_expression RPAREN statement
	                         {FOR ($3, $4, $6, $8, $1)}
|   IDENT COLON statement
		                 {LABEL (fst $1, $3, snd $1)}
|   CASE expression COLON
	                         {CASE (fst $2, NOP $1, $1)}
|   CASE expression ELLIPSIS expression COLON
	                         {CASERANGE (fst $2, fst $4, NOP $1, $1)}
|   DEFAULT COLON
	                         {DEFAULT (NOP $1, $1)}
|   RETURN SEMICOLON		 {RETURN (NOTHING, $1)}
|   RETURN comma_expression SEMICOLON
	                         {RETURN (smooth_expression (fst $2), $1)}
|   BREAK SEMICOLON     {BREAK $1}
|   CONTINUE SEMICOLON	 {CONTINUE $1}
|   GOTO IDENT SEMICOLON
		                 {GOTO (fst $2, $1)}
|   GOTO STAR comma_expression SEMICOLON 
                                 { COMPGOTO (smooth_expression (fst $3), $1) }
|   ASM asmattr LPAREN asmtemplate asmoutputs RPAREN SEMICOLON
                        { let (outs,ins,clobs) = $5 in
                          ASM ($2, $4, outs, ins, clobs, $1) }
|   MSASM               { ASM ([], [fst $1], [], [], [], snd $1)}
|   error location   SEMICOLON   { (NOP $2)}
;


for_clause: 
    opt_expression SEMICOLON     { FC_EXP $1 }
|   declaration                  { FC_DECL $1 }
;

declaration:                                /* ISO 6.7.*/
    decl_spec_list init_declarator_list SEMICOLON
                                       { doDeclaration (snd $1) (fst $1) $2 }
|   decl_spec_list SEMICOLON	       { doDeclaration (snd $1) (fst $1) [] }
;
init_declarator_list:                       /* ISO 6.7 */
    init_declarator                              { [$1] }
|   init_declarator COMMA init_declarator_list   { $1 :: $3 }

;
init_declarator:                             /* ISO 6.7 */
    declarator                          { ($1, NO_INIT) }
|   declarator EQ init_expression
                                        { ($1, $3) }
;

decl_spec_list:                         /* ISO 6.7 */
                                        /* ISO 6.7.1 */
|   TYPEDEF decl_spec_list_opt          { SpecTypedef :: $2, $1  }    
|   EXTERN decl_spec_list_opt           { SpecStorage EXTERN :: $2, $1 }
|   STATIC  decl_spec_list_opt          { SpecStorage STATIC :: $2, $1 }
|   AUTO   decl_spec_list_opt           { SpecStorage AUTO :: $2, $1 }
|   REGISTER decl_spec_list_opt         { SpecStorage REGISTER :: $2, $1}
                                        /* ISO 6.7.2 */
|   type_spec decl_spec_list_opt_no_named { SpecType (fst $1) :: $2, snd $1 }
                                        /* ISO 6.7.4 */
|   INLINE decl_spec_list_opt           { SpecInline :: $2, $1 }
|   cvspec decl_spec_list_opt           { (fst $1) :: $2, snd $1 }
|   attribute_nocv decl_spec_list_opt   { SpecAttr (fst $1) :: $2, snd $1 }
/* specifier pattern variable (must be last in spec list) */
|   AT_SPECIFIER LPAREN IDENT RPAREN    { [ SpecPattern(fst $3) ], $1 }
;
/* (* In most cases if we see a NAMED_TYPE we must shift it. Thus we declare 
    * NAMED_TYPE to have right associativity  *) */
decl_spec_list_opt: 
    /* empty */                         { [] } %prec NAMED_TYPE
|   decl_spec_list                      { fst $1 }
;
/* (* We add this separate rule to handle the special case when an appearance 
    * of NAMED_TYPE should not be considered as part of the specifiers but as 
    * part of the declarator. IDENT has higher precedence than NAMED_TYPE  *)
 */
decl_spec_list_opt_no_named: 
    /* empty */                         { [] } %prec IDENT
|   decl_spec_list                      { fst $1 }
;
type_spec:   /* ISO 6.7.2 */
    VOID            { Tvoid, $1}
|   CHAR            { Tchar, $1 }
|   SHORT           { Tshort, $1 }
|   INT             { Tint, $1 }
|   LONG            { Tlong, $1 }
|   INT64           { Tint64, $1 }
|   FLOAT           { Tfloat, $1 }
|   DOUBLE          { Tdouble, $1 }
|   SIGNED          { Tsigned, $1 }
|   UNSIGNED        { Tunsigned, $1 }
|   STRUCT                 id_or_typename
                                                   { Tstruct ($2, None,    []), $1 }
|   STRUCT                 id_or_typename LBRACE struct_decl_list RBRACE
                                                   { Tstruct ($2, Some $4, []), $1 }
|   STRUCT                                LBRACE struct_decl_list RBRACE
                                                   { Tstruct ("", Some $3, []), $1 }
|   STRUCT just_attributes id_or_typename LBRACE struct_decl_list RBRACE
                                                   { Tstruct ($3, Some $5, $2), $1 }
|   STRUCT just_attributes                LBRACE struct_decl_list RBRACE
                                                   { Tstruct ("", Some $4, $2), $1 }
|   UNION                  id_or_typename
                                                   { Tunion  ($2, None,    []), $1 }
|   UNION                  id_or_typename LBRACE struct_decl_list RBRACE
                                                   { Tunion  ($2, Some $4, []), $1 }
|   UNION                                 LBRACE struct_decl_list RBRACE
                                                   { Tunion  ("", Some $3, []), $1 }
|   UNION  just_attributes id_or_typename LBRACE struct_decl_list RBRACE
                                                   { Tunion  ($3, Some $5, $2), $1 }
|   UNION  just_attributes                LBRACE struct_decl_list RBRACE
                                                   { Tunion  ("", Some $4, $2), $1 }
|   ENUM                   id_or_typename
                                                   { Tenum   ($2, None,    []), $1 }
|   ENUM                   id_or_typename LBRACE enum_list maybecomma RBRACE
                                                   { Tenum   ($2, Some $4, []), $1 }
|   ENUM                                  LBRACE enum_list maybecomma RBRACE
                                                   { Tenum   ("", Some $3, []), $1 }
|   ENUM   just_attributes id_or_typename LBRACE enum_list maybecomma RBRACE
                                                   { Tenum   ($3, Some $5, $2), $1 }
|   ENUM   just_attributes                LBRACE enum_list maybecomma RBRACE
                                                   { Tenum   ("", Some $4, $2), $1 }
|   NAMED_TYPE      { Tnamed (fst $1), snd $1 }
|   TYPEOF LPAREN expression RPAREN     { TtypeofE (fst $3), $1 }
|   TYPEOF LPAREN type_name RPAREN      { let s, d = $3 in
                                          TtypeofT (s, d), $1 }
;
struct_decl_list: /* (* ISO 6.7.2. Except that we allow empty structs. We 
                      * also allow missing field names. *)
                   */
   /* empty */                           { [] }
|  decl_spec_list                 SEMICOLON struct_decl_list
                                         { (fst $1, 
                                            [(missingFieldDecl, None)]) :: $3 }
|  decl_spec_list field_decl_list SEMICOLON struct_decl_list
                                          { (fst $1, $2) 
                                            :: $4 }
|  error                          SEMICOLON struct_decl_list
                                          { $3 } 
;
field_decl_list: /* (* ISO 6.7.2 *) */
    field_decl                           { [$1] }
|   field_decl COMMA field_decl_list     { $1 :: $3 }
;
field_decl: /* (* ISO 6.7.2. Except that we allow unnamed fields. *) */
|   declarator                      { ($1, None) }
|   declarator COLON expression     { ($1, Some (fst $3)) }    
|              COLON expression     { (missingFieldDecl, Some (fst $2)) }
;

enum_list: /* (* ISO 6.7.2.2 *) */
    enumerator				{[$1]}
|   enum_list COMMA enumerator	        {$1 @ [$3]}
|   enum_list COMMA error               { $1 } 
;
enumerator:	
    IDENT			{(fst $1, NOTHING, snd $1)}
|   IDENT EQ expression		{(fst $1, fst $3, snd $1)}
;


declarator:  /* (* ISO 6.7.5. Plus Microsoft declarators.*) */
   pointer_opt direct_decl attributes_with_asm
                                         { let (n, decl) = $2 in
                                           (n, applyPointer (fst $1) decl, $3, snd $1) }
;


direct_decl: /* (* ISO 6.7.5 *) */
                                   /* (* We want to be able to redefine named
                                    * types as variable names *) */
|   id_or_typename                 { ($1, JUSTBASE) }

|   LPAREN attributes declarator RPAREN
                                   { let (n,decl,al,loc) = $3 in
                                     (n, PARENTYPE($2,decl,al)) }

|   direct_decl LBRACKET attributes comma_expression_opt RBRACKET
                                   { let (n, decl) = $1 in
                                     (n, ARRAY(decl, $3, $4)) }
|   direct_decl LBRACKET attributes error RBRACKET
                                   { let (n, decl) = $1 in
                                     (n, ARRAY(decl, $3, NOTHING)) }
|   direct_decl parameter_list_startscope rest_par_list RPAREN
                                   { let (n, decl) = $1 in
                                     let (params, isva) = $3 in
                                     !E.pop_context ();
                                     (n, PROTO(decl, params, isva))
                                   }
;
parameter_list_startscope: 
    LPAREN                         { !E.push_context () }
;
rest_par_list:
|   /* empty */                    { ([], false) }
|   parameter_decl rest_par_list1  { let (params, isva) = $2 in 
                                     ($1 :: params, isva) 
                                   }
;
rest_par_list1: 
    /* empty */                         { ([], false) }
|   COMMA ELLIPSIS                      { ([], true) }
|   COMMA parameter_decl rest_par_list1 { let (params, isva) = $3 in 
                                          ($2 :: params, isva)
                                        }  
;    


parameter_decl: /* (* ISO 6.7.5 *) */
   decl_spec_list declarator              { (fst $1, $2) }
|  decl_spec_list abstract_decl           { let d, a = $2 in
                                            (fst $1, ("", d, a, cabslu)) }
|  decl_spec_list                         { (fst $1, ("", JUSTBASE, [], cabslu)) }
|  LPAREN parameter_decl RPAREN           { $2 } 
;

/* (* Old style prototypes. Like a declarator *) */
old_proto_decl:
  pointer_opt direct_old_proto_decl   { let (n, decl, a) = $2 in
					  (n, applyPointer (fst $1) decl, a, snd $1) }
;
direct_old_proto_decl:
  direct_decl LPAREN old_parameter_list_ne RPAREN old_pardef_list
                                   { let par_decl, isva = doOldParDecl $3 $5 in
                                     let n, decl = $1 in
                                     (n, PROTO(decl, par_decl, isva), [])
                                   }
| direct_decl LPAREN                       RPAREN
                                   { let n, decl = $1 in
                                     (n, PROTO(decl, [], false), [])
                                   }
;

old_parameter_list_ne:
|  IDENT                                       { [fst $1] }
|  IDENT COMMA old_parameter_list_ne           { let rest = $3 in
                                                 (fst $1 :: rest) }
;

old_pardef_list: 
   /* empty */                            { ([], false) }
|  decl_spec_list old_pardef SEMICOLON ELLIPSIS
                                          { ([(fst $1, $2)], true) }  
|  decl_spec_list old_pardef SEMICOLON old_pardef_list  
                                          { let rest, isva = $4 in
                                            ((fst $1, $2) :: rest, isva) 
                                          }
;

old_pardef: 
   declarator                             { [$1] }
|  declarator COMMA old_pardef            { $1 :: $3 }
|  error                                  { [] }
;


pointer: /* (* ISO 6.7.5 *) */ 
   STAR attributes pointer_opt  { $2 :: fst $3, $1 }
;
pointer_opt:
   /**/                          { [], currentLoc () }
|  pointer                           { $1 }
;

type_name: /* (* ISO 6.7.6 *) */
  decl_spec_list abstract_decl { let d, a = $2 in
                                 if a <> [] then begin
                                   parse_error "attributes in type name";
                                   raise Parsing.Parse_error
                                 end;
                                 (fst $1, d) 
                               }
| decl_spec_list               { (fst $1, JUSTBASE) }
;
abstract_decl: /* (* ISO 6.7.6. *) */
  pointer_opt abs_direct_decl attributes  { applyPointer (fst $1) $2, $3 }
| pointer                                 { applyPointer (fst $1) JUSTBASE, [] }
;

abs_direct_decl: /* (* ISO 6.7.6. We do not support optional declarator for 
                     * functions. Plus Microsoft attributes. See the 
                     * discussion for declarator. *) */
|   LPAREN attributes abstract_decl RPAREN
                                   { let d, a = $3 in
                                     PARENTYPE ($2, d, a)
                                   }
            
|   LPAREN error RPAREN
                                   { JUSTBASE } 
            
|   abs_direct_decl_opt LBRACKET comma_expression_opt RBRACKET
                                   { ARRAY($1, [], $3) }
/*(* The next shoudl be abs_direct_decl_opt but we get conflicts *)*/
|   abs_direct_decl  parameter_list_startscope rest_par_list RPAREN
                                   { let (params, isva) = $3 in
                                     !E.pop_context ();
                                     PROTO ($1, params, isva)
                                   } 
;
abs_direct_decl_opt:
    abs_direct_decl                 { $1 }
|   /* empty */                     { JUSTBASE }
;
function_def:  /* (* ISO 6.9.1 *) */
  function_def_start block   
          { let (loc, specs, decl) = $1 in
            currentFunctionName := "<__FUNCTION__ used outside any functions>";
            !E.pop_context (); (* The context pushed by 
                                    * announceFunctionName *)
            doFunctionDef loc (trd3 $2) specs decl (fst3 $2)
          } 


function_def_start:  /* (* ISO 6.9.1 *) */
  decl_spec_list declarator   
                            { announceFunctionName $2;
                              (snd $1, fst $1, $2)
                            } 

/* (* Old-style function prototype *) */
| decl_spec_list old_proto_decl 
                            { announceFunctionName $2;
                              (snd $1, fst $1, $2)
                            } 
/* (* New-style function that does not have a return type *) */
| IDENT parameter_list_startscope rest_par_list RPAREN 
                           { let (params, isva) = $3 in
                             let fdec = 
                               (fst $1, PROTO(JUSTBASE, params, isva), [], snd $1) in
                             announceFunctionName fdec;
                             (* Default is int type *)
                             let defSpec = [SpecType Tint] in
                             (snd $1, defSpec, fdec)
                           }

/* (* No return type and old-style parameter list *) */
| IDENT LPAREN old_parameter_list_ne RPAREN old_pardef_list
                           { (* Convert pardecl to new style *)
                             let pardecl, isva = doOldParDecl $3 $5 in
                             (* Make the function declarator *)
                             let fdec = (fst $1,
                                         PROTO(JUSTBASE, pardecl,isva), 
                                         [], snd $1) in
                             announceFunctionName fdec;
                             (* Default is int type *)
                             let defSpec = [SpecType Tint] in
                             (snd $1, defSpec, fdec) 
                            }
/* (* No return type and no parameters *) */
| IDENT LPAREN                      RPAREN
                           { (* Make the function declarator *)
                             let fdec = (fst $1,
                                         PROTO(JUSTBASE, [], false), 
                                         [], snd $1) in
                             announceFunctionName fdec;
                             (* Default is int type *)
                             let defSpec = [SpecType Tint] in
                             (snd $1, defSpec, fdec)
                            }
;

/* const/volatile as type specifier elements */
cvspec:
    CONST                               { SpecCV(CV_CONST), $1 }
|   VOLATILE                            { SpecCV(CV_VOLATILE), $1 }
|   RESTRICT                            { SpecCV(CV_RESTRICT), $1 }
;

/*** GCC attributes ***/
attributes:
    /* empty */				{ []}
|   attribute attributes	        { fst $1 :: $2 }
;

/* (* In some contexts we can have an inline assembly to specify the name to 
    * be used for a global. We treat this as a name attribute *) */
attributes_with_asm:
    /* empty */                         { [] }
|   attribute attributes_with_asm       { fst $1 :: $2 }
|   ASM LPAREN string_constant RPAREN attributes        
                                        { ("__asm__", 
					   [CONSTANT(CONST_STRING (fst $3))]) :: $5 }
;

/* things like __attribute__, but no const/volatile */
attribute_nocv:
    ATTRIBUTE LPAREN paren_attr_list_ne RPAREN	
                                        { ("__attribute__", $3), $1 }
|   DECLSPEC paren_attr_list_ne         { ("__declspec", $2), $1 }
|   MSATTR                              { (fst $1, []), snd $1 }
                                        /* ISO 6.7.3 */
|   THREAD                              { ("__thread",[]), $1 }
;

/* __attribute__ plus const/volatile */
attribute:
    attribute_nocv                      { $1 }
|   CONST                               { ("const", []), $1 }
|   RESTRICT                            { ("restrict",[]), $1 }
|   VOLATILE                            { ("volatile",[]), $1 }
;

/* sm: I need something that just includes __attribute__ and nothing more,
 * to support them appearing between the 'struct' keyword and the type name */
just_attribute:
    ATTRIBUTE LPAREN paren_attr_list_ne RPAREN
                                        { ("__attribute__", $3) }
;

/* this can't be empty, b/c I folded that possibility into the calling
 * productions to avoid some S/R conflicts */
just_attributes:
    just_attribute                      { [$1] }
|   just_attribute just_attributes      { $1 :: $2 }
;

/** (* PRAGMAS and ATTRIBUTES *) ***/
/* (* We want to allow certain strange things that occur in pragmas, so we 
    * cannot use directly the language of expressions *) */ 
attr: 
|   id_or_typename                       { VARIABLE $1 }
|   IDENT COLON CST_INT                  { VARIABLE (fst $1 ^ ":" ^ fst $3) }
|   DEFAULT COLON CST_INT                { VARIABLE ("default:" ^ fst $3) }
                                         /* (* use a VARIABLE "" so that the 
                                             * parentheses are printed *) */
|   IDENT LPAREN  RPAREN                 { CALL(VARIABLE (fst $1), [VARIABLE ""]) }
|   IDENT paren_attr_list_ne             { CALL(VARIABLE (fst $1), $2) }
|   CST_INT                              { CONSTANT(CONST_INT (fst $1)) }
|   string_constant                      { CONSTANT(CONST_STRING (fst $1)) }
                                           /*(* Const when it appears in 
                                            * attribute lists, is translated 
                                            * to aconst *)*/
|   CONST                                { VARIABLE "aconst" }
|   SIZEOF expression                     {EXPR_SIZEOF (fst $2)}
|   SIZEOF LPAREN type_name RPAREN
		                         {let b, d = $3 in TYPE_SIZEOF (b, d)}

|   ALIGNOF expression                   {EXPR_ALIGNOF (fst $2)}
|   ALIGNOF LPAREN type_name RPAREN      {let b, d = $3 in TYPE_ALIGNOF (b, d)}
|   PLUS expression    	                 {UNARY (PLUS, fst $2)}
|   MINUS expression 		        {UNARY (MINUS, fst $2)}
|   STAR expression		        {UNARY (MEMOF, fst $2)}
|   AND expression				                 %prec ADDROF
	                                {UNARY (ADDROF, fst $2)}
|   EXCLAM expression		        {UNARY (NOT, fst $2)}
|   TILDE expression		        {UNARY (BNOT, fst $2)}
|   attr PLUS attr                      {BINARY(ADD ,$1 , $3)} 
|   attr MINUS attr                     {BINARY(SUB ,$1 , $3)}
|   attr STAR expression                {BINARY(MUL ,$1 , fst $3)}
|   attr SLASH attr			{BINARY(DIV ,$1 , $3)}
|   attr PERCENT attr			{BINARY(MOD ,$1 , $3)}
|   attr AND_AND attr			{BINARY(AND ,$1 , $3)}
|   attr PIPE_PIPE attr			{BINARY(OR ,$1 , $3)}
|   attr AND attr			{BINARY(BAND ,$1 , $3)}
|   attr PIPE attr			{BINARY(BOR ,$1 , $3)}
|   attr CIRC attr			{BINARY(XOR ,$1 , $3)}
|   attr EQ_EQ attr			{BINARY(EQ ,$1 , $3)}
|   attr EXCLAM_EQ attr			{BINARY(NE ,$1 , $3)}
|   attr INF attr			{BINARY(LT ,$1 , $3)}
|   attr SUP attr			{BINARY(GT ,$1 , $3)}
|   attr INF_EQ attr			{BINARY(LE ,$1 , $3)}
|   attr SUP_EQ attr			{BINARY(GE ,$1 , $3)}
|   attr INF_INF attr			{BINARY(SHL ,$1 , $3)}
|   attr SUP_SUP attr			{BINARY(SHR ,$1 , $3)}
|   attr ARROW id_or_typename           {MEMBEROFPTR ($1, $3)} 
|   attr DOT id_or_typename             {MEMBEROF ($1, $3)}  
|   LPAREN attr RPAREN                  { $2 } 
;

attr_list_ne:
|  attr                                  { [$1] }
|  attr COMMA attr_list_ne               { $1 :: $3 }
|  error COMMA attr_list_ne              { $3 }
;
paren_attr_list_ne: 
   LPAREN attr_list_ne RPAREN            { $2 }
|  LPAREN error RPAREN                   { [] }
;
/*** GCC ASM instructions ***/
asmattr:
     /* empty */                        { [] }
|    VOLATILE  asmattr                  { ("volatile", []) :: $2 }
|    CONST asmattr                      { ("const", []) :: $2 } 
;
asmtemplate: 
    one_string_constant                          { [$1] }
|   one_string_constant asmtemplate              { $1 :: $2 }
;
asmoutputs: 
  /* empty */           { ([], [], []) }
| COLON asmoperands asminputs
                        { let (ins, clobs) = $3 in
                          ($2, ins, clobs) }
;
asmoperands:
     /* empty */                        { [] }
|    asmoperandsne                      { List.rev $1 }
;
asmoperandsne:
     asmoperand                         { [$1] }
|    asmoperandsne COMMA asmoperand     { $3 :: $1 }
;
asmoperand:
     string_constant LPAREN expression RPAREN    { (fst $1, fst $3) }
|    string_constant LPAREN error RPAREN         { (fst $1, NOTHING ) } 
; 
asminputs: 
  /* empty */                { ([], []) }
| COLON asmoperands asmclobber
                        { ($2, $3) }
;
asmclobber:
    /* empty */                         { [] }
| COLON asmcloberlst_ne                 { $2 }
;
asmcloberlst_ne:
   one_string_constant                           { [$1] }
|  one_string_constant COMMA asmcloberlst_ne     { $1 :: $3 }
;
  
%%



