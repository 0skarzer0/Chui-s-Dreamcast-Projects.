#!/usr/local/bin/perl 
# $Id: Random.pl,v 1.1 2003/11/27 07:00:39 jlceb Exp $
# http://www.bagley.org/~doug/shootout/

use strict;

use constant IM => 139968;
use constant IA => 3877;
use constant IC => 29573;

my $LAST = 42;
sub gen_random { ($_[0] * ($LAST = ($LAST * IA + IC) % IM)) / IM }

my $N = ($ARGV[0] || 1) - 1;
gen_random(100.0) while ($N--);
printf "%.9f\n", gen_random(100.0);
