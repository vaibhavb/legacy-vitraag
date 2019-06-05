# ---
# procedures
# ---


#
# Fire the variable select window
#

proc simVariablesSelectWindowFire {module} {
    global simVariablesShown
    global simVariablesNotShown

    set w .variablesSelectWindow
    set fromcolor #b2e2fe
    set tocolor   #ffe2ef
            
    catch {destroy $w}
    toplevel $w
    
    wm title $w "Variable Selection"
    wm minsize $w 470 300
    wm geometry $w 500x330
    
    # draw the title
    label $w.tt -text "Select the variables to shown in simulation"
    pack $w.tt -side top -pady 5 -anchor s

    # draw the grooved frame
    frame $w.lf -relief groove -bd 2
    pack $w.lf -in $w -fill both -expand 1 -padx 5

    set lf $w.lf
    
    # draw the "Variables Available" list box
    frame $lf.f1 -bd 2 -relief raised
    pack $lf.f1 -in $lf -side left -fill both -expand 1 \
            -padx 5  -pady 5
    
    label $lf.f1.lb -text "Variables Available" -bd 2
    tixScrolledListBox $lf.f1.lbox \
            -bd 2 \
            -scrollbar auto \
            -relief sunken 

    $lf.f1.lbox subwidget listbox config -bg $fromcolor
    
    pack $lf.f1.lb -fill x -side top -in $lf.f1
    pack $lf.f1.lbox -fill both -side top -in $lf.f1 -expand 1

    # draw the Add, Remove and Show private variables buttons.
    frame $lf.f3 -bd 2 -relief flat
    pack $lf.f3 -in $lf -side left

    tixButtonBox $lf.f3.bbox -orientation vertical -relief flat
    pack $lf.f3.bbox -in $lf.f3 -side top
        $lf.f3.bbox add add    -text "Add -->"   -width 10
        $lf.f3.bbox add remove -text "<-- Remove" -width 1
    # define a temporary variable that stores the changes in
    # variablesShown and variablesNotShown
    catch {unset tmpVariableShown tmpVariablesNotShown}
    set tmpVariablesShown $simVariablesShown($module)
    set tmpVariablesNotShown $simVariablesNotShown($module)

    $lf.f3.bbox subwidget add config -command \
	"simAddVariablesShown $module"
    $lf.f3.bbox subwidget remove config -command \
	"simRemoveVariablesShown $module"

    label $lf.f3.lb -text "show private vars" \
            -font -*-helvetica-medium-r-normal-*-10-*-*-*-*-*-*-*
    pack $lf.f3.lb -side left -pady 5

    checkbutton $lf.f3.cb
    pack $lf.f3.cb -side right
    

    # draw the "Variables shown" list box
    frame $lf.f2 -bd 2 -relief raised
    pack $lf.f2 -in $lf -side right -fill both -expand 1 \
            -padx 5  -pady 5
    
    label $lf.f2.lb -text "Variables Selected" -bd 2
    tixScrolledListBox $lf.f2.lbox \
            -bd 2 \
            -scrollbar auto \
            -relief sunken 

    $lf.f2.lbox subwidget listbox config -bg $tocolor

    pack $lf.f2.lb -fill x -side top -in $lf.f2
    pack $lf.f2.lbox -fill both -side top -in $lf.f2 -expand 1

    # draw the "ok, cancel, reset" button
    tixButtonBox $w.bbox
       $w.bbox add ok     -text Ok     -width 10
       $w.bbox add reset  -text Reset  -width 10
       $w.bbox add cancel -text Cancel -width 10
    $w.bbox subwidget ok configure -command \
	"simRefreshSimulator $module;
         destroy $w"
    $w.bbox subwidget cancel configure -command \
	"
         set simVariablesShown($module) [list $tmpVariablesShown];
         set simVariablesNotShown($module) [list $tmpVariablesNotShown];
         destroy $w
        "
    $w.bbox subwidget reset configure -command \
	"simResetVariablesShown $module"


    pack $w.bbox -side top -in $w
    
    # insert the variables in simVariablesShown($module) into
    # the "Variables Selected" listbox

    set tolistbox [$lf.f2.lbox subwidget listbox]
    $tolistbox delete 0 end
    
    foreach var $simVariablesShown($module) {
	$tolistbox insert end $var
    }

    
    # insert the variables in simVariablesShown($module) into
    # the "Variables Selected" listbox

    set fromlistbox [$lf.f1.lbox subwidget listbox]
    $fromlistbox delete 0 end
    
    foreach var $simVariablesNotShown($module) {
	$fromlistbox insert end $var
    }

    # has to wait for the variables selection window to be destroyed
    grab $w
    tkwait window $w
}

