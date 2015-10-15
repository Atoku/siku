#!/usr/bin/env python3

import sys
from math import pi
import shapefile

import siku
from siku import hpgrid
from siku import geocoords

def dupl_del( mlst ):
    
    m2lst = sorted( mlst )


    pred = m2lst[0]
    m3lst = [ pred ]

    for i, m in enumerate( m2lst[1:] ):
        if m[0] == pred[0] and m[1] == pred[1]:
            print ( m, pred )
        else:
            m3lst.append( m )
            pred = m
        
    return m3lst

def main():

    sf = shapefile.Reader( '../data/GSHHS_shp/l/GSHHS_l_L1' )
    shapes = sf.shapes()

    s0 = shapes[0]
    p0 = s0.points

    # duplicates deletion
    print( len(p0) )
    p0 = dupl_del( p0 )
    print( len(p0) )    

    s1 = shapes[2]
    p1 = s1.points

    # duplicates deletion
    print( len(p1) )
    p1 = dupl_del( p1 )
    print( len(p1) )    


    m1 = []
    for p in p1 + p0:
        m1.append( geocoords.xyz_geographic( *p ) )


    grid1 = hpgrid.Grid()

    grid1.points_gen( 4.0 )
    grid1.points_filter( 3.0 )

    grid2 = hpgrid.Grid( hpgrid.Domain( theta=(pi/3,pi/2), 
                                        phi=(2*pi-pi/6,2*pi) ) )
    grid2.points_gen( 0.5 )
    grid2.points_filter( 0.3 )

    grid3 = hpgrid.Grid()
    grid3.points = grid1.points + grid2.points
    grid3.points_filter( 0.3 )

    grid4 = hpgrid.Grid()
    grid4.points = m1 + grid3.points
    grid4.points_filter( 0.3 )

    f = open ( 'shapes.xyz', 'w' )

    for l in grid4.points:
        f.write( '%f %f %f\n' % l )

    f.close()

    return 0

# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
