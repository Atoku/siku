#!/usr/bin/env python3

import sys

from siku import hpgrid
from siku import geofiles
import border_gen as BG

def main():
    print( 'Generating borders:' )

    ## Initializing border generator with loading specific shapefile. 
    print('-loading shapes')
    bord = BG.Border(\
        '/Documents/GMT_DATA/gshhg-shp-2.3.4/GSHHS_shp/l/GSHHS_l_L1' )

    print('-preparing contours')
    ## Generating seed points from actual shores 
    ## better use all shapes, but that takes time 
    bord.make_shapes_contour( domain = (45, 315, 50, 90) )

    ## Adding seed points in spesialized locations
    print('-adding verts')
    bord.add_hp_verts( 5.0, 4.0 ) # sparsely around the globe
    bord.add_hp_verts( 2.0, 2.0, (0, 360, 60, 90) ) # basic grid in some region
    print('  high resolution domain')
    bord.add_hp_verts( 0.7, 0.7, (150, 270, 60, 85) )    # Several high-reso
    bord.add_hp_verts( 0.1, 0.1, (180, 250, 65, 82) )    # grids with smooth
    bord.add_hp_verts( 0.01, 0.009, (195, 225, 67, 75) ) # gradient density.

    ## Filterering coastal points
    bverts = bord.verts[:]
    print('-filtering contours')
    bord.filter_contour( 0.05, (0, 360, 50, 90) )

    ## Automatically generate borders - create a file with points
    ## close enough to coastline (border zone)
    print('-actually generating borders` file')
    bord.gener_borders( 50, domain=(150, 260, 50, 90) )
    bord.filter_v_by_b( 'borders.ll', 'contours.ll', 150, 'marks.ll' ) 

    ## Adding the rest of seed points (in the coastal area too)
    print('-adding cover')
    verts = geofiles.xyz_to_lonlat( bverts )
    geofiles.w_lonlat( 'verts.ll', verts )

    ## Removing points in coastal area from sea covering points,
    ## combining and saving all the files in proper format.
    print('\n-finalizing files')
    bord.extract_borders_from_v( 'verts.ll', 'borders.ll', 1 )
    verts = geofiles.r_lonlat( 'extrd.ll' )
    bords = geofiles.r_lonlat( 'borders.ll' )
    geofiles.w_xyz( 'h.xyz', geofiles.lonlat_to_xyz( verts + bords ) )

    ## removing temporal (if necessary)
    #os.remove( 'verts.ll' )

    print('done!\n')
    return 

# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
