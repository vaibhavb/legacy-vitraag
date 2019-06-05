#!/usr/bin/perl

$file = './procrypt/progress';
$out = './procrypt/progress.html';
open (INFO, "<$file");
open (OUT, ">$out");
@lines = <INFO>;
close(INFO);

$old_handle = select(OUT);

print "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">
<html>
  <head>
    <title>Progress Pointers</title>
  </head>

  <body>
    <h1>Progress Pointers</h1>
    <ol>";

foreach $i (@lines)
	{
		if ($i =~ s/^###// )
		{	
		print "<p><li>";
		};

	 print "$i";
	}

print "</ol> <hr>
    <address><a href=\"mailto:vaibhav\@cse.ucsc.edu\">Vaibhav 
Bhandari</a></address>
<!-- Created: Thu Nov 15 21:49:44 PST 2001 -->
<!-- hhmts start -->
Last modified: Thu Nov 15 21:50:07 PST 2001
<!-- hhmts end -->
  </body>
</html>";


select($old_handle);
#@args "chmod o+r ./procrypt/progress.html";
#exec @args;
print "Zhala re..\n";


