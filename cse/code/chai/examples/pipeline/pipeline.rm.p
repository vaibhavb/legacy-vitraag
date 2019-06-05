






type opType:{AND_OP, OR_OP, LOAD_OP, STORE_OP, NO_OP}

type regType:bitvector 1
type regRangeType:(0..3)
type regFileType:array regRangeType of regType










		







module pipeline 
external inp:regType; op:opType; src1, src2, dest:regRangeType
interface out:regType; iregFile, iregFile_d1:regFileType; opr1, opr2:regType; aluOut:regType; pipe1_op, pipe2_op:opType; pipe1_dest, pipe2_dest:regRangeType; pipe1_inp, pipe2_inp:regType; wbReg:regType; regFile:regFileType; stall:bool
    
atom RegFile 
controls regFile
reads wbReg,pipe2_op,pipe2_dest,pipe2_inp,regFile
init
[] true -> forall i regFile'[i]:= 0
update
[] pipe2_op = AND_OP | pipe2_op = OR_OP  | pipe2_op = LOAD_OP -> forall i regFile'[i]:= if (pipe2_dest = i) 
									then wbReg else regFile[i] fi
endatom


atom Opr1
controls opr1
reads regFile,pipe1_op,pipe1_dest,pipe1_inp,pipe2_op,pipe2_dest,pipe2_inp,wbReg
awaits src1,aluOut,stall
update
[] ~stall' -> opr1':= if ((src1' = pipe1_dest) & ~(pipe1_op = NO_OP) & ~(pipe1_op = STORE_OP)) then 
		         if (pipe1_op = LOAD_OP) then pipe1_inp else aluOut' fi
	            else if ((src1' = pipe2_dest)  & ~(pipe2_op = NO_OP) & ~(pipe2_op = STORE_OP)) then wbReg
		         else regFile[src1'] fi fi
endatom


atom Opr2
controls opr2
reads regFile,pipe1_op,pipe1_dest,pipe1_inp,pipe2_op,pipe2_dest,pipe2_inp,wbReg
awaits src2,aluOut,stall
update
[] ~stall' -> opr2':= if ((src2' = pipe1_dest) & ~(pipe1_op = NO_OP) & ~(pipe1_op = STORE_OP)) then
		         if (pipe1_op = LOAD_OP) then pipe1_inp else aluOut' fi
	            else if ((src2' = pipe2_dest)  & ~(pipe2_op = NO_OP) & ~(pipe2_op = STORE_OP)) then wbReg
		         else regFile[src2'] fi fi
endatom


atom ALU
controls aluOut
reads pipe1_op,pipe1_dest,pipe1_inp,opr1,opr2
update
[] pipe1_op = AND_OP -> aluOut':= opr1 & opr2
[] pipe1_op = OR_OP -> aluOut':= opr1 | opr2
endatom


atom PipeStage1
controls pipe1_op,pipe1_dest,pipe1_inp
awaits op,dest,inp,stall
init
[] true -> pipe1_op':= NO_OP
update
[] true -> pipe1_op':= if stall' then NO_OP else op' fi; pipe1_dest':= dest'; pipe1_inp':= inp'
endatom


atom PipeStage2
controls pipe2_op,pipe2_dest,pipe2_inp
reads pipe1_op,pipe1_dest,pipe1_inp
init
[] true -> pipe2_op':= NO_OP
update 
[] true -> pipe2_op':= pipe1_op; pipe2_dest':= pipe1_dest; pipe2_inp':= pipe1_inp
endatom


atom WbReg
controls wbReg
reads pipe1_op,pipe1_dest,pipe1_inp
awaits aluOut
update
[] pipe1_op = AND_OP | pipe1_op = OR_OP -> wbReg':= aluOut'
[] pipe1_op = LOAD_OP -> wbReg':= pipe1_inp
endatom

atom Out
controls out
reads regFile
awaits dest,op,stall
 
update
[] ~stall' & op' = STORE_OP -> out':= regFile[dest']
endatom

atom Stall
controls stall
reads pipe1_op,pipe2_op,pipe1_dest,pipe2_dest
awaits dest,op
update 
[] op' = STORE_OP & (~(pipe1_op = NO_OP | pipe1_op = STORE_OP) & (dest' = pipe1_dest)) -> stall':= true
[] op' = STORE_OP & (~(pipe2_op = NO_OP | pipe2_op = STORE_OP) & (dest' = pipe2_dest)) -> stall':= true
[] ~(op' = STORE_OP & (~(pipe1_op = NO_OP | pipe1_op = STORE_OP) & (dest' = pipe1_dest))) & ~(op' = STORE_OP & (~(pipe2_op = NO_OP | pipe2_op = STORE_OP) & (dest' = pipe2_dest))) -> stall':= false
endatom





atom IregFile 
controls iregFile
reads iregFile
awaits op,inp,src1,src2,dest,stall
 
init
[] true -> forall i iregFile'[i]:= 0
update

[] ~stall' & (op' = LOAD_OP) -> forall i iregFile'[i]:= if ( dest' = i) then inp' else iregFile[i] fi

[] ~stall' & (op' = AND_OP) -> forall i iregFile'[i]:= if ( dest' = i) then (iregFile[src1'] & iregFile[src2']) else iregFile[i] fi

[] ~stall' & (op' = OR_OP) -> forall i iregFile'[i]:= if ( dest' = i ) then (iregFile[src1']  | iregFile[src2']) else iregFile[i] fi
endatom

atom IregFile_d1
controls iregFile_d1
reads iregFile
init
[] true -> forall i iregFile_d1'[i]:= 0
update
[] true -> forall i iregFile_d1'[i]:= iregFile[i]
endatom

endmodule



module ISA
external inp:regType; op:opType; src1, src2, dest:regRangeType
interface out:regType; iregFile, iregFile_d1, regFile:regFileType; opr1, opr2:regType; stall:bool


atom IregFile 
controls iregFile
reads iregFile
awaits op,inp,src1,src2,dest,stall
 
init
[] true -> forall i iregFile'[i]:= 0
update

[] ~stall' & (op' = LOAD_OP) -> forall i iregFile'[i]:= if ( dest' = i) then inp' else iregFile[i] fi

[] ~stall' & (op' = AND_OP) -> forall i iregFile'[i]:= if ( dest' = i) then (iregFile[src1'] & iregFile[src2']) else iregFile[i] fi

[] ~stall' & (op' = OR_OP) -> forall i iregFile'[i]:= if ( dest' = i ) then (iregFile[src1']  | iregFile[src2']) else iregFile[i] fi
endatom



atom Out
controls out
reads iregFile
awaits op,dest,stall
update
[] ~stall' & (op' = STORE_OP) -> out':= iregFile[dest']
endatom

atom Stall
controls stall
init update
[] true -> stall':= true
[] true -> stall':= false
endatom


atom Opr1
controls opr1
reads iregFile
awaits src1,stall
update
[] ~stall' -> opr1':= iregFile[src1']
endatom

atom Opr2
controls opr2
reads iregFile
awaits src2,stall
update
[] ~stall' -> opr2':= iregFile[src2']
endatom


atom IregFile_d1
controls iregFile_d1
reads iregFile
init
[] true -> forall i iregFile_d1'[i]:= 0
update
[] true -> forall i iregFile_d1'[i]:= iregFile[i]
endatom

atom regFile 
controls regFile
reads iregFile_d1
init
[] true -> forall i regFile'[i]:= 0
update
[] true -> forall i regFile'[i]:= iregFile_d1[i]
endatom

endmodule

