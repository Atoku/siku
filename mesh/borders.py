#!/usr/bin/env python3

import sys

from siku import hpgrid
from siku import geofiles
import boarder_gen as BG

def main():
    print( 'Generating boarders:' )

    print('-loading shapes')
    board = BG.Boarder( '/home/gleb/Documents/GMT_DATA/gshhg-shp-2.3.4/GSHHS_shp/l/GSHHS_l_L1' )

    print('-preparing contours')
    #better use all shapes, but that takes time
    board.make_shapes_contour( domain = (0, 360, 50, 90) )

    print('-adding verts')
    board.add_hp_verts( 15.0, 14.0 )
    board.add_hp_verts( 2., 1., (0, 360, 60, 90) ) #0.5 0.9
    print('  high resolution domain')
    board.add_hp_verts( 0.1, 0.1, (170, 210, 60, 70) )

    #canal
    print('-adding `canal` high resolution domain')
    board.add_contour( 'canal.ll' )
    board.add_hp_verts( 0.07, 0.09, (180, 185, 55, 60) )

    print('-filtering contours')
    board.filter_contour( 0.15, (0, 360, 50, 90) )

    print('-actually generating boarders` file')
    board.gener_boarders( 100, domain=(90, 270, 50, 90) )
    board.filter_v_by_b( 'boarders.ll', 'contours.ll', 50, 'marks.ll' ) 

    print('-adding coverage')
    grid = hpgrid.Grid()
    grid.points_gen( 5. )
    grid.points_filter( 4. )

    pol = hpgrid.Grid( hpgrid.Domain( theta = (0, 0.44) ) )
    pol.points_gen( 2. )
    pol.points_filter( 2. )

    hig = hpgrid.Grid( hpgrid.Domain( units=0, phi = (170,210), \
                                    theta = (20, 30) ) )
    hig.points_gen( 1. )
    hig.points_filter( 1. )

    can = hpgrid.Grid( hpgrid.Domain( units=0, phi = (180,185), \
                                    theta = (30, 40) ) )
    can.points_gen( 1. )
    can.points_filter( 1. )

    verts = geofiles.xyz_to_lonlat( grid.points + pol.points + hig.points + \
                                    can.points )
    geofiles.w_lonlat( 'verts.ll', verts )

    print('\n-file gen')
    board.extract_boarders_from_v( 'verts.ll', 'boarders.ll', 50 )
    verts = geofiles.r_lonlat( 'extrd.ll' )
    boards = geofiles.r_lonlat( 'boarders.ll' )
    geofiles.w_xyz( 'shapes.xyz', geofiles.lonlat_to_xyz( verts + boards ) )

    #os.remove( 'verts.ll' )

##    print('-actually generating boarders` file')
##    board.gener_boarders( 20, 0.15, domain = ( 0, 360, 50, 90), \
##                          file_b = 'boarders'  )
   
####    print('-actually generating boarders` file')
####    board.gener_boarders( 20, 0.3, domain = ( 0, 360, 60, 90), \
####                          file = 'polar.ll' )
####    print('  high resolution domain')
####    board.gener_boarders( 20, 0.05, domain = ( 170, 210, 54, 70 ), \
####                          file = 'beaufort.ll' )

    print('done!\n')
    return 

# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