proc simRefreshSimulator {module} {
    global simVariablesShown
    global simColumnSeparation
    global traceDisplayMode
    global simSimulationTrace
    
    set w .simSimulationWindow:$module 
    set w1 [$w.f.pw subwidget stp] 
    set w2 [$w1.lf subwidget frame].f1.t2

    catch {unset display_value}
    # clear the successorState window first
    $w2 configure -state normal
    $w2 delete 0.0 end

    # set up some variables
    simVariableWidth $module variableWidth

    foreach state $simSimulationTrace($module) {

	simformatdisplay_new $module $state display_value variableWidth
	simWriteToSimulationWindow $w2 $module display_value
    }
    
    $w2 see end
    $w2 configure -state disabled
    
    # write the headers
    simWriteHeader $module $simVariablesShown($module)

    # write the successor_states
    simWriteSuccessorStates $module

}

proc simAddVariablesShown {module} {

    global simVariablesShown
    global simVariablesNotShown

    set w .variablesSelectWindow 
    set fromlistbox [$w.lf.f1.lbox subwidget listbox]
    set tolistbox   [$w.lf.f2.lbox subwidget listbox]

    catch {unset select}
    set select [$fromlistbox curselection]
    if {$select != ""} {
	set varselected [selection get]
	lappend simVariablesShown($module) $varselected
	$tolistbox insert end $varselected

	set index [lsearch -exact $simVariablesNotShown($module) $varselected]
	set simVariablesNotShown($module) \
	    [lreplace $simVariablesNotShown($module) $index $index]
	$fromlistbox delete $select
    }

}

proc simRemoveVariablesShown {module} {

    global simVariablesShown
    global simVariablesNotShown

    set w .variablesSelectWindow 
    set fromlistbox [$w.lf.f1.lbox subwidget listbox]
    set tolistbox   [$w.lf.f2.lbox subwidget listbox]

    catch {unset select}
    set select [$tolistbox curselection]
    if {$select != ""} {
	set varselected [selection get]
	lappend simVariablesNotShown($module) $varselected
	$fromlistbox insert end $varselected

	set index [lsearch -exact $simVariablesShown($module) $varselected]
	set simVariablesShown($module) \
	    [lreplace $simVariablesShown($module) $index $index]
	$tolistbox delete $select

    }

}

proc simResetVariablesShown {module} {
    global simVariablesShown
    global simVariablesNotShown

    set w .variablesSelectWindow 
    set fromlistbox [$w.lf.f1.lbox subwidget listbox]
    set tolistbox   [$w.lf.f2.lbox subwidget listbox]

    catch {unset tmp}
    catch {unset simVariablesShown($module)}
    catch {unset simVariablesNotShown($module)}

    set tmp [simObtainAllVariables $module]
    set simVariablesShown($module) [lindex $tmp 0]
    set simVariablesNotShown($module) [lindex $tmp 1]

	
    $fromlistbox delete 0 end
    foreach var $simVariablesNotShown($module) {
	$fromlistbox insert end $var
    }

	
    $tolistbox delete 0 end
    foreach var $simVariablesShown($module) {
	$tolistbox insert end $var
    }

}

#
# The simulator 
#

