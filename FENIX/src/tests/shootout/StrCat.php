#!/usr/local/bin/php -f
<?php
/*
 $Id: StrCat.php,v 1.1 2003/11/27 07:00:39 jlceb Exp $
 http://www.bagley.org/~doug/shootout/
*/
$n = ($argc == 2) ? $argv[1] : 1;
$str = "";
while ($n-- > 0) {
    $str .= "hello\n";
}
$len = strlen($str);
print "$len\n";
?>
