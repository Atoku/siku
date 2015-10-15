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

    sf = shapefile.Reader( '/home/gleb/Documents/GMT_DATA/gshhg-shp-2.3.4/GSHHS_shp/l/GSHHS_l_L1' )
    shapes = sf.shapes()

    s0 = shapes[0]
    p0 = s0.points
    # duplicates deletion
    p0 = dupl_del( p0 )    

    s1 = shapes[1]
    p1 = s1.points
    # duplicates deletion
    p1 = dupl_del( p1 )  

    s2 = shapes[2]
    p2 = s1.points
    # duplicates deletion
    p2 = dupl_del( p2 )
    
    s3 = shapes[3]
    p3 = s1.points
    # duplicates deletion
    p3 = dupl_del( p3 )

    m1 = []
    for p in p0 + p1 + p2 + p3:
        m1.append( geocoords.xyz_geographic( *p ) )

    gridM = hpgrid.Grid()
    gridM.points = m1
    gridM.points_filter( 0.3 )
    m1 = gridM.points

    with open('boarders.txt', 'w') as board:
        for l in m1:
            board.write( '%f %f %f\n' % l )


    grid1 = hpgrid.Grid(  )
    grid1.points_gen( 5.0 )
    grid1.points_filter( 4.0 )
    
    grid2 = hpgrid.Grid( hpgrid.Domain( theta = (0, 0.44 ) ) )
    grid2.points_gen( 0.5 )
    grid2.points_filter( 0.9 )

    grid3 = hpgrid.Grid()
    grid3.points = grid1.points + grid2.points
    grid3.points_filter( 0.9 )

    grid4 = hpgrid.Grid()
    grid4.points = m1 + grid3.points

##    grid1 = hpgrid.Grid()
##
##    grid1.points_gen( 4.0 )
##    grid1.points_filter( 3.0 )
##
##    grid2 = hpgrid.Grid( hpgrid.Domain( theta=(pi/3,pi/2), 
##                                        phi=(2*pi-pi/6,2*pi) ) )
##    grid2.points_gen( 0.5 )
##    grid2.points_filter( 0.3 )
##
##    grid3 = hpgrid.Grid()
##    grid3.points = grid1.points + grid2.points
##    grid3.points_filter( 0.3 )
##
##    grid4 = hpgrid.Grid()
##    grid4.points = m1 + grid3.points
##    #grid4.points_filter( 0.3 )

    f = open ( 'shapes.xyz', 'w' )

    for l in grid4.points:
        f.write( '%f %f %f\n' % l )

    f.close()

    return 0

# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
