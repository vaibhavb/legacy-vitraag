proc IntfInfoWindowFire {module} {
    

    set w .infoWindow:$module
    set bgcolor gray60
    

    catch {destroy $w}
    toplevel $w -bg $bgcolor
    wm title $w "Module Information: $module"
    wm minsize $w 1 1
    wm geometry $w 570x550

    
    frame $w.menu -relief raised -bd 2
    pack $w.menu -side top -fill x

    # the File menu button
    menubutton $w.menu.file -text "File" -menu $w.menu.file.m 
    menu $w.menu.file.m -tearoff 0
    $w.menu.file.m add command -label "Save" 
    $w.menu.file.m add command -label "Close" \
        -command "destroy $w"

    pack $w.menu.file -side left


    # the Misc button
#    menubutton $w.menu.misc -text "Miscellaneous" -menu $w.menu.misc.m
#    menu $w.menu.misc.m -tearoff 0
#    $w.menu.misc.m add command -label "Simulation" \
#	-command "enum_initSimulation $module"

#    pack $w.menu.misc -side left

    # draw the notebook
    tixNoteBook $w.nb -ipadx 6 -ipady 6 -bd 2
      $w.nb subwidget nbframe configure -backpagecolor $bgcolor

    $w.nb subwidget nbframe

#    $w.nb add general -label "General"
    $w.nb add definition -label "Definitions"
#    $w.nb add invariants -label "Invariants"


    pack $w.nb -fill both -expand 1 -padx 5 -pady 5

#    set general_page [$w.nb subwidget general]
#    set invar_page [$w.nb subwidget invariants]
    set def_page [$w.nb subwidget definition]

#    IntfInfoWindowGeneralPageDraw $general_page
    IntfInfoWindowDefinitionPageDraw $def_page $module
#    IntfInfoWindowInvariantPageDraw $invar_page $module


}

proc IntfInfoWindowGeneralPageDraw {general_page} {

    tixLabelEntry $general_page.le \
            -bd 2 \
            -pady 10 \
            -label Module:
    pack $general_page.le -side top -fill x

    tixPanedWindow $general_page.pw -bd 2 -panerelief ridge 
    set extvar_pane [$general_page.pw add extvar -size 225]
    set intvar_pane [$general_page.pw add intvar -size 225]

    pack $general_page.pw -side top -fill both -expand 1

    tixLabelFrame $extvar_pane.lf -bd 2 -label "External Variables"
    set f [$extvar_pane.lf subwidget frame]
    pack $extvar_pane.lf -fill both -expand 1

    tixScrolledListBox $f.list 
    pack $f.list -fill both -expand 1


    tixLabelFrame $intvar_pane.lf -bd 2 -label "Interface Variables"
    set f [$intvar_pane.lf subwidget frame]
    pack $intvar_pane.lf -fill both -expand 1

    tixScrolledListBox $f.list 
    pack $f.list -fill both -expand 1

}



proc IntfInfoWindowInvariantPageDraw {invar_page module} {

    tixPanedWindow $invar_page.pw -panebd 2 -panerelief ridge 
    set unchecked_pane [$invar_page.pw add unchecked -size 150]
    set passed_pane [$invar_page.pw add passed -size 150]
    set failed_pane [$invar_page.pw add failed -size 100]

    pack $invar_page.pw -side top -fill both -expand 1

    # draw the unchecked frame

    tixLabelFrame $unchecked_pane.lf -bd 2 -label "Unchecked"
    set f [$unchecked_pane.lf subwidget frame]
    pack $unchecked_pane.lf -fill both -expand 1

    tixScrolledListBox $f.list -height 5
    pack $f.list -fill both -expand 1

    tixButtonBox $f.bbox  -relief flat 
      $f.bbox add add    -text Add   -width 8
      $f.bbox add delete -text Delete -width 8
      $f.bbox add check  -text Check -width 8

    pack $f.bbox -side left -fill x

#    tixOptionMenu $f.om -width 20 -label "search mode"
#      $f.om add command enumdfs -label "enumerative (DFS)"
#      $f.om add command enumbfs -label "enumerative (BFS)"
#      $f.om add command symbfs -label  "symbolic (BFS)"

#    pack $f.om -side left 

    # draw the passed frame

    tixLabelFrame $passed_pane.lf -bd 2 -label "Passed"
    set f [$passed_pane.lf subwidget frame]
    pack $passed_pane.lf -fill both -expand 1

    tixScrolledListBox $f.list -height 5
    pack $f.list -fill both -expand 1


    tixButtonBox $f.bbox -relief flat 
      $f.bbox add uncheck   -text Uncheck   -width 8
      $f.bbox add delete    -text Delete -width 8

    pack $f.bbox -side left -fill x


    # draw the failed frame

    tixLabelFrame $failed_pane.lf -bd 2 -label "Failed"
    set f [$failed_pane.lf subwidget frame]
    pack $failed_pane.lf -fill both -expand 1

    tixScrolledListBox $f.list -height 5
    pack $f.list -fill both -expand 1
    
    tixButtonBox $f.bbox  -relief flat 
      $f.bbox add uncheck -text Uncheck   -width 8
      $f.bbox add delete  -text Delete   -width 8

    pack $f.bbox -side left -fill x
	
    # fill in the invariants 
    set passlist [[$passed_pane.lf subwidget frame].list \
		      subwidget listbox]
    set faillist [[$failed_pane.lf subwidget frame].list \
		      subwidget listbox]
    set uncheckedlist [[$unchecked_pane.lf subwidget frame].list \
			   subwidget listbox]

    set all_invariants [show_inv $module]
    set all_invariants [split $all_invariants \n]
    set i 1
    set length [llength $all_invariants]
    while {$i < $length - 1} {
	set inv [lindex $all_invariants $i]
	set status [string trim [lindex $all_invariants [expr $i+1]]]
	set status [string trim [lindex [split $status =] 1]]
	set inv [string trim [lindex [split $inv :] 1]]

	switch -exact $status {
	    passed \
		{ IntfInsertInvariant $inv $passlist}
	    unchecked \
		{ IntfInsertInvariant $inv $uncheckedlist}
	    failed \
		{ IntfInsertInvariant $inv $faillist}

	}
	incr i
	incr i
    }	
}	

proc IntfInsertInvariant {inv lb} {
    $lb insert end $inv
}



proc IntfInfoWindowDefinitionPageDraw {def_page module} {
    set bcolor white
    set fcolor black
    set txtfont fixed

    frame $def_page.f
    
    pack $def_page.f -fill both -expand 1

    text $def_page.f.t -relief sunken -bd 2 \
	-width 60 \
	-height 10 \
	-relief sunken \
	-xscrollcommand "$def_page.xsb set"  \
	-yscrollcommand "$def_page.f.ysb set" \
	-bg $bcolor -fg $fcolor \
	-wrap none \
	-font $txtfont

    scrollbar $def_page.f.ysb -command "$def_page.f.t yview"               
    pack $def_page.f.t -side left -fill both -expand 1
    pack $def_page.f.ysb -side right -fill both

    scrollbar $def_page.xsb \
            -orient horizontal \
            -command "$def_page.f.t xview"

    pack $def_page.xsb -side top -fill both

    tixButtonBox $def_page.bbox 
      $def_page.bbox add simulate -text Execute -width 10 \
	-command  "SimInitSimulatorGUI $module"

    pack $def_page.bbox -side bottom

    # puts the definition in the text box

    set tb $def_page.f.t
    catch {$tb delete 0.0 end}
    $tb insert insert [show_mdls -l $module]

    $tb configure -state disabled
}






