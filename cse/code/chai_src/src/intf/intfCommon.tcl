###CFile#######################################################################
#
#  FileName    [intfCommon.tcl]
#
#  PackageName [intf]
#
#  Synopsis    [Routines for the user interface]
#
#  Description [This file contains routines for input/output which is common
#		to both sessions with or without graphical user interface.]
#
#  SeeAlso     [optional]
#
#  Author      [Freddy Mang]
#
#  Copyright   [Copyright (c) 1994-1996 The Regents of the Univ. of California.
#  All rights reserved.
#
#  Permission is hereby granted, without written agreement and without license
#  or royalty fees, to use, copy, modify, and distribute this software and its
#  documentation for any purpose, provided that the above copyright notice and
#  the following two paragraphs appear in all copies of this software.
#
#  IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
#  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
#  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
#  CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#  THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
#  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
#  FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN
#  "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE
#  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.]
#
##############################################################################
#
# Std I/O, assuming no gui enabled
#

proc IntfWriteStdout {msg} {
    puts -nonewline $msg
    update idletasks
}

proc IntfWriteStderr {msg} {
    puts -nonewline $msg
    update idletasks
}


#
# help in Mocha
#

proc IntfHelpAddCommand {fun_name synopsis arguments description} {
    global MOCHACMDDES
    global MOCHACMDSYN
    global MOCHACMDARG

    catch {unset MOCHACMDDES($fun_name)}
    catch {unset MOCHACMDSYN($fun_name)}
    catch {unset MOCHACMDARG($fun_name)}
    set MOCHACMDDES($fun_name) $description
    set MOCHACMDSYN($fun_name) $synopsis
    set MOCHACMDARG($fun_name) $arguments
    
}


