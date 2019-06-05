###Tcl/TkFile##################################################################
#
#  FileName    [trcMain.tcl]
#
#  PackageName [trc]
#
#  Synopsis    [(error) trace handling package]
#
#  Description [This file contains all tcl/tk procedures that 1. draws the 
#               error trace window 2. handles the printing of error trace.]
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
###############################################################################

#
# procs exported by this package 
#
# trc_errorTraceInit
# trc_errorTraceClear
# trc_searchMethodSet
# trc_returnErrorTraceWindow
# trc_errorTraceWindowFire
# trc_propertyFailedSet
# trc_errorTraceWindowDestroy
# trc_errorTraceAppend
# trc_variableOrderSet
# trc_moduleRead
# trc_errorTraceRefresh

# procs used by this package.  not supposed to be visible to the users.
#
# trcWriteErrorTrace
# trctraceDisplayStateTransitionMode
# trcReturnErrorTraceVariableHeader
# trcWriteVariableHeader
# trcWriteSearchMethod
# trcSpacesPack
# trcReturnErrorTraceTextWidget
# trcTraceWrite
# trcWritePropertyFailed

#
#
# (global) associative arrays associated with each error trace window
#
# global trcVariableOrder
# global trcModule
# global trcPropertyFailed
# global trcSearchMethod
# global trcErrorTrace
# global trcTraceDisplayMode
#

#
# example of use of the procs
#
# set errTrcID trc_ErrorTraceInit Pete
# trc_variableOrderSet 


proc trc_errorTraceWindowFire {module args} {
    global trcIDCounter

    set bcolor white
    set boldtxtfont  -*-courier-bold-*-normal--14-*-*-*-*-*-*-*	
    set txtfont -*-courier-medium-*-normal--14-*-*-*-*-*-*-*
    incr trcIDCounter
    set w .errorTraceWindow:$trcIDCounter

    catch {trc_errorTraceWindowDestroy $trcIDCounter}
    toplevel $w

    if {[llength $args] != 0} {
	wm title $w [lindex $args 0]
    } else {
	wm title $w "Error Trace"
    }

    # draw the menu bar

    frame $w.menu -relief raised -bd 2
    pack $w.menu -side top -fill x
    
    # the File menu button
    menubutton $w.menu.file -text "File" -menu $w.menu.file.m 
    menu $w.menu.file.m -tearoff 0
    $w.menu.file.m add command -label "Save" \
        -command {mainSaveFile}
    $w.menu.file.m add command -label "Close" \
	-command "trc_errorTraceWindowDestroy $trcIDCounter"

    pack $w.menu.file -side left

#    menubutton $w.menu.option -text "Option" -menu $w.menu.option.m 
#    menu $w.menu.option.m -tearoff 0
#    $w.menu.option.m add command -label "Select Variables" \
#	-command "sim:variablesSelectWindowFire $module"
#    pack $w.menu.option -side left

    # draw the error trace text widget
    frame $w.f -bd 2 
    pack $w.f -fill both -expand 1

    frame $w.f.f -bd 2
    pack $w.f.f -side top -fill both -expand 1

    scrollbar $w.f.sb \
	-orient horizontal
    pack $w.f.sb -side bottom -fill x

    frame $w.f.f.f1
    pack $w.f.f.f1 -side left -fill both -expand 1
    
    text $w.f.f.f1.t1 -bd 2 -relief groove -height 1 \
	-bg gray75 -wrap none \
	-xscrollcommand "$w.f.sb set" \
	-font $boldtxtfont \
	-state disabled 
    pack $w.f.f.f1.t1 -side top -fill both
    
    text $w.f.f.f1.t2 -bd 2 -relief sunken \
	-bg $bcolor \
	-yscrollcommand "$w.f.f.f2.sb set" \
	-wrap none \
	-font $txtfont \
	-state disabled
    pack $w.f.f.f1.t2 -side top -expand 1 -fill both

    frame $w.f.f.f2
    pack $w.f.f.f2 -side right -fill both
    
    scrollbar $w.f.f.f2.sb -command "$w.f.f.f1.t2 yview"
    pack $w.f.f.f2.sb -fill both -expand 1

    # bind the horizontal scrollbar to control both the header textbox
    # and the successor_states textbox
    
    $w.f.sb configure -command \
	[list BindXView [list $w.f.f.f1.t1 $w.f.f.f1.t2]]


    # draw the entry box, displaying information pertaining to module
    frame $w.f2 -bd 2
    pack $w.f2 -fill both 

    tixLabelEntry $w.f2.le1 -bd 2 \
	-label Module: 
    pack $w.f2.le1 -fill x 
    $w.f2.le1 subwidget label configure -width 20
    $w.f2.le1 subwidget entry insert 0 $module
    $w.f2.le1 subwidget entry configure -state disabled

    tixLabelEntry $w.f2.le2 -bd 2 \
	-label "Property Failed" \
	-state disabled
    pack $w.f2.le2 -fill x 
    $w.f2.le2 subwidget label configure -width 20
    
    tixLabelEntry $w.f2.le3 -bd 2 \
	-label "Search Method:" \
	-state disabled
    pack $w.f2.le3 -fill x 
    $w.f2.le3 subwidget label configure -width 20
    
    button $w.b -bd 2 -text "Close" \
	-command "trc_errorTraceWindowDestroy $trcIDCounter"
    pack $w.b

    return $trcIDCounter

}

