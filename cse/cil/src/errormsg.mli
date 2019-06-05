(*
 *
 * Copyright (c) 2001-2002, 
 *  George C. Necula    <necula@cs.berkeley.edu>
 *  Scott McPeak        <smcpeak@cs.berkeley.edu>
 *  Wes Weimer          <weimer@cs.berkeley.edu>
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
 *)

(** Utilities for error reporting. *)

(* The parsing info *)
type parseinfo

val newline: unit -> unit  (* Call this function to announce a new line *)


val parse_error: string -> (* A message *) 
                 int -> (* token_start *)
                 int -> (* token_end *)
                 unit

type parseWhat = 
    ParseString of string
  | ParseFile of string

val startParsing: parseWhat -> Lexing.lexbuf (* Call this function to start 
                                              * parsing  *)
val finishParsing: unit -> unit (* Call this function to finish parsing and 
                                 * close the input channel *)

val setCurrentLine: int -> unit
val setCurrentFile: string -> unit

(* Will keep here the pattern for the Formatlex *)
val currentPattern: string ref


val getPosition: unit -> int * string * int (* Line number, file name, 
                                               current byte count in file *)



(* The parser needs to access functions in the lexer. But since the lexer 
 * depends on the parser module we store here the pointers *)
val push_context: (unit -> unit) ref (* Start a context *)
val add_type: (string -> unit) ref (* Add a new string as a type name *)
val add_identifier: (string -> unit) ref (* Add a new string as a variable 
                                          * name  *)
val pop_context: (unit -> unit) ref (* Remove all names added in this context 
                                     * *)


(** A channel for printing log messages *)
val logChannel : out_channel ref

(** If set then print debugging info *)
val debugFlag  : bool ref               

val verboseFlag : bool ref

(** Set to true if you want to see all warnings. *)
val warnFlag: bool ref

val theLexbuf     : Lexing.lexbuf ref

(** Error reporting functions raise this exception *)
exception Error


   (* Error reporting. All of these functions take same arguments as a 
    * Pretty.eprintf. They raise the exception Error after they print their 
    * stuff. However, their type indicates that they return a "Pretty.doc" 
    * (due to the need to use the built-in type "format") return a doc. Thus 
    * use as follows:  E.s (E.bug "different lengths (%d != %d)" l1 l2)
     *)

(** Prints an error message of the form [Error: ...] and then raises the 
    exception [Error]. Use in conjunction with s, for example: [E.s (E.error 
    ... )]. *)
val error         : ('a,unit,Pretty.doc) format -> 'a

(** Similar to [error] except that its output has the form [Bug: ...] *)
val bug           : ('a,unit,Pretty.doc) format -> 'a

(** Similar to [error] except that its output has the form [Unimplemented: ...] *)
val unimp         : ('a,unit,Pretty.doc) format -> 'a

val s             : Pretty.doc -> 'a

(** This is set whenever one of the above error functions are called. It must
    be cleared manually *)
val hadErrors : bool ref  

(** Like {!Errormsg.error} but does not raise the {!Errormsg.Error} 
 * exception. Use: [ignore (E.warn ...)] *)
val warn:    ('a,unit,Pretty.doc) format -> 'a

(** Like {!Errormsg.warn} but optional. Printed only if the 
 * {!Errormsg.warnFlag} is set *)
val warnOpt: ('a,unit,Pretty.doc) format -> 'a

(** Print something to [logChannel] *)
val log           : ('a,unit,Pretty.doc) format -> 'a

   (* All of the error and warning reporting functions can also print a 
    * context. To register a context printing function use "pushContext". To 
    * remove the last registered one use "popContext". If one of the error 
    * reporting functions is called it will invoke all currently registered 
    * context reporting functions in the reverse order they were registered. *)

(** Registers a context printing function *)
val pushContext  : (unit -> Pretty.doc) -> unit

(** Removes the last registered context printing function *)
val popContext   : unit -> unit

(** Show the context stack to stderr *)
val showContext : unit -> unit

(** To ensure that the context is registered and removed properly, use the 
    function below *)
val withContext  : (unit -> Pretty.doc) -> ('a -> 'b) -> 'a -> 'b

