#!/usr/local/bin/perl

require "cgi-lib.pl";

MAIN:
{

  # Read in all the variables set by the form
    &ReadParse(*input);

  # Print the header
    print &PrintHeader;
    print &HtmlTop ("cgi-lib.pl demo form output");

  # Do some processing, and print some output
    ($text = $input{'text'}) =~ s/\n/\n<BR>/g; 
                                   # add <BR>'s after carriage returns
                                   # to multline input, since HTML does not
                                   # preserve line breaks

    print <<ENDOFTEXT;

    You, $input{'name'}, whose favorite color is $input{'color'} are on a
	quest which is $input{'quest'}, and are looking for the weight of an
	    $input{'swallow'} swallow.  And this is what you have to say for
yourself:<P> $text<P>

ENDOFTEXT


  # If you want, just print out a list of all of the variables.
    print "<HR>And here is a list of the variables you entered...<P>";
    print &PrintVariables(*input);

  # Close the document cleanly.
    print &HtmlBot;
}