proc simSimulatorWindowFire {module} {
    global simulationSteps
    global simulationMode
    global simulationTurn
    global traceDisplayMode

    set w .simSimulationWindow:$module

    set bcolor white
    set boldtxtfont fixed
    set txtfont fixed
#    set boldtxtfont  -*-courier-bold-*-normal--14-*-*-*-*-*-*-*
#    set txtfont -*-courier-medium-*-normal--14-*-*-*-*-*-*-*

    if {[winfo exists $w]} {
	wm iconify $w
	wm deiconify $w
	return 
    }

    toplevel $w
    
    wm title $w "Simulation of Module $module"
    wm geometry $w 815x750
    # menu

    frame $w.menu -relief raised -bd 2
    pack $w.menu -side top -fill x
    
    # the File menu button
    menubutton $w.menu.file -text "File" -menu $w.menu.file.m 
    menu $w.menu.file.m -tearoff 0 
#    $w.menu.file.m add command -label "Load input waveform" \
#        -command {mainLoadFile}
#    $w.menu.file.m add command -label "Save" \
#        -command {mainSaveFile}
    $w.menu.file.m add command -label "Close" \
	-command "sim_end $module; destroy $w"

    pack $w.menu.file -side left

    menubutton $w.menu.option -text "Option" -menu $w.menu.option.m 
    set m [menu $w.menu.option.m -tearoff 0]
    $m add command -label "Select Variables" \
	-command "simVariablesSelectWindowFire $module"
    $m add command -label "Change Mode" \
	-command "simChangeModeMenuInvoked $module"
    $m add cascade -label "Display Mode" \
	-menu $m.sub1

    set m2 [menu $m.sub1 -tearoff 0]
    $m2 add radio -label {one line} \
	-variable traceDisplayMode($module) \
	-value {hardware} \
	-command "simRefreshSimulator $module"
    $m2 add radio -label {two lines} \
	-variable traceDisplayMode($module) \
	-value {state transition} \
	-command "simRefreshSimulator $module"

    $w.menu.option.m configure -postcommand \
	"simMenuOptionPostCommand $module $w.menu.option.m"
    pack $w.menu.option -side left

    # draw the paned window
    frame $w.f -bd 2
    pack $w.f -padx 5 -pady 3 -expand 1 -fill both

    tixPanedWindow $w.f.pw -panebd 2 -panerelief ridge 
    set successor_state_pane  [$w.f.pw add ssp -size 150]
    set simulation_trace_pane [$w.f.pw add stp -size 150]

    pack $w.f.pw -side top -fill both -expand 1

    #
    # draw the successor_state_pane
    #

    set ssp $successor_state_pane
    
    tixLabelFrame $ssp.lf -bd 2 -label "Successor States"
    set ssplf [$ssp.lf subwidget frame]
    pack $ssp.lf -fill both -expand 1

    scrollbar $ssplf.sb \
	-orient horizontal
    pack $ssplf.sb -side bottom -fill x

    frame $ssplf.f1
    pack $ssplf.f1 -side left -fill both -expand 1
    
    text $ssplf.f1.t1 -bd 2 -relief groove -height 1 \
	-bg gray75 -wrap none \
	-xscrollcommand "$ssplf.sb set" \
	-font $boldtxtfont
    pack $ssplf.f1.t1 -side top -fill both

    text $ssplf.f1.t2 -bd 2 -relief sunken \
	-yscrollcommand "$ssplf.f2.sb set" \
	-wrap none \
	-font $txtfont 
    pack $ssplf.f1.t2 -side top -expand 1 -fill both

    frame $ssplf.f2
    pack $ssplf.f2 -side right -fill both
    
    scrollbar $ssplf.f2.sb -command "$ssplf.f1.t2 yview"
    pack $ssplf.f2.sb -fill both -expand 1


    # bind the horizontal scrollbar to control both the header textbox
    # and the successor_states textbox
    
    $ssplf.sb configure -command \
	[list BindXView [list $ssplf.f1.t1 $ssplf.f1.t2]]

    #
    # draw the simulation_trace_pane
    #

    set stp $simulation_trace_pane
    
    # draw the "simulation mode" etc. labels
    
    frame $stp.f 
    pack $stp.f -side top -fill x

    button $stp.f.but -text Go! \
	-command "simStartSimulation $module"
    
    pack $stp.f.but -side right -padx 4

    $stp.f.but configure -command \
	"simStartSimulation $module"

    radiobutton $stp.f.userbut -variable simulationTurn \
	-value user -state disabled
    label $stp.f.userlb -text User 
    radiobutton $stp.f.systembut -variable simulationTurn \
	-value system -state disabled
    label $stp.f.systemlb -text System    
    
    pack $stp.f.systemlb $stp.f.systembut $stp.f.userlb $stp.f.userbut -side right
    
    # draw the simulation trace textboxes

    tixLabelFrame $stp.lf -bd 2 -label "Simulation Trace" 
    set stplf [$stp.lf subwidget frame]
    pack $stp.lf -fill both -expand 1

    scrollbar $stplf.sb \
	-orient horizontal
    pack $stplf.sb -side bottom -fill x

    frame $stplf.f1
    pack $stplf.f1 -side left -fill both -expand 1
    
    text $stplf.f1.t1 -bd 2 -relief groove -height 1 \
	-bg gray75 -wrap none \
	-xscrollcommand "$stplf.sb set" \
	-font $boldtxtfont
    pack $stplf.f1.t1 -side top -fill both
    
    text $stplf.f1.t2 -bd 2 -relief sunken \
	-bg $bcolor \
	-yscrollcommand "$stplf.f2.sb set" \
	-wrap none \
	-font $txtfont
    pack $stplf.f1.t2 -side top -expand 1 -fill both

    frame $stplf.f2
    pack $stplf.f2 -side right -fill both
    
    scrollbar $stplf.f2.sb -command "$stplf.f1.t2 yview"
    pack $stplf.f2.sb -fill both -expand 1

    # bind the horizontal scrollbar to control both the header textbox
    # and the successor_states textbox
    
    $stplf.sb configure -command \
	[list BindXView [list $stplf.f1.t1 $stplf.f1.t2]]

    # tag binding
    $ssplf.f1.t2 tag configure state_highlight \
	-foreground white \
	-background black

    # mouse binding
    bind $ssplf.f1.t2 <Button-1> \
	"stateHighlight $ssplf.f1.t2 \$traceDisplayMode($module)"
    bind $ssplf.f1.t2 <Button-2> \
	"stateHighlight $ssplf.f1.t2 \$traceDisplayMode($module)"
    bind $ssplf.f1.t2 <Button-3> \
	"stateHighlight $ssplf.f1.t2 \$traceDisplayMode($module)"
    bind $ssplf.f1.t2 <B1-Motion> "break;"

}


