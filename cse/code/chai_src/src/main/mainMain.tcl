source $MOCHASRCDIR/main/mainUtil.tcl
if {[info exists MOCHAHELPDIR]} {
    source $MOCHASRCDIR/main/mainHelp.tcl
} else {
    # no help is avialable
    proc help:HelpWindowFire {} {
	global MochaIcon
	set w .helpWindow
	set txtfont -*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*
	
	catch {destroy $w}
	toplevel $w
	wm title $w "Mocha Help"
	wm geometry $w +400+300
	
	# draw frame 1
	frame $w.f1
	pack $w.f1 -side top -fill both -expand 1
	
	label $w.f1.lb
	set img [image create compound -window $w.f1.lb]
	$img add line
	$img add bitmap -bitmap $MochaIcon
	$w.f1.lb config -image $img
	pack $w.f1.lb -side left
	
	message $w.f1.msg -bd 2\
	    -text \
	    "No help available.\n" \
	    -width 40c \
	    -justify center \
	    -font $txtfont
	pack $w.f1.msg -side right
	
	button $w.okbut -text ok -command "destroy $w"
	pack $w.okbut -side top
	
    }
}



