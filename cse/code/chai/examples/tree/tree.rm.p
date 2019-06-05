type ctrlType:{outCS, reqCS, inCS, relCS}


module ArbiterCell

external urx, ury, sa:event
interface uax, uay, sr:event
private pc:ctrlType; priorityTo1, req1, req2:bool

event atom 
controls pc,req1,req2
reads pc,req1,req2,urx,ury,sa
awaits urx,ury,sa
 
init 
[] true -> pc':= outCS 
update
[] pc = outCS & urx? -> pc':= reqCS; req1':= true
[] pc = outCS & ury? -> pc':= reqCS; req2':= true
[] pc = reqCS & sa? -> pc':= inCS
[] pc = inCS & urx? & req1 = true -> pc':= relCS
[] pc = inCS & ury? & req2 = true -> pc':= relCS
[] pc = relCS & sa? -> pc':= outCS
[] pc = relCS & sa? -> pc':= outCS

endatom

atom controls
 sr,uax,uay,priorityTo1
reads pc,req1,req2,priorityTo1,uax,uay,sr
 
update 
[] pc = reqCS -> sr!
[] pc = inCS & req1 = true & req2 = false -> uax!; priorityTo1':= false
[] pc = inCS & req1 = false & req2 = true -> uax!; priorityTo1':= true
[] pc = inCS & req1 = true & req2 = true & priorityTo1 = true -> uax!; priorityTo1':= false
[] pc = inCS & req1 = true & req2 = true & priorityTo1 = false -> uay!; priorityTo1':= true
[] pc = relCS -> sr!
[] pc = outCS & priorityTo1 = false -> uax!
[] pc = outCS & priorityTo1 = true -> uay!

endatom

endmodule


module Process

external ua:event
interface ur:event
private pc:ctrlType

event atom 
controls pc
reads pc,ua
awaits ua
 
init 
[] true -> pc':= outCS
update
[] pc = outCS -> pc':= reqCS
[] pc = reqCS & ua? -> pc':= inCS
[] pc = inCS -> pc':= relCS
[] pc = relCS & ua? -> pc':= outCS

endatom
		
atom 
controls ur
reads pc,ur
 
update
[] pc = reqCS -> ur!
[] pc = inCS -> ur!
endatom

endmodule


module Buffer

external input:event
interface output:event

event atom 
controls output
reads input,output
awaits input
 
update
[] input? -> output!

endatom

endmodule


Position00:= Process[ur, ua:= ur0, ua0] || ArbiterCell[urx, uax, ury, uay, sr, sa:= ur0, ua0, ur1, ua1, sr00, sa00] || Process[ur, ua:= ur1, ua1]

Position01:= Process[ur, ua:= ur2, ua2] || ArbiterCell[urx, uax, ury, uay, sr, sa:= ur2, ua2, ur3, ua3, sr01, sa01] || Process[ur, ua:= ur3, ua3]

Position0:= Position00 || ArbiterCell[urx, uax, ury, uay, sr, sa:= sr00, sa00, sr01, sa01, sr0, sa0] || Position01 || Buffer[input, output:= sr0, sa0]









