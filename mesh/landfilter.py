#!/usr/bin/env python3

import os
import mathutils

from siku import geocoords

def main():
    xyz_to_lonlat( 'shapes.xyz', 'temp.ll' )
        
    os.system('gmt gmtselect temp.ll -Dl -Nk/s > tempf.ll')
    #os.system('gmt gmtselect temp.ll -fg -C10k/boards.txt > tempf.ll')

    lonlat_to_xyz( 'tempf.ll', 'shapesf.xyz' )

##    os.remove('temp.ll')
##    os.remove('tempf.ll')

# --------------------------------------------------------------------------

def xyz_to_lonlat( input_d, output_f ):
    verts = []
    with open( input_d, 'r') as inp:
        for l in inp:
            t = l.split()
            v = [ float(f) for f in t[:-1] ]
            v.append( int( t[-1] ) )
            t = list( geocoords.lonlat_deg( mathutils.Vector( v[:-1] ) ) )
            t.append( v[-1] )
            verts.append( t )

    with open( output_f, 'w') as outp:
        for v in verts:
            outp.write( str( v[0] ) + '\t' + str( v[1] ) + '\t' + \
                        str( v[2] ) + '\n' )

    return

def lonlat_to_xyz( input_d, output_f ):
    verts = []
    with open( input_d, 'r') as inp:
        for l in inp:
            t = l.split()
            v = [ float(f) for f in t[:-1] ]
            v.append( int( t[-1] ) )
            t = list( geocoords.xyz_geographic( v[0], v[1] ) )
            t.append( v[2] )
            verts.append( t )

    with open( output_f, 'w') as outp:
        for v in verts:
            outp.write( str( v[0] ) + '\t' + str( v[1] ) + '\t' + \
                        str( v[2] ) + '\t' + str( v[3] ) + '\n' )
            #outp.write( '%f\t%f\t%f\t%i\n' % v )
            
    return

if __name__ == '__main__':
    main()
