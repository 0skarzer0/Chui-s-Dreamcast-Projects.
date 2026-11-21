#!/usr/local/bin/python
# $Id: Ackermann.py,v 1.1 2003/11/27 07:00:39 jlceb Exp $
# http://www.bagley.org/~doug/shootout/
# from Brad Knotwell

import sys

def Ack(M, N):
    if (not M):
        return( N + 1 )
    if (not N):
        return( Ack(M-1, 1) )
    return( Ack(M-1, Ack(M, N-1)) )

def main():
    NUM = int(sys.argv[1])
    sys.setrecursionlimit(3000)
    print "Ack(3,%d): %d" % (NUM, Ack(3, NUM))

main()
