
# other procedures

proc IntfBrowserWindowFire {} {
    global browser_filter

    set w .browserWindow
    set bgcolor #fff9d8

    catch {destroy $w}
    toplevel $w
    wm title $w "Browser"
    wm minsize $w 1 1
    wm geometry $w 340x350

    #
    # start drawing the widgets
    #

    # draw the listbox listing all the modules
    frame $w.f2 -bd 2
    pack $w.f2 -in $w -side left -fill both -expand 1

    tixComboBox $w.f2.cbox -bd 2 \
            -label "browse:"
      $w.f2.cbox add Modules
#      $w.f2.cbox add Atoms
    
    $w.f2.cbox configure -value Modules
    
    pack $w.f2.cbox -anchor w


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
            -textvariable browser_filter \
            -bg $bgcolor
    button $w.f2.f1.but -bd 2 -text filter \
            -command {IntfBrowserUpdatebrowser $browser_filter}

    pack $w.f2.f1.ebox -in $w.f2.f1 -side left -fill x -expand 1
    pack $w.f2.f1.but -in $w.f2.f1 -side right

    # draw the Open, New and Refresh and Close buttons
    frame $w.f3 -bd 2
    pack $w.f3 -in $w -side right -fill both

    tixButtonBox $w.f3.bbox -orientation vertical
    pack $w.f3.bbox -in $w.f3 -side right
        $w.f3.bbox add open   -text Open 
#        $w.f3.bbox add new    -text New
        $w.f3.bbox add execute  -text Execute
        $w.f3.bbox add refresh -text Refresh
#	$w.f3.bbox add delete  -text Delete
        $w.f3.bbox add close   -text Close
    $w.f3.bbox subwidget open config -command IntfBrowserOpenModule
    $w.f3.bbox subwidget execute config -command IntfExecuteModule
#    $w.f3.bbox subwidget new  config -command "IntfNewModuleWindowFire"
    $w.f3.bbox subwidget refresh  config \
	-command "IntfBrowserUpdatebrowser $browser_filter"
#    $w.f3.bbox subwidget delete config -command "IntfBrowserDeleteModule $w"
    $w.f3.bbox subwidget close config -command "destroy $w"
    
    # some bindings
    set eb $w.f2.f1.ebox
    bind $eb <Return> {IntfBrowserUpdatebrowser $browser_filter}

    # list all modules when start up
    IntfBrowserUpdatebrowser $browser_filter

}

proc IntfBrowserDeleteModule {w} {
    global browser_filter

    set select [[$w.f2.lbox subwidget listbox] curselection]
    if {$select != "" } {
	delete [selection get]
    }

    IntfBrowserUpdatebrowser $browser_filter
}


proc IntfBrowserUpdatebrowser {filter} {
    
    set w .browserWindow
    
    set lb [$w.f2.lbox subwidget listbox]

    set allObjects [show_mdls]

    $lb delete 0 end

    foreach i [lsort $allObjects] {
        if {[string match $filter $i]} {
            $lb insert end $i
        }
    }
}

proc IntfBrowserOpenModule {} {
    set w .browserWindow
    set lb [$w.f2.lbox subwidget listbox]
    
    if {[$lb curselection] != ""} {
	IntfInfoWindowFire [selection get]
    }
}

proc IntfExecuteModule {} {
    set w .browserWindow
    set lb [$w.f2.lbox subwidget listbox]
    
    if {[$lb curselection] != ""} {
	SimInitSimulatorGUI [selection get]
    }
}

global browser_filter

set browser_filter *


