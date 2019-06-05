#! @PERL@

###PerlFile###########################################################
#
#  FileName	[ extdoc ]
#  PackageName	[ ext ]
#  Synopsis	[ Source code documentation extractor ]
#  Description	[ Uses a structured comment format to locate and
#		  extract documentation and declaration information from
#		  C source ]
#
#  Author	[ Stephen Edwards <sedwards@eecs.berkeley.edu> ]
#  Revision	[ $Header: /home/sedwards/berkeley/projects/ext/RCS/extdoc.pl,v 1.30 1996/02/28 22:53:44 sedwards Exp $]
#
######################################################################

#
# List of the known comment types
#

%knownCommentTypes = ("CFile", 1,
		      "CHeaderFile", 1,
		      "Struct", 1,
		      "Enum", 1,
		      "Variable", 1,
		      "Function", 1,
		      "Macro", 1
		      );

#
# Elaborate, reused regular expressions
#

# A C comment: /* ... */

$comment = "\\/\\*([^*]|\\*+[^*\\/])*\\*+\\/";

######################################################################
#
# Global variables:
#

#
# Abstracts:
#	functionName \377 Synopsis
#
# AllAbstracts:
#       functionName \377 Synopsis \377 fileName
#
@internalAbstracts = ();
@externalAbstracts = ();
@staticAbstracts = ();
@allAbstracts = ();

#
# SimplePrototypes:
#	functionName \377 returnType \377 arg type name \377
#	arg type name \377 ...
# 
@internalSimplePrototypes = ();
@externalSimplePrototypes = ();
@staticSimplePrototypes = ();

#
# Prototypes:
#	functionName \377 returnType \377 returnTypeComment \377
#	synopsis \377 description \377 sideEffects \377 seeAlso \377
#       filename \377 typed arg \377 typed arg \377 ...
#
# typed arg:
#	argType \376 argName \376 argComment
#
# seeAlso:
#	reference \376 reference \376 ...
#
@internalPrototypes = ();
@externalPrototypes = ();
@staticPrototypes = ();

#
# Commands:
#	commandName \377 commandArguments \377 commandSynopsis \377
#	commandDescription
#
@commands = ();

#
# SourceFiles:
#	sourceFileName \377 synopsis \377 description \377 seeAlso \377 author
#
@sourceFiles = ();

# Return value for the whole program: 1 for an error condition
$returnValue = 0;

# Debug flag: 1 for debugging
$debug = 0;

#
# List of formats for writing documentation:
#  Currently "text" and "html" are the only valid ones
#
@formats = ();

#
# Get rid of all characters before the final / in the name of this program
#

$0 =~ s/^.*\///;

#
# Switch on the name of the program that was run
#

if ( $0 eq "extdoc" ) {

    &extractDocumentation();

} elsif ( $0 eq "extproto" ) {

    &extractPrototypes();

}

exit($returnValue);

##Sub################################################################
#
# Synopsis	[ Behave as extproto -- extract function prototypes
# 		  and convert to ANSI function prototypes if the --toANSI
#		  flag is given ]
# Description	[ Take filenames from the command line one at a time.
#		  for .c files, extract static function declarations,
#		  for .h files, extract the internal or external
#		  function prototypes from all .c files in the package
#		  according to the name of the file. ]
# SideEffects	[]
#
######################################################################
sub extractPrototypes {

    $environment = "";		# Default environment
    $language = "ANSI";		# ANSI or KR
    $convertToANSI = 0;		# Set to 1 if we're doing conversion

    #
    # Process command-line options
    #

    while ( $ARGV[0] =~ /^-/) {

	$_ = shift(ARGV);

	if ( /^-v/ || /^--version/ ) {
	    print "$0 " . '$Revision: 1.30 $ ' . "\n";
	    exit;
	} elsif ( /^-d/ || /^--debug/ ) {
	    $debug = 1;
	} elsif ( /^-a/ || /^--ANSI/ ) {
	    $language = "ANSI";
	} elsif ( /^-k/ || /^--KR/ ) {
	    $language = "KR";
	} elsif ( /^--toANSI/ ) {
	    $language = "KR";
	    $convertToANSI = 1;
	    print STDERR "Warning: do not run extproto --toANSI on a file with ANSI-style function\ndeclarations.\n";
	} elsif ( /^--polis/ ) {
	    $environment = "polis";
	} else {
	    die "Usage: $0 [-v] [--version] [-a] [--ANSI] [-k] [--KR] [--toANSI] [--polis] fileNames\n";
	}
    }

    #
    # Process the remaining arguments -- filenames
    #

    foreach $pathname (@ARGV) {

	($dirname,$file) = &splitPathname($pathname);

	if ( $file =~ /[.]c$/ ) {

	    #
	    # A .c file: generate all the static function prototypes
	    #

	    open( FILE, $pathname );
	    print "processing $pathname\n";

	    @staticSimplePrototypes = ();
	    &processFile(1);

	    #
	    # Back up the old .c file
	    #

	    ($dev,$ino,$mode) = stat $pathname;
	    rename( $pathname, $pathname . "~" );

	    #
	    # Regenerate the .c file, including the newly-extracted
	    # prototypes in the automatic portion.  Set the access mode
	    # to be the same as the original.
	    #

	    print "Writing $pathname, a C source file\n";
	    open( FILE, ">$pathname" );
	    chmod( $mode, $pathname );
	    print FILE $beforePortion;
	    &writeStaticPrototypes();
	    print FILE $afterPortion;

	} elsif ( $file =~ /[.]h$/ ) {

	    if ( $convertToANSI ) {

		&error("Please use the --toANSI flag with .c files only");

	    } else {

		#
		# A .h file: process it to get the package name
		#

		$headerFile = $pathname;

		open ( FILE, $pathname );
		print "processing $pathname\n";
		@internalSimplePrototypes = ();
		@externalSimplePrototypes = ();
		&processFile(1);
		$package = $packageName;

		#
		# Process each .c file in the package to get the functions to
		# be exported
		#
		
		@cfiles = <$dirname$package*.c>;
		foreach $pathname (@cfiles) {
		    ($dirname,$file) = &splitPathname($pathname);
		    open( FILE, $pathname );
		    print "processing $pathname\n";
		    &processFile(0);
		}

		#
		# Back up the old .h file
		#

		($dev,$ino,$mode) = stat $headerFile;
		rename( $headerFile, $headerFile . "~" );

		#
		# Regenerate the .h file, including the newly-extracted
		# prototypes in the automatic portion
		#

		($dirname,$file) = &splitPathname($headerFile);

		open( FILE, ">$headerFile" );
		chmod($mode, $headerFile);
		print FILE $beforePortion;
		if ( $environment eq "polis" ) {
		    if ( $file =~ /_int[.]h$/ ) {
			print "Writing $headerFile, the internal header\n";
			&writeInternalPrototypes();
		    } else {
			print "Writing $headerFile, the external header\n";
			&writeExternalPrototypes();
		    }
		} else {
		    if ( $file =~ /Int[.]h$/ ) {
			print "Writing $headerFile, the internal header\n";
			&writeInternalPrototypes();
		    } else {
			print "Writing $headerFile, the external header\n";
			&writeExternalPrototypes();
		    }
		}
		print FILE $afterPortion;
	    }

	} else {
	    &error("Unrecognized file type `$pathname'");
	}
    }

}

