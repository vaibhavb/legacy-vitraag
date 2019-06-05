#!/usr/bin/perl

##############################--Mail Variables--############################
$HOME_URL_SOE = "http://www.soe.ucsc.edu/~vaibhav";
$HOME_URL_CSE = "http://www.cse.ucsc.edu/~vaibhav";
$HOME_DIR = "/cse/grads/vaibhav/.html";
$mailprog= "/usr/ucb/Mail -s " ;
$subject = "SharingAndReflecting ";
$recipient= "vaibhav\@cse.ucsc.edu " ;

############################--Form Format--##################################



#################################--MAIN--####################################
%in= &getcgivars ;

open(MAIL, ">counter")
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
open(INMAIL, "<msg");
@wht = <INMAIL>;
close(INMAIL);
if ($in{"contact-action"} eq "add") {&CONTACT_ADD;}


###############################--SUNROUTINES--###########################
#Get the cgi varibles in a associtive array "in" from the environment
sub getcgivars {
    local($in, %in) ;
    local($name, $value) ;

    if ($ENV{'REQUEST_METHOD'} eq 'GET') {
        $in= $ENV{'QUERY_STRING'} ;

    } elsif ($ENV{'REQUEST_METHOD'} eq 'POST') {
        if ($ENV{'CONTENT_TYPE'}=~ m#^application/x-www-form-urlencoded$#i) {
            $ENV{'CONTENT_LENGTH'}
                || &HTMLdie("No Content-Length sent with the POST request.") ;
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

#add contact
sub CONTACT_ADD{
   $tagName = "add-here";
   $file = $in{"contact-file"};
   open(target, "<$file") || &HTMLdie("Couldn't open $file\n");
   open(outpot, ">$file.tmp") || &HTMLdie("Couldn't open $file\n");
   $first = 0; 
   @blurb= <target>;
   foreach $b (@blurb){
        if ($b =~ /<!--contact-$tagName-->/){
	   chomp($name = $in{"contact-name"});
	   chomp($email = $in{"contact-email"});
	   chomp($home = $in{"contact-home"});
	   chomp($office = $in{"contact-office"});
	   chomp($bday = $in{"contact-bday"});
	   chomp($type = $in{"contact-type"});
	   $contact = qq!
<tr><td>$name</td><td>$email</td><td>$home</td><td>$office</td><td>$bday</td><td>$type</td></tr>
!;
	   $b = $contact.$b;
       }
       print outpot $b;
   }
   close(target);
   close(outpot);
   $ABS_URL = $file;
   $ABS_URL =~ s/$HOME_DIR/$HOME_URL_CSE/;
   `mv $file.tmp $file; chmod 755 $file`;
   print <<EOF;
Content-type: text/html

<html>
<head>
<meta http-equiv="refresh" content="1; url=$ABS_URL">
<title>Viked</title>
</head>
<body>U R Conned: ..$in{"contact-file"}</body>
</html>
EOF
exit ;
}


















