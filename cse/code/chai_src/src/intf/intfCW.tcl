# -------
# Some global variables
# -------
global MochaIcon

catch {unset help_allFunctions}

#set txtfont -*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*
set txtfont fixed
set boldfont -*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*
set boldfont -*-times-bold-r-normal-*-12-*-*-*-*-*-*-*
set bcolor white
set MochaIcon  @$MOCHASRCDIR/intf/coffee.xbm
#set MochaIcon @$MOCHASRCDIR/intf/coffee1.xbm
set tickIcon   @$MOCHASRCDIR/intf/tick.xbm
set crossIcon  @$MOCHASRCDIR/intf/cross.xbm


# ---
# Create the main command window for mocha
# ---

wm title . "mocha"
wm iconname . "mocha"
wm minsize . 1 1
wm iconbitmap . $MochaIcon
wm geometry . 80x24

# -------
# Menu items
# -------

frame .cw_menu -relief raised -bd 2
pack .cw_menu -side top -fill x

# the File menu button
menubutton .cw_menu.file -text "File" -menu .cw_menu.file.m 
menu .cw_menu.file.m -tearoff 0
  .cw_menu.file.m add command -label "Read Module" \
        -command {intfCWReadModule}
  .cw_menu.file.m add command -label "Read Spec" \
        -command {intfCWReadSpec}
  .cw_menu.file.m add command -label "Browser" \
        -command {IntfBrowserWindowFire}
#  .cw_menu.file.m add command -label "Save" \
#        -command {mainSaveFile}
  .cw_menu.file.m add command -label "Exit" \
	-command {exit}

pack .cw_menu.file -side left


# the Help menu button
menubutton .cw_menu.help -text "Help" -menu .cw_menu.help.m
menu .cw_menu.help.m -tearoff 0
  .cw_menu.help.m add command -label "About mocha" \
        -command {IntfHelpAboutMochaWindowFire}
  .cw_menu.help.m add command -label "Commands" \
        -command {IntfHelpWindowFire}
pack .cw_menu.help -side right

#-----
# text window
#-----

text .cw_text -relief sunken -bd 2 -yscrollcommand ".cw_sb set"\
    -setgrid true -bg $bcolor -fg black \
    -font $txtfont \
    -setgrid 1

.cw_text tag configure bold -font $boldfont

scrollbar .cw_sb -command ".cw_text yview"
pack .cw_sb -side right -fill both
pack .cw_text -side left -fill both -expand 1

#----
# text window bindings
#----


bind .cw_text <ButtonRelease-2> {
    # set the insert point to the end.
    .cw_text mark set insert {end - 1c}

    # get the selection, if there is any.
    catch {.cw_text insert end [selection get]}

    # see the last line
    .cw_text see insert
    break
}



bind .cw_text <BackSpace> {

    catch {.cw_text tag remove sel sel.first {promptEnd + 1 c}}

    # if no selection
    if {[.cw_text tag nextrange sel 1.0 end] == ""} {
	if [.cw_text compare insert <= {promptEnd +1 c}] {
	    break
	} 
    }
}

bind .cw_text <Delete> {

    catch {.cw_text tag remove sel sel.first {promptEnd + 1 c}}

    # if no selection
    if {[.cw_text tag nextrange sel 1.0 end] == ""} {
	if [.cw_text compare insert <= {promptEnd +1 c}] {
	    break
	} 
    }

}

bind .cw_text <Control-h> {
    catch {.cw_text tag remove sel sel.first {promptEnd + 1 c}}

    # if no selection
    if {[.cw_text tag nextrange sel 1.0 end] == ""} {
	if [.cw_text compare insert <= {promptEnd +1 c}] {
	    break
	} 
    }
}

bind .cw_text <Control-d> {
    catch {.cw_text tag remove sel sel.first {promptEnd + 1 c}}

    # if no selection
    if {[.cw_text tag nextrange sel 1.0 end] == ""} {
	if [.cw_text compare insert <= {promptEnd +1 c}] {
	    break
	} 
    }
}

bind .cw_text <KeyPress> {
    global MOCHAHISTORYCOUNT

    set MOCHAHISTORYCOUNT 0
    if {[lsearch -exact [.cw_text tag names] readOnly]} {
        .cw_text mark set insert {end - 1c}
    }

    .cw_text see insert
}

bind .cw_text <Return> {
    .cw_text mark set insert {end - 1c}
    .cw_text insert insert \n
    IntfInvoke
    break
}

