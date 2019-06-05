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

# redefine stdIO, since gui is enabled.

proc IntfWriteStdout {msg} {
    .cw_text insert insert $msg {readOnly}
    .cw_text mark set promptEnd {insert - 1c}
    .cw_text see end
    update idletasks
}

proc IntfWriteStderr {msg} {
    .cw_text insert insert $msg {readOnly}
    .cw_text mark set promptEnd {insert - 1c}
    .cw_text see end
    update idletasks
}


