# draw the new module window

proc IntfNewModuleWindowFire {} {
    
    global mdlcounter
    global MOCHATMPDIR

    set w .newModuleWindow$mdlcounter
    
    catch {destroy $w}
    toplevel $w 
    wm title $w "New Module"

    
    # -------
    # Menu items
    # -------
    
    frame $w.menu -relief raised -bd 2
    pack  $w.menu -side top -fill x

    # the File menu button
    menubutton $w.menu.file -text "File" -menu $w.menu.file.m 
    menu $w.menu.file.m -tearoff 0
    $w.menu.file.m add command -label "Open" \
            -command "intfLoadFile $w.f.t"
    $w.menu.file.m add command -label "New" \
            -command {IntfNewModuleWindowFire}
    $w.menu.file.m add command -label "Save" \
            -command "intfSaveFile $w.f.t"
    $w.menu.file.m add command -label "Close" \
            -command "destroy $w"

    pack $w.menu.file -side left

    # the Edit button

    menubutton $w.menu.edit -text "Edit" -menu $w.menu.edit.m
    menu $w.menu.edit.m -tearoff 0
#    $w.menu.edit.m add command -label "Copy" 
#    $w.menu.edit.m add command -label "Cut" 
#    $w.menu.edit.m add command -label "Paste" 
#    $w.menu.edit.m add command -label "Clear All" 
#    $w.menu.edit.m add separator
    $w.menu.edit.m add command -label "Open in Editor" \
	-command "intfOpenEditor; intfCopyToTextWindow $w.f.t"

    pack $w.menu.edit -side left

    # draw "enter definition" label:
    label $w.lb -bd 2 -text "Enter Definition:"
    pack $w.lb -anchor w -pady 2 -padx 2

    # draw text frame

    frame $w.f
    pack $w.f -fill both -expand 1
    
    text $w.f.t -relief sunken -bd 2 \
            -xscrollcommand "$w.xsb set"\
            -yscrollcommand "$w.f.ysb set"\
            -setgrid true 
    
    scrollbar $w.f.ysb -command "$w.f.t yview"
    pack $w.f.ysb -side right -fill both
    pack $w.f.t -side left -fill both -expand 1

    scrollbar $w.xsb -orient horizontal -command "$w.f.t xview"
    pack $w.xsb -fill x

    set t $w.f.t

    # draw buttons for different module compositions

    tixButtonBox $w.bbox1 -bd 2
      $w.bbox1 add parcomp -text "Parallel Composition"
      $w.bbox1 add hidevar -text "Hide Variables"
      $w.bbox1 add renvar  -text "Rename Variables"
    $w.bbox1 subwidget parcomp configure \
            -command "IntfParallelCompositionWindowFire $w"
    $w.bbox1 subwidget hidevar configure \
            -command "IntfHideVariableWindowFire $w"
    $w.bbox1 subwidget renvar configure \
            -command "IntfRenameVariableWindowFire $w"

    pack $w.bbox1 -pady 5

    # draw buttons "Apply, Cancel"

    tixButtonBox $w.bbox2 -bd 2
    $w.bbox2 add ok -text "Ok" -width 8
    $w.bbox2 add cancel -text "Cancel" -command "destroy $w" -width 8

    $w.bbox2 subwidget ok configure \
	-command "IntfNewModuleOk $w"

    pack $w.bbox2 
    
#    # insert default module name:
#    $t insert end "module new_module$mdlcounter\n\n"
#    $t insert end "endmodule"
    
    # increase mdlcounter
    incr mdlcounter

}

proc intfOpenEditor {} {
    global MOCHATMPDIR
    global env

    set editor $env(EDITOR)
    
    if {$editor == ""} {
	set editor vi
    } 
    
    catch {file delete $MOCHATMPDIR/mochatmp.rm}

    exec $editor $MOCHATMPDIR/mochatmp.rm
    
}