proc trc_errorTraceClear {id} {

    if {[catch {set w [trc_returnErrorTraceWindow $id]}]} {
	return
    }

    set w1 [trcReturnErrorTraceTextWidget $id]
    $w1 configure -state normal
    $w1 delete 0.0 end
    $w1 configure -state disabled
    
}

proc trc_variableOrderSet {id variable_order} {
    global trcVariableOrder

    set w .errorTraceWindow:$id
    if {![winfo exists $w]} {
	error "trace window $id not exists"
    }
    
    set trcVariableOrder($id) $variable_order


    trcWriteVariableHeader $id
    trc_errorTraceClear $id
    trcWriteErrorTrace $id
}

proc trc_propertyFailedSet {id property_failed} {
    global trcPropertyFailed

    set w .errorTraceWindow:$id
    if {![winfo exists $w]} {
	error "trace window $id not exists"
    }
    
    set trcPropertyFailed($id) $property_failed
    trcWritePropertyFailed $id
}

proc trc_moduleRead {id} {
    global trcModule

    set w .errorTraceWindow:$id
    if {![winfo exists $w]} {
	error "trace window $id not exists"
    }
    
    puts trcModule($id)
}


proc trc_searchMethodSet {id search_method} {
    global trcSearchMethod

    set w .errorTraceWindow:$id
    if {![winfo exists $w]} {
	error "trace window $id not exists"
    }
    
    set trcSearchMethod($id) $search_method

    trcWriteSearchMethod $id
}

proc trc_errorTraceRefresh {id} {
    global trcVariableOrder
    

    if {[catch {trc_returnErrorTraceWindow $id}]} {
	return
    }

    trcWriteVariableHeader $id
    trc_errorTraceClear $id
    trcWriteErrorTrace $id
    trcWritePropertyFailed $id
    trcWriteSearchMethod $id
}

proc trc_errorTraceWindowDestroy {id} {
    global trcVariableOrder
    global trcModule
    global trcPropertyFailed
    global trcSearchMethod
    global trcErrorTrace

    catch {set w [trc_returnErrorTraceWindow $id]}

    catch {unset trcVariableOrder($id)}
    catch {unset trcModule($id)}
    catch {unset trcPropertyFailed($id)}
    catch {unset trcSearchMethod($id)}
    catch {unset trcErrorTrace($id)}
    catch {destroy $w}

}

proc trcWriteVariableHeader {id} {
    global trcVariableColumnSeparation
    global trcVariableOrder

    if {[catch {trc_returnErrorTraceWindow $id}]} {
	return
    }

    catch {unset header}
    set header [join $trcVariableOrder($id) $trcVariableColumnSeparation]
    append header $trcVariableColumnSeparation

    # write the successor_state window header
    set w [trcReturnErrorTraceVariableHeader $id]

    # write out the header for the simulation variables.
    $w configure -state normal
    $w delete 0.0 end
    $w insert insert $header
    $w configure -state disabled
}

proc trcWritePropertyFailed {id} {
    global trcPropertyFailed
    
    set w [trc_returnErrorTraceWindow $id]
    
    set w1 $w.f2.le2
    $w1 subwidget entry configure -state normal
    $w1 subwidget entry delete 0 end
    $w1 subwidget entry insert 0 $trcPropertyFailed($id)
    $w1 subwidget entry configure -state disabled

}

proc trcWriteErrorTrace {id} {
    global trcErrorTrace
    global trcModule
    global trcVariableOrder
    
    if {[catch {set w [trcReturnErrorTraceTextWidget $id]}]} { 
	return
    }
    
    foreach state $trcErrorTrace($id) {
	trcSpacesPack $id $state displayForm
	trcTraceWrite $w $trcModule($id) displayForm $trcVariableOrder($id)
    }
}

proc trcWriteSearchMethod {id} {
    global trcSearchMethod
    
    set w [trc_returnErrorTraceWindow $id]
    
    set w1 $w.f2.le3
    $w1 subwidget entry configure -state normal
    $w1 subwidget entry delete 0 end
    $w1 subwidget entry insert 0 $trcSearchMethod($id)
    $w1 subwidget entry configure -state disabled

}

