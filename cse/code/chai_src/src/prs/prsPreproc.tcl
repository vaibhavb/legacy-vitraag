#
#  Preprocessors for reactive modules
#

proc prsPreproc:initialize {} {
    global MacroDefnString
    global MacroForeachString
    global MacroRangeString
    global MacroEnumString
    global MacroEndForeachString 
    global MacroStructString
    global MacroEndStructString
    global MacroStructFullString
    global MacroTypeDeclarationString
    global MacroStructVariableExpandFlag 
    global PrsCommentString
    global AllSpecList

    set space {[\ \t]}
    set nospace {[^\ ]+}
    set alnum "\[0-9a-zA-Z\_\.\$\]+"
    set alnum1 "\[^:\)..\]+"
    set range "\\(($alnum1)$space*..$space*($alnum1)\\)"
    set enum  "\{\[^\}]*\}"
    set MacroDefnString "\#define$space+($alnum)$space+($alnum)"
    set MacroForeachString "\#foreach"
    set MacroRangeString "($alnum)$space*=$space*$range"
    set MacroEnumString  "($alnum)$space*=$space*($enum)"
    set MacroEndForeachString   "\#endforeach"
    set MacroStructString "\#struct"
    set MacroEndStructString "\#endstruct"
    set MacroStructFullString \
	"\#struct$space+($alnum)$space*:$space*\\{(\[^\}\]*)\\}$space*\#endstruct"
    set MacroTypeDeclarationString "(\[^:\ \t\]+)$space*:$space*(\[^\ ;\]+)"

    set MacroStructVariableExpandFlag 0

    set PrsCommentString {^--}

    set AllSpecList {atl inv}

    # dummy action
    set t ""
}

