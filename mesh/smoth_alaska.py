#!/usr/bin/env python3

import sys
import os

from siku import hpgrid
from siku import geofiles
import border_gen as BG

def main():
    print( 'Generating borders:' )

    print('-loading shapes')
    bord = BG.Border( '/home/gleb/Documents/GMT_DATA/gshhg-shp-2.3.4/GSHHS_shp/l/GSHHS_l_L1' )

    print('-preparing contours')
    #better use all shapes, but that takes time
    bord.make_shapes_contour( domain = (45, 315, 50, 90) )

    print('-adding verts')
    bord.add_hp_verts( 15.0, 14.0 )
    bord.add_hp_verts( 2., 1., (0, 360, 60, 90) ) #0.5 0.9
    print('  high resolution domain')
    bord.add_hp_verts( 0.1, 0.1, (180, 240, 65, 75) )
    bord.add_hp_verts( 0.02, 0.01, (200, 220, 69, 72) )

    print('-filtering contours')
    bord.filter_contour( 0.01, (0, 360, 50, 90) )

    print('-actually generating borders` file')
    bord.gener_borders( 75, 0.01, domain=(190, 240, 60, 80), file_b='b0.ll')
    bord.gener_borders( 150, 0.1, domain=(190, 240, 60, 80), file_b='b1.ll' )
    bord.gener_borders( 300, 0.7, domain=(190, 240, 60, 80), file_b='b2.ll' )
    #bord.filter_v_by_b( 'borders.ll', 'contours.ll', 50, 'marks.ll' ) 

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
    bord.extract_borders_from_v( 'b1.ll', 'b0.ll', 5, 'bb1.ll' )
    bord.extract_borders_from_v( 'b2.ll', 'b0.ll', 78, 'bb2.ll' )
    bord.extract_borders_from_v( 'verts.ll', 'b0.ll', 225, 'vv.ll' )
    
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
