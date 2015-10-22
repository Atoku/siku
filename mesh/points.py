#!/usr/bin/env python3

import sys

from siku import geofiles
from siku import hpgrid

def main():
    boarders = geofiles.r_lonlat( 'polar.ll' )
    boarders = boarders + geofiles.r_lonlat( 'beaufort.ll' )
    geofiles.w_lonlat( 'boarders.ll', boarders )
    boarders = geofiles.lonlat_to_xyz( boarders[:] )

    gridG = hpgrid.Grid()
    gridG.points_gen( 5 )
    gridG.points_filter( 5 )

    grid = hpgrid.Grid( hpgrid.Domain( phi = (0, 360), theta = (0, 30), \
                                       units=0 ) )
    grid.points_gen( 2.0 )
    grid.points_filter( 1.5 )

    gridH = hpgrid.Grid( hpgrid.Domain( phi = (170, 210), theta = (20, 30), \
                                        units=0 ) )
    gridH.points_gen( 0.1 )
    gridH.points_filter( 0.15 )

    #canal
    gridC = hpgrid.Grid( hpgrid.Domain( phi = (179, 187), theta = (30, 36), \
                                        units=0 ) )
    gridC.points_gen( 0.1 )
    gridC.points_filter( 0.15 )

    gridG.points = boarders + grid.points + gridG.points + gridH.points + \
                   gridC.points
    gridG.points_filter(0.01)

    geofiles.w_xyz( 'shapes.xyz', gridG.points )                
    
    return 

# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
