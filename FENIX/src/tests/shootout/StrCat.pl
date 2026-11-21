#!/usr/local/bin/perl 
# $Id: StrCat.pl,v 1.1 2003/11/27 07:00:39 jlceb Exp $
# http://www.bagley.org/~doug/shootout/

use strict;

my $NUM = $ARGV[0];
$NUM = 1 if ($NUM < 1);

my $str = "";
$str .= "hello\n" foreach (1..$NUM);
print length($str),"\n";