# supporting procedures
proc simMenuOptionPostCommand {module w} {
    if {![sim_info $module isStartOfRound]} {
	$w entryconfigure {Change Mode} \
	    -state disabled
    } else {
	$w entryconfigure {Change Mode} \
	    -state normal
    }
}

proc simChangeModeMenuInvoked {module} {
    global simulationMode
    global simulationTurn
    global simComponents

    set ok [simSimulationModeSelectWindowFire $module]

    # commit changes, and change the simulation mode
    if {$ok == 0} {
	return 
    }

    switch -exact $simulationMode($module) {
	random {sim_mode -u $module}
	manual {sim_mode -s $module}
	game   {catch {unset x}
	    append x "sim_mode -n " $module " " 
	    append x [simObtainSelectedAtomNames $module];
	    eval $x}
	default {puts error}
    }
    
    if {[sim_info $module isUserTurn]} {
	set simulationTurn user
    } else {
	set simulationTurn system
    }

    # update the simulator to reflect change in variable ordering
    simRefreshSimulator $module

    # also change the successorStateWindow
    simWriteSuccessorStates $module

}

proc stateHighlight {w mode} {
    global simulationTurn

    set tagname state_highlight

    if {[string match $simulationTurn system]} {
	return -code break
    } 

    if [$w compare end == "current lineend + 1c"] {
	return -code break
    }
    
    $w configure -state normal
    $w tag remove $tagname 0.0 end

    $w tag add $tagname \
	"current linestart" "current lineend + 1 char"
    
    if {[string match $mode "state transition"]} {
	set tmp [lindex [$w tag ranges $tagname] 0]
	regexp {([0-9]+).0} $tmp t line
	if {[expr $line % 2 == 1.0]} {
	    $w tag add $tagname \
		[expr $line + 1].0 "[expr $line+1].end + 1 char"
	} else {
	    $w tag add $tagname \
		[expr $line - 1].0 "[expr $line-1].end + 1 char"
	}
    }
    $w configure -state disabled
    
    return -code break
}

