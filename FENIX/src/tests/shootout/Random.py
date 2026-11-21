#!/usr/local/bin/python
# $Id: Random.py,v 1.1 2003/11/27 07:00:39 jlceb Exp $
# http://www.bagley.org/~doug/shootout/
# with help from Brent Burley

import sys

IM = 139968
IA = 3877
IC = 29573

LAST = 42
def gen_random(max):
    global LAST
    LAST = (LAST * IA + IC) % IM
    return( (max * LAST) / IM )

def main():
    N = int(sys.argv[1])
    if N < 1:
        N = 1
    gr = gen_random
    for i in xrange(1,N):
        gr(100.0)
    print "%.9f" % gr(100.0)

main()
