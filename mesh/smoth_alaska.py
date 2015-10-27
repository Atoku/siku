#!/usr/bin/env python3

import sys
import os

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
    board.add_hp_verts( 0.1, 0.1, (180, 240, 65, 75) )
    board.add_hp_verts( 0.02, 0.01, (200, 220, 69, 72) )

    print('-filtering contours')
    board.filter_contour( 0.01, (0, 360, 50, 90) )

    print('-actually generating boarders` file')
    board.gener_boarders( 75, 0.01, domain=(190, 240, 60, 80), file_b='b0.ll')
    board.gener_boarders( 150, 0.1, domain=(190, 240, 60, 80), file_b='b1.ll' )
    board.gener_boarders( 300, 0.7, domain=(190, 240, 60, 80), file_b='b2.ll' )
    #board.filter_v_by_b( 'boarders.ll', 'contours.ll', 50, 'marks.ll' ) 

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
    board.extract_boarders_from_v( 'b1.ll', 'b0.ll', 5, 'bb1.ll' )
    board.extract_boarders_from_v( 'b2.ll', 'b0.ll', 78, 'bb2.ll' )
    board.extract_boarders_from_v( 'verts.ll', 'b0.ll', 225, 'vv.ll' )
    
    verts = geofiles.r_lonlat( 'vv.ll' )
    b0 = geofiles.r_lonlat( 'b0.ll' )
    b1 = geofiles.r_lonlat( 'bb1.ll' )
    b2 = geofiles.r_lonlat( 'bb2.ll' )
    v = BG.dupl_del( verts + b0 + b1 + b2 )
    geofiles.w_xyz( 'shapes.xyz', geofiles.lonlat_to_xyz( v ) )

    os.remove( 'vv.ll' )
    os.remove( 'b0.ll' )
    os.remove( 'b1.ll' )
    os.remove( 'b2.ll' )
    os.remove( 'bb1.ll' )
    os.remove( 'bb2.ll' )
    os.remove( 'verts.ll' )

    print('done!\n')
    return 

# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
