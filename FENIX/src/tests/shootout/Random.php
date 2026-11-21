#!/usr/local/bin/php -f
<?php
/*
 $Id: Random.php,v 1.1 2003/11/27 07:00:39 jlceb Exp $
 http://www.bagley.org/~doug/shootout/
*/
define("IM", 139968);
define("IA", 3877);
define("IC", 29573);

$LAST = 42;
function gen_random ($max) {
    global $LAST;
    return( ($max * ($LAST = ($LAST * IA + IC) % IM)) / IM );
}

$N = (($argc == 2) ? $argv[1] : 1) - 1;
while ($N--) {
    gen_random(100.0);
}

printf("%.9f\n", gen_random(100.0));
?>