##Sub################################################################
#
# Synopsis	[ Behave as extdoc -- extract human-readable documentation ]
# Description	[ Take package names from the command line and
# 		  extract the documentation from each file in the package. ]
# SideEffects	[]
#
######################################################################
sub extractDocumentation {

    $environment = "";		# Default environment
    $language = "ANSI";		# ANSI or KR

    $htmldir = ".";
    $textdir = ".";
		     
    #
    # Process command-line arguments
    #

    while ( $ARGV[0] =~ /^-/) {

	$_ = shift(ARGV);

	if ( /^-v$/ || /^--version$/ ) {
	    print "$0 " . '$Revision: 1.30 $ ' . "\n";
	    exit;
	} elsif ( /^-d$/ || /^--debug$/ ) {
	    $debug = 1;
	} elsif ( /^-t/ || /^--text/ ) {
	    push( @formats, "text" );
	    if ( /=./ ) {
		($textdir) = /=(.+)$/;
	    }
	} elsif ( /^-h/ || /^--html/ ) {
	    push( @formats, "html" );
	    if ( /=./ ) {
		($htmldir) = /=(.+)$/;
	    }
	} elsif ( /^-a$/ || /^--ANSI$/ ) {
	    $language = "ANSI";
	} elsif ( /^-k$/ || /^--KR$/ ) {
	    $language = "KR";
	} elsif ( /^--polis$/ ) {
	    $environment = "polis";
	} else {
	    die "Usage: $0 [-v] [--version] [-d] [--debug] [-t[=<dir>] [--text[=<dir>] [-h[=<dir>]] [--html[=<dir>]] [-a] [--ANSI] [-k] [--KR]\n\t[--polis] <package name> [<package name> ...]\n";
	}
	
    }

    foreach $pathname (@ARGV) {

	($dirname,$package) = &splitPathname($pathname);
	
	#
	# Find all the .c and .h files in the package
	#

	if ( $environment eq "polis" ) {
	    @cfiles = (<${dirname}com_$package.c>,<$dirname$package*.c>);
	    $internalHeaderFile = "$dirname$package" . "_int.h"
	} else {
	    @cfiles = <$dirname$package*.c>;
	    $internalHeaderFile = "$dirname$package" . "Int.h";
	}
	$externalHeaderFile = "$dirname$package" . ".h";

	#
	# Process each file
	#

	if ( open(FILE, $externalHeaderFile) ) {	    
	    print "processing $externalHeaderFile\n";
	    ($dirname,$file) = &splitPathname($externalHeaderFile);
	    &processFile(0);
	    $externalSynopsis = $headerSynopsis;
	    $externalDescription = $headerDescription;
	    $externalSeeAlso = $headerSeeAlso;
	    $externalAuthor = $headerAuthor;
	}
	
	if ( open(FILE, $internalHeaderFile) ) {
	    print "processing $internalHeaderFile\n";
	    ($dirname,$file) = &splitPathname($internalHeaderFile);
	    &processFile(0);
	    $internalSynopsis = $headerSynopsis;
	    $internalDescription = $headerDescription;
	    $internalSeeAlso = $headerSeeAlso;
	    $internalAuthor = $headerAuthor;
	}

	foreach $pathname (@cfiles) {
	    ($dirname,$file) = &splitPathname($pathname);
	    open( FILE, $pathname );
	    print "processing $pathname\n";
	    &processFile(0);
	}


	#
	# Write documentation in each of the formats requested
	#

	foreach $format (@formats) {
	    if ( $format eq "text" ) {
		&writeTextDocumentation();
	    } elsif ( $format eq "html" ) {
		&writeHTMLDocumentation();
	    }       	    
	}
	
    }

}

##Sub################################################################
#
# Synopsis	[ Write documentation as Text ]
# Description	[ Assumes the files have been processed.  Writes the
#		  documentation to a file pkgDoc.txt ]
# SideEffects	[]
#
######################################################################
sub writeTextDocumentation {
    
    $externalDocFile = "$textdir/$package" . "Doc.txt";

    #
    # Write the external documentation file, e.g., package.doc
    #

    print "writing $externalDocFile\n";	

    open(FILE, ">$externalDocFile");

    select(FILE);

    print "The $package package\n\n";

    $~ = "fillParagraph";

    $paragraph = $externalSynopsis;
    write;

    $paragraph = $externalAuthor;
    write;

    print "**********************************************************************\n\n";

    #
    # Print the function abstracts sorted by name
    #

    foreach (sort @externalAbstracts) {
	($functionName,$synopsis) = split(/\377/);
	$functionName .= "()";
	if ( length($functionName) > 30 ) {
	    $~ = "LengthyAbstract";
	} else {
	    $~ = "Abstract";
	}
	write;
    }

    print "**********************************************************************\n\n";
    
    $~ = "fillParagraph";

    $paragraph = $externalDescription;
    write;

    #
    # Print the function prototypes sorted by name
    #

    foreach (sort @externalPrototypes) {

	#
	# Print the return type and name
	#

	($functionName,$returnType,$returnTypeComment,$synopsis,
	 $description,$sideEffects,$seeAlso,$sourcefile,@typedArgs)
	    = split(/\377/);
	$synopsis =~ s/\s+/ /g;
	$description =~ s/\s+/ /g;
	$sideEffects =~ s/\s+/ /g;

	$functionName .= "(";
	$~ = "Prototype1";
	write;

	#
	# Print the arguments, their types, and comments
	#
	# The last argument is treated differently to prevent a comma
	# being added after it
	#

	$~ = "Prototype2";
	$lastArg = pop(@typedArgs);
	foreach (@typedArgs) {
	    ($argType,$argName,$argComment) = split(/\376/);
	    $argName .= ",";
	    write;
	}
	$_ = $lastArg;
	($argType,$argName,$argComment) = split(/\376/);
	write;

	#
	# Print the description (or synopsis if the description is empty)
	# and the side effects if they are non-empty.
	#

	$~ = "Prototype3";
	if ($description eq "") {
	    $description = $synopsis;
	}
	unless ($sideEffects eq "") {
	    $sideEffects = "Side Effects: $sideEffects";
	}       
	write;
    }
    
}

##Sub################################################################
#
# Synopsis	[ Write documentation as HTML ]
# Description	[ Assumes the files have been processed.  Writes the
#		  documentation to a file pkgDoc.html ]
# SideEffects	[]
#
######################################################################
sub writeHTMLDocumentation {

    $titleFile = "$htmldir/${package}Title.html";

    $descriptionFile = "$htmldir/${package}Desc.html";

    $externalFrameFile = "$htmldir/${package}Ext.html";
    $allFunctionFrameFile = "$htmldir/${package}AllByFunc.html";
    $allFileFrameFile = "$htmldir/${package}AllByFile.html";

    $externalAbstractDocFile = "$htmldir/${package}ExtAbs.html";
    $externalDetailedDocFile = "$htmldir/${package}ExtDet.html";

    $allAbstractDocFile = "$htmldir/${package}AllAbs.html";
    $allFileDocFile = "$htmldir/${package}AllFile.html";
    $allDetailedDocFile = "$htmldir/${package}AllDet.html";

    ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);

    $generationtime = sprintf("%2.2d%2.2d%2.2d %2.2dh%2.2d",
			      $year,$mon+1,$mday,$hour,$min);

    ######################################################################
    #
    # Write the package title file, e.g., pkgTitle.html
    #

    print "writing $titleFile\n";
    open(FILE, ">$titleFile");
   
    print FILE <<EndOfHTML;
