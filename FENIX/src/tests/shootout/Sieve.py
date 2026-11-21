#!/usr/local/bin/python 
# $Id: Sieve.py,v 1.1 2003/11/27 07:00:39 jlceb Exp $
# http://www.bagley.org/~doug/shootout/
# with help from Brad Knotwell

import sys

def main():
    NUM = int(sys.argv[1])
    for foo in xrange(0,NUM):
        flags = (8192+1) * [1]
        count = 0
        for i in xrange(2,8192+1):
            if flags[i]:
                # remove all multiples of prime: i
                k = i + i
                while k <= 8192:
                    flags[k] = 0
                    k = k + i
                count = count + 1
    print "Count:", count

main()
