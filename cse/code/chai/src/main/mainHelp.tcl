#
# help implemented in mocha
#

proc help:readPackageHelpFiles {} {
    global mocha_commands
    global CHAIHELPDIR

    set allcmdfiles [glob $CHAIHELPDIR/*]
    
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



