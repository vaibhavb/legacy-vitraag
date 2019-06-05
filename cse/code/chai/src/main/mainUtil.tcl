#
# utility file
#

# binding of scrollbars

proc BindXView {lists args} {

    foreach l $lists {
	eval {$l xview} $args
    }

}

proc BindYView {lists args} {

    foreach l $lists {
	eval {$l yview} $args
    }

}

# random number generation

proc randomInit seed {
    global rand
    set rand(ia) 9301
    set rand(ic) 49297
    set rand(im) 233280
    set rand(seed) $seed
}

proc random {} {
    global rand
    set rand(seed) [expr ($rand(seed)*$rand(ia) + $rand(ic)) % $rand(im)]
    return [expr $rand(seed) / double ($rand(im))]
}

proc randomRange range {
    expr int ([random]* $range)
}

randomInit [pid]

# documented procedure declaration
proc docproc {name synopsis arguments description formals body} {
    
    global MochaTclCommandSynopsis
    global MochaTclCommandDescription
    
#    set MochaTclCommandSynopsis($name) $synopsis
#    set MochaTclCommandDescription($name) $description

    proc $name $formals	$body

}

