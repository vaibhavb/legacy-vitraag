#
# Initialization of the intf package
#


proc IntfInit {} {
    global MOCHATKENABLED
    global auto_noexec
    
    # set to disable all unix commands
 	# set auto_noexec 1


    # initiate all common routines
    IntfInitCommon

    # initiate the gui related routines
    if {$MOCHATKENABLED} {
	IntfInitGUI
    }

    # print greetings
    IntfGreetings

}

proc IntfInitCommon {} {
    global help
    global help_filter
    global MOCHASRCDIR
    global MOCHATMPDIR
    global MOCHAHISTORYCOUNT

    source $MOCHASRCDIR/intf/intfCommon.tcl

    randomInit [pid]
    
    # set some global variables
    set help_filter *
    
    # read the help documentation
    IntfHelpReadPackageHelpFiles
	
    # set the prompt appropriately
    # /*vaibhav*/
    # IntfSetPrompt {mocha: }
    IntfSetPrompt {mocha > }
    # /*vaibhav*/

    # set the history counter
    set MOCHAHISTORYCOUNT 0

    # create the mocha tmp directory
    if {![file isdirectory $MOCHATMPDIR]} {
	if {[catch {exec mkdir $MOCHATMPDIR}]} {
	    puts [concat Error: Cannot create $MOCHATMPDIR.]
	    exit
	}
    }
	
}

proc IntfInitGUI {} {
    global MOCHASRCDIR
    
    # sources all the gui-related intf files.
    # note, do not change the order.
    set intfdir $MOCHASRCDIR/intf
    
    source $intfdir/intfUtil.tcl
    source $intfdir/intfHelp.tcl
    source $intfdir/intfBrowser.tcl
    source $intfdir/intfModuleInfo.tcl
    source $intfdir/intfNewModule.tcl
    source $intfdir/intfCW.tcl

}

# Intiatialization of the whole package

IntfInit










