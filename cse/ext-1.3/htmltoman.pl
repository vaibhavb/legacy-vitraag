#! @PERL@

###PerlFile#################################################
#
# FileName	[ htmltoman ]
# PackageName	[ ext ]
# Synopsis	[ Convert HTML files to manual pages in troff -man format ]
# Description	[ A simple text substitution ]
# Author	[ Stephen Edwards <sedwards@eecs.berkeley.edu> ]
# Revision	[ $Header: /home/sedwards/berkeley/projects/ext/RCS/htmltoman.pl,v 1.2 1996/01/15 01:03:17 sedwards Exp $]
#
############################################################

while ( <> ) {

    s!^\s+!!;

    s!<html>!.\\\"\n.\\\" Generated automatically by htmltoman.pl\n.\\\"!;

    s!<head><title>([^<]*)</title></head>!.TH \1 1!;

    s!<body>!!;
    s!</body>!!;
    s!</html>!!;

    s!<a[^<]*>!!g;
    s!</a>!!g;

    s!<h2>([^<]*)</h2>!.SH \1!;

    s!<dl>!!;
    s!</dl>!!;

    s!<dt>(.*)$!.TP \1.br\n.ns\n!;
    s!<dd>\s*!!;

    s!<br>!\n.br\n!g;

    s!-!\\-!g;
    
    s!<tt>([^<]*)</tt>\s*!\n.B \1\n!g;

    s!<b>([^<]*)</b>\s*!\n.B \1\n!g;

    s!<i>([^<]*)</i>\s*!\n.I \1\n!g;

    s!<p>!!g;

    s!<blockquote>!.RS\n.sp .5\n.nf\n!;
    s!</blockquote>!.fi\n.RE\n!;

    s!<table>!!;
    s!<tr>!!;
    s!<td>!!g;
    s!</td>!!g;
    s!</tr>!.br!;
    s!</table>!!;

    s!^\n!!;
    s!\n\n!\n!g;

    s!&lt;!<!g;
    s!&gt;!>!g;

    print "$_" unless /^$/;
}


# Local Variables:
# mode: perl
# End: