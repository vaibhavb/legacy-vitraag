#
# Initialization of the intf package
#


proc IntfInit {} {
    global auto_noexec
    
    # set to disable all unix commands
    # set auto_noexec 1


    # initiate all common routines
    IntfInitCommon

    # print greetings
    IntfGreetings

}

proc IntfInitCommon {} {
    global help
    global help_filter
    global CHAISRCDIR
    global CHAITMPDIR
    global MOCHAHISTORYCOUNT

    source $CHAISRCDIR/intf/intfCommon.tcl

    randomInit [pid]
    
    # set some global variables
    set help_filter *
    
    # read the help documentation
    IntfHelpReadPackageHelpFiles
	
    # set the prompt appropriately
    IntfSetPrompt {chai 1.0 > }

    # set the history counter
    set MOCHAHISTORYCOUNT 0

    # create the mocha tmp directory
    if {![file isdirectory $CHAITMPDIR]} {
	if {[catch {exec mkdir $CHAITMPDIR}]} {
	    puts [concat Error: Cannot create $CHAITMPDIR.]
	    exit
	}
    }
	
}

# Intiatialization of the whole package
IntfInit