<HTML>
<HEAD><TITLE>The ${package} package: Title</TITLE></HEAD>
<BODY>

<TABLE BORDER WIDTH="100%">
  <TR>
    <TD ALIGN=center> <A HREF="${package}Ext.html" TARGET="_top">
        Programmer view</A> </TD>
    <TD ALIGN=center> <A HREF="${package}AllByFunc.html" TARGET="_top">
	Maintainer by function</A> </TD>
    <TD ALIGN=center> <A HREF="${package}AllByFile.html" TARGET="_top">
        Maintainer by file</A> </TD>
  </TR>
</TABLE>

</BODY>
</HTML>
EndOfHTML

    ######################################################################
    #
    # Write the package description file, e.g., pkgDesc.html
    #

    print "writing $descriptionFile\n";
    open(FILE, ">$descriptionFile");

    print FILE <<EndOfHTML;
<HTML>
<HEAD><TITLE>The ${package} package: Overview</TITLE></HEAD>
<BODY>

<H1>The ${package} package</H1>
<H2>${externalSynopsis}</H2>
<H3>By ${externalAuthor}</H3>

<UL>
 <LI> <A HREF="${package}Ext.html" TARGET="_top">
    Information for programmers</A>
 <LI> <A HREF="${package}AllByFunc.html" TARGET="_top">
    Information for developers sorted by function</A>
 <LI> <A HREF="${package}AllByFile.html" TARGET="_top">
    Information for developers sorted by file</A>
</UL>

<HR>

${externalDescription}

<HR>

Last updated on ${generationtime}

</BODY>
</HTML>
EndOfHTML

    ######################################################################
    #
    # Write the external frame file, e.g., pkgExt.html
    #

    print "writing $externalFrameFile\n";

    open(FILE, ">$externalFrameFile");

    print FILE <<EndOfHTML;
<HTML>
<HEAD><TITLE>The ${package} Package for Programmers</TITLE></HEAD>

<FRAMESET ROWS="5%,90%,5%">
  <FRAME SRC="${package}Title.html">
  <FRAMESET COLS="40%,60%">
    <FRAME SRC="${package}ExtAbs.html" NAME="ABSTRACT">
    <FRAME SRC="${package}ExtDet.html" NAME="MAIN">
  </FRAMESET>
  <FRAME SRC="credit.html">
</FRAMESET>

</HTML>
EndOfHTML

    ######################################################################
    #
    # Write the function-oriented frame file, e.g., pkgAllByFunc.html
    #

    print "writing $allFunctionFrameFile\n";

    open(FILE, ">$allFunctionFrameFile");

    print FILE <<EndOfHTML;
<HTML>
<HEAD><TITLE>The ${package} package for maintainers</TITLE></HEAD>

<FRAMESET ROWS="5%,90%,5%">
  <FRAME SRC="${package}Title.html">
  <FRAMESET COLS="40%,60%">
    <FRAME SRC="${package}AllAbs.html" NAME="ABSTRACT">
    <FRAME SRC="${package}AllDet.html" NAME="MAIN">
  </FRAMESET>
  <FRAME SRC="credit.html">
</FRAMESET>

</HTML>
EndOfHTML

    select(STDOUT);

    ######################################################################
    #
    # Write the file-oriented frame file, e.g., pkgAllByFile.html
    #

    print "writing $allFileFrameFile\n";

    open(FILE, ">$allFileFrameFile");

    print FILE <<EndOfHTML;
<HTML>
<HEAD><TITLE>The ${package} package for maintainers</TITLE></HEAD>

<FRAMESET ROWS="5%,90%,5%">
  <FRAME SRC="${package}Title.html">
  <FRAMESET COLS="40%,60%">
    <FRAME SRC="${package}AllFile.html" NAME="ABSTRACT">
    <FRAME SRC="${package}AllDet.html" NAME="MAIN">
  </FRAMESET>
  <FRAME SRC="credit.html">
</FRAMESET>

</HTML>
EndOfHTML

    select(STDOUT);

    ######################################################################
    #
    # Write the external Abstract documentation file, e.g., pkgExtAbs.html
    #

    print "writing $externalAbstractDocFile\n";	

    open(FILE, ">$externalAbstractDocFile");

    select(FILE);

    #
    # Write the title of the file
    #

    print "<html>\n<head><title>$package package abstract</title></head>\n";
    print "<body>\n\n";

    #
    # Print the function abstracts sorted by name
    #

    print "\n<!-- Function Abstracts -->\n\n<dl>\n";

    foreach (sort @externalAbstracts) {
	($functionName,$synopsis) = split(/\377/);
	print "<dt> <a href=\"${package}AllDet.html#$functionName\" TARGET=\"MAIN\"><code>$functionName()</code></a>\n";
	print "<dd> $synopsis\n\n";
    }

    print "</dl>\n";

    print "\n<hr>\n\n";

    print "Last updated on ${generationtime}\n";

    #
    # Write the trailer
    #

    print "</body></html>\n";

    ######################################################################
    #
    # Write the internal Abstract documentation file, e.g., pkgAllAbs.html
    #

    select(STDOUT);

    print "writing $allAbstractDocFile\n";	

    open(FILE, ">$allAbstractDocFile\n");

    select(FILE);

    #
    # Write the title of the file
    #

    print "<HTML>\n<HEAD><TITLE>$package package abstract</TITLE></HEAD>\n";
    print "<BODY>\n\n";

    #
    # Print the function abstracts sorted by name
    #

    print "\n<!-- Function Abstracts -->\n\n<dl>\n";

    foreach (sort (@internalAbstracts,@externalAbstracts,@staticAbstracts) ) {
	($functionName,$synopsis) = split(/\377/);
	print "<DT> <A HREF=\"${package}AllDet.html#$functionName\" TARGET=\"MAIN\"><CODE>$functionName()</CODE></A>\n";
	print "<DD> $synopsis\n\n";
    }

    print "</DL>\n";

    print "\n<HR>\n\n";

    print "Last updated on ${generationtime}\n";

    print "</BODY></HTML>\n";

    ######################################################################
    #
    # Write the external detailed documentation file, e.g., pkgDet.html
    #

    select(STDOUT);

    print "writing $externalDetailedDocFile\n";	

    open(FILE, ">$externalDetailedDocFile\n");

    select(FILE);

    #
    # Write the title of the file
    #

    print "<HTML>\n<HEAD><TITLE>The $package package</TITLE></HEAD>\n";
    print "<BODY>\n\n";

    #
    # Write a detailed view of each function
    #

    print "<DL>\n";

    local(@sortedExternalPrototypes) = sort(@externalPrototypes);

    $writeMaintainers = 0;

    foreach ( @sortedExternalPrototypes ) {
	&writeHTMLFunction($_);
    }

    print "\n</DL>\n";

    #
    # Write a trailer
    #

    print "<HR>\n";
    print "Last updated on ${generationtime}\n";

    print "</BODY></HTML>\n";

    ######################################################################
    #
    # Write the internal detailed documentation file, e.g., pkgDetInt.html
    #

    select(STDOUT);

    print "writing $allDetailedDocFile\n";	

    open(FILE, ">$allDetailedDocFile\n");

    select(FILE);

    #
    # Write the title of the file
    #

    print "<html>\n<head><title>The $package package: all functions </title></head>\n";
    print "<body>\n\n";


    print "${internalDescription}\n";

    print "<HR>\n";

    #
    # Print the function prototypes sorted by name
    #

    print "<DL>\n";

    local(@sortedAllPrototypes) = sort(@externalPrototypes,
				       @internalPrototypes,
				       @staticPrototypes );

    $writeMaintainers = 1;

    foreach ( @sortedAllPrototypes ) {
	&writeHTMLFunction($_);
    }

    print "\n</DL>\n";

    print "<HR>\n";
    print "Last updated on ${generationtime}\n";

    print "</BODY></HTML>\n";

    select(STDOUT);

    ######################################################################
    #
    # Write the file documentation file, e.g., pkgAllFile.html
    #

    print "writing ${allFileDocFile}\n";

    open( FILE, ">$allFileDocFile");

    select(FILE);

    print "<HTML>\n<HEAD><TITLE>The $package package: files</TITLE></HEAD>\n";;
    print "<BODY>\n\n";

    #
    # Print a simple list of the files in the package
    #

    print "<DL>\n";
    foreach (@sourceFiles) {
	local($sourceFilename,$synopsis,
	      $description,$seeAlso,$author) = split(/\377/);
	print " <DT> <A HREF=\"#${sourceFilename}\"><CODE>${sourceFilename}</CODE></A>\n";
        print " <DD> ${synopsis}\n"
    }
    print "</DL>";

    #
    # Print a detailed list of each file in the package, including
    # file descriptions and the functions contained within
    #

    foreach (@sourceFiles) {
	local($sourceFilename,$synopsis,
	      $description,$seeAlso,$author) = split(/\377/);

	print "<HR>\n";
        print "<A NAME=\"$sourceFilename\"><H1>$sourceFilename</H1></A>\n";;
	print "$synopsis <P>\n";
	unless ( $author eq "" ) {
	    print "<B>By: $author</B><P>\n";
	}
	unless ( $description eq "" ) {
	    print "$description <P>\n";
	}
	unless ( $seeAlso eq "" ) {
	    print "<P><B>See Also</B>";
	    local(@refs) = split(' ', $seeAlso);
	    foreach (@refs ) {
		print "<A HREF=\"#$_\"><CODE>$_</CODE></A>\n";
	    }
	}

	#
	# Print all the functions that appear in this file
	#

	print "<DL>\n";
	local( @containedFunctions ) = 
	    grep( /${sourceFilename}/, @allAbstracts );
	foreach ( @containedFunctions ) {

	    local($functionName,$synopsis,$sourcefile) = split(/\377/);
	    print " <DT> <A HREF=\"${package}AllDet.html#${functionName}\" TARGET=\"MAIN\"><CODE>${functionName}()</CODE></A>\n";
	    print " <DD> ${synopsis}\n\n";

	}
	print "</DL>\n";
    }

    print "<HR>\n";
    print "Last updated on ${generationtime}\n";
    print "</BODY></HTML>\n";

    select(STDOUT);

    ######################################################################
    #
    # Write the documentation for the commands, if any.
    #

    foreach (@commands) {
	local($commandName,$commandArguments,$commandSynopsis,
	      $commandDescription) = split(/\377/);

	local($filename) = "$htmldir/${commandName}Cmd.html";
	print "writing $filename\n";
	open( FILE, ">$filename\n");
	select(FILE);

	#
	# Write the title of the file
	#

	print "<html>\n<head><title>$commandName</title></head>\n";
	print "<body>\n\n";

	#
	# Write the title and usage
	#

	print "<h3>$commandName - $commandSynopsis</h3>\n<hr>\n";
	print "<code>$commandName $commandArguments</code><p>\n";

	print "$commandDescription\n\n";

        print "<HR>\n";
        print "Last updated on ${generationtime}\n";

	print "</body></html>\n";

	select(STDOUT);
	
    }

}

##Sub################################################################
#
# Synopsis	[ Write a function's HTML documentation ]
# Description	[ Takes a single argument (from a Prototype array)
# 		  and writes to the current file. ]
# SideEffects	[]
#
######################################################################
sub writeHTMLFunction {

    print "<dt><pre>\n";

    local($functionName,$returnType,$returnTypeComment,
	  $synopsis,$description,$sideEffects,$seeAlso,$sourcefile,
	  @typedArgs) = split(/\377/);

    #
    # Print the return type and function name
    #

    print "<A NAME=\"$functionName\"></A>\n";
    print "$returnType <I>$returnTypeComment</I>\n";  
    print "<B>$functionName</B>(\n";

    #
    # Print the arguments, their types, and comments
    #
    # The last argument is treated differently to prevent a comma
    # being added after it
    #

    $~ = "Prototype2";
    local($lastArg) = pop(@typedArgs);
    foreach (@typedArgs) {
	($argType,$argName,$argComment) = split(/\376/);
	print "  $argType <b>$argName</b>, <i>$argComment</i>\n";
    }
    $_ = $lastArg;
    ($argType,$argName,$argComment) = split(/\376/);
    print "  $argType <b>$argName</b> <i>$argComment</i>\n";
    
    print ")\n</pre>\n";
    
    #
    # Print the description (or synopsis if the description is empty)
    # and the side effects if they are non-empty.
    #
    
    if ($description eq "") {
	$description = $synopsis;
    }
    
    print "<dd> $description\n<p>\n\n";
    
    unless ($sideEffects eq "") {
	print "<dd> <b>Side Effects</b> $sideEffects\n<p>\n\n";
    }

    unless ($seeAlso eq "") {

	print "<dd> <b>See Also</b> <code>";
	$_ = $seeAlso;
	local(@refs) = split(/\376/);
	local($thePackage,$theFilename);
	foreach ( @refs ) {

	    #
	    # Get the package name -- the leading alphabetic characters --
	    # and convert to lowercase
	    #

	    if ( $environment eq "polis" ) {
		/^([A-Za-z]*)/;
		$thePackage = $1;
	    } else {
		/^([A-Za-z][a-z]*)/;
		$thePackage = $1;
		$thePackage =~ tr/A-Z/a-z/;
	    }

	    if ( length($thePackage) > 7 || length($thePackage) < 2  ) {

		#
		# The package name is the wrong length -- don't make it a link
		#

		print "$_\n";

	    } else {

		#
		# It's probably a package -- make a link
		#

		print "<a href=\"#$_\">$_</a>\n";

	    }
	}

	print "</code>\n\n";

    }

    if ( $writeMaintainers && $sourcefile ne "" ) {
	print "<DD> <B>Defined in </B> ";
	if ( $sourcefile =~ /(\.c|\.h)$/ ) {
	    print "<A HREF=\"${package}AllFile.html#${sourcefile}\"";
	    print "TARGET=\"ABSTRACT\">";
	    print "<CODE>$sourcefile</CODE></A>\n\n";
	} else {
	    print "<CODE>$sourcefile</CODE>\n\n";
	}
    }
    
}

##Sub################################################################
#
# Synopsis	[ Scan a file for structured comments ]
# Description	[ Scans a file for structured comments and the
#		  AutomaticStart/End directives, processing the
#		  structured comments as appropriate.  Uses the
#		  FILE file. ]
# SideEffects	[ Prepares the before, automatic, and after portions
#		  if requested by the first argument. ]
#
######################################################################
sub processFile {

    local($shouldSave) = @_;

    $saveLines = $shouldSave;

    if ( $shouldSave ) {
	$beforePortion = "";
	$afterPortion = "";
	$beforeAutomatic = 1;
    }

    while ( <FILE> ) {

	&saveLine;
	
	chop;
	
	if ( /^\/\*\*.*[^\/]$/ ) {

	    #
	    # The line begins with /** and doesn't end with /: it's a
	    # structured comment -- process it accordingly
	    #
	
	    ($commentType, %fields) = &processStructuredComment;

	    if ( $debug ) {
		foreach $key (keys %fields) {
		    &error(" $key: $fields{$key}");
		}
	    }

	    if ( $knownCommentTypes{$commentType} ) {
		eval "&process$commentType($commentType, %fields)";
	    } else {
		&errorNearLine("Unknown comment type `$commentType'");
	    }

	} elsif ( /^\/\*\*AutomaticStart/ ) {

	    #
	    # We've found a comment /**AutomaticStart, so discard everything
	    # between it and /**AutomaticEnd
	    #
	    
	    while ( ($line = <FILE>) &&
		   !( $line =~ /^\/\*\*AutomaticEnd/) ) { }
	    if ( $saveLines ) {
		$afterPortion = join('',$afterPortion,$line);
	    }
	    $beforeAutomatic = 0;
	    
	}
	
    }

}

##Sub################################################################
#
# Synopsis	[ Parse a structured comment ]
# Description	[ Looks for directive \[ bracketed-text \] pairs.
#		  Returns the comment type and the directive-value pairs.
#		  Reads from the file FILE. ]
# SideEffects	[]
#
######################################################################
sub processStructuredComment {

    #
    # Strip out the comment and whitespace characters from the first
    # line to get the comment type
    #

    s/[\/\*\s]//g;

    $commentType = $_;

    #
    # Suck in everything up to a closing */
    #

    local($/) = "*/";
    $_ = <FILE>;

    #
    # Count the number of lines we just sucked in
    #

    while ( /\n/g ) { $.++; }
    $.--;

    &saveLine;

    #
    # Extract the directive-text pairs
    #

    #
    # Hide backslash-escaped characters by moving them into the ether
    # (see p. 234 of the Camel Book)
    # 

    s/\\(.)/"\376".ord($1)."\377"/eg;

    #
    # This is it: look for alphanumeric directives separated by some
    # whitespace from bracket-enclosed text.  Put these pairs
    # into the @fields array.
    #

    @fields = /\s*(\w+)\s*\[\s*([^\]]*)\]/g;

    #
    # Convert backslashed-escaped characters into normal ones
    # and remove trailing whitespace.
    #

    grep( do {
	s/\376(\d+)\377/pack(C,$1)/eg;
	s/\s+$//;
    }, @fields);

    #
    # Return the comment type and the fields
    #

    ($commentType,@fields);
}

##Sub################################################################
#
# Synopsis	[ Process a CFile structured comment ]
# Description	[]
# SideEffects	[ Sets the global variable $packageName ]
#
######################################################################
sub processCFile {

    local($commentType,%fields) = @_;

    defined($fields{FileName}) ||
	&errorNearLine("Missing FileName directive");
    defined($fields{PackageName}) ||
	&errorNearLine("Missing PackageName directive");
    defined($fields{Synopsis}) ||
	&errorNearLine("Missing Synopsis directive");
    defined($fields{Copyright}) ||
	&errorNearLine("Missing Copyright directive");

    if ( $fileName = $fields{FileName} ) {
	$fileName =~ s/\s+//g;
	($fileName eq $file) ||
	    &errorNearLine("FileName [$fileName] doesn't match name of file being processed");
    }
   
    if ( $packageName = $fields{PackageName} ) {

	#
	# Translate the package name to lowercase
	#

	$packageName =~ tr/A-Z/a-z/;

	if ( $package ne "" && $packageName ne $package ) {
	    &errorNearLine("PackageName [$packageName] doesn't match name of package being processed");
	}

    }

    push(@sourceFiles, join("\377",($fileName,$fields{Synopsis},
				    $fields{Description},$fields{SeeAlso},
				    $fields{Author})));

}

##Sub################################################################
#
# Synopsis	[ Process a CHeaderFile structured comment ]
# Description	[]
# SideEffects	[ Sets the global variables
#  $packageName:	Name of the package
#  $headerSynopsis:	One-line synopsis
#  $headerDescription:	Lengthy description
#  $headerSeeAlso:	Space-separated list of cross-references
#  $headerAuthor:	File's author(s)
# ]
#
######################################################################
sub processCHeaderFile {

    local($commentType,%fields) = @_;

    defined($fields{FileName}) ||
	&errorNearLine("Missing FileName directive");
    defined($fields{PackageName}) ||
	&errorNearLine("Missing PackageName directive");
    defined($fields{Synopsis}) ||
	&errorNearLine("Missing Synopsis directive");
    defined($fields{Copyright}) ||
	&errorNearLine("Missing Copyright directive");

    if ( $fileName = $fields{FileName} ) {
	$fileName =~ s/\s+//g;
	($fileName eq $file) ||
	    &errorNearLine("FileName [$fileName] doesn't match name of file being processed");
    }

    if ( $packageName = $fields{PackageName} ) {

	#
	# Translate the package name to lowercase
	#

	$packageName =~ tr/A-Z/a-z/;

	if ( $package ne "" && $packageName ne $package ) {
	    &errorNearLine("PackageName [$packageName] doesn't match name of package being processed");
	}

    }

    $headerSynopsis = $fields{Synopsis};
    $headerDescription = $fields{Description};
    $headerSeeAlso = $fields{SeeAlso};
    $headerAuthor = $fields{Author};

    if ( $fileName =~ /$packageName\.h/ ) {
	push(@sourceFiles, join("\377",($fileName,"External header file",
					"",$fields{SeeAlso},
					$fields{Author})));
    } else {
	if ( $fileName =~ /$packageName[I]nt\.h/ ) {
	    push(@sourceFiles, join("\377",($fileName,"Internal header file",
					    "",$fields{SeeAlso},
					    $fields{Author})));
	} else {
	    push(@sourceFiles, join("\377",($fileName,"Other header file",
					    "",$fields{SeeAlso},
					    $fields{Author})));
	}
    }

}

##Sub################################################################
#
# Synopsis	[ Process a Struct structured comment ]
# Description	[]
# SideEffects	[]
#
######################################################################
sub processStruct {

    local($commentType,%fields) = @_;

}

##Sub################################################################
#
# Synopsis	[ Process a Enum structured comment ]
# Description	[]
# SideEffects	[]
#
######################################################################
sub processEnum {

    local($commentType,%fields) = @_;

}

##Sub################################################################
#
# Synopsis	[ Process a Variable structured comment ]
# Description	[]
# SideEffects	[]
#
######################################################################
sub processVariable {

    local($commentType,%fields) = @_;

}

