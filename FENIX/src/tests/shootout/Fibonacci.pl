#!/usr/local/bin/perl
# $Id: Fibonacci.pl,v 1.1 2003/11/27 07:00:39 jlceb Exp $
# http://www.bagley.org/~doug/shootout/

use strict;
use integer;

# from Leif Stensson
sub fib {
    return $_[0] < 2 ? 1 : fib($_[0]-2) + fib($_[0]-1);
}

my $N = ($ARGV[0] < 1) ? 1 : $ARGV[0];
my $fib = fib($N);
print "$fib\n";