bind .cw_text <Key-Up> {
    global MOCHAHISTORYCOUNT

    set eventid [expr [history nextid] - 1 - $MOCHAHISTORYCOUNT]
    if {[catch {set cmd [history event $eventid]}]} {
	break;
    }

    # remove everything after the promptEnd
    .cw_text delete {promptEnd + 1 c} end
    .cw_text insert {promptEnd + 1 c} $cmd
    
    incr MOCHAHISTORYCOUNT
    
    break
}

bind .cw_text <Key-Down> {
    global MOCHAHISTORYCOUNT

    # remove everything after the promptEnd
    .cw_text delete {promptEnd + 1 c} end
    if {$MOCHAHISTORYCOUNT == 0} {
	break
    }

    incr MOCHAHISTORYCOUNT -1

    set eventid [expr [history nextid] - $MOCHAHISTORYCOUNT]
    if {[catch {set cmd [history event $eventid]}]} {
	break
    }

    .cw_text insert {promptEnd + 1 c} $cmd
    
    break
}

bind .cw_text <Key-Left> {
    break
}


#----
#all procedures
#----

proc intfCWReadModule {} {
    set d [tix filedialog tixExFileSelectDialog]
    $d subwidget fsbox configure -filetypes {
        {{*}    {All files}}
        {{*.rm} {Mocha files}}
    }
    
    [$d subwidget fsbox] subwidget types configure -value {Mocha files}
    
    $d popup

    $d configure -command {intfCWReadModule1}

}

proc intfCWReadModule1 {filename} {
    catch {read_module $filename} msg
    IntfWriteStdout $msg
}

proc intfCWReadSpec {} {
    set d [tix filedialog tixExFileSelectDialog]
    $d subwidget fsbox configure -filetypes {
        {{*}    {All files}}
	{{*.spec} {Specification files}}
    }
    
    [$d subwidget fsbox] subwidget types configure \
	-value {Specification files}

    $d popup

    $d configure -command {intfCWReadSpec1}

}

proc intfCWReadSpec1 {filename} {
    catch {read_spec $filename} msg
    IntfWriteStdout $msg
}



proc mainModuleBrowserShow {} {}

proc mainSaveFile {} {}

proc mainAboutMocha {} {}

proc IntfInvoke {} {
    
    set cmd [.cw_text get {promptEnd + 1 char} insert ]
    if {[info complete $cmd]} {
	#  now the command becomes read only
	# .cw_text tag add readOnly promptEnd insert
	.cw_text tag add promptEnd insert
	
#  	if {[catch {eval $cmd} msg]} {
#  	    if {[regexp {invalid command name *} $msg]} {
#  		catch {unset tmpcmd}
#  		append tmpcmd "exec " $cmd
#  		catch {eval $tmpcmd} msg
#  	    }
#  	}
	
	catch {eval $cmd} msg

	if {$msg != ""} {
	    IntfWriteStdout $msg\n
	}

	# add the command to the history list
	history add $cmd    
	IntfCWPrompt
    }
    
    .cw_text yview -pickplace insert
    
}


proc IntfCWPrompt {} {
    global MOCHAPROMPT
	
    #get the current position of the cursor (insert)

    set i1 [.cw_text index insert]

    IntfWriteStdout $MOCHAPROMPT

#    .cw_text insert insert "mocha: "
#    .cw_text mark set promptEnd {insert - 1c}
    .cw_text tag add bold  $i1 promptEnd

}

# this exit cleans up all the filedialog widget.
proc exit {} {
    global MOCHATMPDIR	

    set d [tix filedialog tixExFileSelectDialog]
    destroy $d

#    _mocha_end

    catch {exec sh -c "rm -rf $MOCHATMPDIR"}
    puts "Thank you for using MOCHA 1.0\n"

    old_exit
}

# this clear clears the .cw_text
proc clear {} {
    .cw_text delete 0.0 end
#    IntfCWPrompt
}

# redefine greeting
# this procedure is for greeting.
#

proc IntfGreetings {} {
    
    IntfWriteStdout "Welcome to MOCHA 1.0\n"
    IntfWriteStdout "Please report any problems to mocha@eecs.berkeley.edu\n"
    
    IntfCWPrompt
    focus .cw_text
}


proc IntfHelpAboutMochaWindowFire {} {
    global MochaIcon

    set w .aboutMochaWindow
    set txtfont -*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*
    
    catch {destroy $w}
    toplevel $w
    wm title $w "About Mocha"
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
	"Mocha Version 1.0a\n\
            University of California at Berkeley\n\
	http://www-cad.eecs.berkeley.edu/Respep/Research/mocha/index.html" \
	-width 40c \
	-justify center \
	-font $txtfont
    pack $w.f1.msg -side right

    button $w.okbut -text Close -command "destroy $w"
    pack $w.okbut -side top

}