proc SimInitSimulatorGUI {module} {
    global simVariablesShown
    global simVariablesNotShown
    global simSimulationTrace
    global traceDisplayMode
    global simulationMode
    global simulationTurn
    global simComponents

    set w .simSimulationWindow:$module
    
    # clearing all arrays pertaining to $module
    catch {unset simVariablesShown($module)}
    catch {unset simTrace($module)}
    
    set simSimulationTrace($module) ""
    
    # check if the simulation window for $module exists.
    # if not, fire up.
    # also fire up the simulation mode window
    
    if {![winfo exists $w]} {
	# determine the simulation mode
	set ok [simSimulationModeSelectWindowFire $module]
	if {$ok == 0} {
	    return
	}

	# see if the module was being simulated. If yes, quit the
	# original one, and then restart anew.

	if {[sim_info $module isSimulated]} {
	    sim_end $module
	}
	
	switch -exact $simulationMode($module) {
	    random {sim_start -u $module}
	    manual {sim_start -s $module}
	    game   {catch {unset x}
		append x "sim_start -n " $module " ";
		append x [simObtainSelectedAtomNames $module];
		eval $x}
	    default {puts error}
	}
	
	if {[sim_info $module isUserTurn]} {
	    set simulationTurn user
	} else {
	    set simulationTurn system
	}

	simSimulatorWindowFire $module

    }

    # determine the variables to be shown in the simulation
    catch {unset tmp}
    set tmp [simObtainAllVariables $module]
	
    set simVariablesShown($module) [lindex $tmp 0]
    set simVariablesNotShown($module) [lindex $tmp 1]

    # this is only temporary
    set traceDisplayMode($module) {hardware}

    # write the variable names in the successor-state window
    simWriteSuccessorStates $module
    simWriteHeader $module $simVariablesShown($module)

}

proc simObtainAllVariables {module} {
    
    catch {unset observable_var unobservable_var}
    set observable_var ""
    set unobservable_var ""

    set allvars [join [sim_info $module variableOrdering]]
    
    foreach var $allvars {
	if {[string index $var 0] != "\$"} {
	    lappend observable_var $var
	} else {
	    lappend unobservable_var $var
	}
    }

    return [list $observable_var $unobservable_var]
}

proc simWriteSuccessorStates {module} {
    global simColumnSeparation
    global traceDisplayMode
    
    set w .simSimulationWindow:$module 
    set w1 [$w.f.pw subwidget ssp] 
    set w2 [$w1.lf subwidget frame].f1.t2

    catch {unset display_value}
    # clear the successorState window first
    $w2 configure -state normal
    $w2 delete 0.0 end

    # set up some variables
    simVariableWidth $module variableWidth

    set numSimChoice [sim_choice -n $module]
    set i 0
    while { $i < $numSimChoice } {
	simformatdisplay_new $module [sim_choice -i $i $module] \
		display_value variableWidth
	simWriteToSimulationWindow $w2 $module display_value
	incr i
    }
    
    $w2 see end
    $w2 configure -state disabled
}

proc simformatdisplay_new { module state display_value variableWidth } {
    global simVariablesShown
    
    upvar $display_value dv
    upvar $variableWidth vw

    catch {unset dv}
    set i 0

    set numVar [enum_numVar $state]
    set j 0

    while { $j < $numVar } {
	set varName [enum_var $state $j]
  	set varValue [enum_value $state $j]

  	set dv($varName) $varValue
	incr j
    }
    
    # now, add the $columnSeparation for formatting.
    foreach var $simVariablesShown($module) {
	if {![info exists dv($var)]} {
	    set dv($var) " "
	}
	set i [string length $dv($var)]
	set len $vw($var)
	while {$i < $len} {
	    append dv($var) " "
	    incr i
	}
    }

}


proc simVariableWidth {module variableWidth} {
    global simColumnSeparation
    global simVariablesShown

    upvar $variableWidth vw

    catch {unset vw}
    set col_sep [string length $simColumnSeparation]
    foreach var $simVariablesShown($module) {
	set vw($var) [expr [string length $var] + $col_sep]
    }
}

proc simformatdisplay {module in_state display_value variableWidth} {
    global simVariablesShown
    
    upvar $display_value dv
    upvar $variableWidth vw
    upvar $in_state state

    catch {unset dv}
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
    foreach var $simVariablesShown($module) {
	if {![info exists dv($var)]} {
	    set dv($var) " "
	}
	set i [string length $dv($var)]
	set len $vw($var)
	while {$i < $len} {
	    append dv($var) " "
	    incr i
	}
	
    }
}

proc simTraceDisplayHardwareMode {module displayValue} {
    global simVariablesShown
    
    upvar $displayValue dv

    catch {unset display1}
    catch {unset display2}
    
    set display1 ""
    set display2 ""

    set allVariables [join [sim_info $module variableOrdering]]
    foreach var $allVariables {
	if {[lsearch -exact $simVariablesShown($module) $var] != -1} {
#	    append display2 $dv($var)
#	    regsub -all {\$|[a-zA-Z0-9]|'|_} $dv($var) " " tmp
	    append display1 $dv($var)
	}
    }

    return [list $display1 $display2]

}

