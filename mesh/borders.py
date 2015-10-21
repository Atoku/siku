#!/usr/bin/env python3

import sys

from siku import hpgrid
import boarder_gen as BG

def main():
    print( 'Generating boarders:' )

    print('-loading shapes')
    board = BG.Boarder( '/home/gleb/Documents/GMT_DATA/gshhg-shp-2.3.4/GSHHS_shp/l/GSHHS_l_L1' )

    print('-preparing contours')
    board.make_contour( )#better use all shapes, but that takes time

    print('-adding verts')
    board.add_hp_verts( 5.0, 4.0 )
    board.add_hp_verts( 0.5, 0.9, hpgrid.Domain( theta = (0, 0.44 ) ) )

    print('-actually boarders')
    board.gener_boarders( 10, ( 0, 360, 60, 90) )

    print('done!\n')
    return 

# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
