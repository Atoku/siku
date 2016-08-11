#!/usr/bin/env python3

import sys

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
    bord.add_hp_verts( 5.0, 4.0 )
    bord.add_hp_verts( 2.0, 2.0, (0, 360, 60, 90) ) #0.5 0.9
    print('  high resolution domain')
    bord.add_hp_verts( 0.7, 0.7, (150, 270, 60, 85) )
    bord.add_hp_verts( 0.1, 0.1, (180, 250, 65, 82) )
    bord.add_hp_verts( 0.05, 0.01, (190, 230, 65, 79) )

    h_psi = 0.00995 #0.008
    f_psi = 0.00894

    bord.add_hp_verts( h_psi*3, f_psi*3, (193, 2227, 66, 76) )
    bord.add_hp_verts( h_psi, f_psi, (195, 225, 67, 75) )

    bverts = bord.verts[:]
##    #canal
##    print('-adding `canal` high resolution domain')
##    bord.add_contour( 'canal.ll' )
##    bord.add_hp_verts( 0.07, 0.07, (175, 190, 55, 60) )

    print('-filtering contours')
    bord.filter_contour( 0.05, (0, 360, 50, 90) )

    print('-actually generating borders` file')
##    bord.gener_borders( 50, domain=(90, 270, 50, 90) )
    bord.gener_borders( 50, domain=(150, 260, 50, 90) )
    bord.filter_v_by_b( 'borders.ll', 'contours.ll', 150, 'marks.ll' ) 

    print('-adding cover')
##    grid = hpgrid.Grid()
##    grid.points_gen( 5. )
##    grid.points_filter( 4. )
##
##    pol = hpgrid.Grid( hpgrid.Domain( theta = (0, 0.44) ) )
##    pol.points_gen( 1. )
##    pol.points_filter( 1. )
##
##    hig = hpgrid.Grid( hpgrid.Domain( units=0, phi = (170,240), \
##                                    theta = (14, 30) ) )
##    hig.points_gen( 0.15 )
##    hig.points_filter( 0.15 )
##
##    #alaska north shores
##    can = hpgrid.Grid( hpgrid.Domain( units=0, phi = (200,220), \
##                                      theta = (15, 25) ) )
##    # (0 (180 185) (30 40) )
##    can.points_gen( 0.1 )
##    can.points_filter( 0.1 )
##
##    verts = geofiles.xyz_to_lonlat( grid.points + pol.points + hig.points + \
##                                    can.points )

    verts = geofiles.xyz_to_lonlat( bverts )
    geofiles.w_lonlat( 'verts.ll', verts )

    print('\n-finalizing files')
    bord.extract_borders_from_v( 'verts.ll', 'borders.ll', 1 )
    print('...')
    verts = geofiles.r_lonlat( 'extrd.ll' )
    print('...')
    bords = geofiles.r_lonlat( 'borders.ll' )
    print('...')
    geofiles.w_xyz( 'h.xyz', geofiles.lonlat_to_xyz( verts + bords ) )

    #os.remove( 'verts.ll' )

    print('done!\n')
    return 

# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