proc simTraceDisplaySoftwareMode {module displayValue} {
    global simVariablesShown
    
    upvar $displayValue dv

    catch {unset display1}
    catch {unset display2}

}

proc simTraceDisplayStateTransitionMode {module displayValue} {
    global simVariablesShown
    
    upvar $displayValue dv

    catch {unset display1}
    catch {unset display2}

    set display1 ""
    set display2 ""

    set allVariables [join [sim_info $module variableOrdering]]
    foreach var $allVariables {
	if {[lsearch -exact $simVariablesShown($module) $var] != -1} {
	    if {[isEventVariable $module $var] || [isExternalVariable $module $var]} {
		append display1 $dv($var)
		regsub -all {\$|[a-zA-Z0-9]|'|_} $dv($var) " " tmp
		append display2 $tmp
	    } else {
		append display2 $dv($var)
		regsub -all {\$|[a-zA-Z0-9]|'|_} $dv($var) " " tmp
		append display1 $tmp
	    }
	}
    }
    return [list $display1 $display2]
}


proc simWriteHeader {module varshown} {
    global simColumnSeparation
    global simVariablesShown

    set header ""
    set allVariables [join [sim_info $module variableOrdering]]
    foreach var  $allVariables {
	if {[lsearch -exact $varshown $var] != -1} {
	    append header $var $simColumnSeparation
	}
    }

#    set header [join $simVariablesShown($module) $simColumnSeparation]
#    append header $simColumnSeparation

    # write the successor_state window header
    set w .simSimulationWindow:$module 
    set w1 [$w.f.pw subwidget ssp] 
    set w2 [$w1.lf subwidget frame].f1.t1

    # write out the header for the simulation variables.
    $w2 configure -state normal
    $w2 delete 0.0 end
    $w2 insert insert $header
    $w2 configure -state disabled

    # write the simulation_trace window header
    set w .simSimulationWindow:$module 
    set w1 [$w.f.pw subwidget stp] 
    set w2 [$w1.lf subwidget frame].f1.t1
    
    # write out the header for the simulation variables.
    $w2 configure -state normal
    $w2 delete 0.0 end
    $w2 insert insert $header
    $w2 configure -state disabled

}

proc simStartSimulation {module} {
    global simulationMode

    switch -exact $simulationMode($module) {
	random \
	    {simRandomSimulation $module}
	manual \
	    {simGameSimulation $module}
	game \
	    {simGameSimulation $module}
	default \
	    {error "Error in simStartSimulation"} 
    }
}


proc simRandomSimulation {module} {
    global simulationSteps
    global simSimulationTrace

    set w .simSimulationWindow:$module 
    set w1 [$w.f.pw subwidget stp] 
    set w2 [$w1.lf subwidget frame].f1.t2
    
    catch {unset trace}
    simVariableWidth $module variableWidth

    set i 0
    while {$i < $simulationSteps} {
	set ran_num [randomRange [sim_choice -n $module]]
	set state [sim_select $module $ran_num]

	lappend simSimulationTrace($module) $state
	simformatdisplay_new $module $state display_value variableWidth
	simWriteToSimulationWindow $w2 $module display_value
	
	incr i
    }

    $w2 see end

    # update the successor_states window
    simWriteSuccessorStates $module

}

proc simGameSimulation {module} {
    global simSimulationTrace
    global simulationTurn
    global traceDisplayMode

    set tagname state_highlight
    set w .simSimulationWindow:$module 
    set w1 [$w.f.pw subwidget ssp] 
    set w2 [$w1.lf subwidget frame].f1.t2

    set v1 [$w.f.pw subwidget stp] 
    set v2 [$v1.lf subwidget frame].f1.t2
    
    if {[sim_info $module isUserTurn]} {
	# users turn 
	set tmp [lindex [$w2 tag ranges $tagname] 0]
	if {$tmp == ""} {
	    return
	}
	
	regexp {([0-9]+).0} $tmp t line
	if {[string match $traceDisplayMode($module) {state transition}]} {
	    set index [expr $line/2]
	} else {
	    set index [expr $line - 1]
	}
    } else {
	# system's turn
	set index [randomRange [sim_choice -n $module]]
    }

    set state [sim_select $module $index]

    if {[sim_info $module isEndOfRound]} { 
	lappend simSimulationTrace($module) $state
	simVariableWidth $module variableWidth
    
	simformatdisplay_new $module $state display_value variableWidth
	simWriteToSimulationWindow $v2 $module display_value
    
	$v2 see end
    }

#    simWriteSuccessorStates $module

    if {[sim_info $module isUserTurn]} {
	set simulationTurn user
    } else {
	set simulationTurn system
    }
}

proc simWriteToSimulationWindow {window module display_value} {
    global traceDisplayMode
    upvar $display_value dv

    $window configure -state normal	
    switch -exact $traceDisplayMode($module) {
	"hardware" \
	    {
		set tmp [simTraceDisplayHardwareMode $module dv];
		$window insert end [lindex $tmp 0]\n
#		$window insert end [lindex $tmp 1]\n
	    }
	"software" \
	    {$window insert end [simTraceDisplaySoftwareMode $module \
				     [lindex dv 0]]\n}
	"state transition" \
	    {
		set tmp [simTraceDisplayStateTransitionMode $module dv];
		$window insert end [lindex $tmp 0]\n ;
		$window insert end [lindex $tmp 1]\n
	    }
	default \
	    {error "error in simWriteSuccessorStates"}
    }
    $window configure -state disabled
}


# simulation mode selector
proc simSimulationModeSelectWindowFire {module}  {
    global simulationSteps
    global simulationMode
    global simulationTurn
    global simComponents
    global temporaryGlobalVariable

    set leftmargin 10
    set defaultNumSteps 20

    # first, store the original global values
    if {![info exists simulationSteps]} {
	set simulationSteps $defaultNumSteps
    }
    set simulationStepsOld $simulationSteps

    if {![info exists simulationMode($module)]} {
	set simulationMode($module) manual
    }
    set simulationModeOld $simulationMode($module)

    set w .simulationModeSelectWindow
    catch {destroy $w}
    toplevel $w
    
    wm title $w "Simulation Mode Selection"
    wm minsize $w 450 260
    wm maxsize $w 450 260

    set randomModeState disabled
    set gameModeState disabled

    switch -exact $simulationMode($module) {
	random {set randomModeState normal}
	manual {}
	game   {set gameModeState normal}
	default {puts error}
    }

    # draw the grooved frame
    frame $w.lf -relief groove -bd 2
    pack $w.lf -in $w -fill both -expand 1 -padx 40 -pady 10

    set lf $w.lf

    
    # draw the radio buttons

    # 1. manual mode
    frame $lf.mf -relief groove -bd 2
    pack $lf.mf -in $lf \
	-side top \
	-anchor w \
	-fill both -expand 1 -ipadx 5

    radiobutton $lf.mf.manualbutton -variable simulationMode($module) \
	-text Manual -value manual \
	-command \
	"
	    $lf.rf.f.ctl configure -state disabled;
	    $lf.gf.f.t configure -state disabled;
	    $lf.gf.f.b configure -state disabled;
	"

    pack $lf.mf.manualbutton -side top -anchor w


    # 2. random mode
    frame $lf.rf -relief groove -bd 2
    pack $lf.rf -in $lf \
	-side top \
	-anchor w \
	-fill both -expand 1 -ipadx 5

    radiobutton $lf.rf.randombutton -variable simulationMode($module) \
	-text Random -value random \
	-command \
	"
	    $lf.rf.f.ctl configure -state normal;
	    $lf.gf.f.t configure -state disabled;
	    $lf.gf.f.b configure -state disabled;
	"

    pack $lf.rf.randombutton -side top -anchor w

    frame $lf.rf.f 
    pack $lf.rf.f -side top -fill both -expand 1 -padx $leftmargin -pady 5

    label $lf.rf.f.lb -text "number of steps:" \
	-font [lindex [$lf.rf.randombutton configure -font] 3]

    pack $lf.rf.f.lb -side left

    tixControl $lf.rf.f.ctl \
	-labelside left \
	-integer 1 \
	-max 100 \
	-min 1 \
	-value $defaultNumSteps \
	-variable simulationSteps \
	-state $randomModeState

    pack $lf.rf.f.ctl -side left -anchor w


    # 3. game mode
    frame $lf.gf -relief groove -bd 2
    pack $lf.gf -in $lf \
	-side top \
	-anchor w \
	-fill both -expand 1 -ipadx 5

    radiobutton $lf.gf.gamebutton -variable simulationMode($module) \
	-text Game -value game \
	-command \
	"
	    $lf.rf.f.ctl configure -state disabled;
	    $lf.gf.f.b configure -state normal;
	    $lf.gf.f.t configure -state normal;
	"

    pack $lf.gf.gamebutton -side top -anchor w

    frame $lf.gf.f 
    pack $lf.gf.f -side top -fill both -expand 1 -padx $leftmargin -pady 5


    label $lf.gf.f.lb -text "controlling modules:" \
	-font [lindex [$lf.gf.gamebutton configure -font] 3]

    pack $lf.gf.f.lb -side left

    button $lf.gf.f.b -text "browse" \
	-state $gameModeState \
	-font fixed \
	-command "simPopUpComponentModules $module"

    pack $lf.gf.f.b -side right

    entry $lf.gf.f.t -font fixed -state $gameModeState \
	-textvariable simComponents($module)
    pack $lf.gf.f.t -side left -fill x -expand 1

    set temporaryGlobalVariable 0
    # draw the frame containing OK Cancel buttons
    tixButtonBox $w.bbox
       $w.bbox add ok     -text Ok     -width 10
       $w.bbox add cancel -text Cancel -width 10

    # values are already stored globally, and therefore no 
    # further actions are performed when the ok button is pressed.
    $w.bbox subwidget ok configure -command \
	"set temporaryGlobalVariable 1; destroy $w"	
    $w.bbox subwidget cancel configure -command \
	"
	    #restore the old global values
            set temporaryGlobalVariable 0
	    set simulationSteps $simulationStepsOld
	    set simulationMode($module) $simulationModeOld
	    destroy $w
        "
    pack $w.bbox -side top -in $w


    # has to wait for the window to be destroyed

    grab $w
    tkwait window $w
    
    return $temporaryGlobalVariable
}

proc simPopUpComponentModules {module} {
    global simComponents

    set w .simulationComponentModuleWindow
    catch {destroy $w}
    
    toplevel $w

    wm title $w "Select Component Modules"
    set bgcolor #fff9d8
    wm minsize $w 240 330
    wm maxsize $w 240 330

    #
    # start drawing the widgets
    #

    # draw a frame
    frame $w.f -bd 2
    pack $w.f -side top -fill both -expand 1 -padx 20 -pady 5
    
    set w1 $w.f
    # draw a label
    label $w1.lb -text "Select Component Modules:"
    pack $w1.lb -side top -anchor w -ipady 5


    # draw the ScrolledHListbox listing all components
    frame $w1.f2 -bd 2
    pack $w1.f2 -in $w1 -side top -fill both -expand 1

    tixScrolledHList $w1.f2.h1 -scrollbar auto

    set h [$w1.f2.h1 subwidget hlist]
    $h configure -separator "/" -selectmode extended \
	-itemtype text 
    pack $w1.f2.h1 -fill both -expand 1
    
    #"recursively" put the entries in the list box
    set allAtoms [show_atoms $module]
    foreach a $allAtoms {
	set path [split $a "/"]
	catch {unset dummy}
	foreach p $path {
	    if {[info exists dummy]} {
		append dummy "/"  $p
	    } else {
		set dummy $p
	    }

	    if {![$h info exists $dummy]} {
		$h add $dummy -text $p
	    }
	}
    }

    # There seems no way to  highlight the previously selected components
    
    # draw the Ok, Cancel button
    frame $w1.f3 -bd 2
    pack $w1.f3 -in $w1 -side top -fill both

    tixButtonBox $w1.f3.bbox
    pack $w1.f3.bbox -in $w1.f3 -side top
        $w1.f3.bbox add ok   -text Ok -width 8
        $w1.f3.bbox add cancel    -text Cancel -width 8
    $w1.f3.bbox subwidget ok config -command \
	"set simComponents($module) \[$h selection get\]; destroy $w"

    $w1.f3.bbox subwidget cancel  config -command "destroy $w"
    
    grab $w
    tkwait window $w

}

proc simObtainSelectedAtomNames {module} {
    global simComponents
    
    set allAtoms [show_atoms $module]
    set result ""
    
    foreach component $simComponents($module) {
	foreach atom $allAtoms {
	    if {[string match $component* $atom]} {
		lappend result $atom
	    }
	}
    }   

    
    return $result
}

#
# global variables
#

set simColumnSeparation {     }



