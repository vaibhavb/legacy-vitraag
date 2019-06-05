proc mdl:InvariantCheckWindowFire {} {
    set w .invariantCheckWindow

    catch {destroy $w}
    toplevel $w

    wm title $w {Invariant Check}
    

    # draw the search mode frame
    frame $w.f1
    pack $w.f1 -fill x

    


    pack $w.f1.se
}