##Sub################################################################
#
# Synopsis	[ Process a Function structured comment ]
# Description	[ Parse the function declaration immediately following
#		  the comment, extracting types, names, comments, etc.
#
#		  Interprets function headers differently depending on
#		  the setting of the $language global variable:
#		  $language eq "KR" parses K&R-style (old) comments
#		  $language eq "ANSI" parses ANSI-style (new) comments
#
#		  if $convertToANSI is set and language is KR, the
#		  function prototypes are written to the program text
#		  in ANSI form, rather than verbatim.
#
#		 ]
# SideEffects	[ The following global arrays are updated:
#
#  @internalAbstracts, @externalAbstracts, @staticAbstracts
#  @internalPrototypes, @externalPrototypes, @staticPrototypes
#  @internalSimplePrototypes, @externalSimplePrototypes,
#  @staticSimplePrototypes
#
#  ]
#
######################################################################
sub processFunction {

    local($functionIsNotStatic) = 1;

    local($commentType,%fields) = @_;

    defined($fields{Synopsis}) ||
	&errorNearLine("Missing Synopsis directive");
    defined($fields{SideEffects}) ||
	&errorNearLine("Missing SideEffects directive");

    if ( defined( $fields{CommandName} ) ) {
	
	#
	# The CommandName field is defined -- this function defines a
	# command -- record it accordingly
	#

	local($commandName) = $fields{CommandName};
	$commandName =~ s/\s+//g;
	local($commandArguments) = $fields{CommandArguments};
	$commandArguments =~ s/\s+/ /g;
	local($commandSynopsis) = $fields{CommandSynopsis};
	$commandSynopsis =~ s/^\s+//;
	$commandSynopsis =~ s/\s+$//;
	$commandSynopsis =~ s/\s+/ /g;
	local($commandDescription) = $fields{CommandDescription};

	push(@commands, join("\377", ($commandName, $commandArguments,
				      $commandSynopsis,$commandDescription)) );
    }

    #
    # Convert collections of whitespace in the fields to a single space.
    # 

    local($synopsis) = $fields{Synopsis};
    local($sideEffects) = $fields{SideEffects};
    local($description) = $fields{Description};

    $_ = $fields{SeeAlso};
    s/,/ /g;
    local($seeAlso) = join("\376",split(' '));

    #
    # Suck in everything up to an opening \{
    #

    local($/) = "{";
    local($*) = 1;
    $_ = <FILE>;

    #
    # Count the lines we just sucked in
    #

    while ( /\n/g ) { $.++; }
    $.--;

    if ( $language eq "KR" ) {

	#
	# Parse K&R-style functions
	#

	unless ( $convertToANSI ) {
	    &saveLine;
	}

	#
	# Parse the function declaration
	#
	# Looks for a function name preceding a parenthesis-enclosed
	# list.  The return type precedes the function name.  Everything
	# following the close-paren up to the \{ is considered a list of
	# argument types.
	# 

#    &error("`$_'\n");

	local($returnType,$functionName,$args,$argTypes) =
	    /^\s*(.*)\s+(\w+)\s*\(([^\)]*)\)([^\{]*)\{/;

#    &error("functionName: `$functionName'");
#    &error("args: `$args'");
#    &error("argTypes: `$argTypes'");

	#
	# Extract the return type comment, if any, strip
	# whitespace from around the return type,
	# and strip leading /* and training */
	# from the return type comment
	#

	local($returnTypeComment);    
	$_ = $returnType;
	($returnType,$returnTypeComment) = /^([^\/]*)(|\/\*.*)$/;
	$returnType =~ s/^\s+//;
	$returnType =~ s/\s+$//;
	$returnTypeComment =~ s/^\s*\/\*\s*|\s*\*\/\s*$//g;

	if ( $returnType =~ /^static/ ) {
	    $returnType =~ s/^static\s*//;
	    $functionIsNotStatic = 0;
	}

	#
	# Extract the typed arguments as argument-whitespace/comment
	# ranges
	#
	# Long ranges of whitespace within the comments (e.g., multi-lines
	# are folded into a single space.)
	#
	# The result is the array @typedArgs, whose entries are of the form
	#	 argumentType\376argumentName\376argumentComment
	#

	local(@typedArgs, @simpleArgs);
	local($arg,$argType,$argName,$argComment);

	$_ = $argTypes;
	while ( /\s*([^;]*;(\s|\/\*([^\*]|\*[^\/])*\*\/)*)/g ) {
	    $arg = $1;
	    $arg =~ s/\s+/ /g;
	    ($arg,$argComment) = split(/;/,$arg);
#	    $argComment = $2;
	    $argComment =~ s/^\s+|\s+$//g;
	    $argComment =~ s/^\/\*\s*|\s*\*\/$//g;
	    $argComment =~ s/\*//g;
	    $argComment =~ s/\s\s+/ /g;
	    push(@simpleArgs,$arg);
	    $arg = reverse($arg);
#	&error($arg);
	    $arg =~ /^(\w+)(\W.*)$/;
	    $argName = reverse($1);
	    $argType = reverse($2);
#	&error($argType);
#	&error($argName);
#	    &error($argComment);

	    push(@typedArgs,join("\376",($argType,$argName,$argComment)));
	}

#    &error(join(" , ",@typedArgs));

#    &error("returnType: `$returnType'");
#    &error("returnTypeComment: `$returnTypeComment'");

#    &error("functionName: `$functionName'");

	#
	# Check to see if the function name starts with Package_, in which
	# case it is external.
	#

	if ( $environment eq "polis" ) {
	    local($functionIsExternal) = ($functionName =~ /^$packageName[_]/i)
		|| ($functionName =~ /[init_]$packageName/)
		    || ($functionName =~ /[end_]$packageName/);
	} else {
	    local($functionIsExternal) = ($functionName =~ /^$packageName[_]/i);
	}

	#
	# Add the function information to the internal, external, or static
	# defintions list
	#

	if ( $functionIsExternal ) {
	    push(@externalAbstracts,join("\377",($functionName,$synopsis)));
	    push(@externalPrototypes,
		 join("\377",($functionName,$returnType,$returnTypeComment,
			      $synopsis,$description,$sideEffects,$seeAlso,
			      @typedArgs)));
	    push(@externalSimplePrototypes,
		 join("\377",($functionName,$returnType,@simpleArgs)));
	} else {
	    if ( $functionIsNotStatic ) {
		push(@internalAbstracts,join("\377",($functionName,$synopsis)));
		push(@internalPrototypes,
		     join("\377",($functionName,$returnType,$returnTypeComment,
				  $synopsis,$description,$sideEffects,$seeAlso,
				  @typedArgs)));
		push(@internalSimplePrototypes,
		     join("\377",($functionName,$returnType,@simpleArgs)));
	    } else {
		push(@staticAbstracts,join("\377",($functionName,$synopsis)));
		push(@staticPrototypes,
		     join("\377",($functionName,"static $returnType",
				  $returnTypeComment,
				  $synopsis,$description,$sideEffects,$seeAlso,
				  @typedArgs)));
		push(@staticSimplePrototypes,
		     join("\377",($functionName,$returnType,@simpleArgs)));
	    }

	}

	push( @allAbstracts, join( "\377", ($functionName,$synopsis,$file) ) );

	if ( $convertToANSI ) {

	    #
	    # Converting to ANSI-type function prototypes -- add
	    # the ANSI version of the declaration we just saw to the
	    # saved program text
	    #

	    local($ANSIPrototype) = "\n";
	
	    unless ($functionIsNotStatic) {
                $ANSIPrototype .= "static ";
	    }
	    $ANSIPrototype .= $returnType;
	    if ( $returnTypeComment ne "" ) {
		$ANSIPrototype .= " /* $returnTypeComment */";
	    }
	    $ANSIPrototype .= "\n$functionName(\n";

	    $lastArg = pop(@typedArgs);
	    foreach (@typedArgs) {
		($argType,$argName,$argComment) = split(/\376/);
		$ANSIPrototype .= "  $argType $argName";
		if ( $argComment ne "" ) {
		    $ANSIPrototype .= " /* $argComment */";
		}
		$ANSIPrototype .= ",\n";
	    }
	    $_ = $lastArg;
	    ($argType,$argName,$argComment) = split(/\376/);
	    $ANSIPrototype .= "  $argType $argName";
	    if ( $argComment ne "" ) {
		$ANSIPrototype .= " /* $argComment */";
	    }
	    $ANSIPrototype .= ")\n{";

	    if ($beforeAutomatic) {
		$beforePortion .= $ANSIPrototype;
	    } else {
		$afterPortion .= $ANSIPrototype;
	    }

	}

    } else {

	#
	# Parse ANSI-style functions
	#

	&saveLine;

	#
	# Parse the function declaration
	#
	# Looks for a function name preceding a parenthesis-enclosed
	# list.  The return type precedes the function name.
	# 

	local($returnType,$functionName,$args) =
	    /^\s*(.*)\s+(\w+)\s*\(\s*(([^\/\)]|$comment)*)\s*\)\s*\{/o;

	if ( $debug ) {
	    &error("$returnType $functionName($args)");
	}

	#
	# Extract the return type comment, if any, strip
	# whitespace from around the return type,
	# and strip leading /* and training */
	# from the return type comment
	#

	local($returnTypeComment);
	$_ = $returnType;
	($returnType,$returnTypeComment) = /^([^\/]*)(|\/\*.*)$/;
	$returnType =~ s/^\s+//;
	$returnType =~ s/\s+$//;
	$returnTypeComment =~ s/^\s*\/\*\s*|\s*\*\/\s*$//g;

	if ( $returnType =~ /^static/ ) {
	    $returnType =~ s/^static\s*//;
	    $functionIsNotStatic = 0;
	}

	#
	# Extract the arguments as argument-whitespace/comment
	# ranges.
	#
	# Long ranges of whitespace within the comments (e.g., multi-lines
	# are folded into a single space.)
	#
	# The result is the array @typedArgs, whose entries are of the form
	#	 argumentType\376argumentName\376argumentComment
	#

	local(@typedArgs, @simpleArgs);
	local($arg,$argType,$argName,$argComment);

	if ( $args ne "" ) {

	    $_ = $args . ",";
	    while ( /\s*([^\/,]*)\s*(|$comment)\s*,/go ) {
	
		if ( $debug ) {
		    &error("`$1' `$2'");
		}
		$arg = $1;
		$argComment = $2;
		$argComment =~ s/^\s+|\s+$//g;
		$argComment =~ s/^\/\*\s*|\s*\*\/$//g;
#		$argComment =~ s/\*//g;
		$argComment =~ s/\s\s+/ /g;
		$arg =~ s/\s+/ /g;
		$arg =~ s/\s*$//;
		push(@simpleArgs,$arg);
		$arg = reverse($arg);
		$arg =~ /^(\w+)\s*(\W.*)$/;
		$argName = reverse($1);
		$argType = reverse($2);

		if ( $debug ) {
		    &error("argType: `$argType'");
		    &error("argName: `$argName'");
		    &error("argComment: `$argComment'");
		}

		push(@typedArgs,join("\376",($argType,$argName,$argComment)));
	    }
	}

	#
	# Check to see if the function name starts with Package_, in which
	# case it is external.
	#

	local($functionIsExternal) = ($functionName =~ /^$packageName[_]/i);

	#
	# Add the function information to the internal, external, or static
	# defintions list
	#

	if ( $functionIsExternal ) {
	    push(@externalAbstracts,join("\377",($functionName,$synopsis)));
	    push(@externalPrototypes,
		 join("\377",($functionName,$returnType,$returnTypeComment,
			      $synopsis,$description,$sideEffects,$seeAlso,
			      $file,@typedArgs)));
	    push(@externalSimplePrototypes,
		 join("\377",($functionName,$returnType,@simpleArgs)));
	} else {
	    if ( $functionIsNotStatic ) {
		push(@internalAbstracts,join("\377",($functionName,$synopsis)));
		push(@internalPrototypes,
		     join("\377",($functionName,$returnType,$returnTypeComment,
				  $synopsis,$description,$sideEffects,$seeAlso,
				  $file,@typedArgs)));
		push(@internalSimplePrototypes,
		     join("\377",($functionName,$returnType,@simpleArgs)));
	    } else {
		push(@staticAbstracts,join("\377",($functionName,$synopsis)));
		push(@staticSimplePrototypes,
		     join("\377",($functionName,$returnType,@simpleArgs)));
		push(@staticPrototypes,
		     join("\377",($functionName,"static $returnType",
				  $returnTypeComment,
				  $synopsis,$description,$sideEffects,$seeAlso,
				  $file,@typedArgs)));
	    }

	}

	push( @allAbstracts, join("\377",$functionName,$synopsis,$file) );

    }

}

##Sub################################################################
#
# Synopsis	[ Process a Macro structured comment ]
# Description	[ Parse the macro defintion immediately following,
#		  extracting names and comments of the arguments. ]
# SideEffects	[ The
#  		@internalAbstracts, @internalPrototypes
#		@externalAbstracts, @externalPrototypes
#  		 arrays are updated depending on what sort of macro it is.
#
#		See the processFunction SideEffects for the format
# ]
#
######################################################################
sub processMacro {

    local($commentType,%fields) = @_;

    defined($fields{Synopsis}) ||
	&errorNearLine("Missing Synopsis directive");
    defined($fields{SideEffects}) ||
	&errorNearLine("Missing SideEffects directive");

    #
    # Convert collections of whitespace in the fields to a single space.
    # 

    $synopsis = $fields{Synopsis};
    $sideEffects = $fields{SideEffects};
    $description = $fields{Description};

    $_ = $fields{SeeAlso};
    s/,/ /g;
    local($seeAlso) = join("\376",split(' '));

    #
    # Suck in everything up to a closing \)
    #

    local($/) = ")";
    local($*) = 1;
    $_ = <FILE>;

    #
    # Count the lines we just sucked in
    #

    while ( /\n/g ) { $.++; }
    $.--;

    &saveLine;

#    &error("_: `$_'");

    #
    # Parse the macro definition
    #
    # Looks for the macro name between #define and \(
    # A comment inserted between the #define and the macro name
    #  is interpreted as the return type
    # Everything else is part of the argument list
    #

    if ( $environment eq "polis" ) {

	# A hack -- don't assume there is a () after the macro definition
	local($returnType,$junk,$macroName,$args) =
	    /^#\s*define\s+(|$comment)\s*(\w+)(\s|(\(([^\)]*)\)))/o;
 
    } else {
	local($returnType,$junk,$macroName,$args) =
	    /^#\s*define\s+(|$comment)\s*(\w+)\s*\(([^\)]*)\)/o;
    }
    
    #
    # Strip off leading whitespace and a /* and */ and trailing whitespace
    # from the return type
    #

    $returnType =~ s/^\s*\/\*\s*//;
    $returnType =~ s/\s*\*\/\s*$//;

    #
    # Clean up the argument list by removing newlines and backslashes
    #

    $args =~ s/(\\|)\n/ /g;

#    &error("returnType: `$returnType'");
#    &error("macroName: `$macroName'");
#    &error("args: `$args'");

    #
    # Extract the arguments as argument-whitespace/comment ranges
    #
    # The result is the array @args, whose entries are of the form
    # argumentType\376argumentName\376argumentComment
    #

    local(@args);
    local($argType,$argName,$argComment);
    $_ = $args;
    while ( /\s*(|$comment)\s*(\w+)\s*(|$comment)\s*(,|$)/go ) {
	$argType = $1;
	$argName = $3;
	$argComment = $4;
	$argType =~ s/^\/\*\s*//;
	$argType =~ s/\s*\*\/$//;
#	$argComment =~ s/^\s+|\s+$//g;
#	$argComment =~ s/^\/\*\s*|\s*\*\/$//g;
#	$argComment =~ s/\s\s+/ /g;
	$argComment =~ s/^\/\*\s*//;
	$argComment =~ s/\s*\*\/$//;

#	&error("`$argType' `$argName' `$argComment'");
	push(@args,join("\376",($argType,$argName,$argComment)));
    }

    if ( $environment eq "polis" ) {

	# All commented macros are external in polis

	local($macroIsExternal) = 1;

    } else {
	local($macroIsExternal) = ($macroName =~ /^$packageName[_]/i);
    }

    #
    # Save the information about the macro as either internal or external
    #

    if ( $macroIsExternal ) {
	push(@externalAbstracts,join("\377",($macroName,$synopsis)));
	push(@externalPrototypes,
	     join("\377",($macroName,$returnType,"",
		       $synopsis,$description,$sideEffects,$seeAlso,$file,
		       @args)));
    } else {
	push(@internalAbstracts,join("\377",($macroName,$synopsis)));
	push(@internalPrototypes,
	     join("\377",($macroName,$returnType,"",
		       $synopsis,$description,$sideEffects,$seeAlso,$file,
		       @args)));
    }

    push(@allAbstracts,join("\377",($macroName,$synopsis,$file)));

}

##Sub################################################################
#
# Synopsis	[ Save lines in either the ``before'' or ``after'' area. ]
# Description	[ Depending on
#		  $beforeAutomatic, save $_ in either
#		  $beforePortion or $afterPortion. ]
# SideEffects	[ Global variables $beforePortion or $afterPortion may
#		  be modified. ]
#
######################################################################
sub saveLine {
    if ($saveLines) {
	if ($beforeAutomatic) {
	    $beforePortion .= $_;
	} else {
	    $afterPortion .= $_;
	}
    }
}

##Sub################################################################
#
# Synopsis	[ Split a pathname into a directory and file name. ]
# Description	[ ($dirname,$filename) = &splitPathname("/home/vis/fred.c")
#		  gives $dirname = "/home/vis/", $filename="fred.c" ]
# SideEffects	[]
#
######################################################################
sub splitPathname {
    $_ = $_[0];
    local($dirname,$filename) = /^(.*\/|)(.*)$/;
}

##Sub################################################################
#
# Synopsis	[ Write static prototypes to FILE ]
# Description	[ Print the contents of @staticSimplePrototypes
#		  to the FILE filehandle. ]
# SideEffects	[]
#
######################################################################
sub writeStaticPrototypes {

    if ( @staticSimplePrototypes ne () ) {

	print FILE "
/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

";	   	               
	foreach (@staticSimplePrototypes) {
	    ($functionName,$returnType,@simpleArgs) = split(/\377/);
	    print FILE "static $returnType $functionName(";
	    print FILE join(", ",@simpleArgs);
	    print FILE ");\n";	       	    
	}
	print FILE "\n";
    }
}

##Sub################################################################
#
# Synopsis	[ Write internal prototypes to FILE ]
# Description	[ Print the contents of @internalSimplePrototypes
#		  to the FILE filehandle. ]
# SideEffects	[]
#
######################################################################
sub writeInternalPrototypes {
    if ( @internalSimplePrototypes ne () ) {
	print FILE "
/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

";	   	               
	foreach (@internalSimplePrototypes) {
	    ($functionName,$returnType,@simpleArgs) =
		split(/\377/);
	    if ( $language eq "KR" ) {
		print FILE "EXTERN $returnType $functionName ARGS((";
		print FILE join(", ",@simpleargs);
		print FILE "));\n";
	    } else {
		print FILE "EXTERN $returnType $functionName(";
		print FILE join(", ",@simpleArgs);
		print FILE ");\n";
	    }
	}
	print FILE "\n";
    }
}

##Sub################################################################
#
# Synopsis	[ Write external prototypes to FILE ]
# Description	[ Print the contents of @externalSimplePrototypes
#		  to the FILE filehandle. ]
# SideEffects	[]
#
######################################################################
sub writeExternalPrototypes {
    if ( @externalSimplePrototypes ne () ) {
	print FILE "
/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

";	   	               
	foreach (@externalSimplePrototypes) {
	    ($functionName,$returnType,@simpleArgs) =
		split(/\377/);
	    if ( $language eq "KR" ) {
		print FILE "EXTERN $returnType $functionName ARGS((";
		print FILE join(", ",@simpleargs);
		print FILE "));\n";
	    } else {		
		print FILE "EXTERN $returnType $functionName(";
		print FILE join(", ",@simpleArgs);
		print FILE ");\n";
	    }
	}
	print FILE "\n";
    }
}

##Sub################################################################
#
# Synopsis	[ Print an error ]
# Description	[ Print the given error message to STDERR and prepare
#		  to return a non-zero exit code when the program finally
#		  terminates ]
# SideEffects	[ Exit status set to 1. ]
#
######################################################################
sub error {
    print STDERR "$_[0]\n";
    $returnValue = 1;
}

##Sub################################################################
#
# Synopsis	[ Print an error with a filename and line ]
# Description	[ Print the given error message to STDERR and prepare
#		  to return a non-zero exit code when the program finally
#		  terminates ]
# SideEffects	[ Exit status set to 1. ]
#
######################################################################
sub errorNearLine {
    print STDERR "$package:$file:$. $_[0]\n";
    $returnValue = 1;
}

##Format##############################################################
#
# Synopsis [ Format for function abstracts ]
#
######################################################################
format Abstract =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$functionName,		       $synopsis
~~                             ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			       $synopsis

.

##Format##############################################################
#
# Synopsis [ Format for function abstracts with lengthy names ]
#
######################################################################
format LengthyAbstract =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$functionName
                               ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			       $synopsis
~~                             ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			       $synopsis

.

##Format##############################################################
#
# Synopsis [ First part of format for function prototypes ]
#
######################################################################
format Prototype1 =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$returnType,			     $returnTypeComment
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$functionName
.

##Format##############################################################
#
# Synopsis [ Second part of format for function prototypes ]
#
######################################################################
format Prototype2 =
  @<<<<<<<<<<<<<<<< @<<<<<<<<<<<<<<< ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  $argType,	    $argName,	     $argComment
~~                                   ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
				     $argComment
.

##Format##############################################################
#
# Synopsis [ Third part of format for function prototypes ]
#
######################################################################
format Prototype3 =
)
~~^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  $description

~~^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  $sideEffects

.

##Format##############################################################
#
# Synopsis [ Simple filled paragraph ]
#
######################################################################
format fillParagraph =
^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$paragraph
^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<~~
$paragraph

.

# Local Variables:
# mode: perl
# End: