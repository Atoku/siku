#!/usr/bin/env python3

import sys

#from siku import hpgrid
import boarder_gen as BG

def main():
    print( 'Generating boarders:' )

    print('-loading shapes')
    board = BG.Boarder( '/home/gleb/Documents/GMT_DATA/gshhg-shp-2.3.4/GSHHS_shp/l/GSHHS_l_L1' )

    print('-preparing contours')
    board.make_contour( )#better use all shapes, but that takes time

    print('-adding verts')
    board.add_hp_verts( 5.0, 4.0 )
    board.add_hp_verts( 2.5, 1.5, (0, 360, 60, 90) ) #0.5 0.9
    print('  high resolution domain')
    board.add_hp_verts( 0.2, 0.15, (170, 210, 60, 70) )

    #canal
    print('-adding `canal` high resolution domain')
    board.add_contour( 'canal.ll' )
    board.add_hp_verts( 0.17, 0.1, (180, 185, 55, 60) )

    print('-filtering contours')
    board.filter_contour( 0.15, (0, 360, 50, 90) )

    print('-actually generating boarders` file')
    board.gener_boarders( 20, 0.15, domain = ( 0, 360, 50, 90), \
                          file_b = 'boarders'  )
##    print('-actually generating boarders` file')
##    board.gener_boarders( 20, 0.3, domain = ( 0, 360, 60, 90), \
##                          file = 'polar.ll' )
##    print('  high resolution domain')
##    board.gener_boarders( 20, 0.05, domain = ( 170, 210, 54, 70 ), \
##                          file = 'beaufort.ll' )

    print('done!\n')
    return 

# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