proc prs_preproc {filename} {
    global MacroForeachString
    global MacroDefnString
    global MacroEndForeachString
    global MacroStructString
    global MacroEndStructString
    global PrsPreprocLineno
    global STRUCTS
    global STRUCTVARS
    global MOCHATMPDIR

    
    if {[catch {set fr [open $filename "r"]}]} {
	error "Cannot read $filename"
    } 

    set f [file tail $filename]
    if {[catch {set fw [open $MOCHATMPDIR/$f.p "w"]}]} {
	error "Cannot write $MOCHATMPDIR/$f.p"
    }

    # set the mode so that it can be writable by other people. 
    catch {exec chmod a+w $MOCHATMPDIR/$f.p}
    
    catch {unset CONST}
    catch {unset STRUCTS}
    catch {unset STRUCTVARS}
    
    set command ""
    set dummyVarList ""
    set structure ""

    set foreachCounter 0
    set startStructMacro 0

    # for debugging
    set PrsPreprocLineno 0

    while {![eof $fr]} {
	set line [gets $fr]
	incr PrsPreprocLineno 
	
	# this is to identify the line of comment and remove it.
	if {[prsPreproc:lineIsComment $line]} {
	    set line ""
	}

	# this is to replace the defined constant

	set line [prsPreproc:replaceByConstants $line CONST]

	# this is to expand the macro struct types
	catch {unset dummy}
	#
#	if {$startStructMacro == 0} {
#	    set line [prsPreproc:expandMacroStructTypes $line STRUCTVARS]
#
#	} else {
#	    set line [prsPreproc:expandMacroStructTypes $line dummy]
#
#	}
#
#	# this is to expand the variable of structure type to its components
#	set line [prsPreproc:expandMacroStructVariables $line STRUCTVARS]
	
	# look for macro definitions
	if {[regexp $MacroDefnString $line t name const]} {
	    # i.e. #define abc 100
	    set CONST($name) $const
	    set line ""
	} elseif {[regexp $MacroForeachString $line]} {
	    # i.e. #foreach
	    prsPreproc:parseForeachMacros $line command  \
		foreachCounter dummyVarList
	    set line ""

	} elseif {[regexp $MacroEndForeachString $line]} {
	    # i.e. #endforeach
	    
	    # put down the lineno
	    prsPreproc:parseEndForeachMacros command foreachCounter \
		dummyVarList $fw
	    set line ""

	} else {
	}


	# structures are disabled 
	
	#elseif {[regexp $MacroStructString $line]} {
	#	    #i.e. #struct
	#
	#	    set startStructMacro 1
	#	    append structure $line
	#	    set line ""
	#	    
	#
	#	} else {
	#	    if {[regexp $MacroEndStructString $line]} {
	#		#i.e. #endstruct
	#		append structure $line
	#		if {$foreachCounter != 0} {
	#		    # set tmp $structure
	#		    set tmp \
	    #			[prsPreproc:fixConfusedSymbols \
	    #			     $structure $dummyVarList]
	#		    append command "prsPreproc:parseEndStructMacros \
	    #				 \"$tmp\";"
	#		} else {
	#		    prsPreproc:parseEndStructMacros $structure
	#		}
	#		
	#		set structure ""
	#		set startStructMacro 0
	#		set line ""
	#	    }
	#    }

	# decide if the line should be put into the output file,
	# into the structure,
	# or if the line should be put into the for loop.
	if {$foreachCounter == 0 && $startStructMacro == 0} {
	    puts $fw $line
	} elseif {$startStructMacro == 1} {
	    append structure $line
	} else {
	    # some symbols have to be fixed before evaluating
	    # the command string
	    set line [prsPreproc:fixConfusedSymbols $line $dummyVarList]
	    append command [concat puts \$fw \"$line\" \;]
	    append command " "
	}
    }

   # finally, check sanity of the output file 
    if {$foreachCounter != 0} {
	error "Unmatched \#foreach statement"
    } elseif { $startStructMacro != 0} {
	error "Unmatched \#struct statement"
    }

    close $fr
    close $fw
}

proc prsPreproc:replaceByConstants {line CONST} {
    upvar $CONST constants
    
    foreach i [array names constants] {
	set pattern \\$$i

	regsub -all $pattern $line $constants($i) t

	set line $t
    }
    
    return $line
}

proc prsPreproc:lineIsComment {line} {
    global PrsCommentString

    # there are two kinds of comment
    # one is those starting with --
    # the other is those delimited by /* and */

    
    return [regexp $PrsCommentString $line]

}

proc prsPreproc:setForLoop {dummyVar lb ub} {
    return [
	    concat for \{set $dummyVar \[expr $lb\]\} \
		\{\$$dummyVar <= \[expr $ub\]\} \
	       \{incr $dummyVar\} \{\n
	  ]
}

proc prsPreproc:setForeachLoop {dummyVar domain} {
    regsub -all , $domain " " t

    return [
	    concat foreach $dummyVar $t \{\n
	    ]
}


proc prsPreproc:appendDummyVarList { dummyVarList dummyVar } {
    upvar $dummyVarList varlist

    if {[lsearch -exact $varlist $dummyVar] != -1} {
	return 0
    }

    lappend varlist $dummyVar
    return 1
}



proc prsPreproc:parseForeachMacros {line cmd foreachCounter dummyVarList} {
    global MacroRangeString
    global MacroEnumString
    global PrsPreprocLineno

    upvar $cmd command
    upvar $foreachCounter ctr
    upvar $dummyVarList varlist

    set tmp [concat \#LINENO $PrsPreprocLineno \;]

    if {[regexp $MacroRangeString $line t dummyVar lb ub]} {
	# i.e. #foreach i = (0..100)
	
	append command [prsPreproc:setForLoop $dummyVar $lb $ub]
	
	# test if the dummy Variable already exists
	if {![prsPreproc:appendDummyVarList varlist $dummyVar]} {
	    error [concat error at line $PrsPreprocLineno: dummy variable 
		   $dummyVar has been declared in outer foreach 
		   loop.]
	}
	
	incr ctr
    } elseif {[regexp $MacroEnumString $line t dummyVar domain]} {
	# i.e. #foreach i = {red, blue, green}
	
	append command [prsPreproc:setForeachLoop $dummyVar $domain]
	
	# test if the dummy Variable already exists
	if {![prsPreproc:appendDummyVarList varlist $dummyVar]} {
	    error [concat error at line $PrsPreprocLineno: dummy variable 
		   $dummyVar has been declared in outer foreach 
		   loop.]
	}
	
	incr ctr
    } else {
	# error encountered
	
	error [concat error at line $PrsPreprocLineno: \
		   unrecognized \#foreach construct]
    }

    append command [concat puts \$fw \"$tmp\" \;]
    
}

proc prsPreproc:parseEndForeachMacros {cmd foreachCounter dummyVarList fw} {
    global PrsPreprocLineno

    upvar $cmd command
    upvar $foreachCounter ctr
    upvar $dummyVarList varlist

    # this is the lineno.
    set tmp [concat \#LINENO $PrsPreprocLineno \;]
    append command [concat puts \$fw \"$tmp\" \;]

    # close the braces

    append command \}\; 
    append command " "
    incr ctr -1
    if {$ctr == 0} {
	# execute the for loop.
	# puts $command
	eval $command
	
	# unset the command and the dummyVarList
	set command ""
	set varlist ""
	
    } elseif {$ctr <0} {
	error [concat error at line $PrsPreprocLineno: \
		   unmatched \#foreach encountered ] 
    }
}

proc prsPreproc:parseEndStructMacros {struct} {
    global MacroStructFullString
    global STRUCTS

    #puts $struct

    if {![regexp $MacroStructFullString $struct t structname elements]} {
	error "syntax error in struct definition."
    }	

    if {[info exists STRUCTS($structname)]} {
	error "structure $structname has been declarared"
    }
    
    set elementList [split $elements \;]
    foreach i $elementList {
	set i [string trim $i]
	catch {unset dummy}
	append STRUCTS($structname) "  " \
	    [split [prsPreproc:expandMacroStructTypes $i dummy] \;]
    }
}

proc prsPreproc:expandMacroStructTypes {line structVars} {
    global MacroTypeDeclarationString
    global STRUCTS

    upvar $structVars sv

    set flag 0

    while {[regexp $MacroTypeDeclarationString $line t name type]} {
	set expandedType ""

	if {[string match $* $type]} {
	    # ie. the type is a structure defined previously
	    
	    set tmptype [string trimleft $type \$]
	    
#	    if {![info exists STRUCTS($tmptype)]} {
#		error [concat structure $tmptype is not defined]
#	    }

	    if {![info exists STRUCTS($tmptype)]} {
		set flag 1
	    }
	} else {
	    set flag 1
	}

	if {$flag == 0} {
	    
	    foreach j $STRUCTS($tmptype) {
		catch {unset tmp}
		regsub -all : $j @ t1
		append tmp $name .[string trim $t1]
		set tmp [string trim $tmp]
		lappend expandedType $tmp
		lappend sv($name) [string trim [lindex [split $t1 @] 0]]
	    }

	} else {
	    
	    catch {unset tmp}
	    append tmp [string trim $name] @$type
	    lappend expandedType [string trim $tmp]

	}


	set t1 [prsPreproc:fixRegularExpressionSymbols $t]
#	regsub -all {\$} $t {\\$} t1

	regsub $t1 $line [join $expandedType \;] t2

	set line $t2

    }


    regsub -all @ $line : line
    return $line
}

proc prsPreproc:expandMacroStructVariables {line STRUCTVARS} {
    upvar $STRUCTVARS sv
    global MacroStructVariableExpandFlag 
    
    # only the lines starting with controls, awaits and reads are
    # expanded.

    set startPattern {controls|reads|awaits}
    set endPattern {init|update|endatom}
    
    set line1 [string trim $line]
    set type ""
    
    if {[regexp $startPattern $line]} {
	set MacroStructVariableExpandFlag 1

	# the identifier must always be the first word in the line.
	set type [lindex $line1 0]
	set line1 [lreplace $line1 0 0]
    }


    # if expansion is not needed, return the original line
    if {[regexp {endmodule} $line1]} {
	# end of module, forget all the structvariables
	# catch {unset sv}
	set MacroStructVariableExpandFlag 0
	return $line
    }

    if {[regexp $endPattern $line1] || $MacroStructVariableExpandFlag == 0} {
	set MacroStructVariableExpandFlag 0
	return $line
    }

    # split the list of variables and do the replacement
    if {$MacroStructVariableExpandFlag} {
	set variables [split $line1 ,]
	set expandedVar ""

	foreach v $variables {
	    set v [string trim $v]
	    lappend expandedVar [prsPreproc:structVariableExpand $v sv]
	}

	set line1 [join $expandedVar ,]
	set line ""
	append line $type " "
	append line $line1
    }

    return $line

}


proc prsPreproc:structVariableExpand {var STRUCTVARS} {
    upvar $STRUCTVARS sv

    # see if it is prime. Strip the prime.
    if {![string compare [string index $var [expr [string length $var] - 1]] \
	     "'"]} {
	set prime '
    } else {
	set prime ""
    }

    set var [string trimright $var ']

    # expand by looking up $STRUCTVARS
    set expanded ""
    set pattern {([^ \.]+)\.([^ ]*)}
    if {![regexp $pattern $var t name member]} {
	set name $var
	set member ""
    }

    if {[info exists sv($name)]} {
	foreach i $sv($name) {
	    if {![string compare $member [lindex [split $i .] 0]]} {
		lappend expanded $name.$i$prime
	    } elseif { $member == ""} {
		lappend expanded $name.$i$prime
	    }
	}

	set line [join $expanded ,]
#	puts $expanded
    } else {
	set line $var$prime
    }

    return $line
    
}

proc prsPreproc:fixRegularExpressionSymbols {line} {
    
    set regexpSymbols { \[ \$ \( \) \] }
    
    foreach s $regexpSymbols {
	catch {unset pattern target}
	    
	append pattern \\ $s
	regsub -all $pattern $line $pattern t
	set line $t
    }

    return $line
}

proc prsPreproc:fixConfusedSymbols {line dummyVarList} {
    set confusedSymbols  { \[ \{ #  _  \} \] }
			    
    foreach var $dummyVarList {
	regsub -all \\$$var $line @@$var t
	set line $t
    }

    regsub -all \\$ $line \\\$ t
    regsub -all @@ $t \$ line
    
    
    foreach s $confusedSymbols {
	catch {unset pattern target}
	    
	append pattern \\ $s
	regsub -all $pattern $line $pattern t
	set line $t
    }
	

    return $line
}
		      

# This is the spec parser.
# A spec parser is a preprocessor that splits the input file and
# invokes the each of the spec parser.


docproc read_spec {filename} {
    {Read the specification file.}
    {<filename>}
    {This command reads a file containing all the specifications.}
} {
    # all specification keywords
    global AllSpecList
    global MOCHATMPDIR
    #set TmpDir  "/tmp"
    
    catch {unset fd}

    # check if the file exists
    if {[catch {set fp [open $filename "r"]}]} {
	error [concat $filename not found.]
    }

    # tmp spec files are stored in the /tmp directory
    # open all spec files
    foreach i $AllSpecList {
	set tmpFilename $MOCHATMPDIR/mocha$i.spec
	if {[catch {set fd($i) [open $tmpFilename "w"]}]} {
	    foreach i [array names fd] {
		catch {close $fd($i)}
	    }
	    error [concat Cannot write $tmpFilename]
	}
    }
    
    catch {unset whichSpec}
    
    set lineno 0
    while {![eof $fp]} {
	incr lineno
	set line [gets $fp]
	set line [string trim $line]
	if { $line == "" } {
	    continue
	}
	
	# remove all the comments 
	# all comments start with --
	
	if { [string compare "--" [string range $line 0 1]] == 0} {
	    continue
	}

	if { ![info exists whichSpec] } {
	    set specName [lindex $line 0]
	    if {[lsearch -exact $AllSpecList $specName] == -1} {
		foreach i [array names fd] {
		    catch {close $fd($i)}
		}
		error [concat No specification keyword found at line $lineno.]
	    }
	    set whichSpec $specName
	    puts $fd($whichSpec) [concat \#LINENO $lineno ";"]
	    set line [lreplace $line 0 0]
	}
	
	puts $fd($whichSpec) $line
	# puts [concat $lineno $line]
	# puts [string index $line [expr [string length $line] - 1]]
	if {![string compare [string index $line [expr [string length $line] - 1]] ";"]} {
	  #  puts here
	    puts $fd($whichSpec) \n
	    unset whichSpec
	}
    }
    

    # close all file pointers
    foreach i [array names fd] {
	catch {close $fd($i)}
    }
    
    catch {close $fp}

    # now, call each of the specification parser.
    foreach i [array names fd] {
	catch {unset cmd}
	append cmd $i _read " " $MOCHATMPDIR/mocha$i.spec
	IntfWriteStdout [eval $cmd]
    }

    # now, remove all the tmp files.
    # foreach i [array names fd] {
    #	catch {unset cmd}
    #	append cmd "exec rm " $MOCHATMPDIR/mocha$i.spec
    #	catch {eval $cmd}
    #}

}

docproc show_spec {args} {
    {show the specifications} 
    {\[-hl\] \[&lt;specification name&gt; &lt;specification name&gt; ... \]}
    {
	This command returns all the formulas of the specified specification. 
	If the specification name is not specified, all the formulas are listed.
	Command options:<p>
	
	<dl>
	
	<dt> -l
	<dd> Long listing. Prints the names as well as the formulae.

	<dt> -h
	<dd> Print usage.

	<dt> &lt;specification name&gt; &lt;specification name&gt; ...
	<dd> Specify the listing of formulae of the particular logic. 
	For example, if "inv" is specified, only the invariant formulae 
	are listed.

	</dl>
    } 
} {
    global AllSpecList
    set Usage_showSpec [concat Usage: show_spec \[-lh\] \[<spec> <spec> ...\]]
    set specList {}
    set optString ""

    foreach arg $args {
	switch -regexp -- $arg {
	    -h {IntfWriteStdout $Usage_showSpec\n; return}
	    -l {append optString " -l "}
	    ([0-9a-zA-Z]+) {lappend specList $arg}
	    default {IntfWriteStderr $Usage_showSpec\n; return}
	}
    }

    if {[llength $specList] == 0} {
	set specList $AllSpecList
    }
    foreach specName $specList {
	catch {unset cmd}
	append cmd $specName _print $optString
	if {![catch {eval $cmd} out]} {
	    IntfWriteStdout [concat $specName specifications:]\n
	    IntfWriteStdout $out\n
	
	}
    }
}


#to initialize
prsPreproc:initialize







