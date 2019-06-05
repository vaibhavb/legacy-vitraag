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
open(INMAIL, "<msg");
@wht = <INMAIL>;
close(INMAIL);
if ($in{"viki-action"} eq "edit") {&VIKI_EDIT;}
if ($in{"viki-action"} eq "commit") {&VIKI_COMMIT;}

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

#edit the viki
sub VIKI_EDIT{
    $tagName = $in{"viki-name"};
    if ($in{"viki-file"}) {
	$file = $in{"viki-file"}
    }
    else{ 
	$file = &REL_PATH;
    }
    open(target, "$file") || &HTMLdie("Couldn't open $file\n");
    $first = 0; $viki= "";
    @blurb= <target>;
    foreach $b (@blurb){
	if ($b =~ /<!--viki-begin-$tagName-->/){
	    $first = 1; $b="";
	}
	if ($b =~ /<!--viki-end-$tagName-->/ ){
	    $b = $banner;
	    $first = 2;
	}
	if ($first == 1 ){
	    $viki .= $b;
	}
	
    }
    close(target);

#the way OUTPUT should look.
print <<EOF;
Content-type: text/html

<html>
<body>
<p> hey dude $in{"viki-action"} .. $in{"viki-name"}
<p><a href=$BACK>BACK</a></p>
<form ACTION="viki.pl" method="post">
<input TYPE=HIDDEN NAME="viki-name" VALUE=$in{"viki-name"}>
<input TYPE=HIDDEN NAME="viki-action" VALUE="commit">
<input TYPE=HIDDEN NAME="viki-file" VALUE=$file>
<textarea ROWS=5 COLS=70 NAME="viki-input">$viki</textarea><p align=left>
<input TYPE="SUBMIT" VALUE="Share Your Opinion">
<input TYPE="RESET"></p>
</form>
<hr>
<a>Vaibhav</a>
</body>
</html>
EOF
exit ;
}

#commit the viki
sub VIKI_COMMIT{
   $date = `date`;
   $who = `which cvs`;
#`/usr/local/gnu/bin/cvs -m "Date" commit $in{"viki-file"}` || &HTMLdie("CVS Error\n");

   chomp($tagName = $in{"viki-name"});
   $file = $in{"viki-file"};
   open(target, "<$file") || &HTMLdie("Couldn't open $file\n");
   open(outpot, ">$file.tmp") || &HTMLdie("Couldn't open $file\n");
   $first = 0; 
   @blurb= <target>;
   foreach $b (@blurb){
       if ($b =~ /<!--viki-begin-$tagName-->/){
	   $first = 1;
	   print outpot $b;
       }
       if ($b =~ /<!--viki-end-$tagName-->/){
	   $first = 2;
	   chomp($viki = $in{"viki-input"});
	   $viki =~ s/^\s+//mg; #trim leading white-space
	   $b = $viki.$b;
       }
       if ($first == 1 ){
	   $b = "";
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
<body>U R Viked: ..$in{"viki-input"}</body>
</html>
EOF
exit ;
}



#find the relative path
sub REL_PATH{
    $URL = $HOME_URL_SOE;
    $DIR = $HOME_DIR;
    $ABS_URL= $BACK;
    $ABS_URL =~ s/$URL|$HOME_URL_CSE/$DIR/;
    return $ABS_URL;
}


















