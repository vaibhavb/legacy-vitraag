#!/usr/bin/perl

@files = `find -type d`;

foreach $dir (@files)
{
    chomp($dir);
    print "Adding $dir\/*";
    `cvs add $dir\/*`;
}
