(***********************************************************************)
(*                                                                     *)
(*                           Objective Caml                            *)
(*                                                                     *)
(*            Xavier Leroy, projet Cristal, INRIA Rocquencourt         *)
(*                                                                     *)
(*  Copyright 1996 Institut National de Recherche en Informatique et   *)
(*  en Automatique.  All rights reserved.  This file is distributed    *)
(*  under the terms of the Q Public License version 1.0.               *)
(*                                                                     *)
(***********************************************************************)

(* $Id: emit.mlp,v 1.22 2002/07/22 16:37:50 doligez Exp $ *)

(* Emission of Intel 386 assembly code *)

open Location
open Misc
open Cmm
open Arch
open Proc
open Reg
open Mach
open Linearize
open Emitaux

(* Tradeoff between code size and code speed *)

let fastcode_flag = ref true

let stack_offset = ref 0

(* Layout of the stack frame *)

let frame_size () =                     (* includes return address *)
  !stack_offset + 4 * num_stack_slots.(0) + 8 * num_stack_slots.(1) + 4

let slot_offset loc cl =
  match loc with
    Incoming n -> frame_size() + n
  | Local n ->
      if cl = 0
      then !stack_offset + n * 4
      else !stack_offset + num_stack_slots.(0) * 4 + n * 8
  | Outgoing n -> n

(* Prefixing of symbols with "_" *)

let symbol_prefix =
  match Config.system with
    "linux_elf" -> ""
  | "bsd_elf" -> ""
  | "solaris" -> ""
  | "beos" -> ""
  | _ -> "_"

let emit_symbol s =
  emit_string symbol_prefix; Emitaux.emit_symbol '$' s

(* Output a label *)

let label_prefix =
  match Config.system with
    "linux_elf" -> ".L"
  | "bsd_elf" -> ".L"
  | "solaris" -> ".L"
  | "beos" -> ".L"
  | _ -> "L"

let emit_label lbl =
  emit_string label_prefix; emit_int lbl


(* Some data directives have different names under Solaris *)

let word_dir =
  match Config.system with
    "solaris" -> ".value"
  | _ -> ".word"
let skip_dir =
  match Config.system with
    "solaris" -> ".zero"
  | _ -> ".space"
let use_ascii_dir =
  match Config.system with
    "solaris" -> false
  | _ -> true

(* Output a .align directive.
   The numerical argument to .align is log2 of alignment size, except
   under ELF, where it is the alignment size... *)

let emit_align =
  match Config.system with
    "linux_elf" | "bsd_elf" | "solaris" | "beos" | "cygwin" | "mingw" ->
      (fun n -> (emit_string "	.align	"; emit_int n; emit_char '\n'))
  | _ ->
      (fun n -> (emit_string "	.align	"; emit_int(Misc.log2 n); emit_char '\n'))
  
let emit_Llabel fallthrough lbl =
  if not fallthrough && !fastcode_flag then
    emit_align 16 ;
  emit_label lbl
  
(* Output a pseudo-register *)

let emit_reg = function
    { loc = Reg r } ->
      emit_string (register_name r)
  | { loc = Stack s } as r ->
      let ofs = slot_offset s (register_class r) in
      (emit_int ofs; emit_string "(%esp)")
  | { loc = Unknown } ->
      fatal_error "Emit_i386.emit_reg"

(* Output a reference to the lower 8 bits or lower 16 bits of a register *)

let reg_low_byte_name = [| "%al"; "%bl"; "%cl"; "%dl" |]
let reg_low_half_name = [| "%ax"; "%bx"; "%cx"; "%dx"; "%si"; "%di"; "%bp" |]

let emit_reg8 r =
  match r.loc with
    Reg r when r < 4 -> emit_string (reg_low_byte_name.(r))
  | _ -> fatal_error "Emit_i386.emit_reg8"

let emit_reg16 r =
  match r.loc with
    Reg r when r < 7 -> emit_string (reg_low_half_name.(r))
  | _ -> fatal_error "Emit_i386.emit_reg16"

(* Output an addressing mode *)

let emit_addressing addr r n =
  match addr with
    Ibased(s, d) ->
      (emit_symbol s);
      if d <> 0 then (emit_string " + "; emit_int d)
  | Iindexed d ->
      if d <> 0 then emit_int d;
      (emit_char '('; emit_reg r.(n); emit_char ')')
  | Iindexed2 d ->
      if d <> 0 then emit_int d;
      (emit_char '('; emit_reg r.(n); emit_string ", "; emit_reg r.(n+1); emit_char ')')
  | Iscaled(scale, d) ->
      if d <> 0 then emit_int d;
      (emit_string "(, "; emit_reg r.(n); emit_string ", "; emit_int scale; emit_char ')')
  | Iindexed2scaled(scale, d) ->
      if d <> 0 then emit_int d;
      (emit_char '('; emit_reg r.(n); emit_string ", "; emit_reg r.(n+1); emit_string ", "; emit_int scale; emit_char ')')

(* Record live pointers at call points *)

type frame_descr =
  { fd_lbl: int;                        (* Return address *)
    fd_frame_size: int;                 (* Size of stack frame *)
    fd_live_offset: int list }          (* Offsets/regs of live addresses *)

let frame_descriptors = ref([] : frame_descr list)

let record_frame_label live =
  let lbl = new_label() in
  let live_offset = ref [] in
  Reg.Set.iter
    (function
        {typ = Addr; loc = Reg r} ->
          live_offset := ((r lsl 1) + 1) :: !live_offset
      | {typ = Addr; loc = Stack s} as reg ->
          live_offset := slot_offset s (register_class reg) :: !live_offset
      | _ -> ())
    live;
  frame_descriptors :=
    { fd_lbl = lbl;
      fd_frame_size = frame_size();
      fd_live_offset = !live_offset } :: !frame_descriptors;
  lbl

let record_frame live =
  let lbl = record_frame_label live in (emit_label lbl; emit_string ":\n")

let emit_frame fd =
  (emit_string "	.long	"; emit_label fd.fd_lbl; emit_char '\n');
  (emit_char '	'; emit_string word_dir; emit_char '	'; emit_int fd.fd_frame_size; emit_char '\n');
  (emit_char '	'; emit_string word_dir; emit_char '	'; emit_int (List.length fd.fd_live_offset); emit_char '\n');
  List.iter
    (fun n ->
      (emit_char '	'; emit_string word_dir; emit_char '	'; emit_int n; emit_char '\n'))
    fd.fd_live_offset;
  emit_align 4

(* Record calls to the GC -- we've moved them out of the way *)

type gc_call =
  { gc_lbl: label;                      (* Entry label *)
    gc_return_lbl: label;               (* Where to branch after GC *)
    gc_frame: label }                   (* Label of frame descriptor *)

let call_gc_sites = ref ([] : gc_call list)

let emit_call_gc gc =
  (emit_label gc.gc_lbl; emit_string ":	call	"; emit_symbol "caml_call_gc"; emit_char '\n');
  (emit_label gc.gc_frame; emit_string ":	jmp	"; emit_label gc.gc_return_lbl; emit_char '\n')

(* Names for instructions *)

let instr_for_intop = function
    Iadd -> "addl"
  | Isub -> "subl"
  | Imul -> "imull"
  | Iand -> "andl"
  | Ior -> "orl"
  | Ixor -> "xorl"
  | Ilsl -> "sall"
  | Ilsr -> "shrl"
  | Iasr -> "sarl"
  | _ -> fatal_error "Emit_i386: instr_for_intop"

let instr_for_floatop = function
    Inegf -> "fchs"
  | Iabsf -> "fabs"
  | Iaddf -> "faddl"
  | Isubf -> "fsubl"
  | Imulf -> "fmull"
  | Idivf -> "fdivl"
  | Ispecific Isubfrev -> "fsubrl"
  | Ispecific Idivfrev -> "fdivrl"
  | _ -> fatal_error "Emit_i386: instr_for_floatop"

let instr_for_floatop_reversed = function
    Iaddf -> "faddl"
  | Isubf -> "fsubrl"
  | Imulf -> "fmull"
  | Idivf -> "fdivrl"
  | Ispecific Isubfrev -> "fsubl"
  | Ispecific Idivfrev -> "fdivl"
  | _ -> fatal_error "Emit_i386: instr_for_floatop_reversed"

let instr_for_floatop_pop = function
    Iaddf -> "faddp"
  | Isubf -> "fsubp"
  | Imulf -> "fmulp"
  | Idivf -> "fdivp"
  | Ispecific Isubfrev -> "fsubrp"
  | Ispecific Idivfrev -> "fdivrp"
  | _ -> fatal_error "Emit_i386: instr_for_floatop_pop"

let instr_for_floatarithmem double = function
    Ifloatadd -> if double then "faddl" else "fadds"
  | Ifloatsub -> if double then "fsubl" else "fsubs"
  | Ifloatsubrev -> if double then "fsubrl" else "fsubrs"
  | Ifloatmul -> if double then "fmull" else "fmuls"
  | Ifloatdiv -> if double then "fdivl" else "fdivs"
  | Ifloatdivrev -> if double then "fdivrl" else "fdivrs"

let name_for_cond_branch = function
    Isigned Ceq -> "e"     | Isigned Cne -> "ne"
  | Isigned Cle -> "le"     | Isigned Cgt -> "g"
  | Isigned Clt -> "l"     | Isigned Cge -> "ge"
  | Iunsigned Ceq -> "e"   | Iunsigned Cne -> "ne"
  | Iunsigned Cle -> "be"  | Iunsigned Cgt -> "a"
  | Iunsigned Clt -> "b"  | Iunsigned Cge -> "ae"
    
(* Output an = 0 or <> 0 test. *)

let output_test_zero arg =
  match arg.loc with
    Reg r -> (emit_string "	testl	"; emit_reg arg; emit_string ", "; emit_reg arg; emit_char '\n')
  | _     -> (emit_string "	cmpl	$0, "; emit_reg arg; emit_char '\n')

(* Deallocate the stack frame before a return or tail call *)

let output_epilogue () =
  let n = frame_size() - 4 in
  if n > 0 then (emit_string "	addl	$"; emit_int n; emit_string ", %esp\n")

(* Output the assembly code for an instruction *)

(* Name of current function *)
let function_name = ref ""
(* Entry point for tail recursive calls *)
let tailrec_entry_point = ref 0
(* Label of trap for out-of-range accesses *)
let range_check_trap = ref 0

let float_constants = ref ([] : (int * string) list)

let tos = phys_reg 100


let emit_instr fallthrough i =
    match i.desc with
      Lend -> ()
    | Lop(Imove | Ispill | Ireload) ->
        let src = i.arg.(0) and dst = i.res.(0) in
        if src.loc <> dst.loc then begin
          if src.typ = Float then
            if src = tos then
              (emit_string "	fstpl	"; emit_reg dst; emit_char '\n')
            else begin
              (emit_string "	fldl	"; emit_reg src; emit_char '\n');
              (emit_string "	fstpl	"; emit_reg dst; emit_char '\n')
            end
          else
              (emit_string "	movl	"; emit_reg src; emit_string ", "; emit_reg dst; emit_char '\n')
        end
    | Lop(Iconst_int n) ->
        if n = Nativeint.zero then begin
          match i.res.(0).loc with
            Reg n -> (emit_string "	xorl	"; emit_reg i.res.(0); emit_string ", "; emit_reg i.res.(0); emit_char '\n')
          | _     -> (emit_string "	movl	$0, "; emit_reg i.res.(0); emit_char '\n')
        end else
          (emit_string "	movl	$"; emit_nativeint n; emit_string ", "; emit_reg i.res.(0); emit_char '\n')
    | Lop(Iconst_float s) ->
        let f = float_of_string s in
        if f = 0.0 then
          (emit_string "	fldz\n")
        else if f = 1.0 then
          (emit_string "	fld1\n")
        else begin
          let lbl = new_label() in
          float_constants := (lbl, s) :: !float_constants;
          (emit_string "	fldl	"; emit_label lbl; emit_char '\n')
        end
    | Lop(Iconst_symbol s) ->
        (emit_string "	movl	$"; emit_symbol s; emit_string ", "; emit_reg i.res.(0); emit_char '\n')
    | Lop(Icall_ind) ->
        (emit_string "	call	*"; emit_reg i.arg.(0); emit_char '\n');
        record_frame i.live
    | Lop(Icall_imm s) ->
        (emit_string "	call	"; emit_symbol s; emit_char '\n');
        record_frame i.live
    | Lop(Itailcall_ind) ->
        output_epilogue();
        (emit_string "	jmp	*"; emit_reg i.arg.(0); emit_char '\n')
    | Lop(Itailcall_imm s) ->
        if s = !function_name then
          (emit_string "	jmp	"; emit_label !tailrec_entry_point; emit_char '\n')
        else begin
          output_epilogue();
          (emit_string "	jmp	"; emit_symbol s; emit_char '\n')
        end
    | Lop(Iextcall(s, alloc)) ->
        if alloc then begin
          (emit_string "	movl	$"; emit_symbol s; emit_string ", %eax\n");
          (emit_string "	call	"; emit_symbol "caml_c_call"; emit_char '\n');
          record_frame i.live
        end else begin
          (emit_string "	call	"; emit_symbol s; emit_char '\n')
        end
    | Lop(Istackoffset n) ->
        if n < 0
        then (emit_string "	addl	$"; emit_int(-n); emit_string ", %esp\n")
        else (emit_string "	subl	$"; emit_int(n); emit_string ", %esp\n");
        stack_offset := !stack_offset + n
    | Lop(Iload(chunk, addr)) ->
        let dest = i.res.(0) in
        begin match chunk with
          | Byte_unsigned ->
              (emit_string "	movzbl	"; emit_addressing addr i.arg 0; emit_string ", "; emit_reg dest; emit_char '\n')
          | Byte_signed ->
              (emit_string "	movsbl	"; emit_addressing addr i.arg 0; emit_string ", "; emit_reg dest; emit_char '\n')
          | Sixteen_unsigned ->
              (emit_string "	movzwl	"; emit_addressing addr i.arg 0; emit_string ", "; emit_reg dest; emit_char '\n')
          | Sixteen_signed ->
              (emit_string "	movswl	"; emit_addressing addr i.arg 0; emit_string ", "; emit_reg dest; emit_char '\n')
          | Single ->
            (emit_string "	flds	"; emit_addressing addr i.arg 0; emit_char '\n')
          | Double | Double_u ->
            (emit_string "	fldl	"; emit_addressing addr i.arg 0; emit_char '\n')
          | _ (* Word | Thirtytwo_signed | Thirtytwo_unsigned *) ->
              (emit_string "	movl	"; emit_addressing addr i.arg 0; emit_string ", "; emit_reg dest; emit_char '\n')
        end
    | Lop(Istore(chunk, addr)) ->
        begin match chunk with
          | Word | Thirtytwo_signed | Thirtytwo_unsigned ->
            (emit_string "	movl	"; emit_reg i.arg.(0); emit_string ", "; emit_addressing addr i.arg 1; emit_char '\n')
          | Byte_unsigned | Byte_signed ->
            (emit_string "	movb	"; emit_reg8 i.arg.(0); emit_string ", "; emit_addressing addr i.arg 1; emit_char '\n')
          | Sixteen_unsigned | Sixteen_signed ->
            (emit_string "	movw	"; emit_reg16 i.arg.(0); emit_string ", "; emit_addressing addr i.arg 1; emit_char '\n')
          | Single ->
              if i.arg.(0) = tos then
                (emit_string "	fstps	"; emit_addressing addr i.arg 1; emit_char '\n')
              else begin
                (emit_string "	fldl	"; emit_reg i.arg.(0); emit_char '\n');
                (emit_string "	fstps	"; emit_addressing addr i.arg 1; emit_char '\n')
              end
          | Double | Double_u ->
              if i.arg.(0) = tos then
                (emit_string "	fstpl	"; emit_addressing addr i.arg 1; emit_char '\n')
              else begin
                (emit_string "	fldl	"; emit_reg i.arg.(0); emit_char '\n');
                (emit_string "	fstpl	"; emit_addressing addr i.arg 1; emit_char '\n')
              end
        end
    | Lop(Ialloc n) ->
        if !fastcode_flag then begin
          let lbl_redo = new_label() in
          (emit_label lbl_redo; emit_string ":	movl	"; emit_symbol "young_ptr"; emit_string ", %eax\n");
          (emit_string "	subl	$"; emit_int n; emit_string ", %eax\n");
          (emit_string "	movl	%eax, "; emit_symbol "young_ptr"; emit_char '\n');
          (emit_string "	cmpl	"; emit_symbol "young_limit"; emit_string ", %eax\n");
          let lbl_call_gc = new_label() in
          let lbl_frame = record_frame_label i.live in
          (emit_string "	jb	"; emit_label lbl_call_gc; emit_char '\n');
          (emit_string "	leal	4(%eax), "; emit_reg i.res.(0); emit_char '\n');
          call_gc_sites :=
            { gc_lbl = lbl_call_gc;
              gc_return_lbl = lbl_redo;
              gc_frame = lbl_frame } :: !call_gc_sites
        end else begin
          begin match n with
            8  -> (emit_string "	call	"; emit_symbol "caml_alloc1"; emit_char '\n')
          | 12 -> (emit_string "	call	"; emit_symbol "caml_alloc2"; emit_char '\n')
          | 16 -> (emit_string "	call	"; emit_symbol "caml_alloc3"; emit_char '\n')
          | _  -> (emit_string "	movl	$"; emit_int n; emit_string ", %eax\n");
                  (emit_string "	call	"; emit_symbol "caml_alloc"; emit_char '\n')
          end;
          (record_frame i.live; emit_string "	leal	4(%eax), "; emit_reg i.res.(0); emit_char '\n')
        end
    | Lop(Iintop(Icomp cmp)) ->
        (emit_string "	cmpl	"; emit_reg i.arg.(1); emit_string ", "; emit_reg i.arg.(0); emit_char '\n');
        let b = name_for_cond_branch cmp in
        (emit_string "	set"; emit_string b; emit_string "	%al\n");
        (emit_string "	movzbl	%al, "; emit_reg i.res.(0); emit_char '\n')
    | Lop(Iintop_imm(Icomp cmp, n)) ->
        (emit_string "	cmpl	$"; emit_int n; emit_string ", "; emit_reg i.arg.(0); emit_char '\n');
        let b = name_for_cond_branch cmp in
        (emit_string "	set"; emit_string b; emit_string "	%al\n");
        (emit_string "	movzbl	%al, "; emit_reg i.res.(0); emit_char '\n')
    | Lop(Iintop Icheckbound) ->
        if !range_check_trap = 0 then range_check_trap := new_label();
        (emit_string "	cmpl	"; emit_reg i.arg.(1); emit_string ", "; emit_reg i.arg.(0); emit_char '\n');
        (emit_string "	jbe	"; emit_label !range_check_trap; emit_char '\n')
    | Lop(Iintop_imm(Icheckbound, n)) ->
        if !range_check_trap = 0 then range_check_trap := new_label();
        (emit_string "	cmpl	$"; emit_int n; emit_string ", "; emit_reg i.arg.(0); emit_char '\n');
        (emit_string "	jbe	"; emit_label !range_check_trap; emit_char '\n')
    | Lop(Iintop(Idiv | Imod)) ->
        (emit_string "	cltd\n");
        (emit_string "	idivl	"; emit_reg i.arg.(1); emit_char '\n')
    | Lop(Iintop(Ilsl | Ilsr | Iasr as op)) ->
        (* We have i.arg.(0) = i.res.(0) and i.arg.(1) = %ecx *)
        (emit_char '	'; emit_string(instr_for_intop op); emit_string "	%cl, "; emit_reg i.res.(0); emit_char '\n')
    | Lop(Iintop op) ->
        (* We have i.arg.(0) = i.res.(0) *)
        (emit_char '	'; emit_string(instr_for_intop op); emit_char '	'; emit_reg i.arg.(1); emit_string ", "; emit_reg i.res.(0); emit_char '\n')
    | Lop(Iintop_imm(Iadd, 1) | Iintop_imm(Isub, -1)) ->
        (emit_string "	incl	"; emit_reg i.res.(0); emit_char '\n')
    | Lop(Iintop_imm(Iadd, -1) | Iintop_imm(Isub, 1)) ->
        (emit_string "	decl	"; emit_reg i.res.(0); emit_char '\n')
    | Lop(Iintop_imm(Iadd, n)) when i.arg.(0).loc <> i.res.(0).loc ->
        (emit_string "	leal	"; emit_int n; emit_char '('; emit_reg i.arg.(0); emit_string "), "; emit_reg i.res.(0); emit_char '\n')
    | Lop(Iintop_imm(Idiv, n)) ->
        let l = Misc.log2 n in
        let lbl = new_label() in
        output_test_zero i.arg.(0);
        (emit_string "	jge	"; emit_label lbl; emit_char '\n');
        (emit_string "	addl	$"; emit_int(n-1); emit_string ", "; emit_reg i.arg.(0); emit_char '\n');
        (emit_label lbl; emit_string ":	sarl	$"; emit_int l; emit_string ", "; emit_reg i.arg.(0); emit_char '\n')
    | Lop(Iintop_imm(Imod, n)) ->
        let l = Misc.log2 n in
        let lbl = new_label() in
        (emit_string "	movl	"; emit_reg i.arg.(0); emit_string ", %eax\n");
        (emit_string "	testl	%eax, %eax\n");
        (emit_string "	jge	"; emit_label lbl; emit_char '\n');
        (emit_string "	addl	$"; emit_int(n-1); emit_string ", %eax\n");
        (emit_label lbl; emit_string ":	andl	$"; emit_int(-n); emit_string ", %eax\n");
        (emit_string "	subl	%eax, "; emit_reg i.arg.(0); emit_char '\n')
    | Lop(Iintop_imm(op, n)) ->
        (* We have i.arg.(0) = i.res.(0) *)
        (emit_char '	'; emit_string(instr_for_intop op); emit_string "	$"; emit_int n; emit_string ", "; emit_reg i.res.(0); emit_char '\n')
    | Lop(Inegf | Iabsf as floatop) ->
        if i.arg.(0) <> tos then
          (emit_string "	fldl	"; emit_reg i.arg.(0); emit_char '\n');
        (emit_char '	'; emit_string(instr_for_floatop floatop); emit_char '\n')
    | Lop(Iaddf | Isubf | Imulf | Idivf | Ispecific(Isubfrev | Idivfrev)
          as floatop) ->
        if i.arg.(0) = tos && i.arg.(1) = tos then
          (* both operands on top of FP stack *)
          (emit_char '	'; emit_string(instr_for_floatop_pop floatop); emit_string "	%st, %st(1)\n")
        else if i.arg.(0) = tos then
          (* first operand on stack *)
          (emit_char '	'; emit_string(instr_for_floatop floatop); emit_char '	'; emit_reg i.arg.(1); emit_char '\n')
        else if i.arg.(1) = tos then
          (* second operand on stack *)
          (emit_char '	'; emit_string(instr_for_floatop_reversed floatop); emit_char '	'; emit_reg i.arg.(0); emit_char '\n')
        else begin
          (* both operands in memory *)
          (emit_string "	fldl	"; emit_reg i.arg.(0); emit_char '\n');
          (emit_char '	'; emit_string(instr_for_floatop floatop); emit_char '	'; emit_reg i.arg.(1); emit_char '\n')
        end
    | Lop(Ifloatofint) ->
        begin match i.arg.(0).loc with
          Stack s ->
            (emit_string "	fildl	"; emit_reg i.arg.(0); emit_char '\n')
        | _ ->
            (emit_string "	pushl	"; emit_reg i.arg.(0); emit_char '\n');
            (emit_string "	fildl	(%esp)\n");
            (emit_string "	addl	$4, %esp\n")
        end
    | Lop(Iintoffloat) ->
        if i.arg.(0) <> tos then
          (emit_string "	fldl	"; emit_reg i.arg.(0); emit_char '\n');
        stack_offset := !stack_offset - 8;
        (emit_string "	subl	$8, %esp\n");
        (emit_string "	fnstcw	4(%esp)\n");
        (emit_string "	movl	4(%esp), %eax\n");
        (emit_string "	movb    $12, %ah\n");
        (emit_string "	movl	%eax, (%esp)\n");
        (emit_string "	fldcw	(%esp)\n");
        begin match i.res.(0).loc with
          Stack s ->
            (emit_string "	fistpl	"; emit_reg i.res.(0); emit_char '\n')
        | _ ->
            (emit_string "	fistpl	(%esp)\n");
            (emit_string "	movl	(%esp), "; emit_reg i.res.(0); emit_char '\n')
        end;
        (emit_string "	fldcw	4(%esp)\n");
        (emit_string "	addl	$8, %esp\n");
        stack_offset := !stack_offset + 8
    | Lop(Ispecific(Ilea addr)) ->
        (emit_string "	lea	"; emit_addressing addr i.arg 0; emit_string ", "; emit_reg i.res.(0); emit_char '\n')
    | Lop(Ispecific(Istore_int(n, addr))) ->
        (emit_string "	movl	$"; emit_nativeint n; emit_string ", "; emit_addressing addr i.arg 0; emit_char '\n')
    | Lop(Ispecific(Istore_symbol(s, addr))) ->
        (emit_string "	movl	$"; emit_symbol s; emit_string ", "; emit_addressing addr i.arg 0; emit_char '\n')
    | Lop(Ispecific(Ioffset_loc(n, addr))) ->
        (emit_string "	addl	$"; emit_int n; emit_string ", "; emit_addressing addr i.arg 0; emit_char '\n')
    | Lop(Ispecific(Ipush)) ->
        (* Push arguments in reverse order *)
        for n = Array.length i.arg - 1 downto 0 do
          let r = i.arg.(n) in
          match r with
            {loc = Reg _; typ = Float} ->
              (emit_string "	subl	$8, %esp\n");
              (emit_string "	fstpl	0(%esp)\n");
              stack_offset := !stack_offset + 8
          | {loc = Stack sl; typ = Float} ->
              let ofs = slot_offset sl 1 in
              (emit_string "	pushl	"; emit_int(ofs + 4); emit_string "(%esp)\n");
              (emit_string "	pushl	"; emit_int(ofs + 4); emit_string "(%esp)\n");
              stack_offset := !stack_offset + 8
          | _ ->
              (emit_string "	pushl	"; emit_reg r; emit_char '\n');
              stack_offset := !stack_offset + 4
        done
    | Lop(Ispecific(Ipush_int n)) ->
        (emit_string "	pushl	$"; emit_nativeint n; emit_char '\n');
        stack_offset := !stack_offset + 4
    | Lop(Ispecific(Ipush_symbol s)) ->
        (emit_string "	pushl	$"; emit_symbol s; emit_char '\n');
        stack_offset := !stack_offset + 4
    | Lop(Ispecific(Ipush_load addr)) ->
        (emit_string "	pushl	"; emit_addressing addr i.arg 0; emit_char '\n');
        stack_offset := !stack_offset + 4
    | Lop(Ispecific(Ipush_load_float addr)) ->
        (emit_string "	pushl	"; emit_addressing (offset_addressing addr 4) i.arg 0; emit_char '\n');
        (emit_string "	pushl	"; emit_addressing addr i.arg 0; emit_char '\n');
        stack_offset := !stack_offset + 8
    | Lop(Ispecific(Ifloatarithmem(double, op, addr))) ->
        if i.arg.(0) <> tos then
          (emit_string "	fldl	"; emit_reg i.arg.(0); emit_char '\n');
        (emit_char '	'; emit_string(instr_for_floatarithmem double op); emit_char '	'; emit_addressing addr i.arg 1; emit_char '\n')
    | Lreloadretaddr ->
        ()
    | Lreturn ->
        output_epilogue();
        (emit_string "	ret\n")
    | Llabel lbl ->
        (emit_Llabel fallthrough lbl; emit_string ":\n")
    | Lbranch lbl ->
        (emit_string "	jmp	"; emit_label lbl; emit_char '\n')
    | Lcondbranch(tst, lbl) ->
        begin match tst with
          Itruetest ->
            output_test_zero i.arg.(0);
            (emit_string "	jne	"; emit_label lbl; emit_char '\n')
        | Ifalsetest ->
            output_test_zero i.arg.(0);
            (emit_string "	je	"; emit_label lbl; emit_char '\n')
        | Iinttest cmp ->
            (emit_string "	cmpl	"; emit_reg i.arg.(1); emit_string ", "; emit_reg i.arg.(0); emit_char '\n');
            let b = name_for_cond_branch cmp in
            (emit_string "	j"; emit_string b; emit_char '	'; emit_label lbl; emit_char '\n')
        | Iinttest_imm((Isigned Ceq | Isigned Cne | 
                        Iunsigned Ceq | Iunsigned Cne) as cmp, 0) ->
            output_test_zero i.arg.(0);
            let b = name_for_cond_branch cmp in
            (emit_string "	j"; emit_string b; emit_char '	'; emit_label lbl; emit_char '\n')
        | Iinttest_imm(cmp, n) ->
            (emit_string "	cmpl	$"; emit_int n; emit_string ", "; emit_reg i.arg.(0); emit_char '\n');
            let b = name_for_cond_branch cmp in
            (emit_string "	j"; emit_string b; emit_char '	'; emit_label lbl; emit_char '\n')
        | Ifloattest((Ceq | Cne as cmp), neg) ->
            if i.arg.(1) <> tos then
              (emit_string "	fldl	"; emit_reg i.arg.(1); emit_char '\n');
            if i.arg.(0) <> tos then
              (emit_string "	fldl	"; emit_reg i.arg.(0); emit_char '\n');
            (emit_string "	fucompp\n");
            (emit_string "	fnstsw	%ax\n");
            let neg1 = if cmp = Ceq then neg else not neg in
            if neg1 then begin          (* branch if different *)
              (emit_string "	andb	$68, %ah\n");
              (emit_string "	xorb	$64, %ah\n");
              (emit_string "	jne	"; emit_label lbl; emit_char '\n')
            end else begin              (* branch if equal *)
              (emit_string "	andb	$69, %ah\n");
              (emit_string "	cmpb	$64, %ah\n");
              (emit_string "	je	"; emit_label lbl; emit_char '\n')
            end
        | Ifloattest(cmp, neg) ->
            let actual_cmp =
              if i.arg.(0) = tos && i.arg.(1) = tos then begin
                (* both args on top of FP stack *)
                (emit_string "	fcompp\n");
                cmp
              end else if i.arg.(0) = tos then begin
                (* first arg on top of FP stack *)
                (emit_string "	fcompl	"; emit_reg i.arg.(1); emit_char '\n');
                cmp
              end else if i.arg.(1) = tos then begin
                (* second arg on top of FP stack *)
                (emit_string "	fcompl	"; emit_reg i.arg.(0); emit_char '\n');
                Cmm.swap_comparison cmp
              end else begin
                (emit_string "	fldl	"; emit_reg i.arg.(0); emit_char '\n');
                (emit_string "	fcompl	"; emit_reg i.arg.(1); emit_char '\n');
                cmp
              end in
            (emit_string "	fnstsw	%ax\n");
            begin match actual_cmp with
              Cle ->
                (emit_string "	andb	$69, %ah\n");
                (emit_string "	decb	%ah\n");
                (emit_string "	cmpb	$64, %ah\n");
                if neg
                then (emit_string "	jae	")
                else (emit_string "	jb	")
            | Cge ->
                (emit_string "	andb	$5, %ah\n");
                if neg
                then (emit_string "	jne	")
                else (emit_string "	je	")
            | Clt ->
                (emit_string "	andb	$69, %ah\n");
                (emit_string "	cmpb	$1, %ah\n");
                if neg
                then (emit_string "	jne	")
                else (emit_string "	je	")
            | Cgt ->
                (emit_string "	andb	$69, %ah\n");
                if neg
                then (emit_string "	jne	")
                else (emit_string "	je	")
            | _ -> fatal_error "Emit_i386: floattest"
            end;
            (emit_label lbl; emit_char '\n')
        | Ioddtest ->
            (emit_string "	testl	$1, "; emit_reg i.arg.(0); emit_char '\n');
            (emit_string "	jne	"; emit_label lbl; emit_char '\n')
        | Ieventest ->
            (emit_string "	testl	$1, "; emit_reg i.arg.(0); emit_char '\n');
            (emit_string "	je	"; emit_label lbl; emit_char '\n')
        end
    | Lcondbranch3(lbl0, lbl1, lbl2) ->
            (emit_string "	cmpl	$1, "; emit_reg i.arg.(0); emit_char '\n');
            begin match lbl0 with
              None -> ()
            | Some lbl -> (emit_string "	jb	"; emit_label lbl; emit_char '\n')
            end;
            begin match lbl1 with
              None -> ()
            | Some lbl -> (emit_string "	je	"; emit_label lbl; emit_char '\n')
            end;
            begin match lbl2 with
              None -> ()
            | Some lbl -> (emit_string "	jg	"; emit_label lbl; emit_char '\n')
            end
    | Lswitch jumptbl ->
        let lbl = new_label() in
        (emit_string "	jmp	*"; emit_label lbl; emit_string "(, "; emit_reg i.arg.(0); emit_string ", 4)\n");
        (emit_string "	.data\n");
        (emit_label lbl; emit_char ':');
        for i = 0 to Array.length jumptbl - 1 do
          (emit_string "	.long	"; emit_label jumptbl.(i); emit_char '\n')
        done;
        (emit_string "	.text\n")
    | Lsetuptrap lbl ->
        (emit_string "	call	"; emit_label lbl; emit_char '\n')
    | Lpushtrap ->
        (emit_string "	pushl	"; emit_symbol "caml_exception_pointer"; emit_char '\n');
        (emit_string "	movl	%esp, "; emit_symbol "caml_exception_pointer"; emit_char '\n');
        stack_offset := !stack_offset + 8
    | Lpoptrap ->
        (emit_string "	popl	"; emit_symbol "caml_exception_pointer"; emit_char '\n');
        (emit_string "	addl	$4, %esp\n");
        stack_offset := !stack_offset - 8
    | Lraise ->
        (emit_string "	movl	"; emit_symbol "caml_exception_pointer"; emit_string ", %esp\n");
        (emit_string "	popl    "; emit_symbol "caml_exception_pointer"; emit_char '\n');
        (emit_string "	ret\n")

let rec emit_all fallthrough i =

  match i.desc with
  |  Lend -> ()
  | _ ->
      emit_instr fallthrough i;
      emit_all
        (Linearize.has_fallthrough  i.desc)
        i.next

(* Emission of the floating-point constants *)

let emit_float_constant (lbl, cst) =
  (emit_string "	.data\n");
  (emit_label lbl; emit_string ":	.double	"; emit_string cst; emit_char '\n')

(* Emission of the profiling prelude *)

let emit_profile () =
  match Config.system with
    "linux_elf" ->
      (emit_string "	pushl	%eax\n");
      (emit_string "	movl	%esp, %ebp\n");
      (emit_string "	pushl	%ecx\n");
      (emit_string "	pushl	%edx\n");
      (emit_string "	call	"; emit_symbol "mcount"; emit_char '\n');
      (emit_string "	popl	%edx\n");
      (emit_string "	popl	%ecx\n");
      (emit_string "	popl	%eax\n")
  | "bsd_elf" ->
      (emit_string "	pushl	%eax\n");
      (emit_string "	movl	%esp, %ebp\n");
      (emit_string "	pushl	%ecx\n");
      (emit_string "	pushl	%edx\n");
      (emit_string "	call	.mcount\n");
      (emit_string "	popl	%edx\n");
      (emit_string "	popl	%ecx\n");
      (emit_string "	popl	%eax\n")
  | _ -> () (*unsupported yet*)

(* Emission of a function declaration *)

let fundecl fundecl =
  function_name := fundecl.fun_name;
  fastcode_flag := fundecl.fun_fast;
  tailrec_entry_point := new_label();
  stack_offset := 0;
  float_constants := [];
  call_gc_sites := [];
  range_check_trap := 0;
  (emit_string "	.text\n");
  emit_align 16;
  (emit_string "	.globl	"; emit_symbol fundecl.fun_name; emit_char '\n');
  (emit_symbol fundecl.fun_name; emit_string ":\n");
  if !Clflags.gprofile then emit_profile();
  let n = frame_size() - 4 in
  if n > 0 then
    (emit_string "	subl	$"; emit_int n; emit_string ", %esp\n");
  (emit_label !tailrec_entry_point; emit_string ":\n");
  emit_all true fundecl.fun_body;
  List.iter emit_call_gc !call_gc_sites;
  if !range_check_trap > 0 then
    (emit_label !range_check_trap; emit_string ":	call	"; emit_symbol "caml_array_bound_error"; emit_char '\n');
    (* Never returns, but useful to have retaddr on stack for debugging *)
  List.iter emit_float_constant !float_constants

(* Emission of data *)

let emit_item = function
    Cdefine_symbol s ->
      (emit_string "	.globl	"; emit_symbol s; emit_char '\n');
      (emit_symbol s; emit_string ":\n")
  | Cdefine_label lbl ->
      (emit_label (100000 + lbl); emit_string ":\n")
  | Cint8 n ->
      (emit_string "	.byte	"; emit_int n; emit_char '\n')
  | Cint16 n ->
      (emit_char '	'; emit_string word_dir; emit_char '	'; emit_int n; emit_char '\n')
  | Cint32 n ->
      (emit_string "	.long	"; emit_nativeint n; emit_char '\n')
  | Cint n ->
      (emit_string "	.long	"; emit_nativeint n; emit_char '\n')
  | Csingle f ->
      (emit_string "	.float	"; emit_string f; emit_char '\n')
  | Cdouble f ->
      (emit_string "	.double	"; emit_string f; emit_char '\n')
  | Csymbol_address s ->
      (emit_string "	.long	"; emit_symbol s; emit_char '\n')
  | Clabel_address lbl ->
      (emit_string "	.long	"; emit_label (100000 + lbl); emit_char '\n')
  | Cstring s ->
      if use_ascii_dir
      then emit_string_directive "	.ascii	" s
      else emit_bytes_directive  "	.byte	" s
  | Cskip n ->
      if n > 0 then (emit_char '	'; emit_string skip_dir; emit_char '	'; emit_int n; emit_char '\n')
  | Calign n ->
      emit_align n

let data l =
  (emit_string "	.data\n");
  List.iter emit_item l

(* Beginning / end of an assembly file *)

let begin_assembly() =
  let lbl_begin = Compilenv.current_unit_name() ^ "__data_begin" in
  (emit_string "	.data\n");
  (emit_string "	.globl	"; emit_symbol lbl_begin; emit_char '\n');
  (emit_symbol lbl_begin; emit_string ":\n");
  let lbl_begin = Compilenv.current_unit_name() ^ "__code_begin" in
  (emit_string "	.text\n");
  (emit_string "	.globl	"; emit_symbol lbl_begin; emit_char '\n');
  (emit_symbol lbl_begin; emit_string ":\n")

let end_assembly() =
  let lbl_end = Compilenv.current_unit_name() ^ "__code_end" in
  (emit_string "	.text\n");
  (emit_string "	.globl	"; emit_symbol lbl_end; emit_char '\n');
  (emit_symbol lbl_end; emit_string ":\n");
  (emit_string "	.data\n");
  let lbl_end = Compilenv.current_unit_name() ^ "__data_end" in
  (emit_string "	.globl	"; emit_symbol lbl_end; emit_char '\n');
  (emit_symbol lbl_end; emit_string ":\n");
  (emit_string "	.long	0\n");
  let lbl = Compilenv.current_unit_name() ^ "__frametable" in
  (emit_string "	.globl	"; emit_symbol lbl; emit_char '\n');
  (emit_symbol lbl; emit_string ":\n");
  (emit_string "	.long	"; emit_int (List.length !frame_descriptors); emit_char '\n');
  List.iter emit_frame !frame_descriptors;
  frame_descriptors := []
