#!/usr/bin/perl

use strict;
use CGI;
my $q = new CGI;
my $timestamp = localtime;
print    $q->header( "text/html" ),
        $q->start_html( -title => "The Time", -bgcolor => "#ffffff" ),
        $q->h2( "Current Time" ),
        $q->hr,
        $q->p( "The current time according to this system is: ", $q->b( $timestamp ) ),
        $q->end_html;