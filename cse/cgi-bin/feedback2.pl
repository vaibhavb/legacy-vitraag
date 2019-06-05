#!/usr/bin/perl

$mailprog= "/usr/ucb/Mail -s " ;
$subject = "SharingAndReflecting ";
$recipient= "vaibhav\@cse.ucsc.edu " ;

%in= &getcgivars ;

open(MAIL, ">msg")
    || &HTMLdie("Couldn't send the mail (couldn't run $mailprog).") ;

$ENV{'HTTP_REFERER'} || ($ENV{'HTTP_REFERER'}= "http://www.cse.ucsc.edu/~vaibhav") ;

$BACK = $ENV{'HTTP_REFERER'};

$maxlength= 0 ;

foreach (keys %in) {
    $maxlength= length if length > $maxlength ;
}

$maxlength++ ;
foreach (sort keys %in) {
    if ($in{$_}=~ /\n/) {
        $in{$_}= "\n" . $in{$_} ;
        $in{$_}=~ s/\n/\n    /g ;
        $in{$_}.= "\n" ;
    }
    $in{$_}=~ s/\0/, /g ;
    printf MAIL "%-${maxlength}s  %s\n", "$_:", $in{$_} ;
}
close(MAIL) ;
#chomp($list = `$mailprog $subject $recipient < msg`);
#chomp($list = `rm -f msg`);

print <<EOF ;
Content-type: text/html

<html>
<body>
<h3>Thank You For Sharing Your Thoughts!</h3>
<p><a href=$BACK>BACK</a><p>
<hr>
<a>Vaibhav</a>
</body>
</html>
EOF

exit ;

sub getcgivars {
    local($in, %in) ;
    local($name, $value) ;

    if ($ENV{'REQUEST_METHOD'} eq 'GET') {
        $in= $ENV{'QUERY_STRING'} ;

    } elsif ($ENV{'REQUEST_METHOD'} eq 'POST') {
        if ($ENV{'CONTENT_TYPE'}=~ 
m#^application/x-www-form-urlencoded$#i) {
            $ENV{'CONTENT_LENGTH'}
                || &HTMLdie("No Content-Length sent with the POST 
request.") ;
            read(STDIN, $in, $ENV{'CONTENT_LENGTH'}) ;

        } else { 
            &HTMLdie("Unsupported Content-Type: $ENV{'CONTENT_TYPE'}") ;
        }

    } else {
        &HTMLdie("Script was called with unsupported REQUEST_METHOD.") ;
    }
    
    foreach (split('&', $in)) {
        s/\+/ /g ;
        ($name, $value)= split('=', $_, 2) ;
        $name=~ s/%(..)/chr(hex($1))/ge ;
        $value=~ s/%(..)/chr(hex($1))/ge ;
        $in{$name}.= "\0" if defined($in{$name}) ;  
        $in{$name}.= $value ;
    }

    return %in ;

}

sub HTMLdie {
    local($msg,$title)= @_ ;
    $title || ($title= "CGI Error") ;
    print <<EOF ;
Content-type: text/html

<html>
<head>
<title>$title</title>
</head>
<body>
<h1>$title</h1>
<h3>$msg</h3>
</body>
</html>
EOF

    exit ;
}