proc intfCopyToTextWindow {w} {
    global MOCHATMPDIR
    
    if {[catch {set fr [open $MOCHATMPDIR/mochatmp.rm "r"]}]} {
	return
    } 
    
    $w delete 0.0 end

    while {![eof $fr]} {
	set line [gets $fr]
	$w insert end $line\n
    }

    close $fr
}

proc intfSaveFile {w} {
    
    set d [tix filedialog tixExFileSelectDialog]
    $d subwidget fsbox configure -filetypes {
        {{*}    {All files}}
        {{*.rm} {Mocha files}}
        {{*.txt}        {Text files}}
        {{*.tcl *.tk}   {Tcl/Tk files}}
    }
    
    $d popup
    
    $d configure -command "intfSaveFile1 $w"
}

proc intfSaveFile1 {w filename} {
 
    if {[catch {set fp [open $filename w]}]} {
	tkerror [concat Cannot to write $tmpMochaFile]
    } else {
	puts $fp [$w get 0.0 end]
	close $fp
	IntfWriteStdout "$filename saved"
    }
}

proc intfLoadFile {w} {
    set d [tix filedialog tixExFileSelectDialog]
    $d subwidget fsbox configure -filetypes {
        {{*}    {All files}}
        {{*.rm} {Mocha files}}
        {{*.txt}        {Text files}}
        {{*.tcl *.tk}   {Tcl/Tk files}}
    }
    
    $d popup

    $d configure -command "intfLoadFile1 $w"

}

proc intfLoadFile1 {w filename} {
    
    if {[catch {set fr [open $filename "r"]}]} {
	error [concat cannot read $filename]
    } 
    
    $w delete 0.0 end

    while {![eof $fr]} {
	set line [gets $fr]
	$w insert end $line\n
    }

    close $fr
}


proc IntfNewModuleOk {w} {
    global MOCHATMPDIR
    set newdef [$w.f.t get 0.0 end]
    set tmpMochaFile $MOCHATMPDIR/mochatmp.rm
    
    set trimmednewdef [string trim $newdef]
    if {$trimmednewdef != ""} {
	if {[catch {set fp [open $tmpMochaFile w]}] != 0} {
	    tkerror [concat [Error: unable to open $tmpMochaFile to write module definitions.]]
	} else {
	    puts $fp $trimmednewdef
	    close $fp
	    read_module $tmpMochaFile
	}
    }

    catch {destroy $w}

}


# draw the parallel composition window
proc IntfParallelCompositionWindowFire {pw} {

    set w .parallelCompWindow

    toplevel $w
    wm title $w "Parallel Composition"
    wm geometry $w 250x350

    label $w.lb -text "Choose Modules:" 
    pack $w.lb -padx 2 -anchor w

    tixScrolledListBox $w.slb -bd 2
    set slb [$w.slb subwidget listbox]
    $slb configure -selectmode multiple
    pack $w.slb -padx 5 -fill both -expand 1
    

    tixButtonBox $w.bb -bd 2 
      $w.bb add ok -text OK -width 7
      $w.bb add cancel -text Cancel -width 7 -command "destroy $w"

    $w.bb subwidget ok configure \
	-command "IntfParallelCompositionOk $w $pw"
    
    pack $w.bb -pady 5

    # insert all the available modules into the listbox
    catch {$slb delete 0 end}
    foreach mdl [lsort [show_mdls]] {
	$slb insert end $mdl
    }

    # has to wait for the parallel composition window to be destroyed
    grab $w
    tkwait window $w

}

proc IntfParallelCompositionOk {w pw} {
    global mdlcounter

    set slb [$w.slb subwidget listbox]
    set select [$slb curselection]

    if {$select != ""} {
	set mdlselected [selection get]
	$pw.f.t insert end \n\n
	$pw.f.t insert end [concat new_module$mdlcounter := [join $mdlselected " || "] \n]
	incr mdlcounter
    }

    catch {destroy $w}
}


