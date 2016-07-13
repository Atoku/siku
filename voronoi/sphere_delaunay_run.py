import subprocess
import os

print( 'enter file name:' )
F = input()

print( 'preparing input file...' )
with open( str(F), 'r') as inf:
    with open("_"+str(F), 'w') as outf:
        for l in inf:
            t = ''
            ll = l.split()
            for w in ll:
                t += str( w ) + '   '
            t += '\n'
            outf.write( t )

print( 'calculating triangulation...' )
subprocess.call( "./stripack_delaunay _" + str(F), shell=True )

print( "preparing '.xyzf' file..." )
with open( 'triangulation.txt', 'r') as inf:
    with open( str(F) + 'f', 'w') as outf:
        for l in inf:
            t = ''
            ll = l.split()
            for w in ll:
                t += str( w ) + '   '
            t += ' -1\n'
            outf.write( t )
