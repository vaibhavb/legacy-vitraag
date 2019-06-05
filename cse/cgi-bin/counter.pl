#!/usr/bin/perl

($i = `cat guestcnt.cnt` + 1); 
( `echo $i > guestcnt.cnt `); 
print <<EOF ;
Content-type: text/html

<html><head><title>Puja</title></head><body>
<p>Your are person number $i coming for the puja.
<b>Thanks</b>
</body></html>
EOF
exit;



#In case something in CGI mechanism fails.
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