# draw the "Hide variable" window
proc IntfHideVariableWindowFire {pw} {
    global tmpmdl

    set w .hideVariableWindow

    toplevel $w
    wm title $w "Hide Variable"
    wm geometry $w 250x350

    tixComboBox $w.cbox -bd 2 \
	-label "Module: " \
	-variable tmpmdl \
	-command "IntfHideListVariables $w $tmpmdl "
    
    pack $w.cbox -anchor w

    tixScrolledListBox $w.slb -bd 2
    set slb [$w.slb subwidget listbox]
    $slb configure -selectmode multiple
    pack $w.slb -padx 5 -fill both -expand 1

    tixButtonBox $w.bb -bd 2 
      $w.bb add ok -text OK -width 7 -command "IntfHideVariableOk $w $pw"
      $w.bb add cancel -text Cancel -width 7 -command "destroy $w"

    pack $w.bb -pady 5

    # fill the combo box with the modules
    set mdls [show_mdls]
    foreach m $mdls {
	$w.cbox insert end $m
    }
    
    grab $w
    tkwait window $w

}

proc IntfHideListVariables {w mdl} {
    set lb [$w.slb subwidget listbox]
    
    if {$mdl != ""} {
	foreach var [show_vars -vALL $mdl] {
	    if {[isInterfaceVariable $mdl $var]} {
		$lb insert end $var
	    }
	}
    }
}

proc IntfHideVariableOk {w pw} {
    global mdlcounter
    global tmpmdl

    set slb [$w.slb subwidget listbox]
    set select [$slb curselection]

    if {$select != ""} {
	set varselected [selection get]
	$pw.f.t insert end \n\n
	$pw.f.t insert end [concat new_module$mdlcounter := ]
	$pw.f.t insert end [concat hide [join $varselected " , "] in]
	$pw.f.t insert end [concat $tmpmdl endhide\n]
	incr mdlcounter
    }

    catch {destroy $w}
    
}

# draw the rename variable window

proc IntfRenameVariableWindowFire {pw} {
    global tmpmdl
    global newVariableName

    set w .renameVariableWindow

    toplevel $w
    wm title $w "Rename Variable"
    wm geometry $w 250x350

    tixComboBox $w.cbox -bd 2 \
	-label "Module: " \
	-variable tmpmdl \
	-command "IntfRenameListVariables $w $tmpmdl "
    
    pack $w.cbox -anchor w

    tixScrolledListBox $w.slb -bd 2
    set slb [$w.slb subwidget listbox]
    pack $w.slb -padx 5 -fill both -expand 1

    tixLabelEntry $w.le -bd 2 -labelside top \
	-label "Variable renamed to:"
    pack $w.le -fill x  -pady 3
    $w.le subwidget entry configure -textvariable newVariableName
    
    tixButtonBox $w.bb -bd 2 
      $w.bb add ok -text OK -width 7 -command "IntfRenameVariableOk $w $pw"
      $w.bb add cancel -text Cancel -width 7 -command "destroy $w"

    pack $w.bb -pady 5

    # fill the combo box with the modules
    set mdls [show_mdls]
    foreach m $mdls {
	$w.cbox insert end $m
    }
    
    grab $w
    tkwait window $w

}

proc IntfRenameListVariables {w mdl} {
    set lb [$w.slb subwidget listbox]
    
    if {$mdl != "" } {
	foreach var [show_vars -vALL $mdl] {
	    if {[isInterfaceVariable $mdl $var]} {
		$lb insert end $var
	    }
	}
    }
}

proc IntfRenameVariableOk {w pw} {
    global mdlcounter
    global tmpmdl
    global newVariableName

    set slb [$w.slb subwidget listbox]
    set select [$slb curselection]

    if {$select != ""} {
	set varselected [selection get]
	$pw.f.t insert end \n\n
	$pw.f.t insert end [concat new_module$mdlcounter := $tmpmdl]
	$pw.f.t insert end [concat \[$varselected := $newVariableName\]]
	incr mdlcounter
    }

    catch {destroy $w}
    
}

global mdlcounter
global tmpmdl

set mdlcounter 0
set tmpmdl ""