proc trc_returnErrorTraceWindow {id} {

    set w .errorTraceWindow:$id
    if {![winfo exists $w]} {
	error "trace window $id not exists"
    } else {
	return $w
    }
}

proc trcReturnErrorTraceTextWidget {id} {
    
    set w [trc_returnErrorTraceWindow $id]
    set w1 $w.f.f.f1.t2
    
    return $w1
}

proc trcReturnErrorTraceVariableHeader {id} {
    
    set w [trc_returnErrorTraceWindow $id]
    set w1 $w.f.f.f1.t1
    
    return $w1

}

proc trc_errorTraceInit {module args} {
    global trcVariableOrder
    global trcModule
    global trcPropertyFailed
    global trcSearchMethod
    global trcErrorTrace

    set allVariables [show_vars -vALL $module]
    if {[info exists allVariables]} {
	set wid [trc_errorTraceWindowFire $module $args]
	set trcModule($wid) $module
	set trcPropertyFailed($wid) ""
	set trcSearchMethod($wid)  ""
	set trcErrorTrace($wid)  ""
	
	# only observable variables are shown
	catch {unset trcVariableOrder($wid)}
	foreach var $allVariables {
	    if {[string index $var 0] != "\$"} {
		lappend trcVariableOrder($wid) $var
	    }
	}
	
	trc_errorTraceRefresh $wid

	return $wid
    }

}

# the following is to format the states and print them out.

proc trc_errorTraceAppend {id state} {
    global trcModule
    global trcVariableOrder
    global trcErrorTrace

    if {[catch {set w [trcReturnErrorTraceTextWidget $id]}]} {
	return
    }
    
    lappend trcErrorTrace($id) $state
    # displayForm will be an associative array whose indexes
    # are the variables and whose value will be the state value
    # padded with spaces.

    trcSpacesPack $id $state displayForm
    trcTraceWrite $w $trcModule($id) displayForm $trcVariableOrder($id)
}

proc trcTraceWrite {w module displayForm variableOrder} {
    global trcTraceDisplayMode
    upvar $displayForm dv

    $w configure -state normal	
    switch -exact $trcTraceDisplayMode {
	"hardware" \
	    {$w insert end [trctraceDisplayHardwareMode dv]\n}
	"software" \
	    {$w insert end [trctraceDisplaySoftwareMode dv]\n}
	"state transition" \
	    {
		set tmp [trctraceDisplayStateTransitionMode \
			 $module dv $variableOrder];
		$w insert end [lindex $tmp 0]\n ;
		$w insert end [lindex $tmp 1]\n
	    }
	default \
	    {error "error in trcTraceWrite"}
    }
    $w configure -state disabled
}

proc trctraceDisplayStateTransitionMode {module displayValue variableOrder} {

    global trcModule
    
    upvar $displayValue dv

    set display1 ""
    set display2 ""

    foreach var $variableOrder {
	if {[isEventVariable $module $var] || 
	    [isExternalVariable $module $var]} {
	    append display1 $dv($var)
	    regsub -all {\$|[a-zA-Z0-9]|'|_} $dv($var) " " tmp
	    append display2 $tmp
	} else {
	    append display2 $dv($var)
	    regsub -all {\$|[a-zA-Z0-9]|'|_} $dv($var) " " tmp
	    append display1 $tmp
	}
    }
    return [list $display1 $display2]
}



proc trcSpacesPack {id state displayForm} {
    
    global trcVariableOrder
    global trcVariableColumnSeparation
    
    upvar $displayForm dv

    # set variableValuePairs [state_print $state]
    set variableValuePairs $state

    # split the variableValuePair
    foreach variableValue $variableValuePairs {
	set tmp [split $variableValue =]
	set tmpvar [lindex $tmp 0]
	set tmpvalue [lindex $tmp 1]
	
	if {$tmpvalue == ""} {
	    set tmpvalue x
	}
	set dv($tmpvar) $tmpvalue
    }
    
    # now, add the $columnSeparation for formatting.
    set colwidth [string length $trcVariableColumnSeparation]
    foreach var $trcVariableOrder($id) {
	if {![info exists dv($var)]} {
	    set dv($var) " "
	}
	set i [string length $dv($var)]
	set len [expr [string length $var] + $colwidth]
	while {$i < $len} {
	    append dv($var) " "
	    incr i
	}
	
    }
}

# refresh the whole trace.

# set some global variables.

set trcIDCounter 0
set trcTraceDisplayMode "state transition"
set trcVariableColumnSeparation {     }





