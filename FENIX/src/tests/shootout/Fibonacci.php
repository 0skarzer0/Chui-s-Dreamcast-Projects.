#!/usr/local/bin/php -f
<?php
/*
 $Id: Fibonacci.php,v 1.1 2003/11/27 07:00:39 jlceb Exp $
 http://www.bagley.org/~doug/shootout/
*/
function fibo($n){
    return(($n < 2) ? 1 : fibo($n - 2) + fibo($n - 1));
}
$n = ($argc == 2) ? $argv[1] : 1;
$r = fibo($n);
print "$r\n";
?>
