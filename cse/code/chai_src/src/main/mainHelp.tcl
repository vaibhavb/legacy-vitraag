#
# help implemented in mocha
#


proc help:HelpWindowFire {} {
    global help_filter
    global MOCHASRCDIR
    
    set w .helpWindow
    set bgcolor White

    catch {destroy $w}
    toplevel $w
    wm title $w "mocha help"
    wm minsize $w 1 1
    wm geometry $w 325x380
    wm iconbitmap $w @$MOCHASRCDIR/intf/books.xbm

    #
    # start drawing the widgets
    #

    label $w.lb \
            -bd 2 \
            -text {Functions:}
    pack $w.lb -in $w -side top -anchor w -fill x

    # draw the listbox listing all the commands
    frame $w.f2 -bd 2
    pack $w.f2 -in $w -side left -fill both -expand 1

    tixScrolledListBox $w.f2.lbox \
            -bd 2 \
            -scrollbar auto \
            -relief sunken
    pack $w.f2.lbox -in $w.f2 -side top -fill both -expand 1
    $w.f2.lbox subwidget listbox configure -bg $bgcolor
    
    # draw the filter button and entry box
    frame $w.f2.f1 -bd 2
    pack $w.f2.f1 -in $w.f2 -side bottom -fill x

    entry $w.f2.f1.ebox -relief sunken -bd 2 \
            -textvariable help_filter \
            -bg $bgcolor
    button $w.f2.f1.but -bd 2 -text filter \
            -command {help:listFunctions $help_filter}

    pack $w.f2.f1.ebox -in $w.f2.f1 -side left -fill x -expand 1
    pack $w.f2.f1.but -in $w.f2.f1 -side right

    # draw the issues, detail and refresh buttons
    frame $w.f3 -bd 2
    pack $w.f3 -in $w -side right -fill both

    tixButtonBox $w.f3.bbox -orientation vertical
    pack $w.f3.bbox -in $w.f3 -side right
    #    $w.f3.bbox add issue   -text Issues
        $w.f3.bbox add details -text Details
 #       $w.f3.bbox add refresh -text Refresh
        $w.f3.bbox add close   -text Close
    $w.f3.bbox subwidget close config -command "destroy $w"
    $w.f3.bbox subwidget details config -command "help:commandDetails $w"

#    $w.f3.bbox subwidget refresh config \
#	-command "help:listFunctions $help_filter"
    
    #
    # start initialization of the widget
    #

    help:listFunctions $help_filter

}


proc help:commandDetails {w} {
    global mocha_commands
    
    set select [[$w.f2.lbox subwidget listbox] curselection]
    if { $select == "" } {
	return 
    }

    set cmd [selection get]
    
    # draw the .helpCommandDescriptionWindow
    set w .helpCommandDescriptionWindow
    
    if {![winfo exists $w]} {
	toplevel $w

	wm title $w "Command Description: $cmd"
	
	frame $w.f -bd 2
	pack $w.f -fill both -expand 1
	
	frame $w.f.f -bd 2
	pack $w.f.f -fill both -expand 1
	
	text $w.f.f.t -bd 2 -relief sunken \
	    -yscrollcommand "$w.f.f.vsb set" \
	    -xscrollcommand "$w.f.hsb set"
	pack $w.f.f.t -side left -fill both -expand 1
	
	scrollbar $w.f.f.vsb -command "$w.f.f.t yview"
	pack $w.f.f.vsb -side right -fill both -expand 1
	
	scrollbar $w.f.hsb -command "$w.f.f.t xview" -orient horizontal
	pack $w.f.hsb -fill both
	
	button $w.b -bd 2 -text Close -command "destroy $w"
	pack $w.b -pady 5
    }
    
    $w.f.f.t configure -state normal
    $w.f.f.t delete 0.0 end

    # reads in the file describing the function
    # and puts it in the text box
    
    if {[catch {set fd [open $mocha_commands($cmd)]}]} {
	$w.f.f.t insert insert "No help available for $cmd\n"
	return 
    }

    while {[gets $fd line] >= 0} {
	$w.f.f.t insert insert $line\n
    }
    close $fd

    $w.f.f.t configure -state disabled

}


proc help:addUserFunctions {fun_name fun_help} {
    global mocha_commands

    catch {unset mocha_commands($fun_name)}
    set mocha_commands($fun_name) $fun_help
}

proc help:listFunctions {filter} {
    global mocha_commands

    set w .helpWindow
    set lbox [$w.f2.lbox subwidget listbox]
    set allfunctions [lsort [array names mocha_commands]]
    
    $lbox delete 0 end
	
    foreach f $allfunctions {
        if {[string match $filter $f]} {
            $lbox insert end $f
        }
    }
}

proc help:readPackageHelpFiles {} {
    global mocha_commands
    global MOCHAHELPDIR

    set allcmdfiles [glob $MOCHAHELPDIR/*]
    
    foreach cmdfile $allcmdfiles {
	set filename [file tail $cmdfile]
	regexp {([^.]+)Cmd\.txt} $filename t cmd
	set mocha_commands($cmd) $cmdfile
    }
}



#
# global initialization
#


set help_filter *
help:readPackageHelpFiles



