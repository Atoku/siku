#!/usr/bin/env python3

import sys

from siku import geofiles
from siku import hpgrid

def main():
    verts = geofiles.r_lonlat( 'boarders.ll' )
    verts = geofiles.lonlat_to_xyz( verts[:] )

    gridG = hpgrid.Grid()
    gridG.points_gen( 5 )
    gridG.points_filter( 5 )

    grid = hpgrid.Grid( hpgrid.Domain( theta = ( 0, 0.44 ) ) )
    grid.points_gen( 2.0 )
    grid.points_filter( 1.5 )

    gridG.points = verts + grid.points + gridG.points
    gridG.points_filter(0.5)

    geofiles.w_xyz( 'shapes.xyz', gridG.points )                
    
    return 

# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