proc IntfHelpReadPackageHelpFiles {} {
    global MOCHACMD
    global MOCHAHELPDIR

    if {![catch {set allcmdfiles [glob $MOCHAHELPDIR/*]}]} {
	foreach cmdfile $allcmdfiles {
	    set filename [file tail $cmdfile]
	    if { ! [string compare [file extension $filename] ".txt"] } {
		regexp {([^.]+)Cmd\.txt} $filename t cmd
		set MOCHACMD($cmd) $cmdfile
	    }
	}
    } else {
	IntfWriteStderr "No help available.\n"
    }
}

proc IntfHelpPrintHelp {filter} {
    global MOCHACMD
    global MOCHACMDUSAGE
    
    # a lot of spaces.
    set spaces "                                     "
    set fieldLength 25

    if {[llength $filter] > 1} {
	IntfWriteStderr $MOCHACMDUSAGE(help)
	IntfWriteStderr "\n"
	return
    } 
    
    if {$filter == {}} {
	set filter *
    }
    
    if {![info exists MOCHACMD($filter)]} {
	set allCmd [lsort [array names MOCHACMD]]
	set i 0
	foreach cmd $allCmd {
	    if {[string match $filter $cmd] && 
	    [string compare [string index $cmd 0] "_"] } {
		IntfWriteStdout $cmd
		incr i
		if {$i == 3} {
		    IntfWriteStdout \n
		    set i 0
		} else {
		    # pack spaces
		    set j [expr $fieldLength - [string length $cmd]]

		    if {$j <0} {
			set j 0
		    }

		    IntfWriteStdout [string range $spaces 0 $j]
		}
	    }
	}
	if {$i != 0} {
	    IntfWriteStdout \n
	}
    } else {
	if {[catch {set fd [open $MOCHACMD($filter)]}]} {
	    IntfWriteStderr "No help available for $filter\n"
	    return 
	}
	while {[gets $fd line] >= 0} {
	    IntfWriteStdout $line\n
	}
	close $fd
    }

}


proc IntfUpdateDocForTclCommands {} {
    global MOCHACMDDES
    global MOCHACMDSYN
    global MOCHACMDARG
    global MOCHASRCDIR

    # first create a file containing the command documentations
    set filename $MOCHASRCDIR/intf/intfTclCmdDoc.c
    set delimiter1 \
	{/**Function********************************************************************}
    
    set delimiter2 \
	{******************************************************************************/}
    

    if {[catch {set fw [open $filename "w"]}]} {
	error "Cannot write $filename"
    }
    
    foreach cmd [array names MOCHACMDDES] {
	puts $fw $delimiter1
	puts $fw "Synopsis           \[$MOCHACMDSYN($cmd)\]"
	puts $fw "SideEffects        \[none\]"
	puts $fw "CommandName        \[$cmd\]"
	puts $fw "CommandSynopsis    \[$MOCHACMDSYN($cmd)\]"
	puts $fw "CommandArguments   \[$MOCHACMDARG($cmd)\]"
	puts $fw "CommandDescription \[$MOCHACMDDES($cmd)\]"
	puts $fw $delimiter2
	puts $fw \n
	puts $fw {/* {} */}
	puts $fw \n
	
    }
    
    close $fw

    IntfWriteStdout "$filename created\n"
}


# greeting
proc IntfGreetings {} {
    
    IntfWriteStdout "Welcome to MOCHA 1.0\n"
    IntfWriteStdout "Please report any problems to mocha@eecs.berkeley.edu\n"
}

#
# some utility functions, such as random number generation
#

proc randomInit seed {
    global rand
    set rand(ia) 9301
    set rand(ic) 49297
    set rand(im) 233280
    set rand(seed) $seed
}

proc random {} {
    global rand
    set rand(seed) [expr ($rand(seed)*$rand(ia) + $rand(ic)) % $rand(im)]
    return [expr $rand(seed) / double ($rand(im))]
}

proc randomRange range {
    expr int ([random]* $range)
}



#
# documented procedure declaration
#

proc docproc {name formals documentation body} {
    # for documentation, the fields are in the following order:
    # synopsis, arguments, descriptions
    
    if {[llength $documentation] != 3} {
	IntfWriteStderr [concat Incorrect number of arguments \n]
	IntfWriteStderr [concat in the documentation for command $name.\n]
	error
    }
    
    set synopsis     [lindex $documentation 0]
    set arguments    [lindex $documentation 1]
    set description  [lindex $documentation 2]
    
    IntfHelpAddCommand $name $synopsis $arguments $description
    
    proc $name $formals	$body
    
}

#
# rename help to IntfHelpPrintHelp
#

docproc help {args} {
    {Give help for the commands}
    {\[-rh\] \[&lt;pattern&gt;\]}
    {
	This command gives help on the commands. <p>
	Command options: <p>
	
	<dl>
	
	<dt> -r
	<dd> Force reloading of the command help files before printing the help.
	Command help files 
	are stored in the documentation area provided in the distribution.
	At times, the documentation may change.  This command forces the
	reloading of the help files to reflect the changes.  Only under
	rare circumstances, such as user define new tcl commands using docproc,
	do this option needed.

	<dt> -h
	<dd> Print usage.

	<dt> &lt;pattern&gt;
	<dd> If a pattern is given, only commands which match this pattern 
	are listed. If the pattern given is the exact name of a command,
	a long display giving the complete help on the command will be 
	printed. For example, if * given, all commands are listed.
	If sym* is given, all commands prefixed with "sym" will be listed.
	If no pattern is given, the default pattern is *.
	
	</dl>}
} {
    set pattern ""
    set Usage_help {Usage: help [-rh] <pattern>}

    foreach arg $args {
	switch -regexp -- $arg {
	    -h {IntfWriteStdout $Usage_help\n; return}
	    -r {IntfHelpReadPackageHelpFiles}
	    ([_0-9a-zA-Z]+) {lappend pattern $arg}
	    default {IntfWriteStderr $Usage_help\n; return}
	}
    }

    IntfHelpPrintHelp $pattern
}

#
# rename writeDoc to IntfUpdateDocForTclCommands
#

proc writeDoc {} {
    IntfUpdateDocForTclCommands
}



#
# rename some functions
#

rename exit old_exit


#
# this exit cleans up all the filedialog widget.
#

docproc exit {} {
    {Ends the current MOCHA session}
    {} 
    {This command ends the current mocha session and kills all windows.}
} {
    global MOCHATMPDIR

#    _mocha_end
    
    # cleans up the MOCHATMPDIR directory
    catch {exec sh -c "rm -rf $MOCHATMPDIR"}

    puts "Thank you for using MOCHA 1.0\n"

    old_exit
}

#
# This procedure sets the prompt
#

proc IntfSetPrompt {prompt} {
    global MOCHAPROMPT
    global tcl_prompt1

    set MOCHAPROMPT $prompt
    
    set tcl_prompt1 {global $MOCHAPROMPT; IntfWriteStdout $MOCHAPROMPT}

}

#
# this clear clears the
#
proc clear {} {
    exec clear
}








